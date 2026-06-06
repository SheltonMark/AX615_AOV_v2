#pragma once

#include <functional>
#include "common/media_config.hpp"
#include "common/media_status.hpp"
#include "common/media_types.hpp"

namespace aov::media {

class IAudioStreamService {
public:
    virtual ~IAudioStreamService() = default;

    virtual MediaStatusCode ApplyAudioEncodeConfig(const AudioEncodeApplyConfig& config) = 0;
    virtual MediaStatusCode RegisterAudioCallback(int chn_id, AudioFrameCallback cb) = 0;
    virtual MediaStatusCode StartCapture(int chn_id) = 0;
    virtual void StopCapture(int chn_id) = 0;
    virtual MediaStatusCode StartPlayback(int dev_id) = 0;
    virtual void StopPlayback(int dev_id) = 0;
    virtual MediaStatusCode SendPlaybackFrame(const AudioFrame& frame) = 0;
    virtual bool IsCapturing(int chn_id) const = 0;
};

}  // namespace aov::media
