/**************************************************************************************************
 *
 * Copyright (c) 2019-2025 Axera Semiconductor Co., Ltd. All Rights Reserved.
 *
 * This source file is the property of Axera Semiconductor Co., Ltd. and
 * may not be copied or distributed in any isomorphic form without the prior
 * written consent of Axera Semiconductor Co., Ltd.
 *
 **************************************************************************************************/
#include <stdio.h>
#include <stdlib.h>
#ifndef __RTT_OS__
#include <dlfcn.h>
#endif
#include "vin_config.h"
#include "qsdemo.h"
#include "qs_timer.h"
#include <pthread.h>
#include "qs_utils.h"

#define AX_LIB_SENSOR_PATH  "/opt/lib/"

SAMPLE_CHN_ATTR_T gSingleSnsOutChnAttr[] = {
    // ch0 resolution will autofit with sns0 chn0
    {.nWidth = 2688, .nHeight = 1520, .fFramerate = SENSOR_FRAMERATE, .enCompressMode = 2, .u32CompressLevel = 10, .bVenc = AX_TRUE, .nJencChn = 2, .nRtspChn = 0, .fDetectWidthRatio = 2688.0/640.0f, .fDetectHeightRatio = 1520.0/352.0f, .ptRgn = NULL},
    {.nWidth = 720, .nHeight = 576, .fFramerate = SENSOR_FRAMERATE, .enCompressMode = 2, .u32CompressLevel = 4, .bVenc = AX_TRUE, .nJencChn = -1, .nRtspChn = 1, .fDetectWidthRatio = 720.0/640.0f, .fDetectHeightRatio = 576.0/352.0f, .ptRgn = NULL},
    {.nWidth = 640, .nHeight = 352, .fFramerate = SENSOR_FRAMERATE, .enCompressMode = 0, .u32CompressLevel = 0, .bVenc = AX_FALSE, .nJencChn = -1, .nRtspChn = -1, .fDetectWidthRatio = 1.0f, .fDetectHeightRatio = 1.0f, .ptRgn = NULL},
};

SAMPLE_CHN_ATTR_T gDualSnsOutChnAttr[] = {
    // ch0 resolution will autofit with sns0 chn0
    {.nWidth = 2688, .nHeight = 1520, .fFramerate = SENSOR_FRAMERATE, .enCompressMode = 2, .u32CompressLevel = 10, .bVenc = AX_TRUE, .nJencChn = 2, .nRtspChn = 0, .fDetectWidthRatio = 2688.0/640.0f, .fDetectHeightRatio = 1520.0/352.0f, .ptRgn = NULL},
    {.nWidth = 720, .nHeight = 576, .fFramerate = SENSOR_FRAMERATE, .enCompressMode = 2, .u32CompressLevel = 4, .bVenc = AX_TRUE, .nJencChn = -1, .nRtspChn = 1, .fDetectWidthRatio = 720.0/640.0f, .fDetectHeightRatio = 576.0/352.0f, .ptRgn = NULL},
    {.nWidth = 640, .nHeight = 352, .fFramerate = SENSOR_FRAMERATE, .enCompressMode = 0, .u32CompressLevel = 0, .bVenc = AX_FALSE, .nJencChn = -1, .nRtspChn = -1, .fDetectWidthRatio = 1.0f, .fDetectHeightRatio = 1.0f, .ptRgn = NULL},
    // ch3 resolution will autofit with sns1 chn0
    {.nWidth = 2688, .nHeight = 1520, .fFramerate = SENSOR_FRAMERATE, .enCompressMode = 2, .u32CompressLevel = 10, .bVenc = AX_TRUE, .nJencChn = 5, .nRtspChn = 2, .fDetectWidthRatio = 2688.0/640.0f, .fDetectHeightRatio = 1520.0/352.0f, .ptRgn = NULL},
    {.nWidth = 720, .nHeight = 576, .fFramerate = SENSOR_FRAMERATE, .enCompressMode = 2, .u32CompressLevel = 4, .bVenc = AX_TRUE, .nJencChn = -1, .nRtspChn = 3, .fDetectWidthRatio = 720.0/640.0f, .fDetectHeightRatio = 576.0/352.0f, .ptRgn = NULL},
    {.nWidth = 640, .nHeight = 352, .fFramerate = SENSOR_FRAMERATE, .enCompressMode = 0, .u32CompressLevel = 0, .bVenc = AX_FALSE, .nJencChn = -1, .nRtspChn = -1, .fDetectWidthRatio = 1.0f, .fDetectHeightRatio = 1.0f, .ptRgn = NULL},
};

#if defined(SC200AI_SUPPORT)
QS_COMMON_SYS_POOL_CFG_T gtSysCommPoolSingleSc200ai[] = {
    {1920, 1080, 1920, AX_FORMAT_YUV420_SEMIPLANAR, 2, 2, 10, 0},
    {1920, 128, 1920, AX_FORMAT_YUV420_SEMIPLANAR, 2, 2, 10, 0},
    {720, 576, 768, AX_FORMAT_YUV420_SEMIPLANAR, 2, 2, 4, 0},
    {640, 352, 640, AX_FORMAT_YUV420_SEMIPLANAR, 2, 0, 0, 0},
};
QS_COMMON_SYS_POOL_CFG_T gtSysCommPoolSingleSc200ai_offline[] = {
    {1920, 1080, 1920, AX_FORMAT_YUV420_SEMIPLANAR, 2, 2, 10, 0},
    {720, 576, 768, AX_FORMAT_YUV420_SEMIPLANAR, 2, 2, 4, 0},
    {640, 352, 640, AX_FORMAT_YUV420_SEMIPLANAR, 2, 0, 0, 0},
};
QS_COMMON_SYS_POOL_CFG_T gtPrivatePoolSingleSc200ai[] = {
    {1920, 1080, 1920, AX_FORMAT_BAYER_RAW_10BPP_PACKED, 4, 2, 4, 0},
};

QS_COMMON_SYS_POOL_CFG_T gtSysCommPoolDoubleSc200ai[] = {
    {1920, 1080, 1920, AX_FORMAT_YUV420_SEMIPLANAR, 4, 2, 10, 0},
    {1920, 128, 1920, AX_FORMAT_YUV420_SEMIPLANAR, 4, 2, 10, 0},
    {720, 576, 768, AX_FORMAT_YUV420_SEMIPLANAR, 4, 2, 4, 0},
    {640, 352, 640, AX_FORMAT_YUV420_SEMIPLANAR, 4, 0, 0, 0},
};
QS_COMMON_SYS_POOL_CFG_T gtSysCommPoolDoubleSc200ai_offline[] = {
    {1920, 1080, 1920, AX_FORMAT_YUV420_SEMIPLANAR, 4, 2, 10, 0},
    {720, 576, 768, AX_FORMAT_YUV420_SEMIPLANAR, 4, 2, 4, 0},
    {640, 352, 640, AX_FORMAT_YUV420_SEMIPLANAR, 4, 0, 0, 0},
};
QS_COMMON_SYS_POOL_CFG_T gtPrivatePoolDoubleSc200ai[] = {
    {1920, 1080, 1920, AX_FORMAT_BAYER_RAW_10BPP_PACKED, 8, 2, 4, 0},
};
#endif

#if defined(SC235HAI_SUPPORT)
QS_COMMON_SYS_POOL_CFG_T gtSysCommPoolSingleSc235hai[] = {
    {1920, 1080, 1920, AX_FORMAT_YUV420_SEMIPLANAR, 2, 2, 10, 0},
    {1920, 128, 1920, AX_FORMAT_YUV420_SEMIPLANAR, 2, 2, 10, 0},
    {720, 576, 768, AX_FORMAT_YUV420_SEMIPLANAR, 2, 2, 4, 0},
    {640, 352, 640, AX_FORMAT_YUV420_SEMIPLANAR, 2, 0, 0, 0},
};
QS_COMMON_SYS_POOL_CFG_T gtSysCommPoolSingleSc235hai_offline[] = {
    {1920, 1080, 1920, AX_FORMAT_YUV420_SEMIPLANAR, 2, 2, 10, 0},
    {720, 576, 768, AX_FORMAT_YUV420_SEMIPLANAR, 2, 2, 4, 0},
    {640, 352, 640, AX_FORMAT_YUV420_SEMIPLANAR, 2, 0, 0, 0},
};

QS_COMMON_SYS_POOL_CFG_T gtPrivatePoolSingleSc235hai[] = {
    {1920, 1080, 1920, AX_FORMAT_BAYER_RAW_10BPP_PACKED, 9, 2, 4, 0},
};

//double
QS_COMMON_SYS_POOL_CFG_T gtSysCommPoolDoubleSc235hai[] = {
    {1920, 1080, 1920, AX_FORMAT_YUV420_SEMIPLANAR, 4, 2, 10, 0},
    {1920, 128, 1920, AX_FORMAT_YUV420_SEMIPLANAR, 2, 2, 10, 0},
    {720, 576, 768, AX_FORMAT_YUV420_SEMIPLANAR, 4, 2, 4, 0},
    {640, 352, 640, AX_FORMAT_YUV420_SEMIPLANAR, 4, 0, 0, 0},
};

QS_COMMON_SYS_POOL_CFG_T gtSysCommPoolDoubleSc235hai_offline[] = {
    {1920, 1080, 1920, AX_FORMAT_YUV420_SEMIPLANAR, 4, 2, 10, 0},
    {720, 576, 768, AX_FORMAT_YUV420_SEMIPLANAR, 4, 2, 4, 0},
    {640, 352, 640, AX_FORMAT_YUV420_SEMIPLANAR, 4, 0, 0, 0},
};

QS_COMMON_SYS_POOL_CFG_T gtPrivatePoolDoubleSc235hai[] = {
    {1920, 1080, 1920, AX_FORMAT_BAYER_RAW_12BPP_PACKED, 8, 2, 4, 2},
};
#endif

#if defined(SC500AI_SUPPORT)
QS_COMMON_SYS_POOL_CFG_T gtSysCommPoolSingleSc500ai[] = {
    {2880, 1620, 2944, AX_FORMAT_YUV420_SEMIPLANAR, 2, 2, 10, 0},
    {2880, 128, 2944, AX_FORMAT_YUV420_SEMIPLANAR, 2, 2, 10, 0},
    {720, 576, 768, AX_FORMAT_YUV420_SEMIPLANAR, 2, 2, 4, 0},
    {640, 352, 640, AX_FORMAT_YUV420_SEMIPLANAR, 2, 0, 0, 0},
};
QS_COMMON_SYS_POOL_CFG_T gtSysCommPoolSingleSc500ai_offline[] = {
    {2880, 1620, 2944, AX_FORMAT_YUV420_SEMIPLANAR, 2, 2, 10, 0},
    {720, 576, 768, AX_FORMAT_YUV420_SEMIPLANAR, 2, 2, 4, 0},
    {640, 352, 640, AX_FORMAT_YUV420_SEMIPLANAR, 2, 0, 0, 0},
};
QS_COMMON_SYS_POOL_CFG_T gtPrivatePoolSingleSc500ai[] = {
    {2880, 1620, 2880, AX_FORMAT_BAYER_RAW_10BPP_PACKED, 4, 2, 4, 0},
};
#endif

#if defined(SC450AI_SUPPORT)
QS_COMMON_SYS_POOL_CFG_T gtSysCommPoolSingleSc450ai[] = {
    {2688, 1520, 2688, AX_FORMAT_YUV420_SEMIPLANAR, 2, 2, 10, 0},
    {2688, 128, 2688, AX_FORMAT_YUV420_SEMIPLANAR, 2, 2, 10, 0},
    {720, 576, 768, AX_FORMAT_YUV420_SEMIPLANAR, 2, 2, 4, 0},
    {640, 352, 640, AX_FORMAT_YUV420_SEMIPLANAR, 2, 0, 0, 0},
};
QS_COMMON_SYS_POOL_CFG_T gtSysCommPoolSingleSc450ai_offline[] = {
    {2688, 1520, 2688, AX_FORMAT_YUV420_SEMIPLANAR, 2, 2, 10, 0},
    {720, 576, 768, AX_FORMAT_YUV420_SEMIPLANAR, 2, 2, 4, 0},
    {640, 352, 640, AX_FORMAT_YUV420_SEMIPLANAR, 2, 0, 0, 0},
};
QS_COMMON_SYS_POOL_CFG_T gtPrivatePoolSingleSc450ai[] = {
    {2688, 1520, 2688, AX_FORMAT_BAYER_RAW_10BPP_PACKED, 4, 2, 4, 0},
};
#endif

#if defined(SC438HAI_SUPPORT)
QS_COMMON_SYS_POOL_CFG_T gtSysCommPoolSingleSc438hai[] = {
    {2688, 1520, 2688, AX_FORMAT_YUV420_SEMIPLANAR, 2, 2, 10, 0},      /* vin nv21/nv21 use */
    {2688, 128, 2688, AX_FORMAT_YUV420_SEMIPLANAR, 2, 2, 10, 0},       /* vin nv21/nv21 use */
    {720, 576, 768, AX_FORMAT_YUV420_SEMIPLANAR, 2, 2, 4, 0},
    {640, 352, 640, AX_FORMAT_YUV420_SEMIPLANAR, 2, 0, 0, 0},          /* no fbc for skel */
};
QS_COMMON_SYS_POOL_CFG_T gtSysCommPoolSingleSc438hai_offline[] = {
    {2688, 1520, 2688, AX_FORMAT_YUV420_SEMIPLANAR, 2, 2, 10, 0},      /* vin nv21/nv21 use */
    {720, 576, 768, AX_FORMAT_YUV420_SEMIPLANAR, 2, 2, 4, 0},
    {640, 352, 640, AX_FORMAT_YUV420_SEMIPLANAR, 2, 0, 0, 0},          /* no fbc for skel */
};
QS_COMMON_SYS_POOL_CFG_T gtPrivatePoolSingleSc438hai[] = {
    {2688, 1520, 2688, AX_FORMAT_BAYER_RAW_10BPP_PACKED, 7, 2, 4, 0},  /* vin raw10 use */
};
//double
QS_COMMON_SYS_POOL_CFG_T gtSysCommPoolDoubleSc438hai[] = {
    {2688, 1520, 2688, AX_FORMAT_YUV420_SEMIPLANAR, 4, 2, 10, 0},      /* vin nv21/nv21 use */
    {2688, 128, 2688, AX_FORMAT_YUV420_SEMIPLANAR, 2, 2, 10, 0},       /* vin nv21/nv21 use */
    {720, 576, 768, AX_FORMAT_YUV420_SEMIPLANAR, 4, 2, 4, 0},
    {640, 352, 640, AX_FORMAT_YUV420_SEMIPLANAR, 4, 0, 0, 0},          /* no fbc for skel */
};
QS_COMMON_SYS_POOL_CFG_T gtSysCommPoolDoubleSc438hai_offline[] = {
    {2688, 1520, 2688, AX_FORMAT_YUV420_SEMIPLANAR, 4, 2, 10, 0},      /* vin nv21/nv21 use */
    {720, 576, 768, AX_FORMAT_YUV420_SEMIPLANAR, 4, 2, 4, 0},
    {640, 352, 640, AX_FORMAT_YUV420_SEMIPLANAR, 4, 0, 0, 0},          /* no fbc for skel */
};
QS_COMMON_SYS_POOL_CFG_T gtPrivatePoolDoubleSc438hai[] = {
    {2688, 1520, 2688, AX_FORMAT_BAYER_RAW_10BPP_PACKED, 8, 2, 4, 2},  /* vin raw10 use */
};
#endif

#if defined(SC533HAI_SUPPORT)
QS_COMMON_SYS_POOL_CFG_T gtSysCommPoolSingleSc533hai[] = {
    {2880, 1620, 2880, AX_FORMAT_YUV420_SEMIPLANAR, 2, 2, 10, 0},
    {2880, 128, 2880, AX_FORMAT_YUV420_SEMIPLANAR, 2, 2, 10, 0},
    {720, 576, 768, AX_FORMAT_YUV420_SEMIPLANAR, 2, 2, 4, 0},
    {640, 352, 640, AX_FORMAT_YUV420_SEMIPLANAR, 2, 0, 0, 0},
};
QS_COMMON_SYS_POOL_CFG_T gtSysCommPoolSingleSc533hai_offline[] = {
    {2880, 1620, 2880, AX_FORMAT_YUV420_SEMIPLANAR, 2, 2, 10, 0},
    {720, 576, 768, AX_FORMAT_YUV420_SEMIPLANAR, 2, 2, 4, 0},
    {640, 352, 640, AX_FORMAT_YUV420_SEMIPLANAR, 2, 0, 0, 0},
};
QS_COMMON_SYS_POOL_CFG_T gtPrivatePoolSingleSc533hai[] = {
    {2880, 1620, 2880, AX_FORMAT_BAYER_RAW_10BPP_PACKED, 9, 2, 4, 0},
};
#endif

#if defined(OS04D10_SUPPORT)
QS_COMMON_SYS_POOL_CFG_T gtSysCommPoolSingleOs04d10[] = {
    {2560, 1440, 2560, AX_FORMAT_YUV420_SEMIPLANAR, 2, 2, 10, 0},
    {2560, 128, 2560, AX_FORMAT_YUV420_SEMIPLANAR, 2, 2, 10, 0},
    {720, 576, 768, AX_FORMAT_YUV420_SEMIPLANAR, 2, 2, 4, 0},
    {640, 352, 640, AX_FORMAT_YUV420_SEMIPLANAR, 2, 0, 0, 0},
};

QS_COMMON_SYS_POOL_CFG_T gtSysCommPoolSingleOs04d10_offline[] = {
    {2560, 1440, 2560, AX_FORMAT_YUV420_SEMIPLANAR, 2, 2, 10, 0},
    {720, 576, 768, AX_FORMAT_YUV420_SEMIPLANAR, 2, 2, 4, 0},
    {640, 352, 640, AX_FORMAT_YUV420_SEMIPLANAR, 2, 0, 0, 0},
};

QS_COMMON_SYS_POOL_CFG_T gtPrivatePoolSingleOs04d10[] = {
    {2560, 1440, 2560, AX_FORMAT_BAYER_RAW_10BPP_PACKED, 4, 2, 4, 0},
};

//double
QS_COMMON_SYS_POOL_CFG_T gtSysCommPoolDoubleOs04d10[] = {
    {2560, 1440, 2560, AX_FORMAT_YUV420_SEMIPLANAR, 4, 2, 10, 0},
    {2560, 128, 2560, AX_FORMAT_YUV420_SEMIPLANAR, 2, 2, 10, 0},
    {720, 576, 768, AX_FORMAT_YUV420_SEMIPLANAR, 4, 2, 4, 0},
    {640, 352, 640, AX_FORMAT_YUV420_SEMIPLANAR, 4, 0, 0, 0},
};

QS_COMMON_SYS_POOL_CFG_T gtSysCommPoolDoubleOs04d10_offline[] = {
    {2560, 1440, 2560, AX_FORMAT_YUV420_SEMIPLANAR, 4, 2, 10, 0},
    {720, 576, 768, AX_FORMAT_YUV420_SEMIPLANAR, 4, 2, 4, 0},
    {640, 352, 640, AX_FORMAT_YUV420_SEMIPLANAR, 4, 0, 0, 0},
};

QS_COMMON_SYS_POOL_CFG_T gtPrivatePoolDoubleOs04d10[] = {
    {2560, 1440, 2560, AX_FORMAT_BAYER_RAW_10BPP_PACKED, 8, 2, 4, 2},
};
#endif

#if defined(OS04A10_SUPPORT)
QS_COMMON_SYS_POOL_CFG_T gtSysCommPoolSingleOs04a10[] = {
    {2688, 1520, 2688, AX_FORMAT_YUV420_SEMIPLANAR, 2, 2, 10, 0},
    {2688, 128, 2688, AX_FORMAT_YUV420_SEMIPLANAR, 2, 2, 10, 0},
    {720, 576, 768, AX_FORMAT_YUV420_SEMIPLANAR, 2, 2, 4, 0},
    {640, 352, 640, AX_FORMAT_YUV420_SEMIPLANAR, 2, 0, 0, 0},
};

QS_COMMON_SYS_POOL_CFG_T gtSysCommPoolSingleOs04a10_offline[] = {
    {2688, 1520, 2688, AX_FORMAT_YUV420_SEMIPLANAR, 2, 2, 10, 0},
    {720, 576, 768, AX_FORMAT_YUV420_SEMIPLANAR, 2, 2, 4, 0},
    {640, 352, 640, AX_FORMAT_YUV420_SEMIPLANAR, 2, 0, 0, 0},
};

QS_COMMON_SYS_POOL_CFG_T gtPrivatePoolSingleOs04a10[] = {
    {2688, 1520, 2688, AX_FORMAT_BAYER_RAW_10BPP_PACKED, 4, 2, 4, 0},
};

