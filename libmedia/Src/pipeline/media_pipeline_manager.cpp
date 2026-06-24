#include "media_pipeline_manager.hpp"

#include "../audio/audio_stream_service.hpp"
#include "../detect/detect_service.hpp"
#include "../isp/isp_service.hpp"
#include "../osd/osd_service.hpp"
#include "../video/video_stream_service.hpp"

#include <utility>
#include <cstring>
#include <chrono>
#include <thread>

#if defined(LIBMEDIA_BUILD_AX615_ADAPTERS)
#include "../adapter/ax615/audio/ax_audio_capture_adapter.hpp"
#include "../adapter/ax615/audio/ax_audio_play_adapter.hpp"
#include "../adapter/ax615/detect/ax_detect_adapter.hpp"
#include "../adapter/ax615/isp/ax_isp_adapter.hpp"
#include "../adapter/ax615/ivps/ax_ivps_adapter.hpp"
#include "../adapter/ax615/jenc/ax_jenc_adapter.hpp"
#include "../adapter/ax615/osd/ax_osd_adapter.hpp"
#include "../adapter/ax615/power/ax_aov_power_adapter.hpp"
#include "../adapter/ax615/sys/ax_link_manager.hpp"
#include "../adapter/ax615/sys/ax_sys_adapter.hpp"
#include "../adapter/ax615/venc/ax_venc_adapter.hpp"
#include "../adapter/ax615/vin/ax_vin_adapter.hpp"

// 声明 Sensor 对象（来自 libsns_os04d10.a，C 符号）
extern "C" {
    extern AX_SENSOR_REGISTER_FUNC_T gSnsos04d10Obj;
}
#endif

