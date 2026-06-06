#include "libmedia/Src/adapter/ax615/audio/ax_audio_capture_adapter.hpp"
#include "libmedia/Src/adapter/ax615/audio/ax_audio_play_adapter.hpp"
#include "libmedia/Src/adapter/ax615/detect/ax_detect_adapter.hpp"
#include "libmedia/Src/adapter/ax615/osd/ax_osd_adapter.hpp"
#include "libmedia/Src/audio/audio_stream_service.hpp"
#include "libmedia/Src/video/video_stream_service.hpp"

int main()
{
    using namespace aov::media;

    VideoStreamService video;
    AudioStreamService audio;
    ax615::AxDetectAdapter detect;
    ax615::AxOsdAdapter osd;
    ax615::AxAudioCaptureAdapter audio_capture;
    ax615::AxAudioPlayAdapter audio_play;

    if (video.StartCapture(0) != MediaStatusCode::Ok) {
        return 1;
    }
    if (!video.IsCapturing(0)) {
        return 2;
    }
    video.StopCapture(0);

    AudioFrame frame;
    frame.chn_id = 0;
    frame.codec = AudioCodec::AAC;
    if (audio.StartCapture(0) != MediaStatusCode::Ok) {
        return 3;
    }
    audio.DispatchFrame(frame);
    audio.StopCapture(0);

    DetectResultSummary summary;
    if (detect.RunOnce(summary) != MediaStatusCode::Unsupported) {
        return 4;
    }

    (void)osd;
    (void)audio_capture;
    (void)audio_play;
    return 0;
}
