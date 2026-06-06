#include "libmedia/Src/pipeline/media_pipeline_manager.hpp"
#include "libmedia/Include/audio/i_audio_stream_service.hpp"
#include "libmedia/Include/isp/i_isp_service.hpp"
#include "libmedia/Include/video/i_video_stream_service.hpp"

int main()
{
    using namespace aov::media;

    MediaPipelineManager manager;

    Ax615PipelineConfig cfg;
    cfg.sensor_name = "os04d10";
    cfg.pipe_id = 0;
    cfg.dev_id = 0;
    cfg.ivps_grp_id = 0;
    cfg.normal_fps = 15;

    Ax615VideoChannelConfig video;
    video.ivps_chn_id = 0;
    video.venc_chn_id = 0;
    video.width = 1920;
    video.height = 1080;
    video.fps = 15;
    video.bitrate_kbps = 2048;
    cfg.video_channels.push_back(video);

    Ax615JpegChannelConfig jpeg;
    jpeg.ivps_chn_id = 0;
    jpeg.jenc_chn_id = 8;
    jpeg.width = 1920;
    jpeg.height = 1080;
    cfg.jpeg_channels.push_back(jpeg);

    if (manager.ConfigureAx615Pipeline({}) != MediaStatusCode::InvalidArgument) {
        return 1;
    }
    if (manager.ConfigureAx615Pipeline(cfg) != MediaStatusCode::Ok) {
        return 2;
    }
    if (!manager.IsPipelineConfigured()) {
        return 3;
    }
    if (manager.GetStartupPlan().size() < 7) {
        return 4;
    }
    if (manager.GetLinkPlan().size() != 3) {
        return 5;
    }
    if (manager.EnterLowPowerMode({0, 1, 15, 60}) != MediaStatusCode::InvalidArgument) {
        return 6;
    }
    SensorConfig sensor;
    sensor.pipe_id = cfg.pipe_id;
    sensor.dev_id = cfg.dev_id;
    sensor.fps = cfg.normal_fps;
    sensor.resolution = {1920, 1080};
    if (manager.Init(sensor, CmPoolConfig::DefaultAx615()) != MediaStatusCode::Ok) {
        return 7;
    }
    if (manager.Start() != MediaStatusCode::Ok) {
        return 8;
    }
    IVideoStreamService* stream = manager.GetVideoStreamService();
    if (!stream || !stream->IsCapturing(video.venc_chn_id)) {
        return 9;
    }
    if (!manager.GetAudioStreamService() || !manager.GetIspService()) {
        return 11;
    }
    manager.Stop();
    if (stream->IsCapturing(video.venc_chn_id)) {
        return 10;
    }
    if (manager.Start() != MediaStatusCode::Ok) {
        return 16;
    }
    if (!stream->IsCapturing(video.venc_chn_id)) {
        return 17;
    }
    VideoEncodeGroupConfig live_video_group;
    live_video_group.main_stream.chn_id = video.venc_chn_id;
    live_video_group.main_stream.codec = VideoCodec::H265;
    live_video_group.main_stream.resolution = {1280, 720};
    live_video_group.main_stream.fps_normal = 10;
    live_video_group.main_stream.fps_aov = 1;
    live_video_group.main_stream.rc_mode = RcMode::VBR;
    live_video_group.main_stream.bitrate_kbps = 1024;
    live_video_group.main_stream.gop = 40;
    live_video_group.sub_stream.enabled = false;
    if (manager.ApplyVideoConfig(live_video_group) != MediaStatusCode::Ok) {
        return 26;
    }
    if (!manager.IsRunning() || !stream->IsCapturing(video.venc_chn_id)) {
        return 27;
    }
    manager.Stop();

    AovSleepMediaConfig sleep_config;
    sleep_config.enable_tcm_wakeup = true;
    sleep_config.pipe_id = cfg.pipe_id;
    if (manager.PrepareForAovSleep(sleep_config) != MediaStatusCode::Ok) {
        return 18;
    }
    MediaRuntimeStatus sleep_status = manager.GetRuntimeStatus();
    if (sleep_status.state != MediaRuntimeState::PreparedForAovSleep ||
        sleep_status.running || sleep_status.video_capturing || sleep_status.detect_running) {
        return 19;
    }

    WakeupMediaProfile timer_wakeup;
    timer_wakeup.reason = WakeupReason::Timer;
    timer_wakeup.start_video = true;
    timer_wakeup.start_detect_confirm = false;
    timer_wakeup.low_power_record.target_frame_count = 1;
    timer_wakeup.low_power_record.record_fps = 1;
    timer_wakeup.low_power_record.normal_fps = 15;
    timer_wakeup.low_power_record.gop = 60;
    if (manager.RestoreFromAovSleep(timer_wakeup) != MediaStatusCode::Ok) {
        return 20;
    }
    MediaRuntimeStatus wake_status = manager.GetRuntimeStatus();
    if (wake_status.state != MediaRuntimeState::RunningLowPower ||
        !wake_status.running || !wake_status.video_capturing) {
        return 21;
    }
    manager.Stop();

    MediaPipelineManager configured_from_public_api;
    VideoEncodeGroupConfig video_group;
    video_group.main_stream.chn_id = 2;
    video_group.main_stream.codec = VideoCodec::H265;
    video_group.main_stream.resolution = {2560, 1440};
    video_group.main_stream.fps_normal = 12;
    video_group.main_stream.fps_aov = 1;
    video_group.main_stream.rc_mode = RcMode::VBR;
    video_group.main_stream.bitrate_kbps = 1536;
    video_group.main_stream.gop = 48;
    video_group.sub_stream.enabled = false;
    if (configured_from_public_api.ApplyVideoConfig(video_group) != MediaStatusCode::Ok) {
        return 22;
    }
    SensorConfig sensor2;
    sensor2.pipe_id = 0;
    sensor2.dev_id = 0;
    sensor2.fps = 15;
    sensor2.resolution = {1920, 1080};
    if (configured_from_public_api.Init(sensor2, CmPoolConfig::DefaultAx615()) != MediaStatusCode::Ok) {
        return 23;
    }
    const auto& links = configured_from_public_api.GetLinkPlan();
    if (links.size() != 2 || links[1].dst != "VENC" || links[1].dst_chn != 2) {
        return 24;
    }

    IspImageConfig image;
    image.mirror = true;
    image.flip = true;
    if (configured_from_public_api.ApplyImageConfig(image) != MediaStatusCode::Ok) {
        return 25;
    }

    return 0;
}
