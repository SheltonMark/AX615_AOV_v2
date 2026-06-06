#include "ax_vin_adapter.hpp"

#include <cstdio>
#include <cstring>

namespace aov::media::ax615 {

AxVinAdapter::~AxVinAdapter() {
    Close();
}

bool AxVinAdapter::Open(const VinOpenOptions& options) {
    const SensorProfile* profile = SensorRegistry::Instance().Find(options.sensor_name);
    if (!profile) {
        std::fprintf(stderr, "[AxVinAdapter] sensor not registered: %s\n", options.sensor_name.c_str());
        return false;
    }
    return Open(*profile, options.input_mode, options.lane_combo);
}

bool AxVinAdapter::Open(const SensorProfile& profile,
                        AX_INPUT_MODE_E input_mode,
                        AX_LANE_COMBO_MODE_E lane_combo) {
    if (open_) {
        return true;
    }
    if (profile.sensor_name.empty() || profile.chn_attrs.empty()) {
        return false;
    }

    AX_S32 ret = AX_VIN_Init();
    if (ret != AX_SUCCESS) {
        std::fprintf(stderr, "[AxVinAdapter] AX_VIN_Init failed: 0x%x\n", ret);
        return false;
    }
    vin_initialized_ = true;
    profile_ = profile;

    if (!StartMipi(profile_, input_mode, lane_combo)) {
        Close();
        return false;
    }
    if (!CreateDevice(profile_)) {
        Close();
        return false;
    }
    if (!CreatePipeAndChannels(profile_)) {
        Close();
        return false;
    }

    open_ = true;
    return true;
}

bool AxVinAdapter::Enable() {
    if (!open_ || enabled_) {
        return open_;
    }

    AX_S32 ret = AX_VIN_EnableDev(profile_.dev_id);
    if (ret != AX_SUCCESS) {
        std::fprintf(stderr, "[AxVinAdapter] AX_VIN_EnableDev failed: 0x%x\n", ret);
        return false;
    }
    dev_enabled_ = true;
    enabled_ = true;
    return true;
}

void AxVinAdapter::Disable() {
    if (!enabled_) {
        return;
    }
    if (dev_enabled_) {
        AX_VIN_DisableDev(profile_.dev_id);
        dev_enabled_ = false;
    }
    enabled_ = false;
}

void AxVinAdapter::Close() {
    Disable();
    DestroyPipeAndChannels();
    DestroyDevice();
    StopMipi();

    if (vin_initialized_) {
        AX_VIN_Deinit();
        vin_initialized_ = false;
    }

    open_ = false;
    profile_ = {};
}

bool AxVinAdapter::StartMipi(const SensorProfile& profile,
                             AX_INPUT_MODE_E input_mode,
                             AX_LANE_COMBO_MODE_E lane_combo) {
    AX_MIPI_RX_DEV_T mipi_dev;
    std::memset(&mipi_dev, 0, sizeof(mipi_dev));
    mipi_dev.eInputMode = input_mode;
    mipi_dev.tMipiAttr = profile.mipi_attr;

    AX_MIPI_RX_SetLaneCombo(lane_combo);

    AX_S32 ret = AX_MIPI_RX_SetAttr(profile.rx_dev, &mipi_dev);
    if (ret != AX_SUCCESS) {
        std::fprintf(stderr, "[AxVinAdapter] AX_MIPI_RX_SetAttr failed: 0x%x\n", ret);
        return false;
    }

    ret = AX_MIPI_RX_Reset(profile.rx_dev);
    if (ret != AX_SUCCESS) {
        std::fprintf(stderr, "[AxVinAdapter] AX_MIPI_RX_Reset failed: 0x%x\n", ret);
        return false;
    }

    ret = AX_MIPI_RX_Start(profile.rx_dev);
    if (ret != AX_SUCCESS) {
        std::fprintf(stderr, "[AxVinAdapter] AX_MIPI_RX_Start failed: 0x%x\n", ret);
        return false;
    }

    mipi_started_ = true;
    return true;
}

void AxVinAdapter::StopMipi() {
    if (mipi_started_) {
        AX_MIPI_RX_Stop(profile_.rx_dev);
        mipi_started_ = false;
    }
}

bool AxVinAdapter::CreateDevice(const SensorProfile& profile) {
    AX_VIN_DEV_ATTR_T dev_attr = profile.dev_attr;
    AX_S32 ret = AX_VIN_CreateDev(profile.dev_id, &dev_attr);
    if (ret != AX_SUCCESS) {
        std::fprintf(stderr, "[AxVinAdapter] AX_VIN_CreateDev failed: 0x%x\n", ret);
        return false;
    }
    dev_created_ = true;

    ret = AX_VIN_SetDevAttr(profile.dev_id, &dev_attr);
    if (ret != AX_SUCCESS) {
        std::fprintf(stderr, "[AxVinAdapter] AX_VIN_SetDevAttr failed: 0x%x\n", ret);
        return false;
    }
    return true;
}

void AxVinAdapter::DestroyDevice() {
    if (dev_created_) {
        AX_VIN_DestroyDev(profile_.dev_id);
        dev_created_ = false;
    }
}

bool AxVinAdapter::CreatePipeAndChannels(const SensorProfile& profile) {
    AX_VIN_PIPE_ATTR_T pipe_attr = profile.pipe_attr;
    AX_S32 ret = AX_VIN_CreatePipe(profile.pipe_id, &pipe_attr);
    if (ret != AX_SUCCESS) {
        std::fprintf(stderr, "[AxVinAdapter] AX_VIN_CreatePipe failed: 0x%x\n", ret);
        return false;
    }
    pipe_created_ = true;

    ret = AX_VIN_SetPipeAttr(profile.pipe_id, &pipe_attr);
    if (ret != AX_SUCCESS) {
        std::fprintf(stderr, "[AxVinAdapter] AX_VIN_SetPipeAttr failed: 0x%x\n", ret);
        return false;
    }

    for (size_t i = 0; i < profile.chn_attrs.size(); ++i) {
        if (i >= AX_VIN_CHN_ID_MAX) {
            return false;
        }
        AX_VIN_CHN_ID_E chn = static_cast<AX_VIN_CHN_ID_E>(i);
        AX_VIN_CHN_ATTR_T chn_attr = profile.chn_attrs[i];
        ret = AX_VIN_SetChnAttr(profile.pipe_id, chn, &chn_attr);
        if (ret != AX_SUCCESS) {
            std::fprintf(stderr, "[AxVinAdapter] AX_VIN_SetChnAttr[%zu] failed: 0x%x\n", i, ret);
            return false;
        }

        ret = AX_VIN_EnableChn(profile.pipe_id, chn);
        if (ret != AX_SUCCESS) {
            std::fprintf(stderr, "[AxVinAdapter] AX_VIN_EnableChn[%zu] failed: 0x%x\n", i, ret);
            return false;
        }
        enabled_channels_.push_back(chn);
    }
    return true;
}

void AxVinAdapter::DestroyPipeAndChannels() {
    for (auto it = enabled_channels_.rbegin(); it != enabled_channels_.rend(); ++it) {
        AX_VIN_DisableChn(profile_.pipe_id, *it);
    }
    enabled_channels_.clear();

    if (pipe_created_) {
        AX_VIN_DestroyPipe(profile_.pipe_id);
        pipe_created_ = false;
    }
}

}  // namespace aov::media::ax615