QS_COMMON_SYS_POOL_CFG_T gtSysCommPoolDoubleOs04a10[] = {
    {2688, 1520, 2688, AX_FORMAT_YUV420_SEMIPLANAR, 4, 2, 10, 0},
    {2688, 128, 2688, AX_FORMAT_YUV420_SEMIPLANAR, 4, 2, 10, 0},
    {720, 576, 768, AX_FORMAT_YUV420_SEMIPLANAR, 4, 0, 0, 0},
    {640, 352, 640, AX_FORMAT_YUV420_SEMIPLANAR, 4, 0, 0, 0},
};

QS_COMMON_SYS_POOL_CFG_T gtSysCommPoolDoubleOs04a10_offline[] = {
    {2688, 1520, 2688, AX_FORMAT_YUV420_SEMIPLANAR, 4, 2, 10, 0},
    {720, 576, 768, AX_FORMAT_YUV420_SEMIPLANAR, 4, 0, 0, 0},
    {640, 352, 640, AX_FORMAT_YUV420_SEMIPLANAR, 4, 0, 0, 0},
};

QS_COMMON_SYS_POOL_CFG_T gtPrivatePoolDoubleOs04a10[] = {
    {2688, 1520, 2688, AX_FORMAT_BAYER_RAW_10BPP_PACKED, 8, 2, 4, 0},
};
#endif

#if defined(MIS20S1_SUPPORT)
QS_COMMON_SYS_POOL_CFG_T gtSysCommPoolSingleMis20s1[] = {
    {1920, 1080, 1920, AX_FORMAT_YUV420_SEMIPLANAR, 2, 2, 10, 0},
    {1920, 128, 1920, AX_FORMAT_YUV420_SEMIPLANAR, 1, 2, 10, 0},
    {720, 576, 768, AX_FORMAT_YUV420_SEMIPLANAR, 2, 2, 4, 0},
    {640, 352, 640, AX_FORMAT_YUV420_SEMIPLANAR, 2, 0, 0, 0},
};

QS_COMMON_SYS_POOL_CFG_T gtSysCommPoolSingleMis20s1_offline[] = {
    {1920, 1080, 1920, AX_FORMAT_YUV420_SEMIPLANAR, 2, 2, 10, 0},
    {720, 576, 768, AX_FORMAT_YUV420_SEMIPLANAR, 2, 2, 4, 0},
    {640, 352, 640, AX_FORMAT_YUV420_SEMIPLANAR, 2, 0, 0, 0},
};

QS_COMMON_SYS_POOL_CFG_T gtPrivatePoolSingleMis20s1[] = {
    {1920, 1080, 1920, AX_FORMAT_BAYER_RAW_12BPP_PACKED, 9, 2, 4, 0},
};
//double
QS_COMMON_SYS_POOL_CFG_T gtSysCommPoolDoubleMis20s1[] = {
    {1920, 1080, 1920, AX_FORMAT_YUV420_SEMIPLANAR, 4, 2, 10, 0},
    {1920, 128, 1920, AX_FORMAT_YUV420_SEMIPLANAR, 2, 2, 10, 0},
    {720, 576, 768, AX_FORMAT_YUV420_SEMIPLANAR, 4, 2, 4, 0},
    {640, 352, 640, AX_FORMAT_YUV420_SEMIPLANAR, 4, 0, 0, 0},
};

QS_COMMON_SYS_POOL_CFG_T gtSysCommPoolDoubleMis20s1_offline[] = {
    {1920, 1080, 1920, AX_FORMAT_YUV420_SEMIPLANAR, 4, 2, 10, 0},
    {720, 576, 768, AX_FORMAT_YUV420_SEMIPLANAR, 4, 2, 4, 0},
    {640, 352, 640, AX_FORMAT_YUV420_SEMIPLANAR, 4, 0, 0, 0},
};

QS_COMMON_SYS_POOL_CFG_T gtPrivatePoolDoubleMis20s1[] = {
    {1920, 1080, 1920, AX_FORMAT_BAYER_RAW_12BPP_PACKED, 8, 2, 4, 2},
};
#endif

#if defined(SENSOR_DUMMY_SUPPORT)
QS_COMMON_SYS_POOL_CFG_T gtSysCommPoolSingleDummy[] = {
    {2688, 1520, 2688, AX_FORMAT_YUV420_SEMIPLANAR, 2, 2, 10, 0},      /* vin nv21/nv21 use */
    {2688, 128, 2688, AX_FORMAT_YUV420_SEMIPLANAR, 2, 2, 10, 0},       /* vin nv21/nv21 use */
    {720, 576, 768, AX_FORMAT_YUV420_SEMIPLANAR, 2, 2, 4, 0},
    {640, 352, 640, AX_FORMAT_YUV420_SEMIPLANAR, 2, 0, 0, 0},          /* no fbc for skel */
};

QS_COMMON_SYS_POOL_CFG_T gtSysCommPoolSingleDummy_offline[] = {
    {2688, 1520, 2688, AX_FORMAT_YUV420_SEMIPLANAR, 2, 2, 10, 0},      /* vin nv21/nv21 use */
    {720, 576, 768, AX_FORMAT_YUV420_SEMIPLANAR, 2, 2, 4, 0},
    {640, 352, 640, AX_FORMAT_YUV420_SEMIPLANAR, 2, 0, 0, 0},          /* no fbc for skel */
};
QS_COMMON_SYS_POOL_CFG_T gtPrivatePoolSingleDummy[] = {
    {2688, 1520, 2688, AX_FORMAT_BAYER_RAW_10BPP_PACKED, 4, 2, 4, 0},  /* vin raw10 use */
};
#endif

#if defined(SC465SL_SUPPORT)
QS_COMMON_SYS_POOL_CFG_T gtSysCommPoolSingleSc465sl[] = {
    {2560, 1440, 2560, AX_FORMAT_YUV420_SEMIPLANAR, 2, 2, 10, 0},
    {2560, 128, 2560, AX_FORMAT_YUV420_SEMIPLANAR, 2, 2, 10, 0},
    {720, 576, 768, AX_FORMAT_YUV420_SEMIPLANAR, 2, 2, 4, 0},
    {640, 352, 640, AX_FORMAT_YUV420_SEMIPLANAR, 2, 0, 0, 0},
};

QS_COMMON_SYS_POOL_CFG_T gtSysCommPoolSingleSc465sl_offline[] = {
    {2560, 1440, 2560, AX_FORMAT_YUV420_SEMIPLANAR, 2, 2, 10, 0},
    {720, 576, 768, AX_FORMAT_YUV420_SEMIPLANAR, 2, 2, 4, 0},
    {640, 352, 640, AX_FORMAT_YUV420_SEMIPLANAR, 2, 0, 0, 0},
};

QS_COMMON_SYS_POOL_CFG_T gtPrivatePoolSingleSc465sl[] = {
    {2560, 1440, 2560, AX_FORMAT_BAYER_RAW_10BPP_PACKED, 7, 2, 4, 0},
};
//double
QS_COMMON_SYS_POOL_CFG_T gtSysCommPoolDoubleSc465sl[] = {
    {2560, 1440, 2560, AX_FORMAT_YUV420_SEMIPLANAR, 4, 2, 10, 0},
    {2560, 128, 2560, AX_FORMAT_YUV420_SEMIPLANAR, 2, 2, 10, 0},
    {720, 576, 768, AX_FORMAT_YUV420_SEMIPLANAR, 4, 2, 4, 0},
    {640, 352, 640, AX_FORMAT_YUV420_SEMIPLANAR, 4, 0, 0, 0},
};

QS_COMMON_SYS_POOL_CFG_T gtSysCommPoolDoubleSc465sl_offline[] = {
    {2560, 1440, 2560, AX_FORMAT_YUV420_SEMIPLANAR, 4, 2, 10, 0},
    {720, 576, 768, AX_FORMAT_YUV420_SEMIPLANAR, 4, 2, 4, 0},
    {640, 352, 640, AX_FORMAT_YUV420_SEMIPLANAR, 4, 0, 0, 0},
};

QS_COMMON_SYS_POOL_CFG_T gtPrivatePoolDoubleSc465sl[] = {
    {2560, 1440, 2560, AX_FORMAT_BAYER_RAW_10BPP_PACKED, 8, 2, 4, 2},
};
#endif


#if defined(USE_STATIC_LIB)

#if defined(SC200AI_SUPPORT)
    #if defined(AX_FAST_BOOT)
        extern AX_SENSOR_REGISTER_FUNC_T gSnssc200aiObjQs;
    #endif
    extern AX_SENSOR_REGISTER_FUNC_T gSnssc200aiObj;
#endif

#if defined(SC235HAI_SUPPORT)
    #if defined(AX_FAST_BOOT)
        extern AX_SENSOR_REGISTER_FUNC_T gSnssc235haiObjQs;
    #endif
    extern AX_SENSOR_REGISTER_FUNC_T gSnssc235haiObj;
#endif

#if defined(SC500HAI_SUPPORT)
    #if defined(AX_FAST_BOOT)
        extern AX_SENSOR_REGISTER_FUNC_T gSnssc500aiObjQs;
    #endif
    extern AX_SENSOR_REGISTER_FUNC_T gSnssc500aiObj;
#endif

#if defined(SC450AI_SUPPORT)
    #if defined(AX_FAST_BOOT)
        extern AX_SENSOR_REGISTER_FUNC_T gSnssc450aiObjQs;
    #endif
    extern AX_SENSOR_REGISTER_FUNC_T gSnssc450aiObj;
#endif

#if defined(OS04D10_SUPPORT)
    #if defined(AX_FAST_BOOT)
        extern AX_SENSOR_REGISTER_FUNC_T gSnsos04d10ObjQs;
    #endif
    extern AX_SENSOR_REGISTER_FUNC_T gSnsos04d10Obj;
#endif

#if defined(OS04A10_SUPPORT)
    #if defined(AX_FAST_BOOT)
        extern AX_SENSOR_REGISTER_FUNC_T gSnsos04a10ObjQs;
    #endif
    extern AX_SENSOR_REGISTER_FUNC_T gSnsos04a10Obj;
#endif

#if defined(SC438HAI_SUPPORT)
    #if defined(AX_FAST_BOOT)
        extern AX_SENSOR_REGISTER_FUNC_T gSnssc438haiObjQs;
    #endif
    extern AX_SENSOR_REGISTER_FUNC_T gSnssc438haiObj;
#endif

#if defined(SC533HAI_SUPPORT)
    #if defined(AX_FAST_BOOT)
        extern AX_SENSOR_REGISTER_FUNC_T gSnssc533haiObjQs;
    #endif
    extern AX_SENSOR_REGISTER_FUNC_T gSnssc533haiObj;
#endif

#if defined(MIS20S1_SUPPORT)
    #if defined(AX_FAST_BOOT)
        extern AX_SENSOR_REGISTER_FUNC_T gSnsmis20s1ObjQs;
    #endif
    extern AX_SENSOR_REGISTER_FUNC_T gSnsmis20s1Obj;
#endif

#if defined(SENSOR_DUMMY_SUPPORT)
    #if defined(AX_FAST_BOOT)
        extern AX_SENSOR_REGISTER_FUNC_T gSnsdummyObjQs;
    #endif
    extern AX_SENSOR_REGISTER_FUNC_T gSnsdummyObj;
#endif

#if defined(SC465SL_SUPPORT)
    #if defined(AX_FAST_BOOT)
        extern AX_SENSOR_REGISTER_FUNC_T gSnssc465slObjQs;
    #endif
    extern AX_SENSOR_REGISTER_FUNC_T gSnssc465slObj;
#endif

static AX_S32 QSDEMO_VIN_GetSnsObj(QS_SAMPLE_SNS_TYPE_E eSnsType, AX_SENSOR_REGISTER_FUNC_T *ptSnsHdl[2])
{
    switch (eSnsType) {
    case OMNIVISION_OS04A10:
#if defined(OS04A10_SUPPORT)
        ptSnsHdl[0] = &gSnsos04a10Obj;
        #if defined(AX_FAST_BOOT)
        ptSnsHdl[1] = &gSnsos04a10ObjQS;
        #endif
#endif
        break;
    case OMNIVISION_OS04D10:
#if defined(OS04D10_SUPPORT)
        ptSnsHdl[0] = &gSnsos04d10Obj;
        #if defined(AX_FAST_BOOT)
        ptSnsHdl[1] = &gSnsos04d10ObjQs;
        #endif
#endif
        break;
    case SMARTSENS_SC450AI:
#if defined(SC450AI_SUPPORT)
        ptSnsHdl[0] = &gSnssc450aiObj;
        #if defined(AX_FAST_BOOT)
        ptSnsHdl[1] = &gSnssc450aiObjQs;
         #endif
#endif
        break;
    case SMARTSENS_SC200AI:
#if defined(SC200AI_SUPPORT)
        ptSnsHdl[0] = &gSnssc200aiObj;
        #if defined(AX_FAST_BOOT)
        ptSnsHdl[1] = &gSnssc200aiObjQs;
        #endif
#endif
        break;
    case SMARTSENS_SC500AI:
#if defined(SC500AI_SUPPORT)
        ptSnsHdl[0] = &gSnssc500aiObj;
        #if defined(AX_FAST_BOOT)
        ptSnsHdl[1] = &gSnssc500aiObjQs;
        #endif
#endif
        break;
    case SMARTSENS_SC235HAI:
#if defined(SC235HAI_SUPPORT)
        ptSnsHdl[0] = &gSnssc235haiObj;
        #if defined(AX_FAST_BOOT)
        ptSnsHdl[1] = &gSnssc235haiObjQs;
        #endif
#endif
        break;
    case SMARTSENS_SC438HAI:
#if defined(SC438HAI_SUPPORT)
        ptSnsHdl[0] = &gSnssc438haiObj;
        #if defined(AX_FAST_BOOT)
        ptSnsHdl[1] = &gSnssc438haiObjQs;
        #endif
#endif
        break;
    case SMARTSENS_SC533HAI:
#if defined(SC533HAI_SUPPORT)
        ptSnsHdl[0] = &gSnssc533haiObj;
        #if defined(AX_FAST_BOOT)
        ptSnsHdl[1] = &gSnssc533haiObjQs;
        #endif
#endif
        break;
    case IMAGEDESIGN_MIS20S1:
#if defined(MIS20S1_SUPPORT)
        ptSnsHdl[0] = &gSnsmis20s1Obj;
        #if defined(AX_FAST_BOOT)
        ptSnsHdl[1] = &gSnsmis20s1ObjQs;
        #endif
#endif
        break;
    case SENSOR_DUMMY:
#if defined(SENSOR_DUMMY_SUPPORT)
        ptSnsHdl[0] = &gSnsdummyObj;
        #if defined(AX_FAST_BOOT)
        ptSnsHdl[1] = &gSnsdummyObjQs;
        #endif
#endif
        break;
     case SMARTSENS_SC465SL:
#if defined(SC465SL_SUPPORT)
        ptSnsHdl[0] = &gSnssc465slObj;
        #if defined(AX_FAST_BOOT)
        ptSnsHdl[1] = &gSnssc465slObjQs;
        #endif
#endif
        break;
    default:
#if defined(SC438HAI_SUPPORT)
        ptSnsHdl[0] = &gSnssc438haiObj;
        #if defined(AX_FAST_BOOT)
        ptSnsHdl[1] = &gSnssc438haiObjQs;
        #endif
#endif
        break;
    }
    return 0;
}
#else
static AX_S32 QS_GetSnsLibObj(AX_SENSOR_LIB_INF * pSnsLib, AX_SENSOR_REGISTER_FUNC_T *ptSnsHdl[2])
{
    void *handler = NULL;
    AX_CHAR *err = NULL;
    AX_CHAR path[128] = AX_LIB_SENSOR_PATH;

    strncat(path, pSnsLib->szlibName, sizeof(path) - strlen(AX_LIB_SENSOR_PATH));

    handler = dlopen((void *)path, RTLD_LAZY);
    if (NULL == handler) {
        ALOGI("open %s fail!---%s", path, dlerror());
        return -1;
    }
    ptSnsHdl[0] = (AX_SENSOR_REGISTER_FUNC_T *)dlsym(handler, pSnsLib->szObjName);
    err = dlerror();
    if (NULL != err) {
        ptSnsHdl[0] = NULL;
        ALOGI("dlsym %s fail!", pSnsLib->szObjName);
    }

#if defined(AX_FAST_BOOT)
    ptSnsHdl[1] = (AX_SENSOR_REGISTER_FUNC_T *)dlsym(handler, pSnsLib->szObjQsName);
    err = dlerror();
    if (NULL != err) {
        ptSnsHdl[1] = NULL;
        ALOGI("dlsym %s fail!", pSnsLib->szObjQsName);
    }
#endif

    return 0;
}

static AX_S32 QSDEMO_VIN_GetSnsObj(QS_SAMPLE_SNS_TYPE_E eSnsType, AX_SENSOR_REGISTER_FUNC_T *ptSnsHdl[2])
{
    ptSnsHdl[0] = AX_NULL;
    ptSnsHdl[1] = AX_NULL;

    switch (eSnsType) {
    case OMNIVISION_OS04A10:
#if defined(OS04A10_SUPPORT)
        return QS_GetSnsLibObj(&gOs04a10SnsLib, ptSnsHdl);
#endif
        break;
    case OMNIVISION_OS04D10:
#if defined(OS04D10_SUPPORT)
        return QS_GetSnsLibObj(&gOs04d10SnsLib, ptSnsHdl);
#endif
        break;
    case SMARTSENS_SC450AI:
#if defined(SC450AI_SUPPORT)
        return QS_GetSnsLibObj(&gSc450aiSnsLib, ptSnsHdl);
#endif
        break;
    case SMARTSENS_SC200AI:
#if defined(SC200AI_SUPPORT)
        return QS_GetSnsLibObj(&gSc200aiSnsLib, ptSnsHdl);
#endif
        break;
    case SMARTSENS_SC500AI:
#if defined(SC500AI_SUPPORT)
        return QS_GetSnsLibObj(&gSc500aiSnsLib, ptSnsHdl);
#endif
        break;
    case SMARTSENS_SC235HAI:
#if defined(SC235HAI_SUPPORT)
        return QS_GetSnsLibObj(&gSc235haiSnsLib, ptSnsHdl);
#endif
        break;
    case SMARTSENS_SC438HAI:
#if defined(SC438HAI_SUPPORT)
        return QS_GetSnsLibObj(&gSc438haiSnsLib, ptSnsHdl);
#endif
        break;
    case SMARTSENS_SC533HAI:
#if defined(SC533HAI_SUPPORT)
        return QS_GetSnsLibObj(&gSc533haiSnsLib, ptSnsHdl);
#endif
        break;
    case IMAGEDESIGN_MIS20S1:
#if defined(MIS20S1_SUPPORT)
        return QS_GetSnsLibObj(&gMis20s1SnsLib, ptSnsHdl);
#endif
        break;
    case SENSOR_DUMMY:
#if defined(SENSOR_DUMMY_SUPPORT)
        return QS_GetSnsLibObj(&gDummySnsLib, ptSnsHdl);
#endif
        break;
    case SMARTSENS_SC465SL:
#if defined(SC465SL_SUPPORT)
        return QS_GetSnsLibObj(&gSc465slSnsLib, ptSnsHdl);
#endif
        break;
    default:
#if defined(SC438HAI_SUPPORT)
        return QS_GetSnsLibObj(&gSc438haiSnsLib, ptSnsHdl);
#endif
        break;
    }
    return 0;
}

#endif // USE_STATIC_LIB

