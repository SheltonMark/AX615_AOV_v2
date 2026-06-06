#pragma once

#include <functional>
#include "common/media_config.hpp"
#include "common/media_status.hpp"
#include "common/media_types.hpp"

namespace aov::media {

using DetectCallback = std::function<void(const DetectResultSummary&)>;

class IDetectService {
public:
    virtual ~IDetectService() = default;

    virtual MediaStatusCode ApplyDetectConfig(const DetectApplyConfig& config) = 0;
    virtual MediaStatusCode RunOnceDetect(DetectResultSummary& result) = 0;
    virtual void RegisterDetectCallback(DetectCallback cb) = 0;
    virtual MediaStatusCode StartDetect() = 0;
    virtual void StopDetect() = 0;
    virtual bool IsDetecting() const = 0;
};

}  // namespace aov::media
