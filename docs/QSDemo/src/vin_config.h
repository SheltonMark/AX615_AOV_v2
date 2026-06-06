/**************************************************************************************************
 *
 * Copyright (c) 2019-2025 Axera Semiconductor Co., Ltd. All Rights Reserved.
 *
 * This source file is the property of Axera Semiconductor Co., Ltd. and
 * may not be copied or distributed in any isomorphic form without the prior
 * written consent of Axera Semiconductor Co., Ltd.
 *
 **************************************************************************************************/

#ifndef _VIN_CONFIG__
#define _VIN_CONFIG__

#include "ax_global_type.h"
#include "ax_vin_api.h"
#include "ax_mipi_rx_api.h"
#include "qsdemo.h"

#if defined(AX_FAST_BOOT)
#define SNS_FPS_DEFAULT  15
#else
#define SNS_FPS_DEFAULT  15
#endif

typedef struct _AX_SENSOR_LIB_INF_ {
    AX_CHAR szlibName[32];
    AX_CHAR szObjName[32];
    AX_CHAR szObjQsName[32];
} AX_SENSOR_LIB_INF;

#if defined(OS04A10_SUPPORT)

AX_SENSOR_LIB_INF gOs04a10SnsLib = {
    .szlibName = "libsns_os04a10.so",
    .szObjName = "gSnsos04a10Obj",
    .szObjQsName = "gSnsos04a10ObjQs",
};

AX_MIPI_RX_ATTR_T gOs04a10MipiAttr = {
    .ePhyMode = AX_MIPI_PHY_TYPE_DPHY,
    .eLaneNum = AX_MIPI_DATA_LANE_4,
    .nDataRate =  80,
    .nDataLaneMap[0] = 3,
    .nDataLaneMap[1] = 4,
    .nDataLaneMap[2] = 0,
    .nDataLaneMap[3] = 1,
    .nClkLane[0]     = 5,
    .nClkLane[1]     = 2,
};

AX_SNS_ATTR_T gOs04a10SnsAttr = {
    .nWidth = 2688,
    .nHeight = 1520,
    .fFrameRate = SNS_FPS_DEFAULT,
    .eSnsMode = QS_DEMO_DEFAULT_SNS_MODE,
    .eRawType = AX_RT_RAW10,
    .eBayerPattern = AX_BP_RGGB,
    .bTestPatternEnable = AX_FALSE,
    // .nSettingIndex = 12,
};

QS_AX_SNS_CLK_ATTR_T gOs04a10SnsClkAttr = {
    .nSnsClkIdx = 0,
    .eSnsClkRate = AX_SNS_CLK_24M,
};

AX_VIN_DEV_ATTR_T gOs04a10DevAttr = {
    .bImgDataEnable = AX_TRUE,
    .bNonImgDataEnable = AX_FALSE,
    .eDevMode = AX_VIN_DEV_ONLINE,
    .eDevWorkMode = AX_VIN_DEV_WORK_MODE_1MULTIPLEX,
    .eSnsIntfType = AX_SNS_INTF_TYPE_MIPI_RAW,
    .tDevImgRgn[0] = {0, 0, 2688, 1520},
    .tDevImgRgn[1] = {0, 0, 2688, 1520},
    .tDevImgRgn[2] = {0, 0, 2688, 1520},
    .tDevImgRgn[3] = {0, 0, 2688, 1520},

    /* When users transfer special data, they need to configure VC&DT for szImgVc/szImgDt/szInfoVc/szInfoDt */
    //.tMipiIntfAttr.szImgVc[0] = 0,
    //.tMipiIntfAttr.szImgDt[0] = MIPI_CSI_DT_RAW10,
    //.tMipiIntfAttr.szImgVc[1] = 1,
    //.tMipiIntfAttr.szImgDt[1] = MIPI_CSI_DT_RAW10,

    .ePixelFmt = AX_FORMAT_BAYER_RAW_10BPP_PACKED,
    .eBayerPattern = AX_BP_RGGB,
    .eSnsMode = QS_DEMO_DEFAULT_SNS_MODE,
    .eSnsOutputMode = AX_SNS_NORMAL,
    .tCompressInfo = {AX_COMPRESS_MODE_NONE, 0},
    .tFrameRateCtrl= {AX_INVALID_FRMRATE, AX_INVALID_FRMRATE},
};

AX_VIN_PIPE_ATTR_T gOs04a10PipeAttr = {
    .ePipeWorkMode = AX_VIN_PIPE_NORMAL_MODE1,
    .tPipeImgRgn = {0, 0, 2688, 1520},
    .eBayerPattern = AX_BP_RGGB,
    .ePixelFmt = AX_FORMAT_BAYER_RAW_10BPP_PACKED,
    .eSnsMode = QS_DEMO_DEFAULT_SNS_MODE,
    .tCompressInfo = {AX_COMPRESS_MODE_LOSSY, 4},
    .tNrAttr = {{0, {AX_COMPRESS_MODE_LOSSLESS, 0}}, {0, {AX_COMPRESS_MODE_LOSSY, 0}}},
    .tFrameRateCtrl = {AX_INVALID_FRMRATE, AX_INVALID_FRMRATE},
};

AX_VIN_CHN_ATTR_T gOs04a10Chn0Attr = {
    .nWidth = 2688,
    .nHeight = 1520,
    .nWidthStride = 2688,
    .eImgFormat = AX_FORMAT_YUV420_SEMIPLANAR,
    .nDepth = 0,
    .tCompressInfo = {AX_COMPRESS_MODE_LOSSY, 0},
    .tFrameRateCtrl = {AX_INVALID_FRMRATE, AX_INVALID_FRMRATE},
};
#endif

#if defined(OS04D10_SUPPORT)

AX_SENSOR_LIB_INF gOs04d10SnsLib = {
    .szlibName = "libsns_os04d10.so",
    .szObjName = "gSnsos04d10Obj",
    .szObjQsName = "gSnsos04d10ObjQs",
};

