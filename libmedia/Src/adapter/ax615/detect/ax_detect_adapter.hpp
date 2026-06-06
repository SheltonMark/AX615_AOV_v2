#pragma once

#include "common/media_types.hpp"
#include <functional>
#include <string>

extern "C" {
#include "ax_skel_api.h"
}

namespace aov::media::ax615 {

struct AxDetectConfig {
    std::string model_path = "/opt/etc/skelModels";
    int stream_id = 0;
    int frame_depth = 1;
    int width = 640;
    int height = 352;
    bool enable_track = false;
    bool enable_push = false;
};

using AxDetectCallback = std::function<void(const DetectResultSummary& result)>;

class AxDetectAdapter {
public:
    AxDetectAdapter() = default;
    ~AxDetectAdapter();

    AxDetectAdapter(const AxDetectAdapter&) = delete;
    AxDetectAdapter& operator=(const AxDetectAdapter&) = delete;

    MediaStatusCode Open(const AxDetectConfig& cfg);
    void Close();
    MediaStatusCode RunOnce(DetectResultSummary& result);
    MediaStatusCode SendFrame(int stream_id, uint64_t frame_id, const AX_VIDEO_FRAME_T& frame, int timeout_ms);
    void SetResultCallback(AxDetectCallback cb);

    bool IsOpen() const { return opened_; }

private:
    AxDetectConfig cfg_{};
    AxDetectCallback callback_;
    AX_SKEL_HANDLE handle_ = nullptr;
    bool sdk_initialized_ = false;
    bool opened_ = false;

    static void OnSkelResult(AX_SKEL_HANDLE handle, AX_SKEL_RESULT_T* result, AX_VOID* user_data);
    static DetectResultSummary ToSummary(const AX_SKEL_RESULT_T& result);
};

}  // namespace aov::media::ax615