AX_S32 QSDEMO_VIN_GetSnsConfig(QS_SAMPLE_SNS_TYPE_E eSnsType,
                               AX_MIPI_RX_ATTR_T *ptMipiAttr, AX_SNS_ATTR_T *ptSnsAttr,
                               QS_AX_SNS_CLK_ATTR_T *ptSnsClkAttr, AX_VIN_DEV_ATTR_T *pDevAttr,
                               AX_VIN_PIPE_ATTR_T *pPipeAttr, AX_VIN_CHN_ATTR_T *pChnAttr)
{
    if (eSnsType == SMARTSENS_SC200AI) {
        #if defined(SC200AI_SUPPORT)
        memcpy(ptMipiAttr, &gSc200aiMipiAttr, sizeof(AX_MIPI_RX_ATTR_T));
        memcpy(ptSnsAttr, &gSc200aiSnsAttr, sizeof(AX_SNS_ATTR_T));
        memcpy(ptSnsClkAttr, &gSc200aiSnsClkAttr, sizeof(QS_AX_SNS_CLK_ATTR_T));
        memcpy(pDevAttr, &gSc200aiDevAttr, sizeof(AX_VIN_DEV_ATTR_T));
        memcpy(pPipeAttr, &gSc200aiPipeAttr, sizeof(AX_VIN_PIPE_ATTR_T));
        memcpy(&pChnAttr[0], &gSc200aiChn0Attr, sizeof(AX_VIN_CHN_ATTR_T));
        #endif
    } else if (eSnsType == SMARTSENS_SC235HAI) {
        #if defined(SC235HAI_SUPPORT)
        memcpy(ptMipiAttr, &gSc235haiMipiAttr, sizeof(AX_MIPI_RX_ATTR_T));
        memcpy(ptSnsAttr, &gSc235haiSnsAttr, sizeof(AX_SNS_ATTR_T));
        memcpy(ptSnsClkAttr, &gSc235haiSnsClkAttr, sizeof(QS_AX_SNS_CLK_ATTR_T));
        memcpy(pDevAttr, &gSc235haiDevAttr, sizeof(AX_VIN_DEV_ATTR_T));
        memcpy(pPipeAttr, &gSc235haiPipeAttr, sizeof(AX_VIN_PIPE_ATTR_T));
        memcpy(&pChnAttr[0], &gSc235haiChn0Attr, sizeof(AX_VIN_CHN_ATTR_T));
        #endif
    } else if (eSnsType == SMARTSENS_SC500AI) {
        #if defined(SC500AI_SUPPORT)
        memcpy(ptMipiAttr, &gSc500aiMipiAttr, sizeof(AX_MIPI_RX_ATTR_T));
        memcpy(ptSnsAttr, &gSc500aiSnsAttr, sizeof(AX_SNS_ATTR_T));
        memcpy(ptSnsClkAttr, &gSc500aiSnsClkAttr, sizeof(QS_AX_SNS_CLK_ATTR_T));
        memcpy(pDevAttr, &gSc500aiDevAttr, sizeof(AX_VIN_DEV_ATTR_T));
        memcpy(pPipeAttr, &gSc500aiPipeAttr, sizeof(AX_VIN_PIPE_ATTR_T));
        memcpy(&pChnAttr[0], &gSc500aiChn0Attr, sizeof(AX_VIN_CHN_ATTR_T));
        #endif
    } else if (eSnsType == SMARTSENS_SC450AI) {
        #if defined(SC450AI_SUPPORT)
        memcpy(ptMipiAttr, &gSc450aiMipiAttr, sizeof(AX_MIPI_RX_ATTR_T));
        memcpy(ptSnsAttr, &gSc450aiSnsAttr, sizeof(AX_SNS_ATTR_T));
        memcpy(ptSnsClkAttr, &gSc450aiSnsClkAttr, sizeof(QS_AX_SNS_CLK_ATTR_T));
        memcpy(pDevAttr, &gSc450aiDevAttr, sizeof(AX_VIN_DEV_ATTR_T));
        memcpy(pPipeAttr, &gSc450aiPipeAttr, sizeof(AX_VIN_PIPE_ATTR_T));
        memcpy(&pChnAttr[0], &gSc450aiChn0Attr, sizeof(AX_VIN_CHN_ATTR_T));
        #endif
    } else if (eSnsType == SMARTSENS_SC438HAI) {
        #if defined(SC438HAI_SUPPORT)
        memcpy(ptMipiAttr, &gSc438haiMipiAttr, sizeof(AX_MIPI_RX_ATTR_T));
        memcpy(ptSnsAttr, &gSc438haiSnsAttr, sizeof(AX_SNS_ATTR_T));
        memcpy(ptSnsClkAttr, &gSc438haiSnsClkAttr, sizeof(QS_AX_SNS_CLK_ATTR_T));
        memcpy(pDevAttr, &gSc438haiDevAttr, sizeof(AX_VIN_DEV_ATTR_T));
        memcpy(pPipeAttr, &gSc438haiPipeAttr, sizeof(AX_VIN_PIPE_ATTR_T));
        memcpy(&pChnAttr[0], &gSc438haiChn0Attr, sizeof(AX_VIN_CHN_ATTR_T));
        #endif
    } else if (eSnsType == SMARTSENS_SC533HAI) {
        #if defined(SC533HAI_SUPPORT)
        memcpy(ptMipiAttr, &gSc533haiMipiAttr, sizeof(AX_MIPI_RX_ATTR_T));
        memcpy(ptSnsAttr, &gSc533haiSnsAttr, sizeof(AX_SNS_ATTR_T));
        memcpy(ptSnsClkAttr, &gSc533haiSnsClkAttr, sizeof(QS_AX_SNS_CLK_ATTR_T));
        memcpy(pDevAttr, &gSc533haiDevAttr, sizeof(AX_VIN_DEV_ATTR_T));
        memcpy(pPipeAttr, &gSc533haiPipeAttr, sizeof(AX_VIN_PIPE_ATTR_T));
        memcpy(&pChnAttr[0], &gSc533haiChn0Attr, sizeof(AX_VIN_CHN_ATTR_T));
        #endif
    } else if (eSnsType == OMNIVISION_OS04D10) {
        #if defined(OS04D10_SUPPORT)
        memcpy(ptMipiAttr, &gOs04d10MipiAttr, sizeof(AX_MIPI_RX_ATTR_T));
        memcpy(ptSnsAttr, &gOs04d10SnsAttr, sizeof(AX_SNS_ATTR_T));
        memcpy(ptSnsClkAttr, &gOs04d10SnsClkAttr, sizeof(QS_AX_SNS_CLK_ATTR_T));
        memcpy(pDevAttr, &gOs04d10DevAttr, sizeof(AX_VIN_DEV_ATTR_T));
        memcpy(pPipeAttr, &gOs04d10PipeAttr, sizeof(AX_VIN_PIPE_ATTR_T));
        memcpy(&pChnAttr[0], &gOs04d10Chn0Attr, sizeof(AX_VIN_CHN_ATTR_T));
        #endif
    } else if (eSnsType == OMNIVISION_OS04A10) {
        #if defined(OS04A10_SUPPORT)
        memcpy(ptMipiAttr, &gOs04a10MipiAttr, sizeof(AX_MIPI_RX_ATTR_T));
        memcpy(ptSnsAttr, &gOs04a10SnsAttr, sizeof(AX_SNS_ATTR_T));
        memcpy(ptSnsClkAttr, &gOs04a10SnsClkAttr, sizeof(QS_AX_SNS_CLK_ATTR_T));
        memcpy(pDevAttr, &gOs04a10DevAttr, sizeof(AX_VIN_DEV_ATTR_T));
        memcpy(pPipeAttr, &gOs04a10PipeAttr, sizeof(AX_VIN_PIPE_ATTR_T));
        memcpy(&pChnAttr[0], &gOs04a10Chn0Attr, sizeof(AX_VIN_CHN_ATTR_T));
        #endif
    } else if (eSnsType == IMAGEDESIGN_MIS20S1) {
        #if defined(MIS20S1_SUPPORT)
        memcpy(ptMipiAttr, &gMis20s1MipiAttr, sizeof(AX_MIPI_RX_ATTR_T));
        memcpy(ptSnsAttr, &gMis20s1SnsAttr, sizeof(AX_SNS_ATTR_T));
        memcpy(ptSnsClkAttr, &gMis20s1SnsClkAttr, sizeof(QS_AX_SNS_CLK_ATTR_T));
        memcpy(pDevAttr, &gMis20s1DevAttr, sizeof(AX_VIN_DEV_ATTR_T));
        memcpy(pPipeAttr, &gMis20s1PipeAttr, sizeof(AX_VIN_PIPE_ATTR_T));
        memcpy(&pChnAttr[0], &gMis20s1Chn0Attr, sizeof(AX_VIN_CHN_ATTR_T));
        #endif
    } else if (eSnsType == SENSOR_DUMMY) {
        #if defined(SENSOR_DUMMY_SUPPORT)
        memcpy(ptMipiAttr, &gDummyMipiAttr, sizeof(AX_MIPI_RX_ATTR_T));
        memcpy(ptSnsAttr, &gDummySnsAttr, sizeof(AX_SNS_ATTR_T));
        memcpy(ptSnsClkAttr, &gDummySnsClkAttr, sizeof(QS_AX_SNS_CLK_ATTR_T));
        memcpy(pDevAttr, &gDummyDevAttr, sizeof(AX_VIN_DEV_ATTR_T));
        memcpy(pPipeAttr, &gDummyPipeAttr, sizeof(AX_VIN_PIPE_ATTR_T));
        memcpy(&pChnAttr[0], &gDummyChn0Attr, sizeof(AX_VIN_CHN_ATTR_T));
        #endif
    } else if (eSnsType == SMARTSENS_SC465SL) {
        #if defined(SC465SL_SUPPORT)
        memcpy(ptMipiAttr, &gSc465slMipiAttr, sizeof(AX_MIPI_RX_ATTR_T));
        memcpy(ptSnsAttr, &gSc465slSnsAttr, sizeof(AX_SNS_ATTR_T));
        memcpy(ptSnsClkAttr, &gSc465slSnsClkAttr, sizeof(QS_AX_SNS_CLK_ATTR_T));
        memcpy(pDevAttr, &gSc465slDevAttr, sizeof(AX_VIN_DEV_ATTR_T));
        memcpy(pPipeAttr, &gSc465slPipeAttr, sizeof(AX_VIN_PIPE_ATTR_T));
        memcpy(&pChnAttr[0], &gSc465slChn0Attr, sizeof(AX_VIN_CHN_ATTR_T));
        #endif
    }else {
        return -1;
    }

    return 0;
}

static AX_VOID __set_pipe_hdr_mode(AX_U32 *pHdrSel, AX_SNS_HDR_MODE_E eHdrMode)
{
    if (NULL == pHdrSel) {
        return;
    }

    switch (eHdrMode) {
    case AX_SNS_LINEAR_MODE:
    case AX_SNS_LINEAR_ONLY_MODE:
        *pHdrSel = 0x1;
        break;

    case AX_SNS_HDR_2X_MODE:
        *pHdrSel = 0x1 | 0x2;
        break;

    default:
        *pHdrSel = 0x1;
        break;
    }
}

static AX_VOID __set_vin_attr(QS_AX_CAMERA_T *pCam, QS_SAMPLE_SNS_TYPE_E eSnsType, AX_SNS_HDR_MODE_E eHdrMode,
                              QS_COMMON_VIN_MODE_E eSysMode, AX_BOOL bAiispEnable, SAMPLE_ENTRY_PARAM_T *pEntryParam)
{
    pCam->eSnsType = eSnsType;
    pCam->tSnsAttr.eSnsMode = eHdrMode;
    pCam->tDevAttr.eSnsMode = eHdrMode;
    pCam->eHdrMode = eHdrMode;
    pCam->eSysMode = eSysMode;
    pCam->tPipeAttr.eSnsMode = eHdrMode;
    pCam->tPipeAttr.bAiIspEnable = bAiispEnable;
    if (IS_SNS_HDR_MODE(eHdrMode)) {
        pCam->tDevAttr.eSnsOutputMode = AX_SNS_DOL_HDR;
    }

    if (QS_COMMON_VIN_TPG == eSysMode) {
        pCam->tDevAttr.eSnsIntfType = AX_SNS_INTF_TYPE_TPG;
    }
    if (pCam->tMipiAttr.eLaneNum == AX_MIPI_DATA_LANE_4) {
        pCam->eLaneComboMode = AX_LANE_COMBO_MODE_0;
    } else if (pCam->nRxDev == 0) {
        pCam->eLaneComboMode = AX_LANE_COMBO_MODE_2;
    } else {
        pCam->eLaneComboMode = AX_LANE_COMBO_MODE_1;
    }
    pCam->bChnEn[0] = (pEntryParam->bGdcEnable == 0) ? AX_FALSE : AX_TRUE;
    pCam->nChnFrmMode[0] = (pEntryParam->bGdcEnable == 0) ? 0 : pEntryParam->nVinChnFrmMode;
    pCam->bEnableDev = AX_TRUE;
    pCam->bRegisterSns = AX_TRUE;
#ifdef TCM_SUPPORT
    pCam->nSnsCtrlMode = pEntryParam->nSnsCtrlMode;
#endif
    memcpy(pCam->tBwlimitInfo, pEntryParam->tBwlimitInfo[0], sizeof(pCam->tBwlimitInfo));
}

#if defined(SC200AI_SUPPORT)
static AX_U32 __sample_case_single_sc200ai(QS_AX_CAMERA_T *pCamList, QS_SAMPLE_SNS_TYPE_E eSnsType,
        SAMPLE_ENTRY_PARAM_T *pEntryParam, QS_COMMON_SYS_ARGS_T *pCommonArgs)
{
    QS_AX_CAMERA_T *pCam = NULL;
    AX_SENSOR_REGISTER_FUNC_T *ptSnsHdl[2] = {AX_NULL};
    QS_COMMON_VIN_MODE_E eSysMode = QS_COMMON_VIN_SENSOR;
    AX_SNS_HDR_MODE_E eHdrMode = pEntryParam->eHdrMode;
    AX_S32 j = 0;
    pCommonArgs->nCamCnt = 1;
    pEntryParam->nCamCnt = 1;
    AX_BOOL bAiispEnable = (pEntryParam->nAiIspMode == E_SNS_TISP_MODE_E) ? AX_FALSE : AX_TRUE;
    pCam = &pCamList[0];
    QSDEMO_VIN_GetSnsConfig(eSnsType, &pCam->tMipiAttr, &pCam->tSnsAttr, &pCam->tSnsClkAttr, &pCam->tDevAttr, &pCam->tPipeAttr, pCam->tChnAttr);
    QSDEMO_VIN_GetSnsObj(eSnsType, ptSnsHdl);

    pCam->nDevId = 0;
    pCam->nRxDev = 0;
    pCam->nPipeId = 0;
    pCam->tSnsClkAttr.nSnsClkIdx = 0;
    pCam->tDevBindPipe.nNum = 1;
    pCam->tDevBindPipe.nPipeId[0] = pCam->nPipeId;
#if defined(AX_FAST_BOOT)
    pCam->ptSnsHdl = ptSnsHdl[1];
#else
    pCam->ptSnsHdl = ptSnsHdl[0];
#endif
    pCam->eBusType = ISP_SNS_CONNECT_I2C_TYPE;
    __set_pipe_hdr_mode(&pCam->tDevBindPipe.nHDRSel[0], eHdrMode);
    __set_vin_attr(pCam, eSnsType, eHdrMode, eSysMode, bAiispEnable, pEntryParam);
    for (j = 0; j < pCam->tDevBindPipe.nNum; j++) {
        pCam->tPipeInfo[j].ePipeMode = QS_SAMPLE_PIPE_MODE_VIDEO;
        pCam->tPipeInfo[j].bAiispEnable = bAiispEnable;
    }

    return 0;
}

static AX_U32 __sample_case_double_sc200ai(QS_AX_CAMERA_T *pCamList, QS_SAMPLE_SNS_TYPE_E eSnsType,
        SAMPLE_ENTRY_PARAM_T *pEntryParam, QS_COMMON_SYS_ARGS_T *pCommonArgs)
{
    QS_AX_CAMERA_T *pCam = NULL;
    AX_SENSOR_REGISTER_FUNC_T *ptSnsHdl[2] = {AX_NULL};
    QS_COMMON_VIN_MODE_E eSysMode = QS_COMMON_VIN_SENSOR;
    AX_SNS_HDR_MODE_E eHdrMode = pEntryParam->eHdrMode;
    AX_BOOL bAiispEnable = (pEntryParam->nAiIspMode == E_SNS_TISP_MODE_E) ? AX_FALSE : AX_TRUE;
    AX_S32 j = 0, i = 0;
    pCommonArgs->nCamCnt = 2;
    pEntryParam->nCamCnt = 2;
    QSDEMO_VIN_GetSnsObj(eSnsType, ptSnsHdl);

    for (i = 0; i < pCommonArgs->nCamCnt; i++) {
        pCam = &pCamList[i];
        pCam->nNumber = i;
        QSDEMO_VIN_GetSnsConfig(eSnsType, &pCam->tMipiAttr, &pCam->tSnsAttr, &pCam->tSnsClkAttr, &pCam->tDevAttr, &pCam->tPipeAttr, pCam->tChnAttr);
        pCam->nDevId = i;
        pCam->nRxDev = i;
        pCam->nPipeId = i;
        pCam->nI2cAddr = 0x30;
        if (i == 1) {
            pCam->nI2cAddr = 0x32;  //AX615Q: 0x32, 630C: 0x30
        }
        pCam->tSnsClkAttr.nSnsClkIdx = 0;
        pCam->tDevBindPipe.nNum = 1;
        pCam->tDevBindPipe.nPipeId[0] = pCam->nPipeId;
#if defined(AX_FAST_BOOT)
        pCam->ptSnsHdl = ptSnsHdl[1];
#else
        pCam->ptSnsHdl = ptSnsHdl[0];
#endif
        pCam->eBusType = ISP_SNS_CONNECT_I2C_TYPE;
        pCam->tMipiAttr.eLaneNum = AX_MIPI_DATA_LANE_2;

        __set_pipe_hdr_mode(&pCam->tDevBindPipe.nHDRSel[0], eHdrMode);
        __set_vin_attr(pCam, eSnsType, eHdrMode, eSysMode, bAiispEnable, pEntryParam);
        for (j = 0; j < pCam->tDevBindPipe.nNum; j++) {
            pCam->tPipeInfo[j].ePipeMode = QS_SAMPLE_PIPE_MODE_VIDEO;
            pCam->tPipeInfo[j].bAiispEnable = bAiispEnable;
        }
    }

    return 0;
}

static AX_U32 __sample_case_triple_sc200ai(QS_AX_CAMERA_T *pCamList, QS_SAMPLE_SNS_TYPE_E eSnsType,
        SAMPLE_ENTRY_PARAM_T *pEntryParam, QS_COMMON_SYS_ARGS_T *pCommonArgs)
{
    QS_AX_CAMERA_T *pCam = NULL;
    AX_SENSOR_REGISTER_FUNC_T *ptSnsHdl[2] = {AX_NULL};
    QS_COMMON_VIN_MODE_E eSysMode = QS_COMMON_VIN_SENSOR;
    AX_SNS_HDR_MODE_E eHdrMode = pEntryParam->eHdrMode;
    AX_BOOL bAiispEnable = (pEntryParam->nAiIspMode == E_SNS_TISP_MODE_E) ? AX_FALSE : AX_TRUE;
    AX_S32 j = 0, i = 0;
    pCommonArgs->nCamCnt = 2;
    pEntryParam->nCamCnt = 2;
    QSDEMO_VIN_GetSnsObj(eSnsType, ptSnsHdl);

    for (i = 0; i < pCommonArgs->nCamCnt; i++) {
        pCam = &pCamList[i];
        pCam->nNumber = i;
        QSDEMO_VIN_GetSnsConfig(eSnsType, &pCam->tMipiAttr, &pCam->tSnsAttr, &pCam->tSnsClkAttr, &pCam->tDevAttr, &pCam->tPipeAttr, pCam->tChnAttr);
        pCam->nDevId = i;
        pCam->nRxDev = i;
        pCam->nPipeId = i;
        pCam->nI2cAddr = 0x30;

        pCam->tSnsClkAttr.nSnsClkIdx = 0;
        pCam->tDevBindPipe.nNum = 1;
        pCam->tDevBindPipe.nPipeId[0] = pCam->nPipeId;
#if defined(AX_FAST_BOOT)
        pCam->ptSnsHdl = ptSnsHdl[1];
#else
        pCam->ptSnsHdl = ptSnsHdl[0];
#endif
        pCam->eBusType = ISP_SNS_CONNECT_I2C_TYPE;
        pCam->tMipiAttr.eLaneNum = AX_MIPI_DATA_LANE_2;

        if (i == 1) {
            if (pEntryParam->nSwitchSnsId == 1) {
                pCam->ptSnsHdl1 = ptSnsHdl[0];
            } else {
                pCam->ptSnsHdl1 = ptSnsHdl[1];
                pCam->ptSnsHdl = ptSnsHdl[0];
            }
            pCam->nSwitchSnsId = pEntryParam->nSwitchSnsId;
            pCam->nI2cAddr = 0x32;
            pCam->nI2cAddr1 = 0x32;
            pCam->tSnsAttr.eMasterSlaveSel = AX_SNS_SYNC_SLAVE;
            pCam->bMipiSwitchEnable = AX_TRUE;
            pCam->eLoadRawNode = LOAD_RAW_ITP;
            pCam->eSysMode = QS_COMMON_VIN_LOADRAW;
            pCam->tEzoomSwitchInfo = pEntryParam->tEzoomSwitchInfo;
            if (pEntryParam->tEzoomSwitchInfo.nWidth > 0) {
                pCam->tEzoomSwitchInfo.fSwitchRatio = pCam->tSnsAttr.nWidth * 1.0f / pEntryParam->tEzoomSwitchInfo.nWidth;
            } else {
                pCam->tEzoomSwitchInfo.fSwitchRatio = pEntryParam->tEzoomSwitchInfo.fSwitchRatio;
            }
            pEntryParam->tEzoomSwitchInfo.fSwitchRatio = pCam->tEzoomSwitchInfo.fSwitchRatio;
            ALOGI("fSwitchRatio: %f", pEntryParam->tEzoomSwitchInfo.fSwitchRatio);
            eSysMode = QS_COMMON_VIN_LOADRAW;
        }

        __set_pipe_hdr_mode(&pCam->tDevBindPipe.nHDRSel[0], eHdrMode);
        __set_vin_attr(pCam, eSnsType, eHdrMode, eSysMode, bAiispEnable, pEntryParam);
        for (j = 0; j < pCam->tDevBindPipe.nNum; j++) {
            pCam->tPipeInfo[j].ePipeMode = QS_SAMPLE_PIPE_MODE_VIDEO;
            pCam->tPipeInfo[j].bAiispEnable = bAiispEnable;
        }

        if (pCam->bMipiSwitchEnable) {
            strcpy(pCam->tPipeInfo[0].szBinPath, "nulll.bin"); // wide bin
            strcpy(pCam->tPipeInfo[1].szBinPath, "nulll.bin"); // tele bin
        }
        strcpy(pCam->nIvpsSharpenBinPath, "null.bin");
    }

    return 0;
}
#endif