AX_MIPI_RX_ATTR_T gOs04d10MipiAttr = {
    .ePhyMode = AX_MIPI_PHY_TYPE_DPHY,
    .eLaneNum = AX_MIPI_DATA_LANE_2,
    .nDataRate = 720,
    .nDataLaneMap[0] = 0,
    .nDataLaneMap[1] = 1,
    .nDataLaneMap[2] = 3,
    .nDataLaneMap[3] = 4,
    .nClkLane[0]     = 2,
    .nClkLane[1]     = 5,
};

AX_SNS_ATTR_T gOs04d10SnsAttr = {
    .nWidth = 2560,
    .nHeight = 1440,
    .fFrameRate = 30,
    .eSnsMode = QS_DEMO_DEFAULT_SNS_MODE,
    .eRawType = AX_RT_RAW10,
    .eBayerPattern = AX_BP_RGGB,
    .bTestPatternEnable = AX_FALSE,
    // .nSettingIndex = 12,
};

QS_AX_SNS_CLK_ATTR_T gOs04d10SnsClkAttr = {
    .nSnsClkIdx = 0,
    .eSnsClkRate = AX_SNS_CLK_24M,
};

AX_VIN_DEV_ATTR_T gOs04d10DevAttr = {
    .bImgDataEnable = AX_TRUE,
    .bNonImgDataEnable = AX_FALSE,
    .eDevMode = AX_VIN_DEV_ONLINE,
    .eDevWorkMode = AX_VIN_DEV_WORK_MODE_1MULTIPLEX,
    .eSnsIntfType = AX_SNS_INTF_TYPE_MIPI_RAW,
    .tDevImgRgn[0] = {0, 0, 2560, 1440},
    .tDevImgRgn[1] = {0, 0, 2560, 1440},
    .tDevImgRgn[2] = {0, 0, 2560, 1440},
    .tDevImgRgn[3] = {0, 0, 2560, 1440},

    /* When users transfer special data, they need to configure VC&DT for szImgVc/szImgDt/szInfoVc/szInfoDt */
    //.tMipiIntfAttr.szImgVc[0] = 0,
    //.tMipiIntfAttr.szImgDt[0] = MIPI_CSI_DT_RAW10,
    //.tMipiIntfAttr.szImgVc[1] = 1,
    //.tMipiIntfAttr.szImgDt[1] = MIPI_CSI_DT_RAW10,

    .ePixelFmt = AX_FORMAT_BAYER_RAW_10BPP_PACKED,
    .eBayerPattern = AX_BP_RGGB,
    .eSnsMode = QS_DEMO_DEFAULT_SNS_MODE,
    .eSnsOutputMode = AX_SNS_NORMAL,
    .tCompressInfo = {AX_COMPRESS_MODE_NONE, 0},
    .tFrameRateCtrl= {AX_INVALID_FRMRATE, AX_INVALID_FRMRATE},
};

AX_VIN_PIPE_ATTR_T gOs04d10PipeAttr = {
    .ePipeWorkMode = AX_VIN_PIPE_NORMAL_MODE1,
    .tPipeImgRgn = {0, 0, 2560, 1440},
    .eBayerPattern = AX_BP_RGGB,
    .ePixelFmt = AX_FORMAT_BAYER_RAW_10BPP_PACKED,
    .eSnsMode = QS_DEMO_DEFAULT_SNS_MODE,
    .tCompressInfo = {AX_COMPRESS_MODE_LOSSY, 4},
    .tNrAttr = {{0, {AX_COMPRESS_MODE_LOSSLESS, 0}}, {0, {AX_COMPRESS_MODE_LOSSY, 0}}},
    .tFrameRateCtrl = {AX_INVALID_FRMRATE, AX_INVALID_FRMRATE},
};

AX_VIN_CHN_ATTR_T gOs04d10Chn0Attr = {
    .nWidth = 2560,
    .nHeight = 1440,
    .nWidthStride = 2560,
    .eImgFormat = AX_FORMAT_YUV420_SEMIPLANAR,
    .nDepth = 0,
    .tCompressInfo = {AX_COMPRESS_MODE_LOSSY, 0},
    .tFrameRateCtrl = {AX_INVALID_FRMRATE, AX_INVALID_FRMRATE},
};
#endif

#if defined(SC200AI_SUPPORT)

AX_SENSOR_LIB_INF gSc200aiSnsLib = {
    .szlibName = "libsns_sc200ai.so",
    .szObjName = "gSnssc200aiObj",
    .szObjQsName = "gSnssc200aiObjQs",
};

AX_MIPI_RX_ATTR_T gSc200aiMipiAttr = {
    .ePhyMode = AX_MIPI_PHY_TYPE_DPHY,
    .eLaneNum = AX_MIPI_DATA_LANE_2,
    .nDataRate =  400,
    .nDataLaneMap[0] = 0,
    .nDataLaneMap[1] = 1,
    .nDataLaneMap[2] = 3,
    .nDataLaneMap[3] = 4,
    .nClkLane[0]     = 2,
    .nClkLane[1]     = 5,
};

AX_SNS_ATTR_T gSc200aiSnsAttr = {
    .nWidth = 1920,
    .nHeight = 1080,
    .fFrameRate = SNS_FPS_DEFAULT,
    .eSnsMode = QS_DEMO_DEFAULT_SNS_MODE,
    .eRawType = AX_RT_RAW10,
    .eBayerPattern = AX_BP_RGGB,
    .bTestPatternEnable = AX_FALSE,
    // .nSettingIndex = 12,
};

QS_AX_SNS_CLK_ATTR_T gSc200aiSnsClkAttr = {
    .nSnsClkIdx = 0,
    .eSnsClkRate = AX_SNS_CLK_24M,
};

