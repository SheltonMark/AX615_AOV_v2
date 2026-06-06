#include "libmedia/Include/audio/i_audio_stream_service.hpp"
#include "libmedia/Include/common/media_config.hpp"
#include "libmedia/Include/common/media_status.hpp"
#include "libmedia/Include/common/media_types.hpp"
#include "libmedia/Include/detect/i_detect_service.hpp"
#include "libmedia/Include/isp/i_isp_service.hpp"
#include "libmedia/Include/osd/i_osd_service.hpp"
#include "libmedia/Include/runtime/i_media_runtime.hpp"
#include "libmedia/Include/video/i_video_stream_service.hpp"
#include "libmedia/Src/pipeline/media_pipeline_factory.hpp"

#include <memory>
#include <type_traits>

int main() {
    using namespace aov::media;

    static_assert(std::is_enum<MediaStatusCode>::value, "MediaStatusCode must be an enum");
    static_assert(std::is_enum<VideoCodec>::value, "VideoCodec must be an enum");

    StreamFrame frame;
    frame.chn_id = 0;
    frame.codec = VideoCodec::H264;
    frame.data = nullptr;
    frame.size = 0;
    frame.pts_ms = 0;
    frame.sequence = 0;
    frame.is_keyframe = false;

    LowPowerRecordProfile low_power;
    low_power.target_frame_count = 1;
    low_power.record_fps = 1;
    low_power.normal_fps = 15;
    low_power.gop = 60;

    AovSleepMediaConfig sleep_config;
    sleep_config.enable_low_power_wakeup = true;
    sleep_config.enable_tcm_wakeup = true;

    WakeupMediaProfile wakeup;
    wakeup.reason = WakeupReason::Timer;
    wakeup.start_video = true;
    wakeup.start_detect_confirm = false;
    wakeup.low_power_record = low_power;

    MediaRuntimeStatus runtime_status;
    runtime_status.state = MediaRuntimeState::Uninitialized;

    OsdStatusFlags osd;
    osd.recording = true;
    osd.low_battery = false;

    std::unique_ptr<IMediaRuntime> runtime = CreateAx615MediaRuntime();
    SensorConfig sensor;
    CmPoolConfig pool;
    if (runtime->Init(sensor, pool) != MediaStatusCode::Ok) {
        return 1;
    }
    if (runtime->Start() != MediaStatusCode::Ok) {
        return 2;
    }
    if (!runtime->IsRunning()) {
        return 3;
    }
    runtime->Stop();
    if (runtime->IsRunning()) {
        return 4;
    }
    if (runtime->PrepareForAovSleep(sleep_config) != MediaStatusCode::Ok) {
        return 5;
    }
    if (runtime->GetRuntimeStatus().state != MediaRuntimeState::PreparedForAovSleep) {
        return 6;
    }
    if (runtime->RestoreFromAovSleep(wakeup) != MediaStatusCode::Ok) {
        return 7;
    }
    if (runtime->GetRuntimeStatus().state != MediaRuntimeState::RunningLowPower) {
        return 8;
    }
    runtime->Stop();

    VideoEncodeApplyConfig video_config;
    video_config.chn_id = 0;
    video_config.fps_normal = 15;
    video_config.fps_aov = 1;

    AudioEncodeApplyConfig audio_config;
    audio_config.enabled = true;
    audio_config.sample_rate_hz = 16000;

    IspImageConfig image_config;
    image_config.white_balance.mode = ImageAutoMode::Auto;

    IVideoStreamService* video = nullptr;
    IAudioStreamService* audio = nullptr;
    IIspService* isp = nullptr;
    IDetectService* detect = nullptr;
    IOsdService* osd_service = nullptr;

    (void)frame;
    (void)runtime_status;
    (void)osd;
    (void)video_config;
    (void)audio_config;
    (void)image_config;
    (void)video;
    (void)audio;
    (void)isp;
    (void)detect;
    (void)osd_service;
    return 0;
}
