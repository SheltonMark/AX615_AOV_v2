#pragma once

#include "video/i_video_stream_service.hpp"
#include <functional>
#include <map>
#include <mutex>
#include <set>

namespace aov::media {

class VideoStreamService final : public IVideoStreamService {
public:
    VideoStreamService() = default;
    ~VideoStreamService() override = default;

    MediaStatusCode ApplyVideoEncodeConfig(const VideoEncodeApplyConfig& config) override;
    MediaStatusCode ApplyVideoEncodeGroupConfig(const VideoEncodeGroupConfig& config) override;
    MediaStatusCode RegisterStreamCallback(int chn_id, StreamCallback cb) override;
    MediaStatusCode StartCapture(int chn_id) override;
    void StopCapture(int chn_id) override;
    MediaStatusCode RequestKeyFrame(int chn_id) override;
    MediaStatusCode SetLowPowerFrameRate(int chn_id, int fps_aov) override;
    MediaStatusCode SetNormalFrameRate(int chn_id, int fps_normal) override;
    bool IsCapturing(int chn_id) const override;

    void DispatchFrame(const StreamFrame& frame);
    void SetKeyFrameRequestHandler(std::function<MediaStatusCode(int)> handler);

private:
    mutable std::mutex mutex_;
    std::map<int, StreamCallback> callbacks_;
    std::map<int, VideoEncodeApplyConfig> encode_configs_;
    VideoEncodeGroupConfig group_config_{};
    std::set<int> capturing_channels_;
    std::function<MediaStatusCode(int)> keyframe_request_handler_;
};

}  // namespace aov::media
