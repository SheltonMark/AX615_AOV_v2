#pragma once

#include "ax_audio_types.hpp"
#include <atomic>
#include <functional>
#include <thread>

namespace aov::media::ax615 {

using AxAudioFrameCallback = std::function<void(const AudioFrame& frame)>;

class AxAudioCaptureAdapter {
public:
    AxAudioCaptureAdapter() = default;
    ~AxAudioCaptureAdapter();

    AxAudioCaptureAdapter(const AxAudioCaptureAdapter&) = delete;
    AxAudioCaptureAdapter& operator=(const AxAudioCaptureAdapter&) = delete;

    MediaStatusCode Open(const AxAudioCaptureConfig& cfg);
    MediaStatusCode Start();
    void Stop();
    void Close();
    void SetFrameCallback(AxAudioFrameCallback cb);

    bool IsOpen() const { return opened_; }
    bool IsRunning() const { return running_; }

private:
    void CaptureLoop();

    AxAudioCaptureConfig cfg_{};
    AxAudioFrameCallback callback_;
    std::atomic_bool thread_running_{false};
    std::thread stream_thread_;
    bool ai_initialized_ = false;
    bool ai_enabled_ = false;
    bool aenc_initialized_ = false;
    bool aenc_created_ = false;
    bool opened_ = false;
    bool running_ = false;
};

}  // namespace aov::media::ax615
