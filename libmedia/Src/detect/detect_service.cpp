#include "detect_service.hpp"

#include <chrono>

namespace aov::media {

MediaStatusCode DetectService::ApplyDetectConfig(const DetectApplyConfig& config) {
    if (config.pipe_id < 0 || config.ivps_grp_id < 0 || config.ivps_chn_id < 0) {
        return MediaStatusCode::InvalidArgument;
    }
    for (const auto& rule : config.rules) {
        if (rule.pipe_id < 0 || rule.ivps_grp_id < 0 || rule.ivps_chn_id < 0 ||
            rule.sensitivity < 0 || rule.sensitivity > 100) {
            return MediaStatusCode::InvalidArgument;
        }
    }
    config_ = config;
    return MediaStatusCode::Ok;
}

MediaStatusCode DetectService::RunOnceDetect(DetectResultSummary& result) {
    result = {};
    return run_once_handler_ ? run_once_handler_(result) : MediaStatusCode::Ok;
}

MediaStatusCode DetectService::StartDetect() {
    if (running_.exchange(true)) {
        return MediaStatusCode::Ok;
    }
    thread_ = std::thread(&DetectService::DetectLoop, this);
    return MediaStatusCode::Ok;
}

void DetectService::StopDetect() {
    if (!running_.exchange(false)) {
        return;
    }
    if (thread_.joinable()) {
        thread_.join();
    }
}

void DetectService::DetectLoop() {
    while (running_) {
        if (callback_) {
            DetectResultSummary result;
            const MediaStatusCode status = RunOnceDetect(result);
            if (status == MediaStatusCode::Ok) {
                callback_(result);
            }
        }
        std::this_thread::sleep_for(std::chrono::milliseconds(100));
    }
}

void DetectService::SetRunOnceHandler(std::function<MediaStatusCode(DetectResultSummary&)> handler) {
    run_once_handler_ = std::move(handler);
}

}  // namespace aov::media