#if defined(SC235HAI_SUPPORT)
static AX_U32 __sample_case_single_sc235hai(QS_AX_CAMERA_T *pCamList, QS_SAMPLE_SNS_TYPE_E eSnsType,
        SAMPLE_ENTRY_PARAM_T *pEntryParam, QS_COMMON_SYS_ARGS_T *pCommonArgs)
{
    QS_AX_CAMERA_T *pCam = NULL;
    AX_SENSOR_REGISTER_FUNC_T *ptSnsHdl[2] = {AX_NULL};
    QS_COMMON_VIN_MODE_E eSysMode = QS_COMMON_VIN_SENSOR;
    AX_SNS_HDR_MODE_E eHdrMode = pEntryParam->eHdrMode;
    AX_S32 j = 0;
    pCommonArgs->nCamCnt = 1;
    pEntryParam->nCamCnt = 1;
    AX_BOOL bAiispEnable = (pEntryParam->nAiIspMode == E_SNS_TISP_MODE_E) ? AX_FALSE : AX_TRUE;
    pCam = &pCamList[0];
    QSDEMO_VIN_GetSnsConfig(eSnsType, &pCam->tMipiAttr, &pCam->tSnsAttr, &pCam->tSnsClkAttr, &pCam->tDevAttr, &pCam->tPipeAttr, pCam->tChnAttr);
    QSDEMO_VIN_GetSnsObj(eSnsType, ptSnsHdl);

    pCam->nDevId = 0;
    pCam->nRxDev = 0;
    pCam->nPipeId = 0;
    pCam->tSnsClkAttr.nSnsClkIdx = 0;
    pCam->tDevBindPipe.nNum = 1;
    pCam->tDevBindPipe.nPipeId[0] = pCam->nPipeId;
#ifdef AX_FAST_BOOT
    AX_S32 nSingleSnsPipeId = QS_GetSingleSnsPipeId(-1);
    /* rx0:0 rx1:1 */
    if (nSingleSnsPipeId >= 0) {
        pCam->nDevId = nSingleSnsPipeId;
        pCam->nRxDev = nSingleSnsPipeId;
        pCam->nPipeId = nSingleSnsPipeId;
        /* only for ax615q rx1 */
        if (nSingleSnsPipeId == 1) {
            pCam->nI2cAddr = 0x10;
        }
        pCam->tSnsAttr.nSettingIndex = 0;
        pCam->tMipiAttr.eLaneNum = 4;
    }
    pCam->ptSnsHdl = ptSnsHdl[1];
#else
    if (eHdrMode == AX_SNS_LINEAR_MODE || eHdrMode == AX_SNS_LINEAR_ONLY_MODE) {
        pCam->tSnsAttr.nSettingIndex = 1;
    } else {
        pCam->tSnsAttr.nSettingIndex = 2;
    }
    pCam->ptSnsHdl = ptSnsHdl[0];
#endif
    pCam->eBusType = ISP_SNS_CONNECT_I2C_TYPE;
    __set_pipe_hdr_mode(&pCam->tDevBindPipe.nHDRSel[0], eHdrMode);
    __set_vin_attr(pCam, eSnsType, eHdrMode, eSysMode, bAiispEnable, pEntryParam);
    for (j = 0; j < pCam->tDevBindPipe.nNum; j++) {
        pCam->tPipeInfo[j].ePipeMode = QS_SAMPLE_PIPE_MODE_VIDEO;
        pCam->tPipeInfo[j].bAiispEnable = bAiispEnable;
        if (eHdrMode == AX_SNS_LINEAR_MODE || eHdrMode == AX_SNS_LINEAR_ONLY_MODE) {
            strcpy(pCam->tPipeInfo[j].szBinPath, "/opt/etc/sc235hai_sdr_t_aiisp_reduce_fps.bin");
        } else {
            strcpy(pCam->tPipeInfo[j].szBinPath, "/opt/etc/sc235hai_hdr_t_aiisp_fix_ratio.bin");
        }
    }
    strcpy(pCam->nIvpsSharpenBinPath, "null.bin");
    return 0;
}

static AX_U32 __sample_case_double_sc235hai(QS_AX_CAMERA_T *pCamList, QS_SAMPLE_SNS_TYPE_E eSnsType,
        SAMPLE_ENTRY_PARAM_T *pEntryParam, QS_COMMON_SYS_ARGS_T *pCommonArgs)
{
    QS_AX_CAMERA_T *pCam = NULL;
    AX_SENSOR_REGISTER_FUNC_T *ptSnsHdl[2] = {AX_NULL};
    QS_COMMON_VIN_MODE_E eSysMode = QS_COMMON_VIN_SENSOR;
    AX_SNS_HDR_MODE_E eHdrMode = pEntryParam->eHdrMode;
    AX_BOOL bAiispEnable = (pEntryParam->nAiIspMode == E_SNS_TISP_MODE_E) ? AX_FALSE : AX_TRUE;
    AX_S32 j = 0, i = 0;
    pCommonArgs->nCamCnt = 2;
    pEntryParam->nCamCnt = 2;

    for (i = 0; i < pCommonArgs->nCamCnt; i++) {
        pCam = &pCamList[i];
        pCam->nNumber = i;
        QSDEMO_VIN_GetSnsConfig(eSnsType, &pCam->tMipiAttr, &pCam->tSnsAttr, &pCam->tSnsClkAttr, &pCam->tDevAttr, &pCam->tPipeAttr, pCam->tChnAttr);
        QSDEMO_VIN_GetSnsObj(eSnsType, ptSnsHdl);

        pCam->nDevId = i;
        pCam->nRxDev = i;
        pCam->nPipeId = i;
    #ifdef AX_FAST_BOOT
        AX_S32 nSingleSnsPipeId = QS_GetSingleSnsPipeId(-1);
        /* rx0:0 rx1:1 */
        if (nSingleSnsPipeId >= 0) {
            pCam->nDevId = nSingleSnsPipeId;
            pCam->nRxDev = nSingleSnsPipeId;
            pCam->nPipeId = nSingleSnsPipeId;
            /* only for ax615q rx1 */
            if (nSingleSnsPipeId == 1) {
                pCam->nI2cAddr = 0x10;
            }
            pCam->tSnsAttr.nSettingIndex = 0;
            pCam->tMipiAttr.eLaneNum = 4;
        }
        pCam->ptSnsHdl = ptSnsHdl[1];
    #else
        if (eHdrMode == AX_SNS_LINEAR_MODE || eHdrMode == AX_SNS_LINEAR_ONLY_MODE) {
            pCam->tSnsAttr.nSettingIndex = 1;
        } else {
            pCam->tSnsAttr.nSettingIndex = 2;
        }
        pCam->ptSnsHdl = ptSnsHdl[0];
    #endif

        pCam->tSnsClkAttr.nSnsClkIdx = 0;
        pCam->tDevBindPipe.nNum = 1;
        pCam->tDevBindPipe.nPipeId[0] = pCam->nPipeId;
        pCam->eBusType = ISP_SNS_CONNECT_I2C_TYPE;
        pCam->tMipiAttr.eLaneNum = AX_MIPI_DATA_LANE_2;

        __set_pipe_hdr_mode(&pCam->tDevBindPipe.nHDRSel[0], eHdrMode);
        __set_vin_attr(pCam, eSnsType, eHdrMode, eSysMode, bAiispEnable, pEntryParam);
        for (j = 0; j < pCam->tDevBindPipe.nNum; j++) {
            pCam->tPipeInfo[j].ePipeMode = QS_SAMPLE_PIPE_MODE_VIDEO;
            pCam->tPipeInfo[j].bAiispEnable = bAiispEnable;
            if (eHdrMode == AX_SNS_LINEAR_MODE || eHdrMode == AX_SNS_LINEAR_ONLY_MODE) {
                strcpy(pCam->tPipeInfo[j].szBinPath, "/opt/etc/sc235hai_sdr_t_aiisp_reduce_fps.bin");
            } else {
                strcpy(pCam->tPipeInfo[j].szBinPath, "/opt/etc/sc235hai_hdr_t_aiisp_fix_ratio.bin");
            }
        }
        strcpy(pCam->nIvpsSharpenBinPath, "null.bin");
    }
    return 0;
}
#endif

#if defined(SC500AI_SUPPORT)
static AX_U32 __sample_case_single_sc500ai(QS_AX_CAMERA_T *pCamList, QS_SAMPLE_SNS_TYPE_E eSnsType,
        SAMPLE_ENTRY_PARAM_T *pEntryParam, QS_COMMON_SYS_ARGS_T *pCommonArgs)
{
    QS_AX_CAMERA_T *pCam = NULL;
    AX_SENSOR_REGISTER_FUNC_T *ptSnsHdl[2] = {AX_NULL};
    QS_COMMON_VIN_MODE_E eSysMode = QS_COMMON_VIN_SENSOR;
    AX_SNS_HDR_MODE_E eHdrMode = pEntryParam->eHdrMode;
    AX_S32 j = 0;
    pCommonArgs->nCamCnt = 1;
    pEntryParam->nCamCnt = 1;

    AX_BOOL bAiispEnable = (pEntryParam->nAiIspMode == E_SNS_TISP_MODE_E) ? AX_FALSE : AX_TRUE;
    pCam = &pCamList[0];
    pCam->nNumber = 0;
    QSDEMO_VIN_GetSnsConfig(eSnsType, &pCam->tMipiAttr, &pCam->tSnsAttr, &pCam->tSnsClkAttr, &pCam->tDevAttr, &pCam->tPipeAttr, pCam->tChnAttr);
    QSDEMO_VIN_GetSnsObj(eSnsType, ptSnsHdl);

    pCam->nDevId = 0;
    pCam->nRxDev = 0;
    pCam->nPipeId = 0;
    //pCam->nI2cAddr = 0x30;
    pCam->tSnsClkAttr.nSnsClkIdx = 0;
    pCam->tDevBindPipe.nNum = 1;
    pCam->tDevBindPipe.nPipeId[0] = pCam->nPipeId;
#if defined(AX_FAST_BOOT)
    pCam->ptSnsHdl = ptSnsHdl[1];
#else
    pCam->ptSnsHdl = ptSnsHdl[0];
#endif
    pCam->eBusType = ISP_SNS_CONNECT_I2C_TYPE;

    __set_pipe_hdr_mode(&pCam->tDevBindPipe.nHDRSel[0], eHdrMode);
    __set_vin_attr(pCam, eSnsType, eHdrMode, eSysMode, bAiispEnable, pEntryParam);
    for (j = 0; j < pCam->tDevBindPipe.nNum; j++) {
        pCam->tPipeInfo[j].ePipeMode = QS_SAMPLE_PIPE_MODE_VIDEO;
        pCam->tPipeInfo[j].bAiispEnable = bAiispEnable;
    }
    return 0;
}
#endif

#if defined(SC450AI_SUPPORT)
static AX_U32 __sample_case_single_sc450ai(QS_AX_CAMERA_T *pCamList, QS_SAMPLE_SNS_TYPE_E eSnsType,
        SAMPLE_ENTRY_PARAM_T *pEntryParam, QS_COMMON_SYS_ARGS_T *pCommonArgs)
{
    QS_AX_CAMERA_T *pCam = NULL;
    AX_SENSOR_REGISTER_FUNC_T *ptSnsHdl[2] = {AX_NULL};
    QS_COMMON_VIN_MODE_E eSysMode = QS_COMMON_VIN_SENSOR;
    AX_SNS_HDR_MODE_E eHdrMode = pEntryParam->eHdrMode;
    AX_S32 j = 0;
    pCommonArgs->nCamCnt = 1;
    pEntryParam->nCamCnt = 1;
    AX_BOOL bAiispEnable = (pEntryParam->nAiIspMode == E_SNS_TISP_MODE_E) ? AX_FALSE : AX_TRUE;
    pCam = &pCamList[0];
    QSDEMO_VIN_GetSnsConfig(eSnsType, &pCam->tMipiAttr, &pCam->tSnsAttr, &pCam->tSnsClkAttr, &pCam->tDevAttr, &pCam->tPipeAttr, pCam->tChnAttr);
    QSDEMO_VIN_GetSnsObj(eSnsType, ptSnsHdl);

    pCam->nDevId = 0;
    pCam->nRxDev = 0;
    pCam->nPipeId = 0;
    pCam->tSnsClkAttr.nSnsClkIdx = 0;
    pCam->tDevBindPipe.nNum = 1;
    pCam->tDevBindPipe.nPipeId[0] = pCam->nPipeId;
#if defined(AX_FAST_BOOT)
    pCam->ptSnsHdl = ptSnsHdl[1];
#else
    pCam->ptSnsHdl = ptSnsHdl[0];
#endif
    pCam->eBusType = ISP_SNS_CONNECT_I2C_TYPE;
    __set_pipe_hdr_mode(&pCam->tDevBindPipe.nHDRSel[0], eHdrMode);
    __set_vin_attr(pCam, eSnsType, eHdrMode, eSysMode, bAiispEnable, pEntryParam);
    for (j = 0; j < pCam->tDevBindPipe.nNum; j++) {
        pCam->tPipeInfo[j].ePipeMode = QS_SAMPLE_PIPE_MODE_VIDEO;
        pCam->tPipeInfo[j].bAiispEnable = bAiispEnable;
    }
    return 0;
}
#endif

#if defined(OS04D10_SUPPORT)
static AX_U32 __sample_case_single_os04d10(QS_AX_CAMERA_T *pCamList, QS_SAMPLE_SNS_TYPE_E eSnsType,
        SAMPLE_ENTRY_PARAM_T *pEntryParam, QS_COMMON_SYS_ARGS_T *pCommonArgs)
{
    QS_AX_CAMERA_T *pCam = NULL;
    AX_SENSOR_REGISTER_FUNC_T *ptSnsHdl[2] = {AX_NULL};
    QS_COMMON_VIN_MODE_E eSysMode = QS_COMMON_VIN_SENSOR;
    AX_SNS_HDR_MODE_E eHdrMode = pEntryParam->eHdrMode;
    AX_S32 j = 0;
    pCommonArgs->nCamCnt = 1;
    pEntryParam->nCamCnt = 1;
    AX_BOOL bAiispEnable = (pEntryParam->nAiIspMode == E_SNS_TISP_MODE_E) ? AX_FALSE : AX_TRUE;
    pCam = &pCamList[0];
    QSDEMO_VIN_GetSnsConfig(eSnsType, &pCam->tMipiAttr, &pCam->tSnsAttr, &pCam->tSnsClkAttr, &pCam->tDevAttr, &pCam->tPipeAttr, pCam->tChnAttr);
    QSDEMO_VIN_GetSnsObj(eSnsType, ptSnsHdl);

    pCam->nDevId = 0;
    pCam->nRxDev = 0;
    pCam->nPipeId = 0;
    pCam->tSnsClkAttr.nSnsClkIdx = 0;
    pCam->tDevBindPipe.nNum = 1;
    pCam->tDevBindPipe.nPipeId[0] = pCam->nPipeId;
#if defined(AX_FAST_BOOT)
    pCam->ptSnsHdl = ptSnsHdl[1];
#else
    pCam->ptSnsHdl = ptSnsHdl[0];
#endif
    pCam->eBusType = ISP_SNS_CONNECT_I2C_TYPE;
    __set_pipe_hdr_mode(&pCam->tDevBindPipe.nHDRSel[0], eHdrMode);
    __set_vin_attr(pCam, eSnsType, eHdrMode, eSysMode, bAiispEnable, pEntryParam);
    for (j = 0; j < pCam->tDevBindPipe.nNum; j++) {
        pCam->tPipeInfo[j].ePipeMode = QS_SAMPLE_PIPE_MODE_VIDEO;
        pCam->tPipeInfo[j].bAiispEnable = bAiispEnable;
        if (eHdrMode == AX_SNS_LINEAR_MODE || eHdrMode == AX_SNS_LINEAR_ONLY_MODE) {
            strcpy(pCam->tPipeInfo[j].szBinPath, "/opt/etc/os04d10_sdr_t_aiisp_reduce_fps.bin");
        } else {
            strcpy(pCam->tPipeInfo[j].szBinPath, "/opt/etc/os04d10_hdr_t_aiisp_fix_ratio.bin");
        }
    }
    strcpy(pCam->nIvpsSharpenBinPath, "null.bin");
    return 0;
}
static AX_U32 __sample_case_double_os04d10(QS_AX_CAMERA_T *pCamList, QS_SAMPLE_SNS_TYPE_E eSnsType,
        SAMPLE_ENTRY_PARAM_T *pEntryParam, QS_COMMON_SYS_ARGS_T *pCommonArgs)
{
    QS_AX_CAMERA_T *pCam = NULL;
    AX_SENSOR_REGISTER_FUNC_T *ptSnsHdl[2] = {AX_NULL};
    QS_COMMON_VIN_MODE_E eSysMode = QS_COMMON_VIN_SENSOR;
    AX_SNS_HDR_MODE_E eHdrMode = pEntryParam->eHdrMode;
    AX_S32 j = 0, i = 0;
    pCommonArgs->nCamCnt = 2;
    pEntryParam->nCamCnt = 2;
    AX_BOOL bAiispEnable = (pEntryParam->nAiIspMode == E_SNS_TISP_MODE_E) ? AX_FALSE : AX_TRUE;

    for (i = 0; i < pCommonArgs->nCamCnt; i++) {
        pCam = &pCamList[i];
        pCam->nNumber = i;
        QSDEMO_VIN_GetSnsConfig(eSnsType, &pCam->tMipiAttr, &pCam->tSnsAttr, &pCam->tSnsClkAttr, &pCam->tDevAttr, &pCam->tPipeAttr, pCam->tChnAttr);
        QSDEMO_VIN_GetSnsObj(eSnsType, ptSnsHdl);

        pCam->nDevId = i;
        pCam->nRxDev = i;
        pCam->nPipeId = i;
    #if defined(AX_FAST_BOOT)
        pCam->ptSnsHdl = ptSnsHdl[1];
    #else
        pCam->ptSnsHdl = ptSnsHdl[0];
    #endif
        pCam->tSnsClkAttr.nSnsClkIdx = 0;
        pCam->tDevBindPipe.nNum = 1;
        pCam->tDevBindPipe.nPipeId[0] = pCam->nPipeId;
        pCam->eBusType = ISP_SNS_CONNECT_I2C_TYPE;
        pCam->tMipiAttr.eLaneNum = AX_MIPI_DATA_LANE_2;

        __set_pipe_hdr_mode(&pCam->tDevBindPipe.nHDRSel[0], eHdrMode);
        __set_vin_attr(pCam, eSnsType, eHdrMode, eSysMode, bAiispEnable, pEntryParam);
        for (j = 0; j < pCam->tDevBindPipe.nNum; j++) {
            pCam->tPipeInfo[j].ePipeMode = QS_SAMPLE_PIPE_MODE_VIDEO;
            pCam->tPipeInfo[j].bAiispEnable = bAiispEnable;
            if (eHdrMode == AX_SNS_LINEAR_MODE || eHdrMode == AX_SNS_LINEAR_ONLY_MODE) {
                strcpy(pCam->tPipeInfo[j].szBinPath, "/opt/etc/os04d10_sdr_t_aiisp_reduce_fps.bin");
            } else {
                strcpy(pCam->tPipeInfo[j].szBinPath, "/opt/etc/os04d10_hdr_t_aiisp_fix_ratio.bin");
            }
        }
        strcpy(pCam->nIvpsSharpenBinPath, "null.bin");
    }
    return 0;
}
#endif

