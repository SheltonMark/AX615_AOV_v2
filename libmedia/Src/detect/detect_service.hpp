#pragma once
#include "detect/i_detect_service.hpp"
#include <atomic>
#include <functional>
#include <thread>

#ifdef LIBMEDIA_BUILD_AX615_ADAPTERS
#include "ax_skel_api.h"
#endif

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

#ifdef LIBMEDIA_BUILD_AX615_ADAPTERS
    MediaStatusCode InitSkel();
    void DeinitSkel();
    MediaStatusCode GetFrameAndDetect();
    static void SkelResultCallback(AX_SKEL_HANDLE handle, AX_SKEL_RESULT_T* result, void* user_data);
#endif

    DetectCallback  callback_;
    DetectApplyConfig config_{};
    std::function<MediaStatusCode(DetectResultSummary&)> run_once_handler_;
    std::atomic<bool> running_{false};
    std::thread     thread_;

#ifdef LIBMEDIA_BUILD_AX615_ADAPTERS
    AX_SKEL_HANDLE skel_handle_{nullptr};
    std::atomic<uint64_t> frame_id_{0};
#endif
};

}  // namespace aov::media