AX_VIN_DEV_ATTR_T gSc200aiDevAttr = {
    .bImgDataEnable = AX_TRUE,
    .bNonImgDataEnable = AX_FALSE,
    .eDevMode = AX_VIN_DEV_ONLINE,
    .eDevWorkMode = AX_VIN_DEV_WORK_MODE_1MULTIPLEX,
    .eSnsIntfType = AX_SNS_INTF_TYPE_MIPI_RAW,
    .tDevImgRgn[0] = {0, 0, 1920, 1080},
    .tDevImgRgn[1] = {0, 0, 1920, 1080},
    .tDevImgRgn[2] = {0, 0, 1920, 1080},
    .tDevImgRgn[3] = {0, 0, 1920, 1080},

    /* When users transfer special data, they need to configure VC&DT for szImgVc/szImgDt/szInfoVc/szInfoDt */
    // .tMipiIntfAttr.szImgVc[0] = 0;
    // .tMipiIntfAttr.szImgVc[1] = 1;
    // .tMipiIntfAttr.szImgDt[0] = 44;
    // .tMipiIntfAttr.szImgDt[1] = 44;
    // .tMipiIntfAttr.szInfoVc[0] = 31;
    // .tMipiIntfAttr.szInfoVc[1] = 31;
    // .tMipiIntfAttr.szInfoDt[0] = 63;
    // .tMipiIntfAttr.szInfoDt[1] = 63;

    .ePixelFmt = AX_FORMAT_BAYER_RAW_10BPP_PACKED,
    .eBayerPattern = AX_BP_RGGB,
    .eSnsMode = QS_DEMO_DEFAULT_SNS_MODE,
    .eSnsOutputMode = AX_SNS_NORMAL,
    .tCompressInfo = {AX_COMPRESS_MODE_NONE, 0},
    .tFrameRateCtrl= {AX_INVALID_FRMRATE, AX_INVALID_FRMRATE},
};

AX_VIN_PIPE_ATTR_T gSc200aiPipeAttr = {
    .ePipeWorkMode = AX_VIN_PIPE_NORMAL_MODE1,
    .tPipeImgRgn = {0, 0, 1920, 1080},
    .eBayerPattern = AX_BP_RGGB,
    .ePixelFmt = AX_FORMAT_BAYER_RAW_10BPP_PACKED,
    .eSnsMode = QS_DEMO_DEFAULT_SNS_MODE,
    .tCompressInfo = {AX_COMPRESS_MODE_LOSSY, 4},
    .tNrAttr = {{0, {AX_COMPRESS_MODE_LOSSLESS, 0}}, {0, {AX_COMPRESS_MODE_LOSSY, 0}}},
    .tFrameRateCtrl = {AX_INVALID_FRMRATE, AX_INVALID_FRMRATE},
};

AX_VIN_CHN_ATTR_T gSc200aiChn0Attr = {
    .nWidth = 1920,
    .nHeight = 1080,
    .nWidthStride = 1920,
    .eImgFormat = AX_FORMAT_YUV420_SEMIPLANAR,
    .nDepth = 0,
    .tCompressInfo = {AX_COMPRESS_MODE_LOSSY, 0},
    .tFrameRateCtrl = {AX_INVALID_FRMRATE, AX_INVALID_FRMRATE},
};
#endif

#if defined(SC500AI_SUPPORT)

AX_SENSOR_LIB_INF gSc500aiSnsLib = {
    .szlibName = "libsns_sc500ai.so",
    .szObjName = "gSnssc500aiObj",
    .szObjQsName = "gSnssc500aiObjQs",
};

AX_MIPI_RX_ATTR_T gSc500aiMipiAttr = {
    .ePhyMode = AX_MIPI_PHY_TYPE_DPHY,
    .eLaneNum = AX_MIPI_DATA_LANE_4,
    .nDataRate = 396,
    .nDataLaneMap[0] = 0,
    .nDataLaneMap[1] = 1,
    .nDataLaneMap[2] = 3,
    .nDataLaneMap[3] = 4,
    .nClkLane[0]     = 2,
    .nClkLane[1]     = 5,
};

AX_SNS_ATTR_T gSc500aiSnsAttr = {
    .nWidth = 2880,
    .nHeight = 1620,
    .fFrameRate = SNS_FPS_DEFAULT,
    .eSnsMode = QS_DEMO_DEFAULT_SNS_MODE,
    .eRawType = AX_RT_RAW10,
    .eBayerPattern = AX_BP_RGGB,
    .bTestPatternEnable = AX_FALSE,
    // .nSettingIndex = 12,
};

QS_AX_SNS_CLK_ATTR_T gSc500aiSnsClkAttr = {
    .nSnsClkIdx = 0,
    .eSnsClkRate = AX_SNS_CLK_24M,
};

AX_VIN_DEV_ATTR_T gSc500aiDevAttr = {
    .bImgDataEnable = AX_TRUE,
    .bNonImgDataEnable = AX_FALSE,
    .eDevMode = AX_VIN_DEV_ONLINE,
    .eDevWorkMode = AX_VIN_DEV_WORK_MODE_1MULTIPLEX,
    .eSnsIntfType = AX_SNS_INTF_TYPE_MIPI_RAW,
    .tDevImgRgn[0] = {0, 0, 2880, 1620},
    .tDevImgRgn[1] = {0, 0, 2880, 1620},
    .tDevImgRgn[2] = {0, 0, 2880, 1620},
    .tDevImgRgn[3] = {0, 0, 2880, 1620},

    /* When users transfer special data, they need to configure VC&DT for szImgVc/szImgDt/szInfoVc/szInfoDt */
    //.tMipiIntfAttr.szImgVc[0] = 0,
    //.tMipiIntfAttr.szImgDt[0] = MIPI_CSI_DT_RAW10,
    //.tMipiIntfAttr.szImgVc[1] = 1,
    //.tMipiIntfAttr.szImgDt[1] = MIPI_CSI_DT_RAW10,

    .ePixelFmt = AX_FORMAT_BAYER_RAW_10BPP_PACKED,
    .eBayerPattern = AX_BP_RGGB,
    .eSnsMode = QS_DEMO_DEFAULT_SNS_MODE,
    .eSnsOutputMode = AX_SNS_NORMAL,
    .tCompressInfo = {AX_COMPRESS_MODE_NONE, 0},
    .tFrameRateCtrl= {AX_INVALID_FRMRATE, AX_INVALID_FRMRATE},
};

