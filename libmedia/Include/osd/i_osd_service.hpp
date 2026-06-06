#pragma once

#include "common/media_config.hpp"
#include "common/media_status.hpp"
#include "common/media_types.hpp"

namespace aov::media {

class IOsdService {
public:
    virtual ~IOsdService() = default;

    virtual MediaStatusCode ApplyOsdConfig(const OsdApplyConfig& config) = 0;
    virtual void UpdateDetectOverlay(const DetectResultSummary& result) = 0;
    virtual void UpdateTimestamp(uint64_t epoch_ms) = 0;
    virtual void UpdateStatusIcon(const OsdStatusFlags& flags) = 0;
    virtual void ClearAll() = 0;
};

}  // namespace aov::media
