#pragma once

#include "common/media_config.hpp"
#include "common/media_status.hpp"

namespace aov::media {

class IIspService {
public:
    virtual ~IIspService() = default;

    virtual MediaStatusCode ApplyImageConfig(const IspImageConfig& config) = 0;
    virtual MediaStatusCode GetImageConfig(int pipe_id, IspImageConfig& config) const = 0;
    virtual MediaStatusCode ApplyWhiteBalance(const WhiteBalanceConfig& config) = 0;
    virtual MediaStatusCode ApplyExposure(const ExposureConfig& config) = 0;
    virtual MediaStatusCode ApplyScene(const SceneConfig& config) = 0;
    virtual MediaStatusCode ApplyLdc(const LdcConfig& config) = 0;
    virtual MediaStatusCode ApplySharpen(const SharpenConfig& config) = 0;
    virtual MediaStatusCode ApplyWbc(const WbcConfig& config) = 0;
    virtual MediaStatusCode SetMirrorFlip(int pipe_id, bool mirror, bool flip) = 0;
    virtual MediaStatusCode SetBrightness(int pipe_id, int value) = 0;
    virtual MediaStatusCode SetContrast(int pipe_id, int value) = 0;
    virtual MediaStatusCode SetSaturation(int pipe_id, int value) = 0;
    virtual MediaStatusCode SetHue(int pipe_id, int value) = 0;
};

}  // namespace aov::media
