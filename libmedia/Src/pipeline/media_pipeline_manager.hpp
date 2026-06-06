#pragma once

#include "runtime/i_media_runtime.hpp"
#include <memory>
#include <string>
#include <vector>

namespace aov::media {

class DetectService;
class AudioStreamService;
class IspService;
class OsdService;
class VideoStreamService;

namespace ax615 {
class AxSysAdapter;
class AxVinAdapter;
class AxIspAdapter;
class AxIvpsAdapter;
class AxVencAdapter;
class AxJencAdapter;
class AxLinkManager;
class AxAovPowerAdapter;
class AxAudioCaptureAdapter;
class AxAudioPlayAdapter;
class AxDetectAdapter;
class AxOsdAdapter;
}  // namespace ax615

struct Ax615VideoChannelConfig {
    int ivps_chn_id = 0;
    int venc_chn_id = 0;
    int width = 1920;
    int height = 1080;
    int fps = 15;
    int fps_aov = 1;
    int bitrate_kbps = 4096;
    int gop = 50;
    VideoCodec codec = VideoCodec::H264;
    RcMode rc_mode = RcMode::CBR;
};

struct Ax615JpegChannelConfig {
    int ivps_chn_id = 0;
    int jenc_chn_id = 8;
    int width = 1920;
    int height = 1080;
    int capture_fps = 1;
    int quality = 60;
};

struct Ax615PipelineConfig {
    std::string sensor_name;
    int pipe_id = 0;
    int dev_id = 0;
    int ivps_grp_id = 0;
    int normal_fps = 15;
    bool enable_tcm = false;
    std::vector<Ax615VideoChannelConfig> video_channels;
    std::vector<Ax615JpegChannelConfig> jpeg_channels;
};

struct PipelineLink {
    std::string src;
    int src_dev = 0;
    int src_chn = 0;
    std::string dst;
    int dst_dev = 0;
    int dst_chn = 0;
};

class MediaPipelineManager final : public IMediaRuntime {
public:
    MediaPipelineManager();
#if defined(LIBMEDIA_BUILD_AX615_ADAPTERS)
    MediaPipelineManager(
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
        std::unique_ptr<ax615::AxOsdAdapter> osd);
#endif
    ~MediaPipelineManager() override;

    MediaStatusCode ConfigureAx615Pipeline(const Ax615PipelineConfig& cfg);
    bool IsPipelineConfigured() const { return pipeline_configured_; }
    const std::vector<std::string>& GetStartupPlan() const { return startup_plan_; }
    const std::vector<PipelineLink>& GetLinkPlan() const { return link_plan_; }

    MediaStatusCode Init(const MediaRuntimeInitConfig& config) override;
    MediaStatusCode Init(const SensorConfig& sensor, const CmPoolConfig& pool) override;
    MediaStatusCode Start() override;
    void Stop() override;
    MediaStatusCode ApplyVideoConfig(const VideoEncodeGroupConfig& config) override;
    MediaStatusCode ApplyAudioConfig(const AudioEncodeApplyConfig& config) override;
    MediaStatusCode ApplyImageConfig(const IspImageConfig& config) override;
    MediaStatusCode ApplyDetectConfig(const DetectApplyConfig& config) override;
    MediaStatusCode ApplyOsdConfig(const OsdApplyConfig& config) override;
    MediaStatusCode PrepareForAovSleep(const AovSleepMediaConfig& config) override;
    MediaStatusCode RestoreFromAovSleep(const WakeupMediaProfile& profile) override;
    MediaStatusCode EnterLowPowerMode(const LowPowerRecordProfile& profile) override;
    MediaStatusCode ResumeNormalMode() override;
    MediaStatusCode CaptureSnapshot(const SnapshotRequest& request, SnapshotResult& result) override;

    IIspService* GetIspService() override;
    IVideoStreamService* GetVideoStreamService() override;
    IAudioStreamService* GetAudioStreamService() override;
    IDetectService* GetDetectService() override;
    IOsdService* GetOsdService() override;

    bool IsRunning() const override;
    MediaRuntimeStatus GetRuntimeStatus() const override;

private:
    SensorConfig sensor_{};
    CmPoolConfig pool_{};
    MediaRuntimeInitConfig init_config_{};
    Ax615PipelineConfig ax615_cfg_{};
    VideoEncodeGroupConfig video_config_{};
    AudioEncodeApplyConfig audio_config_{};
    DetectApplyConfig detect_config_{};
    OsdApplyConfig osd_config_{};
    LowPowerRecordProfile low_power_profile_{};
    AovSleepMediaConfig aov_sleep_config_{};
    WakeupMediaProfile wakeup_profile_{};
    MediaRuntimeStatus runtime_status_{};
    bool initialized_ = false;
    bool running_ = false;
    bool pipeline_configured_ = false;
    bool public_video_configured_ = false;
    bool low_power_ = false;
    bool prepared_for_aov_sleep_ = false;
    uint64_t video_frames_ = 0;
    uint64_t audio_frames_ = 0;
    uint64_t detect_results_ = 0;
    std::vector<std::string> startup_plan_;
    std::vector<PipelineLink> link_plan_;

    std::unique_ptr<DetectService> detect_service_;
    std::unique_ptr<VideoStreamService> video_service_;
    std::unique_ptr<AudioStreamService> audio_service_;
    std::unique_ptr<IspService> isp_service_;
    std::unique_ptr<OsdService> osd_service_;

#if defined(LIBMEDIA_BUILD_AX615_ADAPTERS)
    std::unique_ptr<ax615::AxSysAdapter> sys_;
    std::unique_ptr<ax615::AxVinAdapter> vin_;
    std::unique_ptr<ax615::AxIspAdapter> isp_;
    std::unique_ptr<ax615::AxIvpsAdapter> ivps_;
    std::unique_ptr<ax615::AxVencAdapter> venc_;
    std::unique_ptr<ax615::AxJencAdapter> jenc_;
    std::unique_ptr<ax615::AxLinkManager> link_;
    std::unique_ptr<ax615::AxAovPowerAdapter> power_;
    std::unique_ptr<ax615::AxAudioCaptureAdapter> audio_capture_;
    std::unique_ptr<ax615::AxAudioPlayAdapter> audio_play_;
    std::unique_ptr<ax615::AxDetectAdapter> detect_;
    std::unique_ptr<ax615::AxOsdAdapter> osd_;
#endif

    bool BuildPlans();
    MediaStatusCode StartCaptureChannels();
    void StopCaptureChannels();
    MediaStatusCode StartAx615Adapters();
    void StopAx615Adapters();
    MediaStatusCode EnterAx615LowPower();
    MediaStatusCode ResumeAx615Normal();
    MediaStatusCode RebuildAx615PipelineFromPublicVideoConfig();
    MediaStatusCode BuildAx615PipelineFromPublicVideoConfig(Ax615PipelineConfig& cfg) const;
    void SetLastError(MediaStatusCode code, int sdk_error, std::string message);
    void ClearLastError();
    void RefreshRuntimeStatus();
};

}  // namespace aov::media