AX_VIN_PIPE_ATTR_T gSc500aiPipeAttr = {
    .ePipeWorkMode = AX_VIN_PIPE_NORMAL_MODE1,
    .tPipeImgRgn = {0, 0, 2880, 1620},
    .eBayerPattern = AX_BP_RGGB,
    .ePixelFmt = AX_FORMAT_BAYER_RAW_10BPP_PACKED,
    .eSnsMode = QS_DEMO_DEFAULT_SNS_MODE,
    .tCompressInfo = {AX_COMPRESS_MODE_LOSSY, 4},
    .tNrAttr = {{0, {AX_COMPRESS_MODE_LOSSLESS, 0}}, {0, {AX_COMPRESS_MODE_LOSSY, 0}}},
    .tFrameRateCtrl = {AX_INVALID_FRMRATE, AX_INVALID_FRMRATE},
};

AX_VIN_CHN_ATTR_T gSc500aiChn0Attr = {
    .nWidth = 2880,
    .nHeight = 1620,
    .nWidthStride = 2944,
    .eImgFormat = AX_FORMAT_YUV420_SEMIPLANAR,
    .nDepth = 0,
    .tCompressInfo = {AX_COMPRESS_MODE_LOSSY, 0},
    .tFrameRateCtrl = {AX_INVALID_FRMRATE, AX_INVALID_FRMRATE},
};
#endif

#if defined(SC450AI_SUPPORT)

AX_SENSOR_LIB_INF gSc450aiSnsLib = {
    .szlibName = "libsns_sc450ai.so",
    .szObjName = "gSnssc450aiObj",
    .szObjQsName = "gSnssc450aiObjQs",
};

AX_MIPI_RX_ATTR_T gSc450aiMipiAttr = {
    .ePhyMode = AX_MIPI_PHY_TYPE_DPHY,
    .eLaneNum = AX_MIPI_DATA_LANE_4,
    .nDataRate =  80,
    .nDataLaneMap[0] = 0,
    .nDataLaneMap[1] = 1,
    .nDataLaneMap[2] = 3,
    .nDataLaneMap[3] = 4,
    .nClkLane[0]     = 2,
    .nClkLane[1]     = 5,
};

AX_SNS_ATTR_T gSc450aiSnsAttr = {
    .nWidth = 2688,
    .nHeight = 1520,
    .fFrameRate = SNS_FPS_DEFAULT,
    .eSnsMode = QS_DEMO_DEFAULT_SNS_MODE,
    .eRawType = AX_RT_RAW10,
    .eBayerPattern = AX_BP_RGGB,
    .bTestPatternEnable = AX_FALSE,
    // .nSettingIndex = 12,
};

QS_AX_SNS_CLK_ATTR_T gSc450aiSnsClkAttr = {
    .nSnsClkIdx = 0,
    .eSnsClkRate = AX_SNS_CLK_24M,
};

AX_VIN_DEV_ATTR_T gSc450aiDevAttr = {
    .bImgDataEnable = AX_TRUE,
    .bNonImgDataEnable = AX_FALSE,
    .eDevMode = AX_VIN_DEV_ONLINE,
    .eDevWorkMode = AX_VIN_DEV_WORK_MODE_1MULTIPLEX,
    .eSnsIntfType = AX_SNS_INTF_TYPE_MIPI_RAW,
    .tDevImgRgn[0] = {0, 0, 2688, 1520},
    .tDevImgRgn[1] = {0, 0, 2688, 1520},
    .tDevImgRgn[2] = {0, 0, 2688, 1520},
    .tDevImgRgn[3] = {0, 0, 2688, 1520},

    /* When users transfer special data, they need to configure VC&DT for szImgVc/szImgDt/szInfoVc/szInfoDt */
    //.tMipiIntfAttr.szImgVc[0] = 0,
    //.tMipiIntfAttr.szImgDt[0] = MIPI_CSI_DT_RAW10,
    //.tMipiIntfAttr.szImgVc[1] = 1,
    //.tMipiIntfAttr.szImgDt[1] = MIPI_CSI_DT_RAW10,

    .ePixelFmt = AX_FORMAT_BAYER_RAW_10BPP_PACKED,
    .eBayerPattern = AX_BP_RGGB,
    .eSnsMode = QS_DEMO_DEFAULT_SNS_MODE,
    .eSnsOutputMode = AX_SNS_NORMAL,
    .tCompressInfo = {AX_COMPRESS_MODE_NONE, 0},
    .tFrameRateCtrl= {AX_INVALID_FRMRATE, AX_INVALID_FRMRATE},
};

AX_VIN_PIPE_ATTR_T gSc450aiPipeAttr = {
    .ePipeWorkMode = AX_VIN_PIPE_NORMAL_MODE1,
    .tPipeImgRgn = {0, 0, 2688, 1520},
    .eBayerPattern = AX_BP_RGGB,
    .ePixelFmt = AX_FORMAT_BAYER_RAW_10BPP_PACKED,
    .eSnsMode = QS_DEMO_DEFAULT_SNS_MODE,
    .tCompressInfo = {AX_COMPRESS_MODE_LOSSY, 4},
    .tNrAttr = {{0, {AX_COMPRESS_MODE_LOSSLESS, 0}}, {0, {AX_COMPRESS_MODE_LOSSY, 0}}},
    .tFrameRateCtrl = {AX_INVALID_FRMRATE, AX_INVALID_FRMRATE},
};

AX_VIN_CHN_ATTR_T gSc450aiChn0Attr = {
    .nWidth = 2688,
    .nHeight = 1520,
    .nWidthStride = 2688,
    .eImgFormat = AX_FORMAT_YUV420_SEMIPLANAR,
    .nDepth = 0,
    .tCompressInfo = {AX_COMPRESS_MODE_LOSSY, 0},
    .tFrameRateCtrl = {AX_INVALID_FRMRATE, AX_INVALID_FRMRATE},
};
#endif

#if defined(SC235HAI_SUPPORT)

AX_SENSOR_LIB_INF gSc235haiSnsLib = {
    .szlibName = "libsns_sc235hai.so",
    .szObjName = "gSnssc235haiObj",
    .szObjQsName = "gSnssc235haiObjQs",
};

