#include "ax_aov_power_adapter.hpp"

#include <cstring>

namespace aov::media::ax615 {
namespace {

bool IsValidConfig(const AovPowerConfig& cfg)
{
    return !cfg.pipes.empty() && cfg.normal_fps > 0.0F && cfg.aov_venc_fps > 0.0F;
}

AX_U32 DefaultBitrateKbps(VideoCodec codec, AX_U32 width, AX_U32 height)
{
    const AX_U32 pixels = width * height;
    if (codec == VideoCodec::H265) {
        if (pixels >= 3840U * 2160U) {
            return 4096;
        }
        if (pixels >= 2688U * 1520U) {
            return 2048;
        }
        return 1024;
    }

    if (pixels >= 3840U * 2160U) {
        return 8192;
    }
    if (pixels >= 2688U * 1520U) {
        return 4096;
    }
    return 2048;
}

void SetRcGop(AX_VENC_RC_PARAM_T& rc, AX_U32 gop)
{
    switch (rc.enRcMode) {
        case AX_VENC_RC_MODE_H264CBR:
            rc.stH264Cbr.u32Gop = gop;
            break;
        case AX_VENC_RC_MODE_H264VBR:
            rc.stH264Vbr.u32Gop = gop;
            break;
        case AX_VENC_RC_MODE_H264FIXQP:
            rc.stH264FixQp.u32Gop = gop;
            break;
        case AX_VENC_RC_MODE_H265CBR:
            rc.stH265Cbr.u32Gop = gop;
            break;
        case AX_VENC_RC_MODE_H265VBR:
            rc.stH265Vbr.u32Gop = gop;
            break;
        case AX_VENC_RC_MODE_H265FIXQP:
            rc.stH265FixQp.u32Gop = gop;
            break;
        default:
            break;
    }
}

void SetRcBitrate(AX_VENC_RC_PARAM_T& rc, AX_U32 bitrate)
{
    switch (rc.enRcMode) {
        case AX_VENC_RC_MODE_H264CBR:
            rc.stH264Cbr.u32BitRate = bitrate;
            break;
        case AX_VENC_RC_MODE_H264VBR:
            rc.stH264Vbr.u32MaxBitRate = bitrate;
            break;
        case AX_VENC_RC_MODE_H265CBR:
            rc.stH265Cbr.u32BitRate = bitrate;
            break;
        case AX_VENC_RC_MODE_H265VBR:
            rc.stH265Vbr.u32MaxBitRate = bitrate;
            break;
        default:
            break;
    }
}

}  // namespace

MediaStatusCode AxAovPowerAdapter::ApplyLowPower(const AovPowerConfig& cfg)
{
    if (!IsValidConfig(cfg)) {
        return MediaStatusCode::InvalidArgument;
    }

    MediaStatusCode status = UpdatePipeFps(cfg, true);
    if (status != MediaStatusCode::Ok) {
        return status;
    }

    status = UpdateTcmSensorInfo(cfg, true);
    if (status != MediaStatusCode::Ok) {
        return status;
    }

    status = UpdateVencRc(cfg, true);
    if (status != MediaStatusCode::Ok) {
        return status;
    }

    // 参考 QSDemo AOV 分析文档 (docs/QSDemo/QSDemo_AOV_Analysis.md 第 7.1 节)
    // 显式唤醒 sensor，让它吐出 cap_frame_num 帧用于检测
    status = WakeupTcmSensors(cfg.pipes);
    if (status != MediaStatusCode::Ok) {
        return status;
    }

    // 设置捕获帧数，确保 sensor 吐出指定数量的帧供 SKEL 检测
    for (const auto& pipe : cfg.pipes) {
        if (pipe.enable_tcm) {
            const AX_U32 cap_frames = cfg.tcm_cap_frame_num == 0 ? 1 : cfg.tcm_cap_frame_num;
            AX_S32 ret = AX_VIN_SetCapFrameNumbers(pipe.pipe_id, cap_frames);
            if (ret != AX_SUCCESS) {
                return MediaStatusCode::InternalError;
            }
        }
    }

    low_power_ = true;
    return MediaStatusCode::Ok;
}

MediaStatusCode AxAovPowerAdapter::RestoreNormal(const AovPowerConfig& cfg)
{
    if (!IsValidConfig(cfg)) {
        return MediaStatusCode::InvalidArgument;
    }

    MediaStatusCode status = UpdateTcmSensorInfo(cfg, false);
    if (status != MediaStatusCode::Ok) {
        return status;
    }

    status = WakeupTcmSensors(cfg.pipes);
    if (status != MediaStatusCode::Ok) {
        return status;
    }

    status = UpdatePipeFps(cfg, false);
    if (status != MediaStatusCode::Ok) {
        return status;
    }

    status = UpdateVencRc(cfg, false);
    if (status != MediaStatusCode::Ok) {
        return status;
    }

    low_power_ = false;
    return MediaStatusCode::Ok;
}

MediaStatusCode AxAovPowerAdapter::WakeupSensors(const std::vector<AovPipeConfig>& pipes)
{
    if (pipes.empty()) {
        return MediaStatusCode::InvalidArgument;
    }
    return WakeupTcmSensors(pipes);
}

MediaStatusCode AxAovPowerAdapter::UpdatePipeFps(const AovPowerConfig& cfg, bool low_power)
{
    for (const auto& pipe : cfg.pipes) {
        AX_VIN_PIPE_ATTR_T pipe_attr;
        std::memset(&pipe_attr, 0, sizeof(pipe_attr));

        AX_S32 ret = AX_VIN_GetPipeAttr(pipe.pipe_id, &pipe_attr);
        if (ret != AX_SUCCESS) {
            return MediaStatusCode::InternalError;
        }

        pipe_attr.tFrameRateCtrl.fDstFrameRate = low_power ? cfg.aov_pipe_dst_fps : cfg.normal_fps;
        pipe_attr.tFrameRateCtrl.fSrcFrameRate = low_power ? cfg.aov_pipe_src_fps : cfg.normal_fps;

        ret = AX_VIN_SetPipeAttr(pipe.pipe_id, &pipe_attr);
        if (ret != AX_SUCCESS) {
            return MediaStatusCode::InternalError;
        }
    }

    return MediaStatusCode::Ok;
}

MediaStatusCode AxAovPowerAdapter::UpdateVencRc(const AovPowerConfig& cfg, bool low_power)
{
    for (const auto& ch : cfg.venc_channels) {
        AX_VENC_RC_PARAM_T rc_param;
        std::memset(&rc_param, 0, sizeof(rc_param));

        AX_S32 ret = AX_VENC_GetRcParam(ch.chn_id, &rc_param);
        if (ret != AX_SUCCESS) {
            return MediaStatusCode::InternalError;
        }

        rc_param.stFrameRate.fSrcFrameRate = low_power ? cfg.aov_venc_fps : cfg.normal_fps;
        rc_param.stFrameRate.fDstFrameRate = low_power ? cfg.aov_venc_fps : cfg.normal_fps;

        const AX_U32 base_bitrate = DefaultBitrateKbps(ch.codec, ch.width, ch.height);
        const AX_U32 bitrate = low_power
            ? static_cast<AX_U32>(base_bitrate * cfg.aov_bitrate_ratio)
            : base_bitrate;
        SetRcBitrate(rc_param, bitrate);
        SetRcGop(rc_param, low_power ? cfg.aov_gop : cfg.normal_gop);

        ret = AX_VENC_SetRcParam(ch.chn_id, &rc_param);
        if (ret != AX_SUCCESS) {
            return MediaStatusCode::InternalError;
        }

        if (!low_power) {
            ret = AX_VENC_RequestIDR(ch.chn_id, AX_TRUE);
            if (ret != AX_SUCCESS) {
                return MediaStatusCode::InternalError;
            }
        }
    }

    return MediaStatusCode::Ok;
}

MediaStatusCode AxAovPowerAdapter::UpdateTcmSensorInfo(const AovPowerConfig& cfg, bool low_power)
{
    for (const auto& pipe : cfg.pipes) {
        if (!pipe.enable_tcm) {
            continue;
        }

        ax_tcm_sensor_info_t sensor_info;
        std::memset(&sensor_info, 0, sizeof(sensor_info));

        int ret = ax_tcm_sns_info_get(pipe.pipe_id, &sensor_info);
        if (ret != 0) {
            return MediaStatusCode::InternalError;
        }

        sensor_info.work_mode = low_power ? AX_TCM_SNS_WORKMODE_LOW_POWER : AX_TCM_SNS_WORKMODE_NORMAL;
        sensor_info.cap_frame_num = cfg.tcm_cap_frame_num == 0 ? 1 : cfg.tcm_cap_frame_num;

        ret = ax_tcm_sns_info_set(pipe.pipe_id, &sensor_info);
        if (ret != 0) {
            return MediaStatusCode::InternalError;
        }
    }

    return MediaStatusCode::Ok;
}

MediaStatusCode AxAovPowerAdapter::WakeupTcmSensors(const std::vector<AovPipeConfig>& pipes)
{
    for (const auto& pipe : pipes) {
        if (!pipe.enable_tcm) {
            continue;
        }

        const int ret = ax_tcm_sns_wakeup(pipe.pipe_id);
        if (ret != 0) {
            return MediaStatusCode::InternalError;
        }
    }

    return MediaStatusCode::Ok;
}

}  // namespace aov::media::ax615
