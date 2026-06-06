#include "audio_stream_service.hpp"

namespace aov::media {

MediaStatusCode AudioStreamService::ApplyAudioEncodeConfig(const AudioEncodeApplyConfig& config) {
    if (config.capture_dev_id < 0 || config.play_dev_id < 0 || config.sample_rate_hz <= 0 ||
        config.bit_depth <= 0 || config.channels <= 0 || config.input_volume < 0 ||
        config.input_volume > 100 || config.output_volume < 0 || config.output_volume > 100) {
        return MediaStatusCode::InvalidArgument;
    }
    std::lock_guard<std::mutex> lock(mutex_);
    encode_config_ = config;
    return MediaStatusCode::Ok;
}

MediaStatusCode AudioStreamService::RegisterAudioCallback(int chn_id, AudioFrameCallback cb) {
    if (chn_id < 0 || !cb) {
        return MediaStatusCode::InvalidArgument;
    }
    std::lock_guard<std::mutex> lock(mutex_);
    callbacks_[chn_id] = std::move(cb);
    return MediaStatusCode::Ok;
}

MediaStatusCode AudioStreamService::StartCapture(int chn_id) {
    if (chn_id < 0) {
        return MediaStatusCode::InvalidArgument;
    }
    if (capture_start_handler_) {
        const MediaStatusCode status = capture_start_handler_(chn_id);
        if (status != MediaStatusCode::Ok) {
            return status;
        }
    }
    std::lock_guard<std::mutex> lock(mutex_);
    capturing_channels_.insert(chn_id);
    return MediaStatusCode::Ok;
}

void AudioStreamService::StopCapture(int chn_id) {
    if (capture_stop_handler_) {
        capture_stop_handler_(chn_id);
    }
    std::lock_guard<std::mutex> lock(mutex_);
    capturing_channels_.erase(chn_id);
}

bool AudioStreamService::IsCapturing(int chn_id) const {
    std::lock_guard<std::mutex> lock(mutex_);
    return capturing_channels_.count(chn_id) > 0;
}

MediaStatusCode AudioStreamService::StartPlayback(int dev_id) {
    if (dev_id < 0) {
        return MediaStatusCode::InvalidArgument;
    }
    if (playback_start_handler_) {
        const MediaStatusCode status = playback_start_handler_(dev_id);
        if (status != MediaStatusCode::Ok) {
            return status;
        }
    }
    std::lock_guard<std::mutex> lock(mutex_);
    playback_devices_.insert(dev_id);
    return MediaStatusCode::Ok;
}

void AudioStreamService::StopPlayback(int dev_id) {
    if (playback_stop_handler_) {
        playback_stop_handler_(dev_id);
    }
    std::lock_guard<std::mutex> lock(mutex_);
    playback_devices_.erase(dev_id);
}

MediaStatusCode AudioStreamService::SendPlaybackFrame(const AudioFrame& frame) {
    if (!frame.data || frame.size == 0 || frame.chn_id < 0) {
        return MediaStatusCode::InvalidArgument;
    }
    {
        std::lock_guard<std::mutex> lock(mutex_);
        if (playback_devices_.empty()) {
            return MediaStatusCode::InvalidState;
        }
    }
    return playback_frame_handler_ ? playback_frame_handler_(frame) : MediaStatusCode::Unsupported;
}

void AudioStreamService::DispatchFrame(const AudioFrame& frame) {
    AudioFrameCallback cb;
    {
        std::lock_guard<std::mutex> lock(mutex_);
        if (capturing_channels_.count(frame.chn_id) == 0) {
            return;
        }
        auto it = callbacks_.find(frame.chn_id);
        if (it == callbacks_.end()) {
            return;
        }
        cb = it->second;
    }
    cb(frame);
}

void AudioStreamService::SetCaptureStartHandler(std::function<MediaStatusCode(int)> handler) {
    capture_start_handler_ = std::move(handler);
}

void AudioStreamService::SetCaptureStopHandler(std::function<void(int)> handler) {
    capture_stop_handler_ = std::move(handler);
}

void AudioStreamService::SetPlaybackStartHandler(std::function<MediaStatusCode(int)> handler) {
    playback_start_handler_ = std::move(handler);
}

void AudioStreamService::SetPlaybackStopHandler(std::function<void(int)> handler) {
    playback_stop_handler_ = std::move(handler);
}

void AudioStreamService::SetPlaybackFrameHandler(std::function<MediaStatusCode(const AudioFrame&)> handler) {
    playback_frame_handler_ = std::move(handler);
}

}  // namespace aov::media
