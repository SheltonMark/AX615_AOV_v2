#pragma once

#include "audio/i_audio_stream_service.hpp"
#include <functional>
#include <map>
#include <mutex>
#include <set>

namespace aov::media {

class AudioStreamService final : public IAudioStreamService {
public:
    AudioStreamService() = default;
    ~AudioStreamService() override = default;

    MediaStatusCode ApplyAudioEncodeConfig(const AudioEncodeApplyConfig& config) override;
    MediaStatusCode RegisterAudioCallback(int chn_id, AudioFrameCallback cb) override;
    MediaStatusCode StartCapture(int chn_id) override;
    void StopCapture(int chn_id) override;
    MediaStatusCode StartPlayback(int dev_id) override;
    void StopPlayback(int dev_id) override;
    MediaStatusCode SendPlaybackFrame(const AudioFrame& frame) override;
    bool IsCapturing(int chn_id) const override;

    void DispatchFrame(const AudioFrame& frame);
    void SetCaptureStartHandler(std::function<MediaStatusCode(int)> handler);
    void SetCaptureStopHandler(std::function<void(int)> handler);
    void SetPlaybackStartHandler(std::function<MediaStatusCode(int)> handler);
    void SetPlaybackStopHandler(std::function<void(int)> handler);
    void SetPlaybackFrameHandler(std::function<MediaStatusCode(const AudioFrame&)> handler);

private:
    mutable std::mutex mutex_;
    std::map<int, AudioFrameCallback> callbacks_;
    AudioEncodeApplyConfig encode_config_{};
    std::set<int> capturing_channels_;
    std::set<int> playback_devices_;
    std::function<MediaStatusCode(int)> capture_start_handler_;
    std::function<void(int)> capture_stop_handler_;
    std::function<MediaStatusCode(int)> playback_start_handler_;
    std::function<void(int)> playback_stop_handler_;
    std::function<MediaStatusCode(const AudioFrame&)> playback_frame_handler_;
};

}  // namespace aov::media
