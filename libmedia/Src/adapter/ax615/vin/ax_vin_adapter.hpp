#pragma once

#include "sensor_registry.hpp"

#include <string>
#include <vector>

namespace aov::media::ax615 {

struct VinOpenOptions {
    std::string sensor_name;
    AX_INPUT_MODE_E input_mode = AX_INPUT_MODE_MIPI;
    AX_LANE_COMBO_MODE_E lane_combo = AX_LANE_COMBO_MODE_0;
    int chn_frame_mode = 1;  // default 1, matching QSDemo
};

class AxVinAdapter {
public:
    AxVinAdapter() = default;
    ~AxVinAdapter();

    AxVinAdapter(const AxVinAdapter&) = delete;
    AxVinAdapter& operator=(const AxVinAdapter&) = delete;

    bool Open(const VinOpenOptions& options);
    bool Open(const SensorProfile& profile,
              AX_INPUT_MODE_E input_mode = AX_INPUT_MODE_MIPI,
              AX_LANE_COMBO_MODE_E lane_combo = AX_LANE_COMBO_MODE_0);
    bool Enable();      // EnableChn + StartPipe (不包含 EnableDev)
    bool StartDev();    // EnableDev (必须在 ISP Start 之后调用)
    void Disable();
    void Close();

    bool IsOpen() const { return open_; }
    bool IsEnabled() const { return enabled_; }

private:
    bool StartMipi(const SensorProfile& profile, AX_INPUT_MODE_E input_mode, AX_LANE_COMBO_MODE_E lane_combo);
    void StopMipi();
    bool CreateDevice(const SensorProfile& profile);
    bool CreatePipeAndChannels(const SensorProfile& profile);
    void DestroyPipeAndChannels();
    void DestroyDevice();

    SensorProfile profile_{};
    int chn_frame_mode_ = 1;
    bool open_ = false;
    bool enabled_ = false;
    bool vin_initialized_ = false;
    bool mipi_initialized_ = false;
    bool mipi_started_ = false;
    bool dev_created_ = false;
    bool dev_enabled_ = false;
    bool pipe_created_ = false;
    bool pipe_started_ = false;
    std::vector<AX_VIN_CHN_ID_E> enabled_channels_;
};

}  // namespace aov::media::ax615