#if defined(OS04A10_SUPPORT)
static AX_U32 __sample_case_single_os04a10(QS_AX_CAMERA_T *pCamList, QS_SAMPLE_SNS_TYPE_E eSnsType,
        SAMPLE_ENTRY_PARAM_T *pEntryParam, QS_COMMON_SYS_ARGS_T *pCommonArgs)
{
    QS_AX_CAMERA_T *pCam = NULL;
    AX_SENSOR_REGISTER_FUNC_T *ptSnsHdl[2] = {AX_NULL};
    QS_COMMON_VIN_MODE_E eSysMode = QS_COMMON_VIN_SENSOR;
    AX_SNS_HDR_MODE_E eHdrMode = pEntryParam->eHdrMode;
    AX_S32 j = 0;
    pCommonArgs->nCamCnt = 1;
    pEntryParam->nCamCnt = 1;
    AX_BOOL bAiispEnable = (pEntryParam->nAiIspMode == E_SNS_TISP_MODE_E) ? AX_FALSE : AX_TRUE;
    pCam = &pCamList[0];
    QSDEMO_VIN_GetSnsConfig(eSnsType, &pCam->tMipiAttr, &pCam->tSnsAttr, &pCam->tSnsClkAttr, &pCam->tDevAttr, &pCam->tPipeAttr, pCam->tChnAttr);
    QSDEMO_VIN_GetSnsObj(eSnsType, ptSnsHdl);

    pCam->nDevId = 0;
    pCam->nRxDev = 0;
    pCam->nPipeId = 0;
#if defined(AX_FAST_BOOT)
    AX_S32 nSingleSnsPipeId = QS_GetSingleSnsPipeId(-1);
    /* rx0:0 rx1:1 */
    if (nSingleSnsPipeId >= 0) {
        pCam->nDevId = nSingleSnsPipeId;
        pCam->nRxDev = nSingleSnsPipeId;
        pCam->nPipeId = nSingleSnsPipeId;
        /* only for os04a ax615q rx1 */
        if (nSingleSnsPipeId == 1) {
            pCam->nI2cAddr = 0x10;
        }
        pCam->tSnsAttr.nSettingIndex = 35;
        pCam->tMipiAttr.eLaneNum = 2;
    }
    pCam->ptSnsHdl = ptSnsHdl[1];
#else
    pCam->ptSnsHdl = ptSnsHdl[0];
#endif

    pCam->tSnsClkAttr.nSnsClkIdx = 0;
    pCam->tDevBindPipe.nNum = 1;
    pCam->tDevBindPipe.nPipeId[0] = pCam->nPipeId;

    pCam->eBusType = ISP_SNS_CONNECT_I2C_TYPE;
    __set_pipe_hdr_mode(&pCam->tDevBindPipe.nHDRSel[0], eHdrMode);
    __set_vin_attr(pCam, eSnsType, eHdrMode, eSysMode, bAiispEnable, pEntryParam);
    for (j = 0; j < pCam->tDevBindPipe.nNum; j++) {
        pCam->tPipeInfo[j].ePipeMode = QS_SAMPLE_PIPE_MODE_VIDEO;
        pCam->tPipeInfo[j].bAiispEnable = bAiispEnable;
        if (eHdrMode == AX_SNS_LINEAR_MODE || eHdrMode == AX_SNS_LINEAR_ONLY_MODE) {
            strcpy(pCam->tPipeInfo[j].szBinPath, "/opt/etc/os04a10_sdr_t_aiisp_reduce_fps.bin");
        } else {
            strcpy(pCam->tPipeInfo[j].szBinPath, "/opt/etc/os04a10_hdr_t_aiisp_fix_ratio.bin");
        }
    }
    strcpy(pCam->nIvpsSharpenBinPath, "null.bin");
    return 0;
}


static AX_U32 __sample_case_double_os04a10(QS_AX_CAMERA_T *pCamList, QS_SAMPLE_SNS_TYPE_E eSnsType,
        SAMPLE_ENTRY_PARAM_T *pEntryParam, QS_COMMON_SYS_ARGS_T *pCommonArgs)
{
    QS_AX_CAMERA_T *pCam = NULL;
    AX_SENSOR_REGISTER_FUNC_T *ptSnsHdl[2] = {AX_NULL};
    QS_COMMON_VIN_MODE_E eSysMode = QS_COMMON_VIN_SENSOR;
    AX_SNS_HDR_MODE_E eHdrMode = pEntryParam->eHdrMode;
    AX_BOOL bAiispEnable = (pEntryParam->nAiIspMode == E_SNS_TISP_MODE_E) ? AX_FALSE : AX_TRUE;
    AX_S32 j = 0, i = 0;
    pCommonArgs->nCamCnt = 2;
    pEntryParam->nCamCnt = 2;
    QSDEMO_VIN_GetSnsObj(eSnsType, ptSnsHdl);

    for (i = 0; i < pCommonArgs->nCamCnt; i++) {
        pCam = &pCamList[i];
        pCam->nNumber = i;
        QSDEMO_VIN_GetSnsConfig(eSnsType, &pCam->tMipiAttr, &pCam->tSnsAttr, &pCam->tSnsClkAttr, &pCam->tDevAttr, &pCam->tPipeAttr, pCam->tChnAttr);

#if defined(AX_FAST_BOOT)
        pCam->ptSnsHdl = ptSnsHdl[1];
#else
        pCam->ptSnsHdl = ptSnsHdl[0];
#endif
        pCam->tSnsAttr.nSettingIndex = 35;
        pCam->nDevId = i;
        pCam->nRxDev = i;
        pCam->nPipeId = i;
        pCam->nI2cAddr = 0x36;

        pCam->tSnsClkAttr.nSnsClkIdx = 0;
        pCam->tDevBindPipe.nNum = 1;
        pCam->tDevBindPipe.nPipeId[0] = pCam->nPipeId;

        pCam->eBusType = ISP_SNS_CONNECT_I2C_TYPE;
        pCam->tMipiAttr.eLaneNum = AX_MIPI_DATA_LANE_2;

        __set_pipe_hdr_mode(&pCam->tDevBindPipe.nHDRSel[0], eHdrMode);
        __set_vin_attr(pCam, eSnsType, eHdrMode, eSysMode, bAiispEnable, pEntryParam);
        for (j = 0; j < pCam->tDevBindPipe.nNum; j++) {
            pCam->tPipeInfo[j].ePipeMode = QS_SAMPLE_PIPE_MODE_VIDEO;
            pCam->tPipeInfo[j].bAiispEnable = bAiispEnable;
            if (eHdrMode == AX_SNS_LINEAR_MODE || eHdrMode == AX_SNS_LINEAR_ONLY_MODE) {
                strcpy(pCam->tPipeInfo[j].szBinPath, "/opt/etc/os04a10_sdr_t_aiisp_reduce_fps.bin");
            } else {
                strcpy(pCam->tPipeInfo[j].szBinPath, "/opt/etc/os04a10_hdr_t_aiisp_fix_ratio.bin");
            }
        }
        strcpy(pCam->nIvpsSharpenBinPath, "null.bin");
    }

    return 0;
}
#endif

#if defined(SC438HAI_SUPPORT)
static AX_U32 __sample_case_single_sc438hai(QS_AX_CAMERA_T *pCamList, QS_SAMPLE_SNS_TYPE_E eSnsType,
        SAMPLE_ENTRY_PARAM_T *pEntryParam, QS_COMMON_SYS_ARGS_T *pCommonArgs)
{
    QS_AX_CAMERA_T *pCam = NULL;
    AX_SENSOR_REGISTER_FUNC_T *ptSnsHdl[2] = {AX_NULL};
    QS_COMMON_VIN_MODE_E eSysMode = QS_COMMON_VIN_SENSOR;
    AX_SNS_HDR_MODE_E eHdrMode = pEntryParam->eHdrMode;
    AX_S32 j = 0;
    pCommonArgs->nCamCnt = 1;
    pEntryParam->nCamCnt = 1;
    AX_BOOL bAiispEnable = (pEntryParam->nAiIspMode == E_SNS_TISP_MODE_E) ? AX_FALSE : AX_TRUE;
    pCam = &pCamList[0];
    QSDEMO_VIN_GetSnsConfig(eSnsType, &pCam->tMipiAttr, &pCam->tSnsAttr, &pCam->tSnsClkAttr, &pCam->tDevAttr, &pCam->tPipeAttr, pCam->tChnAttr);
    QSDEMO_VIN_GetSnsObj(eSnsType, ptSnsHdl);

    pCam->nDevId = 0;
    pCam->nRxDev = 0;
    pCam->nPipeId = 0;
#ifdef AX_FAST_BOOT
    AX_S32 nSingleSnsPipeId = QS_GetSingleSnsPipeId(-1);
    /* rx0:0 rx1:1 */
    if (nSingleSnsPipeId >= 0) {
        pCam->nDevId = nSingleSnsPipeId;
        pCam->nRxDev = nSingleSnsPipeId;
        pCam->nPipeId = nSingleSnsPipeId;
        /* only for ax615q rx1 */
        if (nSingleSnsPipeId == 1) {
            pCam->nI2cAddr = 0x10;
        }
        pCam->tSnsAttr.nSettingIndex = 0;
        pCam->tMipiAttr.eLaneNum = 4;
    }
    pCam->ptSnsHdl = ptSnsHdl[1];
#else
    if (eHdrMode == AX_SNS_LINEAR_MODE || eHdrMode == AX_SNS_LINEAR_ONLY_MODE) {
        pCam->tSnsAttr.nSettingIndex = 1;
    } else {
        pCam->tSnsAttr.nSettingIndex = 2;
    }
    pCam->ptSnsHdl = ptSnsHdl[0];
#endif

    pCam->tSnsClkAttr.nSnsClkIdx = 0;
    pCam->tDevBindPipe.nNum = 1;
    pCam->tDevBindPipe.nPipeId[0] = pCam->nPipeId;

    pCam->eBusType = ISP_SNS_CONNECT_I2C_TYPE;
    __set_pipe_hdr_mode(&pCam->tDevBindPipe.nHDRSel[0], eHdrMode);
    __set_vin_attr(pCam, eSnsType, eHdrMode, eSysMode, bAiispEnable, pEntryParam);
    for (j = 0; j < pCam->tDevBindPipe.nNum; j++) {
        pCam->tPipeInfo[j].ePipeMode = QS_SAMPLE_PIPE_MODE_VIDEO;
        pCam->tPipeInfo[j].bAiispEnable = bAiispEnable;
        if (eHdrMode == AX_SNS_LINEAR_MODE || eHdrMode == AX_SNS_LINEAR_ONLY_MODE) {
            strcpy(pCam->tPipeInfo[j].szBinPath, "/opt/etc/sc438hai_sdr_t_aiisp_reduce_fps.bin");
        } else {
            strcpy(pCam->tPipeInfo[j].szBinPath, "/opt/etc/sc438hai_hdr_t_aiisp_fix_ratio.bin");
        }
    }
    strcpy(pCam->nIvpsSharpenBinPath, "null.bin");
    return 0;
}

static AX_U32 __sample_case_double_sc438hai(QS_AX_CAMERA_T *pCamList, QS_SAMPLE_SNS_TYPE_E eSnsType,
        SAMPLE_ENTRY_PARAM_T *pEntryParam, QS_COMMON_SYS_ARGS_T *pCommonArgs)
{
    QS_AX_CAMERA_T *pCam = NULL;
    AX_SENSOR_REGISTER_FUNC_T *ptSnsHdl[2] = {AX_NULL};
    QS_COMMON_VIN_MODE_E eSysMode = QS_COMMON_VIN_SENSOR;
    AX_SNS_HDR_MODE_E eHdrMode = pEntryParam->eHdrMode;
    AX_S32 j = 0, i = 0;
    pCommonArgs->nCamCnt = 2;
    pEntryParam->nCamCnt = 2;
    AX_BOOL bAiispEnable = (pEntryParam->nAiIspMode == E_SNS_TISP_MODE_E) ? AX_FALSE : AX_TRUE;

    for (i = 0; i < pCommonArgs->nCamCnt; i++) {
        pCam = &pCamList[i];
        pCam->nNumber = i;
        QSDEMO_VIN_GetSnsConfig(eSnsType, &pCam->tMipiAttr, &pCam->tSnsAttr, &pCam->tSnsClkAttr, &pCam->tDevAttr, &pCam->tPipeAttr, pCam->tChnAttr);
        QSDEMO_VIN_GetSnsObj(eSnsType, ptSnsHdl);

        pCam->nDevId = i;
        pCam->nRxDev = i;
        pCam->nPipeId = i;
    #ifdef AX_FAST_BOOT
        AX_S32 nSingleSnsPipeId = QS_GetSingleSnsPipeId(-1);
        /* rx0:0 rx1:1 */
        if (nSingleSnsPipeId >= 0) {
            pCam->nDevId = nSingleSnsPipeId;
            pCam->nRxDev = nSingleSnsPipeId;
            pCam->nPipeId = nSingleSnsPipeId;
            /* only for ax615q rx1 */
            if (nSingleSnsPipeId == 1) {
                pCam->nI2cAddr = 0x10;
            }
            pCam->tSnsAttr.nSettingIndex = 0;
            pCam->tMipiAttr.eLaneNum = 4;
        }
        pCam->ptSnsHdl = ptSnsHdl[1];
    #else
        if (eHdrMode == AX_SNS_LINEAR_MODE || eHdrMode == AX_SNS_LINEAR_ONLY_MODE) {
            pCam->tSnsAttr.nSettingIndex = 3; //2688X1520_LINEAR_10bit_RGGB_30FPS_2LANE_24M_900Mbps
            pCam->tMipiAttr.nDataRate = 900;
        } else {
            pCam->tSnsAttr.nSettingIndex = 4; //2688X1520_HDR_10bit_RGGB_25FPS_2LANE_24M_1188Mbps
            pCam->tMipiAttr.nDataRate = 1188;
        }
        pCam->ptSnsHdl = ptSnsHdl[0];
    #endif

        pCam->tSnsClkAttr.nSnsClkIdx = 0;
        pCam->tDevBindPipe.nNum = 1;
        pCam->tDevBindPipe.nPipeId[0] = pCam->nPipeId;
        pCam->eBusType = ISP_SNS_CONNECT_I2C_TYPE;
        pCam->tMipiAttr.eLaneNum = AX_MIPI_DATA_LANE_2;
        pCam->tSnsClkAttr.eSnsClkRate = AX_SNS_CLK_24M;

        __set_pipe_hdr_mode(&pCam->tDevBindPipe.nHDRSel[0], eHdrMode);
        __set_vin_attr(pCam, eSnsType, eHdrMode, eSysMode, bAiispEnable, pEntryParam);
        for (j = 0; j < pCam->tDevBindPipe.nNum; j++) {
            pCam->tPipeInfo[j].ePipeMode = QS_SAMPLE_PIPE_MODE_VIDEO;
            pCam->tPipeInfo[j].bAiispEnable = bAiispEnable;
            if (eHdrMode == AX_SNS_LINEAR_MODE || eHdrMode == AX_SNS_LINEAR_ONLY_MODE) {
                strcpy(pCam->tPipeInfo[j].szBinPath, "/opt/etc/sc438hai_sdr_t_aiisp_reduce_dual_4_4.bin");
            } else {
                strcpy(pCam->tPipeInfo[j].szBinPath, "/opt/etc/sc438hai_hdr_t_aiisp_fix_ratio.bin");
            }
        }
        strcpy(pCam->nIvpsSharpenBinPath, "null.bin");
    }
    return 0;
}
#endif

#if defined(SC533HAI_SUPPORT)
static AX_U32 __sample_case_single_sc533hai(QS_AX_CAMERA_T *pCamList, QS_SAMPLE_SNS_TYPE_E eSnsType,
        SAMPLE_ENTRY_PARAM_T *pEntryParam, QS_COMMON_SYS_ARGS_T *pCommonArgs)
{
    QS_AX_CAMERA_T *pCam = NULL;
    AX_SENSOR_REGISTER_FUNC_T *ptSnsHdl[2] = {AX_NULL};
    QS_COMMON_VIN_MODE_E eSysMode = QS_COMMON_VIN_SENSOR;
    AX_SNS_HDR_MODE_E eHdrMode = pEntryParam->eHdrMode;
    AX_S32 j = 0;
    pCommonArgs->nCamCnt = 1;
    pEntryParam->nCamCnt = 1;
    AX_BOOL bAiispEnable = (pEntryParam->nAiIspMode == E_SNS_TISP_MODE_E) ? AX_FALSE : AX_TRUE;
    pCam = &pCamList[0];
    QSDEMO_VIN_GetSnsConfig(eSnsType, &pCam->tMipiAttr, &pCam->tSnsAttr, &pCam->tSnsClkAttr, &pCam->tDevAttr, &pCam->tPipeAttr, pCam->tChnAttr);
    QSDEMO_VIN_GetSnsObj(eSnsType, ptSnsHdl);

    pCam->nDevId = 0;
    pCam->nRxDev = 0;
    pCam->nPipeId = 0;
#ifdef AX_FAST_BOOT
    AX_S32 nSingleSnsPipeId = QS_GetSingleSnsPipeId(-1);
    /* rx0:0 rx1:1 */
    if (nSingleSnsPipeId >= 0) {
        pCam->nDevId = nSingleSnsPipeId;
        pCam->nRxDev = nSingleSnsPipeId;
        pCam->nPipeId = nSingleSnsPipeId;
        /* only for ax615q rx1 */
        if (nSingleSnsPipeId == 1) {
            pCam->nI2cAddr = 0x10;
        }
        pCam->tSnsAttr.nSettingIndex = 0;
        pCam->tMipiAttr.eLaneNum = 4;
    }
    pCam->ptSnsHdl = ptSnsHdl[1];
#else
    if (eHdrMode == AX_SNS_LINEAR_MODE || eHdrMode == AX_SNS_LINEAR_ONLY_MODE) {
        pCam->tSnsAttr.nSettingIndex = 4;
    } else {
        pCam->tSnsAttr.nSettingIndex = 5;
    }
    pCam->ptSnsHdl = ptSnsHdl[0];
#endif

    pCam->tSnsClkAttr.nSnsClkIdx = 0;
    pCam->tDevBindPipe.nNum = 1;
    pCam->tDevBindPipe.nPipeId[0] = pCam->nPipeId;

    pCam->eBusType = ISP_SNS_CONNECT_I2C_TYPE;
    __set_pipe_hdr_mode(&pCam->tDevBindPipe.nHDRSel[0], eHdrMode);
    __set_vin_attr(pCam, eSnsType, eHdrMode, eSysMode, bAiispEnable, pEntryParam);
    for (j = 0; j < pCam->tDevBindPipe.nNum; j++) {
        pCam->tPipeInfo[j].ePipeMode = QS_SAMPLE_PIPE_MODE_VIDEO;
        pCam->tPipeInfo[j].bAiispEnable = bAiispEnable;
        if (eHdrMode == AX_SNS_LINEAR_MODE || eHdrMode == AX_SNS_LINEAR_ONLY_MODE) {
            strcpy(pCam->tPipeInfo[j].szBinPath, "/opt/etc/sc533hai_sdr_t_aiisp_reduce_fps.bin");
        } else {
            pCam->tSnsAttr.nSettingIndex = 5;
            pCam->tMipiAttr.nDataRate = 1656;
            strcpy(pCam->tPipeInfo[j].szBinPath, "/opt/etc/sc533hai_hdr_t_aiisp_fix_ratio.bin");
        }
    }
    strcpy(pCam->nIvpsSharpenBinPath, "null.bin");
    return 0;
}
#endif


