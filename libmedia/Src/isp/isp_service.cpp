#include "isp_service.hpp"

#if defined(LIBMEDIA_BUILD_AX615_ADAPTERS)
#include "../adapter/ax615/isp/ax_isp_adapter.hpp"
#endif

namespace aov::media {

#if defined(LIBMEDIA_BUILD_AX615_ADAPTERS)
IspService::IspService(ax615::AxIspAdapter* adapter) : adapter_(adapter) {}

void IspService::SetAdapter(ax615::AxIspAdapter* adapter) {
    adapter_ = adapter;
}
#endif

MediaStatusCode IspService::ApplyImageConfig(const IspImageConfig& config) {
    if (!IsPercent(config.brightness) || !IsPercent(config.contrast) || !IsPercent(config.saturation) ||
        !IsPercent(config.hue) || !IsValidPipe(config.pipe_id)) {
        return MediaStatusCode::InvalidArgument;
    }

    MediaStatusCode status = ApplyWhiteBalance(config.white_balance);
    if (status != MediaStatusCode::Ok) {
        return status;
    }
    status = ApplyExposure(config.exposure);
    if (status != MediaStatusCode::Ok) {
        return status;
    }
    status = ApplyScene(config.scene);
    if (status != MediaStatusCode::Ok) {
        return status;
    }
    status = ApplyLdc(config.ldc);
    if (status != MediaStatusCode::Ok) {
        return status;
    }
    status = ApplySharpen(config.sharpen);
    if (status != MediaStatusCode::Ok) {
        return status;
    }
    status = ApplyWbc(config.wbc);
    if (status != MediaStatusCode::Ok) {
        return status;
    }
    {
        std::lock_guard<std::mutex> lock(mutex_);
        image_configs_[config.pipe_id] = config;
    }
    return MediaStatusCode::Ok;
}

MediaStatusCode IspService::GetImageConfig(int pipe_id, IspImageConfig& config) const {
    if (!IsValidPipe(pipe_id)) {
        return MediaStatusCode::InvalidArgument;
    }
    std::lock_guard<std::mutex> lock(mutex_);
    auto it = image_configs_.find(pipe_id);
    if (it == image_configs_.end()) {
        return MediaStatusCode::NotFound;
    }
    config = it->second;
    return MediaStatusCode::Ok;
}

MediaStatusCode IspService::ApplyWhiteBalance(const WhiteBalanceConfig& config) {
    if (!IsValidPipe(config.pipe_id) || !IsNonNegative(config.r_gain) || !IsNonNegative(config.g_gain) ||
        !IsNonNegative(config.b_gain)) {
        return MediaStatusCode::InvalidArgument;
    }
#if defined(LIBMEDIA_BUILD_AX615_ADAPTERS)
    return adapter_ ? adapter_->ApplyWhiteBalance(config) : MediaStatusCode::InvalidState;
#else
    return MediaStatusCode::Ok;
#endif
}

MediaStatusCode IspService::ApplyExposure(const ExposureConfig& config) {
    if (!IsValidPipe(config.pipe_id) || !IsNonNegative(config.max_again) || !IsNonNegative(config.max_dgain) ||
        !IsNonNegative(config.min_shutter_us) || !IsNonNegative(config.max_shutter_us)) {
        return MediaStatusCode::InvalidArgument;
    }
#if defined(LIBMEDIA_BUILD_AX615_ADAPTERS)
    return adapter_ ? adapter_->ApplyExposure(config) : MediaStatusCode::InvalidState;
#else
    return MediaStatusCode::Ok;
#endif
}

MediaStatusCode IspService::ApplyScene(const SceneConfig& config) {
    if (!IsValidPipe(config.pipe_id)) {
        return MediaStatusCode::InvalidArgument;
    }
#if defined(LIBMEDIA_BUILD_AX615_ADAPTERS)
    return adapter_ ? adapter_->ApplyScene(config) : MediaStatusCode::InvalidState;
#else
    return MediaStatusCode::Ok;
#endif
}

MediaStatusCode IspService::ApplyLdc(const LdcConfig& config) {
    if (!IsValidPipe(config.pipe_id) || !IsPercent(config.strength)) {
        return MediaStatusCode::InvalidArgument;
    }
#if defined(LIBMEDIA_BUILD_AX615_ADAPTERS)
    return adapter_ ? adapter_->ApplyLdc(config) : MediaStatusCode::InvalidState;
#else
    return MediaStatusCode::Ok;
#endif
}

MediaStatusCode IspService::ApplySharpen(const SharpenConfig& config) {
    if (!IsValidPipe(config.pipe_id) || !IsPercent(config.strength)) {
        return MediaStatusCode::InvalidArgument;
    }
#if defined(LIBMEDIA_BUILD_AX615_ADAPTERS)
    return adapter_ ? adapter_->ApplySharpen(config) : MediaStatusCode::InvalidState;
#else
    return MediaStatusCode::Ok;
#endif
}

MediaStatusCode IspService::ApplyWbc(const WbcConfig& config) {
    if (!IsValidPipe(config.pipe_id) || !IsNonNegative(config.r_gain) || !IsNonNegative(config.gr_gain) ||
        !IsNonNegative(config.gb_gain) || !IsNonNegative(config.b_gain)) {
        return MediaStatusCode::InvalidArgument;
    }
#if defined(LIBMEDIA_BUILD_AX615_ADAPTERS)
    return adapter_ ? adapter_->ApplyWbc(config) : MediaStatusCode::InvalidState;
#else
    return MediaStatusCode::Ok;
#endif
}

MediaStatusCode IspService::SetMirrorFlip(int pipe_id, bool mirror, bool flip) {
    if (!IsValidPipe(pipe_id)) {
        return MediaStatusCode::InvalidArgument;
    }
    std::lock_guard<std::mutex> lock(mutex_);
    auto& config = image_configs_[pipe_id];
    config.pipe_id = pipe_id;
    config.mirror = mirror;
    config.flip = flip;
    return MediaStatusCode::Unsupported;
}

MediaStatusCode IspService::SetBrightness(int pipe_id, int value) {
    if (!IsValidPipe(pipe_id) || !IsPercent(value)) {
        return MediaStatusCode::InvalidArgument;
    }
    std::lock_guard<std::mutex> lock(mutex_);
    image_configs_[pipe_id].pipe_id = pipe_id;
    image_configs_[pipe_id].brightness = value;
    return MediaStatusCode::Unsupported;
}

MediaStatusCode IspService::SetContrast(int pipe_id, int value) {
    if (!IsValidPipe(pipe_id) || !IsPercent(value)) {
        return MediaStatusCode::InvalidArgument;
    }
    std::lock_guard<std::mutex> lock(mutex_);
    image_configs_[pipe_id].pipe_id = pipe_id;
    image_configs_[pipe_id].contrast = value;
    return MediaStatusCode::Unsupported;
}

MediaStatusCode IspService::SetSaturation(int pipe_id, int value) {
    if (!IsValidPipe(pipe_id) || !IsPercent(value)) {
        return MediaStatusCode::InvalidArgument;
    }
    std::lock_guard<std::mutex> lock(mutex_);
    image_configs_[pipe_id].pipe_id = pipe_id;
    image_configs_[pipe_id].saturation = value;
    return MediaStatusCode::Unsupported;
}

MediaStatusCode IspService::SetHue(int pipe_id, int value) {
    if (!IsValidPipe(pipe_id) || !IsPercent(value)) {
        return MediaStatusCode::InvalidArgument;
    }
    std::lock_guard<std::mutex> lock(mutex_);
    image_configs_[pipe_id].pipe_id = pipe_id;
    image_configs_[pipe_id].hue = value;
    return MediaStatusCode::Unsupported;
}

bool IspService::IsValidPipe(int pipe_id) {
    return pipe_id >= 0;
}

bool IspService::IsPercent(int value) {
    return value >= 0 && value <= 100;
}

bool IspService::IsNonNegative(int value) {
    return value >= 0;
}

}  // namespace aov::media
