#pragma once

#include "osd/i_osd_service.hpp"
#include <functional>
#include <mutex>

namespace aov::media {

class OsdService final : public IOsdService {
public:
    OsdService() = default;
    ~OsdService() override = default;

    MediaStatusCode ApplyOsdConfig(const OsdApplyConfig& config) override;
    void UpdateDetectOverlay(const DetectResultSummary& result) override;
    void UpdateTimestamp(uint64_t epoch_ms) override;
    void UpdateStatusIcon(const OsdStatusFlags& flags) override;
    void ClearAll() override;
    void SetApplyHandler(std::function<MediaStatusCode(const OsdApplyConfig&)> handler);
    void SetDetectOverlayHandler(std::function<MediaStatusCode(const DetectResultSummary&)> handler);
    void SetClearHandler(std::function<MediaStatusCode()> handler);

private:
    mutable std::mutex mutex_;
    DetectResultSummary detect_{};
    OsdStatusFlags flags_{};
    OsdApplyConfig config_{};
    uint64_t timestamp_ms_ = 0;
    std::function<MediaStatusCode(const OsdApplyConfig&)> apply_handler_;
    std::function<MediaStatusCode(const DetectResultSummary&)> detect_overlay_handler_;
    std::function<MediaStatusCode()> clear_handler_;
};

}  // namespace aov::media
