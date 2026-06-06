#pragma once

#include "isp/i_isp_service.hpp"
#include <map>
#include <mutex>

namespace aov::media {

namespace ax615 {
class AxIspAdapter;
}

class IspService final : public IIspService {
public:
    IspService() = default;
    ~IspService() override = default;

#if defined(LIBMEDIA_BUILD_AX615_ADAPTERS)
    explicit IspService(ax615::AxIspAdapter* adapter);
    void SetAdapter(ax615::AxIspAdapter* adapter);
#endif

    MediaStatusCode ApplyImageConfig(const IspImageConfig& config) override;
    MediaStatusCode GetImageConfig(int pipe_id, IspImageConfig& config) const override;
    MediaStatusCode ApplyWhiteBalance(const WhiteBalanceConfig& config) override;
    MediaStatusCode ApplyExposure(const ExposureConfig& config) override;
    MediaStatusCode ApplyScene(const SceneConfig& config) override;
    MediaStatusCode ApplyLdc(const LdcConfig& config) override;
    MediaStatusCode ApplySharpen(const SharpenConfig& config) override;
    MediaStatusCode ApplyWbc(const WbcConfig& config) override;
    MediaStatusCode SetMirrorFlip(int pipe_id, bool mirror, bool flip) override;
    MediaStatusCode SetBrightness(int pipe_id, int value) override;
    MediaStatusCode SetContrast(int pipe_id, int value) override;
    MediaStatusCode SetSaturation(int pipe_id, int value) override;
    MediaStatusCode SetHue(int pipe_id, int value) override;

private:
    static bool IsValidPipe(int pipe_id);
    static bool IsPercent(int value);
    static bool IsNonNegative(int value);

    mutable std::mutex mutex_;
    std::map<int, IspImageConfig> image_configs_;

#if defined(LIBMEDIA_BUILD_AX615_ADAPTERS)
    ax615::AxIspAdapter* adapter_ = nullptr;
#endif
};

}  // namespace aov::media
