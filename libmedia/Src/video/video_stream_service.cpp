#include "video_stream_service.hpp"

namespace aov::media {

MediaStatusCode VideoStreamService::ApplyVideoEncodeConfig(const VideoEncodeApplyConfig& config) {
    if (config.chn_id < 0 || config.resolution.width <= 0 || config.resolution.height <= 0 ||
        config.fps_normal <= 0 || config.fps_aov <= 0 || config.bitrate_kbps <= 0 ||
        config.gop <= 0 || config.fix_qp < 0) {
        return MediaStatusCode::InvalidArgument;
    }
    std::lock_guard<std::mutex> lock(mutex_);
    encode_configs_[config.chn_id] = config;
    return MediaStatusCode::Ok;
}

MediaStatusCode VideoStreamService::ApplyVideoEncodeGroupConfig(const VideoEncodeGroupConfig& config) {
    MediaStatusCode status = ApplyVideoEncodeConfig(config.main_stream);
    if (status != MediaStatusCode::Ok) {
        return status;
    }
    status = ApplyVideoEncodeConfig(config.sub_stream);
    if (status != MediaStatusCode::Ok) {
        return status;
    }
    std::lock_guard<std::mutex> lock(mutex_);
    group_config_ = config;
    return MediaStatusCode::Ok;
}

MediaStatusCode VideoStreamService::RegisterStreamCallback(int chn_id, StreamCallback cb) {
    if (chn_id < 0 || !cb) {
        return MediaStatusCode::InvalidArgument;
    }
    std::lock_guard<std::mutex> lock(mutex_);
    callbacks_[chn_id] = std::move(cb);
    return MediaStatusCode::Ok;
}

MediaStatusCode VideoStreamService::StartCapture(int chn_id) {
    if (chn_id < 0) {
        return MediaStatusCode::InvalidArgument;
    }
    std::lock_guard<std::mutex> lock(mutex_);
    capturing_channels_.insert(chn_id);
    return MediaStatusCode::Ok;
}

void VideoStreamService::StopCapture(int chn_id) {
    std::lock_guard<std::mutex> lock(mutex_);
    capturing_channels_.erase(chn_id);
}

MediaStatusCode VideoStreamService::RequestKeyFrame(int chn_id) {
    if (chn_id < 0) {
        return MediaStatusCode::InvalidArgument;
    }
    return keyframe_request_handler_ ? keyframe_request_handler_(chn_id) : MediaStatusCode::Ok;
}

MediaStatusCode VideoStreamService::SetLowPowerFrameRate(int chn_id, int fps_aov) {
    if (chn_id < 0 || fps_aov <= 0) {
        return MediaStatusCode::InvalidArgument;
    }
    std::lock_guard<std::mutex> lock(mutex_);
    auto it = encode_configs_.find(chn_id);
    if (it == encode_configs_.end()) {
        return MediaStatusCode::NotFound;
    }
    it->second.fps_aov = fps_aov;
    return MediaStatusCode::Ok;
}

MediaStatusCode VideoStreamService::SetNormalFrameRate(int chn_id, int fps_normal) {
    if (chn_id < 0 || fps_normal <= 0) {
        return MediaStatusCode::InvalidArgument;
    }
    std::lock_guard<std::mutex> lock(mutex_);
    auto it = encode_configs_.find(chn_id);
    if (it == encode_configs_.end()) {
        return MediaStatusCode::NotFound;
    }
    it->second.fps_normal = fps_normal;
    return MediaStatusCode::Ok;
}

bool VideoStreamService::IsCapturing(int chn_id) const {
    std::lock_guard<std::mutex> lock(mutex_);
    return capturing_channels_.count(chn_id) > 0;
}

void VideoStreamService::DispatchFrame(const StreamFrame& frame) {
    StreamCallback cb;
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
    cb(frame.chn_id, frame);
}

void VideoStreamService::SetKeyFrameRequestHandler(std::function<MediaStatusCode(int)> handler) {
    keyframe_request_handler_ = std::move(handler);
}

}  // namespace aov::media
