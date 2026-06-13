#include "../sensor_registry.hpp"

namespace aov::media::ax615 {

namespace {

constexpr AX_U32 kWidth = 2560;
constexpr AX_U32 kHeight = 1440;
constexpr AX_F32 kFps = 30.0f;

SensorProfile MakeOs04d10Profile() {
    SensorProfile profile;
    profile.sensor_name = "os04d10";
    profile.driver_lib_name = "libsns_os04d10.so";
    profile.driver_obj_name = "gSnsos04d10Obj";
    profile.quick_start_obj_name = "gSnsos04d10ObjQs";
    profile.default_iq_path = "/opt/etc/iqfiles/os04d10";
    profile.dev_id = 0;
    profile.pipe_id = 0;
    profile.rx_dev = 0;
    profile.sns_clk_idx = 0;
    profile.sns_clk_rate = AX_SNS_CLK_24M;

    profile.mipi_attr.ePhyMode = AX_MIPI_PHY_TYPE_DPHY;
    profile.mipi_attr.eLaneNum = AX_MIPI_DATA_LANE_2;
    profile.mipi_attr.nDataRate = 720;
    profile.mipi_attr.nDataLaneMap[0] = 0;
    profile.mipi_attr.nDataLaneMap[1] = 1;
    profile.mipi_attr.nDataLaneMap[2] = 3;
    profile.mipi_attr.nDataLaneMap[3] = 4;
    profile.mipi_attr.nClkLane[0] = 2;
    profile.mipi_attr.nClkLane[1] = 5;

    profile.sns_attr.nWidth = kWidth;
    profile.sns_attr.nHeight = kHeight;
    profile.sns_attr.fFrameRate = kFps;
    profile.sns_attr.eSnsMode = AX_SNS_LINEAR_MODE;
    profile.sns_attr.eRawType = AX_RT_RAW10;
    profile.sns_attr.eBayerPattern = AX_BP_RGGB;
    profile.sns_attr.bTestPatternEnable = AX_FALSE;

    profile.dev_attr.bImgDataEnable = AX_TRUE;
    profile.dev_attr.bNonImgDataEnable = AX_FALSE;
    // Device attributes (matching QSDemo gOs04d10DevAttr)
    profile.dev_attr.bImgDataEnable = AX_TRUE;
    profile.dev_attr.bNonImgDataEnable = AX_FALSE;
    profile.dev_attr.eDevMode = AX_VIN_DEV_ONLINE;
    profile.dev_attr.eDevWorkMode = AX_VIN_DEV_WORK_MODE_1MULTIPLEX;
    profile.dev_attr.eSnsIntfType = AX_SNS_INTF_TYPE_MIPI_RAW;
    profile.dev_attr.tDevImgRgn[0] = {0, 0, kWidth, kHeight};
    profile.dev_attr.tDevImgRgn[1] = {0, 0, kWidth, kHeight};
    profile.dev_attr.tDevImgRgn[2] = {0, 0, kWidth, kHeight};
    profile.dev_attr.tDevImgRgn[3] = {0, 0, kWidth, kHeight};
    profile.dev_attr.ePixelFmt = AX_FORMAT_BAYER_RAW_10BPP_PACKED;
    profile.dev_attr.eBayerPattern = AX_BP_RGGB;
    profile.dev_attr.eSnsMode = AX_SNS_LINEAR_MODE;
    profile.dev_attr.eSnsOutputMode = AX_SNS_NORMAL;
    profile.dev_attr.tCompressInfo = {AX_COMPRESS_MODE_NONE, 0};
    profile.dev_attr.tFrameRateCtrl = {AX_INVALID_FRMRATE, AX_INVALID_FRMRATE};

    // Match QSDemo's gOs04d10PipeAttr exactly (vin_config.h line 175-184)
    // QSDemo uses NORMAL_MODE1 even with nLowMemMode=0
    profile.pipe_attr.ePipeWorkMode = AX_VIN_PIPE_NORMAL_MODE1;
    profile.pipe_attr.tPipeImgRgn = {0, 0, kWidth, kHeight};
    profile.pipe_attr.eBayerPattern = AX_BP_RGGB;
    profile.pipe_attr.ePixelFmt = AX_FORMAT_BAYER_RAW_10BPP_PACKED;
    profile.pipe_attr.eSnsMode = AX_SNS_LINEAR_MODE;
    profile.pipe_attr.tCompressInfo = {AX_COMPRESS_MODE_LOSSY, 4};
    profile.pipe_attr.tNrAttr.t3DnrAttr.bPwlEnable = AX_FALSE;
    profile.pipe_attr.tNrAttr.t3DnrAttr.tCompressInfo = {AX_COMPRESS_MODE_LOSSLESS, 0};
    profile.pipe_attr.tNrAttr.tAinrAttr.bPwlEnable = AX_FALSE;
    profile.pipe_attr.tNrAttr.tAinrAttr.tCompressInfo = {AX_COMPRESS_MODE_LOSSY, 0};
    profile.pipe_attr.tFrameRateCtrl = {AX_INVALID_FRMRATE, AX_INVALID_FRMRATE};

    AX_VIN_CHN_ATTR_T chn0{};
    chn0.nWidth = kWidth;
    chn0.nHeight = kHeight;
    chn0.nWidthStride = kWidth;
    chn0.eImgFormat = AX_FORMAT_YUV420_SEMIPLANAR;
    chn0.nDepth = 2;  // CRITICAL: Must be >0 to allow AX_VIN_GetYuvFrame (was 0, causing 0x80110180)
    chn0.tCompressInfo = {AX_COMPRESS_MODE_LOSSY, 0};
    chn0.tFrameRateCtrl = {AX_INVALID_FRMRATE, AX_INVALID_FRMRATE};
    profile.chn_attrs.push_back(chn0);

    return profile;
}

struct Os04d10Registrar {
    Os04d10Registrar() {
        SensorRegistry::Instance().Register(MakeOs04d10Profile());
    }
};

static Os04d10Registrar g_os04d10_registrar;

}  // namespace

}  // namespace aov::media::ax615
