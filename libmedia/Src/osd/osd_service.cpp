#include "osd_service.hpp"

namespace aov::media {

MediaStatusCode OsdService::ApplyOsdConfig(const OsdApplyConfig& config) {
    if (config.ivps_grp_id < 0 || config.ivps_chn_id < 0) {
        return MediaStatusCode::InvalidArgument;
    }
    if (apply_handler_) {
        const MediaStatusCode status = apply_handler_(config);
        if (status != MediaStatusCode::Ok) {
            return status;
        }
    }
    std::lock_guard<std::mutex> lock(mutex_);
    config_ = config;
    return MediaStatusCode::Ok;
}

void OsdService::UpdateDetectOverlay(const DetectResultSummary& result) {
    if (detect_overlay_handler_) {
        (void)detect_overlay_handler_(result);
    }
    std::lock_guard<std::mutex> lock(mutex_);
    detect_ = result;
}

void OsdService::UpdateTimestamp(uint64_t epoch_ms) {
    std::lock_guard<std::mutex> lock(mutex_);
    timestamp_ms_ = epoch_ms;
}

void OsdService::UpdateStatusIcon(const OsdStatusFlags& flags) {
    std::lock_guard<std::mutex> lock(mutex_);
    flags_ = flags;
}

void OsdService::ClearAll() {
    if (clear_handler_) {
        (void)clear_handler_();
    }
    std::lock_guard<std::mutex> lock(mutex_);
    detect_ = {};
    flags_ = {};
    timestamp_ms_ = 0;
}

void OsdService::SetApplyHandler(std::function<MediaStatusCode(const OsdApplyConfig&)> handler) {
    apply_handler_ = std::move(handler);
}

void OsdService::SetDetectOverlayHandler(std::function<MediaStatusCode(const DetectResultSummary&)> handler) {
    detect_overlay_handler_ = std::move(handler);
}

void OsdService::SetClearHandler(std::function<MediaStatusCode()> handler) {
    clear_handler_ = std::move(handler);
}

}  // namespace aov::media
