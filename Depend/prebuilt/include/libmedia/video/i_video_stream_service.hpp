#pragma once

#include <functional>
#include "common/media_config.hpp"
#include "common/media_status.hpp"
#include "common/media_types.hpp"

namespace aov::media {

using StreamCallback = std::function<void(int chn_id, const StreamFrame& frame)>;

class IVideoStreamService {
public:
    virtual ~IVideoStreamService() = default;

    virtual MediaStatusCode ApplyVideoEncodeConfig(const VideoEncodeApplyConfig& config) = 0;
    virtual MediaStatusCode ApplyVideoEncodeGroupConfig(const VideoEncodeGroupConfig& config) = 0;
    virtual MediaStatusCode RegisterStreamCallback(int chn_id, StreamCallback cb) = 0;
    virtual MediaStatusCode StartCapture(int chn_id) = 0;
    virtual void StopCapture(int chn_id) = 0;
    virtual MediaStatusCode RequestKeyFrame(int chn_id) = 0;
    virtual MediaStatusCode SetLowPowerFrameRate(int chn_id, int fps_aov) = 0;
    virtual MediaStatusCode SetNormalFrameRate(int chn_id, int fps_normal) = 0;
    virtual bool IsCapturing(int chn_id) const = 0;
};

}  // namespace aov::media