#if defined(MIS20S1_SUPPORT)
static AX_U32 __sample_case_single_mis20s1(QS_AX_CAMERA_T *pCamList, QS_SAMPLE_SNS_TYPE_E eSnsType,
        SAMPLE_ENTRY_PARAM_T *pEntryParam, QS_COMMON_SYS_ARGS_T *pCommonArgs)
{
    QS_AX_CAMERA_T *pCam = NULL;
    AX_SENSOR_REGISTER_FUNC_T *ptSnsHdl[2] = {AX_NULL};
    QS_COMMON_VIN_MODE_E eSysMode = QS_COMMON_VIN_SENSOR;
    AX_SNS_HDR_MODE_E eHdrMode = pEntryParam->eHdrMode;
    AX_S32 j = 0;
    pCommonArgs->nCamCnt = 1;
    pEntryParam->nCamCnt = 1;
    AX_BOOL bAiispEnable = (pEntryParam->nAiIspMode == E_SNS_TISP_MODE_E) ? AX_FALSE : AX_TRUE;
    pCam = &pCamList[0];
    QSDEMO_VIN_GetSnsConfig(eSnsType, &pCam->tMipiAttr, &pCam->tSnsAttr, &pCam->tSnsClkAttr, &pCam->tDevAttr, &pCam->tPipeAttr, pCam->tChnAttr);
    QSDEMO_VIN_GetSnsObj(eSnsType, ptSnsHdl);

    AX_BOOL bIs64M = AX_FALSE;
    if ( 5 == AX_SYS_GetChipType() ) {//TODO: AX615DV100 = 0x5
        bIs64M = AX_TRUE;
    } else {
#ifdef TOTAL_MEM_SIZE_64M
        bIs64M = AX_TRUE;
#else
        bIs64M = AX_FALSE;
#endif
    }

    pCam->nDevId = 0;
    pCam->nRxDev = 0;
    pCam->nPipeId = 0;
#ifdef AX_FAST_BOOT
    AX_S32 nSingleSnsPipeId = QS_GetSingleSnsPipeId(-1);
    /* rx0:0 rx1:1 */
    if (nSingleSnsPipeId >= 0) {
        pCam->nDevId = nSingleSnsPipeId;
        pCam->nRxDev = nSingleSnsPipeId;
        pCam->nPipeId = nSingleSnsPipeId;
        /* only for ax615q rx1 */
        if (nSingleSnsPipeId == 1) {
            pCam->nI2cAddr = 0x10;
        }
        pCam->tSnsAttr.nSettingIndex = 0;
        pCam->tMipiAttr.eLaneNum = 4;
    }
    pCam->ptSnsHdl = ptSnsHdl[1];
#else
    if (eHdrMode == AX_SNS_LINEAR_MODE || eHdrMode == AX_SNS_LINEAR_ONLY_MODE) {
        // pCam->tSnsAttr.nSettingIndex = bIs64M ? 5 : 4;

        if(bIs64M) {
            pCam->tSnsAttr.nSettingIndex = 0;//30fps for 64M
        } else {
            pCam->tSnsAttr.nSettingIndex = 4;//60fps for other
        }
    } else {
        // pCam->tSnsAttr.nSettingIndex = bIs64M ? 6 : 1;
        pCam->tSnsAttr.nSettingIndex = 1;
    }
    // if(bIs64M) {
    //     gtPrivatePoolSingleMis20s1[0].nBlkCnt = 7;
    // }
    pCam->ptSnsHdl = ptSnsHdl[0];
#endif

    pCam->tSnsClkAttr.nSnsClkIdx = 0;
    pCam->tDevBindPipe.nNum = 1;
    pCam->tDevBindPipe.nPipeId[0] = pCam->nPipeId;

    pCam->eBusType = ISP_SNS_CONNECT_I2C_TYPE;
    __set_pipe_hdr_mode(&pCam->tDevBindPipe.nHDRSel[0], eHdrMode);
    __set_vin_attr(pCam, eSnsType, eHdrMode, eSysMode, bAiispEnable, pEntryParam);

    for (j = 0; j < pCam->tDevBindPipe.nNum; j++) {
        pCam->tPipeInfo[j].ePipeMode = QS_SAMPLE_PIPE_MODE_VIDEO;
        pCam->tPipeInfo[j].bAiispEnable = bAiispEnable;
        if (eHdrMode == AX_SNS_LINEAR_MODE || eHdrMode == AX_SNS_LINEAR_ONLY_MODE) {
            if(bIs64M) {
                strcpy(pCam->tPipeInfo[j].szBinPath, "/opt/etc/mis20s1_sdr_t_aiisp_reduce_fps_64M.bin");
            } else {
                strcpy(pCam->tPipeInfo[j].szBinPath, "/opt/etc/mis20s1_sdr_t_aiisp_reduce_fps_aov.bin");
            }
        } else {
            if(bIs64M) {
                strcpy(pCam->tPipeInfo[j].szBinPath, "/opt/etc/mis20s1_hdr_t_aiisp_fix_ratio_64M.bin");
            } else {
                strcpy(pCam->tPipeInfo[j].szBinPath, "/opt/etc/mis20s1_hdr_t_aiisp_fix_ratio.bin");
            }
        }
    }
    strcpy(pCam->nIvpsSharpenBinPath, "null.bin");
    return 0;
}
static AX_U32 __sample_case_double_mis20s1(QS_AX_CAMERA_T *pCamList, QS_SAMPLE_SNS_TYPE_E eSnsType,
        SAMPLE_ENTRY_PARAM_T *pEntryParam, QS_COMMON_SYS_ARGS_T *pCommonArgs)
{
    QS_AX_CAMERA_T *pCam = NULL;
    AX_SENSOR_REGISTER_FUNC_T *ptSnsHdl[2] = {AX_NULL};
    QS_COMMON_VIN_MODE_E eSysMode = QS_COMMON_VIN_SENSOR;
    AX_SNS_HDR_MODE_E eHdrMode = pEntryParam->eHdrMode;
    AX_BOOL bAiispEnable = (pEntryParam->nAiIspMode == E_SNS_TISP_MODE_E) ? AX_FALSE : AX_TRUE;
    AX_S32 j = 0, i = 0;
    pCommonArgs->nCamCnt = 2;
    pEntryParam->nCamCnt = 2;

    for (i = 0; i < pCommonArgs->nCamCnt; i++) {
        pCam = &pCamList[i];
        pCam->nNumber = i;
        QSDEMO_VIN_GetSnsConfig(eSnsType, &pCam->tMipiAttr, &pCam->tSnsAttr, &pCam->tSnsClkAttr, &pCam->tDevAttr, &pCam->tPipeAttr, pCam->tChnAttr);
        QSDEMO_VIN_GetSnsObj(eSnsType, ptSnsHdl);

        pCam->nDevId = i;
        pCam->nRxDev = i;
        pCam->nPipeId = i;
    #ifdef AX_FAST_BOOT
        AX_S32 nSingleSnsPipeId = QS_GetSingleSnsPipeId(-1);
        /* rx0:0 rx1:1 */
        if (nSingleSnsPipeId >= 0) {
            pCam->nDevId = nSingleSnsPipeId;
            pCam->nRxDev = nSingleSnsPipeId;
            pCam->nPipeId = nSingleSnsPipeId;
            /* only for ax615q rx1 */
            if (nSingleSnsPipeId == 1) {
                pCam->nI2cAddr = 0x10;
            }
            pCam->tSnsAttr.nSettingIndex = 0;
            pCam->tMipiAttr.eLaneNum = 4;
        }
        pCam->ptSnsHdl = ptSnsHdl[1];
    #else
        if (eHdrMode == AX_SNS_LINEAR_MODE || eHdrMode == AX_SNS_LINEAR_ONLY_MODE) {
            pCam->tSnsAttr.nSettingIndex = 4;
        } else {
            pCam->tSnsAttr.nSettingIndex = 1;
        }
        pCam->ptSnsHdl = ptSnsHdl[0];
    #endif

        pCam->tSnsClkAttr.nSnsClkIdx = 0;
        pCam->tDevBindPipe.nNum = 1;
        pCam->tDevBindPipe.nPipeId[0] = pCam->nPipeId;
        pCam->eBusType = ISP_SNS_CONNECT_I2C_TYPE;
        pCam->tMipiAttr.eLaneNum = AX_MIPI_DATA_LANE_2;

        __set_pipe_hdr_mode(&pCam->tDevBindPipe.nHDRSel[0], eHdrMode);
        __set_vin_attr(pCam, eSnsType, eHdrMode, eSysMode, bAiispEnable, pEntryParam);
        for (j = 0; j < pCam->tDevBindPipe.nNum; j++) {
            pCam->tPipeInfo[j].ePipeMode = QS_SAMPLE_PIPE_MODE_VIDEO;
            pCam->tPipeInfo[j].bAiispEnable = bAiispEnable;
            if (eHdrMode == AX_SNS_LINEAR_MODE || eHdrMode == AX_SNS_LINEAR_ONLY_MODE) {
                strcpy(pCam->tPipeInfo[j].szBinPath, "/opt/etc/mis20s1_sdr_t_aiisp_reduce_fps_dual_2_2_aov.bin");
            } else {
                strcpy(pCam->tPipeInfo[j].szBinPath, "/opt/etc/mis20s1_hdr_t_aiisp_fix_ratio.bin");
            }
        }
        strcpy(pCam->nIvpsSharpenBinPath, "null.bin");
    }
    return 0;
}
#endif

#if defined(SENSOR_DUMMY_SUPPORT)
static AX_U32 __sample_case_single_dummy(QS_AX_CAMERA_T *pCamList, QS_SAMPLE_SNS_TYPE_E eSnsType,
        SAMPLE_ENTRY_PARAM_T *pEntryParam, QS_COMMON_SYS_ARGS_T *pCommonArgs)
{
    QS_AX_CAMERA_T *pCam = NULL;
    AX_SENSOR_REGISTER_FUNC_T *ptSnsHdl[2] = {AX_NULL};
    QS_COMMON_VIN_MODE_E eSysMode = QS_COMMON_VIN_TPG;
    AX_SNS_HDR_MODE_E eHdrMode = pEntryParam->eHdrMode;
    AX_S32 j = 0;
    pCommonArgs->nCamCnt = 1;
    pEntryParam->nCamCnt = 1;
    AX_BOOL bAiispEnable = (pEntryParam->nAiIspMode == E_SNS_TISP_MODE_E) ? AX_FALSE : AX_TRUE;
    pCam = &pCamList[0];
    QSDEMO_VIN_GetSnsConfig(eSnsType, &pCam->tMipiAttr, &pCam->tSnsAttr, &pCam->tSnsClkAttr, &pCam->tDevAttr, &pCam->tPipeAttr, pCam->tChnAttr);
    QSDEMO_VIN_GetSnsObj(eSnsType, ptSnsHdl);

    pCam->nDevId = 0;
    pCam->nRxDev = 0;
    pCam->nPipeId = 0;
#ifdef AX_FAST_BOOT
    AX_S32 nSingleSnsPipeId = QS_GetSingleSnsPipeId(-1);
    /* rx0:0 rx1:1 */
    if (nSingleSnsPipeId >= 0) {
        pCam->nDevId = nSingleSnsPipeId;
        pCam->nRxDev = nSingleSnsPipeId;
        pCam->nPipeId = nSingleSnsPipeId;
        /* only for ax615q rx1 */
        if (nSingleSnsPipeId == 1) {
            pCam->nI2cAddr = 0x10;
        }
        pCam->tSnsAttr.nSettingIndex = 0;
        pCam->tMipiAttr.eLaneNum = 4;
    }
#endif

    pCam->tSnsClkAttr.nSnsClkIdx = 0;
    pCam->tDevBindPipe.nNum = 1;
    pCam->tDevBindPipe.nPipeId[0] = pCam->nPipeId;

#if defined(AX_FAST_BOOT)
    pCam->ptSnsHdl = ptSnsHdl[1];
#else
    pCam->ptSnsHdl = ptSnsHdl[0];
#endif
    pCam->eBusType = ISP_SNS_CONNECT_I2C_TYPE;
    __set_pipe_hdr_mode(&pCam->tDevBindPipe.nHDRSel[0], eHdrMode);
    __set_vin_attr(pCam, eSnsType, eHdrMode, eSysMode, bAiispEnable, pEntryParam);
    for (j = 0; j < pCam->tDevBindPipe.nNum; j++) {
        pCam->tPipeInfo[j].ePipeMode = QS_SAMPLE_PIPE_MODE_VIDEO;
        pCam->tPipeInfo[j].bAiispEnable = bAiispEnable;
        //strcpy(pCam->tPipeInfo[j].szBinPath, "/opt/etc/dummysensor_sdr.bin");
    }
    strcpy(pCam->nIvpsSharpenBinPath, "null.bin");
    return 0;
}
#endif

#if defined(SC465SL_SUPPORT)
static AX_U32 __sample_case_single_sc465sl(QS_AX_CAMERA_T *pCamList, QS_SAMPLE_SNS_TYPE_E eSnsType,
        SAMPLE_ENTRY_PARAM_T *pEntryParam, QS_COMMON_SYS_ARGS_T *pCommonArgs)
{
    QS_AX_CAMERA_T *pCam = NULL;
    AX_SENSOR_REGISTER_FUNC_T *ptSnsHdl[2] = {AX_NULL};
    QS_COMMON_VIN_MODE_E eSysMode = QS_COMMON_VIN_SENSOR;
    AX_SNS_HDR_MODE_E eHdrMode = pEntryParam->eHdrMode;
    AX_S32 j = 0;
    pCommonArgs->nCamCnt = 1;
    pEntryParam->nCamCnt = 1;
    AX_BOOL bAiispEnable = (pEntryParam->nAiIspMode == E_SNS_TISP_MODE_E) ? AX_FALSE : AX_TRUE;
    pCam = &pCamList[0];
    QSDEMO_VIN_GetSnsConfig(eSnsType, &pCam->tMipiAttr, &pCam->tSnsAttr, &pCam->tSnsClkAttr, &pCam->tDevAttr, &pCam->tPipeAttr, pCam->tChnAttr);
    QSDEMO_VIN_GetSnsObj(eSnsType, ptSnsHdl);

    pCam->nDevId = 0;
    pCam->nRxDev = 0;
    pCam->nPipeId = 0;
#ifdef AX_FAST_BOOT
    AX_S32 nSingleSnsPipeId = QS_GetSingleSnsPipeId(-1);
    /* rx0:0 rx1:1 */
    if (nSingleSnsPipeId >= 0) {
        pCam->nDevId = nSingleSnsPipeId;
        pCam->nRxDev = nSingleSnsPipeId;
        pCam->nPipeId = nSingleSnsPipeId;
        /* only for ax615q rx1 */
        if (nSingleSnsPipeId == 1) {
            pCam->nI2cAddr = 0x10;
        }
        pCam->tSnsAttr.nSettingIndex = 0;
        pCam->tMipiAttr.eLaneNum = 4;
    }
    pCam->ptSnsHdl = ptSnsHdl[1];
#else
    // sensor auto adapt
    // if (eHdrMode == AX_SNS_LINEAR_MODE || eHdrMode == AX_SNS_LINEAR_ONLY_MODE) {
    //     pCam->tSnsAttr.nSettingIndex = 6;
    // } else {
    //     pCam->tSnsAttr.nSettingIndex = 7;
    // }
    pCam->ptSnsHdl = ptSnsHdl[0];
#endif

    pCam->tSnsClkAttr.nSnsClkIdx = 0;
    pCam->tDevBindPipe.nNum = 1;
    pCam->tDevBindPipe.nPipeId[0] = pCam->nPipeId;

    pCam->eBusType = ISP_SNS_CONNECT_I2C_TYPE;
    __set_pipe_hdr_mode(&pCam->tDevBindPipe.nHDRSel[0], eHdrMode);
    __set_vin_attr(pCam, eSnsType, eHdrMode, eSysMode, bAiispEnable, pEntryParam);
    for (j = 0; j < pCam->tDevBindPipe.nNum; j++) {
        pCam->tPipeInfo[j].ePipeMode = QS_SAMPLE_PIPE_MODE_VIDEO;
        pCam->tPipeInfo[j].bAiispEnable = bAiispEnable;
        if (eHdrMode == AX_SNS_LINEAR_MODE || eHdrMode == AX_SNS_LINEAR_ONLY_MODE) {
            strcpy(pCam->tPipeInfo[j].szBinPath, "/opt/etc/sc465sl_sdr_t_aiisp_reduce_fps.bin");
        } else {
            strcpy(pCam->tPipeInfo[j].szBinPath, "/opt/etc/sc465sl_hdr_t_aiisp_fix_ratio.bin");
        }
    }
    strcpy(pCam->nIvpsSharpenBinPath, "null.bin");
    return 0;
}
static AX_U32 __sample_case_double_sc465sl(QS_AX_CAMERA_T *pCamList, QS_SAMPLE_SNS_TYPE_E eSnsType,
        SAMPLE_ENTRY_PARAM_T *pEntryParam, QS_COMMON_SYS_ARGS_T *pCommonArgs)
{
    QS_AX_CAMERA_T *pCam = NULL;
    AX_SENSOR_REGISTER_FUNC_T *ptSnsHdl[2] = {AX_NULL};
    QS_COMMON_VIN_MODE_E eSysMode = QS_COMMON_VIN_SENSOR;
    AX_SNS_HDR_MODE_E eHdrMode = pEntryParam->eHdrMode;
    AX_S32 j = 0, i = 0;
    pCommonArgs->nCamCnt = 2;
    pEntryParam->nCamCnt = 2;
    AX_BOOL bAiispEnable = (pEntryParam->nAiIspMode == E_SNS_TISP_MODE_E) ? AX_FALSE : AX_TRUE;

    for (i = 0; i < pCommonArgs->nCamCnt; i++) {
        pCam = &pCamList[i];
        pCam->nNumber = i;
        QSDEMO_VIN_GetSnsConfig(eSnsType, &pCam->tMipiAttr, &pCam->tSnsAttr, &pCam->tSnsClkAttr, &pCam->tDevAttr, &pCam->tPipeAttr, pCam->tChnAttr);
        QSDEMO_VIN_GetSnsObj(eSnsType, ptSnsHdl);

        pCam->nDevId = i;
        pCam->nRxDev = i;
        pCam->nPipeId = i;
    #ifdef AX_FAST_BOOT
        AX_S32 nSingleSnsPipeId = QS_GetSingleSnsPipeId(-1);
        /* rx0:0 rx1:1 */
        if (nSingleSnsPipeId >= 0) {
            pCam->nDevId = nSingleSnsPipeId;
            pCam->nRxDev = nSingleSnsPipeId;
            pCam->nPipeId = nSingleSnsPipeId;
            /* only for ax615q rx1 */
            if (nSingleSnsPipeId == 1) {
                pCam->nI2cAddr = 0x10;
            }
            pCam->tSnsAttr.nSettingIndex = 0;
            pCam->tMipiAttr.eLaneNum = 4;
        }
        pCam->ptSnsHdl = ptSnsHdl[1];
    #else
        // sensor auto adapt
        if (eHdrMode == AX_SNS_LINEAR_MODE || eHdrMode == AX_SNS_LINEAR_ONLY_MODE) {
            pCam->tSnsAttr.nSettingIndex = 8; //27M_1296MBPS_2LANE_2560x1440_12BIT_SDR_20FPS
            pCam->tSnsAttr.eRawType = AX_RT_RAW12;
            pCam->tMipiAttr.nDataRate = 1296;
            pCam->tDevAttr.ePixelFmt = AX_FORMAT_BAYER_RAW_12BPP_PACKED;
        } else {
            pCam->tSnsAttr.nSettingIndex = 9; //27M_1080MBPS_2LANE_2560x1440_10BIT_HDR_2DOL_20FPS
            pCam->tSnsAttr.eRawType = AX_RT_RAW10;
            pCam->tMipiAttr.nDataRate = 1080;
            pCam->tDevAttr.ePixelFmt = AX_FORMAT_BAYER_RAW_10BPP_PACKED;
        }
        pCam->ptSnsHdl = ptSnsHdl[0];
    #endif

        pCam->tSnsClkAttr.nSnsClkIdx = 0;
        pCam->tDevBindPipe.nNum = 1;
        pCam->tDevBindPipe.nPipeId[0] = pCam->nPipeId;
        pCam->eBusType = ISP_SNS_CONNECT_I2C_TYPE;
        pCam->tMipiAttr.eLaneNum = AX_MIPI_DATA_LANE_2;
        pCam->tSnsClkAttr.eSnsClkRate = AX_SNS_CLK_27M;

        __set_pipe_hdr_mode(&pCam->tDevBindPipe.nHDRSel[0], eHdrMode);
        __set_vin_attr(pCam, eSnsType, eHdrMode, eSysMode, bAiispEnable, pEntryParam);
        for (j = 0; j < pCam->tDevBindPipe.nNum; j++) {
            pCam->tPipeInfo[j].ePipeMode = QS_SAMPLE_PIPE_MODE_VIDEO;
            pCam->tPipeInfo[j].bAiispEnable = bAiispEnable;
            if (eHdrMode == AX_SNS_LINEAR_MODE || eHdrMode == AX_SNS_LINEAR_ONLY_MODE) {
                strcpy(pCam->tPipeInfo[j].szBinPath, "/opt/etc/sc465sl_sdr_t_aiisp_reduce_fps.bin");
            } else {
                strcpy(pCam->tPipeInfo[j].szBinPath, "/opt/etc/sc465sl_hdr_t_aiisp_fix_ratio.bin");
            }
        }
        strcpy(pCam->nIvpsSharpenBinPath, "null.bin");
    }
    return 0;
}
#endif


