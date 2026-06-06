#pragma once
#include "detect/i_detect_service.hpp"
#include <atomic>
#include <functional>
#include <thread>

namespace aov::media {

class DetectService : public IDetectService {
public:
    DetectService() = default;
    ~DetectService() override { StopDetect(); }

    MediaStatusCode ApplyDetectConfig(const DetectApplyConfig& config) override;
    MediaStatusCode RunOnceDetect(DetectResultSummary& result) override;
    void RegisterDetectCallback(DetectCallback cb) override { callback_ = std::move(cb); }
    MediaStatusCode StartDetect() override;
    void StopDetect() override;
    bool IsDetecting() const override { return running_; }
    void SetRunOnceHandler(std::function<MediaStatusCode(DetectResultSummary&)> handler);

private:
    void DetectLoop();

    DetectCallback  callback_;
    DetectApplyConfig config_{};
    std::function<MediaStatusCode(DetectResultSummary&)> run_once_handler_;
    std::atomic<bool> running_{false};
    std::thread     thread_;
};

}  // namespace aov::media
