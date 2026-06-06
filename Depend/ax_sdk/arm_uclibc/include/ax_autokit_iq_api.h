/**************************************************************************************************
 *
 * Copyright (c) 2019-2025 Axera Semiconductor Co., Ltd. All Rights Reserved.
 *
 * This source file is the property of Axera Semiconductor Co., Ltd. and
 * may not be copied or distributed in any isomorphic form without the prior
 * written consent of Axera Semiconductor Co., Ltd.
 *
 **************************************************************************************************/
#ifndef __AX_AUTOKIT_IQ_API_H__
#define __AX_AUTOKIT_IQ_API_H__

#include "ax_isp_iq_api.h"

#ifdef __cplusplus
extern "C"
{
#endif

/************************************************************************************
 *  BLC/BL_DET IQ Auto Ext Param
 ************************************************************************************/
#define AX_ISP_REF_AUTOTBL_BLC_AGAIN_EXPRESS_NUM                         (16)
#define AX_ISP_REF_AUTOTBL_BLC_EXP_TIME_EXPRESS_NUM                      (10)

typedef struct {
    /* ref_mode: exp_time && again */
    AX_U8                            nParamGrpNumExpTime;                                                                                            /* Accuracy: U8.0 Range: [1, 10] */
    AX_U32                           nRefValExpTime[AX_ISP_REF_AUTOTBL_BLC_EXP_TIME_EXPRESS_NUM];                                                    /* Accuracy: U32.0 Range: [0, 4294967295] */
    AX_U8                            nParamGrpNumGain;                                                                                               /* Accuracy: U8.0 Range: [1, 16] */
    AX_U32                           nRefValGain[AX_ISP_REF_AUTOTBL_BLC_AGAIN_EXPRESS_NUM];                                                          /* Accuracy: U22.10 Range: [1024, 4294967295] */
    AX_U32                           nSblVal[AX_ISP_REF_AUTOTBL_BLC_AGAIN_EXPRESS_NUM][AX_ISP_REF_AUTOTBL_BLC_EXP_TIME_EXPRESS_NUM][AX_ISP_BAYER_CHN_NUM];              /* Display: sblVal, Accuracy: U8.10, Range: [0, 262143], auto param, interpolation based on exposure time and gain*/
    AX_S32                           nGblVal[AX_ISP_REF_AUTOTBL_BLC_AGAIN_EXPRESS_NUM][AX_ISP_REF_AUTOTBL_BLC_EXP_TIME_EXPRESS_NUM][AX_ISP_BAYER_CHN_NUM];              /* Display: gblVal, Accuracy: S7.10, Range: [-131072, 131071], auto param, interpolation based on exposure time and gain*/
    AX_U32                           nDpcDetTh[AX_ISP_REF_AUTOTBL_BLC_AGAIN_EXPRESS_NUM][AX_ISP_REF_AUTOTBL_BLC_EXP_TIME_EXPRESS_NUM];               /* Display: bpcDetTh, Accuracy: U8.10, Range: [0, 262143], threshold for quickly checking whether the current point is a bad point*/
} AX_ISP_IQ_BLC_AUTO_TABLE_T;

typedef struct {
    AX_ISP_IQ_BLC_AUTO_TABLE_T       tHcgAutoTable;
    AX_ISP_IQ_BLC_AUTO_TABLE_T       tLcgAutoTable;
} AX_ISP_IQ_BLC_AUTO_T;

typedef struct {
    AX_U8                            nBlcEn;                              /* blc enable, Accuracy: U1.0 Range: [0, 1], 0: Disable, 1: Enable */
    AX_U8                            nSblEnable;                          /* Display: sblCorrEnable, Accuracy: U1.0, Range: [0, 1], sbl corr enable*/
    AX_U8                            nGblEnable;                          /* Display: gblCorrEnable, Accuracy: U1.0, Range: [0, 1], gbl corr enable*/
    AX_U8                            nDetEnable;                          /* Display: detEnable, Accuracy: U1.0, Range: [0, 1], det enable, 0: disable, 1: enable*/
    AX_U8                            nAutoMode;                           /* for ref auto or manual adjust mode, Accuracy: U1.0 Range:[0, 1], 0: manual, 1:auto, default:1 */
    AX_ISP_IQ_BLC_MANUAL_T           tManualParam[AX_ISP_BLC_DEV_CHN_NUM];
    AX_ISP_IQ_BLC_AUTO_T             tAutoParam;
} AX_ISP_IQ_BLC_PARAM_EX_T;

/************************************************************************************
 *  LIN IQ Auto Ext Param
 ************************************************************************************/
typedef struct {
    AX_U8                            nLinEn;                                /* lin enable, Accuracy: U1.0 Range: [0, 1], 0: Disable, 1: Enable */
    AX_ISP_IQ_LIN_MANUAL_T           tManualParam;
} AX_ISP_IQ_LIN_PARAM_EX_T;

/************************************************************************************
 *  DPC IQ Auto Ext Param
 ************************************************************************************/
#define AX_ISP_REF_AUTOTBL_DPC_EXPRESS_NUM                               (16)

typedef struct {
    /* ref_mode: gain/lux */
    AX_U8                            nRefMode;                                                    /* choose ref mode, Accuracy: U1.0 Range: [0, 1], 0:use lux as ref, 1:use gain as ref */
    AX_U8                            nParamGrpNum;                                                /* Accuracy: U8.0 Range: [1, 16] */
    AX_U32                           nRefVal[AX_ISP_REF_AUTOTBL_DPC_EXPRESS_NUM];                 /* Gain: Accuracy: U22.10 Range: [0x400, 0xFFFFFFFF]; Lux: Accuracy: U22.10 Range: [0, 0xFFFFFFFF] */
    AX_U16                           nNoiseRatio[AX_ISP_REF_AUTOTBL_DPC_EXPRESS_NUM];             /* Display: noiseRatio, Accuracy: U4.10, Range: [0, 16383], noise level gain, the larger the value, the larger the noise level, less defect pixels*/
    AX_U8                            nPredetLevelSlope[AX_ISP_REF_AUTOTBL_DPC_EXPRESS_NUM];       /* Display: predetLevelSlope, Accuracy: U0.4, Range: [0, 15], defect pixel margin gain for quick detection, the larger the value, less defect pixel detected*/
    AX_U16                           nPredetLevelOffset[AX_ISP_REF_AUTOTBL_DPC_EXPRESS_NUM];      /* Display: predetLevelOffset, Accuracy: U8.6, Range: [0, 16383], defect pixel base margin for quick detection, the larger the value, less defect pixel detected*/
    AX_U16                           nPredetLevelMax[AX_ISP_REF_AUTOTBL_DPC_EXPRESS_NUM];         /* Display: predetLevelMax, Accuracy: U8.6, Range: [0, 16383], defect pixel max margin for quick detection, the larger the value, less defect pixel detected*/
    AX_U8                            nDefectivePixelType[AX_ISP_REF_AUTOTBL_DPC_EXPRESS_NUM];     /* Display: defectivePixelType, Accuracy: U1.0, Range: [0, 1], Dynamic dp Detect Module, 0:single defective pixel mode, 1:dual defective pixel mode*/
    AX_U8                            nDetFineStr[AX_ISP_REF_AUTOTBL_DPC_EXPRESS_NUM];             /* Display: detFineStr, Accuracy: U0.6, Range: [0, 63], local detction strength for defect pixel detection, the larger the value, more defect pixels will be detected*/
    AX_U8                            nDetCoarseStr[AX_ISP_REF_AUTOTBL_DPC_EXPRESS_NUM];           /* Display: detCoarseStr, Accuracy: U4.4, Range: [0, 255], defect pixel detection threshold, the larger the value, more defect pixels are detected*/
    AX_U16                           nDynamicDpcStr[AX_ISP_REF_AUTOTBL_DPC_EXPRESS_NUM];          /* Display: dynamicDpcStr, Accuracy: U4.8, Range: [0, 4095], the larger the value, the larger the defect pixel probability*/
    AX_U8                            nEdgeStr[AX_ISP_REF_AUTOTBL_DPC_EXPRESS_NUM];                /* Display: edgeStr, Accuracy: U1.7, Range: [0, 255], direction interpolation ratio used in interpolation, the larger the value, more direction interpolated result used in interpolation, better performance for defective pixels at edge locations*/
    AX_U8                            nHotColdTypeStr[AX_ISP_REF_AUTOTBL_DPC_EXPRESS_NUM];         /* Display: hotColdTypeStr, Accuracy: U1.7, Range: [0, 128], hot_cold result ratio, the larger the value, more hot_cold result used*/
    AX_U8                            nSupWinkThr[AX_ISP_REF_AUTOTBL_DPC_EXPRESS_NUM];             /* Display: supWinkThr, Accuracy: U4.4, Range: [0, 255], the threshold to use direction interpoation or local avg interpolation result*/
    AX_U16                           nDynamicDpUpperlimit[AX_ISP_REF_AUTOTBL_DPC_EXPRESS_NUM];    /* Display: dynamicDpUpperlimit, Accuracy: U8.6, Range: [0, 16383], up margin for color limit, the larger the value, less color limitation*/
    AX_U16                           nDynamicDpLowerlimit[AX_ISP_REF_AUTOTBL_DPC_EXPRESS_NUM];    /* Display: dynamicDpLowerlimit, Accuracy: U8.6, Range: [0, 16383], low margin for color limit, the larger the value, less color limitation*/
    AX_U16                           nStaticDpUpperlimit[AX_ISP_REF_AUTOTBL_DPC_EXPRESS_NUM];     /* Display: staticDpUpperlimit, Accuracy: U8.6, Range: [0, 16383], up margin for color limit, the larger the value, less color limitation*/
    AX_U16                           nStaticDpLowerlimit[AX_ISP_REF_AUTOTBL_DPC_EXPRESS_NUM];     /* Display: staticDpLowerlimit, Accuracy: U8.6, Range: [0, 16383], low margin for color limit, the larger the value, less color limitation*/
    AX_U16                           nNormalPixelUpperlimit[AX_ISP_REF_AUTOTBL_DPC_EXPRESS_NUM];  /* Display: normalPixelUpperlimit, Accuracy: U8.6, Range: [0, 16383], up margin for color limit, the larger the value, less color limitation*/
    AX_U16                           nNormalPixelLowerlimit[AX_ISP_REF_AUTOTBL_DPC_EXPRESS_NUM];  /* Display: normalPixelLowerlimit, Accuracy: U8.6, Range: [0, 16383], low margin for color limit, the larger the value, less color limitation*/
} AX_ISP_IQ_DPC_AUTO_T;

typedef struct {
    AX_U8                            nDpcEn;                              /* dpc enable, Accuracy: U1.0 Range: [0, 1], 0: Disable, 1: Enable */
    AX_U8                            nAutoMode;                           /* for ref auto or manual adjust mode, Accuracy: U1.0 Range:[0, 1], 0: manual, 1:auto, default:1 */
    AX_ISP_IQ_DPC_MANUAL_T           tManualParam;
    AX_ISP_IQ_DPC_AUTO_T             tAutoParam;
} AX_ISP_IQ_DPC_PARAM_EX_T;

/************************************************************************************
 *  HDR IQ Auto Ext Param
 ************************************************************************************/

#define AX_ISP_REF_AUTOTBL_HDR_EXPRESS_NUM                               (16)

typedef struct {
    /* ref_mode: gain/lux */
    AX_U8                            nRefMode;                                                    /* choose ref mode, Accuracy: U1.0 Range: [0, 1], 0:use lux as ref, 1:use gain as ref */
    AX_U8                            nParamGrpNum;                                                /* Accuracy: U8.0 Range: [1, 16] */
    AX_U32                           nRefVal[AX_ISP_REF_AUTOTBL_HDR_EXPRESS_NUM];                 /* Gain: Accuracy: U22.10 Range: [0x400, 0xFFFFFFFF]; Lux: Accuracy: U22.10 Range: [0, 0xFFFFFFFF] */
    AX_U16                           nNoiseLutScale[AX_ISP_REF_AUTOTBL_HDR_EXPRESS_NUM];          /* Display: noiseLutScale, Accuracy: U4.12, Range: [0, 65535], Noise profile will be multiplied by this param.*/
    AX_ISP_HDR_DGST_BASE_FID_E       eDgstBaseFid[AX_ISP_REF_AUTOTBL_HDR_EXPRESS_NUM];            /* Display: dgstBaseFid, Accuracy: U1.0, Range: [0, 1], Specify which frame to be used as dgst fusion result when both exp mask is 0 for a pixel. 0 - SF; 1 - LF*/
    AX_U16                           nExpWeightGain[AX_ISP_REF_AUTOTBL_HDR_EXPRESS_NUM][AX_ISP_HDR_EXP_FRAME_NUM];       /* Display: expWeightGain, Accuracy: U1.8, Range: [0, 256], Gain applied on exposure mask before exp mask normalization.*/
    AX_U16                           nExpWeightLutLumaLow[AX_ISP_REF_AUTOTBL_HDR_EXPRESS_NUM][AX_ISP_HDR_EXP_FRAME_NUM][AX_ISP_HDR_EXP_WGT_LUT_NUM]; /* Display: expWeightLutLumaLow, Accuracy: U1.15, Range: [0, 32768], Weight lut of exposure mask for dark area. It's used to generate exposure weight based on pixel luma value.*/
    AX_U16                           nExpWeightLutLumaHigh[AX_ISP_REF_AUTOTBL_HDR_EXPRESS_NUM][AX_ISP_HDR_EXP_FRAME_NUM][AX_ISP_HDR_EXP_WGT_LUT_NUM]; /* Display: expWeightLutLumaHigh, Accuracy: U1.15, Range: [0, 32768], Weight lut of exposure mask for bright area. It's used to generate exposure weight based on pixel luma value.*/
    AX_U16                           nMotionMaskEnhc[AX_ISP_REF_AUTOTBL_HDR_EXPRESS_NUM];         /* Display: motionMaskEnhc, Accuracy: U8.8, Range: [0, 65535], This param will enhance motion mask.*/
    AX_U16                           nMotionMaskNoiseLevel[AX_ISP_REF_AUTOTBL_HDR_EXPRESS_NUM];   /* Display: motionMaskNoiseLevel, Accuracy: U1.11, Range: [0, 4095], This parameter acts like motion mask threshold, cutting the motion mask.*/
} AX_ISP_IQ_HDR_AUTO_T;

typedef struct {
    AX_U8                            nHdrEn;                              /* hdr enable, Accuracy: U1.0 Range: [0, 1], 0: Disable, 1: Enable */
    AX_U8                            nAutoMode;                           /* for ref auto or manual adjust mode, Accuracy: U1.0 Range:[0, 1], 0: manual, 1:auto, default:1 */
    AX_ISP_IQ_HDR_MANUAL_T           tManualParam;
    AX_ISP_IQ_HDR_AUTO_T             tAutoParam;
} AX_ISP_IQ_HDR_PARAM_EX_T;

/************************************************************************************
 *  AINR IQ Auto Ext Param
 ************************************************************************************/
#define AX_ISP_REF_AUTOTBL_AINR_EXPRESS_NUM                              (12)
#define AX_ISP_REF_AUTOTBL_AINR_HDR_RATIO_EXPRESS_NUM                    (4)

typedef struct {
    /* ref_mode: gain/lux && hdr_ratio */
    AX_U8                            nRefMode;                                                                                                    /* choose ref mode, Accuracy: U1.0 Range: [0, 1], 0:use lux as ref, 1:use gain as ref */
    AX_U8                            nParamGrpNum;                                                                                                /* Accuracy: U8.0 Range: [1, 12] */
    AX_U32                           nRefVal[AX_ISP_REF_AUTOTBL_AINR_EXPRESS_NUM];                                                                /* Gain: Accuracy: U22.10 Range: [0x400, 0xFFFFFFFF]; Lux: Accuracy: U22.10 Range: [0, 0xFFFFFFFF] */
    AX_U8                            nParamGrpNumHdrRatio;                                                                                        /* Accuracy: U8.0 Range: [1, 4] */
    AX_U32                           nRefValHdrRatio[AX_ISP_REF_AUTOTBL_AINR_HDR_RATIO_EXPRESS_NUM];                                              /* Accuracy: U7.10 Range: [1024, 130048] */
    AX_U8                            nLutTemporalBlend[AX_ISP_REF_AUTOTBL_AINR_HDR_RATIO_EXPRESS_NUM][AX_ISP_REF_AUTOTBL_AINR_EXPRESS_NUM][AX_ISP_AINR_LUT_NUM_MAX][AX_ISP_AINR_LUT_SIZE_MAX]; /* Display: lutTemporalBlend, Accuracy: U8.0, Range: [0, 255], lut_temporal_blend*/
    AX_U8                            nLutSpatialBlend[AX_ISP_REF_AUTOTBL_AINR_HDR_RATIO_EXPRESS_NUM][AX_ISP_REF_AUTOTBL_AINR_EXPRESS_NUM][AX_ISP_AINR_LUT_NUM_MAX][AX_ISP_AINR_LUT_SIZE_MAX]; /* Display: lutSpatialBlend, Accuracy: U8.0, Range: [0, 255], lut_spatial_blend*/
    AX_U8                            nLutSpatialGuidance[AX_ISP_REF_AUTOTBL_AINR_HDR_RATIO_EXPRESS_NUM][AX_ISP_REF_AUTOTBL_AINR_EXPRESS_NUM][AX_ISP_AINR_LUT_NUM_MAX][AX_ISP_AINR_LUT_SIZE_MAX]; /* Display: lutSpatialGuidance, Accuracy: U8.0, Range: [0, 255], lut_spatial_guidance*/
    AX_U8                            nLutHdrMask[AX_ISP_REF_AUTOTBL_AINR_HDR_RATIO_EXPRESS_NUM][AX_ISP_REF_AUTOTBL_AINR_EXPRESS_NUM][AX_ISP_AINR_LUT_NUM_MAX][AX_ISP_AINR_LUT_SIZE_MAX];      /* Display: lutHdrMask, Accuracy: U8.0, Range: [0, 255], lut_hdr_mask*/
    AX_U8                            nLutExtTemporal[AX_ISP_REF_AUTOTBL_AINR_HDR_RATIO_EXPRESS_NUM][AX_ISP_REF_AUTOTBL_AINR_EXPRESS_NUM][AX_ISP_AINR_LUT_NUM_MAX][AX_ISP_AINR_LUT_SIZE_MAX];  /* Display: lutExtTemporal, Accuracy: U8.0, Range: [0, 255], lut_ext_temporal*/
    AX_U8                            nLutHighFreq[AX_ISP_REF_AUTOTBL_AINR_HDR_RATIO_EXPRESS_NUM][AX_ISP_REF_AUTOTBL_AINR_EXPRESS_NUM][AX_ISP_AINR_LUT_NUM_MAX][AX_ISP_AINR_LUT_SIZE_MAX];     /* Display: lutHighFreq, Accuracy: U8.0, Range: [0, 255], lut_high_frequency*/
    /* ref_mode: gain/lux && hdr_ratio */
    AX_CHAR                          szCurrModelPath[AX_ISP_REF_AUTOTBL_AINR_HDR_RATIO_EXPRESS_NUM][AX_ISP_REF_AUTOTBL_AINR_EXPRESS_NUM][AX_ISP_AINR_MODEL_PATH_SIZE_MAX];    /* Display: currModelPath, curr_model_path */
    AX_CHAR                          szCurrTemporalNrName[AX_ISP_REF_AUTOTBL_AINR_HDR_RATIO_EXPRESS_NUM][AX_ISP_REF_AUTOTBL_AINR_EXPRESS_NUM][AX_ISP_AINR_MODEL_PATH_SIZE_MAX]; /* Display: currTemporalNrName, curr_temporal_nr_name */
    AX_CHAR                          szCurrSpatialNrName[AX_ISP_REF_AUTOTBL_AINR_HDR_RATIO_EXPRESS_NUM][AX_ISP_REF_AUTOTBL_AINR_EXPRESS_NUM][AX_ISP_AINR_MODEL_PATH_SIZE_MAX]; /* Display: currSpatialNrName, curr_spatial_nr_name */
    AX_S16                           nBiasOut[AX_ISP_REF_AUTOTBL_AINR_HDR_RATIO_EXPRESS_NUM][AX_ISP_REF_AUTOTBL_AINR_EXPRESS_NUM][AX_ISP_AINR_BIAS_OUT_NUM_MAX];             /* Display: biasOut, Accuracy: S9.6, Range: [-32768, 32767], bias_out*/
    AX_U16                           nOffset2d[AX_ISP_REF_AUTOTBL_AINR_HDR_RATIO_EXPRESS_NUM][AX_ISP_REF_AUTOTBL_AINR_EXPRESS_NUM][AX_ISP_AINR_BIAS_OUT_NUM_MAX];            /* Display: offset2d, Accuracy: U16.0, Range: [0, 65535], offset_2d*/
    AX_U16                           nOffset3d[AX_ISP_REF_AUTOTBL_AINR_HDR_RATIO_EXPRESS_NUM][AX_ISP_REF_AUTOTBL_AINR_EXPRESS_NUM][AX_ISP_AINR_BIAS_OUT_NUM_MAX];            /* Display: offset3d, Accuracy: U16.0, Range: [0, 65535], offset_3d*/
} AX_ISP_IQ_AINR_AUTO_T;

typedef struct {
    AX_U8                            nAinrEn;                             /* ainr enable, Accuracy: U1.0 Range: [0, 1], 0: Disable, 1: Enable */
    AX_U8                            nAutoMode;                           /* for ref auto or manual adjust mode, Accuracy: U1.0 Range:[0, 1], 0: manual, 1:auto, default:1 */
    AX_ISP_IQ_AINR_MANUAL_T          tManualParam;
    AX_ISP_IQ_AINR_AUTO_T            tAutoParam;
} AX_ISP_IQ_AINR_PARAM_EX_T;

/************************************************************************************
 *  Raw2dnr IQ Auto Ext Param
 ************************************************************************************/
#define AX_ISP_REF_AUTOTBL_RAW2DNR_EXPRESS_NUM                           (12)
#define AX_ISP_REF_AUTOTBL_RAW2DNR_HDR_RATIO_EXPRESS_NUM                 (4)

typedef struct {
    /* ref_mode: gain/lux && hdr_ratio */
    AX_U8                            nRefMode;                                                                                                          /* choose ref mode, Accuracy: U1.0 Range: [0, 1], 0:use lux as ref, 1:use gain as ref */
    AX_U8                            nParamGrpNum;                                                                                                      /* Accuracy: U8.0 Range: [1, 12] */
    AX_U32                           nRefVal[AX_ISP_REF_AUTOTBL_RAW2DNR_EXPRESS_NUM];                                                                   /* Gain: Accuracy: U22.10 Range: [0x400, 0xFFFFFFFF]; Lux: Accuracy: U22.10 Range: [0, 0xFFFFFFFF] */
    AX_U8                            nParamGrpNumHdrRatio;                                                                                              /* Accuracy: U8.0 Range: [1, 4] */
    AX_U32                           nRefValHdrRatio[AX_ISP_REF_AUTOTBL_RAW2DNR_HDR_RATIO_EXPRESS_NUM];                                                 /* Accuracy: U7.10 Range: [1024, 130048] */
    AX_U8                            nMaskThre[AX_ISP_REF_AUTOTBL_RAW2DNR_HDR_RATIO_EXPRESS_NUM][AX_ISP_REF_AUTOTBL_RAW2DNR_EXPRESS_NUM][AX_ISP_RAW2DNR_CORING_LIMIT_NUM];            /* Display: maskThre, Accuracy: U0.8, Range: [0, 255], mask threshold for coring*/
    AX_U8                            nMaskLimit[AX_ISP_REF_AUTOTBL_RAW2DNR_HDR_RATIO_EXPRESS_NUM][AX_ISP_REF_AUTOTBL_RAW2DNR_EXPRESS_NUM][AX_ISP_RAW2DNR_CORING_LIMIT_NUM];           /* Display: maskLimit, Accuracy: U0.8, Range: [0, 255], mask limit for coring*/
    AX_U8                            nEdgePreserveRatio[AX_ISP_REF_AUTOTBL_RAW2DNR_HDR_RATIO_EXPRESS_NUM][AX_ISP_REF_AUTOTBL_RAW2DNR_EXPRESS_NUM];      /* Display: edgePreserveRatio, Accuracy: U0.8, Range: [0, 255], edge_preserve_ratio for freq separation*/
    AX_U8                            nInterGStr[AX_ISP_REF_AUTOTBL_RAW2DNR_HDR_RATIO_EXPRESS_NUM][AX_ISP_REF_AUTOTBL_RAW2DNR_EXPRESS_NUM];              /* Display: interGStr, Accuracy: U0.8, Range: [0, 255], inter_g_strength for freq separation*/
    AX_U8                            nHfNrStren[AX_ISP_REF_AUTOTBL_RAW2DNR_HDR_RATIO_EXPRESS_NUM][AX_ISP_REF_AUTOTBL_RAW2DNR_EXPRESS_NUM][AX_ISP_RAW2DNR_SETTING_NUM];           /* Display: hfNrStren, Accuracy: U0.8, Range: [0, 255], hf noise reduction strength*/
    AX_U8                            nHfNrStrenRgb[AX_ISP_REF_AUTOTBL_RAW2DNR_HDR_RATIO_EXPRESS_NUM][AX_ISP_REF_AUTOTBL_RAW2DNR_EXPRESS_NUM][AX_ISP_RAW2DNR_SETTING_NUM][AX_ISP_RAW2DNR_RGB_CHN_NUM];     /* Display: hfNrStrenRgb, Accuracy: U0.8, Range: [0, 255], hf noise reduction strength for rgb channel*/
    AX_U16                           nWienerFactor[AX_ISP_REF_AUTOTBL_RAW2DNR_HDR_RATIO_EXPRESS_NUM][AX_ISP_REF_AUTOTBL_RAW2DNR_EXPRESS_NUM][AX_ISP_RAW2DNR_SETTING_NUM][AX_ISP_RAW2DNR_LUT_SIZE];    /* Display: wienerFactor, Accuracy: U6.4, Range: [0, 1023], hf noise reduction wiener factor for different brightness*/
    AX_U8                            nMfNrStren[AX_ISP_REF_AUTOTBL_RAW2DNR_HDR_RATIO_EXPRESS_NUM][AX_ISP_REF_AUTOTBL_RAW2DNR_EXPRESS_NUM][AX_ISP_RAW2DNR_SETTING_NUM];           /* Display: mfNrStren, Accuracy: U0.8, Range: [0, 255], mf noise reduction strength*/
    AX_U8                            nMfNrStrenRgb[AX_ISP_REF_AUTOTBL_RAW2DNR_HDR_RATIO_EXPRESS_NUM][AX_ISP_REF_AUTOTBL_RAW2DNR_EXPRESS_NUM][AX_ISP_RAW2DNR_SETTING_NUM][AX_ISP_RAW2DNR_RGB_CHN_NUM];     /* Display: mfNrStrenRgb, Accuracy: U0.8, Range: [0, 255], mf noise reduction strength for rgb channel*/
    AX_U16                           nNlmFactor[AX_ISP_REF_AUTOTBL_RAW2DNR_HDR_RATIO_EXPRESS_NUM][AX_ISP_REF_AUTOTBL_RAW2DNR_EXPRESS_NUM][AX_ISP_RAW2DNR_SETTING_NUM][AX_ISP_RAW2DNR_LUT_SIZE];       /* Display: nlmFactor, Accuracy: U4.6, Range: [0, 1023], mf noise reduction nlm factor for different brightness*/
    AX_U8                            nInterChannelStr[AX_ISP_REF_AUTOTBL_RAW2DNR_HDR_RATIO_EXPRESS_NUM][AX_ISP_REF_AUTOTBL_RAW2DNR_EXPRESS_NUM];        /* Display: interChannelStr, Accuracy: U0.8, Range: [0, 255], inter_channel_strength for mf nlm*/
    AX_U16                           nNoiseProfileFactor[AX_ISP_REF_AUTOTBL_RAW2DNR_HDR_RATIO_EXPRESS_NUM][AX_ISP_REF_AUTOTBL_RAW2DNR_EXPRESS_NUM];     /* Display: noiseProfileFactor, Accuracy: U3.7, Range: [0, 1023], noise_profile_factor for noise_profile*/
} AX_ISP_IQ_RAW2DNR_AUTO_T;

typedef struct {
    AX_U8                            nRaw2dnrEn;                          /* raw2dnr enable, Accuracy: U1.0 Range: [0, 1], 0: Disable, 1: Enable */
    AX_U8                            nAutoMode;                           /* for ref auto or manual adjust mode, Accuracy: U1.0 Range:[0, 1], 0: manual, 1:auto, default:1 */
    AX_ISP_IQ_RAW2DNR_MANUAL_T       tManualParam;
    AX_ISP_IQ_RAW2DNR_AUTO_T         tAutoParam;
} AX_ISP_IQ_RAW2DNR_PARAM_EX_T;

/***********************************************************************************
 *  LSC IQ Auto Ext Param
 ***********************************************************************************/
#define AX_ISP_REF_AUTOTBL_LSC_EXPRESS_NUM                               (4)
#define AX_ISP_REF_AUTOTBL_LSC_COLOR_TEMP_EXPRESS_NUM                    (4)

typedef struct {
    /* ref_mode: gain/lux */
    AX_U8                            nRefMode;                                                    /* choose ref mode, Accuracy: U1.0 Range: [0, 1], 0:use lux as ref, 1:use gain as ref */
    AX_U8                            nParamGrpNum;                                                /* Accuracy: U8.0 Range: [1, 12] */
    AX_U32                           nRefVal[AX_ISP_REF_AUTOTBL_LSC_EXPRESS_NUM];                 /* Gain: Accuracy: U22.10 Range: [0x400, 0xFFFFFFFF]; Lux: Accuracy: U22.10 Range: [0, 0xFFFFFFFF] */
    AX_U8                            nLumaRatio[AX_ISP_REF_AUTOTBL_LSC_EXPRESS_NUM];              /* Display: lumaRatio, Accuracy: U8.0, Range: [0, 100], luma shading attenuation ratio*/
    AX_U32                           nLumaMeshLut[AX_ISP_LSC_MESH_SIZE_V][AX_ISP_LSC_MESH_SIZE_H];/* Accuacy U4.14 Range: [0x4000, 0x3FFFF] */
    /* ref_mode: color_temp */
    AX_U8                            nParamGrpNumCt;                                                         /* Accuracy: U8.0 Range: [1, 8] */
    AX_U32                           nRefValCt[AX_ISP_REF_AUTOTBL_LSC_COLOR_TEMP_EXPRESS_NUM];               /* Accuracy: U14.0 Range: [0, 16000] */
    AX_U8                            nColorRatio[AX_ISP_REF_AUTOTBL_LSC_COLOR_TEMP_EXPRESS_NUM];             /* Display: colorRatio, Accuracy: U8.0, Range: [0, 100], color shading attenuation ratio*/
    AX_U32                           nRRMeshLut[AX_ISP_REF_AUTOTBL_LSC_COLOR_TEMP_EXPRESS_NUM][AX_ISP_LSC_MESH_SIZE_V][AX_ISP_LSC_MESH_SIZE_H];      /* Display: RRMeshLut, Accuracy: U4.14, Range: [16384, 262143], color shading R channel correction mesh lut*/
    AX_U32                           nGRMeshLut[AX_ISP_REF_AUTOTBL_LSC_COLOR_TEMP_EXPRESS_NUM][AX_ISP_LSC_MESH_SIZE_V][AX_ISP_LSC_MESH_SIZE_H];      /* Display: GRMeshLut, Accuracy: U4.14, Range: [16384, 262143], color shading GR channel correction mesh lut*/
    AX_U32                           nGBMeshLut[AX_ISP_REF_AUTOTBL_LSC_COLOR_TEMP_EXPRESS_NUM][AX_ISP_LSC_MESH_SIZE_V][AX_ISP_LSC_MESH_SIZE_H];      /* Display: GBMeshLut, Accuracy: U4.14, Range: [16384, 262143], color shading GB channel correction mesh lut*/
    AX_U32                           nBBMeshLut[AX_ISP_REF_AUTOTBL_LSC_COLOR_TEMP_EXPRESS_NUM][AX_ISP_LSC_MESH_SIZE_V][AX_ISP_LSC_MESH_SIZE_H];      /* Display: BBMeshLut, Accuracy: U4.14, Range: [16384, 262143], color shading B channel correction mesh lut*/
} AX_ISP_IQ_LSC_AUTO_T;

typedef struct {
    AX_U8                            nLscEn;                              /* lsc enable, Accuracy: U1.0 Range: [0, 1], 0: Disable, 1: Enable */
    AX_U8                            nAutoMode;                           /* for ref auto or manual adjust mode, Accuracy: U1.0 Range:[0, 1], 0: manual, 1:auto, default:1 */
    AX_ISP_IQ_LSC_MANUAL_T           tManualParam;
    AX_ISP_IQ_LSC_AUTO_T             tAutoParam;
} AX_ISP_IQ_LSC_PARAM_EX_T;

/************************************************************************************
 *  WBC IQ Auto Ext Param
 ************************************************************************************/

typedef struct {
    AX_U8                            nWbcEn;                              /* wbc enable, Accuracy: U1.0 Range: [0, 1], 0: Disable, 1: Enable */
    AX_ISP_IQ_WBC_MANUAL_T           tManualParam;
} AX_ISP_IQ_WBC_PARAM_EX_T;

/************************************************************************************
 *  RLTM IQ Auto Ext Param
 ************************************************************************************/
#define AX_ISP_REF_AUTOTBL_RLTM_EXPRESS_NUM                              (12)
#define AX_ISP_REF_AUTOTBL_RLTM_HDR_RATIO_EXPRESS_NUM                    (4)

typedef struct {
    /* ref_mode: gain/lux */
    AX_U8                            nRefMode;                                                     /* choose ref mode, Accuracy: U1.0 Range: [0, 1], 0:use lux as ref, 1:use gain as ref */
    AX_U8                            nParamGrpNum;                                                 /* Accuracy: U8.0 Range: [1, 12] */
    AX_U32                           nRefVal[AX_ISP_REF_AUTOTBL_RLTM_EXPRESS_NUM];                 /* Gain: Accuracy: U22.10 Range: [0x400, 0xFFFFFFFF]; Lux: Accuracy: U22.10 Range: [0, 0xFFFFFFFF] */
    AX_U8                            nDarkContrastEnhc[AX_ISP_REF_AUTOTBL_RLTM_EXPRESS_NUM][AX_ISP_RLTM_LUMA_WEIGHT_NUM];    /* Display: darkContrastEnhc, Accuracy: U0.8, Range: [0, 255], Max Luma Value & Pixel Num for Contrast Enhance*/
    AX_U16                           nKMax[AX_ISP_REF_AUTOTBL_RLTM_EXPRESS_NUM];                   /* Display: kMax, Accuracy: U12.4, Range: [16, 65535], max gain for rltm*/
    AX_U16                           nBaseCurve[AX_ISP_REF_AUTOTBL_RLTM_EXPRESS_NUM][AX_ISP_RLTM_BASECURVE_MAX_LEN];         /* Display: baseCurve, Accuracy: U1.15, Range: [0, 32768], base-curve*/
    AX_U16                           nSCurveList[AX_ISP_REF_AUTOTBL_RLTM_EXPRESS_NUM][AX_ISP_RLTM_SCURVE_MAX_LEN];       /* Display: sCurve, Accuracy: U1.15, Range: [0, 32768], s-curve*/
    AX_U16                           nDetailGainLimitPos[AX_ISP_REF_AUTOTBL_RLTM_EXPRESS_NUM];     /* Display: detailLimitPos, Accuracy: U3.13, Range: [0, 65535], the limit for high freq pos detail*/
    AX_U16                           nDetailGainLimitNeg[AX_ISP_REF_AUTOTBL_RLTM_EXPRESS_NUM];     /* Display: detailLimitNeg, Accuracy: U3.13, Range: [0, 65535], the limit for high freq neg detail*/
    AX_U8                            nDetailExtraStrengthPos[AX_ISP_REF_AUTOTBL_RLTM_EXPRESS_NUM]; /* Display: detailExtraStrengthPos, Accuracy: U4.4, Range: [0, 255], the global strength for high freq pos detail*/
    AX_U8                            nDetailExtraStrengthNeg[AX_ISP_REF_AUTOTBL_RLTM_EXPRESS_NUM]; /* Display: detailExtraStrengthNeg, Accuracy: U4.4, Range: [0, 255], the global strength for high freq neg detail*/
    AX_U8                            nDetailStrengthLut[AX_ISP_REF_AUTOTBL_RLTM_EXPRESS_NUM][AX_ISP_RLTM_DETAIL_STRENGTH_LUT_NUM];  /* Display: detailStrengthLut, Accuracy: U4.4, Range: [0, 255], the strength for high freq detail by detail*/
    AX_U8                            nDetailGainPosLow[AX_ISP_REF_AUTOTBL_RLTM_EXPRESS_NUM];       /* Display: detailGainPosLow, Accuracy: U3.5, Range: [0, 255], the gain for low freq pos detail*/
    AX_U8                            nDetailGainNegLow[AX_ISP_REF_AUTOTBL_RLTM_EXPRESS_NUM];       /* Display: detailGainNegLow, Accuracy: U3.5, Range: [0, 255], the gain for low freq neg detail*/
    AX_U16                           nDetailLimitPosLow[AX_ISP_REF_AUTOTBL_RLTM_EXPRESS_NUM];      /* Display: detailLimitPosLow, Accuracy: U3.13, Range: [0, 65535], the limit for low freq pos detail*/
    AX_U16                           nDetailLimitNegLow[AX_ISP_REF_AUTOTBL_RLTM_EXPRESS_NUM];      /* Display: detailLimitNegLow, Accuracy: U3.13, Range: [0, 65535], the limit for low freq neg detail*/
    AX_U8                            nDetailStrengthLutPosLow[AX_ISP_REF_AUTOTBL_RLTM_EXPRESS_NUM][AX_ISP_RLTM_DETAIL_STRENGTH_LUT_LOW_NUM]; /* Display: detailStrengthLutPosLow, Accuracy: U1.7, Range: [0, 255], the strength for low freq pos detail by luma*/
    AX_U8                            nDetailStrengthLutNegLow[AX_ISP_REF_AUTOTBL_RLTM_EXPRESS_NUM][AX_ISP_RLTM_DETAIL_STRENGTH_LUT_LOW_NUM]; /* Display: detailStrengthLutNegLow, Accuracy: U1.7, Range: [0, 255], the strength for low freq neg detail by luma*/
    AX_U16                           nSigmaDisPst[AX_ISP_REF_AUTOTBL_RLTM_EXPRESS_NUM];            /* Display: sigmaDisPst, Accuracy: U3.13, Range: [1, 65535], spatial domain weight low freq*/
    AX_U16                           nSigmaValPst[AX_ISP_REF_AUTOTBL_RLTM_EXPRESS_NUM];            /* Display: sigmaValPst, Accuracy: U3.13, Range: [1, 2048], intensity domain weight low freq*/
    /* ref_mode: gain/lux && hdr_ratio */
    AX_U8                            nParamGrpNumHdrRatio;                                                                                        /* Accuracy: U8.0 Range: [1, 4] */
    AX_U32                           nRefValHdrRatio[AX_ISP_REF_AUTOTBL_RLTM_HDR_RATIO_EXPRESS_NUM];                                              /* Accuracy: U7.10 Range: [1024, 130048] */
    AX_U8                            nHighlightSup[AX_ISP_REF_AUTOTBL_RLTM_HDR_RATIO_EXPRESS_NUM][AX_ISP_REF_AUTOTBL_RLTM_EXPRESS_NUM];           /* Display: highlightSup, Accuracy: U5.3, Range: [0, 255], highlight suppression*/
    AX_U8                            nPostGamma[AX_ISP_REF_AUTOTBL_RLTM_HDR_RATIO_EXPRESS_NUM][AX_ISP_REF_AUTOTBL_RLTM_EXPRESS_NUM];              /* Display: pstGamma, Accuracy: U3.5, Range: [32, 255], pst gamma*/
    AX_U8                            nLocalFactor[AX_ISP_REF_AUTOTBL_RLTM_HDR_RATIO_EXPRESS_NUM][AX_ISP_REF_AUTOTBL_RLTM_EXPRESS_NUM];            /* Display: localFactor, Accuracy: U1.7, Range: [0, 128], factor for local hist and global hist*/
    AX_U8                            nRltmStrength[AX_ISP_REF_AUTOTBL_RLTM_HDR_RATIO_EXPRESS_NUM][AX_ISP_REF_AUTOTBL_RLTM_EXPRESS_NUM];           /* Display: rltmStrength, Accuracy: U1.7, Range: [0, 128], rltm strength*/
    AX_U8                            nContrastStrength[AX_ISP_REF_AUTOTBL_RLTM_HDR_RATIO_EXPRESS_NUM][AX_ISP_REF_AUTOTBL_RLTM_EXPRESS_NUM];       /* Display: contrastStrength, Accuracy: U1.7, Range: [1, 255], contrast strength*/
    AX_U8                            nBaseCurveStrength[AX_ISP_REF_AUTOTBL_RLTM_HDR_RATIO_EXPRESS_NUM][AX_ISP_REF_AUTOTBL_RLTM_EXPRESS_NUM];      /* Display: baseCurveStrength, Accuracy: U1.7, Range: [0, 128], rltm base curve strength*/
    AX_U16                           nDetailCoringPos[AX_ISP_REF_AUTOTBL_RLTM_HDR_RATIO_EXPRESS_NUM][AX_ISP_REF_AUTOTBL_RLTM_EXPRESS_NUM];        /* Display: detailCoringPos, Accuracy: U3.13, Range: [0, 65535], the coring for high freq pos detail*/
    AX_U16                           nDetailCoringNeg[AX_ISP_REF_AUTOTBL_RLTM_HDR_RATIO_EXPRESS_NUM][AX_ISP_REF_AUTOTBL_RLTM_EXPRESS_NUM];        /* Display: detailCoringNeg, Accuracy: U3.13, Range: [0, 65535], the coring for high freq neg detail*/
    AX_U8                            nDetailGainPos[AX_ISP_REF_AUTOTBL_RLTM_HDR_RATIO_EXPRESS_NUM][AX_ISP_REF_AUTOTBL_RLTM_EXPRESS_NUM];          /* Display: detailGainPos, Accuracy: U3.5, Range: [0, 255], the gain for high freq pos detail*/
    AX_U8                            nDetailGainNeg[AX_ISP_REF_AUTOTBL_RLTM_HDR_RATIO_EXPRESS_NUM][AX_ISP_REF_AUTOTBL_RLTM_EXPRESS_NUM];          /* Display: detailGainNeg, Accuracy: U3.5, Range: [0, 255], the gain for high freq neg detail*/
    AX_U16                           nDetailSigmaDis[AX_ISP_REF_AUTOTBL_RLTM_HDR_RATIO_EXPRESS_NUM][AX_ISP_REF_AUTOTBL_RLTM_EXPRESS_NUM];         /* Display: detailSigmaDis, Accuracy: U3.13, Range: [1, 65535], bilateral filter sigma distance*/
    AX_U16                           nDetailSigmaVal[AX_ISP_REF_AUTOTBL_RLTM_HDR_RATIO_EXPRESS_NUM][AX_ISP_REF_AUTOTBL_RLTM_EXPRESS_NUM];         /* Display: detailSigmaVal, Accuracy: U3.13, Range: [1, 65535], bilateral filter sigma value*/
} AX_ISP_IQ_RLTM_AUTO_T;

typedef struct {
    AX_U8                            nRltmEn;                             /* rltm enable, Accuracy: U1.0 Range: [0, 1], 0: Disable, 1: Enable */
    AX_U8                            nAutoMode;                           /* for ref auto or manual adjust mode, Accuracy: U1.0 Range:[0, 1], 0: manual, 1:auto, default:1 */
    AX_ISP_IQ_RLTM_MANUAL_T          tManualParam;
    AX_ISP_IQ_RLTM_AUTO_T            tAutoParam;
} AX_ISP_IQ_RLTM_PARAM_EX_T;

/************************************************************************************
 *  DEMOSAIC IQ Auto Ext Param
 ************************************************************************************/

#define AX_ISP_REF_AUTOTBL_DEMOSAIC_EXPRESS_NUM                               (16)

typedef struct {
    /* ref_mode: gain/lux */
    AX_U8                            nRefMode;                                                         /* choose ref mode, Accuracy: U1.0 Range: [0, 1], 0:use lux as ref, 1:use gain as ref */
    AX_U8                            nParamGrpNum;                                                     /* Accuracy: U8.0 Range: [1, 16] */
    AX_U32                           nRefVal[AX_ISP_REF_AUTOTBL_DEMOSAIC_EXPRESS_NUM];                 /* Gain: Accuracy: U22.10 Range: [0x400, 0xFFFFFFFF]; Lux: Accuracy: U22.10 Range: [0, 0xFFFFFFFF] */
    AX_U8                            nEdgeDirHighFreqSens[AX_ISP_REF_AUTOTBL_DEMOSAIC_EXPRESS_NUM];    /* Display: edgeDirHighFreqSens, Accuracy: U1.4, Range: [0, 16], bigger value==> more sensitivity to high freq*/
    AX_U16                           nEdgeDirEdgeSensLut[AX_ISP_REF_AUTOTBL_DEMOSAIC_EXPRESS_NUM][AX_ISP_DEMOSAIC_EDES_LUT_SIZE];              /* Display: edgeDirEdgeSensLut, Accuracy: U4.8, Range: [0, 4095], default inv noise lut. lut[0]:0, [1]:32, [2]:64, ... [8]:256*/
    AX_U8                            nEdgeDirHvHighFreqStr[AX_ISP_REF_AUTOTBL_DEMOSAIC_EXPRESS_NUM];   /* Display: edgeDirHvHighFreqStr, Accuracy: U2.2, Range: [0, 15], level for adjusting high frequency gradient*/
    AX_U8                            nHvInterpHfCoeff[AX_ISP_REF_AUTOTBL_DEMOSAIC_EXPRESS_NUM];        /* Display: hvInterpHfCoeff, Accuracy: U1.5, Range: [0, 32], level for adjusting high frequency gradient*/
    AX_U8                            nPnInterpHfCoeff[AX_ISP_REF_AUTOTBL_DEMOSAIC_EXPRESS_NUM];        /* Display: pnInterpHfCoeff, Accuracy: U1.5, Range: [0, 32], level for adjusting high frequency gradient*/
    AX_U8                            nXInterpHfCoeff[AX_ISP_REF_AUTOTBL_DEMOSAIC_EXPRESS_NUM];         /* Display: xInterpHfCoeff, Accuracy: U1.5, Range: [0, 32], level for adjusting high frequency gradient*/
} AX_ISP_IQ_DEMOSAIC_AUTO_T;

typedef struct {
    AX_U8                            nDemEn;                              /* demosaic enable, Accuracy: U1.0 Range: [0, 1], 0: Disable, 1: Enable */
    AX_U8                            nAutoMode;                           /* for ref auto or manual adjust mode, Accuracy: U1.0 Range:[0, 1], 0: manual, 1:auto, default:1 */
    AX_ISP_IQ_DEMOSAIC_MANUAL_T      tManualParam;
    AX_ISP_IQ_DEMOSAIC_AUTO_T        tAutoParam;
} AX_ISP_IQ_DEMOSAIC_PARAM_EX_T;

/************************************************************************************
 *  FCC IQ Auto Ext Param
 ************************************************************************************/

#define AX_ISP_REF_AUTOTBL_FCC_EXPRESS_NUM                               (16)

typedef struct {
    /* ref_mode: gain/lux */
    AX_U8                            nRefMode;                                                    /* choose ref mode, Accuracy: U1.0 Range: [0, 1], 0:use lux as ref, 1:use gain as ref */
    AX_U8                            nParamGrpNum;                                                /* Accuracy: U8.0 Range: [1, 16] */
    AX_U32                           nRefVal[AX_ISP_REF_AUTOTBL_FCC_EXPRESS_NUM];                 /* Gain: Accuracy: U22.10 Range: [0x400, 0xFFFFFFFF]; Lux: Accuracy: U22.10 Range: [0, 0xFFFFFFFF] */
    AX_U8                            nFccSens[AX_ISP_REF_AUTOTBL_FCC_EXPRESS_NUM];                /* Display: fccSens, Accuracy: U2.2, Range: [0, 15], larger value means higher fcc sensitivity, makes the corrected color more grayish*/
    AX_U8                            nFccStr[AX_ISP_REF_AUTOTBL_FCC_EXPRESS_NUM];                 /* Display: fccStr, Accuracy: U4.4, Range: [0, 255], larger value means stronger fcc strength, makes the corrected color more grayish*/
} AX_ISP_IQ_FCC_AUTO_T;

typedef struct {
    AX_U8                            nFccEn;                              /* fcc enable, Accuracy: U1.0 Range: [0, 1], 0: Disable, 1: Enable */
    AX_U8                            nAutoMode;                           /* for ref auto or manual adjust mode, Accuracy: U1.0 Range:[0, 1], 0: manual, 1:auto, default:1 */
    AX_ISP_IQ_FCC_MANUAL_T           tManualParam;
    AX_ISP_IQ_FCC_AUTO_T             tAutoParam;
} AX_ISP_IQ_FCC_PARAM_EX_T;

/************************************************************************************
 *  GIC IQ Auto Ext Param
 ************************************************************************************/

#define AX_ISP_REF_AUTOTBL_GIC_EXPRESS_NUM                               (16)

typedef struct {
    /* ref_mode: gain/lux */
    AX_U8                            nRefMode;                                                    /* choose ref mode, Accuracy: U1.0 Range: [0, 1], 0:use lux as ref, 1:use gain as ref */
    AX_U8                            nParamGrpNum;                                                /* Accuracy: U8.0 Range: [1, 16] */
    AX_U32                           nRefVal[AX_ISP_REF_AUTOTBL_GIC_EXPRESS_NUM];                 /* Gain: Accuracy: U22.10 Range: [0x400, 0xFFFFFFFF]; Lux: Accuracy: U22.10 Range: [0, 0xFFFFFFFF] */
    AX_U8                            nGicStrFactor[AX_ISP_REF_AUTOTBL_GIC_EXPRESS_NUM];           /* Display: gicStrFactor, Accuracy: U2.6, Range: [0, 255], */
} AX_ISP_IQ_GIC_AUTO_T;

typedef struct {
    AX_U8                            nGicEn;                              /* gic enable, Accuracy: U1.0 Range: [0, 1], 0: Disable, 1: Enable */
    AX_U8                            nAutoMode;                           /* for ref auto or manual adjust mode, Accuracy: U1.0 Range:[0, 1], 0: manual, 1:auto, default:1 */
    AX_ISP_IQ_GIC_MANUAL_T           tManualParam;
    AX_ISP_IQ_GIC_AUTO_T             tAutoParam;
} AX_ISP_IQ_GIC_PARAM_EX_T;

/************************************************************************************
 *  EDGE_ENHANCE IQ Auto Ext Param
 ************************************************************************************/

#define AX_ISP_REF_AUTOTBL_EDGE_ENHANCE_EXPRESS_NUM                      (16)

typedef struct {
    /* ref_mode: gain/lux */
    AX_U8                            nRefMode;                                                             /* choose ref mode, Accuracy: U1.0 Range: [0, 1], 0:use lux as ref, 1:use gain as ref */
    AX_U8                            nParamGrpNum;                                                         /* Accuracy: U8.0 Range: [1, 16] */
    AX_U32                           nRefVal[AX_ISP_REF_AUTOTBL_EDGE_ENHANCE_EXPRESS_NUM];                 /* Gain: Accuracy: U22.10 Range: [0x400, 0xFFFFFFFF]; Lux: Accuracy: U22.10 Range: [0, 0xFFFFFFFF] */
    AX_U8                            nEdgeSmoothStr[AX_ISP_REF_AUTOTBL_EDGE_ENHANCE_EXPRESS_NUM][AX_ISP_EDGE_ENHANCE_LUT_SIZE];       /* Display: edgeSmoothStr, Accuracy: U1.4, Range: [0, 16], larger values means more smooth. lut[0]:0, [1]:32, [2]:64, ... [8]:256*/
    AX_U8                            nEdgeSharpStr[AX_ISP_REF_AUTOTBL_EDGE_ENHANCE_EXPRESS_NUM][AX_ISP_EDGE_ENHANCE_LUT_SIZE];        /* Display: edgeSharpStr, Accuracy: U1.4, Range: [0, 16], larger values means more sharpen. lut[0]:0, [1]:32, [2]:64, ... [8]:256*/
} AX_ISP_IQ_EDGE_ENHANCE_AUTO_T;

typedef struct {
    AX_U8                            nEdgeEnhanceEn;                      /* edge_enhance enable, Accuracy: U1.0 Range: [0, 1], 0: Disable, 1: Enable */
    AX_U8                            nAutoMode;                           /* for ref auto or manual adjust mode, Accuracy: U1.0 Range:[0, 1], 0: manual, 1:auto, default:1 */
    AX_ISP_IQ_EDGE_ENHANCE_MANUAL_T  tManualParam;
    AX_ISP_IQ_EDGE_ENHANCE_AUTO_T    tAutoParam;
} AX_ISP_IQ_EDGE_ENHANCE_PARAM_EX_T;

/************************************************************************************
 *  CC IQ Auto Ext Param
 ************************************************************************************/

#define AX_ISP_REF_AUTOTBL_CC_EXPRESS_NUM                                (5)
#define AX_ISP_REF_AUTOTBL_CC_COLOR_TEMP_EXPRESS_NUM                     (12)

typedef struct {
    /* ref_mode: gain/lux && color_temp */
    AX_U8                            nRefMode;                                                                                                 /* choose ref mode, Accuracy: U1.0 Range: [0, 1], 0:use lux as ref, 1:use gain as ref */
    AX_U8                            nParamGrpNum;                                                                                             /* Accuracy: U8.0 Range: [1, 5] */
    AX_U32                           nRefVal[AX_ISP_REF_AUTOTBL_CC_EXPRESS_NUM];                                                               /* Gain: Accuracy: U22.10 Range: [0x400, 0xFFFFFFFF]; Lux: Accuracy: U22.10 Range: [0, 0xFFFFFFFF] */
    AX_U8                            nParamGrpNumCt;                                                                                           /* Accuracy: U8.0 Range: [1, 12] */
    AX_U32                           nRefValCt[AX_ISP_REF_AUTOTBL_CC_COLOR_TEMP_EXPRESS_NUM];                                                  /* Accuracy: U14.0 Range: [0, 16000] */
    AX_S16                           nCcm[AX_ISP_REF_AUTOTBL_CC_COLOR_TEMP_EXPRESS_NUM][AX_ISP_REF_AUTOTBL_CC_EXPRESS_NUM][AX_ISP_CC_CCM_V_SIZE][AX_ISP_CC_CCM_H_SIZE];              /* Display: ccm, Accuracy: S3.8, Range: [-2047, 2047], clc ccm matrix*/
    AX_S8                            nCcmSat[AX_ISP_REF_AUTOTBL_CC_COLOR_TEMP_EXPRESS_NUM][AX_ISP_REF_AUTOTBL_CC_EXPRESS_NUM];                 /* Display: ccmSat, Accuracy: S1.6, Range: [-64, 64], clc global saturation adjust*/
    AX_S16                           nCcmHue[AX_ISP_REF_AUTOTBL_CC_COLOR_TEMP_EXPRESS_NUM][AX_ISP_REF_AUTOTBL_CC_EXPRESS_NUM];                 /* Display: ccmHue, Accuracy: S5.6, Range: [-1920, 1920], clc global hue adjust*/
    AX_U16                           nCcmCtrlLevel[AX_ISP_REF_AUTOTBL_CC_COLOR_TEMP_EXPRESS_NUM][AX_ISP_REF_AUTOTBL_CC_EXPRESS_NUM];           /* Display: ccmCtrlLevel, Accuracy: U1.8, Range: [0, 256], clc global adjust ctrl level*/
    AX_S8                            nXcmSats[AX_ISP_REF_AUTOTBL_CC_COLOR_TEMP_EXPRESS_NUM][AX_ISP_REF_AUTOTBL_CC_EXPRESS_NUM][AX_ISP_CC_ANGLE_SIZE];            /* Display: xcmSats, Accuracy: S1.6, Range: [-32, 32], clc local saturation adjust*/
    AX_S16                           nXcmHues[AX_ISP_REF_AUTOTBL_CC_COLOR_TEMP_EXPRESS_NUM][AX_ISP_REF_AUTOTBL_CC_EXPRESS_NUM][AX_ISP_CC_ANGLE_SIZE];            /* Display: xcmHues, Accuracy: S5.6, Range: [-640, 640], clc local hue adjust*/
    AX_U16                           nXcmCtrlLevels[AX_ISP_REF_AUTOTBL_CC_COLOR_TEMP_EXPRESS_NUM][AX_ISP_REF_AUTOTBL_CC_EXPRESS_NUM][AX_ISP_CC_ANGLE_SIZE];      /* Display: xcmCtrlLevels, Accuracy: U1.8, Range: [0, 256], clc local adjust ctrl level*/
} AX_ISP_IQ_CC_AUTO_T;

typedef struct {
    AX_U8                            nCcEn;                               /* cc enable, Accuracy: U1.0 Range: [0, 1], 0: Disable, 1: Enable */
    AX_U8                            nAutoMode;                           /* for ref auto or manual adjust mode, Accuracy: U1.0 Range:[0, 1], 0: manual, 1:auto, default:1 */
    AX_ISP_IQ_CC_MANUAL_T            tManualParam;
    AX_ISP_IQ_CC_AUTO_T              tAutoParam;
} AX_ISP_IQ_CC_PARAM_EX_T;

/************************************************************************************
 *  Gamma IQ Auto Ext Param
 ************************************************************************************/

#define AX_ISP_REF_AUTOTBL_GAMMA_EXPRESS_NUM                             (4)

typedef struct {
    /* ref_mode: gain/lux */
    AX_U8                            nRefMode;                                                      /* choose ref mode, Accuracy: U1.0 Range: [0, 1], 0:use lux as ref, 1:use gain as ref */
    AX_U8                            nParamGrpNum;                                                  /* Accuracy: U8.0 Range: [1, 16] */
    AX_U32                           nRefVal[AX_ISP_REF_AUTOTBL_GAMMA_EXPRESS_NUM];                 /* Gain: Accuracy: U22.10 Range: [0x400, 0xFFFFFFFF]; Lux: Accuracy: U22.10 Range: [0, 0xFFFFFFFF] */
    AX_U16                           nCustomLut[AX_ISP_REF_AUTOTBL_GAMMA_EXPRESS_NUM][AX_ISP_GAMMA_CUSTOM_LUT_SIZE];         /* Display: customLut, Accuracy: U8.4, Range: [0, 4095], gamma curve. default is AX_ISP_GAM_AX_GAM0*/
} AX_ISP_IQ_GAMMA_AUTO_T;

typedef struct {
    AX_U8                            nGammaEn;                            /* gamma enable, Accuracy: U1.0 Range: [0, 1], 0: Disable, 1: Enable */
    AX_U8                            nAutoMode;                           /* for ref auto or manual adjust mode, Accuracy: U1.0 Range:[0, 1], 0: manual, 1:auto, default:1 */
    AX_ISP_IQ_GAMMA_MANUAL_T         tManualParam;
    AX_ISP_IQ_GAMMA_AUTO_T           tAutoParam;
} AX_ISP_IQ_GAMMA_PARAM_EX_T;

/************************************************************************************
 *  Dehaze IQ Auto Ext Param
 ************************************************************************************/

#define AX_ISP_REF_AUTOTBL_DEHAZE_EXPRESS_NUM                            (12)

typedef struct {
    /* ref_mode: gain/lux */
    AX_U8                            nRefMode;                                                       /* choose ref mode, Accuracy: U1.0 Range: [0, 1], 0:use lux as ref, 1:use gain as ref */
    AX_U8                            nParamGrpNum;                                                   /* Accuracy: U8.0 Range: [1, 12] */
    AX_U32                           nRefVal[AX_ISP_REF_AUTOTBL_DEHAZE_EXPRESS_NUM];                 /* Gain: Accuracy: U22.10 Range: [0x400, 0xFFFFFFFF]; Lux: Accuracy: U22.10 Range: [0, 0xFFFFFFFF] */
    AX_U16                           nEffectStrength[AX_ISP_REF_AUTOTBL_DEHAZE_EXPRESS_NUM];         /* Display: effectStr, Accuracy: U1.15, Range: [0, 32768], adjust defogging intensity*/
} AX_ISP_IQ_DEHAZE_AUTO_T;

typedef struct {
    AX_U8                            nDehazeEn;                           /* dehaze enable, Accuracy: U1.0 Range: [0, 1], 0: Disable, 1: Enable */
    AX_U8                            nAutoMode;                           /* for ref auto or manual adjust mode, Accuracy: U1.0 Range:[0, 1], 0: manual, 1:auto, default:1 */
    AX_ISP_IQ_DEHAZE_MANUAL_T        tManualParam;
    AX_ISP_IQ_DEHAZE_AUTO_T          tAutoParam;
} AX_ISP_IQ_DEHAZE_PARAM_EX_T;

/************************************************************************************
 *  CSC IQ Auto Ext Param
 ************************************************************************************/

typedef struct {
    AX_ISP_IQ_CSC_MANUAL_T           tManualParam;
} AX_ISP_IQ_CSC_PARAM_EX_T;

/************************************************************************************
 *  YUV3DNR IQ Auto Ext Param
 ************************************************************************************/
#define AX_ISP_REF_AUTOTBL_YUV3DNR_EXPRESS_NUM                           (12)

typedef struct {
    /* ref_mode: gain/lux */
    AX_U8                            nRefMode;                                                        /* choose ref mode, Accuracy: U1.0 Range: [0, 1], 0:use lux as ref, 1:use gain as ref */
    AX_U8                            nParamGrpNum;                                                    /* Accuracy: U8.0 Range: [1, 12] */
    AX_U32                           nRefVal[AX_ISP_REF_AUTOTBL_YUV3DNR_EXPRESS_NUM];                 /* Gain: Accuracy: U22.10 Range: [0x400, 0xFFFFFFFF]; Lux: Accuracy: U22.10 Range: [0, 0xFFFFFFFF] */
    AX_U8                            nExtMaskThr[AX_ISP_REF_AUTOTBL_YUV3DNR_EXPRESS_NUM];             /* Display: extMask.thr, Accuracy: U1.7, Range: [0, 128], The starting point for remapping ext mask, below which the probability will be mapped to 0*/
    AX_U16                           nExtMaskGain[AX_ISP_REF_AUTOTBL_YUV3DNR_EXPRESS_NUM];            /* Display: extMask.gain, Accuracy: U8.8, Range: [0, 65535], The slope of ext mask remapping, the larger the value, the easier it is to map the mask to 1.0*/
    AX_U8                            nExtMaskRatio[AX_ISP_REF_AUTOTBL_YUV3DNR_EXPRESS_NUM];           /* Display: extMask.ratio, Accuracy: U1.7, Range: [0, 128], Only effective in AI mode. The reference level of AI mask. The larger the size, the more likely it is to use an AI mask as the motion mask*/
    AX_U16                           nMotSmallSel[AX_ISP_REF_AUTOTBL_YUV3DNR_EXPRESS_NUM];            /* Display: detMotion.smallSel, Accuracy: U9.0, Range: [0, 256], Applied to the feature of motion judgement, the larger the value, the larger the detection window used for motion judgement, which improves the noise resistance and smoothness of motion detection while losing the ability to detect texture details.*/
    AX_U16                           nMotLargeSel[AX_ISP_REF_AUTOTBL_YUV3DNR_EXPRESS_NUM];            /* Display: detMotion.largeSel, Accuracy: U9.0, Range: [0, 256], The greater the values of features applied to time-domain fusion coefficient filtering, the larger the detection window used for motion judgement. This improves the noise resistance and smoothness of motion detection while losing texture detail detection ability.*/
    AX_U8                            nMotSmoothStr[AX_ISP_REF_AUTOTBL_YUV3DNR_EXPRESS_NUM];           /* Display: detMotion.smoothStr, Accuracy: U1.7, Range: [0, 128], The pre smoothing degree of the motion judgement features, the larger the value, the smoother the motion judgement features.*/
    AX_U8                            nMotSensThr[AX_ISP_REF_AUTOTBL_YUV3DNR_EXPRESS_NUM];             /* Display: detMotion.sensThr, Accuracy: U8.0, Range: [0, 255], Representing the difference between two types of motion judgement features, used to map the fusion ratio of texture sensitive motion judgement features. The larger the value, the less texture sensitive motion judgement features are used, and the probability of misjudgment caused by noise decreases. However, it may lead to some areas being misjudged as stationary.*/
    AX_U16                           nMotSensGain[AX_ISP_REF_AUTOTBL_YUV3DNR_EXPRESS_NUM];            /* Display: detMotion.sensGain, Accuracy: U1.8, Range: [0, 511], Based on the fusion mapping of texture sensitive motion judgement features. The larger the value, the faster the texture sensitive motion judgement features are used, and the probability of misjudgment caused by noise increases, resulting in more accurate motion judgement.*/
    AX_U8                            nMotSensRatio[AX_ISP_REF_AUTOTBL_YUV3DNR_EXPRESS_NUM][YUV3DNR_REGS_GROUP_NUM];        /* Display: detMotion.sensRatio, Accuracy: U1.7, Range: [0, 128], The upper and lower limits of the fusion ratio of texture sensitive motion judgement features. The larger the value, the more texture sensitive motion judgement features are used, and the probability of misjudgment caused by noise increases, resulting in more accurate motion judgement*/
    AX_U16                           nMotShpDiff[AX_ISP_REF_AUTOTBL_YUV3DNR_EXPRESS_NUM];             /* Display: detMotion.shpDiff, Accuracy: U8.2, Range: [0, 1023], For highlight areas, the frame difference judgment threshold for motion sharpening is used. If the frame difference is higher than this threshold, sharpening will occur, and the larger the value, the less effective motion sharpening will be.*/
    AX_U8                            nMotShpLumaThr[AX_ISP_REF_AUTOTBL_YUV3DNR_EXPRESS_NUM];          /* Display: detMotion.shpLumaThr, Accuracy: U8.0, Range: [0, 255], For highlight areas, the brightness judgment threshold for motion sharpening is used. Only when the brightness is higher than this threshold will it sharpen. The larger the value, the less effective the motion sharpening will be.*/
    AX_U16                           nMotShpLumaGain[AX_ISP_REF_AUTOTBL_YUV3DNR_EXPRESS_NUM];         /* Display: detMotion.shpLumaGain, Accuracy: U2.8, Range: [0, 1023], For the highlight area, the brightness judgment gain using motion sharpening is used. The larger the value, the faster the transition from motion sharpening in the dark area to brightness.*/
    AX_U8                            nMotShpVarThr[AX_ISP_REF_AUTOTBL_YUV3DNR_EXPRESS_NUM];           /* Display: detMotion.shpVarThr, Accuracy: U7.0, Range: [0, 127], For highlight areas, the variance judgment threshold for motion sharpening is used. Only when the variance is higher than this threshold will the sharpening be effective. The larger the value, the less effective the motion sharpening will be.*/
    AX_U16                           nMotShpVarGain[AX_ISP_REF_AUTOTBL_YUV3DNR_EXPRESS_NUM];          /* Display: detMotion.shpVarGain, Accuracy: U2.7, Range: [0, 511], For the highlight area, the variance judgment gain of motion sharpening is used. The larger the value, the faster the transition from small variance to large variance motion sharpening*/
    AX_U8                            nMotShpStr[AX_ISP_REF_AUTOTBL_YUV3DNR_EXPRESS_NUM];              /* Display: detMotion.shpStr, Accuracy: U3.5, Range: [0, 255], For highlight areas, motion sharpening intensity is used. The higher the value, the greater the intensity, making it easier to identify as motion.*/
    AX_U8                            nMotYRedCorStr[AX_ISP_REF_AUTOTBL_YUV3DNR_EXPRESS_NUM];          /* Display: detMotion.yRedCorStr, Accuracy: U3.5, Range: [0, 255], The intensity of Y channel motion correction in the red area, the larger the value, the easier it is to determine as stationary. It does not take effect when the nMotSmallSel is at its maximum.*/
    AX_U8                            nMotCRedCorStr[AX_ISP_REF_AUTOTBL_YUV3DNR_EXPRESS_NUM];          /* Display: detMotion.cRedCorStr, Accuracy: U3.5, Range: [0, 255], The intensity of UV channel motion correction in the red area, the larger the value, the easier it is to determine as stationary. It does not take effect when the nMotSmallSel is at its maximum.*/
    AX_U8                            nMotYBlueCorStr[AX_ISP_REF_AUTOTBL_YUV3DNR_EXPRESS_NUM];         /* Display: detMotion.yBlueCorStr, Accuracy: U3.5, Range: [0, 255], The intensity of Y channel motion correction in the blue area, the larger the value, the easier it is to determine as stationary. It does not take effect when the nMdSmallSel is at its maximum*/
    AX_U8                            nMotCBlueCorStr[AX_ISP_REF_AUTOTBL_YUV3DNR_EXPRESS_NUM];         /* Display: detMotion.cBlueCorStr, Accuracy: U3.5, Range: [0, 255], The intensity of UV channel motion correction in the blue area, the larger the value, the easier it is to determine as stationary. It does not take effect when the nMdSmallSel is at its maximum.*/
    AX_U8                            nMotYDecThr[AX_ISP_REF_AUTOTBL_YUV3DNR_EXPRESS_NUM];             /* Display: detMotion.yDecThr, Accuracy: U8.0, Range: [0, 255], Y motion judgement threshold, the more the whole frame tends to be static.*/
    AX_U16                           nMotYDecGain[AX_ISP_REF_AUTOTBL_YUV3DNR_EXPRESS_NUM];            /* Display: detMotion.yDecGain, Accuracy: U1.8, Range: [0, 511], Y determines the transition speed based on movement. The higher the value, the more inclined the whole frame is towards motion.*/
    AX_U8                            nMotYM2sDecThr[AX_ISP_REF_AUTOTBL_YUV3DNR_EXPRESS_NUM];          /* Display: detMotion.yM2sDecThr, Accuracy: U8.0, Range: [0, 255], From dynamic to static region Y, the threshold for motion judgement. The higher the value, the more the whole frame tends to be static.*/
    AX_U16                           nMotYM2sDecGain[AX_ISP_REF_AUTOTBL_YUV3DNR_EXPRESS_NUM];         /* Display: detMotion.yM2sDecGain, Accuracy: U1.8, Range: [0, 511], From dynamic to static zone Y, determine the transition speed. The higher the value, the more inclined the whole frame is towards motion.*/
    AX_U8                            nMotYHdrAdjThr[AX_ISP_REF_AUTOTBL_YUV3DNR_EXPRESS_NUM];          /* Display: detMotion.yHdrAdjThr, Accuracy: U8.0, Range: [0, 255], When applied to Y, the larger the value, the easier it is to correct the short frame area to be still.*/
    AX_U8                            nMotCDecThr[AX_ISP_REF_AUTOTBL_YUV3DNR_EXPRESS_NUM];             /* Display: detMotion.cDecThr, Accuracy: U8.0, Range: [0, 255], UV motion judgement threshold. The higher the value, the more the whole frame tends to be static.*/
    AX_U16                           nMotCDecGain[AX_ISP_REF_AUTOTBL_YUV3DNR_EXPRESS_NUM];            /* Display: detMotion.cDecGain, Accuracy: U1.8, Range: [0, 511], UV motion judgement of transition speed. The higher the value, the more inclined the whole frame is towards motion.*/
    AX_U8                            nMotCAdjThr[AX_ISP_REF_AUTOTBL_YUV3DNR_EXPRESS_NUM];             /* Display: detMotion.cAdjThr, Accuracy: U8.0, Range: [0, 255], The UV motion judgement correction threshold will make the UV more inclined to use the current frame, based on normal motion judgement. The higher the value, the more the overall UV tends to be static.*/
    AX_U16                           nMotCAdjGain[AX_ISP_REF_AUTOTBL_YUV3DNR_EXPRESS_NUM];            /* Display: detMotion.cAdjGain, Accuracy: U1.8, Range: [0, 511], UV motion judgement correction excessive speed will make UV more inclined to use the current frame. The higher the value, the more the overall UV tends to move.*/
    AX_U8                            nMotCHdrAdjThr[AX_ISP_REF_AUTOTBL_YUV3DNR_EXPRESS_NUM];          /* Display: detMotion.cHdrAdjThr, Accuracy: U8.0, Range: [0, 255], When applied to UV, the larger the value, the easier it is to correct the short frame area to be still. */
    AX_U16                           nMotYNoiseLut[AX_ISP_REF_AUTOTBL_YUV3DNR_EXPRESS_NUM][YUV3DNR_LUT_SIZE];        /* Display: detMotion.yNoiseLut, Accuracy: U8.2, Range: [0, 1023], Noise correction motion judgement Y channel feature lookup table, with brightness on the x-axis and subtraction method. The larger the value, the stronger the correction, and the easier it is to mistake real motion for stillness.*/
    AX_U16                           nMotCNoiseLut[AX_ISP_REF_AUTOTBL_YUV3DNR_EXPRESS_NUM][YUV3DNR_LUT_SIZE];        /* Display: detMotion.cNoiseLut, Accuracy: U8.2, Range: [0, 1023], Noise correction motion judgement UV channel feature lookup table, with brightness on the x-axis and subtraction method. The larger the value, the stronger the correction, and the easier it is to mistake real motion for stillness.*/
    AX_U8                            nMotYHvsLut[AX_ISP_REF_AUTOTBL_YUV3DNR_EXPRESS_NUM][YUV3DNR_LUT_SIZE];          /* Display: detMotion.yHvsLut, Accuracy: U3.5, Range: [0, 255], Noise correction motion judgement Y channel feature lookup table, with brightness on the x-axis and multiplication method. The smaller the value, the stronger the correction, and the easier it is to mistake real motion for stillness.*/
    AX_U8                            nMotCHvsLut[AX_ISP_REF_AUTOTBL_YUV3DNR_EXPRESS_NUM][YUV3DNR_LUT_SIZE];          /* Display: detMotion.cHvsLut, Accuracy: U3.5, Range: [0, 255], Noise correction motion judgement UV channel feature lookup table, with brightness on the x-axis and multiplication method. The smaller the value, the stronger the correction, and the easier it is to mistake real motion for stillness.*/
    AX_U8                            nTWtCurW[AX_ISP_REF_AUTOTBL_YUV3DNR_EXPRESS_NUM];                /* Display: tnr.curW, Accuracy: U1.7, Range: [0, 128], The fusion ratio of current motion judgement and historical motion information, the larger the value, the smaller the historical motion information reference, the faster the convergence speed of dynamic and static, and the higher the probability of misjudging as static.*/
    AX_U8                            nTWtPreLimit[AX_ISP_REF_AUTOTBL_YUV3DNR_EXPRESS_NUM];            /* Display: tnr.preLimit, Accuracy: U1.7, Range: [0, 128], The reference threshold for historical motion information, significant motion information greater than this value will be referenced and integrated into the current motion judgment. The larger the value, the smaller the historical motion information reference, and the faster the convergence speed of motion and stillness, increasing the probability of misjudging as stationary.*/
    AX_U16                           nTWtMaxStr[AX_ISP_REF_AUTOTBL_YUV3DNR_EXPRESS_NUM];              /* Display: tnr.maxStr, Accuracy: U10.0, Range: [0, 512], The maximum filtering strength of the time-domain fusion coefficient is greater, and the larger the value, the greater the filtering strength. The larger the proportion of using the current frame, the higher the probability of raindrop noise appearing.*/
    AX_U16                           nTWtSmoothStr[AX_ISP_REF_AUTOTBL_YUV3DNR_EXPRESS_NUM];           /* Display: tnr.smoothStr, Accuracy: U10.0, Range: [0, 512], The average filtering strength of the time-domain fusion coefficient is higher, and the larger the value, the greater the average filtering strength. The proportion of using the current frame is smaller, and the probability of raindrop noise appearing is reduced. */
    AX_U8                            nTWtSfThr0[AX_ISP_REF_AUTOTBL_YUV3DNR_EXPRESS_NUM];              /* Display: tnr.sfThr0, Accuracy: U8.0, Range: [0, 255], Guiding the selection of time-domain fusion coefficients based on regional motion judgement features, the larger the value, the more the global result is biased towards smooth filtering*/
    AX_U16                           nTWtSfGain0[AX_ISP_REF_AUTOTBL_YUV3DNR_EXPRESS_NUM];             /* Display: tnr.sfGain0, Accuracy: U1.8, Range: [0, 511], Guiding the selection of time-domain fusion coefficients based on regional motion judgement features, the larger the value, the steeper the transition*/
    AX_U8                            nTWtSfThr1[AX_ISP_REF_AUTOTBL_YUV3DNR_EXPRESS_NUM];              /* Display: tnr.sfThr1, Accuracy: U8.0, Range: [0, 255], By using detail sensitive motion judgement features to guide the selection of time-domain fusion coefficients, the larger the value, the more the global result is biased towards smooth filtering*/
    AX_U16                           nTWtSfGain1[AX_ISP_REF_AUTOTBL_YUV3DNR_EXPRESS_NUM];             /* Display: tnr.sfGain1, Accuracy: U1.8, Range: [0, 511], Guiding the selection of time-domain fusion coefficients based on detail sensitive motion judgement features, the larger the value, the steeper the transition*/
    AX_U8                            nTWtSfThr2[AX_ISP_REF_AUTOTBL_YUV3DNR_EXPRESS_NUM];              /* Display: tnr.sfThr2, Accuracy: U7.0, Range: [0, 81], Guiding the selection of time-domain fusion coefficients based on pixel relative relationships, the larger the value, the more the global result is biased towards smooth filtering*/
    AX_U8                            nTWtSfGain2[AX_ISP_REF_AUTOTBL_YUV3DNR_EXPRESS_NUM];             /* Display: tnr.sfGain2, Accuracy: U1.7, Range: [0, 255], Guiding the selection of time-domain fusion coefficients through pixel relative relationships, the larger the value, the steeper the transition*/
    AX_U8                            nTWtStr[AX_ISP_REF_AUTOTBL_YUV3DNR_EXPRESS_NUM];                 /* Display: tnr.str, Accuracy: U0.7, Range: [0, 127], The stronger the time-domain filtering strength, the more biased it is towards using the current frame result, and the greater the Y noise in the stationary region.*/
    AX_U8                            nRtCAdjThr[AX_ISP_REF_AUTOTBL_YUV3DNR_EXPRESS_NUM];              /* Display: tnr.cAdjThr, Accuracy: U1.7, Range: [0, 128], Based on the modified UV motion judgement, the threshold for mapping the UV correction temporal fusion coefficient is determined. The smaller the value, the more inclined it is to use the current frame for global UV*/
    AX_U16                           nRtCAdjGain[AX_ISP_REF_AUTOTBL_YUV3DNR_EXPRESS_NUM];             /* Display: tnr.cAdjGain, Accuracy: U4.8, Range: [0, 4095], According to the modified UV motion judgement, the transition speed of the mapping UV correction temporal fusion coefficient is determined. The larger the value, the more inclined it is to use the current frame for the global UV*/
    AX_U8                            nRtCAdj[AX_ISP_REF_AUTOTBL_YUV3DNR_EXPRESS_NUM];                 /* Display: tnr.cAdj, Accuracy: U1.7, Range: [0, 128], According to the modified UV motion judgement, the correction strength of the mapping UV correction temporal fusion coefficient is determined. The larger the value, the more inclined it is to use the current frame for the global UV*/
    AX_U8                            nRtCLimit[AX_ISP_REF_AUTOTBL_YUV3DNR_EXPRESS_NUM];               /* Display: tnr.cLimit, Accuracy: U1.7, Range: [0, 128], The stronger the UV time-domain denoising intensity, the more the current frame is used, and the greater the UV noise in the stationary area.*/
    AX_U8                            nM2sCur[AX_ISP_REF_AUTOTBL_YUV3DNR_EXPRESS_NUM];                 /* Display: tnr.m2sCur, Accuracy: U1.7, Range: [0, 128], In the determination of moving to stationary areas, the current frame needs to be stationary and the historical frames need to be moving. This corresponds to the threshold for the current frame being stationary, and the larger the value, the more likely it is to be ineffective.*/
    AX_U8                            nM2sRef[AX_ISP_REF_AUTOTBL_YUV3DNR_EXPRESS_NUM];                 /* Display: tnr.m2sRef, Accuracy: U1.7, Range: [0, 128], In the determination of moving to stationary areas, the current frame needs to be stationary, and the historical frames need to be motion. Here, the corresponding historical frames are the threshold for motion, and the smaller the value, the more likely it is not to take effect.*/
    AX_U8                            nM2sVarThr[AX_ISP_REF_AUTOTBL_YUV3DNR_EXPRESS_NUM];              /* Display: tnr.m2sVarThr, Accuracy: U7.0, Range: [0, 127], The variance threshold of the texture part in the transition area from motion to stillness is located, and the larger the value, the fewer textures are located*/
    AX_U16                           nM2sVarGain[AX_ISP_REF_AUTOTBL_YUV3DNR_EXPRESS_NUM][YUV3DNR_REGS_GROUP_NUM];          /* Display: tnr.m2sVarGain, Accuracy: U1.11, Range: [0, 4095], Locate the texture overlap speed of the texture part in the transition zone from motion to stillness, with smaller values indicating less overlap, [0]: lf, [1]: hf*/
    AX_U8                            nM2sBlendRatio[AX_ISP_REF_AUTOTBL_YUV3DNR_EXPRESS_NUM][YUV3DNR_REGS_GROUP_NUM];       /* Display: tnr.m2sBlendRatio, Accuracy: U1.7, Range: [0, 128], Locate the texture overlap ratio of the texture part in the transition area from motion to stillness, with smaller values indicating less overlap, [0]: lf, [1]: hf*/
    AX_U8                            nTWtStaticLut[AX_ISP_REF_AUTOTBL_YUV3DNR_EXPRESS_NUM][YUV3DNR_TW_LUT_SIZE];       /* Display: tnr.staticLut, Accuracy: U1.7, Range: [0, 128], Time domain fusion speed curve, the horizontal axis represents the degree to which the history has been stationary, the smaller the value, the longer the stationary time, the vertical axis represents the current mapped time domain fusion coefficient, and the smaller the value, the more reference frames are used.*/
    AX_U8                            nSfYLfRefWtThr[AX_ISP_REF_AUTOTBL_YUV3DNR_EXPRESS_NUM];          /* Display: ynr.lf.refWtThr, Accuracy: U1.7, Range: [0, 128], When providing the next reference frame, the lower the threshold of the time-domain fusion coefficient between the low-frequency denoising result and the denoised image, the more inclined it is to use the denoising result globally*/
    AX_U16                           nSfYLfRefWtGain[AX_ISP_REF_AUTOTBL_YUV3DNR_EXPRESS_NUM];         /* Display: ynr.lf.refWtGain, Accuracy: U4.8, Range: [0, 4095], When providing the next reference frame, the transition speed of the time-domain fusion coefficient between the low-frequency denoising result and the denoised image fusion is faster, and the larger the transition speed, the more inclined it is to use the denoising result globally*/
    AX_U8                            nSfYLfRefRatio[AX_ISP_REF_AUTOTBL_YUV3DNR_EXPRESS_NUM];          /* Display: ynr.lf.refRatio, Accuracy: U1.7, Range: [0, 128], When providing the next reference frame, the larger the ratio of the time-domain fusion coefficient between the low-frequency denoising result and the denoised image, the more inclined it is to use the denoising result globally. The larger the value, the lower the clarity of the still area and the smaller the noise.*/
    AX_U8                            nSfYLfCurWtThr[AX_ISP_REF_AUTOTBL_YUV3DNR_EXPRESS_NUM];          /* Display: ynr.lf.curWtThr, Accuracy: U1.7, Range: [0, 128], When provided to the output frame, the lower the threshold of the time-domain fusion coefficient between the low-frequency denoising result and the denoised image, the more inclined it is to use the denoising result globally*/
    AX_U16                           nSfYLfCurWtGain[AX_ISP_REF_AUTOTBL_YUV3DNR_EXPRESS_NUM];         /* Display: ynr.lf.curWtGain, Accuracy: U4.8, Range: [0, 4095], When providing the output frame, the transition speed of the time-domain fusion coefficient between the low-frequency denoising result and the denoised image fusion is faster, and the larger the transition speed, the more inclined it is to use the denoising result globally*/
    AX_U8                            nSfYLfCurRatio[AX_ISP_REF_AUTOTBL_YUV3DNR_EXPRESS_NUM];          /* Display: ynr.lf.curRatio, Accuracy: U1.7, Range: [0, 128], When providing the output frame, the larger the ratio of the time-domain fusion coefficient between the low-frequency denoising result and the denoised image, the more inclined it is to use the denoising result globally. The larger the value, the lower the clarity of the still area and the smaller the noise.*/
    AX_U8                            nSfYLfEdgeThr[AX_ISP_REF_AUTOTBL_YUV3DNR_EXPRESS_NUM];           /* Display: ynr.lf.edgeThr, Accuracy: U8.0, Range: [0, 255], The threshold for low-frequency edge judgment, the larger the value, the fewer areas are judged as edges.*/
    AX_U16                           nSfYLfEdgeGain[AX_ISP_REF_AUTOTBL_YUV3DNR_EXPRESS_NUM];          /* Display: ynr.lf.edgeGain, Accuracy: U1.8, Range: [0, 511], Low frequency edge judgment transition speed, the smaller the value, the fewer areas are judged as edges.*/
    AX_U8                            nSfYLfDetailThr[AX_ISP_REF_AUTOTBL_YUV3DNR_EXPRESS_NUM];         /* Display: ynr.lf.detailThr, Accuracy: U8.0, Range: [0, 255], The threshold for low-frequency texture judgment is that the larger the value, the fewer areas are judged as texture.*/
    AX_U16                           nSfYLfDetailGain[AX_ISP_REF_AUTOTBL_YUV3DNR_EXPRESS_NUM];        /* Display: ynr.lf.detailGain, Accuracy: U1.8, Range: [0, 511], Low frequency texture determines transition speed, and the smaller the value, the fewer areas are judged as texture.*/
    AX_U8                            nSfYLfSpWinSel[AX_ISP_REF_AUTOTBL_YUV3DNR_EXPRESS_NUM][YUV3DNR_SF_REGS_NUM];       /* Display: ynr.lf.spWinSel, Accuracy: U2.0, Range: [0, 2], The size of the low-frequency denoising window is larger, and the larger the value, the larger the window filter is used, 0:flat, 1:detail, 2:edge*/
    AX_U16                           nSfYLfSpSigma[AX_ISP_REF_AUTOTBL_YUV3DNR_EXPRESS_NUM][YUV3DNR_SF_REGS_NUM];        /* Display: ynr.lf.spSigma, Accuracy: U9.0, Range: [1, 320], The low-frequency denoising spatial filtering coefficient tends to use all pixels within the window as the value increases, and tends to use pixels in the center area of the window as the value decreases., 0:flat, 1:detail, 2:edge*/
    AX_U16                           nSfYLfThr[AX_ISP_REF_AUTOTBL_YUV3DNR_EXPRESS_NUM][YUV3DNR_SF_REGS_NUM];            /* Display: ynr.lf.thr, Accuracy: U8.2, Range: [0, 1023], Low frequency denoising threshold. The larger the value, the greater the denoising intensity, 0:flat, 1:detail, 2:edge*/
    AX_U16                           nSfYLfGain[AX_ISP_REF_AUTOTBL_YUV3DNR_EXPRESS_NUM][YUV3DNR_SF_REGS_NUM];           /* Display: ynr.lf.gain, Accuracy: U2.8, Range: [0, 1023], The degree of excessive edge preservation in low-frequency denoising. The larger the value, the smaller the denoising intensity, 0:flat, 1:detail, 2:edge*/
    AX_U8                            nSfYLfFAlpha[AX_ISP_REF_AUTOTBL_YUV3DNR_EXPRESS_NUM][YUV3DNR_SF_REGS_NUM];         /* Display: ynr.lf.fAlpha, Accuracy: U1.5, Range: [0, 32], The fusion weight of bilateral and NLM filtering tends to use NLM as the weight increases, 0:flat, 1:detail, 2:edge*/
    AX_U16                           nSfYLfHdrAdjThr[AX_ISP_REF_AUTOTBL_YUV3DNR_EXPRESS_NUM];         /* Display: ynr.lf.hdrAdjThr, Accuracy: U8.2, Range: [0, 1023], Short frame current frame brightness low-frequency noise reduction intensity correction, the larger the value, the greater the low-frequency noise reduction intensity of the short frame*/
    AX_U16                           nSfYLfOriStr[AX_ISP_REF_AUTOTBL_YUV3DNR_EXPRESS_NUM];            /* Display: ynr.lf.oriStr, Accuracy: U8.4, Range: [0, 4095], The low-frequency denoising result overlaps the original noise ratio, and the larger the value, the closer it is to the original frame.*/
    AX_S16                           nSfYLfThrLut[AX_ISP_REF_AUTOTBL_YUV3DNR_EXPRESS_NUM][YUV3DNR_LUT_SIZE];         /* Display: ynr.lf.thrLut, Accuracy: S6.2, Range: [-256, 255], Adjust the low-frequency noise reduction threshold based on brightness. The horizontal axis represents brightness, and the larger the vertical axis value, the stronger the noise reduction intensity.*/
    AX_U8                            nSfYLfGainLut[AX_ISP_REF_AUTOTBL_YUV3DNR_EXPRESS_NUM][YUV3DNR_LUT_SIZE];        /* Display: ynr.lf.gainLut, Accuracy: U3.5, Range: [0, 255], Adjust the transition degree of denoising and edge preservation based on brightness. The horizontal axis represents brightness, and the larger the vertical axis value, the smaller the denoising intensity.*/
    AX_U8                            nSfYHfRefWtThr[AX_ISP_REF_AUTOTBL_YUV3DNR_EXPRESS_NUM];          /* Display: ynr.hf.refWtThr, Accuracy: U1.7, Range: [0, 128], When providing the next reference frame, the lower the threshold of the time-domain fusion coefficient between the high-frequency denoising result and the denoised image, the more inclined it is to use the denoising result globally*/
    AX_U16                           nSfYHfRefWtGain[AX_ISP_REF_AUTOTBL_YUV3DNR_EXPRESS_NUM];         /* Display: ynr.hf.refWtGain, Accuracy: U4.8, Range: [0, 4095], When providing the next reference frame, the transition speed of the time-domain fusion coefficient between the high-frequency denoising result and the denoised image fusion is faster, and the larger the transition speed, the more inclined it is to use the denoising result globally*/
    AX_U8                            nSfYHfRefRatio[AX_ISP_REF_AUTOTBL_YUV3DNR_EXPRESS_NUM];          /* Display: ynr.hf.refRatio, Accuracy: U1.7, Range: [0, 128], When providing the next reference frame, the higher the ratio of the time-domain fusion coefficient between the high-frequency denoising result and the denoised image, the more inclined it is to use the denoising result globally. The larger the value, the lower the clarity of the still area and the smaller the noise.*/
    AX_U8                            nSfYHfCurWtThr[AX_ISP_REF_AUTOTBL_YUV3DNR_EXPRESS_NUM];          /* Display: ynr.hf.curWtThr, Accuracy: U1.7, Range: [0, 128], When provided to the output frame, the threshold of the time-domain fusion coefficient between the high-frequency denoising result and the denoised image is smaller, and the smaller the threshold, the more inclined it is to use the denoising result globally*/
    AX_U16                           nSfYHfCurWtGain[AX_ISP_REF_AUTOTBL_YUV3DNR_EXPRESS_NUM];         /* Display: ynr.hf.curWtGain, Accuracy: U4.8, Range: [0, 4095], When providing the output frame, the transition speed of the time-domain fusion coefficient between the high-frequency denoising result and the denoised image fusion is faster, and the larger the transition speed, the more inclined it is to use the denoising result globally*/
    AX_U8                            nSfYHfCurRatio[AX_ISP_REF_AUTOTBL_YUV3DNR_EXPRESS_NUM];          /* Display: ynr.hf.curRatio, Accuracy: U1.7, Range: [0, 128], When providing the output frame, the higher the ratio of the time-domain fusion coefficient between the high-frequency denoising result and the denoised image, the more inclined it is to use the denoising result globally. The larger the value, the lower the clarity of the still area and the smaller the noise.*/
    AX_U8                            nSfYHfEdgeThr[AX_ISP_REF_AUTOTBL_YUV3DNR_EXPRESS_NUM];           /* Display: ynr.hf.edgeThr, Accuracy: U8.0, Range: [0, 255], The threshold for high-frequency edge judgment is that the larger the value, the fewer areas are judged as edges.*/
    AX_U16                           nSfYHfEdgeGain[AX_ISP_REF_AUTOTBL_YUV3DNR_EXPRESS_NUM];          /* Display: ynr.hf.edgeGain, Accuracy: U1.8, Range: [0, 511], High frequency edge judgment transition speed, the smaller the value, the fewer areas are judged as edges.*/
    AX_U8                            nSfYHfDetailThr[AX_ISP_REF_AUTOTBL_YUV3DNR_EXPRESS_NUM];         /* Display: ynr.hf.detailThr, Accuracy: U8.0, Range: [0, 255], High frequency texture judgment threshold, the larger the value, the fewer areas are judged as texture.*/
    AX_U16                           nSfYHfDetailGain[AX_ISP_REF_AUTOTBL_YUV3DNR_EXPRESS_NUM];        /* Display: ynr.hf.detailGain, Accuracy: U1.8, Range: [0, 511], High frequency texture determines transition speed, and the smaller the value, the fewer areas are judged as texture.*/
    AX_U8                            nSfYHfSpWinSel[AX_ISP_REF_AUTOTBL_YUV3DNR_EXPRESS_NUM][YUV3DNR_SF_REGS_NUM];       /* Display: ynr.hf.spWinSel, Accuracy: U1.0, Range: [0, 1], The size of the high-frequency edge preservation denoising window is larger, and the larger the value, the larger the window filtering is used. 0:flat, 1:detail, 2:edge*/
    AX_U16                           nSfYHfSpSigma[AX_ISP_REF_AUTOTBL_YUV3DNR_EXPRESS_NUM][YUV3DNR_SF_REGS_NUM];        /* Display: ynr.hf.spSigma, Accuracy: U9.0, Range: [1, 320], The high-frequency edge preserving denoising spatial filtering coefficient tends to use all pixels within the window as the value increases, and tends to use pixels in the center area of the window as the value decreases., 0:flat, 1:detail, 2:edge*/
    AX_U16                           nSfYHfThr[AX_ISP_REF_AUTOTBL_YUV3DNR_EXPRESS_NUM][YUV3DNR_SF_REGS_NUM];            /* Display: ynr.hf.thr, Accuracy: U8.2, Range: [0, 1023], High frequency edge preservation denoising threshold. The larger the value, the greater the denoising intensity, 0:flat, 1:detail, 2:edge*/
    AX_U16                           nSfYHfGain[AX_ISP_REF_AUTOTBL_YUV3DNR_EXPRESS_NUM][YUV3DNR_SF_REGS_NUM];           /* Display: ynr.hf.gain, Accuracy: U2.8, Range: [0, 1023], High frequency edge preservation denoising and excessive edge preservation. The larger the value, the smaller the denoising intensity, 0:flat, 1:detail, 2:edge*/
    AX_U8                            nSfYHfFAlpha[AX_ISP_REF_AUTOTBL_YUV3DNR_EXPRESS_NUM][YUV3DNR_SF_REGS_NUM];         /* Display: ynr.hf.fAlpha, Accuracy: U1.5, Range: [0, 32], The fusion weight of high-frequency edge preservation denoising bilateral and NLM filtering tends to use NLM as the weight increases, 0:flat, 1:detail, 2:edge*/
    AX_U8                            nSfYHfSimpWinSel[AX_ISP_REF_AUTOTBL_YUV3DNR_EXPRESS_NUM];        /* Display: ynr.hf.simpWinSel, Accuracy: U2.0, Range: [0, 2], The window size of high-frequency non edge preserving denoising filter is larger, and the larger the value, the more pixels are used for denoising, resulting in a blurred effect, 0:flat, 1:detail, 2:edge*/
    AX_U16                           nSfYHfSimpSigma[AX_ISP_REF_AUTOTBL_YUV3DNR_EXPRESS_NUM];         /* Display: ynr.hf.simpSigma, Accuracy: U9.0, Range: [1, 320], High frequency non edge preserving denoising filter spatial filtering coefficient, the larger the value, the more inclined it is to use all pixels within the window, and the smaller the value, the more inclined it is to use pixels in the center area of the window, 0:flat, 1:detail, 2:edge*/
    AX_U8                            nSfYHfSimpRatio[AX_ISP_REF_AUTOTBL_YUV3DNR_EXPRESS_NUM][YUV3DNR_SF_REGS_NUM];      /* Display: ynr.hf.simpRatio, Accuracy: U1.5, Range: [0, 32], The fusion coefficient of high-frequency non edge preserving denoising filtering and edge preserving filtering, the larger the value, the more the non edge preserving filtering result is used. 0:flat, 1:detail, 2:edge.*/
    AX_U16                           nSfYHfHdrAdjThr[AX_ISP_REF_AUTOTBL_YUV3DNR_EXPRESS_NUM];         /* Display: synr.hf.hdrAdjThr, Accuracy: U8.2, Range: [0, 1023], Short frame current frame brightness high-frequency noise reduction intensity correction, the larger the value, the greater the high-frequency noise reduction intensity of the short frame*/
    AX_U16                           nSfYHfOriStr[AX_ISP_REF_AUTOTBL_YUV3DNR_EXPRESS_NUM];            /* Display: ynr.hf.oriStr, Accuracy: U8.4, Range: [0, 4095], The high-frequency denoising result overlaps the original noise ratio, and the larger the value, the closer it is to the original frame.*/
    AX_S16                           nSfYHfThrLut[AX_ISP_REF_AUTOTBL_YUV3DNR_EXPRESS_NUM][YUV3DNR_LUT_SIZE];         /* Display: ynr.hf.thrLut, Accuracy: S6.2, Range: [-256, 255], Adjust the high-frequency edge preservation and noise reduction threshold based on brightness. The horizontal axis represents brightness, and the larger the vertical axis value, the stronger the noise reduction intensity.*/
    AX_U8                            nSfYHfGainLut[AX_ISP_REF_AUTOTBL_YUV3DNR_EXPRESS_NUM][YUV3DNR_LUT_SIZE];        /* Display: ynr.hf.gainLut, Accuracy: U3.5, Range: [0, 255], Adjust the transition degree of denoising and edge preservation based on brightness. The horizontal axis represents brightness, and the larger the vertical axis value, the smaller the denoising intensity.*/
    AX_U8                            nSfCLfRefWtThr[AX_ISP_REF_AUTOTBL_YUV3DNR_EXPRESS_NUM];          /* Display: cnr.lf.refWtThr, Accuracy: U1.7, Range: [0, 128], When providing the next reference frame, the lower the threshold of the time-domain fusion coefficient between the low-frequency denoising result and the denoised image, the more inclined it is to use the denoising result globally*/
    AX_U16                           nSfCLfRefWtGain[AX_ISP_REF_AUTOTBL_YUV3DNR_EXPRESS_NUM];         /* Display: cnr.lf.refWtGain, Accuracy: U4.8, Range: [0, 4095], When providing the next reference frame, the transition speed of the time-domain fusion coefficient between the low-frequency denoising result and the denoised image fusion is faster, and the larger the transition speed, the more inclined it is to use the denoising result globally*/
    AX_U8                            nSfCLfRefRatio[AX_ISP_REF_AUTOTBL_YUV3DNR_EXPRESS_NUM];          /* Display: cnr.lf.refRatio, Accuracy: U1.7, Range: [0, 128], When providing the next reference frame, the larger the ratio of the time-domain fusion coefficient between the low-frequency denoising result and the denoised image, the more inclined it is to use the denoising result globally. The larger the value, the lower the clarity of the still area UV and the smaller the color noise.*/
    AX_U8                            nSfCLfCurWtThr[AX_ISP_REF_AUTOTBL_YUV3DNR_EXPRESS_NUM];          /* Display: cnr.lf.curWtThr, Accuracy: U1.7, Range: [0, 128], When provided to the output frame, the lower the threshold of the time-domain fusion coefficient between the low-frequency denoising result and the denoised image, the more inclined it is to use the denoising result globally*/
    AX_U16                           nSfCLfCurWtGain[AX_ISP_REF_AUTOTBL_YUV3DNR_EXPRESS_NUM];         /* Display: cnr.lf.curWtGain, Accuracy: U4.8, Range: [0, 4095], When providing the output frame, the transition speed of the time-domain fusion coefficient between the low-frequency denoising result and the denoised image fusion is faster, and the larger the transition speed, the more inclined it is to use the denoising result globally*/
    AX_U8                            nSfCLfCurRatio[AX_ISP_REF_AUTOTBL_YUV3DNR_EXPRESS_NUM];          /* Display: cnr.lf.curRatio, Accuracy: U1.7, Range: [0, 128], When provided to the output frame, the ratio of the time-domain fusion coefficient between the low-frequency denoising result and the denoised image fusion is higher, and the larger the ratio, the more inclined it is to use the denoising result globally. The larger the value, the lower the clarity of the still area UV and the smaller the color noise.*/
    AX_U16                           nSfCLfThr[AX_ISP_REF_AUTOTBL_YUV3DNR_EXPRESS_NUM];               /* Display: cnr.lf.thr, Accuracy: U7.2, Range: [0, 511], The threshold for reducing saturation based on its original size, the smaller the value, the more likely it is to be globally ineffective*/
    AX_U16                           nSfCLfGain[AX_ISP_REF_AUTOTBL_YUV3DNR_EXPRESS_NUM];              /* Display: cnr.lf.gain, Accuracy: U2.7, Range: [0, 511], Reduce the transition speed of saturation based on its original size. The larger the value, the more likely it is to be globally ineffective*/
    AX_U8                            nSfCLfLimit[AX_ISP_REF_AUTOTBL_YUV3DNR_EXPRESS_NUM];             /* Display: cnr.lf.limit, Accuracy: U1.7, Range: [0, 128], According to the proportion of saturation overlaid color noise, the smaller the value, the more inclined it is to reduce saturation*/
    AX_U16                           nSfCLfHdrAdjThr[AX_ISP_REF_AUTOTBL_YUV3DNR_EXPRESS_NUM];         /* Display: cnr.lf.hdrAdjThr, Accuracy: U8.2, Range: [0, 1023], Short frame current frame color low-frequency noise reduction intensity correction, the larger the value, the greater the low-frequency noise reduction intensity of the short frame*/
    AX_U8                            nSfCLfOriStr[AX_ISP_REF_AUTOTBL_YUV3DNR_EXPRESS_NUM];            /* Display: cnr.lf.oriStr, Accuracy: U1.5, Range: [0, 32], The ratio of low-frequency fading noise to aliasing noise, the larger the value, the closer it is to the original input*/
    AX_U16                           nSfCLfThrPreLut[AX_ISP_REF_AUTOTBL_YUV3DNR_EXPRESS_NUM][YUV3DNR_LUT_SIZE];      /* Display: cnr.lf.thrPreLut, Accuracy: U8.2, Range: [0, 1023], Low frequency color first stage denoising intensity, horizontal axis brightness, the larger the value, the greater the denoising intensity*/
    AX_U16                           nSfCLfThrLut[AX_ISP_REF_AUTOTBL_YUV3DNR_EXPRESS_NUM][YUV3DNR_LUT_SIZE];         /* Display: cnr.lf.thrLut, Accuracy: U8.2, Range: [0, 1023], Low frequency color two-stage denoising intensity, horizontal axis brightness, the larger the value, the greater the denoising intensity*/
    AX_U16                           nSfCLfPrThrLut[AX_ISP_REF_AUTOTBL_YUV3DNR_EXPRESS_NUM][YUV3DNR_LUT_SIZE];       /* Display: cnr.lf.prThrLut, Accuracy: U8.2, Range: [0, 1023], Low frequency color denoising protection threshold, based on the diff aliasing color noise before and after denoising, the larger the value, the more likely it is not to aliasing*/
    AX_U16                           nSfCLfPrGainLut[AX_ISP_REF_AUTOTBL_YUV3DNR_EXPRESS_NUM][YUV3DNR_LUT_SIZE];      /* Display: cnr.lf.prGainLut, Accuracy: U2.8, Range: [0, 1023], Low frequency color denoising protection transition speed, based on the diff of color noise before and after denoising, the larger the value, the more likely it is to be overlapped*/
    AX_U8                            nSfCHfRefWtThr[AX_ISP_REF_AUTOTBL_YUV3DNR_EXPRESS_NUM];          /* Display: cnr.hf.refWtThr, Accuracy: U1.7, Range: [0, 128], When providing the next reference frame, the lower the threshold of the time-domain fusion coefficient between the high-frequency denoising result and the denoised image, the more inclined it is to use the denoising result globally*/
    AX_U16                           nSfCHfRefWtGain[AX_ISP_REF_AUTOTBL_YUV3DNR_EXPRESS_NUM];         /* Display: cnr.hf.refWtGain, Accuracy: U4.8, Range: [0, 4095], When provided to the next reference frame, the transition speed of the time-domain fusion coefficient between the high-frequency denoising result and the denoised image fusion is faster, and the larger the transition speed, the more inclined it is to use the denoising result globally*/
    AX_U8                            nSfCHfRefRatio[AX_ISP_REF_AUTOTBL_YUV3DNR_EXPRESS_NUM];          /* Display: cnr.hf.refRatio, Accuracy: U1.7, Range: [0, 128], When provided to the next reference frame, the higher the ratio of the temporal fusion coefficient between the high-frequency denoising result and the denoised image, the more inclined it is to use the denoising result globally. The larger the value, the lower the clarity of the still area UV and the smaller the color noise.*/
    AX_U8                            nSfCHfCurWtThr[AX_ISP_REF_AUTOTBL_YUV3DNR_EXPRESS_NUM];          /* Display: cnr.hf.curWtThr, Accuracy: U1.7, Range: [0, 128], When provided to the output frame, the lower the threshold of the time-domain fusion coefficient between the high-frequency denoising result and the denoised image, the more inclined it is to use the denoising result globally*/
    AX_U16                           nSfCHfCurWtGain[AX_ISP_REF_AUTOTBL_YUV3DNR_EXPRESS_NUM];         /* Display: cnr.hf.curWtGain, Accuracy: U4.8, Range: [0, 4095], When provided to the output frame, the transition speed of the time-domain fusion coefficient between the high-frequency denoising result and the denoised image fusion is faster, and the larger the transition speed, the more inclined it is to use the denoising result globally*/
    AX_U8                            nSfCHfCurRatio[AX_ISP_REF_AUTOTBL_YUV3DNR_EXPRESS_NUM];          /* Display: cnr.hf.curRatio, Accuracy: U1.7, Range: [0, 128], When provided to the output frame, the ratio of the time-domain fusion coefficient between the high-frequency denoising result and the denoised image fusion is higher, and the larger the ratio, the more inclined it is to use the denoising result globally. The larger the value, the lower the clarity of the stationary area UV and the smaller the color noise.*/
    AX_U16                           nSfCHfThr[AX_ISP_REF_AUTOTBL_YUV3DNR_EXPRESS_NUM];               /* Display: cnr.hf.thr, Accuracy: U8.2, Range: [0, 1023], High frequency denoising threshold, the larger the value, the stronger the denoising intensity*/
    AX_U16                           nSfCHfGain[AX_ISP_REF_AUTOTBL_YUV3DNR_EXPRESS_NUM];              /* Display: cnr.hf.gain, Accuracy: U2.8, Range: [0, 1023], High frequency denoising transition speed, the larger the value, the weaker the denoising intensity*/
    AX_U8                            nSfCHfHdrAdjGain[AX_ISP_REF_AUTOTBL_YUV3DNR_EXPRESS_NUM];        /* Display: cnr.hf.hdrAdjGain, Accuracy: U1.5, Range: [0, 32], Short frame current frame color high-frequency noise reduction intensity correction, the larger the value, the greater the high-frequency noise reduction intensity of the short frame*/
    AX_U8                            nSfCHfOriStr[AX_ISP_REF_AUTOTBL_YUV3DNR_EXPRESS_NUM];            /* Display: cnr.hf.oriStr, Accuracy: U1.5, Range: [0, 32], High frequency color fading noise and aliasing noise, the larger the value, the closer it is to the original image*/
    AX_U8                            nEhcCWin[AX_ISP_REF_AUTOTBL_YUV3DNR_EXPRESS_NUM];                /* Display: cnr.ehc.win, Accuracy: U3.0, Range: [0, 5], The size of the window for enhance color sf filtering, the larger the value, the greater the intensity of color noise removal*/
    AX_U8                            nEhcCStr[AX_ISP_REF_AUTOTBL_YUV3DNR_EXPRESS_NUM];                /* Display: cnr.ehc.str, Accuracy: U8.0, Range: [1, 255], The strength for enhance color sf filtering to remove color noise, the larger the value, the greater the intensity of color noise removal*/
    AX_U8                            nEhcCDiffThr[AX_ISP_REF_AUTOTBL_YUV3DNR_EXPRESS_NUM];            /* Display: cnr.ehc.diffThr, Accuracy: U8.0, Range: [0, 255], The diff protection strength of enhance color sf filtering for color noise reduction results is weaker as the value increases, and closer to non enhance color sf filtering as the value decreases*/
    AX_U8                            nEhcCSatThr[AX_ISP_REF_AUTOTBL_YUV3DNR_EXPRESS_NUM];             /* Display: cnr.ehc.satThr, Accuracy: U7.0, Range: [0, 127], The results of enhance color sf filtering for removing color noise are based on the sat protection strength. The larger the value, the more inclined it is to use the enhance color sf filtering result*/
    AX_U16                           nEhcCSatGain[AX_ISP_REF_AUTOTBL_YUV3DNR_EXPRESS_NUM];            /* Display: cnr.ehc.satGain, Accuracy: U1.8, Range: [0, 511], The enhance color sf filtering denoising result is based on the transition speed of SAT protection. The larger the value, the more inclined it is to use the original denoising result*/
    AX_U8                            nEhcCSatRatio[AX_ISP_REF_AUTOTBL_YUV3DNR_EXPRESS_NUM][YUV3DNR_REGS_GROUP_NUM];        /* Display: cnr.ehc.satRatio, Accuracy: U1.7, Range: [0, 128], The enhance color sf filtering denoising result is based on the fusion ratio between sat and the original denoising result. The larger the value, the more inclined it is to the original denoising result*/
    AX_U8                            nEhcCRatioThr[AX_ISP_REF_AUTOTBL_YUV3DNR_EXPRESS_NUM];           /* Display: cnr.ehc.ratioThr, Accuracy: U1.7, Range: [0, 128], Determine the time-domain fusion coefficient threshold for the fusion enhance color sf filtering result of the dynamic and static regions based on the time-domain fusion coefficient. It is hoped that different fusion ratios will be used for the dynamic and static regions*/
    AX_S16                           nEhcCRatioGain[AX_ISP_REF_AUTOTBL_YUV3DNR_EXPRESS_NUM];          /* Display: cnr.ehc.ratioGain, Accuracy: S7.7, Range: [-16384, 16383], Determine the transition speed of the fusion enhance color sf filtering result for the dynamic and static regions based on the time-domain fusion coefficient. It is hoped that different fusion ratios will be used for the dynamic and static regions*/
    AX_U8                            nEhcCRatio[AX_ISP_REF_AUTOTBL_YUV3DNR_EXPRESS_NUM][YUV3DNR_REGS_GROUP_NUM];           /* Display: cnr.ehc.ratio, Accuracy: U1.7, Range: [0, 128], According to the time-domain fusion coefficient, the proportion of the fusion enhance color sf filtering results in the dynamic and static regions is determined. The first value corresponds to the static region, and the second value corresponds to the motion region. The larger the value, the more inclined it is to use the guided filtering results*/
} AX_ISP_IQ_YUV3DNR_AUTO_T;

typedef struct {
    AX_U8                            nYuv3dnrEn;                          /* yuv3dnr enable, Accuracy: U1.0 Range: [0, 1], 0: Disable, 1: Enable */
    AX_U8                            nAutoMode;                           /* for ref auto or manual adjust mode, Accuracy: U1.0 Range:[0, 1], 0: manual, 1:auto, default:1 */
    AX_ISP_IQ_YUV3DNR_MANUAL_T       tManualParam;
    AX_ISP_IQ_YUV3DNR_AUTO_T         tAutoParam;
} AX_ISP_IQ_YUV3DNR_PARAM_EX_T;

/************************************************************************************
 *  SHARPEN IQ Auto Ext Param
 ************************************************************************************/
#define AX_ISP_REF_AUTOTBL_SHARPEN_EXPRESS_NUM                           (12)

typedef struct {
    /* ref_mode: gain/lux */
    AX_U8                            nRefMode;                                                        /* choose ref mode, Accuracy: U1.0 Range: [0, 1], 0:use lux as ref, 1:use gain as ref */
    AX_U8                            nParamGrpNum;                                                    /* Accuracy: U8.0 Range: [1, 12] */
    AX_U32                           nRefVal[AX_ISP_REF_AUTOTBL_SHARPEN_EXPRESS_NUM];                 /* Gain: Accuracy: U22.10 Range: [0x400, 0xFFFFFFFF]; Lux: Accuracy: U22.10 Range: [0, 0xFFFFFFFF] */
    AX_U16                           nUdGainLut[AX_ISP_REF_AUTOTBL_SHARPEN_EXPRESS_NUM][AX_ISP_SHP_GAIN_SIZE];          /* Display: udGainLut, Accuracy: U8.4, Range: [0, 4095], set undirectional gain w.r.t. texture strength*/
    AX_U8                            nUdGainLutAnchor[AX_ISP_REF_AUTOTBL_SHARPEN_EXPRESS_NUM];        /* Display: udGainLutAnchor, Accuracy: U3.0, Range: [0, 7], smaller value means more anchor points in flat/weak texture regions*/
    AX_U16                           nDirGainLut[AX_ISP_REF_AUTOTBL_SHARPEN_EXPRESS_NUM][AX_ISP_SHP_GAIN_SIZE];         /* Display: dirGainLut, Accuracy: U8.4, Range: [0, 4095], set directional gain w.r.t. edge strength*/
    AX_U8                            nDirGainLutAnchor[AX_ISP_REF_AUTOTBL_SHARPEN_EXPRESS_NUM];       /* Display: dirGainLutAnchor, Accuracy: U3.0, Range: [0, 7], smaller value means more anchor points in flat/weak texture regions*/
    AX_U16                           nUdFreq[AX_ISP_REF_AUTOTBL_SHARPEN_EXPRESS_NUM];                 /* Display: udFreq, Accuracy: U5.5, Range: [0, 1023], larger value means higher undirectional frequecny will be enhanced*/
    AX_U16                           nDirFreq[AX_ISP_REF_AUTOTBL_SHARPEN_EXPRESS_NUM];                /* Display: dirFreq, Accuracy: U5.5, Range: [0, 1023], larger value means higher directional frequecny will be enhanced*/
    AX_U8                            nEdgeStr[AX_ISP_REF_AUTOTBL_SHARPEN_EXPRESS_NUM];                /* Display: edgeStr, Accuracy: U4.0, Range: [0, 15], larger value means to use more directional filter's result*/
    AX_U16                           nEdgeThr[AX_ISP_REF_AUTOTBL_SHARPEN_EXPRESS_NUM];                /* Display: edgeThr, Accuracy: U8.4, Range: [0, 4095], larger value means less pixels will be considered as edge pixels*/
    AX_U8                            nEdgeSmoothThinRatio[AX_ISP_REF_AUTOTBL_SHARPEN_EXPRESS_NUM];    /* Display: edgeSmoothThinRatio, Accuracy: U4.0, Range: [0, 15], larger value means to make the edge much thinner*/
    AX_U8                            nOvershoot[AX_ISP_REF_AUTOTBL_SHARPEN_EXPRESS_NUM];              /* Display: overshoot, Accuracy: U0.7, Range: [0, 127], larger value means stronger overshoot*/
    AX_U8                            nUndershoot[AX_ISP_REF_AUTOTBL_SHARPEN_EXPRESS_NUM];             /* Display: undershoot, Accuracy: U0.7, Range: [0, 127], larger value means stronger undershoot*/
    AX_U8                            nDetailRegionThr[AX_ISP_REF_AUTOTBL_SHARPEN_EXPRESS_NUM];        /* Display: detailRegionThr, Accuracy: U8.0, Range: [0, 255], larger value means more pixels will be considered as detail region*/
    AX_S8                            nDetailOvershootAdj[AX_ISP_REF_AUTOTBL_SHARPEN_EXPRESS_NUM];     /* Display: detailOvershootAdj, Accuracy: S0.7, Range: [-127, 127], larger value means stronger overshoot for detail region*/
    AX_S8                            nDetailUndershootAdj[AX_ISP_REF_AUTOTBL_SHARPEN_EXPRESS_NUM];    /* Display: detailUndershootAdj, Accuracy: S0.7, Range: [-127, 127], larger value means stronger undershoot for detail region*/
    AX_U8                            nShootReleaseRatio[AX_ISP_REF_AUTOTBL_SHARPEN_EXPRESS_NUM];      /* Display: shootReleaseRatio, Accuracy: U1.4, Range: [0, 16], larger value means shoot suppression tends to ignore more weak texture/detail region*/
    AX_U8                            nShootSupRange[AX_ISP_REF_AUTOTBL_SHARPEN_EXPRESS_NUM];          /* Display: shootSupRange, Accuracy: U8.0, Range: [0, 255], larger value means shoot suppression tends to control more edges*/
    AX_U8                            nShootSupStr[AX_ISP_REF_AUTOTBL_SHARPEN_EXPRESS_NUM];            /* Display: shootSupStr, Accuracy: U0.7, Range: [0, 127], larger value means stronger shoot suppression on edges*/
    AX_U8                            nShpExtraGain[AX_ISP_REF_AUTOTBL_SHARPEN_EXPRESS_NUM];           /* Display: shpExtraGain, Accuracy: U4.4, Range: [0, 255], larger value means more extra gain applied to ud/dir gain lut*/
    AX_U16                           nShpLimit[AX_ISP_REF_AUTOTBL_SHARPEN_EXPRESS_NUM];               /* Display: shpLimit, Accuracy: U5.5, Range: [0, 1023], smaller value means more restriction on the pixel value change, and may reduce the number of white/black points*/
    AX_U8                            nLumaGainLutNeg[AX_ISP_REF_AUTOTBL_SHARPEN_EXPRESS_NUM][AX_ISP_SHP_GAIN_SIZE];     /* Display: lumaGainLutNeg, Accuracy: U1.5, Range: [0, 63], adjust sharpen gain for negative hf based on image brightness*/
    AX_U8                            nLumaGainLutPos[AX_ISP_REF_AUTOTBL_SHARPEN_EXPRESS_NUM][AX_ISP_SHP_GAIN_SIZE];     /* Display: lumaGainLutPos, Accuracy: U1.5, Range: [0, 63], adjust sharpen gain for positive hf based on image brightness*/
    AX_U8                            nRedGain[AX_ISP_REF_AUTOTBL_SHARPEN_EXPRESS_NUM];                /* Display: redGain, Accuracy: U1.5, Range: [0, 32], smaller value means less sharpen for red region*/
    AX_U8                            nGreenGain[AX_ISP_REF_AUTOTBL_SHARPEN_EXPRESS_NUM];              /* Display: greenGain, Accuracy: U3.5, Range: [0, 255], smaller value means less sharpen for green region*/
    AX_U8                            nBlueGain[AX_ISP_REF_AUTOTBL_SHARPEN_EXPRESS_NUM];               /* Display: blueGain, Accuracy: U1.5, Range: [0, 32], smaller value means less sharpen for blue region*/
    AX_U8                            nSkinGain[AX_ISP_REF_AUTOTBL_SHARPEN_EXPRESS_NUM];               /* Display: skinGain, Accuracy: U1.5, Range: [0, 32], smaller value means less sharpen for skin region*/
    AX_U8                            nMotionOvershoot[AX_ISP_REF_AUTOTBL_SHARPEN_EXPRESS_NUM];        /* Display: motionOvershoot, Accuracy: U0.7, Range: [0, 127], larger value means stronger overshoot*/
    AX_U8                            nMotionUndershoot[AX_ISP_REF_AUTOTBL_SHARPEN_EXPRESS_NUM];       /* Display: motionUndershoot, Accuracy: U0.7, Range: [0, 127], larger value means stronger undershoot*/
    AX_U16                           nMotionUdGainLut[AX_ISP_REF_AUTOTBL_SHARPEN_EXPRESS_NUM][AX_ISP_SHP_GAIN_SIZE];    /* Display: motionUdGainLut, Accuracy: U8.4, Range: [0, 4095], set undirectional gain w.r.t. texture strength*/
    AX_U16                           nMotionDirGainLut[AX_ISP_REF_AUTOTBL_SHARPEN_EXPRESS_NUM][AX_ISP_SHP_GAIN_SIZE];   /* Display: motionDirGainLut, Accuracy: U8.4, Range: [0, 4095], set directional gain w.r.t. edge strength*/
    AX_U8                            nNrStr[AX_ISP_REF_AUTOTBL_SHARPEN_EXPRESS_NUM];                  /* Display: nrStr, Accuracy: U1.5, Range: [0, 32], larger value means stronger noise reduction*/
    AX_U8                            nNrUdTextureSens[AX_ISP_REF_AUTOTBL_SHARPEN_EXPRESS_NUM];        /* Display: nrUdTextureSens, Accuracy: U0.8, Range: [0, 255], larger value means image has more undirectional textures and apply less noise reduction*/
    AX_U8                            nNrUdTextureThr[AX_ISP_REF_AUTOTBL_SHARPEN_EXPRESS_NUM];         /* Display: nrUdTextureThr, Accuracy: U0.8, Range: [0, 255], larger value means less pixels will be considered as real texture and apply more noise reduction*/
    AX_U16                           nNrUdLimit[AX_ISP_REF_AUTOTBL_SHARPEN_EXPRESS_NUM];              /* Display: nrUdLimit, Accuracy: U8.2, Range: [0, 1023], larger value means less restriction on undirectional noise reduction*/
    AX_U8                            nUdCoring[AX_ISP_REF_AUTOTBL_SHARPEN_EXPRESS_NUM];               /* Display: udCoring, Accuracy: U3.3, Range: [0, 63], larger value means stronger coring for undirectional details*/
    AX_U8                            nNrDirEdgeSens[AX_ISP_REF_AUTOTBL_SHARPEN_EXPRESS_NUM];          /* Display: nrDirEdgeSens, Accuracy: U0.8, Range: [0, 255], larger value means image has more directional edges and apply less noise reduction*/
    AX_U8                            nNrDirEdgeThr[AX_ISP_REF_AUTOTBL_SHARPEN_EXPRESS_NUM];           /* Display: nrDirEdgeThr, Accuracy: U4.4, Range: [0, 255], larger value means less pixels will be considered as real edge and apply more noise reduction*/
    AX_U16                           nNrDirLimit[AX_ISP_REF_AUTOTBL_SHARPEN_EXPRESS_NUM];             /* Display: nrDirLimit, Accuracy: U8.2, Range: [0, 1023], larger value means less restriction on directional noise reduction*/
    AX_U8                            nDirCoring[AX_ISP_REF_AUTOTBL_SHARPEN_EXPRESS_NUM];              /* Display: dirCoring, Accuracy: U3.3, Range: [0, 63], larger value means stronger coring for directional details*/
} AX_ISP_IQ_SHARPEN_AUTO_T;

typedef struct {
    AX_U8                            nSharpenEn;                          /* sharpen enable, Accuracy: U1.0 Range: [0, 1], 0: Disable, 1: Enable */
    AX_U8                            nAutoMode;                           /* for ref auto or manual adjust mode, Accuracy: U1.0 Range:[0, 1], 0: manual, 1:auto, default:1 */
    AX_ISP_IQ_SHARPEN_MANUAL_T       tManualParam;
    AX_ISP_IQ_SHARPEN_AUTO_T         tAutoParam;
} AX_ISP_IQ_SHARPEN_PARAM_EX_T;

/************************************************************************************
 *  Depurple IQ Auto Ext Param
 ************************************************************************************/

#define AX_ISP_REF_AUTOTBL_DEPURPLE_EXPRESS_NUM                          (16)

typedef struct {
    /* ref_mode: gain/lux */
    AX_U8                            nRefMode;                                                         /* choose ref mode, Accuracy: U1.0 Range: [0, 1], 0:use lux as ref, 1:use gain as ref */
    AX_U8                            nParamGrpNum;                                                     /* Accuracy: U8.0 Range: [1, 16] */
    AX_U32                           nRefVal[AX_ISP_REF_AUTOTBL_DEPURPLE_EXPRESS_NUM];                 /* Gain: Accuracy: U22.10 Range: [0x400, 0xFFFFFFFF]; Lux: Accuracy: U22.10 Range: [0, 0xFFFFFFFF] */
    AX_U16                           nDetSeledgeThrY[AX_ISP_REF_AUTOTBL_DEPURPLE_EXPRESS_NUM];         /* Display: detSeledgeThrY, Accuracy: U8.6, Range: [0, 16383], for luma compensation when detect edge, used to compute for mesh-wisely*/
    AX_U16                           nDetMaskStr[AX_ISP_REF_AUTOTBL_DEPURPLE_EXPRESS_NUM];             /* Display: detMaskStr, Accuracy: U3.6, Range: [0, 511],  in pfd hardware, strength adjust for mask*/
    AX_U8                            nDesatStr[AX_ISP_REF_AUTOTBL_DEPURPLE_EXPRESS_NUM];               /* Display: desatStr, Accuracy: U4.4, Range: [0, 240], in lce hardware, desat_strength*/
    AX_U8                            nDesatAngleRatioLut[AX_ISP_REF_AUTOTBL_DEPURPLE_EXPRESS_NUM][AX_ISP_DEPURPLE_DESAT_ANGLE_LUT_NUM]; /* Display: desatAngleRatioLut, Accuracy: U1.7, Range: [0, 128], in lce hardware, will calculate 16 phase ratio for clc hardware, color adjust 16 phase ratio for mask receiving from pfd*/
} AX_ISP_IQ_DEPURPLE_AUTO_T;

typedef struct {
    AX_U8                            nDepurpleEn;                         /* depurple enable, Accuracy: U1.0 Range: [0, 1], 0: Disable, 1: Enable */
    AX_U8                            nAutoMode;                           /* for ref auto or manual adjust mode, Accuracy: U1.0 Range:[0, 1], 0: manual, 1:auto, default:1 */
    AX_ISP_IQ_DEPURPLE_MANUAL_T      tManualParam;
    AX_ISP_IQ_DEPURPLE_AUTO_T        tAutoParam;
} AX_ISP_IQ_DEPURPLE_PARAM_EX_T;

/************************************************************************************
 *  HS2DLUT IQ Auto Ext Param
 ************************************************************************************/
#define AX_ISP_REF_AUTOTBL_HS2DLUT_EXPRESS_NUM                           (6)
#define AX_ISP_REF_AUTOTBL_HS2DLUT_COLOR_TEMP_EXPRESS_NUM                (6)

typedef struct {
    /* ref_mode: gain/lux && color_temp */
    AX_U8                            nRefMode;                                                                                                           /* choose ref mode, Accuracy: U1.0 Range: [0, 1], 0:use lux as ref, 1:use gain as ref */
    AX_U8                            nParamGrpNum;                                                                                                       /* Accuracy: U8.0 Range: [1, 12] */
    AX_U32                           nRefVal[AX_ISP_REF_AUTOTBL_HS2DLUT_EXPRESS_NUM];                                                                    /* Gain: Accuracy: U22.10 Range: [0x400, 0xFFFFFFFF]; Lux: Accuracy: U22.10 Range: [0, 0xFFFFFFFF] */
    AX_U8                            nParamGrpNumCt;                                                                                                     /* Accuracy: U8.0 Range: [1, 12] */
    AX_U32                           nRefValCt[AX_ISP_REF_AUTOTBL_HS2DLUT_COLOR_TEMP_EXPRESS_NUM];                                                       /* Accuracy: U14.0 Range: [0, 16000] */
    AX_U16                           nHs2dlutHueTable[AX_ISP_REF_AUTOTBL_HS2DLUT_COLOR_TEMP_EXPRESS_NUM][AX_ISP_REF_AUTOTBL_HS2DLUT_EXPRESS_NUM][AX_ISP_HS2DLUT_H_SIZE][AX_ISP_HS2DLUT_V_SIZE]; /* Display: hs2dlutHueTable, Accuracy: U9.7, Range: [0, 46080], */
    AX_U16                           nHs2dlutSatTable[AX_ISP_REF_AUTOTBL_HS2DLUT_COLOR_TEMP_EXPRESS_NUM][AX_ISP_REF_AUTOTBL_HS2DLUT_EXPRESS_NUM][AX_ISP_HS2DLUT_H_SIZE][AX_ISP_HS2DLUT_V_SIZE]; /* Display: hs2dlutSatTable, Accuracy: U1.15, Range: [0, 32768], */
} AX_ISP_IQ_HS2DLUT_AUTO_T;

typedef struct {
    AX_U8                            nHs2dlutEn;                          /* hs2dlut enable, Accuracy: U1.0 Range: [0, 1], 0: Disable, 1: Enable */
    AX_U8                            nAutoMode;                           /* for ref auto or manual adjust mode, Accuracy: U1.0 Range:[0, 1], 0: manual, 1:auto, default:1 */
    AX_ISP_IQ_HS2DLUT_MANUAL_T       tManualParam;
    AX_ISP_IQ_HS2DLUT_AUTO_T         tAutoParam;
} AX_ISP_IQ_HS2DLUT_PARAM_EX_T;

/************************************************************************************
 *  CCMP IQ Auto Ext Param
 ************************************************************************************/
#define AX_ISP_REF_AUTOTBL_CCMP_EXPRESS_NUM                              (12)

typedef struct {
    /* ref_mode: gain/lux */
    AX_U8                            nRefMode;                                                     /* choose ref mode, Accuracy: U1.0 Range: [0, 1], 0:use lux as ref, 1:use gain as ref */
    AX_U8                            nParamGrpNum;                                                 /* Accuracy: U8.0 Range: [1, 12] */
    AX_U32                           nRefVal[AX_ISP_REF_AUTOTBL_CCMP_EXPRESS_NUM];                 /* Gain: Accuracy: U22.10 Range: [0x400, 0xFFFFFFFF]; Lux: Accuracy: U22.10 Range: [0, 0xFFFFFFFF] */
    AX_U16                           nCcmpYLut[AX_ISP_REF_AUTOTBL_CCMP_EXPRESS_NUM][AX_ISP_CCMP_Y_SIZE];           /* Display: ccmpYLut, Accuracy: U1.9, Range: [0, 512], smaller value means stronger saturation compression*/
    AX_U16                           nCcmpSatLut[AX_ISP_REF_AUTOTBL_CCMP_EXPRESS_NUM][AX_ISP_CCMP_SAT_SIZE];         /* Display: ccmpSatLut, Accuracy: U1.9, Range: [0, 512], smaller value means stronger saturation compression*/
} AX_ISP_IQ_CCMP_AUTO_T;

typedef struct {
    AX_U8                            nCcmpEn;                             /* ccmp enable, Accuracy: U1.0 Range: [0, 1], 0: Disable, 1: Enable */
    AX_U8                            nAutoMode;                           /* for ref auto or manual adjust mode, Accuracy: U1.0 Range:[0, 1], 0: manual, 1:auto, default:1 */
    AX_ISP_IQ_CCMP_MANUAL_T          tManualParam;
    AX_ISP_IQ_CCMP_AUTO_T            tAutoParam;
} AX_ISP_IQ_CCMP_PARAM_EX_T;

/************************************************************************************
 *  SCM IQ Auto Ext Param
 ************************************************************************************/
#define AX_ISP_REF_AUTOTBL_SCM_EXPRESS_NUM                               (12)

typedef struct {
    /* ref_mode: gain/lux */
    AX_U8                            nRefMode;                                                    /* choose ref mode, Accuracy: U1.0 Range: [0, 1], 0:use lux as ref, 1:use gain as ref */
    AX_U8                            nParamGrpNum;                                                /* Accuracy: U8.0 Range: [1, 12] */
    AX_U32                           nRefVal[AX_ISP_REF_AUTOTBL_SCM_EXPRESS_NUM];                 /* Gain: Accuracy: U22.10 Range: [0x400, 0xFFFFFFFF]; Lux: Accuracy: U22.10 Range: [0, 0xFFFFFFFF] */
    AX_U16                           nScmSrcY[AX_ISP_REF_AUTOTBL_SCM_EXPRESS_NUM];                /* Display: scmSrcY, Accuracy: U8.2, Range: [0, 1023], color mask center y*/
    AX_S16                           nScmSrcUv[AX_ISP_REF_AUTOTBL_SCM_EXPRESS_NUM][AX_ISP_SCM_MASK_CENTER_UV_SIZE];            /* Display: scmSrcUv, Accuracy: S7.2, Range: [-512, 511], color mask center uv*/
    AX_U16                           nScmRadius[AX_ISP_REF_AUTOTBL_SCM_EXPRESS_NUM][AX_ISP_SCM_MASK_SIZE];           /* Display: scmRadius, Accuracy: U7.2, Range: [0, 511], color mask radius, [0]: y, [1]: u, [2]: v*/
    AX_U8                            nScmSmooth[AX_ISP_REF_AUTOTBL_SCM_EXPRESS_NUM][AX_ISP_SCM_MASK_SIZE];           /* Display: scmSmooth, Accuracy: U3.0, Range: [0, 7], color mask transition band*/
    AX_S16                           nScmDstUv[AX_ISP_REF_AUTOTBL_SCM_EXPRESS_NUM][AX_ISP_SCM_COLOR_SIZE];            /* Display: scmDstUv, Accuracy: S7.2, Range: [-512, 511], target color*/
} AX_ISP_IQ_SCM_AUTO_T;

typedef struct {
    AX_U8                            nScmEn;                              /* scm enable, Accuracy: U1.0 Range: [0, 1], 0: Disable, 1: Enable */
    AX_U8                            nAutoMode;                           /* for ref auto or manual adjust mode, Accuracy: U1.0 Range:[0, 1], 0: manual, 1:auto, default:1 */
    AX_ISP_IQ_SCM_MANUAL_T           tManualParam;
    AX_ISP_IQ_SCM_AUTO_T             tAutoParam;
} AX_ISP_IQ_SCM_PARAM_EX_T;

/************************************************************************************
 *  YCPROC IQ Auto Ext Param
 ************************************************************************************/
typedef struct {
    AX_U8                            nYcprocEn;                           /* ycproc enable, Accuracy: U1.0 Range: [0, 1], 0: Disable, 1: Enable */
    AX_ISP_IQ_YCPROC_MANUAL_T        tManualParam;
} AX_ISP_IQ_YCPROC_PARAM_EX_T;

/************************************************************************************
 *  Y2DNR IQ Auto Ext Param
 ************************************************************************************/
#define AX_ISP_REF_AUTOTBL_Y2DNR_EXPRESS_NUM                             (12)

typedef struct {
    /* ref_mode: gain/lux */
    AX_U8                            nRefMode;                                                      /* choose ref mode, Accuracy: U1.0 Range: [0, 1], 0:use lux as ref, 1:use gain as ref */
    AX_U8                            nParamGrpNum;                                                  /* Accuracy: U8.0 Range: [1, 12] */
    AX_U32                           nRefVal[AX_ISP_REF_AUTOTBL_Y2DNR_EXPRESS_NUM];                 /* Gain: Accuracy: U22.10 Range: [0x400, 0xFFFFFFFF]; Lux: Accuracy: U22.10 Range: [0, 0xFFFFFFFF] */
    AX_U8                            nDirStr[AX_ISP_REF_AUTOTBL_Y2DNR_EXPRESS_NUM];                 /* Display: dirStr, Accuracy: U0.4, Range: [0, 15], direction detection strength*/
    AX_U16                           nDirBase[AX_ISP_REF_AUTOTBL_Y2DNR_EXPRESS_NUM];                /* Display: dirBase, Accuracy: U8.4, Range: [0, 4095], base noise for direciton detection*/
    AX_U16                           nDirDetCorStrFg[AX_ISP_REF_AUTOTBL_Y2DNR_EXPRESS_NUM][AX_ISP_STRENGTH_FG_SIZE];      /* Display: dirDetCorStrFg, Accuracy: U8.2, Range: [0, 1023], direction detection correction strength for foreground*/
    AX_U16                           nDirDetCorStrBg[AX_ISP_REF_AUTOTBL_Y2DNR_EXPRESS_NUM][AX_ISP_STRENGTH_BG_SIZE];      /* Display: dirDetCorStrBg, Accuracy: U8.2, Range: [0, 1023], direction detection correction strength for background*/
    AX_U16                           nDetailDetStrFg[AX_ISP_REF_AUTOTBL_Y2DNR_EXPRESS_NUM][AX_ISP_STRENGTH_FG_SIZE];      /* Display: detailDetStrFg, Accuracy: U8.2, Range: [0, 1023], detail detection strength for foreground*/
    AX_U16                           nDetailDetStrBg[AX_ISP_REF_AUTOTBL_Y2DNR_EXPRESS_NUM][AX_ISP_STRENGTH_BG_SIZE];      /* Display: detailDetStrBg, Accuracy: U8.2, Range: [0, 1023], detail detection strength for background*/
    AX_U8                            nDpDetStr[AX_ISP_REF_AUTOTBL_Y2DNR_EXPRESS_NUM];               /* Display: dpDetStr, Accuracy: U4.4, Range: [0, 255], dp detection strength*/
    AX_U16                           nDpStr[AX_ISP_REF_AUTOTBL_Y2DNR_EXPRESS_NUM];                  /* Display: dpStr, Accuracy: U4.8, Range: [0, 4095], dp strength*/
    AX_U8                            nDpNoiseLevel[AX_ISP_REF_AUTOTBL_Y2DNR_EXPRESS_NUM];           /* Display: dpNoiseLevel, Accuracy: U8.0, Range: [0, 255], dp noise level*/
    AX_U8                            nDpNoiseStr[AX_ISP_REF_AUTOTBL_Y2DNR_EXPRESS_NUM][AX_ISP_DP_GAIN_SIZE];          /* Display: dpNoiseStr, Accuracy: U0.8, Range: [0, 255], dp noise gain*/
    AX_U8                            nDpDetailGain[AX_ISP_REF_AUTOTBL_Y2DNR_EXPRESS_NUM][AX_ISP_DP_GAIN_SIZE];        /* Display: dpDetailGain, Accuracy: U0.8, Range: [0, 255], dp detail gain*/
    AX_U16                           nSf1Str[AX_ISP_REF_AUTOTBL_Y2DNR_EXPRESS_NUM];                 /* Display: sf1Str, Accuracy: U1.8, Range: [0, 256], sf1 strength, the larger the value, the more nosie reduction*/
    AX_U8                            nSf1LocalStr[AX_ISP_REF_AUTOTBL_Y2DNR_EXPRESS_NUM][AX_ISP_LOCAL_STRENGTH_SIZE];        /* Display: sf1LocalStr, Accuracy: U0.8, Range: [0, 255], sf1 local strength, the larger the value, the more nosie reduction*/
    AX_U8                            nSf1StyleAdj[AX_ISP_REF_AUTOTBL_Y2DNR_EXPRESS_NUM];            /* Display: sf1StyleAdj, Accuracy: U8.0, Range: [0, 255], sf1 style adjust, the larger the value, the more nosie reduction, also may be more annoying patterns*/
    AX_U8                            nSf1DirStrScale[AX_ISP_REF_AUTOTBL_Y2DNR_EXPRESS_NUM];         /* Display: sf1DirStrScale, Accuracy: U1.4, Range: [0, 16], sf1 direction strength scale, the larger the value, the more direction noise reduction result used*/
    AX_U16                           nSf1DirStr[AX_ISP_REF_AUTOTBL_Y2DNR_EXPRESS_NUM][AX_ISP_DIR_STRENGTH_SIZE];           /* Display: sf1DirStr, Accuracy: U8.2, Range: [0, 1023], sf1 direction strength, the larger the value, the more noise reduction*/
    AX_U16                           nSf1DetailStr[AX_ISP_REF_AUTOTBL_Y2DNR_EXPRESS_NUM][AX_ISP_DETAIL_STRENGTH_SIZE];        /* Display: sf1DetailStr, Accuracy: U8.2, Range: [0, 1023], sf1 detail strength, the larger the value, the more noise reduction*/
    AX_U16                           nSf1FlatStr[AX_ISP_REF_AUTOTBL_Y2DNR_EXPRESS_NUM][AX_ISP_FLAT_STRENGTH_SIZE];          /* Display: sf1FlatStr, Accuracy: U8.2, Range: [0, 1023], sf1 flat strength, the larger the value, the more noise reduction*/
    AX_U16                           nSf2Str[AX_ISP_REF_AUTOTBL_Y2DNR_EXPRESS_NUM];                 /* Display: sf2Str, Accuracy: U1.8, Range: [0, 256], sf2 strength, the larger the value, the more nosie reduction*/
    AX_U8                            nSf2LocalStr[AX_ISP_REF_AUTOTBL_Y2DNR_EXPRESS_NUM][AX_ISP_LOCAL_STRENGTH_SIZE];        /* Display: sf2LocalStr, Accuracy: U0.8, Range: [0, 255], sf2 local strength, the larger the value, the more nosie reduction*/
    AX_U8                            nSf2StyleAdj[AX_ISP_REF_AUTOTBL_Y2DNR_EXPRESS_NUM];            /* Display: sf2StyleAdj, Accuracy: U8.0, Range: [0, 255], sf2 style adjust, the larger the value, the more nosie reduction, also may be more annoying patterns*/
    AX_U16                           nSf2DirStr[AX_ISP_REF_AUTOTBL_Y2DNR_EXPRESS_NUM][AX_ISP_DIR_STRENGTH_SIZE];           /* Display: sf2DirStr, Accuracy: U8.2, Range: [0, 1023], sf2 direction strength, the larger the value, the more noise reduction*/
    AX_U16                           nSf2DetailStr[AX_ISP_REF_AUTOTBL_Y2DNR_EXPRESS_NUM][AX_ISP_DETAIL_STRENGTH_SIZE];        /* Display: sf2DetailStr, Accuracy: U8.2, Range: [0, 1023], sf2 detail strength, the larger the value, the more noise reduction*/
    AX_U16                           nSf2FlatStr[AX_ISP_REF_AUTOTBL_Y2DNR_EXPRESS_NUM][AX_ISP_FLAT_STRENGTH_SIZE];          /* Display: sf2FlatStr, Accuracy: U8.2, Range: [0, 1023], sf2 flat strength, the larger the value, the more noise reduction*/
    AX_U8                            nEdgeStyleAdjFg[AX_ISP_REF_AUTOTBL_Y2DNR_EXPRESS_NUM];         /* Display: edgeStyleAdjFg, Accuracy: U0.8, Range: [0, 255], edge style adjust for foreground, the larger the value, the smoother the edge, the smaller the vaule, the more annoying pixel along edge*/
    AX_U8                            nDetailStyleAdjFg[AX_ISP_REF_AUTOTBL_Y2DNR_EXPRESS_NUM];       /* Display: detailStyleAdjFg, Accuracy: U0.8, Range: [0, 255], detail style adjust for foreground, the larger the value, maybe the more annoying patterns in detail region*/
    AX_U8                            nFlatStyleAdjFg[AX_ISP_REF_AUTOTBL_Y2DNR_EXPRESS_NUM];         /* Display: flatStyleAdjFg, Accuracy: U0.8, Range: [0, 255], flat style adjust for foreground, the larger the value, maybe the more annoying patterns in flat region*/
    AX_U8                            nEdgeStyleAdjBg[AX_ISP_REF_AUTOTBL_Y2DNR_EXPRESS_NUM];         /* Display: edgeStyleAdjBg, Accuracy: U0.8, Range: [0, 255], edge style adjust for background, the larger the value, the smoother the edge, the smaller the vaule, the more annoying pixel along edge*/
    AX_U8                            nDetailStyleAdjBg[AX_ISP_REF_AUTOTBL_Y2DNR_EXPRESS_NUM];       /* Display: detailStyleAdjBg, Accuracy: U0.8, Range: [0, 255], detail style adjust for background, the larger the value, maybe the more annoying patterns in detail region*/
    AX_U8                            nFlatStyleAdjBg[AX_ISP_REF_AUTOTBL_Y2DNR_EXPRESS_NUM];         /* Display: flatStyleAdjBg, Accuracy: U0.8, Range: [0, 255], flat style adjust for background, the larger the value, maybe the more annoying patterns in flat region*/
    AX_U16                           nLumaGainFg[AX_ISP_REF_AUTOTBL_Y2DNR_EXPRESS_NUM][AX_ISP_LUMA_GAIN_FG_SIZE];         /* Display: lumaGainFg, Accuracy: U9.2, Range: [0, 2047], luma adjust strength for foreground, the larger the value, more noise reduciton in corresponding luma region*/
    AX_U16                           nLumaGainBg[AX_ISP_REF_AUTOTBL_Y2DNR_EXPRESS_NUM][AX_ISP_LUMA_GAIN_BG_SIZE];         /* Display: lumaGainBg, Accuracy: U9.2, Range: [0, 2047], luma adjust strength for background, the larger the value, more noise reduciton in corresponding luma region*/
    AX_U16                           nLumaGainFgEx[AX_ISP_REF_AUTOTBL_Y2DNR_EXPRESS_NUM][AX_ISP_LUMA_GAIN_FG_SIZE];       /* Display: lumaGainFgEx, Accuracy: U3.8, Range: [0, 2047], extension luma adjust strength for foreground, the larger the value, more noise reduciton in corresponding luma region, used when luma_gain_fg is not enough*/
    AX_U16                           nLumaGainBgEx[AX_ISP_REF_AUTOTBL_Y2DNR_EXPRESS_NUM][AX_ISP_LUMA_GAIN_BG_SIZE];       /* Display: lumaGainBgEx, Accuracy: U3.8, Range: [0, 2047], extension luma adjust strength for background, the larger the value, more noise reduciton in corresponding luma region, used when luma_gain_bg is not enough*/
    AX_U8                            nMotionLut[AX_ISP_REF_AUTOTBL_Y2DNR_EXPRESS_NUM][AX_ISP_MOTION_LUT_SIZE];           /* Display: motionLut, Accuracy: U0.8, Range: [0, 255], motion mapping lut*/
} AX_ISP_IQ_Y2DNR_AUTO_T;

typedef struct {
    AX_U8                            nY2dnrEn;                            /* y2dnr enable, Accuracy: U1.0 Range: [0, 1], 0: Disable, 1: Enable */
    AX_U8                            nAutoMode;                           /* for ref auto or manual adjust mode, Accuracy: U1.0 Range:[0, 1], 0: manual, 1:auto, default:1 */
    AX_ISP_IQ_Y2DNR_MANUAL_T         tManualParam;
    AX_ISP_IQ_Y2DNR_AUTO_T           tAutoParam;
} AX_ISP_IQ_Y2DNR_PARAM_EX_T;

/************************************************************************************
 *  ACP IQ Auto Ext Param
 ************************************************************************************/
#define AX_ISP_REF_AUTOTBL_ACP_EXPRESS_NUM                               (4)

typedef struct {
    /* ref_mode: gain/lux */
    AX_U8                            nRefMode;                                                         /* choose ref mode, Accuracy: U1.0 Range: [0, 1], 0:use lux as ref, 1:use gain as ref */
    AX_U8                            nParamGrpNum;                                                     /* Accuracy: U8.0 Range: [1, 16] */
    AX_U32                           nRefVal[AX_ISP_REF_AUTOTBL_ACP_EXPRESS_NUM];                      /* Gain: Accuracy: U22.10 Range: [0x400, 0xFFFFFFFF]; Lux: Accuracy: U22.10 Range: [0, 0xFFFFFFFF] */
    AX_S32                           nHcgShotNoiseCoef[AX_ISP_REF_AUTOTBL_ACP_EXPRESS_NUM][AX_ISP_ACP_NOISE_PARAM_NUM][AX_ISP_ACP_SHOT_NOISE_NUM];             /* Display: hcgShotNoiseCoef, Accuracy: S0.31, Range: [-2147483648, 2147483647], Hcg Calibration Params*/
    AX_S32                           nHcgReadNoiseCoef[AX_ISP_REF_AUTOTBL_ACP_EXPRESS_NUM][AX_ISP_ACP_NOISE_PARAM_NUM][AX_ISP_ACP_READ_NOISE_NUM];             /* Display: hcgReadNoiseCoef, Accuracy: S0.31, Range: [-2147483648, 2147483647], Hcg Calibration Params*/
    AX_S32                           nLcgShotNoiseCoef[AX_ISP_REF_AUTOTBL_ACP_EXPRESS_NUM][AX_ISP_ACP_NOISE_PARAM_NUM][AX_ISP_ACP_SHOT_NOISE_NUM];             /* Display: lcgShotNoiseCoef, Accuracy: S0.31, Range: [-2147483648, 2147483647], Lcg Calibration Params*/
    AX_S32                           nLcgReadNoiseCoef[AX_ISP_REF_AUTOTBL_ACP_EXPRESS_NUM][AX_ISP_ACP_NOISE_PARAM_NUM][AX_ISP_ACP_READ_NOISE_NUM];             /* Display: lcgReadNoiseCoef, Accuracy: S0.31, Range: [-2147483648, 2147483647], Lcg Calibration Params*/
} AX_ISP_IQ_ACP_AUTO_T;
typedef struct {
    AX_U8                            nAutoMode;                           /* for ref auto or manual adjust mode, Accuracy: U1.0 Range:[0, 1], 0: manual, 1:auto, default:1 */
    AX_ISP_IQ_ACP_MANUAL_T           tManualParam;
    AX_ISP_IQ_ACP_AUTO_T             tAutoParam;
} AX_ISP_IQ_ACP_PARAM_EX_T;

/************************************************************************************
 *  ME IQ Ext Param
 ************************************************************************************/

typedef struct {
    AX_U8                            nMeEn;                               /* me enable, Accuracy: U1.0 Range: [0, 1], 0: Disable, 1: Enable */
    AX_ISP_IQ_ME_MANUAL_T            tManualParam;
} AX_ISP_IQ_ME_PARAM_EX_T;

/************************************************************************************
 *  LDC IQ Ext Param
 ************************************************************************************/
typedef struct {
    AX_U8 nLdcEnable;                       /* LDC enable, Accuracy: U8.0 Range: [0, 1] */
    AX_U8 nType;                            /* LDC type, Accuracy: U8.0 Range: [AX_ISP_IQ_LDC_TYPE_V1, AX_ISP_IQ_LDC_TYPE_V2] */
    AX_ISP_IQ_LDC_V1_PARAM_T tLdcV1Param;   /* LDC V1 Param */
    AX_ISP_IQ_LDC_V2_PARAM_T tLdcV2Param;   /* LDC V2 Param */
} AX_ISP_IQ_LDC_PARAM_EX_T;

/************************************************************************************
 *  DIS IQ Ext Param
 ************************************************************************************/
typedef struct {
    AX_U8                            nDisEn;                              /* dis enable, Accuracy: U1.0 Range: [0, 1], 0: Disable, 1: Enable */
    AX_ISP_IQ_DIS_MANUAL_T           tManualParam;
} AX_ISP_IQ_DIS_PARAM_EX_T;

/************************************************************************************
 *  SCENE IQ Ext Param
 ************************************************************************************/
#define AX_ISP_AUTO_SCENE_MAX_NUM (4)

typedef struct {
    AX_U8   nParamGrpNum;                                                       /* Accuracy: U8.0 Range: [0, 4] */
    AX_U32  nDelta;                                                             /* Accuracy: U22.10 Range: [0, 2147483647] <= */
    AX_U32  nRefValStart[AX_ISP_AUTO_SCENE_MAX_NUM];                            /* Accuracy: U22.10 Range: [0, 4294967295] <= */
    AX_U32  nRefValEnd[AX_ISP_AUTO_SCENE_MAX_NUM];                              /* Accuracy: U22.10 Range: [0, 4294967295] <= */
    AX_U8   nAiWorkMode[AX_ISP_AUTO_SCENE_MAX_NUM];                           	/* ainr work mode, mapping reference AX_ISP_AI_WORK_MODE_E, Accuracy: U8.0 Range: [0, 1], 0: AX_AINR_DISABLE, 1: AX_AINR_ENABLE */
} AX_ISP_IQ_SCENE_AUTO_T;

typedef struct {
    AX_U8                      nAutoMode;                                      /* for auto or manual adjust mode. Accuracy: U1.0 Range: [0, 1] */
    AX_ISP_IQ_SCENE_MANUAL_T   tManualParam;
    AX_ISP_IQ_SCENE_AUTO_T     tAutoParam;
} AX_ISP_IQ_SCENE_PARAM_EX_T;

/************************************************************************************
 *  AE_STAT IQ Ext Param
 ************************************************************************************/
typedef struct {
  AX_U8 nEnable[AX_AE_INST_NUM];        /* AE Enable. Accuracy: U1.0 Range: [0, 1] */
  AX_U8 nIspGainEnable;                 /* nIspGainEnable 0: not need * isp_gain. nIspGainEnable 1 : need * isp_gain., Accuracy: U1.0 Range: [0, 1] */
  AX_U8 nSkipNum;                       /* 3A STAT Skip Num. 0:no frame skip, 1:1/2 frame skip for 3a stat, 2:2/3 frame skip for 3a stat, Accuracy: U1.0 Range: [0, 2] */
  AX_ISP_IQ_AE_STAT_POS_T tAeStatPos;   /* ae stat position */
  AX_U8 nGridEnable[AX_AE_GIRD_NUM];    /* Grid Enable. 0: disable, 1: enable, Accuracy: U1.0 Range: [0, 1] */
  AX_U8 nGridMode[AX_AE_GIRD_NUM];      /* Grid Mode. 0: Y(1ch), 1: RGGB(4ch) Accuracy: U1.0 Range: [0, 1] */
  AX_U16 nGridYcoeff[AX_AE_GRID_CHN];   /* Grid Ycoeff(R, Gr, Gb, B).  Accuracy: U0.12 Range: [0, 4095], sum <= 4096 */
  AX_ISP_IQ_AE_STAT_THR_T tGridSatThr[AX_AE_GIRD_NUM];
  AX_ISP_IQ_3A_GRID_ROI_PLACEMT_T tGridRoi[AX_AE_GIRD_NUM];
  AX_U8 nHistEnable;                                                        /* Hist Enable. 0: disable, 1: log hist enable, 2 linear hist enable Accuracy: U2.0 Range: [0, 2] */
  AX_U8 nHistMode;                                                          /* Hist Mode. 0: Y(1ch), 1: YRGB(4ch), 2:RGGB(4ch) Accuracy: U2.0 Range: [0, 2] */
  AX_U8 nHistLinearBinNum;                                                  /* Hist Bin Num. 0: 256, 1: 512, 2:1024, Accuracy: U2.0 Range: [0, 2], 1,2 is only available when nHistMode=0 */
  AX_U16 nHistYcoeff[AX_AE_HIST_CHN];                                       /* Hist Ycoeff(R, Gr, Gb, B).  Accuracy: U0.12 Range: [0, 4095], sum <= 4096 */
  AX_U8 nHistWeight[AX_AE_HIST_WEIGHT_BLK_ROW * AX_AE_HIST_WEIGHT_BLK_COL]; /* Hist Weight, Accuracy: U8.0, Range: [0, 255], 16 x 16 block */
  AX_ISP_IQ_3A_ROI_PLACEMT_T tHistRoi;
  AX_U8 nItemEnable[AX_AE_ITEM_NUM];    /* Item Enable. 0: disable, 1: enable, Accuracy: U1.0 Range: [0, 1] */
  AX_ISP_IQ_3A_ROI_PLACEMT_T tItemRoi[AX_AE_ITEM_NUM];
} AX_ISP_IQ_AE_STAT_PARAM_EX_T;

/************************************************************************************
 *  AWB_STAT IQ Ext Param
 ************************************************************************************/
typedef struct {
  AX_U8 nEnable;                        /* AWB Enable. Accuracy: U1.0 Range: [0, 1] */
  AX_U8 nIspGainEnable;                 /* nIspGainEnable 0: not need * isp_gain. nIspGainEnable 1 : need * isp_gain., Accuracy: U1.0 Range: [0, 1] */
  AX_U8 nSkipNum;                       /* 3A STAT Skip Num. 0:no frame skip, 1:1/2 frame skip for 3a stat, 2:2/3 frame skip for 3a stat, Accuracy: U1.0 Range: [0, 2]  */
  AX_ISP_IQ_AWB_STAT_POS_E eAwbStatPos; /* AWB STAT position. Accuracy: U3.0 Range: [0, 2], 0:after LSC, 1:after RAW2DNR 3: after BPC0 4: after BPC1 */
  AX_U8 nGridEnable;                    /* Grid Enable. 0: disable, 1: enable, Accuracy: U1.0 Range: [0, 1] */
  AX_U8 nGridMode;                      /* AWB GridMode. Accuracy: U2.0 Range: [0, 3], 0:RGB, 1:RGGB, 2: RGGB*Luma4ch, 3:RGGB*Luma2ch  */
  AX_U16 nGridYcoeff[AX_AWB_GRID_CHN];  /* AWB Grid Ycoeff(R, Gr, Gb, B).  Accuracy: U0.12 Range: [0, 4095], sum <= 4096 */
  AX_ISP_IQ_3A_GRID_ROI_PLACEMT_T tGridRoi;
  AX_ISP_IQ_AWB_STAT_THR_T tGridSatThr;
  AX_ISP_IQ_AWB_VALID_THR_T tGridValidThr;
  AX_U8 nItemEnable[AX_AWB_ITEM_NUM];   /* Item Enable. 0: disable, 1: enable, Accuracy: U1.0 Range: [0, 1] */
  AX_ISP_IQ_3A_ROI_PLACEMT_T tItemRoi[AX_AWB_ITEM_NUM];
} AX_ISP_IQ_AWB_STAT_PARAM_EX_T;

/************************************************************************************
 *  AF_STAT IQ Ext Param
 ************************************************************************************/
typedef struct {
  AX_U8 nAfEnable;                            /* AF Enable. Accuracy: U1.0 Range: [0, 1] */
  AX_ISP_IQ_AF_STAT_POS_E eAfStatPos;         /* af stat position. 0: after itp_rltm, 1: after itp_wbc, 2: after itp_hdr, 3: after itp_bpc0, 4: after itp_bpc1, Accuracy: U3.0 Range: [0, 4] */
  AX_ISP_IQ_3A_GRID_ROI_PLACEMT_T tAfGridRoi;
  AX_ISP_IQ_AF_MODE_T tAfMode;
  AX_ISP_IQ_AF_BAYER2Y_T tAfBayer2Y;
  AX_ISP_IQ_AF_DOWNSCALE_T tAfScaler;
  AX_ISP_IQ_AF_CORING_T tAfCoring;
  AX_ISP_IQ_AF_LDG_T tAfLdg;
  AX_U16 nAfHighLumaCntThr[AX_AF_STAT_NUM];     /* Accuracy: U8.4, Range: [0, 4095] */
  AX_ISP_IQ_AF_DRC_T tAfDrc;
  AX_U8 nAfFVEnable[AX_AF_STAT_NUM];                               /* 0: disable, 1: enable, Accuracy: U1.0 Range: [0, 1] for V1, V2, H1, H2*/
  AX_S16 nAfVertIirCoef[AX_AF_VSTAT_NUM][AX_AF_IIR_COEF_NUM];       /* Accuracy: S2.12, Range: [-16384, 16383] */
  AX_S16 nAfHoriIirCoef[AX_AF_HSTAT_NUM][AX_AF_IIR_COEF_NUM];       /* Accuracy: S2.12, Range: [-16384, 16383] */
  AX_U8 nAfHoriFirEnable[AX_AF_HSTAT_NUM];                          /* 0: disable, 1: enable, Accuracy: U1.0 Range: [0, 1] for H1, H2*/
  AX_S8 nAfHoriFirCoef[AX_AF_HSTAT_NUM][AX_AF_FIR_COEF_NUM];        /* Accuracy: S6, Range: [-64, 63] */
} AX_ISP_IQ_AF_STAT_PARAM_EX_T;


/************************************************************************************
 *  ISP IQ API EX
 ***********************************************************************************/

AX_S32 AX_ISP_IQ_SetBlcParamEx(AX_U8 nPipeId, AX_ISP_IQ_BLC_PARAM_EX_T *pIspBlcParam);
AX_S32 AX_ISP_IQ_GetBlcParamEx(AX_U8 nPipeId, AX_ISP_IQ_BLC_PARAM_EX_T *pIspBlcParam);

AX_S32 AX_ISP_IQ_SetLinParamEx(AX_U8 nPipeId, AX_ISP_IQ_LIN_PARAM_EX_T *pIspLinParam);
AX_S32 AX_ISP_IQ_GetLinParamEx(AX_U8 nPipeId, AX_ISP_IQ_LIN_PARAM_EX_T *pIspLinParam);

AX_S32 AX_ISP_IQ_SetDpcParamEx(AX_U8 nPipeId, AX_ISP_IQ_DPC_PARAM_EX_T *pIspDpcParam);
AX_S32 AX_ISP_IQ_GetDpcParamEx(AX_U8 nPipeId, AX_ISP_IQ_DPC_PARAM_EX_T *pIspDpcParam);

AX_S32 AX_ISP_IQ_SetHdrParamEx(AX_U8 nPipeId, AX_ISP_IQ_HDR_PARAM_EX_T *pIspHdrParam);
AX_S32 AX_ISP_IQ_GetHdrParamEx(AX_U8 nPipeId, AX_ISP_IQ_HDR_PARAM_EX_T *pIspHdrParam);

AX_S32 AX_ISP_IQ_SetAinrParamEx(AX_U8 nPipeId, AX_ISP_IQ_AINR_PARAM_EX_T *pIspAinrParam);
AX_S32 AX_ISP_IQ_GetAinrParamEx(AX_U8 nPipeId, AX_ISP_IQ_AINR_PARAM_EX_T *pIspAinrParam);

AX_S32 AX_ISP_IQ_SetRaw2dnrParamEx(AX_U8 nPipeId, AX_ISP_IQ_RAW2DNR_PARAM_EX_T *pIspRaw2dnrParam);
AX_S32 AX_ISP_IQ_GetRaw2dnrParamEx(AX_U8 nPipeId, AX_ISP_IQ_RAW2DNR_PARAM_EX_T *pIspRaw2dnrParam);

AX_S32 AX_ISP_IQ_SetLscParamEx(AX_U8 nPipeId, AX_ISP_IQ_LSC_PARAM_EX_T *pIspLscParam);
AX_S32 AX_ISP_IQ_GetLscParamEx(AX_U8 nPipeId, AX_ISP_IQ_LSC_PARAM_EX_T *pIspLscParam);

AX_S32 AX_ISP_IQ_SetWbcParamEx(AX_U8 nPipeId, AX_ISP_IQ_WBC_PARAM_EX_T *pIspWbcParam);
AX_S32 AX_ISP_IQ_GetWbcParamEx(AX_U8 nPipeId, AX_ISP_IQ_WBC_PARAM_EX_T *pIspWbcParam);

AX_S32 AX_ISP_IQ_SetRltmParamEx(AX_U8 nPipeId, AX_ISP_IQ_RLTM_PARAM_EX_T *pIspRltmParam);
AX_S32 AX_ISP_IQ_GetRltmParamEx(AX_U8 nPipeId, AX_ISP_IQ_RLTM_PARAM_EX_T *pIspRltmParam);

AX_S32 AX_ISP_IQ_SetDemosaicParamEx(AX_U8 nPipeId, AX_ISP_IQ_DEMOSAIC_PARAM_EX_T *pIspDemosaicParam);
AX_S32 AX_ISP_IQ_GetDemosaicParamEx(AX_U8 nPipeId, AX_ISP_IQ_DEMOSAIC_PARAM_EX_T *pIspDemosaicParam);

AX_S32 AX_ISP_IQ_SetFccParamEx(AX_U8 nPipeId, AX_ISP_IQ_FCC_PARAM_EX_T *pIspFccParam);
AX_S32 AX_ISP_IQ_GetFccParamEx(AX_U8 nPipeId, AX_ISP_IQ_FCC_PARAM_EX_T *pIspFccParam);

AX_S32 AX_ISP_IQ_SetGicParamEx(AX_U8 nPipeId, AX_ISP_IQ_GIC_PARAM_EX_T *pIspGicParam);
AX_S32 AX_ISP_IQ_GetGicParamEx(AX_U8 nPipeId, AX_ISP_IQ_GIC_PARAM_EX_T *pIspGicParam);

AX_S32 AX_ISP_IQ_SetEdgeEnhanceParamEx(AX_U8 nPipeId, AX_ISP_IQ_EDGE_ENHANCE_PARAM_EX_T *pIspEdgeEnhanceParam);
AX_S32 AX_ISP_IQ_GetEdgeEnhanceParamEx(AX_U8 nPipeId, AX_ISP_IQ_EDGE_ENHANCE_PARAM_EX_T *pIspEdgeEnhanceParam);

AX_S32 AX_ISP_IQ_SetCcParamEx(AX_U8 nPipeId, AX_ISP_IQ_CC_PARAM_EX_T *pIspCcParam);
AX_S32 AX_ISP_IQ_GetCcParamEx(AX_U8 nPipeId, AX_ISP_IQ_CC_PARAM_EX_T *pIspCcParam);

AX_S32 AX_ISP_IQ_SetGammaParamEx(AX_U8 nPipeId, AX_ISP_IQ_GAMMA_PARAM_EX_T *pIspGammaParam);
AX_S32 AX_ISP_IQ_GetGammaParamEx(AX_U8 nPipeId, AX_ISP_IQ_GAMMA_PARAM_EX_T *pIspGammaParam);

AX_S32 AX_ISP_IQ_SetDehazeParamEx(AX_U8 nPipeId, AX_ISP_IQ_DEHAZE_PARAM_EX_T *pIspDehazeParam);
AX_S32 AX_ISP_IQ_GetDehazeParamEx(AX_U8 nPipeId, AX_ISP_IQ_DEHAZE_PARAM_EX_T *pIspDehazeParam);

AX_S32 AX_ISP_IQ_SetCscParamEx(AX_U8 nPipeId, AX_ISP_IQ_CSC_PARAM_EX_T *pIspCscParam);
AX_S32 AX_ISP_IQ_GetCscParamEx(AX_U8 nPipeId, AX_ISP_IQ_CSC_PARAM_EX_T *pIspCscParam);

AX_S32 AX_ISP_IQ_SetYuv3dnrParamEx(AX_U8 nPipeId, AX_ISP_IQ_YUV3DNR_PARAM_EX_T *pIspYuv3dnrParam);
AX_S32 AX_ISP_IQ_GetYuv3dnrParamEx(AX_U8 nPipeId, AX_ISP_IQ_YUV3DNR_PARAM_EX_T *pIspYuv3dnrParam);

AX_S32 AX_ISP_IQ_SetSharpenParamEx(AX_U8 nPipeId, AX_ISP_IQ_SHARPEN_PARAM_EX_T *pIspSharpenParam);
AX_S32 AX_ISP_IQ_GetSharpenParamEx(AX_U8 nPipeId, AX_ISP_IQ_SHARPEN_PARAM_EX_T *pIspSharpenParam);

AX_S32 AX_ISP_IQ_SetDepurpleParamEx(AX_U8 nPipeId, AX_ISP_IQ_DEPURPLE_PARAM_EX_T *pIspDepurpleParam);
AX_S32 AX_ISP_IQ_GetDepurpleParamEx(AX_U8 nPipeId, AX_ISP_IQ_DEPURPLE_PARAM_EX_T *pIspDepurpleParam);

AX_S32 AX_ISP_IQ_SetHs2dlutParamEx(AX_U8 nPipeId, AX_ISP_IQ_HS2DLUT_PARAM_EX_T *pIspHs2dlutParam);
AX_S32 AX_ISP_IQ_GetHs2dlutParamEx(AX_U8 nPipeId, AX_ISP_IQ_HS2DLUT_PARAM_EX_T *pIspHs2dlutParam);

AX_S32 AX_ISP_IQ_SetCcmpParamEx(AX_U8 nPipeId, AX_ISP_IQ_CCMP_PARAM_EX_T *pIspCcmpParam);
AX_S32 AX_ISP_IQ_GetCcmpParamEx(AX_U8 nPipeId, AX_ISP_IQ_CCMP_PARAM_EX_T *pIspCcmpParam);

AX_S32 AX_ISP_IQ_SetScmParamEx(AX_U8 nPipeId, AX_ISP_IQ_SCM_PARAM_EX_T *pIspScmParam);
AX_S32 AX_ISP_IQ_GetScmParamEx(AX_U8 nPipeId, AX_ISP_IQ_SCM_PARAM_EX_T *pIspScmParam);

AX_S32 AX_ISP_IQ_SetYcprocParamEx(AX_U8 nPipeId, AX_ISP_IQ_YCPROC_PARAM_EX_T *pIspYcprocParam);
AX_S32 AX_ISP_IQ_GetYcprocParamEx(AX_U8 nPipeId, AX_ISP_IQ_YCPROC_PARAM_EX_T *pIspYcprocParam);

AX_S32 AX_ISP_IQ_SetY2dnrParamEx(AX_U8 nPipeId, AX_ISP_IQ_Y2DNR_PARAM_EX_T *pIspY2dnrParam);
AX_S32 AX_ISP_IQ_GetY2dnrParamEx(AX_U8 nPipeId, AX_ISP_IQ_Y2DNR_PARAM_EX_T *pIspY2dnrParam);

AX_S32 AX_ISP_IQ_SetAcpParamEx(AX_U8 nPipeId, AX_ISP_IQ_ACP_PARAM_EX_T *pIspAcpParam);
AX_S32 AX_ISP_IQ_GetAcpParamEx(AX_U8 nPipeId, AX_ISP_IQ_ACP_PARAM_EX_T *pIspAcpParam);

AX_S32 AX_ISP_IQ_SetMeParamEx(AX_U8 nPipeId, AX_ISP_IQ_ME_PARAM_EX_T *pIspMeParam);
AX_S32 AX_ISP_IQ_GetMeParamEx(AX_U8 nPipeId, AX_ISP_IQ_ME_PARAM_EX_T *pIspMeParam);

AX_S32 AX_ISP_IQ_SetLdcParamEx(AX_U8 nPipeId, AX_ISP_IQ_LDC_PARAM_EX_T *pIspLdcParam);
AX_S32 AX_ISP_IQ_GetLdcParamEx(AX_U8 nPipeId, AX_ISP_IQ_LDC_PARAM_EX_T *pIspLdcParam);

AX_S32 AX_ISP_IQ_SetDisParamEx(AX_U8 nPipeId, AX_ISP_IQ_DIS_PARAM_EX_T *pIspDisParam);
AX_S32 AX_ISP_IQ_GetDisParamEx(AX_U8 nPipeId, AX_ISP_IQ_DIS_PARAM_EX_T *pIspDisParam);

AX_S32 AX_ISP_IQ_SetSceneParamEx(AX_U8 nPipeId, AX_ISP_IQ_SCENE_PARAM_EX_T *pIspSceneParam);
AX_S32 AX_ISP_IQ_GetSceneParamEx(AX_U8 nPipeId, AX_ISP_IQ_SCENE_PARAM_EX_T *pIspSceneParam);

AX_S32 AX_ISP_IQ_SetAeStatParamEx(AX_U8 nPipeId, AX_ISP_IQ_AE_STAT_PARAM_EX_T *pAeStatParam);
AX_S32 AX_ISP_IQ_GetAeStatParamEx(AX_U8 nPipeId, AX_ISP_IQ_AE_STAT_PARAM_EX_T *pAeStatParam);

AX_S32 AX_ISP_IQ_SetAwbStatParamEx(AX_U8 nPipeId, AX_ISP_IQ_AWB_STAT_PARAM_EX_T *pAwbStatParam);
AX_S32 AX_ISP_IQ_GetAwbStatParamEx(AX_U8 nPipeId, AX_ISP_IQ_AWB_STAT_PARAM_EX_T *pAwbStatParam);

AX_S32 AX_ISP_IQ_SetAfStatParamEx(AX_U8 nPipeId, AX_ISP_IQ_AF_STAT_PARAM_EX_T *pAfStatParam);
AX_S32 AX_ISP_IQ_GetAfStatParamEx(AX_U8 nPipeId, AX_ISP_IQ_AF_STAT_PARAM_EX_T *pAfStatParam);

#ifdef __cplusplus
}
#endif
#endif // __AX_AUTOKIT_IQ_API_H__