AX_MIPI_RX_ATTR_T gSc235haiMipiAttr = {
    .ePhyMode = AX_MIPI_PHY_TYPE_DPHY,
    .eLaneNum = AX_MIPI_DATA_LANE_2,
    .nDataRate =  396,
    .nDataLaneMap[0] = 0,
    .nDataLaneMap[1] = 1,
    .nClkLane[0]     = 2,
    .nClkLane[1]     = 5,
};

AX_SNS_ATTR_T gSc235haiSnsAttr = {
    .nWidth = 1920,
    .nHeight = 1080,
    .fFrameRate = SNS_FPS_DEFAULT,
    .eSnsMode = QS_DEMO_DEFAULT_SNS_MODE,
    .eRawType = AX_RT_RAW10,
    .eBayerPattern = AX_BP_RGGB,
    .bTestPatternEnable = AX_FALSE,
    // .nSettingIndex = 12,
};

QS_AX_SNS_CLK_ATTR_T gSc235haiSnsClkAttr = {
    .nSnsClkIdx = 0,
    .eSnsClkRate = AX_SNS_CLK_24M,
};

AX_VIN_DEV_ATTR_T gSc235haiDevAttr = {
    .bImgDataEnable = AX_TRUE,
    .bNonImgDataEnable = AX_FALSE,
    .eDevMode = AX_VIN_DEV_ONLINE,
    .eSnsIntfType = AX_SNS_INTF_TYPE_MIPI_RAW,
    .tDevImgRgn[0] = {0, 0, 1920, 1080},
    .tDevImgRgn[1] = {0, 0, 1920, 1080},
    .tDevImgRgn[2] = {0, 0, 1920, 1080},
    .tDevImgRgn[3] = {0, 0, 1920, 1080},

    .ePixelFmt = AX_FORMAT_BAYER_RAW_10BPP_PACKED,
    .eBayerPattern = AX_BP_RGGB,
    .eSnsMode = QS_DEMO_DEFAULT_SNS_MODE,
    .eSnsOutputMode = AX_SNS_NORMAL,
    .tCompressInfo = {AX_COMPRESS_MODE_NONE, 0},
    .tFrameRateCtrl= {AX_INVALID_FRMRATE, AX_INVALID_FRMRATE},
};

AX_VIN_PIPE_ATTR_T gSc235haiPipeAttr = {
    .ePipeWorkMode = AX_VIN_PIPE_NORMAL_MODE1,
    .tPipeImgRgn = {0, 0, 1920, 1080},
    .eBayerPattern = AX_BP_RGGB,
    .ePixelFmt = AX_FORMAT_BAYER_RAW_10BPP_PACKED,
    .eSnsMode = QS_DEMO_DEFAULT_SNS_MODE,
    .tCompressInfo = {AX_COMPRESS_MODE_LOSSY, 4},
    .tNrAttr = {{0, {AX_COMPRESS_MODE_LOSSLESS, 0}}, {0, {AX_COMPRESS_MODE_LOSSY, 0}}},
    .tFrameRateCtrl = {AX_INVALID_FRMRATE, AX_INVALID_FRMRATE},
};

AX_VIN_CHN_ATTR_T gSc235haiChn0Attr = {
    .nWidth = 1920,
    .nHeight = 1080,
    .nWidthStride = 1920,
    .eImgFormat = AX_FORMAT_YUV420_SEMIPLANAR,
    .nDepth = 0,
    .tCompressInfo = {AX_COMPRESS_MODE_LOSSY, 0},
    .tFrameRateCtrl = {AX_INVALID_FRMRATE, AX_INVALID_FRMRATE},
};
#endif

#if defined(SC438HAI_SUPPORT)

AX_SENSOR_LIB_INF gSc438haiSnsLib = {
    .szlibName = "libsns_sc438hai.so",
    .szObjName = "gSnssc438haiObj",
    .szObjQsName = "gSnssc438haiObjQs",
};

AX_MIPI_RX_ATTR_T gSc438haiMipiAttr = {
    .ePhyMode = AX_MIPI_PHY_TYPE_DPHY,
    .eLaneNum = AX_MIPI_DATA_LANE_4,
    .nDataRate =  756,
    .nDataLaneMap[0] = 0,
    .nDataLaneMap[1] = 1,
    .nDataLaneMap[2] = 3,
    .nDataLaneMap[3] = 4,
    .nClkLane[0]     = 2,
    .nClkLane[1]     = 5,
};

AX_SNS_ATTR_T gSc438haiSnsAttr = {
    .nWidth = 2688,
    .nHeight = 1520,
    .fFrameRate = SNS_FPS_DEFAULT,
    .eSnsMode = QS_DEMO_DEFAULT_SNS_MODE,
    .eRawType = AX_RT_RAW10,
    .eBayerPattern = AX_BP_RGGB,
    .bTestPatternEnable = AX_FALSE,
    .nSettingIndex = 0,
};

QS_AX_SNS_CLK_ATTR_T gSc438haiSnsClkAttr = {
    .nSnsClkIdx = 0,
    .eSnsClkRate = AX_SNS_CLK_27M,
};

AX_VIN_DEV_ATTR_T gSc438haiDevAttr = {
    .bImgDataEnable = AX_TRUE,
    .bNonImgDataEnable = AX_FALSE,
    .eDevMode = AX_VIN_DEV_ONLINE,
    .eDevWorkMode = AX_VIN_DEV_WORK_MODE_1MULTIPLEX,
    .eSnsIntfType = AX_SNS_INTF_TYPE_MIPI_RAW,
    .tDevImgRgn[0] = {0, 0, 2688, 1520},
    .tDevImgRgn[1] = {0, 0, 2688, 1520},
    .tDevImgRgn[2] = {0, 0, 2688, 1520},
    .tDevImgRgn[3] = {0, 0, 2688, 1520},

    .ePixelFmt = AX_FORMAT_BAYER_RAW_10BPP_PACKED,
    .eBayerPattern = AX_BP_RGGB,
    .eSnsMode = QS_DEMO_DEFAULT_SNS_MODE,
    .eSnsOutputMode = AX_SNS_NORMAL,
    .tCompressInfo = {AX_COMPRESS_MODE_NONE, 0},
    .tFrameRateCtrl= {AX_INVALID_FRMRATE, AX_INVALID_FRMRATE},
};