AX_S32 qs_cam_config(SAMPLE_ENTRY_PARAM_T *pEntryParam, QS_COMMON_SYS_ARGS_T *pCommonArgs, QS_COMMON_SYS_ARGS_T *pPrivArgs, QS_AX_CAMERA_T* pCamList)
{
    QS_SAMPLE_SNS_TYPE_E  eSnsType = QS_SAMPLE_SNS_TYPE_NONE;

    if (pEntryParam->nCaseId == QS_CASE_SC200AI_SINGLE) {
#if defined(SC200AI_SUPPORT)
        eSnsType = (QS_SAMPLE_SNS_TYPE_E)(SMARTSENS_SC200AI);

        /* comm pool config */
        if (pEntryParam->nIvpsVencLinkMode == (AX_S32)AX_MOD_DATA_FLOW_SW_LOW_MEM) {
            pCommonArgs->nPoolCfgCnt = sizeof(gtSysCommPoolSingleSc200ai) / sizeof(gtSysCommPoolSingleSc200ai[0]);
            pCommonArgs->pPoolCfg = gtSysCommPoolSingleSc200ai;
        } else {
            pCommonArgs->nPoolCfgCnt = sizeof(gtSysCommPoolSingleSc200ai_offline) / sizeof(gtSysCommPoolSingleSc200ai_offline[0]);
            pCommonArgs->pPoolCfg = gtSysCommPoolSingleSc200ai_offline;
        }

        /* private pool config */
        pPrivArgs->nPoolCfgCnt = sizeof(gtPrivatePoolSingleSc200ai) / sizeof(gtPrivatePoolSingleSc200ai[0]);
        pPrivArgs->pPoolCfg = gtPrivatePoolSingleSc200ai;

        /* cams config */
        __sample_case_single_sc200ai(pCamList, eSnsType, pEntryParam, pCommonArgs);
#endif
    } else if (pEntryParam->nCaseId == QS_CASE_SC200AI_DOUBLE) {
#if defined(SC200AI_SUPPORT)
        eSnsType = (QS_SAMPLE_SNS_TYPE_E)(SMARTSENS_SC200AI);

        /* comm pool config */
        if (pEntryParam->nIvpsVencLinkMode == (AX_S32)AX_MOD_DATA_FLOW_SW_LOW_MEM) {
            pCommonArgs->nPoolCfgCnt = sizeof(gtSysCommPoolDoubleSc200ai) / sizeof(gtSysCommPoolDoubleSc200ai[0]);
            pCommonArgs->pPoolCfg = gtSysCommPoolDoubleSc200ai;
        } else {
            pCommonArgs->nPoolCfgCnt = sizeof(gtSysCommPoolDoubleSc200ai_offline) / sizeof(gtSysCommPoolDoubleSc200ai_offline[0]);
            pCommonArgs->pPoolCfg = gtSysCommPoolDoubleSc200ai_offline;
        }

        /* private pool config */
        pPrivArgs->nPoolCfgCnt = sizeof(gtPrivatePoolDoubleSc200ai) / sizeof(gtPrivatePoolDoubleSc200ai[0]);
        pPrivArgs->pPoolCfg = gtPrivatePoolDoubleSc200ai;

        /* cams config */
        __sample_case_double_sc200ai(pCamList, eSnsType, pEntryParam, pCommonArgs);
#endif
    } else if (pEntryParam->nCaseId == QS_CASE_SC200AI_TRIPLE) {
#if defined(SC200AI_SUPPORT)
        eSnsType = (QS_SAMPLE_SNS_TYPE_E)(SMARTSENS_SC200AI);

        /* comm pool config */
        if (pEntryParam->nIvpsVencLinkMode == (AX_S32)AX_MOD_DATA_FLOW_SW_LOW_MEM) {
            pCommonArgs->nPoolCfgCnt = sizeof(gtSysCommPoolDoubleSc200ai) / sizeof(gtSysCommPoolDoubleSc200ai[0]);
            pCommonArgs->pPoolCfg = gtSysCommPoolDoubleSc200ai;
        } else {
            pCommonArgs->nPoolCfgCnt = sizeof(gtSysCommPoolDoubleSc200ai_offline) / sizeof(gtSysCommPoolDoubleSc200ai_offline[0]);
            pCommonArgs->pPoolCfg = gtSysCommPoolDoubleSc200ai_offline;
        }

        /* private pool config */
        pPrivArgs->nPoolCfgCnt = sizeof(gtPrivatePoolDoubleSc200ai) / sizeof(gtPrivatePoolDoubleSc200ai[0]);
        pPrivArgs->pPoolCfg = gtPrivatePoolDoubleSc200ai;

        /* cams config */
        __sample_case_triple_sc200ai(pCamList, eSnsType, pEntryParam, pCommonArgs);
#endif
    } else if (pEntryParam->nCaseId == QS_CASE_SC235HAI_SINGLE) {
#if defined(SC235HAI_SUPPORT)
        eSnsType = (QS_SAMPLE_SNS_TYPE_E)(SMARTSENS_SC235HAI);

        /* comm pool config */
        if (pEntryParam->nIvpsVencLinkMode == (AX_S32)AX_MOD_DATA_FLOW_SW_LOW_MEM) {
            pCommonArgs->nPoolCfgCnt = sizeof(gtSysCommPoolSingleSc235hai) / sizeof(gtSysCommPoolSingleSc235hai[0]);
            pCommonArgs->pPoolCfg = gtSysCommPoolSingleSc235hai;
        } else {
            pCommonArgs->nPoolCfgCnt = sizeof(gtSysCommPoolSingleSc235hai_offline) / sizeof(gtSysCommPoolSingleSc235hai_offline[0]);
            pCommonArgs->pPoolCfg = gtSysCommPoolSingleSc235hai_offline;
        }

        /* private pool config */
        pPrivArgs->nPoolCfgCnt = sizeof(gtPrivatePoolSingleSc235hai) / sizeof(gtPrivatePoolSingleSc235hai[0]);
        pPrivArgs->pPoolCfg = gtPrivatePoolSingleSc235hai;

        /* cams config */
        __sample_case_single_sc235hai(pCamList, eSnsType, pEntryParam, pCommonArgs);
#endif
    } else if (pEntryParam->nCaseId == QS_CASE_SC235HAI_DOUBLE) {
#if defined(SC235HAI_SUPPORT)
        eSnsType = (QS_SAMPLE_SNS_TYPE_E)(SMARTSENS_SC235HAI);

        /* comm pool config */
        if (pEntryParam->nIvpsVencLinkMode == (AX_S32)AX_MOD_DATA_FLOW_SW_LOW_MEM) {
            pCommonArgs->nPoolCfgCnt = sizeof(gtSysCommPoolDoubleSc235hai) / sizeof(gtSysCommPoolDoubleSc235hai[0]);
            pCommonArgs->pPoolCfg = gtSysCommPoolDoubleSc235hai;
        } else {
            pCommonArgs->nPoolCfgCnt = sizeof(gtSysCommPoolDoubleSc235hai_offline) / sizeof(gtSysCommPoolDoubleSc235hai_offline[0]);
            pCommonArgs->pPoolCfg = gtSysCommPoolDoubleSc235hai_offline;
        }

        /* private pool config */
        pPrivArgs->nPoolCfgCnt = sizeof(gtPrivatePoolDoubleSc235hai) / sizeof(gtPrivatePoolDoubleSc235hai[0]);
        pPrivArgs->pPoolCfg = gtPrivatePoolDoubleSc235hai;

        /* cams config */
        __sample_case_double_sc235hai(pCamList, eSnsType, pEntryParam, pCommonArgs);
#endif
    } else if (pEntryParam->nCaseId == QS_CASE_SC450AI_SINGLE) {
#if defined(SC450AI_SUPPORT)
        eSnsType = SMARTSENS_SC450AI;
        /* comm pool config */
        if (pEntryParam->nIvpsVencLinkMode == (AX_S32)AX_MOD_DATA_FLOW_SW_LOW_MEM) {
            pCommonArgs->nPoolCfgCnt = sizeof(gtSysCommPoolSingleSc450ai) / sizeof(gtSysCommPoolSingleSc450ai[0]);
            pCommonArgs->pPoolCfg = gtSysCommPoolSingleSc450ai;
        } else {
            pCommonArgs->nPoolCfgCnt = sizeof(gtSysCommPoolSingleSc450ai_offline) / sizeof(gtSysCommPoolSingleSc450ai_offline[0]);
            pCommonArgs->pPoolCfg = gtSysCommPoolSingleSc450ai_offline;
        }

        /* private pool config */
        pPrivArgs->nPoolCfgCnt = sizeof(gtPrivatePoolSingleSc450ai) / sizeof(gtPrivatePoolSingleSc450ai[0]);
        pPrivArgs->pPoolCfg = gtPrivatePoolSingleSc450ai;

        /* cams config */
        __sample_case_single_sc450ai(pCamList, eSnsType, pEntryParam, pCommonArgs);
#endif
    } else if (pEntryParam->nCaseId == QS_CASE_SC438HAI_SINGLE) {
#if defined(SC438HAI_SUPPORT)
        eSnsType = SMARTSENS_SC438HAI;
        /* comm pool config */
        if (pEntryParam->nIvpsVencLinkMode == (AX_S32)AX_MOD_DATA_FLOW_SW_LOW_MEM) {
            pCommonArgs->nPoolCfgCnt = sizeof(gtSysCommPoolSingleSc438hai) / sizeof(gtSysCommPoolSingleSc438hai[0]);
            pCommonArgs->pPoolCfg = gtSysCommPoolSingleSc438hai;
        } else {
            pCommonArgs->nPoolCfgCnt = sizeof(gtSysCommPoolSingleSc438hai_offline) / sizeof(gtSysCommPoolSingleSc438hai_offline[0]);
            pCommonArgs->pPoolCfg = gtSysCommPoolSingleSc438hai_offline;
        }

        /* private pool config */
        pPrivArgs->nPoolCfgCnt = sizeof(gtPrivatePoolSingleSc438hai) / sizeof(gtPrivatePoolSingleSc438hai[0]);
        pPrivArgs->pPoolCfg = gtPrivatePoolSingleSc438hai;

        /* cams config */
        __sample_case_single_sc438hai(pCamList, eSnsType, pEntryParam, pCommonArgs);
#endif
    } else if (pEntryParam->nCaseId == QS_CASE_SC438HAI_DOUBLE) {
#if defined(SC438HAI_SUPPORT)
        eSnsType = SMARTSENS_SC438HAI;
        /* comm pool config */
        if (pEntryParam->nIvpsVencLinkMode == (AX_S32)AX_MOD_DATA_FLOW_SW_LOW_MEM) {
            pCommonArgs->nPoolCfgCnt = sizeof(gtSysCommPoolDoubleSc438hai) / sizeof(gtSysCommPoolDoubleSc438hai[0]);
            pCommonArgs->pPoolCfg = gtSysCommPoolDoubleSc438hai;
        } else {
            pCommonArgs->nPoolCfgCnt = sizeof(gtSysCommPoolDoubleSc438hai_offline) / sizeof(gtSysCommPoolDoubleSc438hai_offline[0]);
            pCommonArgs->pPoolCfg = gtSysCommPoolDoubleSc438hai_offline;
        }

        /* private pool config */
        pPrivArgs->nPoolCfgCnt = sizeof(gtPrivatePoolDoubleSc438hai) / sizeof(gtPrivatePoolDoubleSc438hai[0]);
        pPrivArgs->pPoolCfg = gtPrivatePoolDoubleSc438hai;

        /* cams config */
        __sample_case_double_sc438hai(pCamList, eSnsType, pEntryParam, pCommonArgs);
#endif
    } else if (pEntryParam->nCaseId == QS_CASE_SC533HAI_SINGLE) {
#if defined(SC533HAI_SUPPORT)
        eSnsType = SMARTSENS_SC533HAI;
        /* comm pool config */
        if (pEntryParam->nIvpsVencLinkMode == (AX_S32)AX_MOD_DATA_FLOW_SW_LOW_MEM) {
            pCommonArgs->nPoolCfgCnt = sizeof(gtSysCommPoolSingleSc533hai) / sizeof(gtSysCommPoolSingleSc533hai[0]);
            pCommonArgs->pPoolCfg = gtSysCommPoolSingleSc533hai;
        } else {
            pCommonArgs->nPoolCfgCnt = sizeof(gtSysCommPoolSingleSc533hai_offline) / sizeof(gtSysCommPoolSingleSc533hai_offline[0]);
            pCommonArgs->pPoolCfg = gtSysCommPoolSingleSc533hai_offline;
        }

        /* private pool config */
        pPrivArgs->nPoolCfgCnt = sizeof(gtPrivatePoolSingleSc533hai) / sizeof(gtPrivatePoolSingleSc533hai[0]);
        pPrivArgs->pPoolCfg = gtPrivatePoolSingleSc533hai;

        /* cams config */
        __sample_case_single_sc533hai(pCamList, eSnsType, pEntryParam, pCommonArgs);
#endif
    } else if (pEntryParam->nCaseId == QS_CASE_OS04D10_SINGLE) {
#if defined(OS04D10_SUPPORT)
        eSnsType = OMNIVISION_OS04D10;
        /* comm pool config */
        if (pEntryParam->nIvpsVencLinkMode == (AX_S32)AX_MOD_DATA_FLOW_SW_LOW_MEM) {
            pCommonArgs->nPoolCfgCnt = sizeof(gtSysCommPoolSingleOs04d10) / sizeof(gtSysCommPoolSingleOs04d10[0]);
            pCommonArgs->pPoolCfg = gtSysCommPoolSingleOs04d10;
        } else {
            pCommonArgs->nPoolCfgCnt = sizeof(gtSysCommPoolSingleOs04d10_offline) / sizeof(gtSysCommPoolSingleOs04d10_offline[0]);
            pCommonArgs->pPoolCfg = gtSysCommPoolSingleOs04d10_offline;
        }

        /* private pool config */
        pPrivArgs->nPoolCfgCnt = sizeof(gtPrivatePoolSingleOs04d10) / sizeof(gtPrivatePoolSingleOs04d10[0]);
        pPrivArgs->pPoolCfg = gtPrivatePoolSingleOs04d10;

        /* cams config */
        __sample_case_single_os04d10(pCamList, eSnsType, pEntryParam, pCommonArgs);
#endif
    } else if (pEntryParam->nCaseId == QS_CASE_OS04D10_DOUBLE) {
#if defined(OS04D10_SUPPORT)
        eSnsType = OMNIVISION_OS04D10;
        /* comm pool config */
        if (pEntryParam->nIvpsVencLinkMode == (AX_S32)AX_MOD_DATA_FLOW_SW_LOW_MEM) {
            pCommonArgs->nPoolCfgCnt = sizeof(gtSysCommPoolDoubleOs04d10) / sizeof(gtSysCommPoolDoubleOs04d10[0]);
            pCommonArgs->pPoolCfg = gtSysCommPoolDoubleOs04d10;
        } else {
            pCommonArgs->nPoolCfgCnt = sizeof(gtSysCommPoolDoubleOs04d10_offline) / sizeof(gtSysCommPoolDoubleOs04d10_offline[0]);
            pCommonArgs->pPoolCfg = gtSysCommPoolDoubleOs04d10_offline;
        }

        /* private pool config */
        pPrivArgs->nPoolCfgCnt = sizeof(gtPrivatePoolDoubleOs04d10) / sizeof(gtPrivatePoolDoubleOs04d10[0]);
        pPrivArgs->pPoolCfg = gtPrivatePoolDoubleOs04d10;

        /* cams config */
        __sample_case_double_os04d10(pCamList, eSnsType, pEntryParam, pCommonArgs);
#endif
    } else if (pEntryParam->nCaseId == QS_CASE_OS04A10_SINGLE) {
#if defined(OS04A10_SUPPORT)
        eSnsType = OMNIVISION_OS04A10;

        /* comm pool config */
        if (pEntryParam->nIvpsVencLinkMode == (AX_S32)AX_MOD_DATA_FLOW_SW_LOW_MEM) {
            pCommonArgs->nPoolCfgCnt = sizeof(gtSysCommPoolSingleOs04a10) / sizeof(gtSysCommPoolSingleOs04a10[0]);
            pCommonArgs->pPoolCfg = gtSysCommPoolSingleOs04a10;
        } else {
            pCommonArgs->nPoolCfgCnt = sizeof(gtSysCommPoolSingleOs04a10_offline) / sizeof(gtSysCommPoolSingleOs04a10_offline[0]);
            pCommonArgs->pPoolCfg = gtSysCommPoolSingleOs04a10_offline;
        }

        /* private pool config */
        pPrivArgs->nPoolCfgCnt = sizeof(gtPrivatePoolSingleOs04a10) / sizeof(gtPrivatePoolSingleOs04a10[0]);
        pPrivArgs->pPoolCfg = gtPrivatePoolSingleOs04a10;

        /* cams config */
        __sample_case_single_os04a10(pCamList, eSnsType, pEntryParam, pCommonArgs);
#endif
    } else if (pEntryParam->nCaseId == QS_CASE_OS04A10_DOUBLE) {
#if defined(OS04A10_SUPPORT)
        eSnsType = OMNIVISION_OS04A10;

        /* comm pool config */
        if (pEntryParam->nIvpsVencLinkMode == (AX_S32)AX_MOD_DATA_FLOW_SW_LOW_MEM) {
            pCommonArgs->nPoolCfgCnt = sizeof(gtSysCommPoolDoubleOs04a10) / sizeof(gtSysCommPoolDoubleOs04a10[0]);
            pCommonArgs->pPoolCfg = gtSysCommPoolDoubleOs04a10;
        } else {
            pCommonArgs->nPoolCfgCnt = sizeof(gtSysCommPoolDoubleOs04a10_offline) / sizeof(gtSysCommPoolDoubleOs04a10_offline[0]);
            pCommonArgs->pPoolCfg = gtSysCommPoolDoubleOs04a10_offline;
        }

        /* private pool config */
        pPrivArgs->nPoolCfgCnt = sizeof(gtPrivatePoolDoubleOs04a10) / sizeof(gtPrivatePoolDoubleOs04a10[0]);
        pPrivArgs->pPoolCfg = gtPrivatePoolDoubleOs04a10;

        /* cams config */
        __sample_case_double_os04a10(pCamList, eSnsType, pEntryParam, pCommonArgs);
#endif
    } else if (pEntryParam->nCaseId == QS_CASE_MIS20S1_SINGLE) {
#if defined(MIS20S1_SUPPORT)
        eSnsType = IMAGEDESIGN_MIS20S1;

        /* comm pool config */
        if (pEntryParam->nIvpsVencLinkMode == (AX_S32)AX_MOD_DATA_FLOW_SW_LOW_MEM) {
            pCommonArgs->nPoolCfgCnt = sizeof(gtSysCommPoolSingleMis20s1) / sizeof(gtSysCommPoolSingleMis20s1[0]);
            pCommonArgs->pPoolCfg = gtSysCommPoolSingleMis20s1;
        } else {
            pCommonArgs->nPoolCfgCnt = sizeof(gtSysCommPoolSingleMis20s1_offline) / sizeof(gtSysCommPoolSingleMis20s1_offline[0]);
            pCommonArgs->pPoolCfg = gtSysCommPoolSingleMis20s1_offline;
        }

        /* private pool config */
        pPrivArgs->nPoolCfgCnt = sizeof(gtPrivatePoolSingleMis20s1) / sizeof(gtPrivatePoolSingleMis20s1[0]);
        pPrivArgs->pPoolCfg = gtPrivatePoolSingleMis20s1;

        /* cams config */
        __sample_case_single_mis20s1(pCamList, eSnsType, pEntryParam, pCommonArgs);
#endif
    } else if (pEntryParam->nCaseId == QS_CASE_MIS20S1_DOUBLE) {
#if defined(MIS20S1_SUPPORT)
        eSnsType = IMAGEDESIGN_MIS20S1;

        /* comm pool config */
        if (pEntryParam->nIvpsVencLinkMode == (AX_S32)AX_MOD_DATA_FLOW_SW_LOW_MEM) {
            pCommonArgs->nPoolCfgCnt = sizeof(gtSysCommPoolDoubleMis20s1) / sizeof(gtSysCommPoolDoubleMis20s1[0]);
            pCommonArgs->pPoolCfg = gtSysCommPoolDoubleMis20s1;
        } else {
            pCommonArgs->nPoolCfgCnt = sizeof(gtSysCommPoolDoubleMis20s1_offline) / sizeof(gtSysCommPoolDoubleMis20s1_offline[0]);
            pCommonArgs->pPoolCfg = gtSysCommPoolDoubleMis20s1_offline;
        }

        /* private pool config */
        pPrivArgs->nPoolCfgCnt = sizeof(gtPrivatePoolDoubleMis20s1) / sizeof(gtPrivatePoolDoubleMis20s1[0]);
        pPrivArgs->pPoolCfg = gtPrivatePoolDoubleMis20s1;

        /* cams config */
        __sample_case_double_mis20s1(pCamList, eSnsType, pEntryParam, pCommonArgs);
#endif
    } else if (pEntryParam->nCaseId == QS_CASE_DUMMY_SINGLE) {
#if defined(SENSOR_DUMMY_SUPPORT)
        eSnsType = SENSOR_DUMMY;

        /* comm pool config */
        if (pEntryParam->nIvpsVencLinkMode == (AX_S32)AX_MOD_DATA_FLOW_SW_LOW_MEM) {
            pCommonArgs->nPoolCfgCnt = sizeof(gtSysCommPoolSingleDummy) / sizeof(gtSysCommPoolSingleDummy[0]);
            pCommonArgs->pPoolCfg = gtSysCommPoolSingleDummy;
        } else {
            pCommonArgs->nPoolCfgCnt = sizeof(gtSysCommPoolSingleDummy_offline) / sizeof(gtSysCommPoolSingleDummy_offline[0]);
            pCommonArgs->pPoolCfg = gtSysCommPoolSingleDummy_offline;
        }

        /* private pool config */
        pPrivArgs->nPoolCfgCnt = sizeof(gtPrivatePoolSingleDummy) / sizeof(gtPrivatePoolSingleDummy[0]);
        pPrivArgs->pPoolCfg = gtPrivatePoolSingleDummy;

        /* cams config */
        __sample_case_single_dummy(pCamList, eSnsType, pEntryParam, pCommonArgs);
#endif
    } else if (pEntryParam->nCaseId == QS_CASE_SC465SL_SINGLE) {
#if defined(SC465SL_SUPPORT)
        eSnsType = SMARTSENS_SC465SL;
        /* comm pool config */
        if (pEntryParam->nIvpsVencLinkMode == (AX_S32)AX_MOD_DATA_FLOW_SW_LOW_MEM) {
            pCommonArgs->nPoolCfgCnt = sizeof(gtSysCommPoolSingleSc465sl) / sizeof(gtSysCommPoolSingleSc465sl[0]);
            pCommonArgs->pPoolCfg = gtSysCommPoolSingleSc465sl;
        } else {
            pCommonArgs->nPoolCfgCnt = sizeof(gtSysCommPoolSingleSc465sl_offline) / sizeof(gtSysCommPoolSingleSc465sl_offline[0]);
            pCommonArgs->pPoolCfg = gtSysCommPoolSingleSc465sl_offline;
        }

        /* private pool config */
        pPrivArgs->nPoolCfgCnt = sizeof(gtPrivatePoolSingleSc465sl) / sizeof(gtPrivatePoolSingleSc465sl[0]);
        pPrivArgs->pPoolCfg = gtPrivatePoolSingleSc465sl;

        /* cams config */
        __sample_case_single_sc465sl(pCamList, eSnsType, pEntryParam, pCommonArgs);
#endif
    } else if (pEntryParam->nCaseId == QS_CASE_SC465SL_DOUBLE) {
#if defined(SC465SL_SUPPORT)
        eSnsType = SMARTSENS_SC465SL;
        /* comm pool config */
        if (pEntryParam->nIvpsVencLinkMode == (AX_S32)AX_MOD_DATA_FLOW_SW_LOW_MEM) {
            pCommonArgs->nPoolCfgCnt = sizeof(gtSysCommPoolDoubleSc465sl) / sizeof(gtSysCommPoolDoubleSc465sl[0]);
            pCommonArgs->pPoolCfg = gtSysCommPoolDoubleSc465sl;
        } else {
            pCommonArgs->nPoolCfgCnt = sizeof(gtSysCommPoolDoubleSc465sl_offline) / sizeof(gtSysCommPoolDoubleSc465sl_offline[0]);
            pCommonArgs->pPoolCfg = gtSysCommPoolDoubleSc465sl_offline;
        }

        /* private pool config */
        pPrivArgs->nPoolCfgCnt = sizeof(gtPrivatePoolDoubleSc465sl) / sizeof(gtPrivatePoolDoubleSc465sl[0]);
        pPrivArgs->pPoolCfg = gtPrivatePoolDoubleSc465sl;

        /* cams config */
        __sample_case_double_sc465sl(pCamList, eSnsType, pEntryParam, pCommonArgs);
#endif
    }else {
        ALOGE("unknown case id: %d", pEntryParam->nCaseId);
        return -1;
    }

    if (pEntryParam->nCamCnt == 1) {
        if(pEntryParam->nInterpolationWidth > 0 && pEntryParam->nInterpolationHeight > 0) {
            if (pPrivArgs->pPoolCfg) {
                if (pEntryParam->nIvpsVencLinkMode == (AX_S32)AX_MOD_DATA_FLOW_SW_LOW_MEM) {
                    pCommonArgs->pPoolCfg[1].nWidth = pEntryParam->nInterpolationWidth;
                    // pCommonArgs->pPoolCfg[1].nHeight = pEntryParam->nInterpolationHeight;
                    pCommonArgs->pPoolCfg[1].nWidthStride = pEntryParam->nInterpolationWidth;
                } else {
                    pCommonArgs->pPoolCfg[0].nWidth = pEntryParam->nInterpolationWidth;
                    pCommonArgs->pPoolCfg[0].nHeight = pEntryParam->nInterpolationHeight;
                    pCommonArgs->pPoolCfg[0].nWidthStride = pEntryParam->nInterpolationWidth;
                }
            }

            gSingleSnsOutChnAttr[0].nWidth =  pEntryParam->nInterpolationWidth;
            gSingleSnsOutChnAttr[0].nHeight =  pEntryParam->nInterpolationHeight;
        } else {
            gSingleSnsOutChnAttr[0].nWidth =  pCamList[0].tChnAttr[0].nWidth;
            gSingleSnsOutChnAttr[0].nHeight =  pCamList[0].tChnAttr[0].nHeight;
        }
        gSingleSnsOutChnAttr[0].fDetectWidthRatio =  gSingleSnsOutChnAttr[0].nWidth / 640.0f;
        gSingleSnsOutChnAttr[0].fDetectHeightRatio =  gSingleSnsOutChnAttr[0].nHeight / 352.0f;
        pEntryParam->pOutChnAttr = gSingleSnsOutChnAttr;

        // vin and ivps mode, modify yuv buffer number
        if (pEntryParam->nVinIvpsLinkMode == (AX_S32)AX_MOD_DATA_FLOW_SW_LOW_MEM) {
            pCommonArgs->pPoolCfg[0].nBlkCnt = 1;
        }

        // set vin mode (ife and itp mode)
        if (pEntryParam->nCaseId == QS_CASE_OS04D10_SINGLE) {
            pCamList[0].nLowMemMode = 0;
        } else if ( 5 == AX_SYS_GetChipType() ) {//TODO: AX615DV100 = 0x5
            pCamList[0].nLowMemMode = 4;//ptn:256
        } else {
#ifdef TOTAL_MEM_SIZE_64M
            pCamList[0].nLowMemMode = 4;
#else
            pCamList[0].nLowMemMode = 0;
#endif
        }
        if (pPrivArgs->pPoolCfg && (pCamList[0].nLowMemMode & (0x01<<2))) {//ptn:256
            pPrivArgs->pPoolCfg[0].ubufMode |= (0x01<<2);
        }
        pCamList[0].eLatencyMode = AX_VIN_LATENCY_MODE_MAX;
        pCamList[0].eProfileMode = AX_VIN_PROFILE_MODE_NORMAL;
    } else {
        if(pEntryParam->nInterpolationWidth > 0 && pEntryParam->nInterpolationHeight > 0) {
            if (pPrivArgs->pPoolCfg) {
                if (pEntryParam->nIvpsVencLinkMode == (AX_S32)AX_MOD_DATA_FLOW_SW_LOW_MEM) {
                    pCommonArgs->pPoolCfg[1].nWidth = pEntryParam->nInterpolationWidth;
                    // pCommonArgs->pPoolCfg[1].nHeight = pEntryParam->nInterpolationHeight;
                    pCommonArgs->pPoolCfg[1].nWidthStride = pEntryParam->nInterpolationWidth;
                } else {
                    pCommonArgs->pPoolCfg[0].nWidth = pEntryParam->nInterpolationWidth;
                    pCommonArgs->pPoolCfg[0].nHeight = pEntryParam->nInterpolationHeight;
                    pCommonArgs->pPoolCfg[0].nWidthStride = pEntryParam->nInterpolationWidth;
                }
            }

            gDualSnsOutChnAttr[0].nWidth =  pEntryParam->nInterpolationWidth;
            gDualSnsOutChnAttr[0].nHeight =  pEntryParam->nInterpolationHeight;
            gDualSnsOutChnAttr[3].nWidth =  pEntryParam->nInterpolationWidth;
            gDualSnsOutChnAttr[3].nHeight =  pEntryParam->nInterpolationHeight;
        } else {
            gDualSnsOutChnAttr[0].nWidth =  pCamList[0].tChnAttr[0].nWidth;
            gDualSnsOutChnAttr[0].nHeight =  pCamList[0].tChnAttr[0].nHeight;
            gDualSnsOutChnAttr[3].nWidth =  pCamList[1].tChnAttr[0].nWidth;
            gDualSnsOutChnAttr[3].nHeight =  pCamList[1].tChnAttr[0].nHeight;
        }
        gDualSnsOutChnAttr[0].fDetectWidthRatio =  gDualSnsOutChnAttr[0].nWidth / 640.0f;
        gDualSnsOutChnAttr[0].fDetectHeightRatio =  gDualSnsOutChnAttr[0].nHeight / 352.0f;
        gDualSnsOutChnAttr[3].fDetectWidthRatio =  gDualSnsOutChnAttr[3].nWidth / 640.0f;
        gDualSnsOutChnAttr[3].fDetectHeightRatio =  gDualSnsOutChnAttr[3].nHeight / 352.0f;
        pEntryParam->pOutChnAttr = gDualSnsOutChnAttr;

        // vin and ivps mode, modify yuv buffer number
        if (pEntryParam->nVinIvpsLinkMode == (AX_S32)AX_MOD_DATA_FLOW_SW_LOW_MEM) {
            pCommonArgs->pPoolCfg[0].nBlkCnt = 2;
        }

        // set vin mode (ife and itp mode)
        pCamList[0].nLowMemMode = 2;
        pCamList[0].eLatencyMode = AX_VIN_LATENCY_MODE_FRAME;
        pCamList[0].eProfileMode = AX_VIN_PROFILE_MODE_HIGH;
        pCamList[1].nLowMemMode = 2;
        pCamList[1].eLatencyMode = AX_VIN_LATENCY_MODE_FRAME;
        pCamList[1].eProfileMode = AX_VIN_PROFILE_MODE_HIGH;

        if (pPrivArgs->pPoolCfg) {
            if (pCamList[0].nLowMemMode & (0x01<<2)) {//ptn:256
                pPrivArgs->pPoolCfg[0].ubufMode |= (0x01<<2);
            }

            if (pCamList[1].nLowMemMode & (0x01<<2)) {//ptn:256
                pPrivArgs->pPoolCfg[0].ubufMode |= (0x01<<2);
            }
        }
    }
    return 0;
}

