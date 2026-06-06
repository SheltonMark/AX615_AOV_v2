/**************************************************************************************************
 *
 * Copyright (c) 2019-2025 Axera Semiconductor Co., Ltd. All Rights Reserved.
 *
 * This source file is the property of Axera Semiconductor Co., Ltd. and
 * may not be copied or distributed in any isomorphic form without the prior
 * written consent of Axera Semiconductor Co., Ltd.
 *
 **************************************************************************************************/

#ifndef __AX_VENC_RC_H__
#define __AX_VENC_RC_H__

#include "ax_global_type.h"

#ifdef __cplusplus
#if __cplusplus
extern "C" {
#endif
#endif /* __cplusplus */

/* For RC, in kbps */
#define AX_VENC_MIN_BITRATE (3)
#define AX_VENC_MAX_BITRATE (160 * 1000)

#define AX_VENC_BITRATE_RATIO    (1000)
#define AX_VENC_MIN_INTRADELTAQP (-51)
#define AX_VENC_MAX_INTRADELTAQP (51)

/* IDR interval */
#define AX_VENC_MIN_IDR_INTERVAL (1)
#define AX_VENC_MAX_IDR_INTERVAL (65536)

/* qp range */
#define AX_VENC_MIN_FIXED_QP (-1)
#define AX_VENC_MIN_QP       (0)
#define AX_VENC_MAX_QP       (51)
#define AX_VENC_IDR_DELTA_QP_MAX     (10)
#define AX_VENC_IDR_DELTA_QP_MIN     (2)

/* frame rate range */
#define AX_VENC_MIN_FRAME_RATE (1)
#define AX_VENC_MAX_FRAME_RATE (240)

#define AX_MAX_EXTRA_BITRATE (1000 * 1024)

/* I/P frame size proportion */
#define AX_VENC_MIN_I_PROP (1)
#define AX_VENC_MAX_I_PROP (100)

/* cvbr params */
#define AX_VENC_MIN_SHORT_TERM_STAT_TIME (1)   /* in seconds */
#define AX_VENC_MAX_SHORT_TERM_STAT_TIME (120) /* in seconds */

#define AX_VENC_MIN_LONG_TERM_STAT_TIME (1)    /* in seconds */
#define AX_VENC_MAX_LONG_TERM_STAT_TIME (1440) /* in seconds */

#define AX_VENC_MAX_LONG_TERM_BITRATE_LOW  (2)      /* in kbps */
#define AX_VENC_MAX_LONG_TERM_BITRATE_HIGH (614400) /* in kbps */

#define AX_VENC_MIN_LONG_TERM_BITRATE_LOW  (0)      /* in kbps */
#define AX_VENC_MIN_LONG_TERM_BITRATE_HIGH (614400) /* in kbps */
/* Difference between FrameLevelMinQp and MinQp */
#define AX_VENC_MIN_QP_DELTA_LOW  (0)
#define AX_VENC_MIN_QP_DELTA_HIGH (4)
/* Difference between FrameLevelMaxQp and MaxQp */
#define AX_VENC_MAX_QP_DELTA_LOW  (0)
#define AX_VENC_MAX_QP_DELTA_HIGH (4)

#define VENC_MIN_STATIC_SCENE_THRESHOLD (1)
#define VENC_MAX_STATIC_SCENE_THRESHOLD (10)
#define VENC_DEF_STATIC_SCENE_THRESHOLD (5)

#define VENC_MIN_CHANGE_POS (20)
#define VENC_MAX_CHANGE_POS (100)
#define VENC_DEF_CHANGE_POS (90)

#define VENC_MIN_STILL_PERCENT (10)
#define VENC_MAX_STILL_PERCENT (100)
#define VENC_DEF_STILL_PERCENT (25)

#define VENC_MAX_STILL_QP_DEF (36)

/* rc mode */
typedef enum
{
    AX_VENC_RC_MODE_H264CBR = 1,
    AX_VENC_RC_MODE_H264VBR,
    AX_VENC_RC_MODE_H264AVBR,
    AX_VENC_RC_MODE_H264QVBR,
    AX_VENC_RC_MODE_H264CVBR,
    AX_VENC_RC_MODE_H264FIXQP,
    AX_VENC_RC_MODE_H264QPMAP,

    AX_VENC_RC_MODE_MJPEGCBR,
    AX_VENC_RC_MODE_MJPEGVBR,
    AX_VENC_RC_MODE_MJPEGFIXQP,

    AX_VENC_RC_MODE_H265CBR,
    AX_VENC_RC_MODE_H265VBR,
    AX_VENC_RC_MODE_H265AVBR,
    AX_VENC_RC_MODE_H265QVBR,
    AX_VENC_RC_MODE_H265CVBR,
    AX_VENC_RC_MODE_H265FIXQP,
    AX_VENC_RC_MODE_H265QPMAP,

    AX_VENC_RC_MODE_BUTT,

} AX_VENC_RC_MODE_E;

/* qpmap mode*/
typedef enum
{
    AX_VENC_RC_QPMAP_MODE_MEANQP = 0,
    AX_VENC_RC_QPMAP_MODE_MINQP,
    AX_VENC_RC_QPMAP_MODE_MAXQP,

    AX_VENC_RC_QPMAP_MODE_BUTT,
} AX_VENC_RC_QPMAP_MODE_E;

typedef enum
{
    AX_VENC_RC_CTBRC_DISABLE = 0,      /* Disable CTB QP adjustment. */
    AX_VENC_RC_CTBRC_QUALITY = 1,      /* CTB QP adjustment for Subjective Quality only. */
    AX_VENC_RC_CTBRC_RATE = 2,         /* CTB QP adjustment for Rate Control only. */
    AX_VENC_RC_CTBRC_QUALITY_RATE = 3, /* CTB QP adjustment for both Subjective Quality and Rate Control. */
    AX_VENC_RC_CTBRC_BUTT,
} AX_VENC_RC_CTBRC_MODE_E;

typedef enum
{
    AX_VENC_QPMAP_QP_DISABLE = 0,
    AX_VENC_QPMAP_QP_DELTA = 1, /* qp rang in [-31, 32] */
    AX_VENC_QPMAP_QP_ABS = 2,   /* qp range in [0, 51] */
    AX_VENC_QPMAP_QP_BUTT,
} AX_VENC_QPMAP_QP_TYPE_E;

typedef enum
{
    AX_VENC_QPMAP_BLOCK_DISABLE = 0,
    AX_VENC_QPMAP_BLOCK_SKIP = 1,
    AX_VENC_QPMAP_BLOCK_IPCM = 2,
    AX_VENC_QPMAP_BLOCK_BUTT,
} AX_VENC_QPMAP_BLOCK_TYPE_E;

typedef enum
{
    /**
     * Block Unit Size Of QpMap:
     * h265: support 64x64、32x32、16x16
     * h264: support 64x64、32x32、16x16
     *******************************
     * Block Unit Size Of SkipMap:
     * h265: support 64x64、32x32
     * h264: support 64x64、32x32、16x16
     *******************************
     * Block Unit Size Of IpcmMap:
     * h265: support 64x64
     * h264: support 16x16
     */
    AX_VENC_QPMAP_BLOCK_UNIT_NONE = -1,
    AX_VENC_QPMAP_BLOCK_UNIT_64x64 = 0,
    AX_VENC_QPMAP_BLOCK_UNIT_32x32 = 1,
    AX_VENC_QPMAP_BLOCK_UNIT_16x16 = 2,
    AX_VENC_QPMAP_BLOCK_UNIT_BUTT,
} AX_VENC_QPMAP_BLOCK_UNIT_E;

/* quality level for vbr */
typedef enum axVENC_VBR_QUALITY_LEVEL_E
{
    AX_VENC_VBR_QUALITY_LEVEL_INV = 0,
    AX_VENC_VBR_QUALITY_LEVEL_0,
    AX_VENC_VBR_QUALITY_LEVEL_1,
    AX_VENC_VBR_QUALITY_LEVEL_2,
    AX_VENC_VBR_QUALITY_LEVEL_3,
    AX_VENC_VBR_QUALITY_LEVEL_4,
    AX_VENC_VBR_QUALITY_LEVEL_5,
    AX_VENC_VBR_QUALITY_LEVEL_6,
    AX_VENC_VBR_QUALITY_LEVEL_7,
    AX_VENC_VBR_QUALITY_LEVEL_8,
    AX_VENC_VBR_QUALITY_LEVEL_AUTO,
    AX_VENC_VBR_QUALITY_LEVEL_BUTT,
} AX_VENC_VBR_QUALITY_LEVEL_E;

typedef struct axVENC_CUSTOM_MAP_INFO
{
    AX_BOOL  bUseMap;
    AX_U32   u32MbWidth;
    AX_U32   u32MapSize;
    AX_U64   u64QpMapPhyAddr;
    AX_U8    *pu8QpMapVirAddr;
    AX_BOOL  bUseMbuf;
    AX_S32   s32BlkId;
} AX_VENC_CUSTOM_MAP_INFO_T;

typedef struct axVENC_USE_LOCAL_MAP_INFO
{
    AX_BOOL  bUseMap;
    AX_U32   u32MbWidth;
    AX_U32   u32MapSize;
    AX_U64   u64QpMapPhyAddr;
    AX_U8    *pu8QpMapVirAddr;
    AX_BOOL  bUseMbuf;
    AX_S32   s32BlkId;
} AX_VENC_USELOCAL_MAP_INFO_T;

typedef struct axVENC_QPMAP_META_T
{
    AX_VENC_RC_CTBRC_MODE_E enCtbRcMode;         /* Ctb rc mode */
    AX_VENC_QPMAP_QP_TYPE_E enQpmapQpType;       /* The qp type of qpmap */
    AX_VENC_QPMAP_BLOCK_TYPE_E enQpmapBlockType; /* encode corresponding macro-block in skip/ipcm mode */
    AX_VENC_QPMAP_BLOCK_UNIT_E enQpmapBlockUnit; /* qpmap macro-block unit size */
} AX_VENC_QPMAP_META_T;

typedef struct axVENC_QPMAP_META_V1_T
{
    AX_BOOL bActOnIFrm;
    AX_BOOL bQpMapFinely;
    AX_VENC_CUSTOM_MAP_INFO_T stQpMap1;
    AX_VENC_CUSTOM_MAP_INFO_T stQpMap2;
    AX_VENC_USELOCAL_MAP_INFO_T stUseLocalMap;
} AX_VENC_QPMAP_META_V1_T;

/* the attribute of h264e cbr*/
typedef struct axVENC_H264_CBR_T
{
    AX_U32 u32Gop;                      /* Range:[1, 65536]; the interval of I Frame. */
    AX_U32 u32StatTime;                 /* Range:[1, 60]; the rate statistic time, the unit is senconds(s) */
    AX_U32 u32BitRate;                  /* Range:[3, 160000]; Average bitrate(kbps) */

    AX_U32 u32MaxQp;                    /* Range:[1, 51]; Constraint: min_range(u32MaxQp)=max(u32MinQp,1); the max P qp value */
    AX_U32 u32MinQp;                    /* Range:[0, 51]; the min P qp value */
    AX_U32 u32MaxIQp;                   /* Range:[1, 51]; Constraint: min_range(u32MaxIQp)=max(u32MinIQp,1); the max I qp value */
    AX_U32 u32MinIQp;                   /* Range:[0, 51]; the min I qp value */
    AX_U32 u32MaxIprop;                 /* Range:[1, 100]; the max I P size ratio */
    AX_U32 u32MinIprop;                 /* Range:[1, 100]; Constraint: max_range(u32MinIprop)=u32MaxIprop; the min I P size ratio */
    AX_S32 s32IntraQpDelta;             /* Range:[-10, 10]; QP difference between target QP and intra frame QP */
    AX_S32 s32DeBreathQpDelta;          /* Not supported on AX615; Range:[-51, 51]; de breath effetct QP */
    AX_U32 u32IdrQpDeltaRange;          /* Range:[2, 16]; QP rang between CU QP and I frame QP */
    AX_VENC_QPMAP_META_T stQpmapInfo;   /* Usage on the other AX platforms, Qpmap related info;N/A for AX615 */
    AX_VENC_QPMAP_META_V1_T stQpmapInfoV1; /* Usage on the AX615 platform, Qpmap related info;Suitable for AX615 */

    AX_U32 u32BlockRcSize;              /* Range:[1, 2]; Minimum processing unit for block-level rate control: 0: 64x64; 1: 32x32; 2: 16x16
                                           Under H.265, supports 1: 32x32; 2: 16x16
                                           Under H.264, only supports 2: 16x16 */
    AX_S32 s32MinFrameQp;               /* Range:[0, 51]; Minimum Frame-Level QP for P-Frames */
    AX_S32 s32MaxFrameQp;               /* Range:[1, 51]; Constraint: min_range(s32MaxFrameQp)=max(s32MinFrameQp,1); Maximum Frame-Level QP for P-Frames */                                       
    AX_S32 s32MinIFrameQp;              /* Range:[0, 51]; Minimum Frame-Level QP for I-Frames */
    AX_S32 s32MaxIFrameQp;              /* Range:[1, 51]; Constraint: min_range(s32MaxIFrameQp)=max(s32MinIFrameQp,1); Maximum Frame-Level QP for I-Frames */
    AX_S32 s32CbQpOffset;               /* Range:[-12, 12]; Block-level Cb signal quantization parameter offset. A smaller value results in better quality for the Cb component */
    AX_S32 s32CrQpOffset;               /* Range:[-12, 12]; Block-level Cr signal quantization parameter offset. A smaller value results in better quality for the Cr component */
    AX_U32 u32MotionSen;                /* Range:[0, 16]; Sensitivity to motion in rate control */
    AX_U32 u32MotionAlpha;              /* Range:[0, 128]; Motion convergence speed in rate control */
    AX_U32 u32StillIBitPercent;         /* Range:[0,100]; Proportion of Static I-Frames, 0 means no intervention */
    AX_BOOL bCtuBitsRefEn;              /* Range:[0, 1]; Enabling the CtuBitsRefine feature of RC */
    AX_S32 s32CtuBitsRefQpDeltaRange;   /* Range:[0, 10]; Adjustment range of the CtuBitsRefine feature in RC, relative to frame-level QP */

} AX_VENC_H264_CBR_T;

/* the attribute of h264e vbr*/
typedef struct axVENC_H264_VBR_T
{
    AX_U32 u32Gop;                      /* Range:[1, 65536]; the interval of ISLICE. */
    AX_U32 u32StatTime;                 /* Range:[1, 60]; the rate statistic time, the unit is senconds(s) */
    AX_U32 u32MaxBitRate;               /* Range:[3, 160000]; The max bitrate(kbps) */
    AX_VENC_VBR_QUALITY_LEVEL_E enVQ;   /* Ref to AX_VENC_VBR_QUALITY_LEVEL_E */

    /* min/max qp is invalid when enVQ != AX_VENC_VBR_QUALITY_LEVEL_INV */
    AX_U32 u32MaxQp;                    /* Range:[1, 51]; Constraint: min_range(u32MaxQp)=max(u32MinQp,1); the max P qp value */
    AX_U32 u32MinQp;                    /* Range:[0, 51]; the min P qp value */
    AX_U32 u32MaxIQp;                   /* Range:[1, 51]; Constraint: min_range(u32MaxIQp)=max(u32MinIQp,1); the max I qp value */
    AX_U32 u32MinIQp;                   /* Range:[0, 51]; the min I qp value */
    AX_S32 s32IntraQpDelta;             /* Range:[-10, 10]; QP difference between target QP and intra frame QP */
    AX_S32 s32DeBreathQpDelta;          /* Not supported on AX615; Range:[-51, 51]; de breath effetct QP */
    AX_U32 u32IdrQpDeltaRange;          /* Range:[2, 16]; QP rang between CU QP and I frame QP */
    AX_VENC_QPMAP_META_T stQpmapInfo;   /* Usage on the other AX platforms, Qpmap related info;N/A for AX615 */
    AX_VENC_QPMAP_META_V1_T stQpmapInfoV1; /* Usage on the AX615 platform, Qpmap related info;Suitable for AX615  */

    AX_U32 u32SceneChgThr;              /* Not supported on AX615; static scene threshold [1, 10], def 5 */
    AX_U32 u32ChangePos;                /* Range:[1, 100]; The stress level for bitrate adjustment, representing the ratio of the bitrate at the start of Qp adjustment to the maximum bitrate */

    AX_U32 u32BlockRcSize;              /* Range:[1, 2]; Minimum processing unit for block-level rate control: 0: 64x64; 1: 32x32; 2: 16x16
                                           Under H.265, supports 1: 32x32; 2: 16x16
                                           Under H.264, only supports 2: 16x16 */
    AX_S32 s32MinFrameQp;               /* Range:[0, 51]; Minimum Frame-Level QP for P-Frames */
    AX_S32 s32MaxFrameQp;               /* Range:[1, 51]; Constraint: min_range(s32MaxFrameQp)=max(s32MinFrameQp,1); Maximum Frame-Level QP for P-Frames */                                       
    AX_S32 s32MinIFrameQp;              /* Range:[0, 51]; Minimum Frame-Level QP for I-Frames */
    AX_S32 s32MaxIFrameQp;              /* Range:[1, 51]; Constraint: min_range(s32MaxIFrameQp)=max(s32MinIFrameQp,1); Maximum Frame-Level QP for I-Frames */
    AX_S32 s32CbQpOffset;               /* Range:[-12, 12]; Block-level Cb signal quantization parameter offset. A smaller value results in better quality for the Cb component */
    AX_S32 s32CrQpOffset;               /* Range:[-12, 12]; Block-level Cr signal quantization parameter offset. A smaller value results in better quality for the Cr component */
    AX_U32 u32MotionSen;                /* Range:[0, 16]; Sensitivity to motion in rate control */
    AX_U32 u32MotionAlpha;              /* Range:[0, 128]; Motion convergence speed in rate control */
    AX_U32 u32StillIBitPercent;         /* Range:[0, 100]; Proportion of Static I-Frames, 0 means no intervention */
    AX_BOOL bCtuBitsRefEn;              /* Range:[0, 1]; Enabling the CtuBitsRefine feature of RC */
    AX_S32 s32CtuBitsRefQpDeltaRange;   /* Range:[0, 10]; Adjustment range of the CtuBitsRefine feature in RC, relative to frame-level QP */

} AX_VENC_H264_VBR_T;

/* the attribute of h264e cvbr*/
typedef struct axVENC_H264_CVBR_T
{
    AX_U32 u32Gop;                      /* Range:[1, 65536]; the interval of ISLICE. */
    AX_U32 u32StatTime;                 /* Range:[1, 60]; the rate statistic time, the unit is senconds(s) */

    AX_U32 u32MaxQp;                    /* Range:[1, 51]; Constraint: min_range(u32MaxQp)=max(u32MinQp,1); the max P qp value */
    AX_U32 u32MinQp;                    /* Range:[0, 51]; the min P qp value, can not be larger than u32MaxQp */
    AX_U32 u32MaxIQp;                   /* Range:[1, 51]; Constraint: min_range(u32MaxIQp)=max(u32MinIQp,1); the max I qp value */
    AX_U32 u32MinIQp;                   /* Range:[0, 51]; the min I qp value, can not be larger than u32MaxIQp */

    AX_U32 u32MinQpDelta;               /* Not supported on AX615; Range:[0, 4]; Difference between FrameLevelMinQp and MinQp, FrameLevelMinQp = MinQp(or
                                           MinIQp) + MinQpDelta */
    AX_U32 u32MaxQpDelta;               /* Not supported on AX615; Range:[0, 4]; Difference between FrameLevelMaxQp and MaxQp, FrameLevelMaxQp = MaxQp(or
                                           MaxIQp) - MaxQpDelta */
    AX_S32 s32DeBreathQpDelta;          /* Not supported on AX615; Range:[-51, 51]; de breath effetct QP */

    AX_U32 u32IdrQpDeltaRange;          /* Range:[2, 16]; QP rang between CU QP and I frame QP */
    AX_U32 u32MaxIprop;                 /* Range:[1, 100]; the max I P size ratio */
    AX_U32 u32MinIprop;                 /* Range:[1, 100]; Constraint: max_range(u32MinIprop)=u32MaxIprop; the min I P size ratio */

    AX_U32 u32MaxBitRate;               /* Range:[3, 160000]; The max bitrate, the unit is kbps */
    AX_U32 u32ShortTermStatTime;        /* Range:[1, 120]; the short-term rate statistic time, the unit is second (s)*/

    AX_U32 u32LongTermStatTime;         /* Range:[1, 1440]; the long-term rate statistic time, the unit is second (s)*/
    AX_U32 u32LongTermMaxBitrate;       /* Range:[0, 160000]; Constraint: min_range(u32LongTermMaxBitrate)=max(0,u32LongTermMinBitrate); 
                                           max_range(u32LongTermMaxBitrate)=min(u32MaxBitRate,160000);
                                           the long-term target max bitrate, can not be larger than u32MaxBitRate, the unit is kbps */
    AX_U32 u32LongTermMinBitrate;       /* Range:[0, 160000]; the long-term target min bitrate,  can not be larger than
                                           u32LongTermMaxBitrate, the unit is kbps */
    AX_U32 u32ExtraBitPercent;          /* Range:[0, 100]; equal to u32UpperLimitPercent 
                                           The percentage by which the maximum bit rate can be exceeded in high-motion scenes under CVBR (overflow bit rate) */
    AX_U32 u32OverDraftBitPercent;      /* Range:[0, 1000]; percent of bits allowed for early overdraft under CVBR */
    AX_U32 u32LongTermStatTimeUnit;     /* Range:[1, 6000]; Time unit for long-term bitrate statistics */
    AX_S32 s32IntraQpDelta;             /* Range:[-10, 10]; To Alleviate Breath_effect, QP difference between target QP and intra frame QP */
    AX_VENC_QPMAP_META_T stQpmapInfo;   /* Usage on the other AX platforms, Qpmap related info;N/A for AX615 */
    AX_VENC_QPMAP_META_V1_T stQpmapInfoV1; /* Usage on the AX615 platform, Qpmap related info;Suitable for AX615 */

    AX_U32 u32BlockRcSize;              /* Range:[1, 2]; Minimum processing unit for block-level rate control: 0: 64x64; 1: 32x32; 2: 16x16
                                           Under H.265, supports 1: 32x32; 2: 16x16
                                           Under H.264, only supports 2: 16x16 */
    AX_S32 s32MinFrameQp;               /* Range:[0, 51]; Minimum Frame-Level QP for P-Frames */
    AX_S32 s32MaxFrameQp;               /* Range:[1, 51]; Constraint: min_range(s32MaxFrameQp)=max(s32MinFrameQp,1); Maximum Frame-Level QP for P-Frames */                                       
    AX_S32 s32MinIFrameQp;              /* Range:[0, 51]; Minimum Frame-Level QP for I-Frames */
    AX_S32 s32MaxIFrameQp;              /* Range:[1, 51]; Constraint: min_range(s32MaxIFrameQp)=max(s32MinIFrameQp,1); Maximum Frame-Level QP for I-Frames */
    AX_S32 s32CbQpOffset;               /* Range:[-12, 12]; Block-level Cb signal quantization parameter offset. A smaller value results in better quality for the Cb component */
    AX_S32 s32CrQpOffset;               /* Range:[-12, 12]; Block-level Cr signal quantization parameter offset. A smaller value results in better quality for the Cr component */
    AX_U32 u32MotionSen;                /* Range:[0, 16]; Sensitivity to motion in rate control */
    AX_U32 u32MotionAlpha;              /* Range:[0, 128]; Motion convergence speed in rate control */
    AX_U32 u32StillIBitPercent;         /* Range:[0, 100]; Proportion of Static I-Frames, 0 means no intervention */
    AX_BOOL bCtuBitsRefEn;              /* Range:[0, 1]; Enabling the CtuBitsRefine feature of RC */
    AX_S32 s32CtuBitsRefQpDeltaRange;   /* Range:[0, 10]; Adjustment range of the CtuBitsRefine feature in RC, relative to frame-level QP */
    AX_S32 s32Alpha;                    /* Range:[0, 64]; Bitrate fluctuation coefficient: the larger the value, the greater the bitrate varies according to the actual content */
    AX_S32 s32Beta;                     /* Range:[0, 64]; Reference coefficient: the larger the value, the higher the reference bitrate */
    AX_S32 s32WeightSum;                /* Range:[1, 128]; Normalization coefficient: the larger the value, the lower the bitrate */

} AX_VENC_H264_CVBR_T;

/* the attribute of h264e avbr*/
typedef struct axVENC_H264_AVBR_T
{
    AX_U32 u32Gop;                      /* Range:[1, 65536]; the interval of ISLICE. */
    AX_U32 u32StatTime;                 /* Range:[1, 60]; the rate statistic time, the unit is senconds(s) */
    AX_U32 u32MaxBitRate;               /* Range:[3, 160000]; The max bitrate, the unit is kbps */

    AX_U32 u32MaxQp;                    /* Range:[1, 51]; Constraint: min_range(u32MaxQp)=max(u32MinQp,1); the max P qp value */
    AX_U32 u32MinQp;                    /* Range:[0, 51]; the min P qp value */
    AX_U32 u32MaxIQp;                   /* Range:[1, 51]; Constraint: min_range(u32MaxIQp)=max(u32MinIQp,1); the max I qp value */
    AX_U32 u32MinIQp;                   /* Range:[0, 51]; the min I qp value */
    AX_S32 s32IntraQpDelta;             /* Range:[-10, 10]; QP difference between target QP and intra frame QP */
    AX_S32 s32DeBreathQpDelta;          /* Not supported on AX615; Range:[-51, 51]; de breath effetct QP */
    AX_U32 u32IdrQpDeltaRange;          /* Range:[2, 16]; QP rang between CU QP and I frame QP */
    AX_VENC_QPMAP_META_T stQpmapInfo;   /* Usage on the other AX platforms, Qpmap related info;N/A for AX615 */
    AX_VENC_QPMAP_META_V1_T stQpmapInfoV1; /* Usage on the AX615 platform, Qpmap related info;Suitable for AX615 */

    AX_U32 u32SceneChgThr;              /* Not supported on AX615; static scene threshold [1, 10], def 5 */
    AX_U32 u32ChangePos;                /* Range:[1, 100]; The stress level for bitrate adjustment, representing the ratio of the bitrate at the start of Qp adjustment to the maximum bitrate */
    AX_U32 u32MinStillPercent;          /* Range:[10, 100]; Minimum Bit Rate Percentage for Static Scenes */
    AX_U32 u32MaxStillQp;               /* Range:[0, 51]; Constraint: min_range(u32MaxStillQp)=s32MinIFrameQp; max_range(u32MaxStillQp)=s32MaxIFrameQp; 
                                           Maximum Frame-Level QP for I-Frames in Static Scenes */

    AX_U32 u32BlockRcSize;              /* Range:[1, 2]; Minimum processing unit for block-level rate control: 0: 64x64; 1: 32x32; 2: 16x16
                                           Under H.265, supports 1: 32x32; 2: 16x16
                                           Under H.264, only supports 2: 16x16 */
    AX_S32 s32MinFrameQp;               /* Range:[0, 51]; Minimum Frame-Level QP for P-Frames */
    AX_S32 s32MaxFrameQp;               /* Range:[1, 51]; Constraint: min_range(s32MaxFrameQp)=max(s32MinFrameQp,1); Maximum Frame-Level QP for P-Frames */                                       
    AX_S32 s32MinIFrameQp;              /* Range:[0, 51]; Minimum Frame-Level QP for I-Frames */
    AX_S32 s32MaxIFrameQp;              /* Range:[1, 51]; Constraint: min_range(s32MaxIFrameQp)=max(s32MinIFrameQp,1); Maximum Frame-Level QP for I-Frames */
    AX_S32 s32CbQpOffset;               /* Range:[-12, 12]; Block-level Cb signal quantization parameter offset. A smaller value results in better quality for the Cb component */
    AX_S32 s32CrQpOffset;               /* Range:[-12, 12]; Block-level Cr signal quantization parameter offset. A smaller value results in better quality for the Cr component */
    AX_U32 u32MotionSen;                /* Range:[0, 16]; Sensitivity to motion in rate control */
    AX_U32 u32MotionAlpha;              /* Range:[0, 128]; Motion convergence speed in rate control */
    AX_U32 u32StillIBitPercent;         /* Range:[0, 100]; Proportion of Static I-Frames, 0 means no intervention */
    AX_BOOL bCtuBitsRefEn;              /* Range:[0, 1]; Enabling the CtuBitsRefine feature of RC */
    AX_S32 s32CtuBitsRefQpDeltaRange;   /* Range:[0, 10]; Adjustment range of the CtuBitsRefine feature in RC, relative to frame-level QP */
    AX_S32 s32Alpha;                    /* Range:[0, 64]; Bitrate fluctuation coefficient: the larger the value, the greater the bitrate varies according to the actual content */
    AX_S32 s32Beta;                     /* Range:[0, 64]; Reference coefficient: the larger the value, the higher the reference bitrate */
    AX_S32 s32WeightSum;                /* Range:[1, 128]; Normalization coefficient: the larger the value, the lower the bitrate */

} AX_VENC_H264_AVBR_T;

/* the attribute of h264e fixqp*/
typedef struct axVENC_H264_FIXQP_T
{
    AX_U32 u32Gop;          /* Range:[1, 65536]; the interval of ISLICE. */
    AX_U32 u32IQp;          /* Range:[0, 51]; qp of the i frame */
    AX_U32 u32PQp;          /* Range:[0, 51]; qp of the p frame */
    AX_U32 u32BQp;          /* Range:[0, 51]; qp of the b frame */
} AX_VENC_H264_FIXQP_T;

/* the attribute of h264e qpmap*/
typedef struct axVENC_H264_QPMAP_T
{
    AX_U32 u32Gop;           /* Range:[1, 65536]; the interval of ISLICE. */
    AX_U32 u32StatTime;      /* Range:[1, 60]; the rate statistic time, the unit is senconds(s) */
    AX_U32 u32TargetBitRate; /* the target bitrate, the unit is kbps */

    AX_VENC_QPMAP_META_T    stQpmapInfo;   /* Usage on the other AX platforms, Qpmap related info */
    AX_VENC_QPMAP_META_V1_T stQpmapInfoV1; /* Usage on the AX615 platform, Qpmap related info */ 
} AX_VENC_H264_QPMAP_T;

typedef struct axVENC_H264_QVBR_T
{
    AX_U32 u32Gop;           /* Range:[1, 65536];the interval of ISLICE. */
    AX_U32 u32StatTime;      /* Range:[1, 60]; the rate statistic time, the unit is senconds(s) */
    AX_U32 u32TargetBitRate; /* the target bitrate, the unit is kbps */
} AX_VENC_H264_QVBR_T;


/* the attribute of h265e qpmap*/
typedef struct axVENC_H265_QPMAP_T
{
    AX_U32 u32Gop;           /* Range:[1, 65536]; the interval of ISLICE. */
    AX_U32 u32StatTime;      /* Range:[1, 60]; the rate statistic time, the unit is senconds(s) */
    AX_U32 u32TargetBitRate; /* the target bitrate, the unit is kbps */

    AX_VENC_QPMAP_META_T    stQpmapInfo;   /* Usage on the other AX platforms, Qpmap related info */
    AX_VENC_QPMAP_META_V1_T stQpmapInfoV1; /* Usage on the AX615 platform, Qpmap related info */
} AX_VENC_H265_QPMAP_T;

typedef struct axVENC_H264_CBR_T AX_VENC_H265_CBR_T;
typedef struct axVENC_H264_VBR_T AX_VENC_H265_VBR_T;
typedef struct axVENC_H264_AVBR_T AX_VENC_H265_AVBR_T;
typedef struct axVENC_H264_FIXQP_T AX_VENC_H265_FIXQP_T;
typedef struct axVENC_H264_QVBR_T AX_VENC_H265_QVBR_T;
typedef struct axVENC_H264_CVBR_T AX_VENC_H265_CVBR_T;


/* the attribute of mjpege fixqp*/
typedef struct axVENC_MJPEG_FIXQP_T
{
    /**
     * Range:[0, 51]; Fixed qp for every frame.
     */
    AX_S32 s32FixedQp;
} AX_VENC_MJPEG_FIXQP_T;

/* the attribute of mjpege cbr*/
typedef struct axVENC_MJPEG_CBR_T
{
    AX_U32 u32StatTime;      /* Range:[1, 60]; the rate statistic time, the unit is senconds(s) */
    AX_U32 u32BitRate;       /* average bitrate ,the unit is kbps*/
    AX_U32 u32MaxQp;         /* Range:[1, 51]; the max Qfactor value*/
    AX_U32 u32MinQp;         /* Range:[0, 51]; the min Qfactor value ,can not be larger than u32MaxQfactor */
} AX_VENC_MJPEG_CBR_T;

/* the attribute of mjpege vbr*/
typedef struct axVENC_MJPEG_VBR_T
{
    AX_U32 u32StatTime;      /* Range:[1, 60]; the rate statistic time, the unit is senconds(s) */
    AX_U32 u32MaxBitRate;    /* the max bitrate ,the unit is kbps*/
    AX_U32 u32MaxQp;         /* Range:[1, 51]; max image quailty allowed */
    AX_U32 u32MinQp;         /* Range:[0, 51]; min image quality allowed ,can not be larger than u32MaxQfactor*/
} AX_VENC_MJPEG_VBR_T;

/* the attribute of rc*/
typedef struct axVENC_RC_ATTR_T
{
    AX_VENC_RC_MODE_E enRcMode; /* the type of rc*/
    /**
     * Range[-1, 51]; Start QP value of the first frame
     * -1: Encoder calculates initial QP.
     */
    AX_S32 s32FirstFrameStartQp;
    /**
     * use nSrcFrameRate and nDstFrameRate include in struct.
     */
    AX_FRAME_RATE_CTRL_T stFrameRate;
    union
    {
        AX_VENC_H264_CBR_T stH264Cbr;
        AX_VENC_H264_VBR_T stH264Vbr;
        AX_VENC_H264_AVBR_T stH264AVbr;
        AX_VENC_H264_QVBR_T stH264QVbr;
        AX_VENC_H264_CVBR_T stH264CVbr;
        AX_VENC_H264_FIXQP_T stH264FixQp;
        AX_VENC_H264_QPMAP_T stH264QpMap; /* N/A for AX615 */

        AX_VENC_H265_CBR_T stH265Cbr;
        AX_VENC_H265_VBR_T stH265Vbr;
        AX_VENC_H265_AVBR_T stH265AVbr;
        AX_VENC_H265_QVBR_T stH265QVbr;
        AX_VENC_H265_CVBR_T stH265CVbr;
        AX_VENC_H265_FIXQP_T stH265FixQp;
        AX_VENC_H265_QPMAP_T stH265QpMap; /* N/A for AX615 */

        AX_VENC_MJPEG_CBR_T stMjpegCbr;
        AX_VENC_MJPEG_VBR_T stMjpegVbr;
        AX_VENC_MJPEG_FIXQP_T stMjpegFixQp;
    };
} AX_VENC_RC_ATTR_T;

typedef struct axVENC_SCENE_CHANGE_DETECT_T
{
    AX_BOOL bDetectSceneChange;      /* Range:[0, 1]; enable detect scene change.*/
    AX_BOOL bAdaptiveInsertIDRFrame; /* Range:[0, 1]; enable a daptive insertIDR frame.*/
} AX_VENC_SCENE_CHANGE_DETECT_T;

typedef enum
{
    /* def disabled */
    AX_VENC_BPS_AUTO_ADAPT_DISABLE = 0,
    /* when enabled, bps will keep stable when fps lower than dst fps
    *  1、when real fps < dst fps, i frms will insert to stream
    *  2、gop size also changed when fps lower than dst fps
    * */
    AX_VENC_BPS_AUTO_ADAPT_ENABLE = 1
} AX_VENC_BPS_AUTO_ADAPT_E;

typedef enum
{
    /* def disabled */
    AX_VENC_FPS_AUTO_ADAPT_DISABLE = 0,
    /* when enabled, just smooth image qualiy, not set fps_new to rate control alg. 
    * */
    AX_VENC_FPS_AUTO_ADAPT_ENABLE = 1
} AX_VENC_FPS_AUTO_ADAPT_E;

/* The param of rc*/
typedef struct axVENC_RC_PARAM_T
{
    AX_U32 u32RowQpDelta;                   /* Range:[0, 10]; The start QP value of each macroblock row relative to the start QP value */
    AX_S32 s32FirstFrameStartQp;            /* Range:[-1, 51]; Start QP value of the first frame */
    AX_VENC_SCENE_CHANGE_DETECT_T stSceneChangeDetect;
    AX_VENC_RC_MODE_E enRcMode;             /* Ref to AX_VENC_RC_MODE_E; The type of RC */

    AX_VENC_BPS_AUTO_ADAPT_E enBpsAdapt;    /* Ref to AX_VENC_BPS_AUTO_ADAPT_E; The bit rate is adaptively adjusted according to the fps */
    AX_VENC_FPS_AUTO_ADAPT_E enFpsAdaptSpecial;
    
    /**
     * use nSrcFrameRate and nDstFrameRate include in struct.
     */
    AX_FRAME_RATE_CTRL_T stFrameRate;
    union
    {
        AX_VENC_H264_CBR_T stH264Cbr;
        AX_VENC_H264_VBR_T stH264Vbr;
        AX_VENC_H264_AVBR_T stH264AVbr;
        AX_VENC_H264_QVBR_T stH264QVbr;
        AX_VENC_H264_CVBR_T stH264CVbr;
        AX_VENC_H264_FIXQP_T stH264FixQp;
        AX_VENC_H264_QPMAP_T stH264QpMap;

        AX_VENC_H265_CBR_T stH265Cbr;
        AX_VENC_H265_VBR_T stH265Vbr;
        AX_VENC_H265_AVBR_T stH265AVbr;
        AX_VENC_H265_QVBR_T stH265QVbr;
        AX_VENC_H265_CVBR_T stH265CVbr;
        AX_VENC_H265_FIXQP_T stH265FixQp;
        AX_VENC_H265_QPMAP_T stH265QpMap;

        AX_VENC_MJPEG_CBR_T stMjpegCbr;
        AX_VENC_MJPEG_VBR_T stMjpegVbr;
        AX_VENC_MJPEG_FIXQP_T stMjpegFixQp;
    };
} AX_VENC_RC_PARAM_T;

/* the frame lost mode*/
typedef enum
{
    AX_VENC_DROPFRM_NORMAL = 0, /*normal mode*/
    AX_VENC_DROPFRM_PSKIP,      /*pskip*/
    AX_VENC_DROPFRM_BUTT,
} AX_VENC_DROPFRAME_MODE_E;

/* The param of the rate jam mode*/
typedef struct axVENC_RATE_JAM_T
{
    AX_BOOL bDropFrmEn;                     /* RW; Range:[0, 1]; Indicates whether to enable rate jam stragety */
    AX_U32 u32DropFrmThrBps;                /* RW; Range:[64, 163840]; The instant bit rate threshold, the unit is kbps */
    AX_VENC_DROPFRAME_MODE_E enDropFrmMode; /* RW; Ref to AX_VENC_DROPFRAME_MODE_E; Drop frame mode */
    AX_U32 u32EncFrmGaps;                   /* RW; Range:[0, 65535]; The continue frame numbers to do rate jam stragegy */
} AX_VENC_RATE_JAM_CFG_T;

/* the rc priority*/
typedef enum
{
    AX_VENC_RC_PRIORITY_FRAMEBITS_FIRST = 0, /* framebits first*/
    AX_VENC_RC_PRIORITY_BITRATE_FIRST,       /* bitrate first */

    AX_VENC_RC_PRIORITY_BUTT,
} AX_VENC_RC_PRIORITY_E;

/* the config of the superframe */
typedef struct axVENC_SUPERFRAME_CFG_T
{
    AX_BOOL bStrategyEn;        /* RW; Range:[0, 1]; super frame strategy enable */
    AX_U32 u32SuperIFrmBitsThr; /* RW; Range:[0, 4294967295]; Indicate the threshold of the super I frame for enabling the
                                   super frame processing mode */
    AX_U32 u32SuperPFrmBitsThr; /* RW; Range:[0, 4294967295]; Indicate the threshold of the super P frame for enabling the
                                   super frame processing mode */
    AX_U32 u32SuperBFrmBitsThr; /* RW; Range:[0, 4294967295]; Indicate the threshold of the super B frame for enabling the
                                   super frame processing mode */
    AX_VENC_RC_PRIORITY_E enRcPriority; /* RW; Ref to AX_VENC_RC_PRIORITY_E; Rate control priority */
} AX_VENC_SUPERFRAME_CFG_T;


#ifdef __cplusplus
#if __cplusplus
}
#endif
#endif /* __cplusplus */

#endif /* __AX_VENC_RC_H__ */