AX_VIN_PIPE_ATTR_T gSc438haiPipeAttr = {
    .ePipeWorkMode = AX_VIN_PIPE_NORMAL_MODE1,
    .tPipeImgRgn = {0, 0, 2688, 1520},
    .eBayerPattern = AX_BP_RGGB,
    .ePixelFmt = AX_FORMAT_BAYER_RAW_10BPP_PACKED,
    .eSnsMode = QS_DEMO_DEFAULT_SNS_MODE,
    .tCompressInfo = {AX_COMPRESS_MODE_LOSSY, 4},
    .tNrAttr = {{0, {AX_COMPRESS_MODE_LOSSLESS, 0}}, {0, {AX_COMPRESS_MODE_LOSSY, 0}}},
    .tFrameRateCtrl = {AX_INVALID_FRMRATE, AX_INVALID_FRMRATE},
};

AX_VIN_CHN_ATTR_T gSc438haiChn0Attr = {
    .nWidth = 2688,
    .nHeight = 1520,
    .nWidthStride = 2688,
    .eImgFormat = AX_FORMAT_YUV420_SEMIPLANAR,
    .nDepth = 0,
    .tCompressInfo = {AX_COMPRESS_MODE_LOSSY, 0},
    .tFrameRateCtrl = {AX_INVALID_FRMRATE, AX_INVALID_FRMRATE},
};
#endif

#if defined(SC533HAI_SUPPORT)

AX_SENSOR_LIB_INF gSc533haiSnsLib = {
    .szlibName = "libsns_sc533hai.so",
    .szObjName = "gSnssc533haiObj",
    .szObjQsName = "gSnsc533haiObjQs",
};

AX_MIPI_RX_ATTR_T gSc533haiMipiAttr = {
    .ePhyMode = AX_MIPI_PHY_TYPE_DPHY,
    .eLaneNum = AX_MIPI_DATA_LANE_2,
    .nDataRate =  972,
    .nDataLaneMap[0] = 0,
    .nDataLaneMap[1] = 1,
    .nDataLaneMap[2] = 3,
    .nDataLaneMap[3] = 4,
    .nClkLane[0]     = 2,
    .nClkLane[1]     = 5,
};

AX_SNS_ATTR_T gSc533haiSnsAttr = {
    .nWidth = 2880,
    .nHeight = 1620,
    .fFrameRate = SNS_FPS_DEFAULT,
    .eSnsMode = QS_DEMO_DEFAULT_SNS_MODE,
    .eRawType = AX_RT_RAW10,
    .eBayerPattern = AX_BP_RGGB,
    .bTestPatternEnable = AX_FALSE,
    .nSettingIndex = 4,
};

QS_AX_SNS_CLK_ATTR_T gSc533haiSnsClkAttr = {
    .nSnsClkIdx = 0,
    .eSnsClkRate = AX_SNS_CLK_24M,
};

AX_VIN_DEV_ATTR_T gSc533haiDevAttr = {
    .bImgDataEnable = AX_TRUE,
    .bNonImgDataEnable = AX_FALSE,
    .eDevMode = AX_VIN_DEV_ONLINE,
    .eDevWorkMode = AX_VIN_DEV_WORK_MODE_1MULTIPLEX,
    .eSnsIntfType = AX_SNS_INTF_TYPE_MIPI_RAW,
    .tDevImgRgn[0] = {0, 0, 2880, 1620},
    .tDevImgRgn[1] = {0, 0, 2880, 1620},
    .tDevImgRgn[2] = {0, 0, 2880, 1620},
    .tDevImgRgn[3] = {0, 0, 2880, 1620},

    .ePixelFmt = AX_FORMAT_BAYER_RAW_10BPP_PACKED,
    .eBayerPattern = AX_BP_RGGB,
    .eSnsMode = QS_DEMO_DEFAULT_SNS_MODE,
    .eSnsOutputMode = AX_SNS_NORMAL,
    .tCompressInfo = {AX_COMPRESS_MODE_NONE, 0},
    .tFrameRateCtrl= {AX_INVALID_FRMRATE, AX_INVALID_FRMRATE},
};

AX_VIN_PIPE_ATTR_T gSc533haiPipeAttr = {
    .ePipeWorkMode = AX_VIN_PIPE_NORMAL_MODE1,
    .tPipeImgRgn = {0, 0, 2880, 1620},
    .eBayerPattern = AX_BP_RGGB,
    .ePixelFmt = AX_FORMAT_BAYER_RAW_10BPP_PACKED,
    .eSnsMode = QS_DEMO_DEFAULT_SNS_MODE,
    .tCompressInfo = {AX_COMPRESS_MODE_LOSSY, 4},
    .tNrAttr = {{0, {AX_COMPRESS_MODE_LOSSLESS, 0}}, {0, {AX_COMPRESS_MODE_LOSSY, 0}}},
    .tFrameRateCtrl = {AX_INVALID_FRMRATE, AX_INVALID_FRMRATE},
};

AX_VIN_CHN_ATTR_T gSc533haiChn0Attr = {
    .nWidth = 2880,
    .nHeight = 1620,
    .nWidthStride = 2880,
    .eImgFormat = AX_FORMAT_YUV420_SEMIPLANAR,
    .nDepth = 0,
    .tCompressInfo = {AX_COMPRESS_MODE_LOSSY, 0},
    .tFrameRateCtrl = {AX_INVALID_FRMRATE, AX_INVALID_FRMRATE},
};
#endif

#if defined(MIS20S1_SUPPORT)

AX_SENSOR_LIB_INF gMis20s1SnsLib = {
    .szlibName = "libsns_mis20s1.so",
    .szObjName = "gSnsmis20s1Obj",
    .szObjQsName = "gSnsmis20s1ObjQs",
};

