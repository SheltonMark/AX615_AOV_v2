#include "ax_isp_adapter.hpp"

#include "ax_isp_3a_api.h"
#include "ax_autokit_iq_api.h"
#include "ax_isp_iq_api.h"
#include <cstdio>

namespace aov::media::ax615 {
namespace {

AX_U16 ToAxGain(int value) {
    return static_cast<AX_U16>(value < 0 ? 0 : (value > 0xFFF ? 0xFFF : value));
}

MediaStatusCode FromAxRet(const char* api, AX_S32 ret) {
    if (ret == AX_SUCCESS) {
        return MediaStatusCode::Ok;
    }
    std::fprintf(stderr, "[AxIspAdapter] %s failed: 0x%x\n", api, ret);
    return MediaStatusCode::InternalError;
}

}  // namespace

AxIspAdapter::~AxIspAdapter() {
    Close();
}

bool AxIspAdapter::Open(const IspConfig& cfg) {
    if (opened_) {
        return true;
    }

    cfg_ = cfg;

    if (cfg_.sns_clk_rate != AX_SNS_CLK_NOT_CFG) {
        AX_S32 ret = AX_ISP_OpenSnsClk(cfg_.sns_clk_idx, cfg_.sns_clk_rate);
        if (ret != AX_SUCCESS) {
            std::fprintf(stderr, "[AxIspAdapter] AX_ISP_OpenSnsClk failed: 0x%x\n", ret);
            return false;
        }
        sns_clk_opened_ = true;
    }

    AX_S32 ret = AX_ISP_Create(cfg_.pipe_id);
    if (ret != AX_SUCCESS) {
        std::fprintf(stderr, "[AxIspAdapter] AX_ISP_Create failed: 0x%x\n", ret);
        Close();
        return false;
    }
    created_ = true;

    if (cfg_.register_sensor && !RegisterSensor()) {
        Close();
        return false;
    }

    if (cfg_.register_3a && cfg_.sensor_handle && !Register3A()) {
        Close();
        return false;
    }

    LoadIqBin();

    ret = AX_ISP_Open(cfg_.pipe_id);
    if (ret != AX_SUCCESS) {
        std::fprintf(stderr, "[AxIspAdapter] AX_ISP_Open failed: 0x%x\n", ret);
        Close();
        return false;
    }
    opened_ = true;
    return true;
}

bool AxIspAdapter::Start() {
    if (!opened_) {
        return false;
    }
    if (running_) {
        return true;
    }

    AX_S32 ret = AX_ISP_Start(cfg_.pipe_id);
    if (ret != AX_SUCCESS) {
        std::fprintf(stderr, "[AxIspAdapter] AX_ISP_Start failed: 0x%x\n", ret);
        return false;
    }
    running_ = true;
    return true;
}

void AxIspAdapter::Stop() {
    if (!running_) {
        return;
    }
    AX_ISP_Stop(cfg_.pipe_id);
    running_ = false;
}

void AxIspAdapter::Close() {
    Stop();

    if (opened_) {
        AX_ISP_Close(cfg_.pipe_id);
        opened_ = false;
    }

    Unregister3A();
    UnregisterSensor();

    if (created_) {
        AX_ISP_Destroy(cfg_.pipe_id);
        created_ = false;
    }

    if (sns_clk_opened_) {
        AX_ISP_CloseSnsClk(cfg_.sns_clk_idx);
        sns_clk_opened_ = false;
    }

    cfg_ = {};
}

MediaStatusCode AxIspAdapter::ApplyWhiteBalance(const WhiteBalanceConfig& config) {
    AX_ISP_IQ_AWB_PARAM_T param = {};
    AX_S32 ret = AX_ISP_IQ_GetAwbParam(static_cast<AX_U8>(config.pipe_id), &param);
    if (ret != AX_SUCCESS) {
        return FromAxRet("AX_ISP_IQ_GetAwbParam", ret);
    }

    const bool awb_auto_mode = config.mode == ImageAutoMode::Auto;
    param.nEnable = awb_auto_mode ? AX_TRUE : AX_FALSE;
    if (!awb_auto_mode) {
        param.tManualParam.nManualMode = 0;
        if (config.r_gain > 0) {
            param.tManualParam.tGain.nGainR = ToAxGain(config.r_gain);
        }
        if (config.g_gain > 0) {
            const AX_U16 gain = ToAxGain(config.g_gain);
            param.tManualParam.tGain.nGainGr = gain;
            param.tManualParam.tGain.nGainGb = gain;
        }
        if (config.b_gain > 0) {
            param.tManualParam.tGain.nGainB = ToAxGain(config.b_gain);
        }
    }

    ret = AX_ISP_IQ_SetAwbParam(static_cast<AX_U8>(config.pipe_id), &param);
    return FromAxRet("AX_ISP_IQ_SetAwbParam", ret);
}

MediaStatusCode AxIspAdapter::ApplyExposure(const ExposureConfig& config) {
    AX_ISP_IQ_AE_PARAM_T param = {};
    AX_S32 ret = AX_ISP_IQ_GetAeParam(static_cast<AX_U8>(config.pipe_id), &param);
    if (ret != AX_SUCCESS) {
        return FromAxRet("AX_ISP_IQ_GetAeParam", ret);
    }

    param.nEnable = config.mode == ImageAutoMode::Auto ? AX_TRUE : AX_FALSE;
    param.tAeAlgAuto.tSlowShutterParam.nFrameRateMode =
        static_cast<AX_U8>(config.slow_shutter_enabled ? 1 : 0);
    if (config.max_again > 0) {
        param.tAeAlgAuto.nMaxUserTotalAgain = static_cast<AX_U32>(config.max_again);
    }
    if (config.max_dgain > 0) {
        param.tAeAlgAuto.nMaxUserDgain = static_cast<AX_U32>(config.max_dgain);
    }
    // TODO: map compensation once the product-side unit is defined.

    ret = AX_ISP_IQ_SetAeParam(static_cast<AX_U8>(config.pipe_id), &param);
    return FromAxRet("AX_ISP_IQ_SetAeParam", ret);
}

MediaStatusCode AxIspAdapter::ApplyScene(const SceneConfig& config) {
    AX_ISP_IQ_SCENE_PARAM_EX_T param = {};
    AX_S32 ret = AX_ISP_IQ_GetSceneParamEx(static_cast<AX_U8>(config.pipe_id), &param);
    if (ret != AX_SUCCESS) {
        return FromAxRet("AX_ISP_IQ_GetSceneParamEx", ret);
    }

    if (config.day_night_mode == DayNightMode::Auto) {
        param.nAutoMode = 1;
    } else {
        param.nAutoMode = 0;
        param.tManualParam.nAiWorkMode =
            static_cast<AX_U8>(config.day_night_mode == DayNightMode::Day ? AX_AINR_ENABLE : AX_AINR_DISABLE);
    }
    // TODO: ir_cut_mode and anti_flicker are board/policy controls, not direct SceneParamEx fields.

    ret = AX_ISP_IQ_SetSceneParamEx(static_cast<AX_U8>(config.pipe_id), &param);
    return FromAxRet("AX_ISP_IQ_SetSceneParamEx", ret);
}

MediaStatusCode AxIspAdapter::ApplyLdc(const LdcConfig& config) {
    AX_ISP_IQ_LDC_PARAM_T param = {};
    AX_S32 ret = AX_ISP_IQ_GetLdcParam(static_cast<AX_U8>(config.pipe_id), &param);
    if (ret != AX_SUCCESS) {
        return FromAxRet("AX_ISP_IQ_GetLdcParam", ret);
    }

    param.nLdcEnable = config.enabled ? AX_TRUE : AX_FALSE;
    param.nType = AX_ISP_IQ_LDC_TYPE_V1;
    param.tLdcV1Param.bAspect = AX_TRUE;
    param.tLdcV1Param.nXRatio = 0;
    param.tLdcV1Param.nYRatio = 0;
    param.tLdcV1Param.nXYRatio = static_cast<AX_S16>(config.strength);
    param.tLdcV1Param.nCenterXOffset = 0;
    param.tLdcV1Param.nCenterYOffset = 0;
    param.tLdcV1Param.nDistortionRatio = static_cast<AX_S16>(config.strength * 100);
    param.tLdcV1Param.nSpreadCoef = 0;
    param.tLdcV2Param.nMatrix[0][0] = 1;
    param.tLdcV2Param.nMatrix[1][1] = 1;
    param.tLdcV2Param.nMatrix[2][2] = 1;

    ret = AX_ISP_IQ_SetLdcParam(static_cast<AX_U8>(config.pipe_id), &param);
    return FromAxRet("AX_ISP_IQ_SetLdcParam", ret);
}

MediaStatusCode AxIspAdapter::ApplySharpen(const SharpenConfig& config) {
    AX_ISP_IQ_SHARPEN_PARAM_T param = {};
    AX_S32 ret = AX_ISP_IQ_GetSharpenParam(static_cast<AX_U8>(config.pipe_id), &param);
    if (ret != AX_SUCCESS) {
        return FromAxRet("AX_ISP_IQ_GetSharpenParam", ret);
    }

    param.nSharpenEn = config.strength > 0 ? AX_TRUE : AX_FALSE;
    // TODO: map product strength to sharpen LUT fields after IQ tuning policy is defined.

    ret = AX_ISP_IQ_SetSharpenParam(static_cast<AX_U8>(config.pipe_id), &param);
    return FromAxRet("AX_ISP_IQ_SetSharpenParam", ret);
}

MediaStatusCode AxIspAdapter::ApplyWbc(const WbcConfig& config) {
    AX_ISP_IQ_WBC_PARAM_T param = {};
    AX_S32 ret = AX_ISP_IQ_GetWbcParam(static_cast<AX_U8>(config.pipe_id), &param);
    if (ret != AX_SUCCESS) {
        return FromAxRet("AX_ISP_IQ_GetWbcParam", ret);
    }

    param.nWbcEn = config.enabled ? AX_TRUE : AX_FALSE;
    if (config.r_gain > 0) {
        param.tManualParam.nRGain = ToAxGain(config.r_gain);
    }
    if (config.gr_gain > 0) {
        param.tManualParam.nGrGain = ToAxGain(config.gr_gain);
    }
    if (config.gb_gain > 0) {
        param.tManualParam.nGbGain = ToAxGain(config.gb_gain);
    }
    if (config.b_gain > 0) {
        param.tManualParam.nBGain = ToAxGain(config.b_gain);
    }

    ret = AX_ISP_IQ_SetWbcParam(static_cast<AX_U8>(config.pipe_id), &param);
    return FromAxRet("AX_ISP_IQ_SetWbcParam", ret);
}

bool AxIspAdapter::RegisterSensor() {
    if (!cfg_.sensor_handle) {
        return false;
    }

    AX_S32 ret = AX_ISP_RegisterSensor(cfg_.pipe_id, cfg_.sensor_handle);
    if (ret != AX_SUCCESS) {
        std::fprintf(stderr, "[AxIspAdapter] AX_ISP_RegisterSensor failed: 0x%x\n", ret);
        return false;
    }

    AX_SNS_COMMBUS_T bus_info;
    bus_info.busType = ISP_SNS_CONNECT_I2C_TYPE;
    bus_info.I2cDev = cfg_.dev_id == 0 ? 1 : 0;
    bus_info.nPwdnGpio = cfg_.dev_id == 0 ? 72 : 59;
    if (cfg_.sensor_handle->pfn_sensor_set_bus_info) {
        ret = cfg_.sensor_handle->pfn_sensor_set_bus_info(cfg_.pipe_id, bus_info);
        if (ret != AX_SUCCESS) {
            std::fprintf(stderr, "[AxIspAdapter] sensor_set_bus_info failed: 0x%x\n", ret);
            AX_ISP_UnRegisterSensor(cfg_.pipe_id);
            return false;
        }
    }

    sensor_registered_ = true;
    return true;
}

void AxIspAdapter::UnregisterSensor() {
    if (sensor_registered_) {
        AX_ISP_UnRegisterSensor(cfg_.pipe_id);
        sensor_registered_ = false;
    }
}

bool AxIspAdapter::Register3A() {
    AX_ISP_AE_REGFUNCS_T ae_funcs;
    ae_funcs.pfnAe_Create = AX_ISP_ALG_AeCreate;
    ae_funcs.pfnAe_Destroy = AX_ISP_ALG_AeDestroy;
    ae_funcs.pfnAe_Init = AX_ISP_ALG_AeInit;
    ae_funcs.pfnAe_Run = AX_ISP_ALG_AeRun;
    ae_funcs.pfnAe_Exit = AX_ISP_ALG_AeDeInit;
    ae_funcs.pfnAe_Ctrl = AX_ISP_ALG_AeCtrl;

    AX_S32 ret = AX_ISP_ALG_AeRegisterSensor(cfg_.pipe_id, cfg_.sensor_handle);
    if (ret != AX_SUCCESS) {
        std::fprintf(stderr, "[AxIspAdapter] AX_ISP_ALG_AeRegisterSensor failed: 0x%x\n", ret);
        return false;
    }

    ret = AX_ISP_RegisterAeLibCallback(cfg_.pipe_id, &ae_funcs);
    if (ret != AX_SUCCESS) {
        std::fprintf(stderr, "[AxIspAdapter] AX_ISP_RegisterAeLibCallback failed: 0x%x\n", ret);
        AX_ISP_ALG_AeUnRegisterSensor(cfg_.pipe_id);
        return false;
    }
    ae_registered_ = true;

    AX_ISP_AWB_REGFUNCS_T awb_funcs;
    awb_funcs.pfnAwb_Create = AX_ISP_ALG_AwbCreate;
    awb_funcs.pfnAwb_Destroy = AX_ISP_ALG_AwbDestroy;
    awb_funcs.pfnAwb_Init = AX_ISP_ALG_AwbInit;
    awb_funcs.pfnAwb_Run = AX_ISP_ALG_AwbRun;
    awb_funcs.pfnAwb_Exit = AX_ISP_ALG_AwbDeInit;
    awb_funcs.pfnAwb_Ctrl = AX_ISP_ALG_AwbCtrl;

    ret = AX_ISP_ALG_AwbRegisterSensor(cfg_.pipe_id, cfg_.sensor_handle);
    if (ret != AX_SUCCESS) {
        std::fprintf(stderr, "[AxIspAdapter] AX_ISP_ALG_AwbRegisterSensor failed: 0x%x\n", ret);
        return false;
    }

    ret = AX_ISP_RegisterAwbLibCallback(cfg_.pipe_id, &awb_funcs);
    if (ret != AX_SUCCESS) {
        std::fprintf(stderr, "[AxIspAdapter] AX_ISP_RegisterAwbLibCallback failed: 0x%x\n", ret);
        AX_ISP_ALG_AwbUnRegisterSensor(cfg_.pipe_id);
        return false;
    }
    awb_registered_ = true;
    return true;
}

void AxIspAdapter::Unregister3A() {
    if (awb_registered_) {
        AX_ISP_UnRegisterAwbLibCallback(cfg_.pipe_id);
        AX_ISP_ALG_AwbUnRegisterSensor(cfg_.pipe_id);
        awb_registered_ = false;
    }
    if (ae_registered_) {
        AX_ISP_UnRegisterAeLibCallback(cfg_.pipe_id);
        AX_ISP_ALG_AeUnRegisterSensor(cfg_.pipe_id);
        ae_registered_ = false;
    }
}

bool AxIspAdapter::LoadIqBin() {
    if (cfg_.iq_bin_path.empty() || cfg_.iq_bin_path == "null.bin") {
        return true;
    }

    AX_S32 ret = AX_ISP_LoadBinParams(cfg_.pipe_id, cfg_.iq_bin_path.c_str());
    if (ret != AX_SUCCESS) {
        std::fprintf(stderr, "[AxIspAdapter] AX_ISP_LoadBinParams warning: 0x%x path=%s\n",
                     ret, cfg_.iq_bin_path.c_str());
        return false;
    }
    return true;
}

}  // namespace aov::media::ax615