AX_S32 qs_cam_change_hdrmode(SAMPLE_ENTRY_PARAM_T *pEntryParam, QS_AX_CAMERA_T* pCamList)
{
    QS_SAMPLE_SNS_TYPE_E   eSnsType = QS_SAMPLE_SNS_TYPE_NONE;
    QS_COMMON_SYS_ARGS_T   commonArgs = {0};
    AX_SENSOR_REGISTER_FUNC_T *ptSnsHdl[2] = {AX_NULL};

    if (pEntryParam->nCaseId == QS_CASE_SC200AI_SINGLE) {
#if defined(SC200AI_SUPPORT)
        eSnsType = (QS_SAMPLE_SNS_TYPE_E)(SMARTSENS_SC200AI);
        QSDEMO_VIN_GetSnsObj(eSnsType, ptSnsHdl);
        /* cams config */
        __sample_case_single_sc200ai(pCamList, eSnsType, pEntryParam, &commonArgs);
        pCamList[0].ptSnsHdl = ptSnsHdl[0];
#endif
    } else if (pEntryParam->nCaseId == QS_CASE_SC200AI_DOUBLE) {
#if defined(SC200AI_SUPPORT)
        eSnsType = (QS_SAMPLE_SNS_TYPE_E)(SMARTSENS_SC200AI);
        QSDEMO_VIN_GetSnsObj(eSnsType, ptSnsHdl);
        /* cams config */
        __sample_case_double_sc200ai(pCamList, eSnsType, pEntryParam, &commonArgs);
        pCamList[0].ptSnsHdl = ptSnsHdl[0];
        pCamList[1].ptSnsHdl = ptSnsHdl[0];
#endif
    } else if (pEntryParam->nCaseId == QS_CASE_SC200AI_TRIPLE) {
#if defined(SC200AI_SUPPORT)
        eSnsType = (QS_SAMPLE_SNS_TYPE_E)(SMARTSENS_SC200AI);
        QSDEMO_VIN_GetSnsObj(eSnsType, ptSnsHdl);
        /* cams config */
        __sample_case_triple_sc200ai(pCamList, eSnsType, pEntryParam, &commonArgs);
            pCamList[0].ptSnsHdl = ptSnsHdl[0];
            pCamList[1].ptSnsHdl = ptSnsHdl[0];
            pCamList[1].ptSnsHdl1 = ptSnsHdl[0];
#endif
    } else if (pEntryParam->nCaseId == QS_CASE_SC235HAI_SINGLE) {
#if defined(SC235HAI_SUPPORT)
        eSnsType = (QS_SAMPLE_SNS_TYPE_E)(SMARTSENS_SC235HAI);
        QSDEMO_VIN_GetSnsObj(eSnsType, ptSnsHdl);
        /* cams config */
        __sample_case_single_sc235hai(pCamList, eSnsType, pEntryParam, &commonArgs);
        pCamList[0].ptSnsHdl = ptSnsHdl[0];
#endif
    }  else if (pEntryParam->nCaseId == QS_CASE_SC235HAI_DOUBLE) {
#if defined(SC235HAI_SUPPORT)
        eSnsType = (QS_SAMPLE_SNS_TYPE_E)(SMARTSENS_SC235HAI);
        QSDEMO_VIN_GetSnsObj(eSnsType, ptSnsHdl);
        /* cams config */
        __sample_case_double_sc235hai(pCamList, eSnsType, pEntryParam, &commonArgs);
        pCamList[0].ptSnsHdl = ptSnsHdl[0];
        pCamList[1].ptSnsHdl = ptSnsHdl[0];
#endif
    } else if (pEntryParam->nCaseId == QS_CASE_SC500AI_SINGLE) {
#if defined(SC500AI_SUPPORT)
        eSnsType = (QS_SAMPLE_SNS_TYPE_E)(SMARTSENS_SC500AI);
        QSDEMO_VIN_GetSnsObj(eSnsType, ptSnsHdl);
        /* cams config */
        __sample_case_single_sc500ai(pCamList, eSnsType, pEntryParam, &commonArgs);
        pCamList[0].ptSnsHdl = ptSnsHdl[0];
#endif
    } else if (pEntryParam->nCaseId == QS_CASE_SC450AI_SINGLE) {
#if defined(SC450AI_SUPPORT)
        eSnsType = SMARTSENS_SC450AI;
        QSDEMO_VIN_GetSnsObj(eSnsType, ptSnsHdl);
        /* cams config */
        __sample_case_single_sc450ai(pCamList, eSnsType, pEntryParam, &commonArgs);
        pCamList[0].ptSnsHdl = ptSnsHdl[0];
#endif
    } else if (pEntryParam->nCaseId == QS_CASE_SC438HAI_SINGLE) {
#if defined(SC438HAI_SUPPORT)
        eSnsType = SMARTSENS_SC438HAI;
        QSDEMO_VIN_GetSnsObj(eSnsType, ptSnsHdl);

        /* cams config */
        __sample_case_single_sc438hai(pCamList, eSnsType, pEntryParam, &commonArgs);
        pCamList[0].ptSnsHdl = ptSnsHdl[0];
#endif
    } else if (pEntryParam->nCaseId == QS_CASE_SC438HAI_DOUBLE) {
#if defined(SC438HAI_SUPPORT)
        eSnsType = SMARTSENS_SC438HAI;
        QSDEMO_VIN_GetSnsObj(eSnsType, ptSnsHdl);
        /* cams config */
        __sample_case_double_sc438hai(pCamList, eSnsType, pEntryParam, &commonArgs);
        pCamList[0].ptSnsHdl = ptSnsHdl[0];
        pCamList[1].ptSnsHdl = ptSnsHdl[0];
#endif
    } else if (pEntryParam->nCaseId == QS_CASE_SC533HAI_SINGLE) {
#if defined(SC533HAI_SUPPORT)
        eSnsType = SMARTSENS_SC533HAI;
        QSDEMO_VIN_GetSnsObj(eSnsType, ptSnsHdl);

        /* cams config */
        __sample_case_single_sc533hai(pCamList, eSnsType, pEntryParam, &commonArgs);
        pCamList[0].ptSnsHdl = ptSnsHdl[0];
#endif
    } else if (pEntryParam->nCaseId == QS_CASE_OS04D10_SINGLE) {
#if defined(OS04D10_SUPPORT)
        eSnsType = OMNIVISION_OS04D10;
        QSDEMO_VIN_GetSnsObj(eSnsType, ptSnsHdl);
        /* cams config */
        __sample_case_single_os04d10(pCamList, eSnsType, pEntryParam, &commonArgs);
        pCamList[0].ptSnsHdl = ptSnsHdl[0];
#endif
    } else if (pEntryParam->nCaseId == QS_CASE_OS04D10_DOUBLE) {
#if defined(OS04D10_SUPPORT)
        eSnsType = OMNIVISION_OS04D10;
        QSDEMO_VIN_GetSnsObj(eSnsType, ptSnsHdl);
        /* cams config */
        __sample_case_double_os04d10(pCamList, eSnsType, pEntryParam, &commonArgs);
        pCamList[0].ptSnsHdl = ptSnsHdl[0];
        pCamList[1].ptSnsHdl = ptSnsHdl[0];
#endif
    } else if (pEntryParam->nCaseId == QS_CASE_OS04A10_SINGLE) {
#if defined(OS04A10_SUPPORT)
        eSnsType = OMNIVISION_OS04A10;
        QSDEMO_VIN_GetSnsObj(eSnsType, ptSnsHdl);
        /* cams config */
        __sample_case_single_os04a10(pCamList, eSnsType, pEntryParam, &commonArgs);
        pCamList[0].ptSnsHdl = ptSnsHdl[0];
#endif
    } else if (pEntryParam->nCaseId == QS_CASE_OS04A10_DOUBLE) {
#if defined(OS04A10_SUPPORT)
        eSnsType = OMNIVISION_OS04A10;
        QSDEMO_VIN_GetSnsObj(eSnsType, ptSnsHdl);
        /* cams config */
        __sample_case_double_os04a10(pCamList, eSnsType, pEntryParam, &commonArgs);
        pCamList[0].ptSnsHdl = ptSnsHdl[0];
#endif
    } else if (pEntryParam->nCaseId == QS_CASE_MIS20S1_SINGLE) {
#if defined(MIS20S1_SUPPORT)
        eSnsType = IMAGEDESIGN_MIS20S1;
        QSDEMO_VIN_GetSnsObj(eSnsType, ptSnsHdl);
        /* cams config */
        __sample_case_single_mis20s1(pCamList, eSnsType, pEntryParam, &commonArgs);
        pCamList[0].ptSnsHdl = ptSnsHdl[0];
#endif
    } else if (pEntryParam->nCaseId == QS_CASE_MIS20S1_DOUBLE) {
#if defined(MIS20S1_SUPPORT)
        eSnsType = IMAGEDESIGN_MIS20S1;
        QSDEMO_VIN_GetSnsObj(eSnsType, ptSnsHdl);
        /* cams config */
        __sample_case_double_mis20s1(pCamList, eSnsType, pEntryParam, &commonArgs);
        pCamList[0].ptSnsHdl = ptSnsHdl[0];
        pCamList[1].ptSnsHdl = ptSnsHdl[0];
#endif
    } else if (pEntryParam->nCaseId == QS_CASE_DUMMY_SINGLE) {
#if defined(SENSOR_DUMMY_SUPPORT)
        eSnsType = SENSOR_DUMMY;
        QSDEMO_VIN_GetSnsObj(eSnsType, ptSnsHdl);
        /* cams config */
        __sample_case_single_dummy(pCamList, eSnsType, pEntryParam, &commonArgs);
        pCamList[0].ptSnsHdl = ptSnsHdl[0];
#endif
    } else if (pEntryParam->nCaseId == QS_CASE_SC465SL_SINGLE) {
#if defined(SC465SL_SUPPORT)
        eSnsType = SMARTSENS_SC465SL;
        QSDEMO_VIN_GetSnsObj(eSnsType, ptSnsHdl);
        /* cams config */
        __sample_case_single_sc465sl(pCamList, eSnsType, pEntryParam, &commonArgs);
        pCamList[0].ptSnsHdl = ptSnsHdl[0];
#endif
    } else if (pEntryParam->nCaseId == QS_CASE_SC465SL_DOUBLE) {
#if defined(SC465SL_SUPPORT)
        eSnsType = SMARTSENS_SC465SL;
        QSDEMO_VIN_GetSnsObj(eSnsType, ptSnsHdl);
        /* cams config */
        __sample_case_double_sc465sl(pCamList, eSnsType, pEntryParam, &commonArgs);
        pCamList[0].ptSnsHdl = ptSnsHdl[0];
        pCamList[1].ptSnsHdl = ptSnsHdl[0];
#endif
    }
    else {
        ALOGE("unknown case id: %d", pEntryParam->nCaseId);
        return -1;
    }
    return 0;
}