AX_MIPI_RX_ATTR_T gMis20s1MipiAttr = {
    .ePhyMode = AX_MIPI_PHY_TYPE_DPHY,
    .eLaneNum = AX_MIPI_DATA_LANE_2,
    .nDataRate = 990,
    .nDataLaneMap[0] = 0,
    .nDataLaneMap[1] = 1,
    .nDataLaneMap[2] = 3,
    .nDataLaneMap[3] = 4,
    .nClkLane[0]     = 2,
    .nClkLane[1]     = 5,
};

AX_SNS_ATTR_T gMis20s1SnsAttr = {
    .nWidth = 1920,
    .nHeight = 1080,
    .fFrameRate = SNS_FPS_DEFAULT,
    .eSnsMode = QS_DEMO_DEFAULT_SNS_MODE,
    .eRawType = AX_RT_RAW12,
    .eBayerPattern = AX_BP_RGGB,
    .bTestPatternEnable = AX_FALSE,
    .nSettingIndex = 4,
};

QS_AX_SNS_CLK_ATTR_T gMis20s1SnsClkAttr = {
    .nSnsClkIdx = 0,
    .eSnsClkRate = AX_SNS_CLK_27M,
};

AX_VIN_DEV_ATTR_T gMis20s1DevAttr = {
    .bImgDataEnable = AX_TRUE,
    .bNonImgDataEnable = AX_FALSE,
    .eDevMode = AX_VIN_DEV_ONLINE,
    .eDevWorkMode = AX_VIN_DEV_WORK_MODE_1MULTIPLEX,
    .eSnsIntfType = AX_SNS_INTF_TYPE_MIPI_RAW,
    .tDevImgRgn[0] = {0, 0, 1920, 1080},
    .tDevImgRgn[1] = {0, 0, 1920, 1080},
    .tDevImgRgn[2] = {0, 0, 1920, 1080},
    .tDevImgRgn[3] = {0, 0, 1920, 1080},

    .ePixelFmt = AX_FORMAT_BAYER_RAW_12BPP_PACKED,
    .eBayerPattern = AX_BP_RGGB,
    .eSnsMode = QS_DEMO_DEFAULT_SNS_MODE,
    .eSnsOutputMode = AX_SNS_NORMAL,
    .tCompressInfo = {AX_COMPRESS_MODE_NONE, 0},
    .tFrameRateCtrl= {AX_INVALID_FRMRATE, AX_INVALID_FRMRATE},
};

AX_VIN_PIPE_ATTR_T gMis20s1PipeAttr = {
    .ePipeWorkMode = AX_VIN_PIPE_NORMAL_MODE1,
    .tPipeImgRgn = {0, 0, 1920, 1080},
    .eBayerPattern = AX_BP_RGGB,
    .ePixelFmt = AX_FORMAT_BAYER_RAW_12BPP_PACKED,
    .eSnsMode = QS_DEMO_DEFAULT_SNS_MODE,
    .tCompressInfo = {AX_COMPRESS_MODE_LOSSY, 4},
    .tNrAttr = {{0, {AX_COMPRESS_MODE_LOSSLESS, 0}}, {0, {AX_COMPRESS_MODE_LOSSY, 0}}},
    .tFrameRateCtrl = {AX_INVALID_FRMRATE, AX_INVALID_FRMRATE},
};

AX_VIN_CHN_ATTR_T gMis20s1Chn0Attr = {
    .nWidth = 1920,
    .nHeight = 1080,
    .nWidthStride = 1920,
    .eImgFormat = AX_FORMAT_YUV420_SEMIPLANAR,
    .nDepth = 0,
    .tCompressInfo = {AX_COMPRESS_MODE_LOSSY, 0},
    .tFrameRateCtrl = {AX_INVALID_FRMRATE, AX_INVALID_FRMRATE},
};
#endif

#if defined(SENSOR_DUMMY_SUPPORT)

AX_SENSOR_LIB_INF gDummySnsLib = {
    .szlibName = "libsns_dummy.so",
    .szObjName = "gSnsdummyObj",
    .szObjQsName = "gSnsdummyObjQs",
};

AX_MIPI_RX_ATTR_T gDummyMipiAttr = {
    .ePhyMode = AX_MIPI_PHY_TYPE_DPHY,
    .eLaneNum = AX_MIPI_DATA_LANE_4,
    .nDataRate = 400,
    .nDataLaneMap[0] = 0,
    .nDataLaneMap[1] = 1,
    .nDataLaneMap[2] = 3,
    .nDataLaneMap[3] = 4,
    .nClkLane[0]     = 2,
    .nClkLane[1]     = 5,
};

AX_SNS_ATTR_T gDummySnsAttr = {
    .nWidth = 2688,
    .nHeight = 1520,
    .fFrameRate = SNS_FPS_DEFAULT,
    .eSnsMode = QS_DEMO_DEFAULT_SNS_MODE,
    .eRawType = AX_RT_RAW10,
    .eBayerPattern = AX_BP_RGGB,
    .bTestPatternEnable = AX_FALSE,
    .nSettingIndex = 0,
};

QS_AX_SNS_CLK_ATTR_T gDummySnsClkAttr = {
    .nSnsClkIdx = 0,
    .eSnsClkRate = AX_SNS_CLK_NOT_CFG,
};

AX_VIN_DEV_ATTR_T gDummyDevAttr = {
    .bImgDataEnable = AX_TRUE,
    .bNonImgDataEnable = AX_FALSE,
    .eDevMode = AX_VIN_DEV_ONLINE,
    .eDevWorkMode = AX_VIN_DEV_WORK_MODE_1MULTIPLEX,
    .eSnsIntfType = AX_SNS_INTF_TYPE_TPG,
    .nRealWidth = 2688,
    .nRealHeight = 1520,
    .tDevImgRgn[0] = {0, 0, 2688, 1520},
    .tDevImgRgn[1] = {0, 0, 2688, 1520},
    .tDevImgRgn[2] = {0, 0, 2688, 1520},
    .tDevImgRgn[3] = {0, 0, 2688, 1520},

    .ePixelFmt = AX_FORMAT_BAYER_RAW_10BPP_PACKED,
    .eBayerPattern = AX_BP_RGGB,
    .eSnsMode = QS_DEMO_DEFAULT_SNS_MODE,
    .eSnsOutputMode = AX_SNS_NORMAL,
    .tCompressInfo = {AX_COMPRESS_MODE_NONE, 0},
    .tFrameRateCtrl= {AX_INVALID_FRMRATE, AX_INVALID_FRMRATE},

    .tTpgIntfAttr = {0, 30, ISP_TPG_BAR_H_INC},
};

