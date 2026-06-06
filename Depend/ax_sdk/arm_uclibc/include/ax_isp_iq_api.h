/**************************************************************************************************
 *
 * Copyright (c) 2019-2025 Axera Semiconductor Co., Ltd. All Rights Reserved.
 *
 * This source file is the property of Axera Semiconductor Co., Ltd. and
 * may not be copied or distributed in any isomorphic form without the prior
 * written consent of Axera Semiconductor Co., Ltd.
 *
 **************************************************************************************************/
// clang-format off

#ifndef __AX_ISP_IQ_API_H__
#define __AX_ISP_IQ_API_H__

#include "ax_base_type.h"
#include "ax_global_type.h"
#include "ax_isp_common.h"

#ifdef __cplusplus
extern "C"
{
#endif

#define AX_ISP_MAX_PATH_SIZE                        (256)
#define AX_ISP_AUTO_TABLE_MAX_NUM                   (16)
#define AX_ISP_GAIN_GRP_NUM                         (16)
#define AX_ISP_EXPOSE_TIME_GRP_NUM                  (10)
#define AX_ISP_BAYER_CHN_NUM                        (4)

#define AX_ISP_REF_AUTOTBL_RAW2DNR_EXPRESS_NUM      (12)
#define AX_ISP_REF_AUTOTBL_YUV2DNR_EXPRESS_NUM      (12)
#define AX_ISP_HDR_RATIO_GRP_NUM                    (4)

/************************************************************************************
 *  BLC/BL_DET IQ Param: SBL
 ************************************************************************************/
#define AX_ISP_BLC_DEV_CHN_NUM                      (2)

typedef struct {
    AX_U32                           nDpcDetTh;                           /* Display: bpcDetTh, Accuracy: U8.10, Range: [0, 262143], threshold for quickly checking whether the current point is a bad point*/
    AX_U32                           nSblVal[AX_ISP_BAYER_CHN_NUM];       /* Display: sblVal, Accuracy: U8.10, Range: [0, 262143], auto param, interpolation based on exposure time and gain*/
    AX_S32                           nGblVal[AX_ISP_BAYER_CHN_NUM];       /* Display: gblVal, Accuracy: S7.10, Range: [-131072, 131071], auto param, interpolation based on exposure time and gain*/
} AX_ISP_IQ_BLC_MANUAL_T;

typedef struct {
    AX_U8                            nBlcEn;                              /* blc enable, Accuracy: U1.0 Range: [0, 1], 0: Disable, 1: Enable */
    AX_U8                            nSblEnable;                          /* Display: sblCorrEnable, Accuracy: U1.0, Range: [0, 1], sbl corr enable*/
    AX_U8                            nGblEnable;                          /* Display: gblCorrEnable, Accuracy: U1.0, Range: [0, 1], gbl corr enable*/
    AX_U8                            nDetEnable;                          /* Display: detEnable, Accuracy: U1.0, Range: [0, 1], det enable, 0: disable, 1: enable*/
    AX_ISP_IQ_BLC_MANUAL_T           tManualParam[AX_ISP_BLC_DEV_CHN_NUM];
} AX_ISP_IQ_BLC_PARAM_T;

/************************************************************************************
 *  LIN IQ Param: Lin
 ************************************************************************************/
#define AX_ISP_LIN_LUT_NUM                          (16)

typedef struct {
    AX_U16                           nLinLutX[AX_ISP_LIN_LUT_NUM];                          /* Display: linLutX, Accuracy: U8.4, Range: [16, 4095], x coordinate of lin lut, assume there is a leading zero point by default*/
    AX_U16                           nLinLutY[AX_ISP_LIN_LUT_NUM][AX_ISP_BAYER_CHN_NUM];    /* Display: linLutY, Accuracy: U8.6, Range: [64, 16383], y coordinate of lin lut, assume there is a leading zero point by default*/
} AX_ISP_IQ_LIN_MANUAL_T;

typedef struct {
    AX_U8                            nLinEn;                               /* Accuracy: U1.0, Range: [0, 1], 0: disable, 1: enable*/
    AX_ISP_IQ_LIN_MANUAL_T           tManualParam;
} AX_ISP_IQ_LIN_PARAM_T;

/************************************************************************************
 *  DPC IQ Param
 ************************************************************************************/
#define AX_ISP_DPC_STATIC_BUFFER_ARR_NUM        (1024)

 typedef struct {
    AX_U16                           nStaticDpcLength;                    /* Display: staticDpcLength, Accuracy: U16.0, Range: [0, 4096], static defect pixel storage length*/
    AX_U32                           nStaticDpcBuffer[AX_ISP_DPC_STATIC_BUFFER_ARR_NUM];              /* Display: staticDpcBuffer, Accuracy: U32.0, Range: [0, 4294967295], static defect pixel coordinates info*/
    AX_U16                           nNoiseRatio;                         /* Display: noiseRatio, Accuracy: U4.10, Range: [0, 16383], noise level gain, the larger the value, the larger the noise level, less defect pixels*/
    AX_U8                            nPredetEnable;                       /* Display: predetEnable, Accuracy: U1.0, Range: [0, 1], 0:disable predet, 1:enable predet*/
    AX_U8                            nPredetLevelSlope;                   /* Display: predetLevelSlope, Accuracy: U0.4, Range: [0, 15], defect pixel margin gain for quick detection, the larger the value, less defect pixel detected*/
    AX_U16                           nPredetLevelOffset;                  /* Display: predetLevelOffset, Accuracy: U8.6, Range: [0, 16383], defect pixel base margin for quick detection, the larger the value, less defect pixel detected*/
    AX_U16                           nPredetLevelMax;                     /* Display: predetLevelMax, Accuracy: U8.6, Range: [0, 16383], defect pixel max margin for quick detection, the larger the value, less defect pixel detected*/
    AX_U8                            nStaticDpcEnable;                    /* Display: staticDpcEnable, Accuracy: U1.0, Range: [0, 1], 0:disable static dpc, 1:enable static dpc*/
    AX_U8                            nDynamicDpcEnable;                   /* Display: dynamicDpcEnable, Accuracy: U1.0, Range: [0, 1], 0:disable dynamic dpc, 1:enable dynamic dpc*/
    AX_U8                            nColorLimitEnable;                   /* Display: colorLimitEnable, Accuracy: U1.0, Range: [0, 1], 0:disable color limit, 1:enable color limit*/
    AX_U8                            nDefectivePixelType;                 /* Display: defectivePixelType, Accuracy: U1.0, Range: [0, 1], Dynamic dp Detect Module, 0:single defective pixel mode, 1:dual defective pixel mode*/
    AX_U8                            nDetFineStr;                         /* Display: detFineStr, Accuracy: U0.6, Range: [0, 63], local detction strength for defect pixel detection, the larger the value, more defect pixels will be detected*/
    AX_U8                            nDetCoarseStr;                       /* Display: detCoarseStr, Accuracy: U4.4, Range: [0, 255], defect pixel detection threshold, the larger the value, more defect pixels are detected*/
    AX_U16                           nDynamicDpcStr;                      /* Display: dynamicDpcStr, Accuracy: U4.8, Range: [0, 4095], the larger the value, the larger the defect pixel probability*/
    AX_U8                            nEdgeStr;                            /* Display: edgeStr, Accuracy: U1.7, Range: [0, 255], direction interpolation ratio used in interpolation, the larger the value, more direction interpolated result used in interpolation, better performance for defective pixels at edge locations*/
    AX_U8                            nHotColdTypeStr;                     /* Display: hotColdTypeStr, Accuracy: U1.7, Range: [0, 128], hot_cold result ratio, the larger the value, more hot_cold result used*/
    AX_U8                            nSupWinkThr;                         /* Display: supWinkThr, Accuracy: U4.4, Range: [0, 255], the threshold to use direction interpoation or local avg interpolation result*/
    AX_U16                           nDynamicDpUpperlimit;                /* Display: dynamicDpUpperlimit, Accuracy: U8.6, Range: [0, 16383], up margin for color limit, the larger the value, less color limitation*/
    AX_U16                           nDynamicDpLowerlimit;                /* Display: dynamicDpLowerlimit, Accuracy: U8.6, Range: [0, 16383], low margin for color limit, the larger the value, less color limitation*/
    AX_U16                           nStaticDpUpperlimit;                 /* Display: staticDpUpperlimit, Accuracy: U8.6, Range: [0, 16383], up margin for color limit, the larger the value, less color limitation*/
    AX_U16                           nStaticDpLowerlimit;                 /* Display: staticDpLowerlimit, Accuracy: U8.6, Range: [0, 16383], low margin for color limit, the larger the value, less color limitation*/
    AX_U16                           nNormalPixelUpperlimit;              /* Display: normalPixelUpperlimit, Accuracy: U8.6, Range: [0, 16383], up margin for color limit, the larger the value, less color limitation*/
    AX_U16                           nNormalPixelLowerlimit;              /* Display: normalPixelLowerlimit, Accuracy: U8.6, Range: [0, 16383], low margin for color limit, the larger the value, less color limitation*/
} AX_ISP_IQ_DPC_MANUAL_T;

typedef struct {
    AX_U8                            nDpcEn;                              /* dpc enable, Accuracy: U1.0 Range: [0, 1], 0: Disable, 1: Enable */
    AX_ISP_IQ_DPC_MANUAL_T           tManualParam;
} AX_ISP_IQ_DPC_PARAM_T;

/************************************************************************************
 *  HDR IQ Param
 ************************************************************************************/
#define AX_ISP_HDR_EXP_FRAME_NUM            (2)
#define AX_ISP_HDR_OVER_EXP_PROT_THRE_NUM   (2)
#define AX_ISP_HDR_DGST_STREN_LIMIT_NUM     (2)
#define AX_ISP_HDR_EXP_WGT_LUT_NUM          (33)

typedef enum {
    AX_ISP_HDR_NORMAL                = 0,
    AX_ISP_HDR_GENERAL_MASK          = 1,
    AX_ISP_HDR_MOTION_MASK           = 2,
    AX_ISP_HDR_EXPOSURE_MASK_SF      = 3,
    AX_ISP_HDR_EXPOSURE_MASK_LF      = 4,
} AX_ISP_HDR_DEBUG_SHOW_MASK_E;

typedef enum {
    AX_ISP_HDR_SF                    = 0,
    AX_ISP_HDR_LF                    = 1,
} AX_ISP_HDR_DGST_BASE_FID_E;

typedef struct {
    AX_ISP_HDR_DEBUG_SHOW_MASK_E     eDebugShowMask;                      /* Display: debugShowMask, Accuracy: U3.0, Range: [0, 4], 0 - normal; 1 - general mask; 2 - motion mask; 3 - exposure mask sf; 4 - exposure mask lf*/
    AX_U16                           nNoiseLutScale;                      /* Display: noiseLutScale, Accuracy: U4.12, Range: [0, 65535], Noise profile will be multiplied by this param.*/
    AX_U8                            nFusDarkProtEn;                      /* Display: fusDarkProtEn, Accuracy: U1.0, Range: [0, 1], 0 - Disable dark protection feature for normal fusion; 1 - Enable dark protection feature for normal fusion*/
    AX_U16                           nFusOverExpProtThre[AX_ISP_HDR_EXP_FRAME_NUM][AX_ISP_HDR_OVER_EXP_PROT_THRE_NUM];           /* Display: fusOverExpProtThre, Accuracy: U8.8, Range: [0, 65535], fus_over_exp_prot_thre[x] for input frame x; For Luma value from fus_over_exp_prot_thre[x][0] to fus_over_exp_prot_thre[x][1], protection weight will drop from 1 to 0. When weight == 0, corresponding frame will not be used in normal fusion(area that satisfies hdr_ratio). Example: fus_over_exp_prot_thre = [[251, 254], [247, 249]], Luma value larger than 254 in frame0 will NOT be used in fusion and 251~254 is transition range; Luma value larger than 249 in frame1 will NOT be used in fusion and 247~249 is transition range.*/
    AX_U16                           nFusProtWbRatio[AX_ISP_HDR_EXP_FRAME_NUM];                  /* Display: fusProtWbRatio, Accuracy: U1.8, Range: [0, 256], fus_prot_wb_ratio[x] for input frame x. This param controls how much WB gain is considered when calculating Luma value used for over-exposure protection.*/
    AX_ISP_HDR_DGST_BASE_FID_E       eDgstBaseFid;                        /* Display: dgstBaseFid, Accuracy: U1.0, Range: [0, 1], Specify which frame to be used as dgst fusion result when both exp mask is 0 for a pixel. 0 - SF; 1 - LF*/
    AX_U8                            nDgstEnable;                         /* Display: dgstEnable, Accuracy: U1.0, Range: [0, 1], 0 - dgst disable; 1 - dgst enable*/
    AX_U16                           nDgstStrenLimit[AX_ISP_HDR_DGST_STREN_LIMIT_NUM];                  /* Display: dgstStrenLimit, Accuracy: U1.8, Range: [0, 256], Forcibly set motion mask used in dgst to be from dgst_stren_limit[0] to dgst_stren_limit[1]*/
    AX_U16                           nExpWeightGain[AX_ISP_HDR_EXP_FRAME_NUM];                   /* Display: expWeightGain, Accuracy: U1.8, Range: [0, 256], Gain applied on exposure mask before exp mask normalization.*/
    AX_U16                           nExpWeightLutLumaLow[AX_ISP_HDR_EXP_FRAME_NUM][AX_ISP_HDR_EXP_WGT_LUT_NUM];         /* Display: expWeightLutLumaLow, Accuracy: U1.15, Range: [0, 32768], Weight lut of exposure mask for dark area. It's used to generate exposure weight based on pixel luma value.*/
    AX_U16                           nExpWeightLutLumaHigh[AX_ISP_HDR_EXP_FRAME_NUM][AX_ISP_HDR_EXP_WGT_LUT_NUM];        /* Display: expWeightLutLumaHigh, Accuracy: U1.15, Range: [0, 32768], Weight lut of exposure mask for bright area. It's used to generate exposure weight based on pixel luma value.*/
    AX_U16                           nExpYRatio[AX_ISP_HDR_EXP_FRAME_NUM];                       /* Display: expYRatio, Accuracy: U1.8, Range: [0, 256], This param controls how much WB gain is considered when calculating Luma value used for exp mask generation.*/
    AX_U16                           nMotionMaskEnhc;                     /* Display: motionMaskEnhc, Accuracy: U8.8, Range: [0, 65535], This param will enhance motion mask.*/
    AX_U16                           nMotionMaskNoiseLevel;               /* Display: motionMaskNoiseLevel, Accuracy: U1.11, Range: [0, 4095], This parameter acts like motion mask threshold, cutting the motion mask.*/
    AX_U16                           nMotionMaskContinuity;               /* Display: motionMaskContinuity, Accuracy: U1.8, Range: [0, 256], The higher this parameter is, the more continuous(slightly) motion mask can be. Can be helpful for some scenes.*/
} AX_ISP_IQ_HDR_MANUAL_T;

typedef struct {
    AX_U8                            nHdrEn;                              /* hdr enable, Accuracy: U1.0 Range: [0, 1], 0: Disable, 1: Enable */
    AX_ISP_IQ_HDR_MANUAL_T           tManualParam;
} AX_ISP_IQ_HDR_PARAM_T;

#define AX_ISP_AINR_MODEL_LIST_NUM_MAX              (6)
#define AX_ISP_AINR_MODEL_PATH_SIZE_MAX             (256)
#define AX_ISP_AINR_BIAS_OUT_NUM_MAX                (4)
#define AX_ISP_AINR_LUT_NUM_MAX                     (17)
#define AX_ISP_AINR_LUT_SIZE_MAX                    (2)
#define AX_ISP_AINR_SUBMODEL_LIST_MAX_NUM           (12)
#define AX_ISP_AINR_SUBMODEL_NAME_SIZE_MAX          (128)

/************************************************************************************
 *  AINR IQ Param
 ************************************************************************************/
typedef struct {
    AX_U8                            nModelListNum;                       /* Display: modelListNum, Accuracy: U8.0, Range: [0, 6], model_list_num*/
    AX_CHAR                          szModelListPath[AX_ISP_AINR_MODEL_LIST_NUM_MAX][AX_ISP_AINR_MODEL_PATH_SIZE_MAX];             /* Display: modelListPath, model_list_path */
    AX_CHAR                          szCurrModelPath[AX_ISP_AINR_MODEL_PATH_SIZE_MAX];                /* Display: currModelPath, curr_model_path */
    AX_CHAR                          szCurrTemporalNrName[AX_ISP_AINR_MODEL_PATH_SIZE_MAX];           /* Display: currTemporalNrName, curr_temporal_nr_name */
    AX_CHAR                          szCurrSpatialNrName[AX_ISP_AINR_MODEL_PATH_SIZE_MAX];            /* Display: currSpatialNrName, curr_spatial_nr_name */
    AX_U8                            nNrLevelMode;                        /* Display: nrLevelMode, Accuracy: U1.0, Range: [0, 1], 0: lut fusion, 1: only use nr level*/
    AX_S8                            nTemporalNrLevel;                    /* Display: temporalNrLevel, Accuracy: S7.0, Range: [-127, 127], temporal_nr_level*/
    AX_S8                            nSpatialNrLevel;                     /* Display: spatialNrLevel, Accuracy: S7.0, Range: [-127, 127], spatial_nr_level*/
    AX_S16                           nBiasOut[AX_ISP_AINR_BIAS_OUT_NUM_MAX];                         /* Display: biasOut, Accuracy: S9.6, Range: [-32768, 32767], bias_out*/
    AX_U16                           nOffset2d[AX_ISP_AINR_BIAS_OUT_NUM_MAX];                        /* Display: offset2d, Accuracy: U16.0, Range: [0, 65535], offset_2d*/
    AX_U16                           nOffset3d[AX_ISP_AINR_BIAS_OUT_NUM_MAX];                        /* Display: offset3d, Accuracy: U16.0, Range: [0, 65535], offset_3d*/
    AX_U8                            nLutTemporalBlend[AX_ISP_AINR_LUT_NUM_MAX][AX_ISP_AINR_LUT_SIZE_MAX];            /* Display: lutTemporalBlend, Accuracy: U8.0, Range: [0, 255], lut_temporal_blend*/
    AX_U8                            nLutSpatialBlend[AX_ISP_AINR_LUT_NUM_MAX][AX_ISP_AINR_LUT_SIZE_MAX];             /* Display: lutSpatialBlend, Accuracy: U8.0, Range: [0, 255], lut_spatial_blend*/
    AX_U8                            nLutSpatialGuidance[AX_ISP_AINR_LUT_NUM_MAX][AX_ISP_AINR_LUT_SIZE_MAX];          /* Display: lutSpatialGuidance, Accuracy: U8.0, Range: [0, 255], lut_spatial_guidance*/
    AX_U8                            nLutHdrMask[AX_ISP_AINR_LUT_NUM_MAX][AX_ISP_AINR_LUT_SIZE_MAX];                  /* Display: lutHdrMask, Accuracy: U8.0, Range: [0, 255], lut_hdr_mask*/
    AX_U8                            nLutExtTemporal[AX_ISP_AINR_LUT_NUM_MAX][AX_ISP_AINR_LUT_SIZE_MAX];              /* Display: lutExtTemporal, Accuracy: U8.0, Range: [0, 255], lut_ext_temporal*/
    AX_U8                            nLutHighFreq[AX_ISP_AINR_LUT_NUM_MAX][AX_ISP_AINR_LUT_SIZE_MAX];                 /* Display: lutHighFreq, Accuracy: U8.0, Range: [0, 255], lut_high_frequency*/
} AX_ISP_IQ_AINR_MANUAL_T;

typedef struct {
    AX_U8                            nAinrEn;                             /* ainr enable, Accuracy: U1.0 Range: [0, 1], 0: Disable, 1: Enable */
    AX_ISP_IQ_AINR_MANUAL_T          tManualParam;
} AX_ISP_IQ_AINR_PARAM_T;

typedef struct {
    AX_CHAR                          szModelPath[AX_ISP_AINR_MODEL_PATH_SIZE_MAX];                    /* Accuracy: U8.0 Range: [0, 255], szModelPath */
    AX_U8                            nSuppBiasOut;                        /* Accuracy: U1.0 Range: [0, 1], nSuppBiasOut */
    AX_U8                            nSuppOffset3D;                       /* Accuracy: U1.0 Range: [0, 1], nSuppOffset3D */
    AX_U8                            nSuppOffset2D;                       /* Accuracy: U1.0 Range: [0, 1], nSuppOffset2D */
    AX_U8                            nSuppLutTemporalBlend;               /* Accuracy: U1.0 Range: [0, 1], nSuppLutTemporalBlend */
    AX_U8                            nSuppLutSpatialBlend;                /* Accuracy: U1.0 Range: [0, 1], nSuppLutSpatialBlend */
    AX_U8                            nSuppLutSpatialGuidance;             /* Accuracy: U1.0 Range: [0, 1], nSuppLutSpatialGuidance */
    AX_U8                            nSuppLutHdrMask;                     /* Accuracy: U1.0 Range: [0, 1], nSuppLutHdrMask */
    AX_U8                            nSuppLutExtTemporal;                 /* Accuracy: U1.0 Range: [0, 1], nSuppLutExtTemporal */
    AX_U8                            nSuppLutHighFreq;                    /* Accuracy: U1.0 Range: [0, 1], nSuppLutHighFreq */
    AX_U8                            nTemporalBaseNrValidNum;             /* Accuracy: U8.0 Range: [0, 12], nTemporalBaseNrValidNum */
    AX_CHAR                          szTemporalBaseNrList[AX_ISP_AINR_SUBMODEL_LIST_MAX_NUM][AX_ISP_AINR_SUBMODEL_NAME_SIZE_MAX];       /* Accuracy: U8.0 Range: [0, 255], szTemporalBaseNrList */
    AX_U8                            nSpatialBaseNrValidNum;              /* Accuracy: U8.0 Range: [0, 12], nSpatialBaseNrValidNum */
    AX_CHAR                          szSpatialBaseNrList[AX_ISP_AINR_SUBMODEL_LIST_MAX_NUM][AX_ISP_AINR_SUBMODEL_NAME_SIZE_MAX];        /* Accuracy: U8.0 Range: [0, 255], szSpatialBaseNrList */
} AX_ISP_IQ_AINR_CAP_TABLE_T;

typedef struct {
    AX_U8                            nValidNum;                           /* Accuracy: U8.0 Range: [0, 6], valid_num */
    AX_ISP_IQ_AINR_CAP_TABLE_T       tModelCapList[AX_ISP_AINR_MODEL_LIST_NUM_MAX];
} AX_ISP_IQ_AINR_CAPABILITY_T;

/************************************************************************************
 *  RAW2DNR IQ Param
 ************************************************************************************/
#define AX_ISP_RAW2DNR_CORING_LIMIT_NUM      (2)
#define AX_ISP_RAW2DNR_SETTING_NUM           (2)
#define AX_ISP_RAW2DNR_RGB_CHN_NUM           (3)
#define AX_ISP_RAW2DNR_LUT_SIZE              (17)

typedef struct {
    AX_U8                            nMaskThre[AX_ISP_RAW2DNR_CORING_LIMIT_NUM];                        /* Display: maskThre, Accuracy: U0.8, Range: [0, 255], mask threshold for coring*/
    AX_U8                            nMaskLimit[AX_ISP_RAW2DNR_CORING_LIMIT_NUM];                       /* Display: maskLimit, Accuracy: U0.8, Range: [0, 255], mask limit for coring*/
    AX_U8                            nEdgePreserveRatio;                  /* Display: edgePreserveRatio, Accuracy: U0.8, Range: [0, 255], edge_preserve_ratio for freq separation*/
    AX_U8                            nInterGStr;                          /* Display: interGStr, Accuracy: U0.8, Range: [0, 255], inter_g_strength for freq separation*/
    AX_U8                            nHfNrEnable;                         /* Display: hfNrEnable, Accuracy: U1.0, Range: [0, 1], hf noise reduction enable*/
    AX_U8                            nHfNrStren[AX_ISP_RAW2DNR_SETTING_NUM];                       /* Display: hfNrStren, Accuracy: U0.8, Range: [0, 255], hf noise reduction strength*/
    AX_U8                            nHfNrStrenRgb[AX_ISP_RAW2DNR_SETTING_NUM][AX_ISP_RAW2DNR_RGB_CHN_NUM];                 /* Display: hfNrStrenRgb, Accuracy: U0.8, Range: [0, 255], hf noise reduction strength for rgb channel*/
    AX_U16                           nWienerFactor[AX_ISP_RAW2DNR_SETTING_NUM][AX_ISP_RAW2DNR_LUT_SIZE];                /* Display: wienerFactor, Accuracy: U6.4, Range: [0, 1023], hf noise reduction wiener factor for different brightness*/
    AX_U8                            nMfNrEnable;                         /* Display: mfNrEnable, Accuracy: U1.0, Range: [0, 1], mf noise reduction enable*/
    AX_U8                            nMfNrStren[AX_ISP_RAW2DNR_SETTING_NUM];                       /* Display: mfNrStren, Accuracy: U0.8, Range: [0, 255], mf noise reduction strength*/
    AX_U8                            nMfNrStrenRgb[AX_ISP_RAW2DNR_SETTING_NUM][AX_ISP_RAW2DNR_RGB_CHN_NUM];                 /* Display: mfNrStrenRgb, Accuracy: U0.8, Range: [0, 255], mf noise reduction strength for rgb channel*/
    AX_U16                           nNlmFactor[AX_ISP_RAW2DNR_SETTING_NUM][AX_ISP_RAW2DNR_LUT_SIZE];                   /* Display: nlmFactor, Accuracy: U4.6, Range: [0, 1023], mf noise reduction nlm factor for different brightness*/
    AX_U8                            nInterChannelStr;                    /* Display: interChannelStr, Accuracy: U0.8, Range: [0, 255], inter_channel_strength for mf nlm*/
    AX_U8                            nLutMode;                            /* Display: lutMode, Accuracy: U8.0, Range: [0, 255], lut_mode for noise_profile. 0: SDR 256, 1: HDR 512, 2: HDR 1024, 3: HDR 2048, 4: HDR 4096, 5: HDR 8192, 6: HDR 16384, 255: AUTO*/
    AX_U8                            nLutType;                            /* Display: lutType, Accuracy: U1.0, Range: [0, 1], lut_type for noise_profile, 0: Linear, 1: Log/Exponential*/
    AX_U16                           nNoiseProfileFactor;                 /* Display: noiseProfileFactor, Accuracy: U3.7, Range: [0, 1023], noise_profile_factor for noise_profile*/
} AX_ISP_IQ_RAW2DNR_MANUAL_T;

typedef struct {
    AX_U8                            nRaw2dnrEn;                          /* raw2dnr enable, Accuracy: U1.0 Range: [0, 1], 0: Disable, 1: Enable */
    AX_ISP_IQ_RAW2DNR_MANUAL_T       tManualParam;
} AX_ISP_IQ_RAW2DNR_PARAM_T;

/************************************************************************************
 *  LSC IQ Param
 ************************************************************************************/
#define AX_ISP_LSC_MESH_SIZE_V              (15)
#define AX_ISP_LSC_MESH_SIZE_H              (19)

typedef struct {
    AX_U8                            nLumaRatio;                          /* Display: lumaRatio, Accuracy: U8.0, Range: [0, 100], luma shading attenuation ratio*/
    AX_U8                            nColorRatio;                         /* Display: colorRatio, Accuracy: U8.0, Range: [0, 100], color shading attenuation ratio*/
    AX_U32                           nLumaMeshLut[AX_ISP_LSC_MESH_SIZE_V][AX_ISP_LSC_MESH_SIZE_H];                /* Display: LumaMeshLut, Accuracy: U4.14, Range: [16384, 262143], luma shading correction mesh lut*/
    AX_U32                           nRRMeshLut[AX_ISP_LSC_MESH_SIZE_V][AX_ISP_LSC_MESH_SIZE_H];                  /* Display: RRMeshLut, Accuracy: U4.14, Range: [16384, 262143], color shading R channel correction mesh lut*/
    AX_U32                           nGRMeshLut[AX_ISP_LSC_MESH_SIZE_V][AX_ISP_LSC_MESH_SIZE_H];                  /* Display: GRMeshLut, Accuracy: U4.14, Range: [16384, 262143], color shading GR channel correction mesh lut*/
    AX_U32                           nGBMeshLut[AX_ISP_LSC_MESH_SIZE_V][AX_ISP_LSC_MESH_SIZE_H];                  /* Display: GBMeshLut, Accuracy: U4.14, Range: [16384, 262143], color shading GB channel correction mesh lut*/
    AX_U32                           nBBMeshLut[AX_ISP_LSC_MESH_SIZE_V][AX_ISP_LSC_MESH_SIZE_H];                  /* Display: BBMeshLut, Accuracy: U4.14, Range: [16384, 262143], color shading B channel correction mesh lut*/
} AX_ISP_IQ_LSC_MANUAL_T;

typedef struct {
    AX_U8                            nLscEn;                              /* lsc enable, Accuracy: U1.0 Range: [0, 1], 0: Disable, 1: Enable */
    AX_ISP_IQ_LSC_MANUAL_T           tManualParam;
} AX_ISP_IQ_LSC_PARAM_T;

/************************************************************************************
 *  WB Gain Info
 ************************************************************************************/
typedef struct {
    AX_U16 nRGain;                                                      /* WBC RGain. Default data:0x200. Accuracy: U4.8 Range: [0, 0xFFF] */
    AX_U16 nGrGain;                                                     /* WBC GrGain. Default data:0x100. Accuracy: U4.8 Range: [0, 0xFFF] */
    AX_U16 nGbGain;                                                     /* WBC GbGain. Default data:0x100. Accuracy: U4.8 Range: [0, 0xFFF] */
    AX_U16 nBGain;                                                      /* WBC BGain. Default data:0x200.Accuracy: U4.8 Range: [0, 0xFFF] */
} AX_ISP_IQ_WBC_MANUAL_T;

typedef struct {
    AX_U8                            nWbcEn;                              /* wbc enable, Accuracy: U1.0 Range: [0, 1], 0: Disable, 1: Enable */
    AX_ISP_IQ_WBC_MANUAL_T           tManualParam;
} AX_ISP_IQ_WBC_PARAM_T;

/************************************************************************************
 *  RLTM IQ Param
 ************************************************************************************/
#define AX_ISP_RLTM_SCURVE_MAX_LEN                      (1025)
#define AX_ISP_RLTM_BASECURVE_MAX_LEN                   (513)
#define AX_ISP_RLTM_DETAIL_STRENGTH_LUT_NUM             (32)
#define AX_ISP_RLTM_DETAIL_STRENGTH_LUT_LOW_NUM         (33)
#define AX_ISP_RLTM_LUMA_WEIGHT_NUM                     (2)

typedef struct {
    AX_U8                            nAlgMode;                            /* Display: Alg Mode, Accuracy: U2.0, Range: [0, 2], 0: effect priority mode  1: balance mode 2: performance mode*/
    AX_U8                            nMultiCamSyncMode;                   /* Display: Multi Cam Sync Mode, Accuracy: U2.0, Range: [0, 2], 0：INDEPEND MODE; 1： MASTER SLAVE MODE; 2: OVERLAP MODE*/
    AX_U8                            nGtmSwEn;                            /* Display: gtmSwDgainEnable, Accuracy: U1.0, Range: [0, 1], enable dgain mode*/
    AX_U16                           nGtmSwDgain;                         /* Display: gtmSwDgain, Accuracy: U8.8, Range: [256, 65535], dgain when sw gain is on*/
    AX_U16                           nAlpha;                              /* Display: alpha, Accuracy: U1.15, Range: [0, 32768], temporal filter strength*/
    AX_U8                            nReset;                              /* Display: reset, Accuracy: U3.0, Range: [0, 7], use current rltm curve only*/
    AX_U8                            nStopUpdating;                       /* Display: stopUpdating, Accuracy: U3.0, Range: [0, 7], stop updating curve*/
    AX_U8                            nDitherMode;                         /* Display: ditherMode, Accuracy: U1.0, Range: [0, 1], dither enable*/
    AX_U16                           nDitherScale;                        /* Display: ditherScale, Accuracy: U8.4, Range: [0, 4095], dither strength*/
    AX_U8                            nLumaWeight[AX_ISP_RLTM_LUMA_WEIGHT_NUM];                      /* Display: lumaWeight, Accuracy: U1.7, Range: [0, 128], weight of luma_max and luma_rgb for luma and luma_detail calculate*/
    AX_U8                            nHighlightSup;                       /* Display: highlightSup, Accuracy: U5.3, Range: [0, 255], highlight suppression*/
    AX_U8                            nPostGamma;                          /* Display: pstGamma, Accuracy: U3.5, Range: [32, 255], pst gamma*/
    AX_U8                            nLocalFactor;                        /* Display: localFactor, Accuracy: U1.7, Range: [0, 128], factor for local hist and global hist*/
    AX_U8                            nRltmStrength;                       /* Display: rltmStrength, Accuracy: U1.7, Range: [0, 128], rltm strength*/
    AX_U8                            nContrastStrength;                   /* Display: contrastStrength, Accuracy: U1.7, Range: [1, 255], contrast strength*/
    AX_U8                            nDarkContrastEnhc[AX_ISP_RLTM_LUMA_WEIGHT_NUM];                /* Display: darkContrastEnhc, Accuracy: U0.8, Range: [0, 255], Max Luma Value & Pixel Num for Contrast Enhance*/
    AX_U16                           nKMax;                               /* Display: kMax, Accuracy: U12.4, Range: [16, 65535], max gain for rltm*/
    AX_U8                            nBaseCurveStrength;                  /* Display: baseCurveStrength, Accuracy: U1.7, Range: [0, 128], rltm base curve strength*/
    AX_U16                           nBaseCurve[AX_ISP_RLTM_BASECURVE_MAX_LEN];                     /* Display: baseCurve, Accuracy: U1.15, Range: [0, 32768], base-curve*/
    AX_U16                           nSCurveList[AX_ISP_RLTM_SCURVE_MAX_LEN];                   /* Display: sCurve, Accuracy: U1.15, Range: [0, 32768], s-curve*/
    AX_U16                           nDetailCoringPos;                    /* Display: detailCoringPos, Accuracy: U3.13, Range: [0, 65535], the coring for high freq pos detail*/
    AX_U16                           nDetailCoringNeg;                    /* Display: detailCoringNeg, Accuracy: U3.13, Range: [0, 65535], the coring for high freq neg detail*/
    AX_U8                            nDetailGainPos;                      /* Display: detailGainPos, Accuracy: U3.5, Range: [0, 255], the gain for high freq pos detail*/
    AX_U8                            nDetailGainNeg;                      /* Display: detailGainNeg, Accuracy: U3.5, Range: [0, 255], the gain for high freq neg detail*/
    AX_U16                           nDetailGainLimitPos;                 /* Display: detailLimitPos, Accuracy: U3.13, Range: [0, 65535], the limit for high freq pos detail*/
    AX_U16                           nDetailGainLimitNeg;                 /* Display: detailLimitNeg, Accuracy: U3.13, Range: [0, 65535], the limit for high freq neg detail*/
    AX_U8                            nDetailExtraStrengthPos;             /* Display: detailExtraStrengthPos, Accuracy: U4.4, Range: [0, 255], the global strength for high freq pos detail*/
    AX_U8                            nDetailExtraStrengthNeg;             /* Display: detailExtraStrengthNeg, Accuracy: U4.4, Range: [0, 255], the global strength for high freq neg detail*/
    AX_U8                            nDetailStrengthLut[AX_ISP_RLTM_DETAIL_STRENGTH_LUT_NUM];              /* Display: detailStrengthLut, Accuracy: U4.4, Range: [0, 255], the strength for high freq detail by detail*/
    AX_U16                           nDetailSigmaDis;                     /* Display: detailSigmaDis, Accuracy: U3.13, Range: [1, 65535], bilateral filter sigma distance*/
    AX_U16                           nDetailSigmaVal;                     /* Display: detailSigmaVal, Accuracy: U3.13, Range: [1, 65535], bilateral filter sigma value*/
    AX_U8                            nDownsampleRatio;                    /* Display: downsampleRatio, Accuracy: U8.0, Range: [0, 1], the downsample ratio of low freq luma*/
    AX_U8                            nRltmDetailLowEn;                    /* Display: detailLowEnable, Accuracy: U8.0, Range: [0, 1], low freq enhance enable*/
    AX_U8                            nDetailGainPosLow;                   /* Display: detailGainPosLow, Accuracy: U3.5, Range: [0, 255], the gain for low freq pos detail*/
    AX_U8                            nDetailGainNegLow;                   /* Display: detailGainNegLow, Accuracy: U3.5, Range: [0, 255], the gain for low freq neg detail*/
    AX_U16                           nDetailLimitPosLow;                  /* Display: detailLimitPosLow, Accuracy: U3.13, Range: [0, 65535], the limit for low freq pos detail*/
    AX_U16                           nDetailLimitNegLow;                  /* Display: detailLimitNegLow, Accuracy: U3.13, Range: [0, 65535], the limit for low freq neg detail*/
    AX_U8                            nDetailStrengthLutPosLow[AX_ISP_RLTM_DETAIL_STRENGTH_LUT_LOW_NUM];        /* Display: detailStrengthLutPosLow, Accuracy: U1.7, Range: [0, 255], the strength for low freq pos detail by luma*/
    AX_U8                            nDetailStrengthLutNegLow[AX_ISP_RLTM_DETAIL_STRENGTH_LUT_LOW_NUM];        /* Display: detailStrengthLutNegLow, Accuracy: U1.7, Range: [0, 255], the strength for low freq neg detail by luma*/
    AX_U8                            nCoeffWinRad;                        /* Display: coeffWinR, Accuracy: U3.0, Range: [0, 5], guided filter win_radius*/
    AX_U16                           nCoeffEps;                           /* Display: coeffEps, Accuracy: U16.0, Range: [0, 65535], guided filter eps*/
    AX_U16                           nSigmaDisPst;                        /* Display: sigmaDisPst, Accuracy: U3.13, Range: [1, 65535], spatial domain weight low freq*/
    AX_U16                           nSigmaValPst;                        /* Display: sigmaValPst, Accuracy: U3.13, Range: [1, 2048], intensity domain weight low freq*/
    AX_U16                           nSigmaDisBlur;                       /* Display: sigmaDisBlur, Accuracy: U3.13, Range: [1, 65535], blur coeff a and b*/
} AX_ISP_IQ_RLTM_MANUAL_T;

typedef struct {
    AX_U8                            nRltmEn;                                   /* rltm enable, Accuracy: U1.0 Range: [0, 1], 0: Disable, 1: Enable */
    AX_ISP_IQ_RLTM_MANUAL_T          tManualParam;
} AX_ISP_IQ_RLTM_PARAM_T;

typedef struct {
    AX_U8                            nShadowPoint;                        /* Accuracy: U0.8 Range: [1, 255], shadow point position */
    AX_U8                            nShadowStrength;                     /* Accuracy: U0.8 Range: [1, 255], shadow point strength */
    AX_U8                            nHighlightPoint;                     /* Accuracy: U0.8 Range: [1, 255], highlight point position */
    AX_U8                            nHighlightStrength;                  /* Accuracy: U0.8 Range: [1, 255], highlight point strength */
    AX_U8                            nShadowControl;                      /* Accuracy: U0.8 Range: [0, 255], shadow control */
    AX_U8                            nMiddleControl;                      /* Accuracy: U0.8 Range: [0, 255], middle control */
    AX_U8                            nHighlightControl;                   /* Accuracy: U0.8 Range: [0, 255], highlight control */
} AX_ISP_IQ_SCURVE_PARAM_T;

typedef struct {
    AX_U8                            nDarkSuppress;                       /* Accuracy: U8.0 Range: [1, 30], dark suppress */
    AX_U8                            nBrightEnhance;                      /* Accuracy: U8.0 Range: [150, 210], bright enhance */
    AX_U8                            nHorizontalStretch;                  /* Accuracy: U8.0 Range: [30, 60], base curve horizontal stretch */
    AX_U8                            nVerticalCompress;                   /* Accuracy: U8.0 Range: [100, 200], base curve vertical compress */
} AX_ISP_IQ_BASECURVE_PARAM_T;

/************************************************************************************
 *  Demosaic IQ Param
 ************************************************************************************/

#define AX_ISP_DEMOSAIC_EDES_LUT_SIZE       (9)

typedef enum {
    AX_ISP_DEMOSAIC_NORMAL           = 0,
    AX_ISP_DEMOSAIC_COPY_TO_GRAY     = 1,
    AX_ISP_DEMOSAIC_COPY_TO_GREEN    = 2,
} AX_ISP_DEMOSAIC_MODE_E;
typedef enum {
    AX_ISP_DEMOSAIC_DISABLE          = 0,
    AX_ISP_DEMOSAIC_WEAK             = 1,
    AX_ISP_DEMOSAIC_MIDDLE           = 2,
    AX_ISP_DEMOSAIC_STRONG           = 3,
} AX_ISP_DEMOSAIC_HF_DIR_EST_ENHC_LEVEL_E;

typedef struct {
    AX_ISP_DEMOSAIC_MODE_E           eMode;                               /* Display: mode, Accuracy: U2.0, Range: [0, 2], 0: normal, 1: copy to gray, 2: copy to green*/
    AX_U8                            nEdgeDirHighFreqSens;                /* Display: edgeDirHighFreqSens, Accuracy: U1.4, Range: [0, 16], bigger value==> more sensitivity to high freq*/
    AX_U16                           nEdgeDirEdgeSensLut[AX_ISP_DEMOSAIC_EDES_LUT_SIZE];              /* Display: edgeDirEdgeSensLut, Accuracy: U4.8, Range: [0, 4095], default inv noise lut. lut[0]:0, [1]:32, [2]:64, ... [8]:256*/
    AX_U8                            nEdgeDirHvHighFreqStr;               /* Display: edgeDirHvHighFreqStr, Accuracy: U2.2, Range: [0, 15], level for adjusting high frequency gradient*/
    AX_U8                            nHvInterpHfCoeff;                    /* Display: hvInterpHfCoeff, Accuracy: U1.5, Range: [0, 32], level for adjusting high frequency gradient*/
    AX_U8                            nPnInterpHfCoeff;                    /* Display: pnInterpHfCoeff, Accuracy: U1.5, Range: [0, 32], level for adjusting high frequency gradient*/
    AX_U8                            nXInterpHfCoeff;                     /* Display: xInterpHfCoeff, Accuracy: U1.5, Range: [0, 32], level for adjusting high frequency gradient*/
    AX_ISP_DEMOSAIC_HF_DIR_EST_ENHC_LEVEL_E eHfDirEstEnhcLevel;           /* Display: hfDirEstEnhcLevel, Accuracy: U3.0, Range: [0, 3], 0: disable, 1: weak. 2: middle. 3: strong*/
} AX_ISP_IQ_DEMOSAIC_MANUAL_T;

typedef struct {
    AX_U8                            nDemEn;                              /* demosaic enable, Accuracy: U1.0 Range: [0, 1], 0: Disable, 1: Enable */
    AX_ISP_IQ_DEMOSAIC_MANUAL_T      tManualParam;
} AX_ISP_IQ_DEMOSAIC_PARAM_T;

/************************************************************************************
 *  FCC IQ Param
 ************************************************************************************/

#define AX_ISP_FCC_SAT_LUT_SIZE         (9)

typedef struct {
    AX_U16                           nFccLimit;                           /* Display: fccLimit, Accuracy: U8.4, Range: [0, 4095], larger value means more pixels will be considered as false color pixels*/
    AX_U8                            nFccSens;                            /* Display: fccSens, Accuracy: U2.2, Range: [0, 15], larger value means higher fcc sensitivity, makes the corrected color more grayish*/
    AX_U8                            nFccSatLevel;                        /* Display: fccSatLevel, Accuracy: U4.4, Range: [0, 255], larger value means less pixels will be considered as false color pixels*/
    AX_U16                           nFccSatLut[AX_ISP_FCC_SAT_LUT_SIZE];                       /* Display: fccSatLut, Accuracy: U8.4, Range: [0, 4095], larger value means current pixel is higher possibility as false color and more need corrected color*/
    AX_U8                            nFccStr;                             /* Display: fccStr, Accuracy: U4.4, Range: [0, 255], larger value means stronger fcc strength, makes the corrected color more grayish*/
    AX_U8                            nFccSatProtectLevel;                 /* Display: fccSatProtectLevel, Accuracy: U0.8, Range: [0, 255], smaller value means  more corrected color will be used in the final output*/
    AX_U16                           nFccRatioLimit;                      /* Display: fccRatioLimit, Accuracy: U1.8, Range: [0, 256], false color correction ratio limit of upper limit*/
} AX_ISP_IQ_FCC_MANUAL_T;

typedef struct {
    AX_U8                            nFccEn;                              /* fcc enable, Accuracy: U1.0 Range: [0, 1], 0: Disable, 1: Enable */
    AX_ISP_IQ_FCC_MANUAL_T           tManualParam;
} AX_ISP_IQ_FCC_PARAM_T;

/************************************************************************************
 *  GIC IQ Param
 ************************************************************************************/

#define AX_ISP_GIC_STRENGTH_LUT_SIZE    (9)

typedef struct {
    AX_U8                            nGicStrFactor;                       /* Display: gicStrFactor, Accuracy: U2.6, Range: [0, 255], */
    AX_U8                            nGicStrLut[AX_ISP_GIC_STRENGTH_LUT_SIZE];                       /* Display: gicStrLut, Accuracy: U1.7, Range: [0, 128], lut[0]:0, [1]:32, [2]:64, ... [8]:256*/
    AX_U16                           nGicCtLut[AX_ISP_GIC_STRENGTH_LUT_SIZE];                        /* Display: gicCtLut, Accuracy: U8.4, Range: [0, 4095], gic crosstalk level for R/B channel. lut[*][0]:0, [*][1]:32, [*][2]:64, ... [*][8]:256*/
    AX_U16                           nGicLimitLut[AX_ISP_GIC_STRENGTH_LUT_SIZE];                     /* Display: gicLimitLut, Accuracy: U8.4, Range: [0, 4095], gic limit lut, bigger value, stronger gic. lut[0]:0, [1]:32, [2]:64, ... [8]:256*/
} AX_ISP_IQ_GIC_MANUAL_T;

typedef struct {
    AX_U8                            nGicEn;                                            /* gic enable, Accuracy: U1.0 Range: [0, 1], 0: Disable, 1: Enable */
    AX_ISP_IQ_GIC_MANUAL_T           tManualParam;
} AX_ISP_IQ_GIC_PARAM_T;

/************************************************************************************
 *  EDGE ENHANCE IQ Param
 ************************************************************************************/

#define AX_ISP_EDGE_ENHANCE_LUT_SIZE        (9)

typedef struct {
    AX_U8                            nEdgeSmoothStr[AX_ISP_EDGE_ENHANCE_LUT_SIZE];                   /* Display: edgeSmoothStr, Accuracy: U1.4, Range: [0, 16], larger values means more smooth. lut[0]:0, [1]:32, [2]:64, ... [8]:256*/
    AX_U8                            nEdgeSharpStr[AX_ISP_EDGE_ENHANCE_LUT_SIZE];                    /* Display: edgeSharpStr, Accuracy: U1.4, Range: [0, 16], larger values means more sharpen. lut[0]:0, [1]:32, [2]:64, ... [8]:256*/
} AX_ISP_IQ_EDGE_ENHANCE_MANUAL_T;

typedef struct {
    AX_U8                            nEdgeEnhanceEn;                                    /* edge_enhance enable, Accuracy: U1.0 Range: [0, 1], 0: Disable, 1: Enable */
    AX_ISP_IQ_EDGE_ENHANCE_MANUAL_T  tManualParam;
} AX_ISP_IQ_EDGE_ENHANCE_PARAM_T;

/************************************************************************************
 *  CC IQ Param
 ************************************************************************************/
#define AX_ISP_CC_LUMA_RATIO_SIZE           (2)
#define AX_ISP_CC_CCM_V_SIZE                (3)
#define AX_ISP_CC_CCM_H_SIZE                (2)
#define AX_ISP_CC_CCM_SIZE                  (AX_ISP_CC_CCM_V_SIZE * AX_ISP_CC_CCM_H_SIZE)
#define AX_ISP_CC_ANGLE_SIZE                (16)
#define AX_ISP_CC_SAT_MIT_LUT_SIZE          (8)
#define AX_ISP_CC_SAT_MIT_CP_SIZE           (6)

typedef enum {
    AX_ISP_CC_BASIC                  = 0,
    AX_ISP_CC_ADVANCED               = 1,
} AX_ISP_CC_MODE_E;

typedef struct {
    AX_ISP_CC_MODE_E                 eMode;                               /* Display: mode, Accuracy: U1.0, Range: [0, 1], clc ctrl mode, 0 for basic (global); 1 for advanced (16 phase)*/
    AX_U8                            nCtrlLumaRatio[AX_ISP_CC_LUMA_RATIO_SIZE];                   /* Display: ctrlLumaRatio, Accuracy: U1.7, Range: [0, 128], clc control luma ratio, sum will be 1.0*/
    AX_S16                           nCcm[AX_ISP_CC_CCM_V_SIZE][AX_ISP_CC_CCM_H_SIZE];                          /* Display: ccm, Accuracy: S3.8, Range: [-2047, 2047], clc ccm matrix*/
    AX_S8                            nCcmSat;                             /* Display: ccmSat, Accuracy: S1.6, Range: [-64, 64], clc global saturation adjust*/
    AX_S16                           nCcmHue;                             /* Display: ccmHue, Accuracy: S5.6, Range: [-1920, 1920], clc global hue adjust*/
    AX_U16                           nCcmCtrlLevel;                       /* Display: ccmCtrlLevel, Accuracy: U1.8, Range: [0, 256], clc global adjust ctrl level*/
    AX_S8                            nXcmSats[AX_ISP_CC_ANGLE_SIZE];                        /* Display: xcmSats, Accuracy: S1.6, Range: [-32, 32], clc local saturation adjust*/
    AX_S16                           nXcmHues[AX_ISP_CC_ANGLE_SIZE];                        /* Display: xcmHues, Accuracy: S5.6, Range: [-640, 640], clc local hue adjust*/
    AX_U16                           nXcmCtrlLevels[AX_ISP_CC_ANGLE_SIZE];                  /* Display: xcmCtrlLevels, Accuracy: U1.8, Range: [0, 256], clc local adjust ctrl level*/
    AX_U8                            nSatMitEnable;                       /* Display: satMitEnable, Accuracy: U1.0, Range: [0, 1], clc sat_mit_enable*/
    AX_U8                            nSatMitRatioLowerLut[AX_ISP_CC_SAT_MIT_LUT_SIZE];             /* Display: satMitRatioLowerLut, Accuracy: U1.7, Range: [0, 128], clc sat_mit_ratio_lower_lut*/
    AX_U8                            nSatMitRatioUpperLut[AX_ISP_CC_SAT_MIT_LUT_SIZE];             /* Display: satMitRatioUpperLut, Accuracy: U1.7, Range: [0, 128], clc sat_mit_ratio_upper_lut*/
    AX_S16                           nSatMitLowerCp[AX_ISP_CC_SAT_MIT_CP_SIZE];                   /* Display: satMitLowerCp, Accuracy: S8.0, Range: [-255, 255], clc sat_mit_lower's control point with [x0, y0, x1, y1, x2, y2], [x2, y2] is the dividing point from y=x*/
    AX_U16                           nSatMitUpperCp[AX_ISP_CC_SAT_MIT_CP_SIZE];                   /* Display: satMitUpperCp, Accuracy: U9.0, Range: [0, 511], clc sat_mit_upper's control point with [x0, y0, x1, y1, x2, y2], [x0, y0] is the dividing point from y=x, with y0, y1, y2 <= 255*/
} AX_ISP_IQ_CC_MANUAL_T;

typedef struct {
    AX_U8                            nCcEn;                               /* cc enable, Accuracy: U1.0 Range: [0, 1], 0: Disable, 1: Enable */
    AX_ISP_IQ_CC_MANUAL_T            tManualParam;
} AX_ISP_IQ_CC_PARAM_T;

/************************************************************************************
 *  Gamma IQ Param
 ************************************************************************************/
#define AX_ISP_GAMMA_CUSTOM_LUT_SIZE               (129)

typedef enum {
    AX_ISP_GAM_LINEAR                = 0,
    AX_ISP_GAM_BT709                 = 1,
    AX_ISP_GAM_SRGB                  = 2,
    AX_ISP_GAM_AX_GAM0               = 3,
    AX_ISP_GAM_AX_GAM1               = 4,
    AX_ISP_GAM_AX_GAM2               = 5,
    AX_ISP_GAM_MODE_CUSTOMER         = 6,
} AX_ISP_GAM_PRESET_GAMMA_TYPE_E;

typedef struct {
    AX_ISP_GAM_PRESET_GAMMA_TYPE_E   ePresetGammaType;                    /* Display: mode, Accuracy: U8.0, Range: [0, 6], 0: AX_ISP_GAM_LINEAR, 1: AX_ISP_GAM_BT709, 2: AX_ISP_GAM_SRGB, 3:AX_ISP_GAM_AX_GAM0(default gamma), 4: AX_ISP_GAM_AX_GAM1, 5: AX_ISP_GAM_AX_GAM2, 6: AX_ISP_GAM_MODE_CUSTOMER.*/
    AX_U16                           nCustomLut[AX_ISP_GAMMA_CUSTOM_LUT_SIZE];                     /* Display: customLut, Accuracy: U8.4, Range: [0, 4095], gamma curve. default is AX_ISP_GAM_AX_GAM0*/
} AX_ISP_IQ_GAMMA_MANUAL_T;

typedef struct {
    AX_U8                            nGammaEn;                            /* gamma enable, Accuracy: U1.0 Range: [0, 1], 0: Disable, 1: Enable */
    AX_ISP_IQ_GAMMA_MANUAL_T         tManualParam;
} AX_ISP_IQ_GAMMA_PARAM_T;

/************************************************************************************
 *  Dehaze IQ Param
 ************************************************************************************/
#define AX_ISP_DEHAZE_STATS_MAX_H            (240)
#define AX_ISP_DEHAZE_STATS_MAX_W            (135)
#define AX_ISP_DEHAZE_STATS_MAX_NUM          (AX_ISP_DEHAZE_STATS_MAX_H * AX_ISP_DEHAZE_STATS_MAX_W)

typedef struct {
    AX_U8                            nCalcMode;                           /* Display: calcMode, Accuracy: U1.0, Range: [0, 1], 0 using traditional algorithm, 1 using accelerated algorithm.*/
    AX_U16                           nAirReflect;                         /* Display: airReflect, Accuracy: U8.8, Range: [1, 65535], air reflect. Too small will lead to over exposure of the image, and too large will lead to dead black of the image*/
    AX_U16                           nEffectStrength;                     /* Display: effectStr, Accuracy: U1.15, Range: [0, 32768], adjust defogging intensity*/
    AX_U16                           nStrengthLimit;                      /* Display: strLimit, Accuracy: U1.8, Range: [1, 256], effective strength limit*/
    AX_U8                            nSpatialSmoothness;                  /* Display: spSmooth, Accuracy: U2.0, Range: [0, 3], The higher the value, the more global the defogging effect*/
    AX_U16                           nEps;                                /* Display: eps, Accuracy: U8.4, Range: [1, 4095], Generally, no adjustment is required, which affects the intensity of extreme contrast areas*/
    AX_U8                            nBlurEnable;                         /* Display: blurEnable, Accuracy: U1.0, Range: [0, 1], 0 is turned off and 1 is turned on to alleviate the halo problem*/
    AX_U16                           nSigmaBlur;                          /* Display: sigmaBlur, Accuracy: U3.13, Range: [1, 65535], blur intensity of the halo problem*/
    AX_U8                            nMeshSize;                           /* Display: meshSize, Accuracy: U7.0, Range: {32, 64}, Affects the uniformity of defogging intensity. The higher the value, the more uniform the defogging intensity will be, and is equal to 64 when calc_mode == 1*/
} AX_ISP_IQ_DEHAZE_MANUAL_T;

typedef struct {
    AX_U8                            nDehazeEn;                           /* dehaze enable, Accuracy: U1.0 Range: [0, 1], 0: Disable, 1: Enable */
    AX_ISP_IQ_DEHAZE_MANUAL_T        tManualParam;
} AX_ISP_IQ_DEHAZE_PARAM_T;

typedef struct {
    AX_U16 nShapeWidth;
    AX_U16 nShapeHeight;
} AX_ISP_DEHAZE_STAT_SHAPE_T;

typedef struct {
    AX_U64 nSeqNum;
    AX_ISP_DEHAZE_STAT_SHAPE_T tShape;
    AX_U16 nDcStats[AX_ISP_DEHAZE_STATS_MAX_NUM]; /* Accuracy: U8.2, Dark Channel Statitics */
} AX_ISP_DEHAZE_STAT_INFO_T;

/************************************************************************************
 *  CSC IQ Param
 ************************************************************************************/
#define AX_ISP_YUV_CSC_MATRIX_SIZE           (3)

typedef enum {
    AX_ISP_CSC_BT601                 = 0,
    AX_ISP_CSC_BT709                 = 1,
    AX_ISP_CSC_BT2020                = 2,
    AX_ISP_CSC_USER                  = 3,
} AX_ISP_CSC_COLOR_SPACE_MODE_E;

typedef struct {
    AX_ISP_CSC_COLOR_SPACE_MODE_E    eColorSpaceMode;                     /* Display: cscCvtMode, Accuracy: U4.0, Range: [0, 3], csc conversion mode, 0:AX_ISP_CSC_BT601, 1:AX_ISP_CSC_BT709, 2:AX_ISP_CSC_BT2020, 3:AX_ISP_CSC_USER*/
    AX_S16                           nMatrix[AX_ISP_YUV_CSC_MATRIX_SIZE][AX_ISP_YUV_CSC_MATRIX_SIZE];                       /* Display: cscMat, Accuracy: S2.10, Range: [-4096, 4095], csc conversion matrix, valid only for AX_ISP_CSC_USER*/
} AX_ISP_IQ_CSC_MANUAL_T;

typedef struct {
    AX_ISP_IQ_CSC_MANUAL_T           tManualParam;
} AX_ISP_IQ_CSC_PARAM_T;

/************************************************************************************
 *  YUV-3DNR IQ Param
 ************************************************************************************/
#define YUV3DNR_TW_LUT_SIZE                    (17)
#define YUV3DNR_REGS_GROUP_NUM                 (2)
#define YUV3DNR_SF_REGS_NUM                    (3)
#define YUV3DNR_LUT_SIZE                       (9)
typedef struct {
    AX_U8                            nExtMaskMode;                        /* Display: extMask.mode, Accuracy: U1.0, Range: [0, 1], ext mask mode, 0:ai, 1:hdr*/
    AX_U8                            nExtMaskThr;                         /* Display: extMask.thr, Accuracy: U1.7, Range: [0, 128], The starting point for remapping ext mask, below which the probability will be mapped to 0*/
    AX_U16                           nExtMaskGain;                        /* Display: extMask.gain, Accuracy: U8.8, Range: [0, 65535], The slope of ext mask remapping, the larger the value, the easier it is to map the mask to 1.0*/
    AX_U8                            nExtMaskRatio;                       /* Display: extMask.ratio, Accuracy: U1.7, Range: [0, 128], Only effective in AI mode. The reference level of AI mask. The larger the size, the more likely it is to use an AI mask as the motion mask*/
    AX_U16                           nMotSmallSel;                        /* Display: detMotion.smallSel, Accuracy: U9.0, Range: [0, 256], Applied to the feature of motion judgement, the larger the value, the larger the detection window used for motion judgement, which improves the noise resistance and smoothness of motion detection while losing the ability to detect texture details.*/
    AX_U16                           nMotLargeSel;                        /* Display: detMotion.largeSel, Accuracy: U9.0, Range: [0, 256], The greater the values of features applied to time-domain fusion coefficient filtering, the larger the detection window used for motion judgement. This improves the noise resistance and smoothness of motion detection while losing texture detail detection ability.*/
    AX_U8                            nMotSmoothStr;                       /* Display: detMotion.smoothStr, Accuracy: U1.7, Range: [0, 128], The pre smoothing degree of the motion judgement features, the larger the value, the smoother the motion judgement features.*/
    AX_U8                            nMotSensThr;                         /* Display: detMotion.sensThr, Accuracy: U8.0, Range: [0, 255], Representing the difference between two types of motion judgement features, used to map the fusion ratio of texture sensitive motion judgement features. The larger the value, the less texture sensitive motion judgement features are used, and the probability of misjudgment caused by noise decreases. However, it may lead to some areas being misjudged as stationary.*/
    AX_U16                           nMotSensGain;                        /* Display: detMotion.sensGain, Accuracy: U1.8, Range: [0, 511], Based on the fusion mapping of texture sensitive motion judgement features. The larger the value, the faster the texture sensitive motion judgement features are used, and the probability of misjudgment caused by noise increases, resulting in more accurate motion judgement.*/
    AX_U8                            nMotSensRatio[YUV3DNR_REGS_GROUP_NUM];                    /* Display: detMotion.sensRatio, Accuracy: U1.7, Range: [0, 128], The upper and lower limits of the fusion ratio of texture sensitive motion judgement features. The larger the value, the more texture sensitive motion judgement features are used, and the probability of misjudgment caused by noise increases, resulting in more accurate motion judgement*/
    AX_U16                           nMotShpDiff;                         /* Display: detMotion.shpDiff, Accuracy: U8.2, Range: [0, 1023], For highlight areas, the frame difference judgment threshold for motion sharpening is used. If the frame difference is higher than this threshold, sharpening will occur, and the larger the value, the less effective motion sharpening will be.*/
    AX_U8                            nMotShpLumaThr;                      /* Display: detMotion.shpLumaThr, Accuracy: U8.0, Range: [0, 255], For highlight areas, the brightness judgment threshold for motion sharpening is used. Only when the brightness is higher than this threshold will it sharpen. The larger the value, the less effective the motion sharpening will be.*/
    AX_U16                           nMotShpLumaGain;                     /* Display: detMotion.shpLumaGain, Accuracy: U2.8, Range: [0, 1023], For the highlight area, the brightness judgment gain using motion sharpening is used. The larger the value, the faster the transition from motion sharpening in the dark area to brightness.*/
    AX_U8                            nMotShpVarThr;                       /* Display: detMotion.shpVarThr, Accuracy: U7.0, Range: [0, 127], For highlight areas, the variance judgment threshold for motion sharpening is used. Only when the variance is higher than this threshold will the sharpening be effective. The larger the value, the less effective the motion sharpening will be.*/
    AX_U16                           nMotShpVarGain;                      /* Display: detMotion.shpVarGain, Accuracy: U2.7, Range: [0, 511], For the highlight area, the variance judgment gain of motion sharpening is used. The larger the value, the faster the transition from small variance to large variance motion sharpening*/
    AX_U8                            nMotShpStr;                          /* Display: detMotion.shpStr, Accuracy: U3.5, Range: [0, 255], For highlight areas, motion sharpening intensity is used. The higher the value, the greater the intensity, making it easier to identify as motion.*/
    AX_U8                            nMotYRedCorStr;                      /* Display: detMotion.yRedCorStr, Accuracy: U3.5, Range: [0, 255], The intensity of Y channel motion correction in the red area, the larger the value, the easier it is to determine as stationary. It does not take effect when the nMotSmallSel is at its maximum.*/
    AX_U8                            nMotCRedCorStr;                      /* Display: detMotion.cRedCorStr, Accuracy: U3.5, Range: [0, 255], The intensity of UV channel motion correction in the red area, the larger the value, the easier it is to determine as stationary. It does not take effect when the nMotSmallSel is at its maximum.*/
    AX_U8                            nMotYBlueCorStr;                     /* Display: detMotion.yBlueCorStr, Accuracy: U3.5, Range: [0, 255], The intensity of Y channel motion correction in the blue area, the larger the value, the easier it is to determine as stationary. It does not take effect when the nMdSmallSel is at its maximum*/
    AX_U8                            nMotCBlueCorStr;                     /* Display: detMotion.cBlueCorStr, Accuracy: U3.5, Range: [0, 255], The intensity of UV channel motion correction in the blue area, the larger the value, the easier it is to determine as stationary. It does not take effect when the nMdSmallSel is at its maximum.*/
    AX_U8                            nMotYDecThr;                         /* Display: detMotion.yDecThr, Accuracy: U8.0, Range: [0, 255], Y motion judgement threshold, the more the whole frame tends to be static.*/
    AX_U16                           nMotYDecGain;                        /* Display: detMotion.yDecGain, Accuracy: U1.8, Range: [0, 511], Y determines the transition speed based on movement. The higher the value, the more inclined the whole frame is towards motion.*/
    AX_U8                            nMotYM2sDecThr;                      /* Display: detMotion.yM2sDecThr, Accuracy: U8.0, Range: [0, 255], From dynamic to static region Y, the threshold for motion judgement. The higher the value, the more the whole frame tends to be static.*/
    AX_U16                           nMotYM2sDecGain;                     /* Display: detMotion.yM2sDecGain, Accuracy: U1.8, Range: [0, 511], From dynamic to static zone Y, determine the transition speed. The higher the value, the more inclined the whole frame is towards motion.*/
    AX_U8                            nMotYHdrAdjThr;                      /* Display: detMotion.yHdrAdjThr, Accuracy: U8.0, Range: [0, 255], When applied to Y, the larger the value, the easier it is to correct the short frame area to be still.*/
    AX_U8                            nMotCDecThr;                         /* Display: detMotion.cDecThr, Accuracy: U8.0, Range: [0, 255], UV motion judgement threshold. The higher the value, the more the whole frame tends to be static.*/
    AX_U16                           nMotCDecGain;                        /* Display: detMotion.cDecGain, Accuracy: U1.8, Range: [0, 511], UV motion judgement of transition speed. The higher the value, the more inclined the whole frame is towards motion.*/
    AX_U8                            nMotCAdjThr;                         /* Display: detMotion.cAdjThr, Accuracy: U8.0, Range: [0, 255], The UV motion judgement correction threshold will make the UV more inclined to use the current frame, based on normal motion judgement. The higher the value, the more the overall UV tends to be static.*/
    AX_U16                           nMotCAdjGain;                        /* Display: detMotion.cAdjGain, Accuracy: U1.8, Range: [0, 511], UV motion judgement correction excessive speed will make UV more inclined to use the current frame. The higher the value, the more the overall UV tends to move.*/
    AX_U8                            nMotCHdrAdjThr;                      /* Display: detMotion.cHdrAdjThr, Accuracy: U8.0, Range: [0, 255], When applied to UV, the larger the value, the easier it is to correct the short frame area to be still. */
    AX_U16                           nMotYNoiseLut[YUV3DNR_LUT_SIZE];                    /* Display: detMotion.yNoiseLut, Accuracy: U8.2, Range: [0, 1023], Noise correction motion judgement Y channel feature lookup table, with brightness on the x-axis and subtraction method. The larger the value, the stronger the correction, and the easier it is to mistake real motion for stillness.*/
    AX_U16                           nMotCNoiseLut[YUV3DNR_LUT_SIZE];                    /* Display: detMotion.cNoiseLut, Accuracy: U8.2, Range: [0, 1023], Noise correction motion judgement UV channel feature lookup table, with brightness on the x-axis and subtraction method. The larger the value, the stronger the correction, and the easier it is to mistake real motion for stillness.*/
    AX_U8                            nMotYHvsLut[YUV3DNR_LUT_SIZE];                      /* Display: detMotion.yHvsLut, Accuracy: U3.5, Range: [0, 255], Noise correction motion judgement Y channel feature lookup table, with brightness on the x-axis and multiplication method. The smaller the value, the stronger the correction, and the easier it is to mistake real motion for stillness.*/
    AX_U8                            nMotCHvsLut[YUV3DNR_LUT_SIZE];                      /* Display: detMotion.cHvsLut, Accuracy: U3.5, Range: [0, 255], Noise correction motion judgement UV channel feature lookup table, with brightness on the x-axis and multiplication method. The smaller the value, the stronger the correction, and the easier it is to mistake real motion for stillness.*/
    AX_U8                            nTWtCurW;                            /* Display: tnr.curW, Accuracy: U1.7, Range: [0, 128], The fusion ratio of current motion judgement and historical motion information, the larger the value, the smaller the historical motion information reference, the faster the convergence speed of dynamic and static, and the higher the probability of misjudging as static.*/
    AX_U8                            nTWtPreLimit;                        /* Display: tnr.preLimit, Accuracy: U1.7, Range: [0, 128], The reference threshold for historical motion information, significant motion information greater than this value will be referenced and integrated into the current motion judgment. The larger the value, the smaller the historical motion information reference, and the faster the convergence speed of motion and stillness, increasing the probability of misjudging as stationary.*/
    AX_U16                           nTWtMaxStr;                          /* Display: tnr.maxStr, Accuracy: U10.0, Range: [0, 512], The maximum filtering strength of the time-domain fusion coefficient is greater, and the larger the value, the greater the filtering strength. The larger the proportion of using the current frame, the higher the probability of raindrop noise appearing.*/
    AX_U16                           nTWtSmoothStr;                       /* Display: tnr.smoothStr, Accuracy: U10.0, Range: [0, 512], The average filtering strength of the time-domain fusion coefficient is higher, and the larger the value, the greater the average filtering strength. The proportion of using the current frame is smaller, and the probability of raindrop noise appearing is reduced. */
    AX_U8                            nTWtSfThr0;                          /* Display: tnr.sfThr0, Accuracy: U8.0, Range: [0, 255], Guiding the selection of time-domain fusion coefficients based on regional motion judgement features, the larger the value, the more the global result is biased towards smooth filtering*/
    AX_U16                           nTWtSfGain0;                         /* Display: tnr.sfGain0, Accuracy: U1.8, Range: [0, 511], Guiding the selection of time-domain fusion coefficients based on regional motion judgement features, the larger the value, the steeper the transition*/
    AX_U8                            nTWtSfThr1;                          /* Display: tnr.sfThr1, Accuracy: U8.0, Range: [0, 255], By using detail sensitive motion judgement features to guide the selection of time-domain fusion coefficients, the larger the value, the more the global result is biased towards smooth filtering*/
    AX_U16                           nTWtSfGain1;                         /* Display: tnr.sfGain1, Accuracy: U1.8, Range: [0, 511], Guiding the selection of time-domain fusion coefficients based on detail sensitive motion judgement features, the larger the value, the steeper the transition*/
    AX_U8                            nTWtSfThr2;                          /* Display: tnr.sfThr2, Accuracy: U7.0, Range: [0, 81], Guiding the selection of time-domain fusion coefficients based on pixel relative relationships, the larger the value, the more the global result is biased towards smooth filtering*/
    AX_U8                            nTWtSfGain2;                         /* Display: tnr.sfGain2, Accuracy: U1.7, Range: [0, 255], Guiding the selection of time-domain fusion coefficients through pixel relative relationships, the larger the value, the steeper the transition*/
    AX_U8                            nTWtStr;                             /* Display: tnr.str, Accuracy: U0.7, Range: [0, 127], The stronger the time-domain filtering strength, the more biased it is towards using the current frame result, and the greater the Y noise in the stationary region.*/
    AX_U8                            nRtCAdjThr;                          /* Display: tnr.cAdjThr, Accuracy: U1.7, Range: [0, 128], Based on the modified UV motion judgement, the threshold for mapping the UV correction temporal fusion coefficient is determined. The smaller the value, the more inclined it is to use the current frame for global UV*/
    AX_U16                           nRtCAdjGain;                         /* Display: tnr.cAdjGain, Accuracy: U4.8, Range: [0, 4095], According to the modified UV motion judgement, the transition speed of the mapping UV correction temporal fusion coefficient is determined. The larger the value, the more inclined it is to use the current frame for the global UV*/
    AX_U8                            nRtCAdj;                             /* Display: tnr.cAdj, Accuracy: U1.7, Range: [0, 128], According to the modified UV motion judgement, the correction strength of the mapping UV correction temporal fusion coefficient is determined. The larger the value, the more inclined it is to use the current frame for the global UV*/
    AX_U8                            nRtCLimit;                           /* Display: tnr.cLimit, Accuracy: U1.7, Range: [0, 128], The stronger the UV time-domain denoising intensity, the more the current frame is used, and the greater the UV noise in the stationary area.*/
    AX_U8                            nM2sCur;                             /* Display: tnr.m2sCur, Accuracy: U1.7, Range: [0, 128], In the determination of moving to stationary areas, the current frame needs to be stationary and the historical frames need to be moving. This corresponds to the threshold for the current frame being stationary, and the larger the value, the more likely it is to be ineffective.*/
    AX_U8                            nM2sRef;                             /* Display: tnr.m2sRef, Accuracy: U1.7, Range: [0, 128], In the determination of moving to stationary areas, the current frame needs to be stationary, and the historical frames need to be motion. Here, the corresponding historical frames are the threshold for motion, and the smaller the value, the more likely it is not to take effect.*/
    AX_U8                            nM2sVarThr;                          /* Display: tnr.m2sVarThr, Accuracy: U7.0, Range: [0, 127], The variance threshold of the texture part in the transition area from motion to stillness is located, and the larger the value, the fewer textures are located*/
    AX_U16                           nM2sVarGain[YUV3DNR_REGS_GROUP_NUM];                      /* Display: tnr.m2sVarGain, Accuracy: U1.11, Range: [0, 4095], Locate the texture overlap speed of the texture part in the transition zone from motion to stillness, with smaller values indicating less overlap, [0]: lf, [1]: hf*/
    AX_U8                            nM2sBlendRatio[YUV3DNR_REGS_GROUP_NUM];                   /* Display: tnr.m2sBlendRatio, Accuracy: U1.7, Range: [0, 128], Locate the texture overlap ratio of the texture part in the transition area from motion to stillness, with smaller values indicating less overlap, [0]: lf, [1]: hf*/
    AX_U8                            nTWtStaticLut[YUV3DNR_TW_LUT_SIZE];                   /* Display: tnr.staticLut, Accuracy: U1.7, Range: [0, 128], Time domain fusion speed curve, the horizontal axis represents the degree to which the history has been stationary, the smaller the value, the longer the stationary time, the vertical axis represents the current mapped time domain fusion coefficient, and the smaller the value, the more reference frames are used.*/
    AX_U8                            nSfYLfRefWtThr;                      /* Display: ynr.lf.refWtThr, Accuracy: U1.7, Range: [0, 128], When providing the next reference frame, the lower the threshold of the time-domain fusion coefficient between the low-frequency denoising result and the denoised image, the more inclined it is to use the denoising result globally*/
    AX_U16                           nSfYLfRefWtGain;                     /* Display: ynr.lf.refWtGain, Accuracy: U4.8, Range: [0, 4095], When providing the next reference frame, the transition speed of the time-domain fusion coefficient between the low-frequency denoising result and the denoised image fusion is faster, and the larger the transition speed, the more inclined it is to use the denoising result globally*/
    AX_U8                            nSfYLfRefRatio;                      /* Display: ynr.lf.refRatio, Accuracy: U1.7, Range: [0, 128], When providing the next reference frame, the larger the ratio of the time-domain fusion coefficient between the low-frequency denoising result and the denoised image, the more inclined it is to use the denoising result globally. The larger the value, the lower the clarity of the still area and the smaller the noise.*/
    AX_U8                            nSfYLfCurWtThr;                      /* Display: ynr.lf.curWtThr, Accuracy: U1.7, Range: [0, 128], When provided to the output frame, the lower the threshold of the time-domain fusion coefficient between the low-frequency denoising result and the denoised image, the more inclined it is to use the denoising result globally*/
    AX_U16                           nSfYLfCurWtGain;                     /* Display: ynr.lf.curWtGain, Accuracy: U4.8, Range: [0, 4095], When providing the output frame, the transition speed of the time-domain fusion coefficient between the low-frequency denoising result and the denoised image fusion is faster, and the larger the transition speed, the more inclined it is to use the denoising result globally*/
    AX_U8                            nSfYLfCurRatio;                      /* Display: ynr.lf.curRatio, Accuracy: U1.7, Range: [0, 128], When providing the output frame, the larger the ratio of the time-domain fusion coefficient between the low-frequency denoising result and the denoised image, the more inclined it is to use the denoising result globally. The larger the value, the lower the clarity of the still area and the smaller the noise.*/
    AX_U8                            nSfYLfEdgeThr;                       /* Display: ynr.lf.edgeThr, Accuracy: U8.0, Range: [0, 255], The threshold for low-frequency edge judgment, the larger the value, the fewer areas are judged as edges.*/
    AX_U16                           nSfYLfEdgeGain;                      /* Display: ynr.lf.edgeGain, Accuracy: U1.8, Range: [0, 511], Low frequency edge judgment transition speed, the smaller the value, the fewer areas are judged as edges.*/
    AX_U8                            nSfYLfDetailThr;                     /* Display: ynr.lf.detailThr, Accuracy: U8.0, Range: [0, 255], The threshold for low-frequency texture judgment is that the larger the value, the fewer areas are judged as texture.*/
    AX_U16                           nSfYLfDetailGain;                    /* Display: ynr.lf.detailGain, Accuracy: U1.8, Range: [0, 511], Low frequency texture determines transition speed, and the smaller the value, the fewer areas are judged as texture.*/
    AX_U8                            nSfYLfSpWinSel[YUV3DNR_SF_REGS_NUM];                   /* Display: ynr.lf.spWinSel, Accuracy: U2.0, Range: [0, 2], The size of the low-frequency denoising window is larger, and the larger the value, the larger the window filter is used, 0:flat, 1:detail, 2:edge*/
    AX_U16                           nSfYLfSpSigma[YUV3DNR_SF_REGS_NUM];                    /* Display: ynr.lf.spSigma, Accuracy: U9.0, Range: [1, 320], The low-frequency denoising spatial filtering coefficient tends to use all pixels within the window as the value increases, and tends to use pixels in the center area of the window as the value decreases., 0:flat, 1:detail, 2:edge*/
    AX_U16                           nSfYLfThr[YUV3DNR_SF_REGS_NUM];                        /* Display: ynr.lf.thr, Accuracy: U8.2, Range: [0, 1023], Low frequency denoising threshold. The larger the value, the greater the denoising intensity, 0:flat, 1:detail, 2:edge*/
    AX_U16                           nSfYLfGain[YUV3DNR_SF_REGS_NUM];                       /* Display: ynr.lf.gain, Accuracy: U2.8, Range: [0, 1023], The degree of excessive edge preservation in low-frequency denoising. The larger the value, the smaller the denoising intensity, 0:flat, 1:detail, 2:edge*/
    AX_U8                            nSfYLfFAlpha[YUV3DNR_SF_REGS_NUM];                     /* Display: ynr.lf.fAlpha, Accuracy: U1.5, Range: [0, 32], The fusion weight of bilateral and NLM filtering tends to use NLM as the weight increases, 0:flat, 1:detail, 2:edge*/
    AX_U16                           nSfYLfHdrAdjThr;                     /* Display: ynr.lf.hdrAdjThr, Accuracy: U8.2, Range: [0, 1023], Short frame current frame brightness low-frequency noise reduction intensity correction, the larger the value, the greater the low-frequency noise reduction intensity of the short frame*/
    AX_U16                           nSfYLfOriStr;                        /* Display: ynr.lf.oriStr, Accuracy: U8.4, Range: [0, 4095], The low-frequency denoising result overlaps the original noise ratio, and the larger the value, the closer it is to the original frame.*/
    AX_S16                           nSfYLfThrLut[YUV3DNR_LUT_SIZE];                     /* Display: ynr.lf.thrLut, Accuracy: S6.2, Range: [-256, 255], Adjust the low-frequency noise reduction threshold based on brightness. The horizontal axis represents brightness, and the larger the vertical axis value, the stronger the noise reduction intensity.*/
    AX_U8                            nSfYLfGainLut[YUV3DNR_LUT_SIZE];                    /* Display: ynr.lf.gainLut, Accuracy: U3.5, Range: [0, 255], Adjust the transition degree of denoising and edge preservation based on brightness. The horizontal axis represents brightness, and the larger the vertical axis value, the smaller the denoising intensity.*/
    AX_U8                            nSfYHfRefWtThr;                      /* Display: ynr.hf.refWtThr, Accuracy: U1.7, Range: [0, 128], When providing the next reference frame, the lower the threshold of the time-domain fusion coefficient between the high-frequency denoising result and the denoised image, the more inclined it is to use the denoising result globally*/
    AX_U16                           nSfYHfRefWtGain;                     /* Display: ynr.hf.refWtGain, Accuracy: U4.8, Range: [0, 4095], When providing the next reference frame, the transition speed of the time-domain fusion coefficient between the high-frequency denoising result and the denoised image fusion is faster, and the larger the transition speed, the more inclined it is to use the denoising result globally*/
    AX_U8                            nSfYHfRefRatio;                      /* Display: ynr.hf.refRatio, Accuracy: U1.7, Range: [0, 128], When providing the next reference frame, the higher the ratio of the time-domain fusion coefficient between the high-frequency denoising result and the denoised image, the more inclined it is to use the denoising result globally. The larger the value, the lower the clarity of the still area and the smaller the noise.*/
    AX_U8                            nSfYHfCurWtThr;                      /* Display: ynr.hf.curWtThr, Accuracy: U1.7, Range: [0, 128], When provided to the output frame, the threshold of the time-domain fusion coefficient between the high-frequency denoising result and the denoised image is smaller, and the smaller the threshold, the more inclined it is to use the denoising result globally*/
    AX_U16                           nSfYHfCurWtGain;                     /* Display: ynr.hf.curWtGain, Accuracy: U4.8, Range: [0, 4095], When providing the output frame, the transition speed of the time-domain fusion coefficient between the high-frequency denoising result and the denoised image fusion is faster, and the larger the transition speed, the more inclined it is to use the denoising result globally*/
    AX_U8                            nSfYHfCurRatio;                      /* Display: ynr.hf.curRatio, Accuracy: U1.7, Range: [0, 128], When providing the output frame, the higher the ratio of the time-domain fusion coefficient between the high-frequency denoising result and the denoised image, the more inclined it is to use the denoising result globally. The larger the value, the lower the clarity of the still area and the smaller the noise.*/
    AX_U8                            nSfYHfEdgeThr;                       /* Display: ynr.hf.edgeThr, Accuracy: U8.0, Range: [0, 255], The threshold for high-frequency edge judgment is that the larger the value, the fewer areas are judged as edges.*/
    AX_U16                           nSfYHfEdgeGain;                      /* Display: ynr.hf.edgeGain, Accuracy: U1.8, Range: [0, 511], High frequency edge judgment transition speed, the smaller the value, the fewer areas are judged as edges.*/
    AX_U8                            nSfYHfDetailThr;                     /* Display: ynr.hf.detailThr, Accuracy: U8.0, Range: [0, 255], High frequency texture judgment threshold, the larger the value, the fewer areas are judged as texture.*/
    AX_U16                           nSfYHfDetailGain;                    /* Display: ynr.hf.detailGain, Accuracy: U1.8, Range: [0, 511], High frequency texture determines transition speed, and the smaller the value, the fewer areas are judged as texture.*/
    AX_U8                            nSfYHfSpWinSel[YUV3DNR_SF_REGS_NUM];                   /* Display: ynr.hf.spWinSel, Accuracy: U1.0, Range: [0, 1], The size of the high-frequency edge preservation denoising window is larger, and the larger the value, the larger the window filtering is used. 0:flat, 1:detail, 2:edge*/
    AX_U16                           nSfYHfSpSigma[YUV3DNR_SF_REGS_NUM];                    /* Display: ynr.hf.spSigma, Accuracy: U9.0, Range: [1, 320], The high-frequency edge preserving denoising spatial filtering coefficient tends to use all pixels within the window as the value increases, and tends to use pixels in the center area of the window as the value decreases., 0:flat, 1:detail, 2:edge*/
    AX_U16                           nSfYHfThr[YUV3DNR_SF_REGS_NUM];                        /* Display: ynr.hf.thr, Accuracy: U8.2, Range: [0, 1023], High frequency edge preservation denoising threshold. The larger the value, the greater the denoising intensity, 0:flat, 1:detail, 2:edge*/
    AX_U16                           nSfYHfGain[YUV3DNR_SF_REGS_NUM];                       /* Display: ynr.hf.gain, Accuracy: U2.8, Range: [0, 1023], High frequency edge preservation denoising and excessive edge preservation. The larger the value, the smaller the denoising intensity, 0:flat, 1:detail, 2:edge*/
    AX_U8                            nSfYHfFAlpha[YUV3DNR_SF_REGS_NUM];                     /* Display: ynr.hf.fAlpha, Accuracy: U1.5, Range: [0, 32], The fusion weight of high-frequency edge preservation denoising bilateral and NLM filtering tends to use NLM as the weight increases, 0:flat, 1:detail, 2:edge*/
    AX_U8                            nSfYHfSimpWinSel;                    /* Display: ynr.hf.simpWinSel, Accuracy: U2.0, Range: [0, 2], The window size of high-frequency non edge preserving denoising filter is larger, and the larger the value, the more pixels are used for denoising, resulting in a blurred effect, 0:flat, 1:detail, 2:edge*/
    AX_U16                           nSfYHfSimpSigma;                     /* Display: ynr.hf.simpSigma, Accuracy: U9.0, Range: [1, 320], High frequency non edge preserving denoising filter spatial filtering coefficient, the larger the value, the more inclined it is to use all pixels within the window, and the smaller the value, the more inclined it is to use pixels in the center area of the window, 0:flat, 1:detail, 2:edge*/
    AX_U8                            nSfYHfSimpRatio[YUV3DNR_SF_REGS_NUM];                  /* Display: ynr.hf.simpRatio, Accuracy: U1.5, Range: [0, 32], The fusion coefficient of high-frequency non edge preserving denoising filtering and edge preserving filtering, the larger the value, the more the non edge preserving filtering result is used. 0:flat, 1:detail, 2:edge.*/
    AX_U16                           nSfYHfHdrAdjThr;                     /* Display: synr.hf.hdrAdjThr, Accuracy: U8.2, Range: [0, 1023], Short frame current frame brightness high-frequency noise reduction intensity correction, the larger the value, the greater the high-frequency noise reduction intensity of the short frame*/
    AX_U16                           nSfYHfOriStr;                        /* Display: ynr.hf.oriStr, Accuracy: U8.4, Range: [0, 4095], The high-frequency denoising result overlaps the original noise ratio, and the larger the value, the closer it is to the original frame.*/
    AX_S16                           nSfYHfThrLut[YUV3DNR_LUT_SIZE];                     /* Display: ynr.hf.thrLut, Accuracy: S6.2, Range: [-256, 255], Adjust the high-frequency edge preservation and noise reduction threshold based on brightness. The horizontal axis represents brightness, and the larger the vertical axis value, the stronger the noise reduction intensity.*/
    AX_U8                            nSfYHfGainLut[YUV3DNR_LUT_SIZE];                    /* Display: ynr.hf.gainLut, Accuracy: U3.5, Range: [0, 255], Adjust the transition degree of denoising and edge preservation based on brightness. The horizontal axis represents brightness, and the larger the vertical axis value, the smaller the denoising intensity.*/
    AX_U8                            nSfCLfRefWtThr;                      /* Display: cnr.lf.refWtThr, Accuracy: U1.7, Range: [0, 128], When providing the next reference frame, the lower the threshold of the time-domain fusion coefficient between the low-frequency denoising result and the denoised image, the more inclined it is to use the denoising result globally*/
    AX_U16                           nSfCLfRefWtGain;                     /* Display: cnr.lf.refWtGain, Accuracy: U4.8, Range: [0, 4095], When providing the next reference frame, the transition speed of the time-domain fusion coefficient between the low-frequency denoising result and the denoised image fusion is faster, and the larger the transition speed, the more inclined it is to use the denoising result globally*/
    AX_U8                            nSfCLfRefRatio;                      /* Display: cnr.lf.refRatio, Accuracy: U1.7, Range: [0, 128], When providing the next reference frame, the larger the ratio of the time-domain fusion coefficient between the low-frequency denoising result and the denoised image, the more inclined it is to use the denoising result globally. The larger the value, the lower the clarity of the still area UV and the smaller the color noise.*/
    AX_U8                            nSfCLfCurWtThr;                      /* Display: cnr.lf.curWtThr, Accuracy: U1.7, Range: [0, 128], When provided to the output frame, the lower the threshold of the time-domain fusion coefficient between the low-frequency denoising result and the denoised image, the more inclined it is to use the denoising result globally*/
    AX_U16                           nSfCLfCurWtGain;                     /* Display: cnr.lf.curWtGain, Accuracy: U4.8, Range: [0, 4095], When providing the output frame, the transition speed of the time-domain fusion coefficient between the low-frequency denoising result and the denoised image fusion is faster, and the larger the transition speed, the more inclined it is to use the denoising result globally*/
    AX_U8                            nSfCLfCurRatio;                      /* Display: cnr.lf.curRatio, Accuracy: U1.7, Range: [0, 128], When provided to the output frame, the ratio of the time-domain fusion coefficient between the low-frequency denoising result and the denoised image fusion is higher, and the larger the ratio, the more inclined it is to use the denoising result globally. The larger the value, the lower the clarity of the still area UV and the smaller the color noise.*/
    AX_U16                           nSfCLfThr;                           /* Display: cnr.lf.thr, Accuracy: U7.2, Range: [0, 511], The threshold for reducing saturation based on its original size, the smaller the value, the more likely it is to be globally ineffective*/
    AX_U16                           nSfCLfGain;                          /* Display: cnr.lf.gain, Accuracy: U2.7, Range: [0, 511], Reduce the transition speed of saturation based on its original size. The larger the value, the more likely it is to be globally ineffective*/
    AX_U8                            nSfCLfLimit;                         /* Display: cnr.lf.limit, Accuracy: U1.7, Range: [0, 128], According to the proportion of saturation overlaid color noise, the smaller the value, the more inclined it is to reduce saturation*/
    AX_U16                           nSfCLfHdrAdjThr;                     /* Display: cnr.lf.hdrAdjThr, Accuracy: U8.2, Range: [0, 1023], Short frame current frame color low-frequency noise reduction intensity correction, the larger the value, the greater the low-frequency noise reduction intensity of the short frame*/
    AX_U8                            nSfCLfOriStr;                        /* Display: cnr.lf.oriStr, Accuracy: U1.5, Range: [0, 32], The ratio of low-frequency fading noise to aliasing noise, the larger the value, the closer it is to the original input*/
    AX_U16                           nSfCLfThrPreLut[YUV3DNR_LUT_SIZE];                  /* Display: cnr.lf.thrPreLut, Accuracy: U8.2, Range: [0, 1023], Low frequency color first stage denoising intensity, horizontal axis brightness, the larger the value, the greater the denoising intensity*/
    AX_U16                           nSfCLfThrLut[YUV3DNR_LUT_SIZE];                     /* Display: cnr.lf.thrLut, Accuracy: U8.2, Range: [0, 1023], Low frequency color two-stage denoising intensity, horizontal axis brightness, the larger the value, the greater the denoising intensity*/
    AX_U16                           nSfCLfPrThrLut[YUV3DNR_LUT_SIZE];                   /* Display: cnr.lf.prThrLut, Accuracy: U8.2, Range: [0, 1023], Low frequency color denoising protection threshold, based on the diff aliasing color noise before and after denoising, the larger the value, the more likely it is not to aliasing*/
    AX_U16                           nSfCLfPrGainLut[YUV3DNR_LUT_SIZE];                  /* Display: cnr.lf.prGainLut, Accuracy: U2.8, Range: [0, 1023], Low frequency color denoising protection transition speed, based on the diff of color noise before and after denoising, the larger the value, the more likely it is to be overlapped*/
    AX_U8                            nSfCHfRefWtThr;                      /* Display: cnr.hf.refWtThr, Accuracy: U1.7, Range: [0, 128], When providing the next reference frame, the lower the threshold of the time-domain fusion coefficient between the high-frequency denoising result and the denoised image, the more inclined it is to use the denoising result globally*/
    AX_U16                           nSfCHfRefWtGain;                     /* Display: cnr.hf.refWtGain, Accuracy: U4.8, Range: [0, 4095], When provided to the next reference frame, the transition speed of the time-domain fusion coefficient between the high-frequency denoising result and the denoised image fusion is faster, and the larger the transition speed, the more inclined it is to use the denoising result globally*/
    AX_U8                            nSfCHfRefRatio;                      /* Display: cnr.hf.refRatio, Accuracy: U1.7, Range: [0, 128], When provided to the next reference frame, the higher the ratio of the temporal fusion coefficient between the high-frequency denoising result and the denoised image, the more inclined it is to use the denoising result globally. The larger the value, the lower the clarity of the still area UV and the smaller the color noise.*/
    AX_U8                            nSfCHfCurWtThr;                      /* Display: cnr.hf.curWtThr, Accuracy: U1.7, Range: [0, 128], When provided to the output frame, the lower the threshold of the time-domain fusion coefficient between the high-frequency denoising result and the denoised image, the more inclined it is to use the denoising result globally*/
    AX_U16                           nSfCHfCurWtGain;                     /* Display: cnr.hf.curWtGain, Accuracy: U4.8, Range: [0, 4095], When provided to the output frame, the transition speed of the time-domain fusion coefficient between the high-frequency denoising result and the denoised image fusion is faster, and the larger the transition speed, the more inclined it is to use the denoising result globally*/
    AX_U8                            nSfCHfCurRatio;                      /* Display: cnr.hf.curRatio, Accuracy: U1.7, Range: [0, 128], When provided to the output frame, the ratio of the time-domain fusion coefficient between the high-frequency denoising result and the denoised image fusion is higher, and the larger the ratio, the more inclined it is to use the denoising result globally. The larger the value, the lower the clarity of the stationary area UV and the smaller the color noise.*/
    AX_U16                           nSfCHfThr;                           /* Display: cnr.hf.thr, Accuracy: U8.2, Range: [0, 1023], High frequency denoising threshold, the larger the value, the stronger the denoising intensity*/
    AX_U16                           nSfCHfGain;                          /* Display: cnr.hf.gain, Accuracy: U2.8, Range: [0, 1023], High frequency denoising transition speed, the larger the value, the weaker the denoising intensity*/
    AX_U8                            nSfCHfHdrAdjGain;                    /* Display: cnr.hf.hdrAdjGain, Accuracy: U1.5, Range: [0, 32], Short frame current frame color high-frequency noise reduction intensity correction, the larger the value, the greater the high-frequency noise reduction intensity of the short frame*/
    AX_U8                            nSfCHfOriStr;                        /* Display: cnr.hf.oriStr, Accuracy: U1.5, Range: [0, 32], High frequency color fading noise and aliasing noise, the larger the value, the closer it is to the original image*/
    AX_U8                            nEhcCWin;                            /* Display: cnr.ehc.win, Accuracy: U3.0, Range: [0, 5], The size of the window for enhance color sf filtering, the larger the value, the greater the intensity of color noise removal*/
    AX_U8                            nEhcCStr;                            /* Display: cnr.ehc.str, Accuracy: U8.0, Range: [1, 255], The strength for enhance color sf filtering to remove color noise, the larger the value, the greater the intensity of color noise removal*/
    AX_U8                            nEhcCDiffThr;                        /* Display: cnr.ehc.diffThr, Accuracy: U8.0, Range: [0, 255], The diff protection strength of enhance color sf filtering for color noise reduction results is weaker as the value increases, and closer to non enhance color sf filtering as the value decreases*/
    AX_U8                            nEhcCSatThr;                         /* Display: cnr.ehc.satThr, Accuracy: U7.0, Range: [0, 127], The results of enhance color sf filtering for removing color noise are based on the sat protection strength. The larger the value, the more inclined it is to use the enhance color sf filtering result*/
    AX_U16                           nEhcCSatGain;                        /* Display: cnr.ehc.satGain, Accuracy: U1.8, Range: [0, 511], The enhance color sf filtering denoising result is based on the transition speed of SAT protection. The larger the value, the more inclined it is to use the original denoising result*/
    AX_U8                            nEhcCSatRatio[YUV3DNR_REGS_GROUP_NUM]; /* Display: cnr.ehc.satRatio, Accuracy: U1.7, Range: [0, 128], The enhance color sf filtering denoising result is based on the fusion ratio between sat and the original denoising result. The larger the value, the more inclined it is to the original denoising result*/
    AX_U8                            nEhcCRatioThr;                       /* Display: cnr.ehc.ratioThr, Accuracy: U1.7, Range: [0, 128], Determine the time-domain fusion coefficient threshold for the fusion enhance color sf filtering result of the dynamic and static regions based on the time-domain fusion coefficient. It is hoped that different fusion ratios will be used for the dynamic and static regions*/
    AX_S16                           nEhcCRatioGain;                      /* Display: cnr.ehc.ratioGain, Accuracy: S7.7, Range: [-16384, 16383], Determine the transition speed of the fusion enhance color sf filtering result for the dynamic and static regions based on the time-domain fusion coefficient. It is hoped that different fusion ratios will be used for the dynamic and static regions*/
    AX_U8                            nEhcCRatio[YUV3DNR_REGS_GROUP_NUM];    /* Display: cnr.ehc.ratio, Accuracy: U1.7, Range: [0, 128], According to the time-domain fusion coefficient, the proportion of the fusion enhance color sf filtering results in the dynamic and static regions is determined. The first value corresponds to the static region, and the second value corresponds to the motion region. The larger the value, the more inclined it is to use the guided filtering results*/
    AX_U8                            nTfYEn;                              /* Display: enable.tfYEn, Accuracy: U1.0, Range: [0, 1], y tf enable*/
    AX_U8                            nTfCEn;                              /* Display: enable.tfCEn, Accuracy: U1.0, Range: [0, 1], uv tf enable*/
    AX_U8                            nSfYLfEn;                            /* Display: enable.sfYLfEn, Accuracy: U1.0, Range: [0, 1], y lf sf enable*/
    AX_U8                            nSfYHfEn;                            /* Display: enable.sfYHfEn, Accuracy: U1.0, Range: [0, 1], y hf sf enable*/
    AX_U8                            nSfCLfEn;                            /* Display: enable.sfCLfEn, Accuracy: U1.0, Range: [0, 1], uv lf sf enable*/
    AX_U8                            nSfCHfEn;                            /* Display: enable.sfCHfEn, Accuracy: U1.0, Range: [0, 1], uv hf sf enable*/
    AX_U8                            nEhcCEn;                             /* Display: enable.ehcCEn, Accuracy: U1.0, Range: [0, 1], uv gf enable*/
    AX_U8                            nTwtSfEn;                            /* Display: enable.twtSfEn, Accuracy: U1.0, Range: [0, 1], tweight sf enable*/
    AX_U8                            nExtMaskEn;                          /* Display: enable.extMaskEn, Accuracy: U1.0, Range: [0, 1], ext mask enable*/
} AX_ISP_IQ_YUV3DNR_MANUAL_T;

typedef struct {
    AX_U8                            nYuv3dnrEn;                          /* yuv3dnr enable, Accuracy: U1.0 Range: [0, 1], 0: Disable, 1: Enable */
    AX_ISP_IQ_YUV3DNR_MANUAL_T       tManualParam;
} AX_ISP_IQ_YUV3DNR_PARAM_T;

/************************************************************************************
 *  Sharpen IQ Param
 ************************************************************************************/
#define AX_ISP_SHP_GAIN_SIZE               (33)
#define AX_ISP_SHP_MOTION_MASK_LUT_SIZE    (9)
#define AX_ISP_SHP_SKIN_CENTER_NUM         (2)
#define AX_ISP_SHP_SKIN_RADIUS_NUM         (2)

typedef struct {
    AX_U16                           nUdGainLut[AX_ISP_SHP_GAIN_SIZE];                      /* Display: udGainLut, Accuracy: U8.4, Range: [0, 4095], set undirectional gain w.r.t. texture strength*/
    AX_U8                            nUdGainLutAnchor;                    /* Display: udGainLutAnchor, Accuracy: U3.0, Range: [0, 7], smaller value means more anchor points in flat/weak texture regions*/
    AX_U16                           nDirGainLut[AX_ISP_SHP_GAIN_SIZE];                     /* Display: dirGainLut, Accuracy: U8.4, Range: [0, 4095], set directional gain w.r.t. edge strength*/
    AX_U8                            nDirGainLutAnchor;                   /* Display: dirGainLutAnchor, Accuracy: U3.0, Range: [0, 7], smaller value means more anchor points in flat/weak texture regions*/
    AX_U16                           nUdFreq;                             /* Display: udFreq, Accuracy: U5.5, Range: [0, 1023], larger value means higher undirectional frequecny will be enhanced*/
    AX_U16                           nDirFreq;                            /* Display: dirFreq, Accuracy: U5.5, Range: [0, 1023], larger value means higher directional frequecny will be enhanced*/
    AX_U8                            nEdgeStr;                            /* Display: edgeStr, Accuracy: U4.0, Range: [0, 15], larger value means to use more directional filter's result*/
    AX_U16                           nEdgeThr;                            /* Display: edgeThr, Accuracy: U8.4, Range: [0, 4095], larger value means less pixels will be considered as edge pixels*/
    AX_U8                            nEdgeSmoothThinRatio;                /* Display: edgeSmoothThinRatio, Accuracy: U4.0, Range: [0, 15], larger value means to make the edge much thinner*/
    AX_U8                            nOvershoot;                          /* Display: overshoot, Accuracy: U0.7, Range: [0, 127], larger value means stronger overshoot*/
    AX_U8                            nUndershoot;                         /* Display: undershoot, Accuracy: U0.7, Range: [0, 127], larger value means stronger undershoot*/
    AX_U8                            nDetailRegionThr;                    /* Display: detailRegionThr, Accuracy: U8.0, Range: [0, 255], larger value means more pixels will be considered as detail region*/
    AX_S8                            nDetailOvershootAdj;                 /* Display: detailOvershootAdj, Accuracy: S0.7, Range: [-127, 127], larger value means stronger overshoot for detail region*/
    AX_S8                            nDetailUndershootAdj;                /* Display: detailUndershootAdj, Accuracy: S0.7, Range: [-127, 127], larger value means stronger undershoot for detail region*/
    AX_U8                            nShootReleaseRatio;                  /* Display: shootReleaseRatio, Accuracy: U1.4, Range: [0, 16], larger value means shoot suppression tends to ignore more weak texture/detail region*/
    AX_U8                            nShootSupRange;                      /* Display: shootSupRange, Accuracy: U8.0, Range: [0, 255], larger value means shoot suppression tends to control more edges*/
    AX_U8                            nShootSupStr;                        /* Display: shootSupStr, Accuracy: U0.7, Range: [0, 127], larger value means stronger shoot suppression on edges*/
    AX_U8                            nShpExtraGain;                       /* Display: shpExtraGain, Accuracy: U4.4, Range: [0, 255], larger value means more extra gain applied to ud/dir gain lut*/
    AX_U16                           nShpLimit;                           /* Display: shpLimit, Accuracy: U5.5, Range: [0, 1023], smaller value means more restriction on the pixel value change, and may reduce the number of white/black points*/
    AX_U8                            nLumaGainLutNeg[AX_ISP_SHP_GAIN_SIZE];                 /* Display: lumaGainLutNeg, Accuracy: U1.5, Range: [0, 63], adjust sharpen gain for negative hf based on image brightness*/
    AX_U8                            nLumaGainLutPos[AX_ISP_SHP_GAIN_SIZE];                 /* Display: lumaGainLutPos, Accuracy: U1.5, Range: [0, 63], adjust sharpen gain for positive hf based on image brightness*/
    AX_U8                            nRedGain;                            /* Display: redGain, Accuracy: U1.5, Range: [0, 32], smaller value means less sharpen for red region*/
    AX_U8                            nGreenGain;                          /* Display: greenGain, Accuracy: U3.5, Range: [0, 255], smaller value means less sharpen for green region*/
    AX_U8                            nBlueGain;                           /* Display: blueGain, Accuracy: U1.5, Range: [0, 32], smaller value means less sharpen for blue region*/
    AX_U8                            nSkinGain;                           /* Display: skinGain, Accuracy: U1.5, Range: [0, 32], smaller value means less sharpen for skin region*/
    AX_S16                           nSkinCenter[AX_ISP_SHP_SKIN_CENTER_NUM];                      /* Display: skinCenter, Accuracy: S7.2, Range: [-512, 511], skin color center*/
    AX_U16                           nSkinRadius[AX_ISP_SHP_SKIN_RADIUS_NUM];                      /* Display: skinRadius, Accuracy: U8.2, Range: [0, 1023], skin color radius*/
    AX_U8                            nMotionMaskEnable;                   /* Display: motionMaskEnable, Accuracy: U1.0, Range: [0, 1], motion mask enable*/
    AX_U8                            nMotionOvershoot;                    /* Display: motionOvershoot, Accuracy: U0.7, Range: [0, 127], larger value means stronger overshoot*/
    AX_U8                            nMotionUndershoot;                   /* Display: motionUndershoot, Accuracy: U0.7, Range: [0, 127], larger value means stronger undershoot*/
    AX_U16                           nMotionUdGainLut[AX_ISP_SHP_GAIN_SIZE];                /* Display: motionUdGainLut, Accuracy: U8.4, Range: [0, 4095], set undirectional gain w.r.t. texture strength*/
    AX_U16                           nMotionDirGainLut[AX_ISP_SHP_GAIN_SIZE];               /* Display: motionDirGainLut, Accuracy: U8.4, Range: [0, 4095], set directional gain w.r.t. edge strength*/
    AX_U8                            nMotionMaskLut[AX_ISP_SHP_MOTION_MASK_LUT_SIZE];                   /* Display: motionMaskLut, Accuracy: U0.8, Range: [0, 255], used to threshold/reverse the input motion lut*/
    AX_U8                            nNrStr;                              /* Display: nrStr, Accuracy: U1.5, Range: [0, 32], larger value means stronger noise reduction*/
    AX_U8                            nNrUdTextureSens;                    /* Display: nrUdTextureSens, Accuracy: U0.8, Range: [0, 255], larger value means image has more undirectional textures and apply less noise reduction*/
    AX_U8                            nNrUdTextureThr;                     /* Display: nrUdTextureThr, Accuracy: U0.8, Range: [0, 255], larger value means less pixels will be considered as real texture and apply more noise reduction*/
    AX_U16                           nNrUdLimit;                          /* Display: nrUdLimit, Accuracy: U8.2, Range: [0, 1023], larger value means less restriction on undirectional noise reduction*/
    AX_U8                            nUdCoring;                           /* Display: udCoring, Accuracy: U3.3, Range: [0, 63], larger value means stronger coring for undirectional details*/
    AX_U8                            nNrDirEdgeSens;                      /* Display: nrDirEdgeSens, Accuracy: U0.8, Range: [0, 255], larger value means image has more directional edges and apply less noise reduction*/
    AX_U8                            nNrDirEdgeThr;                       /* Display: nrDirEdgeThr, Accuracy: U4.4, Range: [0, 255], larger value means less pixels will be considered as real edge and apply more noise reduction*/
    AX_U16                           nNrDirLimit;                         /* Display: nrDirLimit, Accuracy: U8.2, Range: [0, 1023], larger value means less restriction on directional noise reduction*/
    AX_U8                            nDirCoring;                          /* Display: dirCoring, Accuracy: U3.3, Range: [0, 63], larger value means stronger coring for directional details*/
} AX_ISP_IQ_SHARPEN_MANUAL_T;

typedef struct {
    AX_U8                            nSharpenEn;                          /* sharpen enable, Accuracy: U1.0 Range: [0, 1], 0: Disable, 1: Enable */
    AX_ISP_IQ_SHARPEN_MANUAL_T       tManualParam;
} AX_ISP_IQ_SHARPEN_PARAM_T;

/************************************************************************************
 *  DEPURPLE IQ Param
 ************************************************************************************/
#define AX_ISP_DEPURPLE_DET_C_RATIO_NUM     (4)
#define AX_ISP_DEPURPLE_DET_ADJ_MESH_NUM    (17)
#define AX_ISP_DEPURPLE_DET_THR_Y_MESH_NUM  (17)
#define AX_ISP_DEPURPLE_SELEDGE_WGT_NUM     (7)
#define AX_ISP_DEPURPLE_DIALATE_WGT_NUM     (7)
#define AX_ISP_DEPURPLE_DESAT_LUMA_LUT_NUM  (8)
#define AX_ISP_DEPURPLE_DESAT_SAT_LUT_NUM   (6)
#define AX_ISP_DEPURPLE_DESAT_ANGLE_LUT_NUM (16)
#define AX_ISP_DEPURPLE_DESAT_UV_VAL_NUM    (2)
#define AX_ISP_DEPURPLE_DESAT_DBG_COLOR_NUM (3)

typedef struct {
    AX_S8                            nDetCRatio[AX_ISP_DEPURPLE_DET_C_RATIO_NUM];                       /* Display: detCRatio, Accuracy: S1.6, Range: [-64, 64], in pfd hardware, weight for computr color channel using bayer*/
    AX_U8                            nDetAdjYGam;                         /* Display: detAdjYGam, Accuracy: U3.5, Range: [0, 224], for gamma correction to meet with decent gam*/
    AX_U8                            nDetEdgeCEnable;                     /* Display: detEdgeCEnable, Accuracy: U1.0, Range: [0, 1], in pfd hardware, for color channel edge offset*/
    AX_U8                            nDetEdgeSlopeY;                      /* Display: detEdgeSlopeY, Accuracy: U1.6, Range: [0, 64], in pfd hardware, for luma channel edge gain*/
    AX_S8                            nDetEdgeOffsetY;                     /* Display: detEdgeOffsetY, Accuracy: S1.6, Range: [-64, 64], in pfd hardware, for luma channel edge offset*/
    AX_U8                            nDetEdgeSlopeC;                      /* Display: detEdgeSlopeC, Accuracy: U1.6, Range: [0, 64], in pfd hardware, for color channel edge gain*/
    AX_S8                            nDetEdgeOffsetC;                     /* Display: detEdgeOffsetC, Accuracy: S1.6, Range: [-64, 64], in pfd hardware, for color channel edge offset*/
    AX_U16                           nDetSeledgeSlopeY;                   /* Display: detSeledgeSlopeY, Accuracy: U1.8, Range: [0, 256], in pfd hardware, for compensate luma with slope for*/
    AX_U16                           nDetSeledgeThrY;                     /* Display: detSeledgeThrY, Accuracy: U8.6, Range: [0, 16383], for luma compensation when detect edge, used to compute for mesh-wisely*/
    AX_U8                            nDetSeledgeThrYMesh[AX_ISP_DEPURPLE_DET_THR_Y_MESH_NUM][AX_ISP_DEPURPLE_DET_THR_Y_MESH_NUM];         /* Display: detSeledgeThrYMesh, Accuracy: U1.7, Range: [0, 128], location wise strength adjust for edge compensation*/
    AX_U8                            nDetSeledgeWeight[AX_ISP_DEPURPLE_SELEDGE_WGT_NUM];                /* Display: detSeledgeWeight, Accuracy: U1.6, Range: [0, 64], in pfd hardware, edge detection weight*/
    AX_U16                           nDetMaskStr;                         /* Display: detMaskStr, Accuracy: U3.6, Range: [0, 511],  in pfd hardware, strength adjust for mask*/
    AX_U8                            nDetMaskDialateEnable;               /* Display: detMaskDialateEnable, Accuracy: U1.0, Range: [0, 1],  in pfd hardware, dialate mask enable*/
    AX_U8                            nDetMaskDialateWeight[AX_ISP_DEPURPLE_DIALATE_WGT_NUM];            /* Display: detMaskDialateWeight, Accuracy: U1.6, Range: [0, 64],  in pfd hardware, dialate weight for mask*/
    AX_U8                            nDetAdjMeshEnable;                   /* Display: detAdjMeshEnable, Accuracy: U1.0, Range: [0, 1], in pfd hardware, for enable locationwise strength adjustment*/
    AX_U8                            nDetAdjMesh[AX_ISP_DEPURPLE_DET_ADJ_MESH_NUM][AX_ISP_DEPURPLE_DET_ADJ_MESH_NUM];                 /* Display: detAdjMesh, Accuracy: U2.6, Range: [0, 255], in pfd hardware, mesh for adjust depurple strength*/
    AX_U8                            nDesatStr;                           /* Display: desatStr, Accuracy: U4.4, Range: [0, 240], in lce hardware, desat_strength*/
    AX_U8                            nDesatLumaLut[AX_ISP_DEPURPLE_DESAT_LUMA_LUT_NUM];                    /* Display: desatLumaLut, Accuracy: U1.7, Range: [0, 128], in lce hardware, desat_luma_lut*/
    AX_U8                            nDesatSatLut[AX_ISP_DEPURPLE_DESAT_SAT_LUT_NUM];                     /* Display: desatSatLut, Accuracy: U1.7, Range: [0, 128], in lce hardware, desat_sat_lut*/
    AX_U8                            nDesatAngleRatioLut[AX_ISP_DEPURPLE_DESAT_ANGLE_LUT_NUM];             /* Display: desatAngleRatioLut, Accuracy: U1.7, Range: [0, 128], in lce hardware, will calculate 16 phase ratio for clc hardware, color adjust 16 phase ratio for mask receiving from pfd*/
    AX_S16                           nDesatUvVal[AX_ISP_DEPURPLE_DESAT_UV_VAL_NUM];                      /* Display: desatUvVal, Accuracy: S7.2, Range: [-511, 511], in lce hardware, depurple aera fill color with UV*/
    AX_U8                            nDesatDebugEnable;                   /* Display: desatDebugEnable, Accuracy: U1.0, Range: [0, 1], in lce hardware, desat debug enable*/
    AX_U8                            nDesatDebugThr;                      /* Display: desatDebugThr, Accuracy: U1.4, Range: [0, 16], in lce hardware, debug depurple threshold for showing debug info*/
    AX_S16                           nDesatDebugColor[AX_ISP_DEPURPLE_DESAT_DBG_COLOR_NUM];                 /* Display: desatDebugColor, Accuracy: S8.2, Range: [-1023, 1023], in lce hardware, debug mode's color fill for depurple aera with YUV*/
} AX_ISP_IQ_DEPURPLE_MANUAL_T;

typedef struct {
    AX_U8                            nDepurpleEn;                         /* depurple enable, Accuracy: U1.0 Range: [0, 1], 0: Disable, 1: Enable */
    AX_ISP_IQ_DEPURPLE_MANUAL_T      tManualParam;
} AX_ISP_IQ_DEPURPLE_PARAM_T;

/************************************************************************************
 *  HS2DLUT IQ Param: HSV CONTROL
 ************************************************************************************/
#define AX_ISP_HS2DLUT_H_SIZE            (24)
#define AX_ISP_HS2DLUT_V_SIZE            (16)

typedef struct {
    AX_U16                           nHs2dlutHueTable[AX_ISP_HS2DLUT_H_SIZE][AX_ISP_HS2DLUT_V_SIZE];            /* Display: hs2dlutHueTable, Accuracy: U9.7, Range: [0, 46080], */
    AX_U16                           nHs2dlutSatTable[AX_ISP_HS2DLUT_H_SIZE][AX_ISP_HS2DLUT_V_SIZE];            /* Display: hs2dlutSatTable, Accuracy: U1.15, Range: [0, 32768], */
} AX_ISP_IQ_HS2DLUT_MANUAL_T;

typedef struct {
    AX_U8                            nHs2dlutEn;                          /* hs2dlut enable, Accuracy: U1.0 Range: [0, 1], 0: Disable, 1: Enable */
    AX_ISP_IQ_HS2DLUT_MANUAL_T       tManualParam;
} AX_ISP_IQ_HS2DLUT_PARAM_T;

/************************************************************************************
 *  CCMP IQ Param: CHROMA COMP
 ************************************************************************************/
#define AX_ISP_CCMP_Y_SIZE             (29)
#define AX_ISP_CCMP_SAT_SIZE           (23)

typedef struct {
    AX_U16                           nCcmpYLut[AX_ISP_CCMP_Y_SIZE];                       /* Display: ccmpYLut, Accuracy: U1.9, Range: [0, 512], smaller value means stronger saturation compression*/
    AX_U16                           nCcmpSatLut[AX_ISP_CCMP_SAT_SIZE];                     /* Display: ccmpSatLut, Accuracy: U1.9, Range: [0, 512], smaller value means stronger saturation compression*/
} AX_ISP_IQ_CCMP_MANUAL_T;

typedef struct {
    AX_U8                            nCcmpEn;                             /* ccmp enable, Accuracy: U1.0 Range: [0, 1], 0: Disable, 1: Enable */
    AX_ISP_IQ_CCMP_MANUAL_T          tManualParam;
} AX_ISP_IQ_CCMP_PARAM_T;

/************************************************************************************
 *  SCM IQ Param: Special Color Mapping
 ************************************************************************************/
#define AX_ISP_SCM_COLOR_SIZE          (2)
#define AX_ISP_SCM_MASK_CENTER_UV_SIZE (2)
#define AX_ISP_SCM_MASK_SIZE           (3)

typedef struct {
    AX_U8                            nScmInvSel;                          /* Display: scmInvSel, Accuracy: U1.0, Range: [0, 1], 0: normal selection, 1: inverse selection*/
    AX_U16                           nScmSrcY;                            /* Display: scmSrcY, Accuracy: U8.2, Range: [0, 1023], color mask center y*/
    AX_S16                           nScmSrcUv[AX_ISP_SCM_MASK_CENTER_UV_SIZE];                        /* Display: scmSrcUv, Accuracy: S7.2, Range: [-512, 511], color mask center uv*/
    AX_U16                           nScmRadius[AX_ISP_SCM_MASK_SIZE];                       /* Display: scmRadius, Accuracy: U7.2, Range: [0, 511], color mask radius, [0]: y, [1]: u, [2]: v*/
    AX_U8                            nScmSmooth[AX_ISP_SCM_MASK_SIZE];                       /* Display: scmSmooth, Accuracy: U3.0, Range: [0, 7], color mask transition band*/
    AX_S16                           nScmDstUv[AX_ISP_SCM_COLOR_SIZE];                        /* Display: scmDstUv, Accuracy: S7.2, Range: [-512, 511], target color*/
} AX_ISP_IQ_SCM_MANUAL_T;

typedef struct {
    AX_U8                            nScmEn;                              /* scm enable, Accuracy: U1.0 Range: [0, 1], 0: Disable, 1: Enable */
    AX_ISP_IQ_SCM_MANUAL_T           tManualParam;
} AX_ISP_IQ_SCM_PARAM_T;

/************************************************************************************
 *  YCPROC IQ Param: COLOR PROCESS
 ************************************************************************************/
#define AX_ISP_YCRT_SIZE         (2)

typedef struct {
    AX_U16                           nYcprocBrightness;                   /* Display: ycprocBrightness, Accuracy: U4.8, Range: [0, 4095], brightness control*/
    AX_S16                           nYcprocContrast;                     /* Display: ycprocContrast, Accuracy: S4.8, Range: [-4096, 4095], contrast control*/
    AX_S16                           nYcprocHue;                          /* Display: ycprocHue, Accuracy: S0.15, Range: [-32768, 32767], hue control*/
    AX_U16                           nYcprocSat;                          /* Display: ycprocSat, Accuracy: U4.12, Range: [0, 65535], saturation control*/
    AX_U8                            nYcrtEnable;                         /* Display: ycrtEnable, Accuracy: U1.0, Range: [0, 1], 0: bypass (limited range [HDMI]), 1: enable*/
    AX_U8                            nYcrtSignalRangeMode;                /* Display: ycrtSignalRangeMode, Accuracy: U1.0, Range: [0, 1], 0: full range, 1: limited range (survilliance)*/
    AX_U8                            nYcrtClipLevelY[AX_ISP_YCRT_SIZE];                  /* Display: ycrtClipLevelY, Accuracy: U8.0, Range: [0, 255], y clip level*/
    AX_S8                            nYcrtClipLevelUv[AX_ISP_YCRT_SIZE];                 /* Display: ycrtClipLevelUv, Accuracy: S7.0, Range: [-128, 127], uv clip level*/
    AX_U16                           nInputRangeY[AX_ISP_YCRT_SIZE];       /* Accuracy: U8.2, Range: [0, 1023], input range Y*/
    AX_U16                           nOutputRangeY[AX_ISP_YCRT_SIZE];      /* Accuracy: U8.2, Range: [0, 1023], output range Y*/
    AX_S16                           nInputRangeUV[AX_ISP_YCRT_SIZE];      /* Accuracy: S7.2, Range: [-512, 511], input range UV*/
    AX_S16                           nOutputRangeUV[AX_ISP_YCRT_SIZE];     /* Accuracy: S7.2, Range: [-512, 511], output range UV*/
} AX_ISP_IQ_YCPROC_MANUAL_T;

typedef struct {
    AX_U8                            nYcprocEn;                           /* ycproc enable, Accuracy: U1.0 Range: [0, 1], 0: Disable, 1: Enable */
    AX_ISP_IQ_YCPROC_MANUAL_T        tManualParam;
} AX_ISP_IQ_YCPROC_PARAM_T;

/************************************************************************************
 *  Y2DNR IQ Param
 ************************************************************************************/
#define AX_ISP_STRENGTH_FG_SIZE     (2)
#define AX_ISP_STRENGTH_BG_SIZE     (2)
#define AX_ISP_DP_GAIN_SIZE         (9)
#define AX_ISP_LOCAL_STRENGTH_SIZE  (17)
#define AX_ISP_DIR_STRENGTH_SIZE    (2)
#define AX_ISP_DETAIL_STRENGTH_SIZE (2)
#define AX_ISP_FLAT_STRENGTH_SIZE   (2)
#define AX_ISP_LUMA_GAIN_FG_SIZE    (17)
#define AX_ISP_LUMA_GAIN_BG_SIZE    (17)
#define AX_ISP_MOTION_LUT_SIZE      (9)

typedef struct {
    AX_U8                            nMotionEnable;                       /* Display: motionEnable, Accuracy: U1.0, Range: [0, 1], motion enable, 0:disable, 1:enable*/
    AX_U8                            nDpcEnable;                          /* Display: dpcEnable, Accuracy: U1.0, Range: [0, 1], dpc enable, 0:disable, 1:enable*/
    AX_U8                            nDirStr;                             /* Display: dirStr, Accuracy: U0.4, Range: [0, 15], direction detection strength*/
    AX_U16                           nDirBase;                            /* Display: dirBase, Accuracy: U8.4, Range: [0, 4095], base noise for direciton detection*/
    AX_U16                           nDirDetCorStrFg[AX_ISP_STRENGTH_FG_SIZE];                  /* Display: dirDetCorStrFg, Accuracy: U8.2, Range: [0, 1023], direction detection correction strength for foreground*/
    AX_U16                           nDirDetCorStrBg[AX_ISP_STRENGTH_BG_SIZE];                  /* Display: dirDetCorStrBg, Accuracy: U8.2, Range: [0, 1023], direction detection correction strength for background*/
    AX_U16                           nDetailDetStrFg[AX_ISP_STRENGTH_FG_SIZE];                  /* Display: detailDetStrFg, Accuracy: U8.2, Range: [0, 1023], detail detection strength for foreground*/
    AX_U16                           nDetailDetStrBg[AX_ISP_STRENGTH_BG_SIZE];                  /* Display: detailDetStrBg, Accuracy: U8.2, Range: [0, 1023], detail detection strength for background*/
    AX_U8                            nDpDetMode;                          /* Display: dpDetMode, Accuracy: U1.0, Range: [0, 1], dp detection mode, 0:signgle, 1:dual*/
    AX_U8                            nDpDetStr;                           /* Display: dpDetStr, Accuracy: U4.4, Range: [0, 255], dp detection strength*/
    AX_U16                           nDpStr;                              /* Display: dpStr, Accuracy: U4.8, Range: [0, 4095], dp strength*/
    AX_U8                            nDpNoiseLevel;                       /* Display: dpNoiseLevel, Accuracy: U8.0, Range: [0, 255], dp noise level*/
    AX_U8                            nDpNoiseStr[AX_ISP_DP_GAIN_SIZE];                      /* Display: dpNoiseStr, Accuracy: U0.8, Range: [0, 255], dp noise gain*/
    AX_U8                            nDpDetailGain[AX_ISP_DP_GAIN_SIZE];                    /* Display: dpDetailGain, Accuracy: U0.8, Range: [0, 255], dp detail gain*/
    AX_U16                           nSf1Str;                             /* Display: sf1Str, Accuracy: U1.8, Range: [0, 256], sf1 strength, the larger the value, the more nosie reduction*/
    AX_U8                            nSf1LocalStr[AX_ISP_LOCAL_STRENGTH_SIZE];                    /* Display: sf1LocalStr, Accuracy: U0.8, Range: [0, 255], sf1 local strength, the larger the value, the more nosie reduction*/
    AX_U8                            nSf1StyleAdj;                        /* Display: sf1StyleAdj, Accuracy: U8.0, Range: [0, 255], sf1 style adjust, the larger the value, the more nosie reduction, also may be more annoying patterns*/
    AX_U8                            nSf1DirStrScale;                     /* Display: sf1DirStrScale, Accuracy: U1.4, Range: [0, 16], sf1 direction strength scale, the larger the value, the more direction noise reduction result used*/
    AX_U16                           nSf1DirStr[AX_ISP_DIR_STRENGTH_SIZE];                       /* Display: sf1DirStr, Accuracy: U8.2, Range: [0, 1023], sf1 direction strength, the larger the value, the more noise reduction*/
    AX_U16                           nSf1DetailStr[AX_ISP_DETAIL_STRENGTH_SIZE];                    /* Display: sf1DetailStr, Accuracy: U8.2, Range: [0, 1023], sf1 detail strength, the larger the value, the more noise reduction*/
    AX_U16                           nSf1FlatStr[AX_ISP_FLAT_STRENGTH_SIZE];                      /* Display: sf1FlatStr, Accuracy: U8.2, Range: [0, 1023], sf1 flat strength, the larger the value, the more noise reduction*/
    AX_U16                           nSf2Str;                             /* Display: sf2Str, Accuracy: U1.8, Range: [0, 256], sf2 strength, the larger the value, the more nosie reduction*/
    AX_U8                            nSf2LocalStr[AX_ISP_LOCAL_STRENGTH_SIZE];                    /* Display: sf2LocalStr, Accuracy: U0.8, Range: [0, 255], sf2 local strength, the larger the value, the more nosie reduction*/
    AX_U8                            nSf2StyleAdj;                        /* Display: sf2StyleAdj, Accuracy: U8.0, Range: [0, 255], sf2 style adjust, the larger the value, the more nosie reduction, also may be more annoying patterns*/
    AX_U16                           nSf2DirStr[AX_ISP_DIR_STRENGTH_SIZE];                       /* Display: sf2DirStr, Accuracy: U8.2, Range: [0, 1023], sf2 direction strength, the larger the value, the more noise reduction*/
    AX_U16                           nSf2DetailStr[AX_ISP_DETAIL_STRENGTH_SIZE];                    /* Display: sf2DetailStr, Accuracy: U8.2, Range: [0, 1023], sf2 detail strength, the larger the value, the more noise reduction*/
    AX_U16                           nSf2FlatStr[AX_ISP_FLAT_STRENGTH_SIZE];                      /* Display: sf2FlatStr, Accuracy: U8.2, Range: [0, 1023], sf2 flat strength, the larger the value, the more noise reduction*/
    AX_U8                            nEdgeStyleAdjFg;                     /* Display: edgeStyleAdjFg, Accuracy: U0.8, Range: [0, 255], edge style adjust for foreground, the larger the value, the smoother the edge, the smaller the vaule, the more annoying pixel along edge*/
    AX_U8                            nDetailStyleAdjFg;                   /* Display: detailStyleAdjFg, Accuracy: U0.8, Range: [0, 255], detail style adjust for foreground, the larger the value, maybe the more annoying patterns in detail region*/
    AX_U8                            nFlatStyleAdjFg;                     /* Display: flatStyleAdjFg, Accuracy: U0.8, Range: [0, 255], flat style adjust for foreground, the larger the value, maybe the more annoying patterns in flat region*/
    AX_U8                            nEdgeStyleAdjBg;                     /* Display: edgeStyleAdjBg, Accuracy: U0.8, Range: [0, 255], edge style adjust for background, the larger the value, the smoother the edge, the smaller the vaule, the more annoying pixel along edge*/
    AX_U8                            nDetailStyleAdjBg;                   /* Display: detailStyleAdjBg, Accuracy: U0.8, Range: [0, 255], detail style adjust for background, the larger the value, maybe the more annoying patterns in detail region*/
    AX_U8                            nFlatStyleAdjBg;                     /* Display: flatStyleAdjBg, Accuracy: U0.8, Range: [0, 255], flat style adjust for background, the larger the value, maybe the more annoying patterns in flat region*/
    AX_U16                           nLumaGainFg[AX_ISP_LUMA_GAIN_FG_SIZE];                     /* Display: lumaGainFg, Accuracy: U9.2, Range: [0, 2047], luma adjust strength for foreground, the larger the value, more noise reduciton in corresponding luma region*/
    AX_U16                           nLumaGainBg[AX_ISP_LUMA_GAIN_BG_SIZE];                     /* Display: lumaGainBg, Accuracy: U9.2, Range: [0, 2047], luma adjust strength for background, the larger the value, more noise reduciton in corresponding luma region*/
    AX_U16                           nLumaGainFgEx[AX_ISP_LUMA_GAIN_FG_SIZE];                   /* Display: lumaGainFgEx, Accuracy: U3.8, Range: [0, 2047], extension luma adjust strength for foreground, the larger the value, more noise reduciton in corresponding luma region, used when luma_gain_fg is not enough*/
    AX_U16                           nLumaGainBgEx[AX_ISP_LUMA_GAIN_BG_SIZE];                   /* Display: lumaGainBgEx, Accuracy: U3.8, Range: [0, 2047], extension luma adjust strength for background, the larger the value, more noise reduciton in corresponding luma region, used when luma_gain_bg is not enough*/
    AX_U8                            nMotionLut[AX_ISP_MOTION_LUT_SIZE];                       /* Display: motionLut, Accuracy: U0.8, Range: [0, 255], motion mapping lut*/
} AX_ISP_IQ_Y2DNR_MANUAL_T;

typedef struct {
    AX_U8                            nY2dnrEn;                            /* y2dnr enable, Accuracy: U1.0 Range: [0, 1], 0: Disable, 1: Enable */
    AX_ISP_IQ_Y2DNR_MANUAL_T         tManualParam;
} AX_ISP_IQ_Y2DNR_PARAM_T;

/************************************************************************************
 *  DIS Param Config
 ************************************************************************************/
#define AX_ISP_DIS_POS_WEIGHT_SIZE     (16)

typedef struct {
    AX_U8                            bSwCalcEnable;                       /* Display: swCalcEnable, Accuracy: U1.0, Range: [0, 1], 0: use HW offset 1: use projection calculate offset*/
    AX_U8                            nDelayFrameNum;                      /* Display: delayFrameNum, Accuracy: U8.0, Range: [0, 15], delay num, the more, the smoother. defalut 0. Accuracy: U8 Range: [0, 15]*/
    AX_U8                            nHistoryFrameNum;                    /* Display: historyFrameNum, Accuracy: U8.0, Range: [1, 16], history frame, the more, the smoother*/
    AX_U8                            nCropRatio;                          /* Display: cropRatio, Accuracy: U8.0, Range: [127, 255], crop_ratio for warped image*/
    AX_U32                           nSadThr;                             /* Display: sadThr, Accuracy: U32.0, Range: [0, 65535], threshord for valid offset*/
    AX_U8                            nFramePosWeights[AX_ISP_DIS_POS_WEIGHT_SIZE];                /* Display: framePosWeights, Accuracy: U8.0, Range: [0, 255], the weight of history frame position for weighted sum*/
    AX_U8                            nScaleRatio;                         /* Display: scaleRatio, Accuracy: U8.0, Range: [0, 16], quant scale = (1 << scale ratio)*/
} AX_ISP_IQ_DIS_MANUAL_T;

typedef struct {
    AX_U8                            nDisEn;                              /* dis enable, Accuracy: U1.0 Range: [0, 1], 0: Disable, 1: Enable */
    AX_ISP_IQ_DIS_MANUAL_T           tManualParam;
} AX_ISP_IQ_DIS_PARAM_T;

/************************************************************************************
 *  ME Stat Info
 ************************************************************************************/
#define AX_ME_PROJ_NUM              (5)
#define AX_ME_PROJ_DIV_NUM          (5)
#define AX_ME_SAD_NUM               (256)

typedef struct {
    AX_U32 nSad[AX_ME_PROJ_NUM * AX_ME_PROJ_DIV_NUM][AX_ME_SAD_NUM]; /* Accuracy: U24.0 Range: [0, 16777215] */
    AX_S8  nOffset[AX_ME_PROJ_NUM][AX_ME_PROJ_DIV_NUM]; /* Accuracy: S7.0 Range: [-128, 127] */
} AX_ME_STAT_T;

typedef struct {
    AX_ME_STAT_T tMeHStat;
    AX_ME_STAT_T tMeVStat;
} AX_ISP_ME_STAT_INFO_T;

/************************************************************************************
 *  ME Param Config
 ************************************************************************************/
#define AX_ISP_ME_LUT_NUM         (33)
#define AX_ISP_ME_DIRECTION_NUM   (2)
#define AX_ISP_ME_ROI_PARAM_NUM   (4)

typedef struct {
    AX_U8                            nScaleRatio;                         /* Display: scaleRatio, Accuracy: U2.0, Range: [0, 2], scale ratio, 0: 1/1, 1: 1/2, 2: 1/4*/
    AX_U16                           nSearchRange[AX_ISP_ME_DIRECTION_NUM];                     /* Display: searchRange, Accuracy: U9.0, Range: [0, 504], eisstat post process search range, must be mutiple of 8*/
    AX_U8                            nProjVRegionNum[AX_ISP_ME_DIRECTION_NUM];                  /* Display: projVRegionNum, Accuracy: U3.0, Range: [1, 5], vertical projection region number*/
    AX_U8                            nProjHRegionNum[AX_ISP_ME_DIRECTION_NUM];                  /* Display: projHRegionNum, Accuracy: U3.0, Range: [1, 5], horizontal projection region number*/
    AX_U16                           nProjVRoi[AX_ISP_ME_ROI_PARAM_NUM];                        /* Display: projVRoi, Accuracy: U1.14, Range: [0, 16384], vertical projection normalized roi*/
    AX_U16                           nProjHRoi[AX_ISP_ME_ROI_PARAM_NUM];                        /* Display: projHRoi, Accuracy: U1.14, Range: [0, 16384], horizontal projection normalized roi*/
    AX_U8                            nLutEnable;                          /* Display: lutEnable, Accuracy: U1.0, Range: [0, 1], lut enable, 0: disable, 1: enable*/
    AX_U16                           nLut[AX_ISP_ME_LUT_NUM];                            /* Display: lut, Accuracy: U8.2, Range: [0, 1023], lut table*/
    AX_U8                            nProjVShiftbit;                      /* Display: projVShiftbit, Accuracy: U3.0, Range: [0, 7], vertical projection shift bit*/
    AX_U8                            nProjHShiftbit;                      /* Display: projHShiftbit, Accuracy: U3.0, Range: [0, 7], horizontal projection shift bit*/
} AX_ISP_IQ_ME_MANUAL_T;

typedef struct {
    AX_U8                            nMeEn;                               /* me enable, Accuracy: U1.0 Range: [0, 1], 0: Disable, 1: Enable */
    AX_ISP_IQ_ME_MANUAL_T            tManualParam;
} AX_ISP_IQ_ME_PARAM_T;

/************************************************************************************
 *  LDC IQ Param
 ************************************************************************************/
#define AX_ISP_LDC_V2_MATRIX_V_SIZE     (3)
#define AX_ISP_LDC_V2_MATRIX_H_SIZE     (3)
#define AX_ISP_LDC_V2_COEFF_MAX_NUM     (8)

typedef enum
{
    AX_ISP_IQ_LDC_TYPE_V1,     /* lens distortion correction version 1 */
    AX_ISP_IQ_LDC_TYPE_V2,     /* lens distortion correction version 2 */
} AX_ISP_IQ_LDC_TYPE_E;

typedef struct
{
     AX_BOOL bAspect;          /* whether aspect ration is keep. Accuracy: U1.0 Range: [0, 1]*/
     AX_S16  nXRatio;          /* field angle ration of horizontal, valid when bAspect = 0. Accuracy: S16.0 Range: [0, 100] */
     AX_S16  nYRatio;          /* field angle ration of vertical, valid when bAspect = 0. Accuracy: S16.0 Range: [0, 100] */
     AX_S16  nXYRatio;         /* field angle ration of all,valid when bAspect = 1. Accuracy: S16.0 Range: [0, 100] */
     AX_S16  nCenterXOffset;   /* horizontal offset of the image distortion center relative to image center. Accuracy: S16.0 Range: [-511, 511] */
     AX_S16  nCenterYOffset;   /* vertical offset of the image distortion center relative to image center. Accuracy: S16.0 Range: [-511, 511] */
     AX_S16  nDistortionRatio; /* LDC distortion ratio. [-10000, 0): pincushion distortion; (0, 10000]: barrel distortion. Accuracy: S16.0 Range: [-10000, 10000] */
     AX_S8   nSpreadCoef;      /* LDC spread coefficient. Accuracy: S8.0 Range: [-18, 18] */
} AX_ISP_IQ_LDC_V1_PARAM_T;

typedef struct
{
     AX_U32  nMatrix[AX_ISP_LDC_V2_MATRIX_V_SIZE][AX_ISP_LDC_V2_MATRIX_H_SIZE];  /* Camera Internal Parameter Matrix, {{nXFocus, 0, nXCenter}, {0, nYFocus, nYCenter}, {0, 0, 1}}; Accuracy: have 2 decimal numbers, real value = nMatrix / 100; Range: [0, 0xFFFFFFFF] */
     AX_S64  nDistortionCoeff[AX_ISP_LDC_V2_COEFF_MAX_NUM];                      /* Distortion Coefficients = (k1, k2, p1, p2, k3, k4, k5, k6) Accuracy: have 6 decimal numbers, real value = nDistortionCoeff / 1000000; Range: [-0x7FFFFFFFFFFFFFFF, 0x7FFFFFFFFFFFFFFF] */
} AX_ISP_IQ_LDC_V2_PARAM_T;

typedef struct
{
    AX_U8 nLdcEnable;                       /* LDC enable, Accuracy: U8.0 Range: [0, 1] */
    AX_U8 nType;                            /* LDC type, Accuracy: U8.0 Range: [AX_ISP_IQ_LDC_TYPE_V1, AX_ISP_IQ_LDC_TYPE_V2] */
    AX_ISP_IQ_LDC_V1_PARAM_T tLdcV1Param;   /* LDC V1 Param */
    AX_ISP_IQ_LDC_V2_PARAM_T tLdcV2Param;   /* LDC V2 Param */
} AX_ISP_IQ_LDC_PARAM_T;

/************************************************************************************
 *  ACP IQ Param
 ************************************************************************************/
#define AX_ISP_ACP_NOISE_PARAM_NUM  (4)
#define AX_ISP_ACP_SHOT_NOISE_NUM   (2)
#define AX_ISP_ACP_READ_NOISE_NUM   (3)

typedef enum {
    AX_AFS_DRC_DISABLE         = 0,
    AX_AFS_DRC_LINEAR_8_x      = 1,
    AX_AFS_DRC_GAM_8_x  = 2,
    AX_AFS_DRC_MSB_14_x  = 3,
    AX_AFS_DRC_GAM_14_x  = 4,
    AX_AFS_DRC_MOD_BUTT,
} AX_ISP_AFS_DRC_MODE_E;

typedef struct {
    AX_S32                           nHcgShotNoiseCoef[AX_ISP_ACP_NOISE_PARAM_NUM][AX_ISP_ACP_SHOT_NOISE_NUM];             /* Display: hcgShotNoiseCoef, Accuracy: S0.31, Range: [-2147483648, 2147483647], Hcg Calibration Params*/
    AX_S32                           nHcgReadNoiseCoef[AX_ISP_ACP_NOISE_PARAM_NUM][AX_ISP_ACP_READ_NOISE_NUM];             /* Display: hcgReadNoiseCoef, Accuracy: S0.31, Range: [-2147483648, 2147483647], Hcg Calibration Params*/
    AX_S32                           nLcgShotNoiseCoef[AX_ISP_ACP_NOISE_PARAM_NUM][AX_ISP_ACP_SHOT_NOISE_NUM];             /* Display: lcgShotNoiseCoef, Accuracy: S0.31, Range: [-2147483648, 2147483647], Lcg Calibration Params*/
    AX_S32                           nLcgReadNoiseCoef[AX_ISP_ACP_NOISE_PARAM_NUM][AX_ISP_ACP_READ_NOISE_NUM];             /* Display: lcgReadNoiseCoef, Accuracy: S0.31, Range: [-2147483648, 2147483647], Lcg Calibration Params*/
    AX_U8                            nAestat0InputMux;                    /* NOT-USED. Display: aestat0InputMux, Accuracy: U2.0, Range: [0, 2], aestat0 access point, 0: RawScl0, 1: DPC0, 2: WBC, NO GUI*/
    AX_U8                            nAestat1InputMux;                    /* NOT-USED. Display: aestat1InputMux, Accuracy: U2.0, Range: [0, 2], aestat1 access point, 0: RawScl1, 1: DPC1, 2: WBC, NO GUI*/
    AX_U8                            nAwbstatInputMux;                    /* NOT-USED. Display: awbstatInputMux, Accuracy: U3.0, Range: [0, 4], awbstat access point, 0: RawScl0, 1: RawScl1, 2: DPC0, 3: Raw2DNR, 4: LSC, NO GUI*/
    AX_U8                            nAfstatInputMux;                     /* NOT-USED. Display: afstatInputMux, Accuracy: U2.0, Range: [0, 2], afstat access point, 0: RawScl0, 1: DPC0, 2: WBC, NO GUI*/
    AX_ISP_AFS_DRC_MODE_E            eAfstatDrcMode;                      /* NOT-USED. Display: afstatDrcMode, Accuracy: U8.0, Range: [0, 7], afstat drc mode, set w.r.t input bitwidth and af access point*/
} AX_ISP_IQ_ACP_MANUAL_T;

typedef struct {
    AX_ISP_IQ_ACP_MANUAL_T tManualParam;
} AX_ISP_IQ_ACP_PARAM_T;

/************************************************************************************
 *  SCENE IQ Param
 ************************************************************************************/
typedef enum {
    AX_AINR_DISABLE   = 0,                                                         /* AI-ISP disable */
    AX_AINR_ENABLE    = 1,                                                         /* AI-ISP enable */
} AX_ISP_AI_WORK_MODE_E;

typedef struct {
    AX_U8                       nAiWorkMode;                                /* ainr work mode, Accuracy: U8.0 Range: [0, 1], 0: AX_AINR_DISABLE, 1: AX_AINR_ENABLE */
} AX_ISP_IQ_SCENE_MANUAL_T;

typedef struct {
    AX_ISP_IQ_SCENE_MANUAL_T    tManualParam;
} AX_ISP_IQ_SCENE_PARAM_T;

// clang-format on
/************************************************************************************
 *  3A Stat Placement Setting
 ************************************************************************************/
typedef struct {
  AX_U8 nUseRoi;  /* 0: use auto setting with full image, 1: use ROI setting,  Accuracy: U1.0 Range: [0, 1] */
  AX_U16 nStartX; /* ROI startx (hori) must be even, Accuracy: U16.0, Range: [0, image_width_max -1 ] */
  AX_U16 nStartY; /* ROI starty (vert) must be even, Accuracy: U16.0, Range: [0, image_height_max - 1] */
  AX_U16 nWidth;  /* ROI width, must be even, Accuracy: U16.0, nStartX + nWidth <= image_width_max */
  AX_U16 nHeight; /* ROI height, must be even, Accuracy: U16.0, nStartY + nHeight <= image_height_max */
} AX_ISP_IQ_3A_ROI_PLACEMT_T;

typedef struct {
  AX_ISP_IQ_3A_ROI_PLACEMT_T tRoi;
  AX_U8 nGridNumH; /* hori number of grid [0, 255]*/
  AX_U8 nGridNumV; /* vert number of grid [0, 255]*/
} AX_ISP_IQ_3A_GRID_ROI_PLACEMT_T;

/************************************************************************************
 *  AE Stat Config
 ************************************************************************************/
#define AX_AE_INST_NUM (2)
#define AX_AE_GIRD_NUM (2)
#define AX_AE_PREHDR_NUM (2)
#define AX_AE_GRID0_ROW (32)
#define AX_AE_GRID0_COL (32)
#define AX_AE_GRID1_ROW (48)
#define AX_AE_GRID1_COL (36)
#define AX_AE_GRID_CHN (4)
#define AX_AE_HIST_CHN (4)
#define AX_AE_HIST_LOG_BIN (64)
#define AX_AE_HIST_LINEAR_BIN (256)
#define AX_AE_HIST_WEIGHT_BLK_ROW (16)
#define AX_AE_HIST_WEIGHT_BLK_COL (16)
#define AX_AE_ITEM_NUM (2)

typedef enum {
  AX_ISP_AE0_STAT_ITP_DPC0 = 0,        /* after dpc0 output ae stat data */
  AX_ISP_AE0_STAT_ITP_WBC = 1,         /* after wbc output ae stat data */
  AX_ISP_AE0_STAT_IFE_RAW_SCALER0 = 2, /* after raw_scaler0 output ae stat data */
  AX_ISP_AE0_STAT_POS_MAX,
} AX_ISP_AE0_STAT_POS_E;

typedef enum {
  AX_ISP_AE1_STAT_ITP_DPC1 = 0,        /* after dpc1 output ae stat data */
  AX_ISP_AE1_STAT_ITP_WBC = 1,         /* after wbc output ae stat data */
  AX_ISP_AE1_STAT_IFE_RAW_SCALER1 = 2, /* after raw_scaler1 output ae stat data */
  AX_ISP_AE1_STAT_POS_MAX,
} AX_ISP_AE1_STAT_POS_E;

typedef struct {
  AX_ISP_AE0_STAT_POS_E eAe0StatPos; /* AE0 stat position, default value is DPC0 used for long frame */
  AX_ISP_AE1_STAT_POS_E eAe1StatPos; /* AE1 stat position default value is DPC1 used for shoft frame*/
} AX_ISP_IQ_AE_STAT_POS_T;

typedef struct {
  AX_U32 nRThr;  /* AE RThr (YThr for nGridMode=0). Accuracy: U14.6 Range: [0, 2^20-1] */
  AX_U32 nBThr;  /* AE BThr. Accuracy: U14.6 Range: [0, 2^20-1] */
  AX_U32 nGrThr; /* AE GrThr. Accuracy: U14.6 Range: [0, 2^20-1] */
  AX_U32 nGbThr; /* AE GbThr. Accuracy: U14.6 Range: [0, 2^20-1] */
} AX_ISP_IQ_AE_STAT_THR_T;

typedef struct {
  AX_U8 nEnable[AX_AE_INST_NUM];      /* 0: disable 1: enable */
  AX_U8 nIspGainEnable;               /* nIspGainEnable 0: not need * isp_gain. nIspGainEnable 1 : need * isp_gain., Accuracy: U1.0 Range: [0, 1] */
  AX_U8 nSkipNum;                     /* 3A STAT Skip Num. 0:no frame skip, 1:1/2 frame skip for 3a stat, 2:2/3 frame skip for 3a stat, Accuracy: U1.0 Range: [0, 2]  */
  AX_ISP_IQ_AE_STAT_POS_T tAeStatPos; /* ae stat position */
  AX_U8 nGridEnable[AX_AE_GIRD_NUM];  /* Grid Enable. 0: disable, 1: enable, Accuracy: U1.0 Range: [0, 1] */
  AX_U8 nGridMode[AX_AE_GIRD_NUM];    /* Grid Mode. 0: Y(1ch), 1: RGGB(4ch) Accuracy: U1.0 Range: [0, 1] */
  AX_U16 nGridYcoeff[AX_AE_GRID_CHN]; /* Grid Ycoeff(R, Gr, Gb, B).  Accuracy: U0.12 Range: [0, 4095], sum <= 4096 */
  AX_ISP_IQ_AE_STAT_THR_T tGridSatThr[AX_AE_GIRD_NUM];
  AX_ISP_IQ_3A_GRID_ROI_PLACEMT_T tGridRoi[AX_AE_GIRD_NUM];
  AX_U8 nHistEnable;                                                        /* Hist Enable. 0: disable, 1: log hist enable, 2 linear hist enable Accuracy: U2.0 Range: [0, 2] */
  AX_U8 nHistMode;                                                          /* Hist Mode. 0: Y(1ch), 1: YRGB(4ch), 2:RGGB(4ch) Accuracy: U2.0 Range: [0, 2] */
  AX_U8 nHistLinearBinNum;                                                  /* Hist Bin Num. 0: 256, 1: 512, 2:1024, Accuracy: U2.0 Range: [0, 2], 1,2 is only available when nHistMode=0 */
  AX_U16 nHistYcoeff[AX_AE_HIST_CHN];                                       /* Hist Ycoeff(R, Gr, Gb, B).  Accuracy: U0.12 Range: [0, 4095], sum <= 4096 */
  AX_U8 nHistWeight[AX_AE_HIST_WEIGHT_BLK_ROW * AX_AE_HIST_WEIGHT_BLK_COL]; /* Hist Weight, Accuracy: U8.0, Range: [0, 255], 16 x 16 block */
  AX_ISP_IQ_3A_ROI_PLACEMT_T tHistRoi;
  AX_U8 nItemEnable[AX_AE_ITEM_NUM]; /* Item Enable. 0: disable, 1: enable, Accuracy: U1.0 Range: [0, 1] */
  AX_ISP_IQ_3A_ROI_PLACEMT_T tItemRoi[AX_AE_ITEM_NUM];
} AX_ISP_IQ_AE_STAT_PARAM_T;

/************************************************************************************
 *  AE Stat Info
 ************************************************************************************/
typedef struct {
  AX_U32 nBin[AX_AE_HIST_CHN]; /* linear/log hist, Accuracy: U30.0, Range: [0, 2^30-1] */
} AX_AE_HIST_BIN_T;

typedef struct {
  AX_U8 nValid; /* current frame is valid for 3A algorithm, 1: AX_TRUE 0:AX_FALSE Accuracy: U1.0 Range: [0, 1] */
  AX_AE_HIST_BIN_T nLinearHist[AX_AE_HIST_LINEAR_BIN];
  AX_AE_HIST_BIN_T nLogHist[AX_AE_HIST_LOG_BIN];
} AX_AE_HIST_STAT_T;

typedef struct {
  AX_U32 nGridAvg[AX_AE_GRID_CHN]; /* pixel avg, Accuracy: U14.6, Range: [0, 2^20-1] */
  AX_U16 nGridNum[AX_AE_GRID_CHN]; /* pixel num, Accuracy: U14, Range: [0, 2^14-1] */
} AX_AE_GRID_STATS_T;

typedef struct {
  AX_U8 nValid;       /* current frame is valid for 3A algorithm, 1: AX_TRUE 0:AX_FALSE Accuracy: U1.0 Range: [0, 1] */
  AX_U8 nChnNum;      /* current channel number, 1: Y 4: RGGB or YRGB Accuracy: U3.0 Range: [0, 4] */
  AX_U8 nZoneRowSize; /* vertRegionNum, must be even, Accuracy: U10.0, Range: [2, 54] */
  AX_U8 nZoneColSize; /* horiRegionNum, must be even, Accuracy: U7.0, Range:[2, 72] */
  AX_U16 nOffsetH;    /* horiOffset, must be even, Accuracy: U16.0, Range: [0, image_width_max -1] */
  AX_U16 nOffsetV;    /* vertOffset, must be even, Accuracy: U16.0, Range: [0, image_high_max -1] */
  AX_U16 nGridWidth;  /* grid size width, must be even, Accuracy: U16.0, Range: [2, image_width_max] */
  AX_U16 nGridHeight; /* grid size height, must be even, Accuracy: U16.0, Range: [2, image_high_max] */
  AX_AE_GRID_STATS_T tGridStats[AX_AE_GRID0_ROW * AX_AE_GRID0_COL];
} AX_AE_GRID_STAT0_T;

typedef struct {
  AX_U8 nValid;        /* current frame is valid for 3A algorithm, 1: AX_TRUE 0:AX_FALSE Accuracy: U1.0 Range: [0, 1] */
  AX_U8 nChnNum;       /* current channel number, 1: Y 4: RGGB or YRGB Accuracy: U3.0 Range: [0, 4] */
  AX_U16 nZoneRowSize; /* vertRegionNum, must be even, Accuracy: U10.0, Range: [2, 512] */
  AX_U16 nZoneColSize; /* horiRegionNum, must be even, Accuracy: U4.0, Range: [2, 8] */
  AX_U16 nOffsetH;     /* horiOffset, must be even, Accuracy: U16.0, Range: [0, image_width_max -1] */
  AX_U16 nOffsetV;     /* vertOffset, must be even, Accuracy: U16.0, Range: [0, image_high_max -1] */
  AX_U16 nGridWidth;   /* grid size width, must be even, Accuracy: U16.0, Range: [2, image_width_max] */
  AX_U16 nGridHeight;  /* grid size height, must be even, Accuracy: U16.0, Range: [2, image_high_max] */
  AX_AE_GRID_STATS_T tGridStats[AX_AE_GRID1_ROW * AX_AE_GRID1_COL];
} AX_AE_GRID_STAT1_T;

typedef struct {
  AX_U8 nEnable;
  AX_AE_GRID_STAT0_T tAeGrid0Stat; /* for AE */
  AX_AE_GRID_STAT1_T tAeGrid1Stat; /* for Flicker , grid mode = 0 (1 ch, Y) */
  AX_AE_HIST_STAT_T tAeHistStat;   /* hist stat */
} AX_AE_GRID_HIST_STAT_T;

typedef struct {
  AX_U8 nValid;    /* current frame is valid for 3A algorithm, 1: AX_TRUE 0:AX_FALSE Accuracy: U1.0 Range: [0, 1] */
  AX_U16 nOffsetH; /* horiOffset, must be even, Accuracy: U16.0, Range: [0, image_width_max -1] , used for 3A
                      algorithm in small region*/
  AX_U16 nOffsetV; /* vertOffset, must be even, Accuracy: U16.0, Range: [0, image_high_max -1] , used for 3A algorithm
                      in small region*/
  AX_U16 nWidth;   /* grid size width, must be even, Accuracy: U16.0, Range: [2, image_width_max], used for 3A
                          algorithm in small region */
  AX_U16 nHeight;  /* grid size height, must be even, Accuracy: U16.0, Range: [2, image_high_max], used for 3A
                          algorithm in small region */
  AX_AE_GRID_STATS_T tAeItemStats[AX_AE_ITEM_NUM];
} AX_AE_ITEM_STATS_INFO_T;

typedef struct {
    AX_U8  nHcgLcg;                          /* current in HCG or LCG in sensor's config */
    AX_F32 fSnsFps;                          /* Sensor Fps in sensor's config*/
    AX_F32 fDcIrisPwmDuty;                   /* Dciris PWM Duty in hw's config*/
    AX_F32 fIspGain;                         /* ISP D Gain in isp's config */
    AX_F32 fAGain[AX_HDR_CHN_NUM];           /* total A Gain, sensor Again * hcgratio*/
    AX_F32 fDgain[AX_HDR_CHN_NUM];           /* Sensor D Gain in sensor's config */
    AX_F32 fIntegrationTime[AX_HDR_CHN_NUM]; /* Shutter (us) in sensor's config */
} AX_AE_DIRECT_EXP_SETTING_T;

typedef struct {
  AX_U64 nSeqNum;                                        /* frame seq num , Accuracy: U64.0, Range: [0, 2^64-1] */
  AX_U64 nTimestamp;                                     /* frame timestamp , Accuracy: U64.0, Range: [0, 2^64-1] */
  AX_U64 nUserData;                                      /* user data */
  AX_U32 nSkipNum;                                       /* Algorithm running interval */
  AX_AE_GRID_HIST_STAT_T tAeStatInfo0[AX_AE_PREHDR_NUM]; /* before hdr */
  AX_AE_GRID_HIST_STAT_T tAeStatInfo1;                   /* after hdr */
  AX_AE_ITEM_STATS_INFO_T tAeItemStatsInfo0[AX_AE_PREHDR_NUM];
  AX_AE_ITEM_STATS_INFO_T tAeItemStatsInfo1;
  AX_AE_DIRECT_EXP_SETTING_T tAeDirectExpSetting;
} AX_ISP_AE_STAT_INFO_T;

/************************************************************************************
 *  AWB Stat Config
 ************************************************************************************/
#define AX_AWB_GRID_ROW (32)
#define AX_AWB_GRID_COL (32)
#define AX_AWB_GRID_CHN (4)
#define AX_AWB_GRID_LUMA_CHN (4)
#define AX_AWB_GRID_LUMA_THR_NUM (3)
#define AX_AWB_ITEM_NUM (2)

typedef enum {
    AX_ISP_AWB_STAT_ITP_RAW2DNR          = 0,                    /* after itp raw2dnr */
    AX_ISP_AWB_STAT_ITP_LSC              = 1,                    /* after itp lsc */
    AX_ISP_AWB_STAT_ITP_DPC              = 2,                    /* after itp dpc */
    AX_ISP_AWB_STAT_IFE_RAW_SCALER0      = 3,                    /* after ife raw scaler 0 */
    AX_ISP_AWB_STAT_IFE_RAW_SCALER1      = 4,                    /* after ife raw scaler 1 */
    AX_ISP_AWB_STAT_POSITION_MAX,
} AX_ISP_IQ_AWB_STAT_POS_E;

typedef struct {
  AX_S32 nRThr;  /* AWB RThr.  for tValidThr(tLowThr/tHighThr): Accuracy: S14.6 Range: [-2^20, 2^20-1]   for tSatThr: Accuracy: U14.6 Range: [0, 2^20-1]  */
  AX_S32 nBThr;  /* AWB BThr.  for tValidThr(tLowThr/tHighThr): Accuracy: S14.6 Range: [-2^20, 2^20-1]   for tSatThr: Accuracy: U14.6 Range: [0, 2^20-1] */
  AX_S32 nGrThr; /* AWB GrThr. for tValidThr(tLowThr/tHighThr): Accuracy: S14.6 Range: [-2^20, 2^20-1]   for tSatThr: Accuracy: U14.6 Range: [0, 2^20-1] */
  AX_S32 nGbThr; /* AWB GbThr. for tValidThr(tLowThr/tHighThr): Accuracy: S14.6 Range: [-2^20, 2^20-1]   for tSatThr: Accuracy: U14.6 Range: [0, 2^20-1] */
  AX_S32 nYThr;  /* AWB YThr.  for tValidThr(tLowThr/tHighThr): Accuracy: S14.6 Range: [-2^20, 2^20-1]   for tSatThr: Accuracy: U14.6 Range: [0, 2^20-1] */
} AX_ISP_IQ_AWB_STAT_THR_T;

typedef struct {
  AX_ISP_IQ_AWB_STAT_THR_T tLowThr;
  AX_ISP_IQ_AWB_STAT_THR_T tHighThr;
  AX_S32 nLumaThr[AX_AWB_GRID_LUMA_THR_NUM]; /* thr for luma slice . Accuracy: S16.4 Range: [-2^19, 2^19-1] */
} AX_ISP_IQ_AWB_VALID_THR_T;

typedef struct {
  AX_U8 nEnable;                        /* AWB Enable. Accuracy: U1.0 Range: [0, 1] */
  AX_U8 nIspGainEnable;                 /* nIspGainEnable 0: not need * isp_gain. nIspGainEnable 1 : need * isp_gain., Accuracy: U1.0 Range: [0, 1] */
  AX_U8 nSkipNum;                       /* 3A STAT Skip Num. 0:no frame skip, 1:1/2 frame skip for 3a stat, 2:2/3 frame skip for 3a stat, Accuracy: U1.0 Range: [0, 2]  */
  AX_ISP_IQ_AWB_STAT_POS_E eAwbStatPos; /* AWB STAT position.Accuracy: U3.0 Range: [0, 4], 0:after RAW2DNR, 1:after LSC 2: after DPC0 3: after RAWSCL0 4: after RAWSCL1 */
  AX_U8 nGridEnable;                    /* Grid Enable. 0: disable, 1: enable, Accuracy: U1.0 Range: [0, 1] */
  AX_U8 nGridMode;                      /* AWB GridMode. Accuracy: U2.0 Range: [0, 2], 0:RGB, 1:RGGB, 2: RGGB*Luma4ch */
  AX_U16 nGridYcoeff[AX_AWB_GRID_CHN];  /* AWB Grid Ycoeff(R, Gr, Gb, B).  Accuracy: U0.12 Range: [0, 4095], sum <= 4096 */
  AX_ISP_IQ_3A_GRID_ROI_PLACEMT_T tGridRoi;
  AX_ISP_IQ_AWB_STAT_THR_T tGridSatThr;
  AX_ISP_IQ_AWB_VALID_THR_T tGridValidThr;
  AX_U8 nItemEnable[AX_AWB_ITEM_NUM]; /* Item Enable. 0: disable, 1: enable, Accuracy: U1.0 Range: [0, 1] */
  AX_ISP_IQ_3A_ROI_PLACEMT_T tItemRoi[AX_AWB_ITEM_NUM];
} AX_ISP_IQ_AWB_STAT_PARAM_T;

/************************************************************************************
 *  AWB Stat Info
 ************************************************************************************/
typedef struct {
  AX_U32 nValidGridAvg[AX_AWB_GRID_LUMA_CHN]
                      [AX_AWB_GRID_CHN];      /* valid pixel sum, Accuracy: U14.6, Range: [0, 2^20-1] */
  AX_U16 nValidGridNum[AX_AWB_GRID_LUMA_CHN]; /* valid pixel num, Accuracy: U14 , Range: [0, 2^14-1] */
  AX_U32 nSatGridAvg[AX_AWB_GRID_CHN];        /* saturated pixel sum, Accuracy: U14.6, Range: [0, 2^20-1] */
  AX_U16 nSatGridNum;                         /* saturated pixel count, Accuracy: U14, Range: [0, 2^14-1] */
} AX_AWB_GRID_STATS_T;

typedef struct {
  AX_U8 nValid;       /* current frame is valid for 3A algorithm, 1: AX_TRUE 0:AX_FALSE Accuracy: U1.0 Range: [0, 1] */
  AX_U8 nZoneRowSize; /* vertRegionNum, must be even, Accuracy: U10.0, Range: [2, 54] */
  AX_U8 nZoneColSize; /* horiRegionNum, must be even, Accuracy: U7.0, Range:[2, 72] */
  AX_U16 nOffsetH;    /* horiOffset, must be even, Accuracy: U16.0, Range: [0, image_width_max -1] , used for 3A
                         algorithm in small face exposure*/
  AX_U16 nOffsetV;    /* vertOffset, must be even, Accuracy: U16.0, Range: [0, image_high_max -1] , used for 3A algorithm
                         in small face exposure*/
  AX_U16 nGridWidth;  /* grid size width, must be even, Accuracy: U16.0, Range: [2, image_width_max], used for 3A
                         algorithm in small face exposure */
  AX_U16 nGridHeight; /* grid size height, must be even, Accuracy: U16.0, Range: [2, image_high_max], used for 3A
                         algorithm in small face exposure */
  AX_AWB_GRID_STATS_T tAwbGridStats[AX_AWB_GRID_ROW * AX_AWB_GRID_COL];
} AX_AWB_GRID_STATS_INFO_T;

typedef struct {
  AX_U8 nValid;    /* current frame is valid for 3A algorithm, 1: AX_TRUE 0:AX_FALSE Accuracy: U1.0 Range: [0, 1] */
  AX_U16 nOffsetH; /* horiOffset, must be even, Accuracy: U16.0, Range: [0, image_width_max -1] , used for 3A
                      algorithm in small region*/
  AX_U16 nOffsetV; /* vertOffset, must be even, Accuracy: U16.0, Range: [0, image_high_max -1] , used for 3A algorithm
                      in small region*/
  AX_U16 nWidth;   /* grid size width, must be even, Accuracy: U16.0, Range: [2, image_width_max], used for 3A
                          algorithm in small region */
  AX_U16 nHeight;  /* grid size height, must be even, Accuracy: U16.0, Range: [2, image_high_max], used for 3A
                          algorithm in small region */
  AX_AWB_GRID_STATS_T tAwbItemStats[AX_AWB_ITEM_NUM];
} AX_AWB_ITEM_STATS_INFO_T;

typedef struct {
  AX_U64 nSeqNum;    /* frame seq num , Accuracy: U64.0, Range: [0, 2^64-1]*/
  AX_U64 nTimestamp; /* frame timestamp , Accuracy: U64.0, Range: [0, 2^64-1] */
  AX_U64 nUserData;  /* user data */
  AX_U32 nSkipNum;   /* Algorithm running interval , Accuracy: U32.0, Range: [0, 2^32-1] */
  AX_AWB_GRID_STATS_INFO_T tAwbGridStatsInfo;
  AX_AWB_ITEM_STATS_INFO_T tAwbItemStatsInfo;
} AX_ISP_AWB_STAT_INFO_T;

/************************************************************************************
 *  AF Stat Config
 ************************************************************************************/
#define AX_AF_GRID_ROW (10)
#define AX_AF_GRID_COL (18)
#define AX_AF_ROI_NUM_MAX (AX_AF_GRID_ROW * AX_AF_GRID_COL)
#define AX_AF_HSTAT_NUM (2)
#define AX_AF_VSTAT_NUM (2)
#define AX_AF_STAT_NUM (AX_AF_HSTAT_NUM + AX_AF_VSTAT_NUM)
#define AX_AF_WEIGHT_LUT_NUM (8)
#define AX_AF_CORING_LUT_NUM (16)
#define AX_AF_DRC_LUT_NUM (15)
#define AX_AF_IIR_COEF_NUM (10)
#define AX_AF_FIR_COEF_NUM (13)

typedef enum {
  AX_ISP_AF_STAT_ITP_DPC0 = 0,                          /* after itp dpc */
  AX_ISP_AF_STAT_ITP_WBC = 1,                           /* after itp wbc */
  AX_ISP_AF_STAT_IFE_RAW_SCALER0 = 2,                   /* after after ife raw scaler 0 */
  AX_ISP_AF_STAT_POSITION_MAX
} AX_ISP_IQ_AF_STAT_POS_E;

typedef struct {
  AX_U8 nfv_mode[AX_AF_STAT_NUM]; /* Accuracy: U1, Range: [0, 1], 0:SUM, 1:PEAK */
} AX_ISP_IQ_AF_MODE_T;

typedef struct {
  AX_U16 nCoeffR;  /* Accuracy: U0.12, Range: [0, 4095] */
  AX_U16 nCoeffGb; /* Accuracy: U0.12, Range: [0, 4095] */
  AX_U16 nCoeffGr; /* Accuracy: U0.12, Range: [0, 4095] */
  AX_U16 nCoeffB;  /* Accuracy: U0.12, Range: [0, 4095], nCoeffR + nCoeffGr + nCoeffGb + nCoeffB <= 4096 */
} AX_ISP_IQ_AF_BAYER2Y_T;

typedef struct {
  AX_U8 nScaleRatio;                        /* Accuracy: U3.0, Range: [0, 3], Downsample Ratio.   */
  AX_U8 nScaleWeight[AX_AF_WEIGHT_LUT_NUM]; /* Accuracy: U1.7, Range: [0, 255] */
} AX_ISP_IQ_AF_DOWNSCALE_T;

typedef struct {
  AX_U32 nCoringThr[AX_AF_STAT_NUM];                      /* Accuracy: U8.10, Range:[0, 2^18-1], suggest 18 numbers: {2^0, 2^1, ..., 2^17} */
  AX_U16 nCoringGain[AX_AF_STAT_NUM];                     /* Accuracy: U5.7, Range:[0, 4095] */
  AX_U8  nCoringLut[AX_AF_STAT_NUM][AX_AF_CORING_LUT_NUM]; /* Accuracy: U5.0, Range[0, 31], nCoringLut[i] <= nCoringLut[i+1] */
  AX_U8  nCoringPixSumflg[AX_AF_STAT_NUM];                 /* Accuracy: U1.0, Range:[0, 1] */
  AX_U8  nCoringPixCntflg[AX_AF_STAT_NUM];                 /* Accuracy: U1.0, Range:[0, 1] */
} AX_ISP_IQ_AF_CORING_T;

typedef struct {
  AX_U16 nLdgThr[AX_AF_STAT_NUM];          /* Accuracy: U8.4, Range: [0, 4095] */
  AX_U8  nLdgSlope[AX_AF_STAT_NUM];         /* Accuracy: U0.8, Range: [0, 255] */
  AX_U8  nLdgLimit[AX_AF_STAT_NUM];         /* Accuracy: U1.7, Range: [0, 255] */
  AX_U8  nLdgRange[AX_AF_HSTAT_NUM];        /* Accuracy: U3.0, Range: [0, 2] */
} AX_ISP_IQ_AF_LDG_T;

typedef struct {
  AX_U8  nDrcEnable;                  /* Accuracy: U1.0 Range: [0, 1] 0:Disable Drc,  1:Enable Drc */
  AX_U16 nDrcLut[AX_AF_DRC_LUT_NUM]; /* Accuracy: U8.4, Range: [0, 4095] */
} AX_ISP_IQ_AF_DRC_T;

typedef struct {
  AX_U8 nAfEnable;                            /* AF Enable. Accuracy: U1.0 Range: [0, 1] */
  AX_ISP_IQ_AF_STAT_POS_E eAfStatPos;         /* af stat position. 0: after itp_dpc0, 1: after itp_wbc, 2: after rawscl0, Accuracy: U3.0 Range: [0, 2] */
  AX_ISP_IQ_3A_GRID_ROI_PLACEMT_T tAfGridRoi;
  AX_ISP_IQ_AF_MODE_T tAfMode;
  AX_ISP_IQ_AF_BAYER2Y_T tAfBayer2Y;
  AX_ISP_IQ_AF_DOWNSCALE_T tAfScaler;
  AX_ISP_IQ_AF_CORING_T tAfCoring;
  AX_ISP_IQ_AF_LDG_T tAfLdg;
  AX_U16 nAfHighLumaCntThr[AX_AF_STAT_NUM];     /* Accuracy: U8.4, Range: [0, 4095] */
  AX_ISP_IQ_AF_DRC_T tAfDrc;
  AX_U8 nAfFVEnable[AX_AF_STAT_NUM];                                /* 0: disable, 1: enable, Accuracy: U1.0 Range: [0, 1] for V1, V2, H1, H2*/
  AX_S16 nAfVertIirCoef[AX_AF_VSTAT_NUM][AX_AF_IIR_COEF_NUM];       /* Accuracy: S2.12, Range: [-16384, 16383] */
  AX_S16 nAfHoriIirCoef[AX_AF_HSTAT_NUM][AX_AF_IIR_COEF_NUM];       /* Accuracy: S2.12, Range: [-16384, 16383] */
  AX_U8 nAfHoriFirEnable[AX_AF_HSTAT_NUM];                          /* 0: disable, 1: enable, Accuracy: U1.0 Range: [0, 1] for H1, H2*/
  AX_S8 nAfHoriFirCoef[AX_AF_HSTAT_NUM][AX_AF_FIR_COEF_NUM];        /* Accuracy: S6, Range: [-64, 63] */
} AX_ISP_IQ_AF_STAT_PARAM_T;

/************************************************************************************
 *  AF Stat Info
 ************************************************************************************/
typedef struct {
  AX_U64 nFocusValue;             /* focus value sum (V1, V2, H1, H2) x180, Accuracy: U31.10, Range: [0, 2^41-1] */
  AX_U32 nPixCount;               /* pixel count (V1, V2, H1, H2)x180, Accuracy: U18, Range: [0, 2^18-1] */
  AX_U32 nPixSum;                 /* pixel sum (V1, V2, H1, H2) x180, Accuracy: U26.4, Range: [0, 2^30-1] */
  AX_U32 nHighLumaPixCnt;         /* high luma pixel num (V1, V2, H1, H2) x180, Accuracy: U18, Range: [0, 2^18-1] */
} AX_AF_GRID_STATS_T;

typedef struct {
  AX_U8 nValid;       /* current frame is valid for 3A algorithm, 1: AX_TRUE 0:AX_FALSE Accuracy: U1.0 Range: [0, 1] */
  AX_U8 nZoneRowSize; /* vertRegionNum, Accuracy: U6.0, Range: [1, 64], (nZoneRowSize * nZoneColSize ) % 4 == 0  */
  AX_U8 nZoneColSize; /* horiRegionNum, Accuracy: U6.0, Range: [1, 20], nZoneRowSize * nZoneColSize <= 180 */
  AX_U16 nOffsetH;    /* horiOffset, must be even, Accuracy: U16.0, Range: [0, image_width_max -1] */
  AX_U16 nOffsetV;    /* vertOffset, must be even, Accuracy: U16.0, Range: [0, image_high_max -1] */
  AX_U16 nGridWidth;  /* grid size width, must be even, Accuracy: U16.0, Range: [2, image_width_max] */
  AX_U16 nGridHeight; /* grid size height, must be even, Accuracy: U16.0, Range: [2, image_high_max] */
  AX_AF_GRID_STATS_T tAfRoiV1[AX_AF_ROI_NUM_MAX];
  AX_AF_GRID_STATS_T tAfRoiV2[AX_AF_ROI_NUM_MAX];
  AX_AF_GRID_STATS_T tAfRoiH1[AX_AF_ROI_NUM_MAX];
  AX_AF_GRID_STATS_T tAfRoiH2[AX_AF_ROI_NUM_MAX];
} AX_AF_STATS_INFO_T;

typedef struct {
  AX_U64 nSeqNum;    /* frame seq num , Accuracy: U64.0, Range: [0, 2^64-1]*/
  AX_U64 nTimestamp; /* frame timestamp , Accuracy: U64.0, Range: [0, 2^64-1] */
  AX_AF_STATS_INFO_T tAfStatInfo;
} AX_ISP_AF_STAT_INFO_T;

/************************************************************************************
 *  Stat Normalization Param
 ************************************************************************************/
typedef enum {
  AX_ISP_IQ_STAT_NORM_AE_GRID_AVG = 0,
  AX_ISP_IQ_STAT_NORM_DATA_TYPE_MAX
} AX_ISP_IQ_STAT_NORM_DATA_TYPE_E;

typedef struct {
  AX_U8 nEnable;        /* Normalization Enable. Accuracy: U1.0 Range: [0, 1] */
  AX_ISP_IQ_STAT_NORM_DATA_TYPE_E eDataType; /* Normalization data type. */
  AX_U32 nOriMinVal;       /* READ-ONLY. Min value for original data. */
  AX_U32 nOriMaxVal;       /* READ-ONLY. Max value for original data. */
  AX_U32 nDstMinVal;       /* Min value for normalized data. */
  AX_U32 nDstMaxVal;       /* Max value for Normalized data. */
} AX_ISP_IQ_STAT_NORM_PARAM_T;

// clang-format off

typedef enum {
    AX_ISP_IQ_MODE_AUTO_MANUAL          = 0,
    AX_ISP_IQ_MODE_ONLY_MANUAL          = 1,
} AX_ISP_IQ_MODE_E;

typedef struct {
    AX_U32              nIqMode;        /* Range: [0, 1], 0: auto+manual param, 1: only manual param */
} AX_ISP_IQ_COMM_T;

/*
 * nIqMode==0, The data structure is defined in ax_autokit_iq_api.h.
 * nIqMode==1, The data structure is defined in ax_isp_iq_api.h.
**/
typedef struct _AX_SENSOR_DEFAULT_PARAM_T_ {
    AX_ISP_IQ_COMM_T     tIqComm;
    AX_VOID             *ptBlc;
    AX_VOID             *ptLin;
    AX_VOID             *ptDpc;
    AX_VOID             *ptHdr;
    AX_VOID             *ptAinr;
    AX_VOID             *ptRaw2dnr;
    AX_VOID             *ptLsc;
    AX_VOID             *ptWbGain;
    AX_VOID             *ptRltm;
    AX_VOID             *ptDemosaic;
    AX_VOID             *ptFcc;
    AX_VOID             *ptGic;
    AX_VOID             *ptEdgEnhance;
    AX_VOID             *ptCc;
    AX_VOID             *ptGamma;
    AX_VOID             *ptDehaze;
    AX_VOID             *ptCsc;
    AX_VOID             *ptYuv3dnr;
    AX_VOID             *ptSharpen;
    AX_VOID             *ptDepurple;
    AX_VOID             *ptHs2dlut;
    AX_VOID             *ptCcmp;
    AX_VOID             *ptScm;
    AX_VOID             *ptYcproc;
    AX_VOID             *ptY2dnr;
    AX_VOID             *ptDis;
    AX_VOID             *pIspMeParam;
    AX_VOID             *ptLdc;
    AX_VOID             *ptAcp;
    AX_VOID             *ptScene;
} AX_SENSOR_DEFAULT_PARAM_T;

/************************************************************************************
 *  ISP IQ API
 ************************************************************************************/
AX_S32 AX_ISP_IQ_SetBlcParam(AX_U8 nPipeId, AX_ISP_IQ_BLC_PARAM_T *pIspBlcParam);
AX_S32 AX_ISP_IQ_GetBlcParam(AX_U8 nPipeId, AX_ISP_IQ_BLC_PARAM_T *pIspBlcParam);

AX_S32 AX_ISP_IQ_SetLinParam(AX_U8 nPipeId, AX_ISP_IQ_LIN_PARAM_T *pIspLinParam);
AX_S32 AX_ISP_IQ_GetLinParam(AX_U8 nPipeId, AX_ISP_IQ_LIN_PARAM_T *pIspLinParam);

AX_S32 AX_ISP_IQ_SetDpcParam(AX_U8 nPipeId, AX_ISP_IQ_DPC_PARAM_T *pIspDpcParam);
AX_S32 AX_ISP_IQ_GetDpcParam(AX_U8 nPipeId, AX_ISP_IQ_DPC_PARAM_T *pIspDpcParam);

AX_S32 AX_ISP_IQ_SetHdrParam(AX_U8 nPipeId, AX_ISP_IQ_HDR_PARAM_T *pIspHdrParam);
AX_S32 AX_ISP_IQ_GetHdrParam(AX_U8 nPipeId, AX_ISP_IQ_HDR_PARAM_T *pIspHdrParam);

AX_S32 AX_ISP_IQ_SetAinrParam(AX_U8 nPipeId, AX_ISP_IQ_AINR_PARAM_T *pIspAinrParam);
AX_S32 AX_ISP_IQ_GetAinrParam(AX_U8 nPipeId, AX_ISP_IQ_AINR_PARAM_T *pIspAinrParam);
AX_S32 AX_ISP_IQ_GetAinrCapability(AX_U8 nPipeId, AX_ISP_IQ_AINR_CAPABILITY_T *pIspAinrCapability);

AX_S32 AX_ISP_IQ_SetRaw2dnrParam(AX_U8 nPipeId, AX_ISP_IQ_RAW2DNR_PARAM_T *pIspRaw2dnrParam);
AX_S32 AX_ISP_IQ_GetRaw2dnrParam(AX_U8 nPipeId, AX_ISP_IQ_RAW2DNR_PARAM_T *pIspRaw2dnrParam);

AX_S32 AX_ISP_IQ_SetLscParam(AX_U8 nPipeId, AX_ISP_IQ_LSC_PARAM_T *pIspLscParam);
AX_S32 AX_ISP_IQ_GetLscParam(AX_U8 nPipeId, AX_ISP_IQ_LSC_PARAM_T *pIspLscParam);

AX_S32 AX_ISP_IQ_SetWbcParam(AX_U8 nPipeId, AX_ISP_IQ_WBC_PARAM_T *pIspWbcParam);
AX_S32 AX_ISP_IQ_GetWbcParam(AX_U8 nPipeId, AX_ISP_IQ_WBC_PARAM_T *pIspWbcParam);

AX_S32 AX_ISP_IQ_SetRltmParam(AX_U8 nPipeId, AX_ISP_IQ_RLTM_PARAM_T *pIspRltmParam);
AX_S32 AX_ISP_IQ_GetRltmParam(AX_U8 nPipeId, AX_ISP_IQ_RLTM_PARAM_T *pIspRltmParam);
AX_S32 AX_ISP_IQ_BuildScurve(AX_U8 nPipeId, AX_ISP_IQ_SCURVE_PARAM_T *pIspScurveParam,
                                    AX_U16 nSCurveList[AX_ISP_RLTM_SCURVE_MAX_LEN]);
AX_S32 AX_ISP_IQ_BuildBaseCurve(AX_U8 nPipeId, AX_ISP_IQ_BASECURVE_PARAM_T *pIspBaseCurveParam,
                                    AX_U16 nBaseCurveList[AX_ISP_RLTM_BASECURVE_MAX_LEN]);

AX_S32 AX_ISP_IQ_SetDemosaicParam(AX_U8 nPipeId, AX_ISP_IQ_DEMOSAIC_PARAM_T *pIspDemosaicParam);
AX_S32 AX_ISP_IQ_GetDemosaicParam(AX_U8 nPipeId, AX_ISP_IQ_DEMOSAIC_PARAM_T *pIspDemosaicParam);

AX_S32 AX_ISP_IQ_SetFccParam(AX_U8 nPipeId, AX_ISP_IQ_FCC_PARAM_T *pIspFccParam);
AX_S32 AX_ISP_IQ_GetFccParam(AX_U8 nPipeId, AX_ISP_IQ_FCC_PARAM_T *pIspFccParam);

AX_S32 AX_ISP_IQ_SetGicParam(AX_U8 nPipeId, AX_ISP_IQ_GIC_PARAM_T *pIspGicParam);
AX_S32 AX_ISP_IQ_GetGicParam(AX_U8 nPipeId, AX_ISP_IQ_GIC_PARAM_T *pIspGicParam);

AX_S32 AX_ISP_IQ_SetEdgeEnhanceParam(AX_U8 nPipeId, AX_ISP_IQ_EDGE_ENHANCE_PARAM_T *pIspEdgeEnhanceParam);
AX_S32 AX_ISP_IQ_GetEdgeEnhanceParam(AX_U8 nPipeId, AX_ISP_IQ_EDGE_ENHANCE_PARAM_T *pIspEdgeEnhanceParam);

AX_S32 AX_ISP_IQ_SetCcParam(AX_U8 nPipeId, AX_ISP_IQ_CC_PARAM_T *pIspCcParam);
AX_S32 AX_ISP_IQ_GetCcParam(AX_U8 nPipeId, AX_ISP_IQ_CC_PARAM_T *pIspCcParam);

AX_S32 AX_ISP_IQ_SetGammaParam(AX_U8 nPipeId, AX_ISP_IQ_GAMMA_PARAM_T *pIspGammaParam);
AX_S32 AX_ISP_IQ_GetGammaParam(AX_U8 nPipeId, AX_ISP_IQ_GAMMA_PARAM_T *pIspGammaParam);

AX_S32 AX_ISP_IQ_SetDehazeParam(AX_U8 nPipeId, AX_ISP_IQ_DEHAZE_PARAM_T *pIspDehazeParam);
AX_S32 AX_ISP_IQ_GetDehazeParam(AX_U8 nPipeId, AX_ISP_IQ_DEHAZE_PARAM_T *pIspDehazeParam);
AX_S32 AX_ISP_IQ_GetDehazeStatistics(AX_U8 nPipeId, AX_ISP_DEHAZE_STAT_INFO_T *pIspDehazeStat);

AX_S32 AX_ISP_IQ_SetCscParam(AX_U8 nPipeId, AX_ISP_IQ_CSC_PARAM_T *pIspCscParam);
AX_S32 AX_ISP_IQ_GetCscParam(AX_U8 nPipeId, AX_ISP_IQ_CSC_PARAM_T *pIspCscParam);

AX_S32 AX_ISP_IQ_SetYuv3dnrParam(AX_U8 nPipeId, AX_ISP_IQ_YUV3DNR_PARAM_T *pIspYuv3dnrParam);
AX_S32 AX_ISP_IQ_GetYuv3dnrParam(AX_U8 nPipeId, AX_ISP_IQ_YUV3DNR_PARAM_T *pIspYuv3dnrParam);

AX_S32 AX_ISP_IQ_SetSharpenParam(AX_U8 nPipeId, AX_ISP_IQ_SHARPEN_PARAM_T *pIspSharpenParam);
AX_S32 AX_ISP_IQ_GetSharpenParam(AX_U8 nPipeId, AX_ISP_IQ_SHARPEN_PARAM_T *pIspSharpenParam);

AX_S32 AX_ISP_IQ_SetDepurpleParam(AX_U8 nPipeId, AX_ISP_IQ_DEPURPLE_PARAM_T *pIspDepurpleParam);
AX_S32 AX_ISP_IQ_GetDepurpleParam(AX_U8 nPipeId, AX_ISP_IQ_DEPURPLE_PARAM_T *pIspDepurpleParam);

AX_S32 AX_ISP_IQ_SetHs2dlutParam(AX_U8 nPipeId, AX_ISP_IQ_HS2DLUT_PARAM_T *pIspHs2dlutParam);
AX_S32 AX_ISP_IQ_GetHs2dlutParam(AX_U8 nPipeId, AX_ISP_IQ_HS2DLUT_PARAM_T *pIspHs2dlutParam);

AX_S32 AX_ISP_IQ_SetCcmpParam(AX_U8 nPipeId, AX_ISP_IQ_CCMP_PARAM_T *pIspCcmpParam);
AX_S32 AX_ISP_IQ_GetCcmpParam(AX_U8 nPipeId, AX_ISP_IQ_CCMP_PARAM_T *pIspCcmpParam);

AX_S32 AX_ISP_IQ_SetScmParam(AX_U8 nPipeId, AX_ISP_IQ_SCM_PARAM_T *pIspScmParam);
AX_S32 AX_ISP_IQ_GetScmParam(AX_U8 nPipeId, AX_ISP_IQ_SCM_PARAM_T *pIspScmParam);

AX_S32 AX_ISP_IQ_SetYcprocParam(AX_U8 nPipeId, AX_ISP_IQ_YCPROC_PARAM_T *pIspYcprocParam);
AX_S32 AX_ISP_IQ_GetYcprocParam(AX_U8 nPipeId, AX_ISP_IQ_YCPROC_PARAM_T *pIspYcprocParam);

AX_S32 AX_ISP_IQ_SetY2dnrParam(AX_U8 nPipeId, AX_ISP_IQ_Y2DNR_PARAM_T *pIspY2dnrParam);
AX_S32 AX_ISP_IQ_GetY2dnrParam(AX_U8 nPipeId, AX_ISP_IQ_Y2DNR_PARAM_T *pIspY2dnrParam);

AX_S32 AX_ISP_IQ_SetDisParam(AX_U8 nPipeId, AX_ISP_IQ_DIS_PARAM_T *pIspDisParam);
AX_S32 AX_ISP_IQ_GetDisParam(AX_U8 nPipeId, AX_ISP_IQ_DIS_PARAM_T *pIspDisParam);

AX_S32 AX_ISP_IQ_SetMeParam(AX_U8 nPipeId, AX_ISP_IQ_ME_PARAM_T *pIspMeParam);
AX_S32 AX_ISP_IQ_GetMeParam(AX_U8 nPipeId, AX_ISP_IQ_ME_PARAM_T *pIspMeParam);
AX_S32 AX_ISP_IQ_GetMeStatInfo(AX_U8 nPipeId, AX_ISP_ME_STAT_INFO_T *pIspMeStatInfo);

AX_S32 AX_ISP_IQ_SetLdcParam(AX_U8 nPipeId, AX_ISP_IQ_LDC_PARAM_T *pIspLDCParam);
AX_S32 AX_ISP_IQ_GetLdcParam(AX_U8 nPipeId, AX_ISP_IQ_LDC_PARAM_T *pIspLDCParam);

AX_S32 AX_ISP_IQ_SetAcpParam(AX_U8 nPipeId, AX_ISP_IQ_ACP_PARAM_T *pIspAcpParam);
AX_S32 AX_ISP_IQ_GetAcpParam(AX_U8 nPipeId, AX_ISP_IQ_ACP_PARAM_T *pIspAcpParam);

AX_S32 AX_ISP_IQ_SetSceneParam(AX_U8 nPipeId, AX_ISP_IQ_SCENE_PARAM_T *pIspSceneParam);
AX_S32 AX_ISP_IQ_GetSceneParam(AX_U8 nPipeId, AX_ISP_IQ_SCENE_PARAM_T *pIspSceneParam);

AX_S32 AX_ISP_IQ_SetAeStatParam(AX_U8 nPipeId, const AX_ISP_IQ_AE_STAT_PARAM_T *pAeStatParam);
AX_S32 AX_ISP_IQ_GetAeStatParam(AX_U8 nPipeId, AX_ISP_IQ_AE_STAT_PARAM_T *pAeStatParam);

AX_S32 AX_ISP_IQ_SetAwbStatParam(AX_U8 nPipeId, const AX_ISP_IQ_AWB_STAT_PARAM_T *pAwbStatParam);
AX_S32 AX_ISP_IQ_GetAwbStatParam(AX_U8 nPipeId, AX_ISP_IQ_AWB_STAT_PARAM_T *pAwbStatParam);

AX_S32 AX_ISP_IQ_SetAfStatParam(AX_U8 nPipeId, const AX_ISP_IQ_AF_STAT_PARAM_T *pAfStatParam);
AX_S32 AX_ISP_IQ_GetAfStatParam(AX_U8 nPipeId, AX_ISP_IQ_AF_STAT_PARAM_T *pAfStatParam);

AX_S32 AX_ISP_IQ_SetStatNormParam(AX_U8 nPipeId, AX_ISP_IQ_STAT_NORM_PARAM_T *pStatNormParam);
AX_S32 AX_ISP_IQ_GetStatNormParam(AX_U8 nPipeId, AX_ISP_IQ_STAT_NORM_PARAM_T *pStatNormParam);

AX_S32 AX_ISP_IQ_GetAeStatistics(AX_U8 nPipeId, AX_ISP_AE_STAT_INFO_T *pAeStat);
AX_S32 AX_ISP_IQ_GetAwbStatistics(AX_U8 nPipeId, AX_ISP_AWB_STAT_INFO_T *pAwbStat);
AX_S32 AX_ISP_IQ_GetAfStatistics(AX_U8 nPipeId, AX_ISP_AF_STAT_INFO_T *pAfStat);


#ifdef __cplusplus
}
#endif
#endif  //_AX_ISP_IQ_API_H_
