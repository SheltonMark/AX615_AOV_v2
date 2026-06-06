#pragma once

#include "common/media_config.hpp"
#include "common/media_status.hpp"

#include "ax_isp_api.h"
#include "ax_sensor_struct.h"

#include <cstdint>
#include <string>

namespace aov::media::ax615 {

struct IspConfig {
    AX_U8 pipe_id = 0;
    AX_U8 dev_id = 0;
    AX_U8 sns_clk_idx = 0;
    AX_SNS_CLK_RATE_E sns_clk_rate = AX_SNS_CLK_NOT_CFG;
    std::string iq_bin_path;
    AX_SENSOR_REGISTER_FUNC_T* sensor_handle = nullptr;
    bool register_sensor = false;
    bool register_3a = true;
};

class AxIspAdapter {
public:
    AxIspAdapter() = default;
    ~AxIspAdapter();

    AxIspAdapter(const AxIspAdapter&) = delete;
    AxIspAdapter& operator=(const AxIspAdapter&) = delete;

    bool Open(const IspConfig& cfg);
    bool Start();
    void Stop();
    void Close();

    MediaStatusCode ApplyWhiteBalance(const WhiteBalanceConfig& config);
    MediaStatusCode ApplyExposure(const ExposureConfig& config);
    MediaStatusCode ApplyScene(const SceneConfig& config);
    MediaStatusCode ApplyLdc(const LdcConfig& config);
    MediaStatusCode ApplySharpen(const SharpenConfig& config);
    MediaStatusCode ApplyWbc(const WbcConfig& config);

    bool IsOpen() const { return opened_; }
    bool IsRunning() const { return running_; }

private:
    bool RegisterSensor();
    void UnregisterSensor();
    bool Register3A();
    void Unregister3A();
    bool LoadIqBin();

    IspConfig cfg_{};
    bool created_ = false;
    bool opened_ = false;
    bool running_ = false;
    bool sensor_registered_ = false;
    bool ae_registered_ = false;
    bool awb_registered_ = false;
    bool sns_clk_opened_ = false;
};

}  // namespace aov::media::ax615