AX_VIN_PIPE_ATTR_T gDummyPipeAttr = {
    .ePipeWorkMode = AX_VIN_PIPE_NORMAL_MODE1,
    .tPipeImgRgn = {0, 0, 2688, 1520},
    .eBayerPattern = AX_BP_RGGB,
    .ePixelFmt = AX_FORMAT_BAYER_RAW_10BPP_PACKED,
    .eSnsMode = QS_DEMO_DEFAULT_SNS_MODE,
    .tCompressInfo = {AX_COMPRESS_MODE_LOSSY, 4},
    .tNrAttr = {{0, {AX_COMPRESS_MODE_LOSSLESS, 0}}, {0, {AX_COMPRESS_MODE_LOSSY, 0}}},
    .tFrameRateCtrl = {AX_INVALID_FRMRATE, AX_INVALID_FRMRATE},
};

AX_VIN_CHN_ATTR_T gDummyChn0Attr = {
    .nWidth = 2688,
    .nHeight = 1520,
    .nWidthStride = 2688,
    .eImgFormat = AX_FORMAT_YUV420_SEMIPLANAR,
    .nDepth = 0,
    .tCompressInfo = {AX_COMPRESS_MODE_LOSSY, 0},
    .tFrameRateCtrl = {AX_INVALID_FRMRATE, AX_INVALID_FRMRATE},
};
#endif

#if defined(SC465SL_SUPPORT)

AX_SENSOR_LIB_INF gSc465slSnsLib = {
    .szlibName = "libsns_sc465sl.so",
    .szObjName = "gSnssc465slObj",
    .szObjQsName = "gSnssc465slObjQs",
};

AX_MIPI_RX_ATTR_T gSc465slMipiAttr = {
    .ePhyMode = AX_MIPI_PHY_TYPE_DPHY,
    .eLaneNum = AX_MIPI_DATA_LANE_4,
    .nDataRate =  720,
    .nDataLaneMap[0] = 0,
    .nDataLaneMap[1] = 1,
    .nDataLaneMap[2] = 3,
    .nDataLaneMap[3] = 4,
    .nClkLane[0]     = 2,
    .nClkLane[1]     = 5,
};

AX_SNS_ATTR_T gSc465slSnsAttr = {
    .nWidth = 2560,
    .nHeight = 1440,
    .fFrameRate = SNS_FPS_DEFAULT,
    .eSnsMode = QS_DEMO_DEFAULT_SNS_MODE,
    .eRawType = AX_RT_RAW10,
    .eBayerPattern = AX_BP_RGGB,
    .bTestPatternEnable = AX_FALSE,
    .nSettingIndex = 0,
};

QS_AX_SNS_CLK_ATTR_T gSc465slSnsClkAttr = {
    .nSnsClkIdx = 0,
    .eSnsClkRate = AX_SNS_CLK_24M,
};

AX_VIN_DEV_ATTR_T gSc465slDevAttr = {
    .bImgDataEnable = AX_TRUE,
    .bNonImgDataEnable = AX_FALSE,
    .eDevMode = AX_VIN_DEV_ONLINE,
    .eSnsIntfType = AX_SNS_INTF_TYPE_MIPI_RAW,
    .nRealWidth = 2560,
    .nRealHeight = 1440,
    .tDevImgRgn[0] = {0, 0, 2560, 1440},
    .tDevImgRgn[1] = {0, 0, 2560, 1440},
    .tDevImgRgn[2] = {0, 0, 2560, 1440},
    .tDevImgRgn[3] = {0, 0, 2560, 1440},

    .ePixelFmt = AX_FORMAT_BAYER_RAW_10BPP_PACKED,
    .eBayerPattern = AX_BP_RGGB,
    .eSnsMode = AX_SNS_LINEAR_MODE,
    .eSnsOutputMode = AX_SNS_NORMAL,
    .tCompressInfo = {AX_COMPRESS_MODE_NONE, 0},
    .tFrameRateCtrl= {AX_INVALID_FRMRATE, AX_INVALID_FRMRATE},
};

AX_VIN_PIPE_ATTR_T gSc465slPipeAttr = {
    .ePipeWorkMode = AX_VIN_PIPE_NORMAL_MODE1,
    .tPipeImgRgn = {0, 0, 2560, 1440},
    .eBayerPattern = AX_BP_RGGB,
    .ePixelFmt = AX_FORMAT_BAYER_RAW_10BPP_PACKED,
    .eSnsMode = AX_SNS_LINEAR_MODE,
    .tCompressInfo = {AX_COMPRESS_MODE_LOSSY, 4},
    .tNrAttr = {{0, {AX_COMPRESS_MODE_LOSSLESS, 0}}, {0, {AX_COMPRESS_MODE_LOSSY, 0}}},
    .tFrameRateCtrl = {AX_INVALID_FRMRATE, AX_INVALID_FRMRATE},
};

AX_VIN_CHN_ATTR_T gSc465slChn0Attr = {
    .nWidth = 2560,
    .nHeight = 1440,
    .nWidthStride = 2560,
    .eImgFormat = AX_FORMAT_YUV420_SEMIPLANAR,
    .nDepth = 1,
    .tCompressInfo = {AX_COMPRESS_MODE_LOSSY, 0},
    .tFrameRateCtrl = {AX_INVALID_FRMRATE, AX_INVALID_FRMRATE},
};
#endif

#endif // _VIN_CONFIG__
