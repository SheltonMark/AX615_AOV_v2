#pragma once

#include "ax_audio_types.hpp"

namespace aov::media::ax615 {

class AxAudioPlayAdapter {
public:
    AxAudioPlayAdapter() = default;
    ~AxAudioPlayAdapter();

    AxAudioPlayAdapter(const AxAudioPlayAdapter&) = delete;
    AxAudioPlayAdapter& operator=(const AxAudioPlayAdapter&) = delete;

    MediaStatusCode Open(const AxAudioPlayConfig& cfg);
    MediaStatusCode Play(const AudioFrame& frame);
    void Close();

    bool IsOpen() const { return opened_; }

private:
    AxAudioPlayConfig cfg_{};
    bool adec_initialized_ = false;
    bool adec_created_ = false;
    bool ao_initialized_ = false;
    bool ao_enabled_ = false;
    bool opened_ = false;
};

}  // namespace aov::media::ax615
