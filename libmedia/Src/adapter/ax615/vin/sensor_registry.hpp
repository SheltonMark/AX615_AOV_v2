#pragma once

#include "ax_mipi_rx_api.h"
#include "ax_sensor_struct.h"
#include "ax_vin_api.h"

#include <string>
#include <unordered_map>
#include <vector>

namespace aov::media::ax615 {

struct SensorProfile {
    std::string sensor_name;

    // Sensor driver symbols. Switching sensor should only require adding a new profile.
    std::string driver_lib_name;
    std::string driver_obj_name;
    std::string quick_start_obj_name;
    std::string default_iq_path;

    AX_U8 dev_id = 0;
    AX_U8 pipe_id = 0;
    AX_U8 rx_dev = 0;
    AX_U8 sns_clk_idx = 0;
    AX_SNS_CLK_RATE_E sns_clk_rate = AX_SNS_CLK_NOT_CFG;

    AX_MIPI_RX_ATTR_T mipi_attr{};
    AX_SNS_ATTR_T sns_attr{};
    AX_VIN_DEV_ATTR_T dev_attr{};
    AX_VIN_PIPE_ATTR_T pipe_attr{};
    std::vector<AX_VIN_CHN_ATTR_T> chn_attrs;
};

class SensorRegistry {
public:
    SensorRegistry() = default;

    static SensorRegistry& Instance();

    bool Register(const SensorProfile& profile);
    const SensorProfile* Find(const std::string& name) const;
    bool Has(const std::string& name) const;
    std::vector<std::string> ListSensorNames() const;

private:
    std::unordered_map<std::string, SensorProfile> profiles_;
};

}  // namespace aov::media::ax615
