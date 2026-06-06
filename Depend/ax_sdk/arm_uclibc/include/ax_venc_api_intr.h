/**************************************************************************************************
 *
 * Copyright (c) 2019-2025 Axera Semiconductor Co., Ltd. All Rights Reserved.
 *
 * This source file is the property of Axera Semiconductor Co., Ltd. and
 * may not be copied or distributed in any isomorphic form without the prior
 * written consent of Axera Semiconductor Co., Ltd.
 *
 **************************************************************************************************/

#ifndef __AX_VENC_API_INTR_H__
#define __AX_VENC_API_INTR_H__

#include "ax_venc_comm.h"
#include "ax_global_type.h"

#ifdef __cplusplus
extern "C" {
#endif /* End of #ifdef __cplusplus */

typedef struct axVENC_DEBUG_FIFO_ATTR_T
{
    AX_VENC_DUMP_TYPE_E enDumpType;
    AX_U16 u16InFifoDepth;
    AX_U16 u16OutFifoDepth;
} AX_VENC_DEBUG_FIFO_ATTR_T;

/**************** 供内部图像调优使用PARAM ****************/
typedef struct axVENC_MODEL_WEIGHT_ATTR_T
{
    AX_U32    au32P1ModelWeight[3];     /* RW; Range:[0, 15] */
    AX_U32    au32P2ModelWeight[3];     /* RW; Range:[0, 15] */
} AX_VENC_MODEL_WEIGHT_ATTR_T;

typedef struct axVENC_H26X_PENALTY_T
{
    AX_U32    u32DcPenalty;             /* RW; Range:[0, 1023] */
    AX_U32    u32DcPenaltyRoi;          /* RW; Range:[0, 1023] */
    AX_U32    u32PlanarPenalty;         /* RW; Range:[0, 1023] */
    AX_U32    u32PlanarPenaltyRoi;      /* RW; Range:[0, 1023] */
    AX_U32    u32AngPenalty;            /* RW; Range:[0, 1023] */
    AX_U32    u32AngPenaltyRoi;         /* RW; Range:[0, 1023] */
} AX_VENC_H26X_PENALTY_T;

typedef struct axVENC_INTRA_PENALTY_ATTR_T
{
    AX_U32    u32IntraPredPenaltyMode;  /* RW; Range:[0, 3] */
    AX_U32    u32IntraChromaThd;        /* RW; Range:[0, 1023] */
    AX_U32    u32ChromaNonDmOffset;     /* RW; Range:[0, 255] */
    AX_U32    u32ImdUVModeRate0to3;     /* RW; Range:[0, 1023] */
    AX_U32    u32ImdUVModeRate4;        /* RW; Range:[0, 1023] */
    AX_VENC_H26X_PENALTY_T stH26xPenalty;
} AX_VENC_INTRA_PENALTY_ATTR_T;

typedef struct axVENC_RC_EXT_PARAM_ATTR_T
{
    AX_U32    u32RcExtraBitPercent;     /* RW; Range:[0, 100]; Equal to u32UpperLimitPercent */
    AX_BOOL   bPixelFillMode;           /* RW; Range:[0, 1]; False use under y/u/v value */
    AX_U8     u8PixelFillY;             /* RW; Range:[0, 255] */
    AX_U8     u8PixelFillU;             /* RW; Range:[0, 255] */
    AX_U8     u8PixelFillV;             /* RW; Range:[0, 255] */
} AX_VENC_RC_EXT_PARAM_ATTR_T;

typedef struct axVENC_ADVANCED_PARAM_T
{
    AX_BOOL   bIspInfoEn;               /* RW; Range:[0, 1] */
    AX_BOOL   bCuSearchEn;              /* RW; Range:[0, 1] */
    AX_BOOL   bCavlcTu4LevelLimitEn;    /* RW; Range:[0, 1] */
    AX_BOOL   bFirstFrmReEncEn;         /* RW; Range:[0, 1] */
    AX_BOOL   bTrunSelEn;               /* RW; Range:[0, 1] */

    AX_BOOL   bRdoForceAutoEn;          /* RW; Range:[0, 1] */
    AX_BOOL   bDeblurForceAutoEn;       /* RW; Range:[0, 1] */
    AX_BOOL   bDebreathForceAutoEn;     /* RW; Range:[0, 1] */
    AX_BOOL   bGainProcForceAutoEn;     /* RW; Range:[0, 1] */
    AX_BOOL   bGmProcForceAutoEn;       /* RW; Range:[0, 1] */
    AX_BOOL   bSkinDetectForceAutoEn;   /* RW; Range:[0, 1] */
    AX_BOOL   bChromaProtForceAutoEn;   /* RW; Range:[0, 1] */
    AX_BOOL   bSubjQualRecForceAutoEn;  /* RW; Range:[0, 1] */
    AX_BOOL   bSubjSplitForceAutoEn;    /* RW; Range:[0, 1] */
    AX_BOOL   bLowPowerForceAutoEn;     /* RW; Range:[0, 1] */
    AX_BOOL   bHvsForceAutoEn;          /* RW; Range:[0, 1] */
    AX_BOOL   bInterStatForceAutoEn;    /* RW; Range:[0, 1] */
    AX_BOOL   bSceneDetectForceAutoEn;  /* RW; Range:[0, 1] */
    AX_BOOL   bRsv0ForceAutoEn;         /* RW; Range:[0, 1] */
    AX_BOOL   bRsv1ForceAutoEn;         /* RW; Range:[0, 1] */
    AX_BOOL   bRsv2ForceAutoEn;         /* RW; Range:[0, 1] */
    AX_BOOL   bRsv3ForceAutoEn;         /* RW; Range:[0, 1] */
    AX_BOOL   bRsv4ForceAutoEn;         /* RW; Range:[0, 1] */
    AX_BOOL   bRsv5ForceAutoEn;         /* RW; Range:[0, 1] */
    AX_BOOL   bRsv6ForceAutoEn;         /* RW; Range:[0, 1] */
    AX_BOOL   bRsv7ForceAutoEn;         /* RW; Range:[0, 1] */
    AX_BOOL   bRsv8ForceAutoEn;         /* RW; Range:[0, 1] */
    AX_BOOL   bRsv9ForceAutoEn;         /* RW; Range:[0, 1] */
    AX_BOOL   bRsv10ForceAutoEn;        /* RW; Range:[0, 1] */

    AX_BOOL   bInternalFgIndgEn;        /* RW; Range:[0, 1] */
    AX_BOOL   bIspDynamicInspectEn;     /* RW; Range:[0, 1] */
    AX_U32    u32DeBreathThr0;          /* RW; Range:[0, 255] */
    AX_U32    u32DeBreathThr1;          /* RW; Range:[0, 255] */
    AX_U32    u32SThdOffsetInPred;      /* RW; Range:[0, 255] */

    AX_S32    s32IspGain;               /* RW; Range:[-1, 255], Default：-1
                                            -1：   代表使用内部自适应的isp增益值，该值在venc灌源时固定为0
                                            0-255：代表使用接口传入的isp增益值 */
} AX_VENC_ADVANCED_PARAM_T;

typedef enum
{
    AX_VENC_YUV_PASS_THROUGH       = 0,    /* 数据直通，不进行任何处理，原样读取yuv分量 */
    AX_VENC_YUV_CLIP_RANGE         = 1,    /* YUV限幅，Y[0,255] => Y[16,235]，UV[0,255] => UV[16,240] */
    AX_VENC_YUV_EXPANSION_RANGE    = 2,    /* 将限幅后的YUV范围反映射到正常范围，Y[16,235] => Y[0,255]，UV[16,240] => UV[0,255] */
    AX_VENC_YUV_RANGE_BUTT
}AX_VENC_YUV_RANGE_E;

AX_S32 AX_VENC_SetDbgFifoAttr(VENC_CHN VeChn, AX_VENC_ENCODER_TYPE_E enType, const AX_VENC_DEBUG_FIFO_ATTR_T *pstAttr);
AX_S32 AX_VENC_GetDbgFifoAttr(VENC_CHN VeChn, AX_VENC_ENCODER_TYPE_E enType, AX_VENC_DEBUG_FIFO_ATTR_T *pstAttr);
AX_S32 AX_VENC_GetFrameInDbgFifo(VENC_CHN VeChn, AX_VENC_ENCODER_TYPE_E enType, AX_VIDEO_FRAME_INFO_T *pstVFrame);
AX_S32 AX_VENC_ReleaseFrameInDbgFifo(VENC_CHN VeChn, AX_VENC_ENCODER_TYPE_E enType, const AX_VIDEO_FRAME_INFO_T *pstVFrame);


/***************** 供内部图像调优使用API *****************/
AX_S32 AX_VENC_SetModelWeightAttr(VENC_CHN VeChn, const AX_VENC_MODEL_WEIGHT_ATTR_T *pstModelWeightAttr);
AX_S32 AX_VENC_GetModelWeightAttr(VENC_CHN VeChn, AX_VENC_MODEL_WEIGHT_ATTR_T *pstModelWeightAttr);

AX_S32 AX_VENC_SetIntraPenaltyAttr(VENC_CHN VeChn, const AX_VENC_INTRA_PENALTY_ATTR_T *pstPenaltyAttr);
AX_S32 AX_VENC_GetIntraPenaltyAttr(VENC_CHN VeChn, AX_VENC_INTRA_PENALTY_ATTR_T *pstPenaltyAttr);

AX_S32 AX_VENC_SetRcExtParamAttr(VENC_CHN VeChn, const AX_VENC_RC_EXT_PARAM_ATTR_T *pstRcExtParamAttr);
AX_S32 AX_VENC_GetRcExtParamAttr(VENC_CHN VeChn, AX_VENC_RC_EXT_PARAM_ATTR_T *pstRcExtParamAttr);

AX_S32 AX_VENC_SetAdvancedParam(VENC_CHN VeChn, const AX_VENC_ADVANCED_PARAM_T *pstAdvancedParam);
AX_S32 AX_VENC_GetAdvancedParam(VENC_CHN VeChn, AX_VENC_ADVANCED_PARAM_T *pstAdvancedParam);

AX_S32 AX_VENC_SetYuvRange(VENC_CHN VeChn, AX_VENC_YUV_RANGE_E enYuvRange);

#ifdef __cplusplus
}
#endif /* End of #ifdef __cplusplus */

#endif /* End of __AX_VENC_API_INTR_H__ */