namespace aov::media {
namespace {

bool IsValidPipelineConfig(const Ax615PipelineConfig& cfg) {
    if (cfg.sensor_name.empty() || cfg.pipe_id < 0 || cfg.dev_id < 0 || cfg.ivps_grp_id < 0 ||
        cfg.normal_fps <= 0 || cfg.video_channels.empty()) {
        return false;
    }
    for (const auto& ch : cfg.video_channels) {
        if (ch.ivps_chn_id < 0 || ch.venc_chn_id < 0 || ch.width <= 0 || ch.height <= 0 ||
            ch.fps <= 0 || ch.bitrate_kbps <= 0) {
            return false;
        }
    }
    for (const auto& ch : cfg.jpeg_channels) {
        if (ch.ivps_chn_id < 0 || ch.jenc_chn_id < 0 || ch.width <= 0 || ch.height <= 0 ||
            ch.capture_fps <= 0 || ch.quality < 1 || ch.quality > 99) {
            return false;
        }
    }
    return true;
}

}  // namespace

MediaPipelineManager::MediaPipelineManager()
    : detect_service_(std::make_unique<DetectService>()),
      video_service_(std::make_unique<VideoStreamService>()),
      audio_service_(std::make_unique<AudioStreamService>()),
      isp_service_(std::make_unique<IspService>()),
      osd_service_(std::make_unique<OsdService>()) {}

#if defined(LIBMEDIA_BUILD_AX615_ADAPTERS)
MediaPipelineManager::MediaPipelineManager(
    std::unique_ptr<ax615::AxSysAdapter> sys,
    std::unique_ptr<ax615::AxVinAdapter> vin,
    std::unique_ptr<ax615::AxIspAdapter> isp,
    std::unique_ptr<ax615::AxIvpsAdapter> ivps,
    std::unique_ptr<ax615::AxVencAdapter> venc,
    std::unique_ptr<ax615::AxJencAdapter> jenc,
    std::unique_ptr<ax615::AxLinkManager> link,
    std::unique_ptr<ax615::AxAovPowerAdapter> power,
    std::unique_ptr<ax615::AxAudioCaptureAdapter> audio_capture,
    std::unique_ptr<ax615::AxAudioPlayAdapter> audio_play,
    std::unique_ptr<ax615::AxDetectAdapter> detect,
    std::unique_ptr<ax615::AxOsdAdapter> osd)
    : MediaPipelineManager() {
    sys_ = std::move(sys);
    vin_ = std::move(vin);
    isp_ = std::move(isp);
#if defined(LIBMEDIA_BUILD_AX615_ADAPTERS)
    isp_service_->SetAdapter(isp_.get());
#endif
    ivps_ = std::move(ivps);
    venc_ = std::move(venc);
    jenc_ = std::move(jenc);
    link_ = std::move(link);
    power_ = std::move(power);
    audio_capture_ = std::move(audio_capture);
    audio_play_ = std::move(audio_play);
    detect_ = std::move(detect);
    osd_ = std::move(osd);
    video_service_->SetKeyFrameRequestHandler([this](int chn_id) {
        return venc_ ? venc_->RequestKeyFrame(chn_id) : MediaStatusCode::InvalidState;
    });
    audio_service_->SetCaptureStartHandler([this](int chn_id) {
        if (!audio_config_.enabled) {
            return MediaStatusCode::InvalidState;
        }
        if (!audio_capture_) {
            return MediaStatusCode::InvalidState;
        }
        if (!audio_capture_->IsOpen()) {
            ax615::AxAudioCaptureConfig cfg;
            cfg.card_id = 0;
            cfg.device_id = audio_config_.capture_dev_id;
            cfg.aenc_chn_id = audio_config_.enc_chn_id >= 0 ? audio_config_.enc_chn_id : chn_id;
            cfg.sample_rate = audio_config_.sample_rate_hz;
            cfg.channels = audio_config_.channels;
            cfg.codec = audio_config_.codec;
            MediaStatusCode status = audio_capture_->Open(cfg);
            if (status != MediaStatusCode::Ok) {
                return status;
            }
            audio_capture_->SetFrameCallback([this](const AudioFrame& frame) {
                audio_service_->DispatchFrame(frame);
            });
        }
        return audio_capture_->Start();
    });
    audio_service_->SetCaptureStopHandler([this](int) {
        if (audio_capture_) {
            audio_capture_->Stop();
        }
    });
    audio_service_->SetPlaybackStartHandler([this](int dev_id) {
        if (!audio_config_.enable_talkback_play) {
            return MediaStatusCode::InvalidState;
        }
        if (!audio_play_) {
            return MediaStatusCode::InvalidState;
        }
        if (!audio_play_->IsOpen()) {
            ax615::AxAudioPlayConfig cfg;
            cfg.card_id = 0;
            cfg.device_id = dev_id;
            cfg.adec_chn_id = audio_config_.enc_chn_id;
            cfg.sample_rate = audio_config_.sample_rate_hz;
            cfg.channels = audio_config_.channels;
            cfg.codec = audio_config_.codec;
            return audio_play_->Open(cfg);
        }
        return MediaStatusCode::Ok;
    });
    audio_service_->SetPlaybackStopHandler([this](int) {
        if (audio_play_) {
            audio_play_->Close();
        }
    });
    audio_service_->SetPlaybackFrameHandler([this](const AudioFrame& frame) {
        return audio_play_ ? audio_play_->Play(frame) : MediaStatusCode::InvalidState;
    });
    detect_service_->SetRunOnceHandler([this](DetectResultSummary& result) {
        result = {};
        if (!detect_ || !detect_->IsOpen() || !ivps_ || !ivps_->IsRunning()) {
            return MediaStatusCode::InvalidState;
        }

        // 通过 IVPS adapter 获取帧
        AX_VIDEO_FRAME_T video_frame;
        std::memset(&video_frame, 0, sizeof(video_frame));

        const int ivps_chn = detect_config_.ivps_chn_id;
        const int timeout_ms = 100;  // 100ms timeout

        if (!ivps_->GetChnFrame(ivps_chn, video_frame, timeout_ms)) {
            // Timeout or no frame available, not an error
            return MediaStatusCode::Ok;
        }

        // Send frame to SKEL
        static uint64_t frame_id = 0;
        MediaStatusCode status = detect_->SendFrame(ivps_chn, frame_id++, video_frame, 0);

        // 通过 IVPS adapter 释放帧
        ivps_->ReleaseChnFrame(ivps_chn, video_frame);

        return status;
    });
    osd_service_->SetApplyHandler([this](const OsdApplyConfig& config) {
        if (!config.enabled) {
            return MediaStatusCode::Ok;
        }
        return osd_ ? osd_->Open() : MediaStatusCode::InvalidState;
    });
    osd_service_->SetDetectOverlayHandler([this](const DetectResultSummary& result) {
        return osd_ ? osd_->UpdateDetectOverlay(result) : MediaStatusCode::Ok;
    });
    osd_service_->SetClearHandler([this]() {
        return osd_ ? osd_->ClearAll() : MediaStatusCode::Ok;
    });
}
#endif

MediaPipelineManager::~MediaPipelineManager() {
    Stop();
}

MediaStatusCode MediaPipelineManager::ConfigureAx615Pipeline(const Ax615PipelineConfig& cfg) {
    if (!IsValidPipelineConfig(cfg)) {
        return MediaStatusCode::InvalidArgument;
    }
    if (running_) {
        return MediaStatusCode::Busy;
    }

    ax615_cfg_ = cfg;
    pipeline_configured_ = BuildPlans();
    return pipeline_configured_ ? MediaStatusCode::Ok : MediaStatusCode::InternalError;
}

MediaStatusCode MediaPipelineManager::Init(const MediaRuntimeInitConfig& config) {
    if (initialized_) {
        return MediaStatusCode::InvalidState;
    }
    init_config_ = config;
    MediaStatusCode status = ApplyVideoConfig(config.video);
    if (status != MediaStatusCode::Ok) {
        return status;
    }
    status = ApplyAudioConfig(config.audio);
    if (status != MediaStatusCode::Ok) {
        return status;
    }
    status = ApplyDetectConfig(config.detect);
    if (status != MediaStatusCode::Ok) {
        return status;
    }
    status = ApplyOsdConfig(config.osd);
    if (status != MediaStatusCode::Ok) {
        return status;
    }
    status = Init(config.sensor, config.pool);
    if (status != MediaStatusCode::Ok) {
        return status;
    }
    // Defer ApplyImageConfig to Start() - ISP IQ APIs require ISP to be started first
    return MediaStatusCode::Ok;
}

MediaStatusCode MediaPipelineManager::Init(const SensorConfig& sensor, const CmPoolConfig& pool) {
    if (initialized_) {
        return MediaStatusCode::InvalidState;
    }
    sensor_ = sensor;
    pool_ = pool;
    if (public_video_configured_) {
        const MediaStatusCode status = RebuildAx615PipelineFromPublicVideoConfig();
        if (status != MediaStatusCode::Ok) {
            return status;
        }
    }
    if (!pipeline_configured_) {
        Ax615PipelineConfig cfg;
        cfg.sensor_name = "os04d10";
        cfg.pipe_id = sensor.pipe_id;
        cfg.dev_id = sensor.dev_id;
        cfg.ivps_grp_id = 0;
        cfg.normal_fps = sensor.fps > 0 ? sensor.fps : 15;

        Ax615VideoChannelConfig video;
        video.ivps_chn_id = 0;
        video.venc_chn_id = 0;
        video.width = sensor.resolution.width > 0 ? sensor.resolution.width : 1920;
        video.height = sensor.resolution.height > 0 ? sensor.resolution.height : 1080;
        video.fps = cfg.normal_fps;
        cfg.video_channels.push_back(video);

        const MediaStatusCode status = ConfigureAx615Pipeline(cfg);
        if (status != MediaStatusCode::Ok) {
            return status;
        }
    }
    initialized_ = true;
    prepared_for_aov_sleep_ = false;
    ClearLastError();
    RefreshRuntimeStatus();
    return MediaStatusCode::Ok;
}

MediaStatusCode MediaPipelineManager::ApplyVideoConfig(const VideoEncodeGroupConfig& config) {
    const bool was_running = running_;
    const bool was_low_power = low_power_;
    const bool was_prepared_for_aov_sleep = prepared_for_aov_sleep_;
    const VideoEncodeGroupConfig old_video_config = video_config_;
    const Ax615PipelineConfig old_ax615_cfg = ax615_cfg_;
    const bool old_public_video_configured = public_video_configured_;
    const bool old_pipeline_configured = pipeline_configured_;

    const MediaStatusCode status = video_service_->ApplyVideoEncodeGroupConfig(config);
    if (status != MediaStatusCode::Ok) {
        return status;
    }
    video_config_ = config;
    public_video_configured_ = true;

    if (was_running) {
        Stop();
    }

    if (initialized_ || !sensor_.resolution.width || !sensor_.resolution.height) {
        const MediaStatusCode rebuild_status = RebuildAx615PipelineFromPublicVideoConfig();
        if (rebuild_status != MediaStatusCode::Ok) {
            video_config_ = old_video_config;
            public_video_configured_ = old_public_video_configured;
            ax615_cfg_ = old_ax615_cfg;
            pipeline_configured_ = old_pipeline_configured;
            BuildPlans();
            if (was_running) {
                Start();
                low_power_ = was_low_power;
                prepared_for_aov_sleep_ = was_prepared_for_aov_sleep;
                RefreshRuntimeStatus();
            }
            return rebuild_status;
        }
    }

    if (was_running) {
        const MediaStatusCode start_status = Start();
        if (start_status != MediaStatusCode::Ok) {
            SetLastError(start_status, 0, "failed to restart media pipeline after video config apply");
            return start_status;
        }
        if (was_low_power) {
            const MediaStatusCode low_power_status = EnterLowPowerMode(low_power_profile_);
            if (low_power_status != MediaStatusCode::Ok) {
                return low_power_status;
            }
        }
        prepared_for_aov_sleep_ = was_prepared_for_aov_sleep;
        if (video_service_) {
            for (const auto& ch : ax615_cfg_.video_channels) {
                video_service_->RequestKeyFrame(ch.venc_chn_id);
            }
        }
        RefreshRuntimeStatus();
    }
    return MediaStatusCode::Ok;
}

MediaStatusCode MediaPipelineManager::ApplyAudioConfig(const AudioEncodeApplyConfig& config) {
    const MediaStatusCode status = audio_service_->ApplyAudioEncodeConfig(config);
    if (status == MediaStatusCode::Ok) {
        audio_config_ = config;
    }
    return status;
}

MediaStatusCode MediaPipelineManager::ApplyImageConfig(const IspImageConfig& config) {
    return isp_service_->ApplyImageConfig(config);
}

MediaStatusCode MediaPipelineManager::ApplyDetectConfig(const DetectApplyConfig& config) {
    const MediaStatusCode status = detect_service_->ApplyDetectConfig(config);
    if (status == MediaStatusCode::Ok) {
        detect_config_ = config;
    }
    return status;
}

MediaStatusCode MediaPipelineManager::ApplyOsdConfig(const OsdApplyConfig& config) {
    const MediaStatusCode status = osd_service_->ApplyOsdConfig(config);
    if (status == MediaStatusCode::Ok) {
        osd_config_ = config;
    }
    return status;
}

MediaStatusCode MediaPipelineManager::Start() {
    if (!initialized_) {
        return MediaStatusCode::InvalidState;
    }
    if (running_) {
        return MediaStatusCode::Ok;
    }
    if (!pipeline_configured_) {
        return MediaStatusCode::InvalidState;
    }
    const MediaStatusCode ax_status = StartAx615Adapters();
    if (ax_status != MediaStatusCode::Ok) {
        StopAx615Adapters();
        SetLastError(ax_status, 0, "failed to start AX615 media adapters");
        return ax_status;
    }
    // Apply image config after ISP Start - IQ APIs require ISP to be running
    const MediaStatusCode image_status = ApplyImageConfig(init_config_.image);
    if (image_status != MediaStatusCode::Ok) {
        StopAx615Adapters();
        SetLastError(image_status, 0, "failed to apply image config");
        return image_status;
    }
    const MediaStatusCode capture_status = StartCaptureChannels();
    if (capture_status != MediaStatusCode::Ok) {
        StopAx615Adapters();
        SetLastError(capture_status, 0, "failed to start video capture channels");
        return capture_status;
    }
    running_ = true;
    prepared_for_aov_sleep_ = false;
    RefreshRuntimeStatus();
    return MediaStatusCode::Ok;
}

void MediaPipelineManager::Stop() {
    if (!running_) {
        return;
    }
    StopCaptureChannels();
    StopAx615Adapters();
    detect_service_->StopDetect();
    osd_service_->ClearAll();
    running_ = false;
    low_power_ = false;
    RefreshRuntimeStatus();
}

MediaStatusCode MediaPipelineManager::PrepareForAovSleep(const AovSleepMediaConfig& config) {
    if (!initialized_) {
        return MediaStatusCode::InvalidState;
    }

    aov_sleep_config_ = config;
    if (detect_service_) {
        detect_service_->StopDetect();
    }
    if (audio_service_) {
        audio_service_->StopCapture(audio_config_.enc_chn_id);
        audio_service_->StopPlayback(audio_config_.play_dev_id);
    }
    StopCaptureChannels();
    StopAx615Adapters();
    running_ = false;
    low_power_ = false;
    prepared_for_aov_sleep_ = true;
    RefreshRuntimeStatus();
    return MediaStatusCode::Ok;
}

MediaStatusCode MediaPipelineManager::RestoreFromAovSleep(const WakeupMediaProfile& profile) {
    if (!initialized_) {
        return MediaStatusCode::InvalidState;
    }

    wakeup_profile_ = profile;
    if (!running_) {
        const MediaStatusCode start_status = Start();
        if (start_status != MediaStatusCode::Ok) {
            return start_status;
        }
    }
    prepared_for_aov_sleep_ = false;

    if (profile.start_audio && audio_config_.enabled && audio_service_) {
        const MediaStatusCode audio_status = audio_service_->StartCapture(audio_config_.enc_chn_id);
        if (audio_status != MediaStatusCode::Ok) {
            return audio_status;
        }
    }

    if (profile.start_detect_confirm && detect_service_) {
        const MediaStatusCode detect_status = detect_service_->StartDetect();
        if (detect_status != MediaStatusCode::Ok) {
            return detect_status;
        }
    }

    if (profile.reason == WakeupReason::Timer) {
        const MediaStatusCode low_power_status = EnterLowPowerMode(profile.low_power_record);
        if (low_power_status != MediaStatusCode::Ok) {
            return low_power_status;
        }
    } else {
        const MediaStatusCode normal_status = ResumeNormalMode();
        if (normal_status != MediaStatusCode::Ok) {
            return normal_status;
        }
    }

    if (profile.request_idr && video_service_) {
        for (const auto& ch : ax615_cfg_.video_channels) {
            video_service_->RequestKeyFrame(ch.venc_chn_id);
        }
    }

    RefreshRuntimeStatus();
    return MediaStatusCode::Ok;
}

MediaStatusCode MediaPipelineManager::EnterLowPowerMode(const LowPowerRecordProfile& profile) {
    if (profile.target_frame_count <= 0 || profile.record_fps <= 0 || profile.normal_fps <= 0 || profile.gop <= 0) {
        return MediaStatusCode::InvalidArgument;
    }
    if (!initialized_) {
        return MediaStatusCode::InvalidState;
    }
    low_power_profile_ = profile;
    const MediaStatusCode status = EnterAx615LowPower();
    if (status != MediaStatusCode::Ok) {
        return status;
    }
    low_power_ = true;
    RefreshRuntimeStatus();
    return MediaStatusCode::Ok;
}

MediaStatusCode MediaPipelineManager::ResumeNormalMode() {
    if (!initialized_) {
        return MediaStatusCode::InvalidState;
    }
    const MediaStatusCode status = ResumeAx615Normal();
    if (status != MediaStatusCode::Ok) {
        return status;
    }
    low_power_ = false;
    RefreshRuntimeStatus();
    return MediaStatusCode::Ok;
}

MediaStatusCode MediaPipelineManager::CaptureSnapshot(const SnapshotRequest& request, SnapshotResult& result) {
    if (request.chn_id < 0 || request.quality < 1 || request.quality > 100) {
        return MediaStatusCode::InvalidArgument;
    }
    result = {};
    return MediaStatusCode::Unsupported;
}

IDetectService* MediaPipelineManager::GetDetectService() {
    return detect_service_.get();
}

IIspService* MediaPipelineManager::GetIspService() {
    return isp_service_.get();
}

IVideoStreamService* MediaPipelineManager::GetVideoStreamService() {
    return video_service_.get();
}

IAudioStreamService* MediaPipelineManager::GetAudioStreamService() {
    return audio_service_.get();
}

IOsdService* MediaPipelineManager::GetOsdService() {
    return osd_service_.get();
}

bool MediaPipelineManager::IsRunning() const {
    return running_;
}

MediaRuntimeStatus MediaPipelineManager::GetRuntimeStatus() const {
    MediaRuntimeStatus status = runtime_status_;
    status.initialized = initialized_;
    status.running = running_;
    status.low_power = low_power_;
    status.prepared_for_aov_sleep = prepared_for_aov_sleep_;
    status.video_frames = video_frames_;
    status.audio_frames = audio_frames_;
    status.detect_results = detect_results_;
    return status;
}

bool MediaPipelineManager::BuildPlans() {
    startup_plan_.clear();
    link_plan_.clear();

    startup_plan_.push_back("SYS.Init");
    startup_plan_.push_back("SYS.ConfigCmPool");
    startup_plan_.push_back("VIN.Open");
    startup_plan_.push_back("ISP.Open");
    startup_plan_.push_back("IVPS.Open");
    startup_plan_.push_back("VENC.CreateChannel");
    if (!ax615_cfg_.jpeg_channels.empty()) {
        startup_plan_.push_back("JENC.Open");
    }
    startup_plan_.push_back("LinkModules");
    startup_plan_.push_back("VIN.Enable");
    startup_plan_.push_back("ISP.Start");
    startup_plan_.push_back("IVPS.Start");
    startup_plan_.push_back("VENC.Start");

    link_plan_.push_back({"VIN", ax615_cfg_.pipe_id, 0, "IVPS", ax615_cfg_.ivps_grp_id, 0});
    for (const auto& ch : ax615_cfg_.video_channels) {
        link_plan_.push_back({"IVPS", ax615_cfg_.ivps_grp_id, ch.ivps_chn_id, "VENC", 0, ch.venc_chn_id});
    }
    for (const auto& ch : ax615_cfg_.jpeg_channels) {
        link_plan_.push_back({"IVPS", ax615_cfg_.ivps_grp_id, ch.ivps_chn_id, "JENC", 0, ch.jenc_chn_id});
    }

    return !startup_plan_.empty() && link_plan_.size() >= 2;
}

MediaStatusCode MediaPipelineManager::StartCaptureChannels() {
    for (const auto& ch : ax615_cfg_.video_channels) {
        const MediaStatusCode status = video_service_->StartCapture(ch.venc_chn_id);
        if (status != MediaStatusCode::Ok) {
            return status;
        }
    }
    for (const auto& ch : ax615_cfg_.jpeg_channels) {
        const MediaStatusCode status = video_service_->StartCapture(ch.jenc_chn_id);
        if (status != MediaStatusCode::Ok) {
            return status;
        }
    }
    return MediaStatusCode::Ok;
}

void MediaPipelineManager::StopCaptureChannels() {
    for (const auto& ch : ax615_cfg_.video_channels) {
        video_service_->StopCapture(ch.venc_chn_id);
    }
    for (const auto& ch : ax615_cfg_.jpeg_channels) {
        video_service_->StopCapture(ch.jenc_chn_id);
    }
}

MediaStatusCode MediaPipelineManager::StartAx615Adapters() {
#if defined(LIBMEDIA_BUILD_AX615_ADAPTERS)
    if (!sys_ || !vin_ || !isp_ || !ivps_ || !venc_ || !jenc_ || !link_) {
        return MediaStatusCode::InvalidState;
    }

    std::fprintf(stderr, "[Pipeline][v20260611] StartAx615Adapters begin\n");

    MediaStatusCode status = sys_->Init();
    if (status != MediaStatusCode::Ok) {
        std::fprintf(stderr, "[Pipeline][v20260611] sys_->Init() failed\n");
        return status;
    }
    status = sys_->ConfigCmPool(pool_);
    if (status != MediaStatusCode::Ok) {
        std::fprintf(stderr, "[Pipeline][v20260611] sys_->ConfigCmPool() failed\n");
        return status;
    }

    // Initialize NPU for AI detection (SKEL)
    std::fprintf(stderr, "[Pipeline][v20260611] Initializing NPU\n");
    status = sys_->InitNpu();
    if (status != MediaStatusCode::Ok) {
        std::fprintf(stderr, "[Pipeline][v20260611] sys_->InitNpu() failed\n");
        return status;
    }

    // CRITICAL: Initialize IVPS BEFORE VIN/ISP (matches QSDemo and SDK sample order)
    std::fprintf(stderr, "[Pipeline][v20260611] Opening IVPS (before VIN/ISP)\n");
    ax615::IvpsGroupConfig ivps_cfg;
    ivps_cfg.grp_id = ax615_cfg_.ivps_grp_id;
    for (const auto& ch : ax615_cfg_.video_channels) {
        ax615::IvpsChannelConfig ivps_ch;
        ivps_ch.chn_id = ch.ivps_chn_id;
        ivps_ch.width = ch.width;
        ivps_ch.height = ch.height;
        ivps_ch.stride = ch.width;
        ivps_ch.src_fps = ch.fps;
        ivps_ch.dst_fps = ch.fps;
        ivps_cfg.channels.push_back(ivps_ch);
    }
    if (!ivps_->Open(ivps_cfg)) {
        std::fprintf(stderr, "[Pipeline][v20260611] ivps_->Open() failed\n");
        return MediaStatusCode::InternalError;
    }
    std::fprintf(stderr, "[Pipeline][v20260611] IVPS Open OK\n");

    // CRITICAL: Establish VIN→IVPS link BEFORE starting any modules (matches SDK sample order)
    // SDK sample creates links immediately after SYS init, before IVPS/VIN start
    std::fprintf(stderr, "[Pipeline][v20260611] ========== Establishing links (BEFORE module start) ==========\n");
    for (const auto& link : link_plan_) {
        ax615::AxModPort src{ax615::AxModId::VIN, link.src_dev, link.src_chn};
        ax615::AxModPort dst{ax615::AxModId::IVPS, link.dst_dev, link.dst_chn};
        if (link.src == "IVPS") {
            src.mod = ax615::AxModId::IVPS;
        }
        if (link.dst == "VENC") {
            dst.mod = ax615::AxModId::VENC;
        } else if (link.dst == "JENC") {
            dst.mod = ax615::AxModId::VENC;
        }
        std::fprintf(stderr, "[Pipeline][DEBUG] Linking %s[%d,%d] -> %s[%d,%d]\n",
                    link.src.c_str(), link.src_dev, link.src_chn,
                    link.dst.c_str(), link.dst_dev, link.dst_chn);
        if (!link_->Link(src, dst)) {
            std::fprintf(stderr, "[Pipeline][v20260611] link_->Link() failed\n");
            return MediaStatusCode::InternalError;
        }
    }
    std::fprintf(stderr, "[Pipeline][v20260611] ========== All links established ==========\n");

    std::fprintf(stderr, "[Pipeline][v20260611] Opening VIN: %s\n", ax615_cfg_.sensor_name.c_str());
    ax615::VinOpenOptions vin_options;
    vin_options.sensor_name = ax615_cfg_.sensor_name;
    vin_options.chn_frame_mode = sensor_.chn_frame_mode;
    vin_options.lane_combo = AX_LANE_COMBO_MODE_2;  // ✅ CRITICAL: OS04D10 使用 LANE_COMBO_MODE_2（匹配 QSDemo）
    if (!vin_->Open(vin_options)) {
        std::fprintf(stderr, "[Pipeline][v20260611] vin_->Open() failed\n");
        return MediaStatusCode::InternalError;
    }
    std::fprintf(stderr, "[Pipeline][v20260611] VIN Open OK\n");

    std::fprintf(stderr, "[Pipeline][v20260611] Opening ISP, iq_bin=%s\n", sensor_.iq_bin_path.c_str());

    // 构造 OS04D10 Sensor 属性
    AX_SNS_ATTR_T sns_attr;
    std::memset(&sns_attr, 0, sizeof(sns_attr));
    sns_attr.nWidth = 2560;
    sns_attr.nHeight = 1440;
    sns_attr.fFrameRate = 30.0f;
    sns_attr.eSnsMode = AX_SNS_LINEAR_MODE;
    sns_attr.eRawType = AX_RT_RAW10;
    sns_attr.eBayerPattern = AX_BP_RGGB;
    sns_attr.bTestPatternEnable = AX_FALSE;

    ax615::IspConfig isp_cfg;
    isp_cfg.pipe_id = ax615_cfg_.pipe_id;
    isp_cfg.dev_id = ax615_cfg_.dev_id;
    isp_cfg.iq_bin_path = sensor_.iq_bin_path;
    isp_cfg.sns_clk_idx = 0;                     // ✅ CRITICAL: Sensor 时钟索引
    isp_cfg.sns_clk_rate = AX_SNS_CLK_24M;       // ✅ CRITICAL: OS04D10 需要 24MHz 时钟！
    isp_cfg.register_sensor = true;              // ✅ CRITICAL: 必须注册 Sensor！
    isp_cfg.register_3a = true;                  // ✅ CRITICAL: 必须注册 3A 算法！
    isp_cfg.sensor_handle = &gSnsos04d10Obj;    // ✅ CRITICAL: 传入 Sensor 句柄！
    isp_cfg.sns_attr = &sns_attr;                // ✅ CRITICAL: 传入 Sensor 属性！
    std::fprintf(stderr, "[Pipeline][DEBUG] ISP config: register_sensor=%d, register_3a=%d, sensor_handle=%p, sns_clk_rate=%d\n",
                 isp_cfg.register_sensor, isp_cfg.register_3a, isp_cfg.sensor_handle, isp_cfg.sns_clk_rate);
    if (!isp_->Open(isp_cfg)) {
        std::fprintf(stderr, "[Pipeline][v20260611] isp_->Open() failed\n");
        return MediaStatusCode::InternalError;
    }
    std::fprintf(stderr, "[Pipeline][v20260611] ISP Open OK\n");

    // ✅ CRITICAL: 立即 EnableChn（匹配 QSDemo，在 ISP Open 后立即调用）
    // QSDemo 在 ISP Init 后立即调用 StartChn (EnableChn)，不能延迟到后面
    std::fprintf(stderr, "[Pipeline][v20260611] VIN EnableChn (immediately after ISP Open)\n");
    if (!vin_->Enable()) {
        std::fprintf(stderr, "[Pipeline][v20260611] vin_->Enable() failed\n");
        return MediaStatusCode::InternalError;
    }
    std::fprintf(stderr, "[Pipeline][v20260611] VIN EnableChn OK\n");

    std::fprintf(stderr, "[Pipeline][v20260611] Creating VENC channels, count=%zu\n", ax615_cfg_.video_channels.size());
    for (const auto& ch : ax615_cfg_.video_channels) {
        ax615::VencChannelConfig venc_cfg;
        venc_cfg.chn_id = ch.venc_chn_id;
        venc_cfg.width = ch.width;
        venc_cfg.height = ch.height;
        venc_cfg.fps = ch.fps;
        venc_cfg.bitrate_kbps = ch.bitrate_kbps;
        venc_cfg.codec = ch.codec;
        venc_cfg.rc_mode = ch.rc_mode;
        venc_cfg.gop = static_cast<uint32_t>(ch.gop);
        venc_->SetFrameCallback(venc_cfg.chn_id, [this](const StreamFrame& frame) {
            ++video_frames_;
            video_service_->DispatchFrame(frame);
        });
        status = venc_->CreateChannel(venc_cfg);
        if (status != MediaStatusCode::Ok) {
            std::fprintf(stderr, "[Pipeline][v20260611] venc_->CreateChannel(%d) failed\n", venc_cfg.chn_id);
            return status;
        }
        std::fprintf(stderr, "[Pipeline][v20260611] VENC channel %d created OK\n", venc_cfg.chn_id);
    }

    for (const auto& ch : ax615_cfg_.jpeg_channels) {
        ax615::JencChannelConfig jenc_cfg;
        jenc_cfg.chn_id = ch.jenc_chn_id;
        jenc_cfg.width = ch.width;
        jenc_cfg.height = ch.height;
        jenc_cfg.sensor_fps = static_cast<float>(ax615_cfg_.normal_fps);
        jenc_cfg.capture_fps = static_cast<float>(ch.capture_fps);
        jenc_cfg.quality = ch.quality;
        jenc_->SetFrameCallback(jenc_cfg.chn_id, [this](const StreamFrame& frame) {
            ++video_frames_;
            video_service_->DispatchFrame(frame);
        });
        status = jenc_->Open(jenc_cfg);
        if (status != MediaStatusCode::Ok) {
            return status;
        }
    }
    std::fprintf(stderr, "[Pipeline][DEBUG] detect_config_.enabled=%d, detect_=%p\n", detect_config_.enabled, (void*)detect_.get());

    if (detect_config_.enabled && detect_) {
        ax615::AxDetectConfig detect_cfg;
        detect_cfg.stream_id = detect_config_.ivps_chn_id;
        if (!ax615_cfg_.video_channels.empty()) {
            detect_cfg.width = ax615_cfg_.video_channels.front().width;
            detect_cfg.height = ax615_cfg_.video_channels.front().height;
        }
        detect_->SetResultCallback([this](const DetectResultSummary& result) {
            ++detect_results_;
            osd_service_->UpdateDetectOverlay(result);
        });
        status = detect_->Open(detect_cfg);
        if (status != MediaStatusCode::Ok) {
            return status;
        }
    }

    if (osd_config_.enabled && osd_) {
        status = osd_->Open();
        if (status != MediaStatusCode::Ok) {
            return status;
        }
    }

    // NOTE: Links already established earlier (after IVPS Open, before VIN/ISP Open)
    // This matches SDK sample order: SYS Init → Link → IVPS Init → Camera Open

    // ========== QSDemo Sequence (qs_common_cam.c lines 282-348) ==========
    // 1. ISP Init (Create + Register + Open) ✓ Already done in StartAx615Adapters
    // 2. VIN StartChn (EnableChn) → happens in vin_->Enable() below
    // 3. VIN StartPipe → happens in vin_->Enable() below
    // 4. ISP Start → isp_->Start() below
    // 5. VIN EnableDev → happens in vin_->Enable() below
    // 6. ISP StreamOn → isp_->StreamOn() below
    // 7. IVPS Start → ivps_->Start() below
    // 8. VENC Start → venc_->Start() below

    std::fprintf(stderr, "[Pipeline][v20260611] ========== Starting Camera (VIN + ISP) ==========\n");

    // Step 1: ISP Start
    // NOTE: VIN Enable (EnableChn + StartPipe) 已经在 StartAx615Adapters 中调用了
    // CRITICAL: 必须在 VIN StartPipe 之后、VIN EnableDev 之前调用
    std::fprintf(stderr, "[Pipeline][v20260611] [1/3] Starting ISP\n");
    if (!isp_->Start()) {
        std::fprintf(stderr, "[Pipeline][v20260611] isp_->Start() failed\n");
        return MediaStatusCode::InternalError;
    }
    std::fprintf(stderr, "[Pipeline][v20260611] [1/3] ISP Start OK\n");

    // Step 2: VIN StartDev (EnableDev)
    // CRITICAL: 必须在 ISP Start 之后调用（匹配 QSDemo）
    std::fprintf(stderr, "[Pipeline][v20260611] [2/3] VIN StartDev (EnableDev)\n");
    if (!vin_->StartDev()) {
        std::fprintf(stderr, "[Pipeline][v20260611] vin_->StartDev() failed\n");
        return MediaStatusCode::InternalError;
    }
    std::fprintf(stderr, "[Pipeline][v20260611] [2/3] VIN StartDev OK\n");

    // Step 3: ISP StreamOn
    std::fprintf(stderr, "[Pipeline][v20260611] [3/3] ISP StreamOn\n");
    if (!isp_->StreamOn()) {
        std::fprintf(stderr, "[Pipeline][v20260611] isp_->StreamOn() failed\n");
        return MediaStatusCode::InternalError;
    }
    std::fprintf(stderr, "[Pipeline][v20260611] ========== Camera started successfully ==========\n");

    // ========== Starting backend processors (IVPS + VENC) ==========
    std::fprintf(stderr, "[Pipeline][v20260611] ========== Starting backend processors ==========\n");

    // Step 4: IVPS Start
    std::fprintf(stderr, "[Pipeline][v20260611] Starting IVPS\n");
    if (!ivps_->Start()) {
        std::fprintf(stderr, "[Pipeline][v20260611] ivps_->Start() failed\n");
        return MediaStatusCode::InternalError;
    }

    // Step 5: VENC Start (all channels)
    std::fprintf(stderr, "[Pipeline][v20260611] Starting VENC channels (count=%zu)\n", ax615_cfg_.video_channels.size());
    for (const auto& ch : ax615_cfg_.video_channels) {
        status = venc_->Start(ch.venc_chn_id);
        if (status != MediaStatusCode::Ok) {
            std::fprintf(stderr, "[Pipeline][v20260611] venc_->Start(%d) failed\n", ch.venc_chn_id);
            return status;
        }
    }
    std::fprintf(stderr, "[Pipeline][v20260611] ========== All modules started successfully ==========\n");

    // DEBUG: Commented out frame capture test - it was interfering with normal operation
    // The unlink/relink sequence may have caused issues with the data flow
    /*
    std::fprintf(stderr, "[Pipeline][DEBUG] Attempting to capture VIN frames for debugging...\n");
    {
        ax615::AxModPort src{ax615::AxModId::VIN, 0, 0};
        ax615::AxModPort dst{ax615::AxModId::IVPS, 0, 0};

        std::fprintf(stderr, "[Pipeline][DEBUG] Unlinking VIN->IVPS temporarily\n");
        if (link_->UnLink(src, dst)) {
            std::this_thread::sleep_for(std::chrono::milliseconds(500));

            int success_count = 0;
            for (int i = 0; i < 5 && success_count < 3; i++) {
                AX_IMG_INFO_T img_info;
                std::memset(&img_info, 0, sizeof(img_info));
                AX_S32 ret = AX_VIN_GetYuvFrame(0, AX_VIN_CHN_ID_MAIN, &img_info, 1000);
                if (ret == AX_SUCCESS) {
                    std::fprintf(stderr, "[Pipeline][DEBUG] Got VIN frame #%d: %ux%u, stride=%u, format=%d, phyAddr=0x%llx\n",
                                success_count + 1,
                                img_info.tFrameInfo.stVFrame.u32Width,
                                img_info.tFrameInfo.stVFrame.u32Height,
                                img_info.tFrameInfo.stVFrame.u32PicStride[0],
                                img_info.tFrameInfo.stVFrame.enImgFormat,
                                img_info.tFrameInfo.stVFrame.u64PhyAddr[0]);

                    if (success_count == 0) {
                        char filename[128];
                        snprintf(filename, sizeof(filename), "/tmp/vin_frame_%ux%u.yuv",
                                img_info.tFrameInfo.stVFrame.u32Width,
                                img_info.tFrameInfo.stVFrame.u32Height);
                        FILE* fp = fopen(filename, "wb");
                        if (fp) {
                            size_t y_size = img_info.tFrameInfo.stVFrame.u32PicStride[0] * img_info.tFrameInfo.stVFrame.u32Height;
                            size_t uv_size = y_size / 2;
                            if (img_info.tFrameInfo.stVFrame.u64VirAddr[0]) {
                                fwrite((void*)img_info.tFrameInfo.stVFrame.u64VirAddr[0], 1, y_size, fp);
                            }
                            if (img_info.tFrameInfo.stVFrame.u64VirAddr[1]) {
                                fwrite((void*)img_info.tFrameInfo.stVFrame.u64VirAddr[1], 1, uv_size, fp);
                            }
                            fclose(fp);
                            std::fprintf(stderr, "[Pipeline][DEBUG] Saved frame to %s\n", filename);
                        }
                    }

                    AX_VIN_ReleaseYuvFrame(0, AX_VIN_CHN_ID_MAIN, &img_info);
                    success_count++;
                } else {
                    std::fprintf(stderr, "[Pipeline][DEBUG] VIN GetYuvFrame attempt %d failed: 0x%x\n", i + 1, ret);
                }
            }

            std::fprintf(stderr, "[Pipeline][DEBUG] Captured %d frames, re-linking VIN->IVPS\n", success_count);
            link_->Link(src, dst);
        } else {
            std::fprintf(stderr, "[Pipeline][DEBUG] UnLink failed, skipping frame capture\n");
        }
    }
    */

    if (audio_config_.enabled) {
        status = audio_service_->StartCapture(audio_config_.enc_chn_id);
        if (status != MediaStatusCode::Ok) {
            return status;
        }
    }
    if (detect_config_.enabled) {
        status = detect_service_->StartDetect();
        if (status != MediaStatusCode::Ok) {
            return status;
        }
    }
#endif
    return MediaStatusCode::Ok;
}

void MediaPipelineManager::StopAx615Adapters() {
    std::fprintf(stderr, "[Pipeline][DEBUG] StopAx615Adapters begin\n");
#if defined(LIBMEDIA_BUILD_AX615_ADAPTERS)
    // Shutdown sequence matching SDK sample (reverse of initialization):
    // 1. Stop downstream consumers first (VENC, JENC, OSD, Detect)
    // 2. Stop pipeline processors (IVPS, ISP, VIN)
    // 3. Unlink modules (after all modules stopped)
    // 4. Deinitialize system

    std::fprintf(stderr, "[Pipeline][DEBUG] Checking detect_service\n");
    if (detect_service_) {
        detect_service_->StopDetect();
    }
    std::fprintf(stderr, "[Pipeline][DEBUG] Checking audio_service\n");
    if (audio_service_) {
        audio_service_->StopCapture(audio_config_.enc_chn_id);
        audio_service_->StopPlayback(audio_config_.play_dev_id);
    }
    std::fprintf(stderr, "[Pipeline][DEBUG] Checking detect\n");
    if (detect_) {
        detect_->Close();
    }
    std::fprintf(stderr, "[Pipeline][DEBUG] Checking osd\n");
    if (osd_) {
        osd_->ClearAll();
        osd_->Close();
    }
    std::fprintf(stderr, "[Pipeline][DEBUG] Checking audio_capture\n");
    if (audio_capture_) {
        audio_capture_->Close();
    }
    std::fprintf(stderr, "[Pipeline][DEBUG] Checking audio_play\n");
    if (audio_play_) {
        audio_play_->Close();
    }

    // Stop and destroy VENC channels
    std::fprintf(stderr, "[Pipeline][DEBUG] Stopping VENC\n");
    if (venc_) {
        for (const auto& ch : ax615_cfg_.video_channels) {
            venc_->Stop(ch.venc_chn_id);
            venc_->DestroyChannel(ch.venc_chn_id);
        }
    }
    std::fprintf(stderr, "[Pipeline][DEBUG] Checking jenc\n");
    if (jenc_) {
        jenc_->CloseAll();
    }

    // Stop and close pipeline modules
    // CRITICAL: Stop VIN data flow first, then stop ISP, then close in reverse order
    std::fprintf(stderr, "[Pipeline][DEBUG] Disabling VIN (stop data flow)\n");
    if (vin_) {
        vin_->Disable();
    }
    std::fprintf(stderr, "[Pipeline][DEBUG] Stopping ISP\n");
    if (isp_) {
        isp_->Stop();
    }
    std::fprintf(stderr, "[Pipeline][DEBUG] Stopping IVPS\n");
    if (ivps_) {
        ivps_->Stop();
    }

    // Now close in reverse order (IVPS -> ISP -> VIN)
    std::fprintf(stderr, "[Pipeline][DEBUG] Closing IVPS\n");
    if (ivps_) {
        ivps_->Close();
    }
    std::fprintf(stderr, "[Pipeline][DEBUG] Closing ISP\n");
    if (isp_) {
        isp_->Close();
    }
    std::fprintf(stderr, "[Pipeline][DEBUG] Closing VIN\n");
    if (vin_) {
        vin_->Close();
    }

    // CRITICAL: Unlink AFTER all modules are stopped (matches SDK sample order)
    std::fprintf(stderr, "[Pipeline][DEBUG] UnLinking\n");
    if (link_) {
        link_->UnLinkAll();
    }

    // Finally deinitialize system
    std::fprintf(stderr, "[Pipeline][DEBUG] Deinit SYS\n");
    if (sys_) {
        sys_->Deinit();
    }
    std::fprintf(stderr, "[Pipeline][DEBUG] StopAx615Adapters completed\n");
#endif
}

MediaStatusCode MediaPipelineManager::EnterAx615LowPower() {
#if defined(LIBMEDIA_BUILD_AX615_ADAPTERS)
    if (!power_) {
        return MediaStatusCode::InvalidState;
    }
    ax615::AovPowerConfig cfg;
    cfg.normal_fps = static_cast<float>(low_power_profile_.normal_fps > 0 ? low_power_profile_.normal_fps
                                                                          : ax615_cfg_.normal_fps);
    cfg.aov_venc_fps = static_cast<float>(low_power_profile_.record_fps);
    cfg.aov_pipe_dst_fps = static_cast<float>(low_power_profile_.record_fps);
    cfg.normal_gop = static_cast<uint32_t>(low_power_profile_.gop > 0 ? low_power_profile_.gop
                                                                      : ax615_cfg_.normal_fps * 2);
    cfg.aov_gop = static_cast<uint32_t>(low_power_profile_.record_fps > 0
                                            ? low_power_profile_.record_fps * 2
                                            : 2);
    cfg.tcm_cap_frame_num = static_cast<uint32_t>(low_power_profile_.target_frame_count);
    cfg.pipes.push_back({ax615_cfg_.pipe_id, ax615_cfg_.enable_tcm});
    for (const auto& ch : ax615_cfg_.video_channels) {
        cfg.venc_channels.push_back({ch.venc_chn_id, ch.codec,
                                     static_cast<uint32_t>(ch.width), static_cast<uint32_t>(ch.height)});
    }
    return power_->ApplyLowPower(cfg);
#else
    return MediaStatusCode::Ok;
#endif
}

MediaStatusCode MediaPipelineManager::ResumeAx615Normal() {
#if defined(LIBMEDIA_BUILD_AX615_ADAPTERS)
    if (!power_) {
        return MediaStatusCode::InvalidState;
    }
    ax615::AovPowerConfig cfg;
    cfg.normal_fps = static_cast<float>(low_power_profile_.normal_fps > 0 ? low_power_profile_.normal_fps
                                                                          : ax615_cfg_.normal_fps);
    cfg.aov_venc_fps = static_cast<float>(low_power_profile_.record_fps > 0 ? low_power_profile_.record_fps : 1);
    cfg.aov_pipe_dst_fps = cfg.aov_venc_fps;
    cfg.normal_gop = static_cast<uint32_t>(low_power_profile_.gop > 0 ? low_power_profile_.gop
                                                                      : ax615_cfg_.normal_fps * 2);
    cfg.aov_gop = static_cast<uint32_t>(low_power_profile_.record_fps > 0
                                            ? low_power_profile_.record_fps * 2
                                            : 2);
    cfg.tcm_cap_frame_num = static_cast<uint32_t>(low_power_profile_.target_frame_count > 0
                                                      ? low_power_profile_.target_frame_count
                                                      : 1);
    cfg.pipes.push_back({ax615_cfg_.pipe_id, ax615_cfg_.enable_tcm});
    for (const auto& ch : ax615_cfg_.video_channels) {
        cfg.venc_channels.push_back({ch.venc_chn_id, ch.codec,
                                     static_cast<uint32_t>(ch.width), static_cast<uint32_t>(ch.height)});
    }
    return power_->RestoreNormal(cfg);
#else
    return MediaStatusCode::Ok;
#endif
}

MediaStatusCode MediaPipelineManager::BuildAx615PipelineFromPublicVideoConfig(Ax615PipelineConfig& cfg) const {
    cfg = {};
    cfg.sensor_name = sensor_.sensor_id == SensorId::OS04D10 ? "os04d10" : "os04d10";
    cfg.pipe_id = sensor_.pipe_id;
    cfg.dev_id = sensor_.dev_id;
    cfg.ivps_grp_id = 0;
    cfg.normal_fps = video_config_.main_stream.fps_normal > 0 ? video_config_.main_stream.fps_normal
                                                              : (sensor_.fps > 0 ? sensor_.fps : 15);
    cfg.enable_tcm = ax615_cfg_.enable_tcm;

    const auto append_video = [&cfg](const VideoEncodeApplyConfig& src, int default_ivps_chn) {
        if (!src.enabled) {
            return;
        }
        Ax615VideoChannelConfig dst;
        dst.ivps_chn_id = default_ivps_chn;
        dst.venc_chn_id = src.chn_id;
        dst.width = src.resolution.width;
        dst.height = src.resolution.height;
        dst.fps = src.fps_normal;
        dst.fps_aov = src.fps_aov;
        dst.bitrate_kbps = src.bitrate_kbps;
        dst.gop = src.gop;
        dst.codec = src.codec;
        dst.rc_mode = src.rc_mode;
        cfg.video_channels.push_back(dst);
    };

    append_video(video_config_.main_stream, 0);
    append_video(video_config_.sub_stream, 1);

    if (cfg.video_channels.empty()) {
        return MediaStatusCode::InvalidArgument;
    }
    return MediaStatusCode::Ok;
}

MediaStatusCode MediaPipelineManager::RebuildAx615PipelineFromPublicVideoConfig() {
    Ax615PipelineConfig cfg;
    const MediaStatusCode status = BuildAx615PipelineFromPublicVideoConfig(cfg);
    if (status != MediaStatusCode::Ok) {
        return status;
    }
    return ConfigureAx615Pipeline(cfg);
}

void MediaPipelineManager::SetLastError(MediaStatusCode code, int sdk_error, std::string message) {
    runtime_status_.last_error.code = code;
    runtime_status_.last_error.sdk_error = sdk_error;
    runtime_status_.last_error.message = std::move(message);
    runtime_status_.state = MediaRuntimeState::Error;
}

void MediaPipelineManager::ClearLastError() {
    runtime_status_.last_error = {};
}

void MediaPipelineManager::RefreshRuntimeStatus() {
    runtime_status_.initialized = initialized_;
    runtime_status_.running = running_;
    runtime_status_.low_power = low_power_;
    runtime_status_.prepared_for_aov_sleep = prepared_for_aov_sleep_;
    runtime_status_.video_frames = video_frames_;
    runtime_status_.audio_frames = audio_frames_;
    runtime_status_.detect_results = detect_results_;
    runtime_status_.video_capturing = false;
    runtime_status_.audio_capturing = false;
    runtime_status_.detect_running = detect_service_ && detect_service_->IsDetecting();

    for (const auto& ch : ax615_cfg_.video_channels) {
        if (video_service_ && video_service_->IsCapturing(ch.venc_chn_id)) {
            runtime_status_.video_capturing = true;
            break;
        }
    }
    if (audio_service_) {
        runtime_status_.audio_capturing = audio_service_->IsCapturing(audio_config_.enc_chn_id);
    }

    if (runtime_status_.last_error.code != MediaStatusCode::Ok) {
        runtime_status_.state = MediaRuntimeState::Error;
        return;
    }
    if (!initialized_) {
        runtime_status_.state = MediaRuntimeState::Uninitialized;
    } else if (prepared_for_aov_sleep_) {
        runtime_status_.state = MediaRuntimeState::PreparedForAovSleep;
    } else if (running_ && low_power_) {
        runtime_status_.state = MediaRuntimeState::RunningLowPower;
    } else if (running_) {
        runtime_status_.state = MediaRuntimeState::RunningNormal;
    } else {
        runtime_status_.state = MediaRuntimeState::Initialized;
    }
}

}  // namespace aov::media
