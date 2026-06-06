/**************************************************************************************************
 *
 * Copyright (c) 2019-2025 Axera Semiconductor Co., Ltd. All Rights Reserved.
 *
 * This source file is the property of Axera Semiconductor Co., Ltd. and
 * may not be copied or distributed in any isomorphic form without the prior
 * written consent of Axera Semiconductor Co., Ltd.
 *
 **************************************************************************************************/
//clang-format off
#ifndef _AX_ISP_3A_STRUCT_H_
#define _AX_ISP_3A_STRUCT_H_

#include "ax_base_type.h"

#ifdef __cplusplus
extern "C"
{
#endif

#define AX_SUB_ID_AE         0x01
#define AX_SUB_ID_AWB        0x02
#define AX_SUB_ID_AF         0x03

////////////////////////////////////////////////////////////////////////////////////
//  AWB ALG Param
////////////////////////////////////////////////////////////////////////////////////
#define AX_ISP_AWB_GRID_NUM_MAX             (4096)
#define AX_ISP_AWB_ILLUM_NAME_LEN_MAX       (32)
#define AX_ISP_AWB_DESCRIPTION_LEN_MAX      (32)
#define AX_ISP_AWB_POLY_PNT_NUM_MAX         (15)
#define AX_ISP_AWB_ILLUM_NUM_MAX            (16)
#define AX_ISP_AWB_EXT_ILLUM_NUM_MAX        (32)
#define AX_ISP_AWB_ALL_ILLUM_NUM_MAX        (AX_ISP_AWB_ILLUM_NUM_MAX + AX_ISP_AWB_EXT_ILLUM_NUM_MAX)
#define AX_ISP_AWB_CTRL_PNT_PART_NUM_MAX    (32)
#define AX_ISP_AWB_CTRL_PNT_ALL_NUM_MAX     (AX_ISP_AWB_CTRL_PNT_PART_NUM_MAX * AX_ISP_AWB_ILLUM_NUM_MAX)
#define AX_ISP_AWB_DOMINANT_ZONE_NUM        (4)
#define AX_ISP_AWB_PLANCKIAN_ZONE_NUM       (24)
#define AX_ISP_AWB_SPATIAL_SEG_MAX_NUM      (8)
#define AX_ISP_AWB_PREFER_CCT_MAX_NUM       (32)
#define AX_ISP_AWB_LUX_TYPE_NUM             (8)
#define AX_ISP_AWB_ZONE_MAX_NUM             (40)
#define AX_ISP_AWB_LUMA_WEIGHT_MAX_NUM      (32)
#define AX_ISP_AWB_MIXLIGHT_CCT_MAX_NUM     (32)
#define AX_ISP_AWB_GRID_WEIGHT_ROW_MAX      (54)
#define AX_ISP_AWB_GRID_WEIGHT_COLUMN_MAX   (72)
#define AX_ISP_AWB_MANUAL_ILLUM_NUM_MAX     (15)
#define AX_ISP_AWB_MLC_ZONE_MAX_NUM         (15)
#define AX_ISP_AWB_DETECTION_ZONE_GROUP_MAX_NUM   (10)
#define AX_ISP_AWB_DETECTION_ZONE_MAX_NUM   (5)
#define AX_ISP_AWB_GC_LUX_MAX_NUM           (8)
#define AX_ISP_AWB_GC_CCT_MAX_NUM           (8)

typedef struct {
    AX_U32 nRg; /* Accuracy:U4.20 Range:[0, 16777215 (16*1024*1024)] */
    AX_U32 nBg; /* Accuracy:U4.20 Range:[0, 16777215 (16*1024*1024)] */
} AX_ISP_IQ_AWB_PNT_T;

typedef struct {
    AX_S32 nK; /* Accuracy:S21.10 Range:[-2147483647, 2147483647 (2*1024*1024*1024)] */
    AX_S32 nB; /* Accuracy:S21.10 Range:[-2147483647, 2147483647 (2*1024*1024*1024)] */
} AX_ISP_IQ_AWB_LINE_KB_T;

typedef struct {
    /* Illum Calib Info */
    AX_CHAR             szName[AX_ISP_AWB_ILLUM_NAME_LEN_MAX];
    AX_U16              nCct;    /* Accuracy:U14 Range:[0, 16000] */
    AX_U32              nRadius; /* Accuracy:U4.20 Range:[0, 16777215 (16*1024*1024)] */
    AX_ISP_IQ_AWB_PNT_T tCoord;
} AX_ISP_IQ_AWB_ILLUM_T;

typedef struct {
    AX_U8               nPntCnt; /*  Accuracy:U8 Range:[0, AX_ISP_AWB_POLY_PNT_NUM_MAX-1] */
    AX_ISP_IQ_AWB_PNT_T tPntArray[AX_ISP_AWB_POLY_PNT_NUM_MAX];
} AX_ISP_IQ_AWB_POLY_T;

typedef struct {
    AX_CHAR              szDescription[AX_ISP_AWB_DESCRIPTION_LEN_MAX]; /* Display: Zone Description */
    AX_U8                nZoneType; /* Display: Zone Type, Accuracy:U8 Range:[0, 32]  Rectangle=0, Triangle=1 */
    AX_ISP_IQ_AWB_POLY_T tPoly;     /* Display: Zone ROI Parameter */
    AX_U32               nLux[2];   /* Display: Lux, Accuracy:U22.10  Range:[0, 4294967295 (4*1024*1024*1024)] */
} AX_ISP_IQ_AWB_DETECTION_ZONE_T;

typedef struct {
    AX_U8                          nDetectionZoneNum; /* Display: Detection Zone Nums, Accuracy:U8 Range:[0, 5] */
    AX_ISP_IQ_AWB_DETECTION_ZONE_T tDetectionZoneList[AX_ISP_AWB_DETECTION_ZONE_MAX_NUM];
} AX_ISP_IQ_AWB_GROUP_DETECTION_ZONE_T;

typedef struct {
    AX_U8                                nEnable;   /* Display: Enable, Accuracy:U8 Range:[0, 1]*/
    AX_CHAR                              szDescription[AX_ISP_AWB_DESCRIPTION_LEN_MAX]; /* Display: Zone Description */
    AX_U8                                nZoneType; /* Display: Zone Type, Accuracy:U8 Range:[0, 32]  Rectangle=0, Triangle=1 */
    AX_ISP_IQ_AWB_POLY_T                 tPoly;     /* Display: Zone ROI Parameter */
    AX_U8                                nDetectionZoneGroupNum; /* Display: Detection Zone Group Nums, Accuracy:U8 Range:[0, 10]*/
    AX_ISP_IQ_AWB_GROUP_DETECTION_ZONE_T tGroupDetectionZoneList[AX_ISP_AWB_DETECTION_ZONE_GROUP_MAX_NUM];
    AX_U8                                nTrigerType1st;         /* Display: 1st Trigger Level, Accuracy:U8 Range:[0, 14] */
    AX_U8                                nTrigerType2nd;         /* Display: 2nd Trigger Level, Accuracy:U8 Range:[0, 14] */
    AX_U8                                nTrigerType3rd;         /* Display: 3rd Trigger Level, Accuracy:U8 Range:[0] */
    AX_U32 nTrigerValue1st[4]; /* Lux                      nTrigerType1st = 0  Accuracy:U22.10  Range:[0, 4294967295
                                  (4*1024*1024*1024)] CCT                      nTrigerType1st = 1  Accuracy:U32
                                  Range:[0, 20000] Valid Stats Cnt Ratio    nTrigerType1st = 2  Accuracy:U1.10 Range:[0,
                                  1024] Agw Rg                   nTrigerType1st = 3  Accuracy:U4.8 Range:[0, 4096] Agw
                                  Bg                   nTrigerType1st = 4  Accuracy:U4.8 Range:[0, 4096] Zone Group 0-9
                                  Cnt Ratio nTrigerType1st = [5, 14] Accuracy:U1.8 Range:[0, 255]
                               */
    AX_U32 nTrigerValue2nd[4]; /* Lux                      nTrigerType2nd = 0  Accuracy:U22.10  Range:[0, 4294967295
                                  (4*1024*1024*1024)] CCT                      nTrigerType2nd = 1  Accuracy:U32
                                  Range:[0, 20000] Valid Stats Cnt Ratio    nTrigerType2nd = 2  Accuracy:U1.10 Range:[0,
                                  1024] Agw Rg                   nTrigerType2nd = 3  Accuracy:U4.8 Range:[0, 4096] Agw
                                  Bg                   nTrigerType2nd = 4  Accuracy:U4.8 Range:[0, 4096] Zone Group 0-9
                                  Cnt Ratio nTrigerType2nd = [5, 14] Accuracy:U1.8 Range:[0, 255]
                               */
    AX_U8 nTrigerValue3rd;     /* Flash Sensitivity       nTrigerType3rd = 0  Accuracy:U8 Range:[0, 1] */
} AX_ISP_IQ_AWB_MLC_ZONE_T;

typedef struct {
    AX_U8  nDominantEnable;       /* Display: Enable, Accuracy:U1 Range:[0, 1] */
    AX_U16 nDomMinCctThresh;      /* Display: Min Cct Thresh, Accuracy:U14 Range:[0, 16000] */
    AX_U16 nDomMaxCctThresh;      /* Display: Max Cct Thresh, Accuracy:U14 Range:[0, 16000] */
    AX_U16 nDom2AllRatioThresh;   /* Display: Dominant to All Ratio Thresh, Accuracy:U1.10 Range:[0, 1024] */
    AX_U32 nDom2MinorRatioThresh; /* Display: Dominant to Minor Ratio Thresh, Accuracy:U10.10 Range:[0, 1,048,575(1024*1024-1)] */
    AX_U16 nMinorWeight;          /* Display: Minor Weight, Accuracy:U1.10 Range:[0, 1024] */
    AX_U32 nSmoothPercent;        /* Display: Smooth Percentage, Accuracy:U7.10 Range:[0, 131071 (128*1024)] */
} AX_ISP_3A_AWB_DOMINANT_T;

typedef struct {
    AX_U16 nGainR;  /* Display: Red, Accuracy:U4.8 Range:[256, 4095] */
    AX_U16 nGainGr; /* Display: Green.R, Accuracy:U4.8 Range:[256, 4095] */
    AX_U16 nGainGb; /* Display: Green.B, Accuracy:U4.8 Range:[256, 4095] */
    AX_U16 nGainB;  /* Display: Blue, Accuracy:U4.8 Range:[256, 4095] */
} AX_ISP_IQ_AWB_GAIN_T;

typedef struct {
    AX_ISP_IQ_AWB_GAIN_T tGains;
    AX_U32               nDampRatio; /* Display: Damp Ratio, Accuracy:U1.20 Range:[0, 1048576 (1024*1024)] */
    // AX_U8                nFrameSkipping;   /* Accuracy:U8 Range:[0, 255] */
} AX_ISP_IQ_AWB_AUTO_START_T;

typedef struct {
    /* Gray Zone Common Info */
    AX_ISP_IQ_AWB_PNT_T tCenterPnt;
    AX_U32              nCenterPntRadius; /* Accuracy:U4.20 Range:[0, 16777215 (16*1024*1024)] */

    AX_U8  nLowCut;                       /* Accuracy:U8 Range:[0, 64] */
    AX_U8  nHighCut;                      /* Accuracy:U8 Range:[0, 64] */
    AX_U16 nCctMax;                       /* Accuracy:U14 Range:[0, 16000] */
    AX_U16 nCctMin;                       /* Accuracy:U14 Range:[0, 16000] */

    AX_U8 nPartCtrlPntNum;                /* Accuracy:U8 Range:[0, 15] */
    AX_U8 nCtrlPntNum;                    /* Accuracy:U8 Range:[0, 128] */
    AX_U8 nCtrlSegKbNum;                  /* Accuracy:U8 Range:[0, 127] Should Always be nCtrlPntNum-1 */

    AX_U16                  nCctList[AX_ISP_AWB_CTRL_PNT_ALL_NUM_MAX]; /* Accuracy:U14 Range:[0, 16000] */
    AX_ISP_IQ_AWB_LINE_KB_T tChordKB;
    AX_ISP_IQ_AWB_PNT_T     tChordPntList[AX_ISP_AWB_CTRL_PNT_ALL_NUM_MAX];
    AX_ISP_IQ_AWB_PNT_T     tArcPointList[AX_ISP_AWB_CTRL_PNT_ALL_NUM_MAX];
    AX_ISP_IQ_AWB_LINE_KB_T tRadiusLineList[AX_ISP_AWB_CTRL_PNT_ALL_NUM_MAX];

    /* Gray Zone Borders */
    AX_ISP_IQ_AWB_PNT_T tInLeftBorderPntList[AX_ISP_AWB_CTRL_PNT_ALL_NUM_MAX];
    AX_ISP_IQ_AWB_PNT_T tInRightBorderPntList[AX_ISP_AWB_CTRL_PNT_ALL_NUM_MAX];

    AX_ISP_IQ_AWB_PNT_T tOutLeftBorderPntList[AX_ISP_AWB_CTRL_PNT_ALL_NUM_MAX];
    AX_ISP_IQ_AWB_PNT_T tOutRightBorderPntList[AX_ISP_AWB_CTRL_PNT_ALL_NUM_MAX];

    /* Illum Info */
    AX_U8                 nIllumNum;    /* Accuracy:U8 Range:[0, 64] */
    AX_ISP_IQ_AWB_ILLUM_T tIllumList[AX_ISP_AWB_ILLUM_NUM_MAX];
    AX_U8                 nExtIllumNum; /* Accuracy:U8 Range:[0, 16] */
    AX_ISP_IQ_AWB_ILLUM_T tExtIllumList[AX_ISP_AWB_EXT_ILLUM_NUM_MAX];
} AX_ISP_IQ_AWB_CALIB_T;

typedef struct {
    /* MLC Info */
    AX_U8                    nMLCNum; /* Display: MLC Zone Group Nums, Accuracy:U8 Range:[0, 15] */
    AX_ISP_IQ_AWB_MLC_ZONE_T tMLCZoneList[AX_ISP_AWB_MLC_ZONE_MAX_NUM];

    /*Auto Initialization Parameters */
    AX_ISP_IQ_AWB_AUTO_START_T tInitParam;

    /* Common Settings */
    AX_U8  nMode;        /* Accuracy:U6 Range:[0, 2] INVALID=0, MANUAL=1, AUTO=2 */
    AX_U8  nIndex;       /* Accuracy:U8 Range:[0, 64] */
    AX_U32 nDampRatio;   /* Display: Damp Ratio, Accuracy:U1.20 Range:[0, 1048576 (1024*1024)] */
    AX_U32 nToleranceRg; /* Display: Tolerance R/G, Accuracy:U4.20 Range:[0, 16777215 (16*1024*1024)] */
    AX_U32 nToleranceBg; /* Display: Tolerance R/G, Accuracy:U4.20 Range:[0, 16777215 (16*1024*1024)] */

    /* Lux Type Threshold */
    AX_U32 nLuxVeryDarkStart;   /* Display: VeryDark, Accuracy:U22.10 Range:[0, 4294967295 (4*1024*1024*1024)] */
    AX_U32 nLuxVeryDarkEnd;     /* Display: VeryDark, Accuracy:U22.10 Range:[0, 4294967295 (4*1024*1024*1024)] */
    AX_U32 nLuxDarkStart;       /* Display: Dark, Accuracy:U22.10 Range:[0, 4294967295 (4*1024*1024*1024)] */
    AX_U32 nLuxDarkEnd;         /* Display: Dark, Accuracy:U22.10 Range:[0, 4294967295 (4*1024*1024*1024)] */
    AX_U32 nLuxIndoorStart;     /* Display: Indoor, Accuracy:U22.10 Range:[0, 4294967295 (4*1024*1024*1024)] */
    AX_U32 nLuxIndoorEnd;       /* Display: Indoor, Accuracy:U22.10 Range:[0, 4294967295 (4*1024*1024*1024)] */
    AX_U32 nLuxTransInStart;    /* Display: Transition Indoor, Accuracy:U22.10 Range:[0, 4294967295 (4*1024*1024*1024)] */
    AX_U32 nLuxTransInEnd;      /* Display: Transition Indoor, Accuracy:U22.10 Range:[0, 4294967295 (4*1024*1024*1024)] */
    AX_U32 nLuxTransOutStart;   /* Display: Transition Outdoor, Accuracy:U22.10 Range:[0, 4294967295 (4*1024*1024*1024)] */
    AX_U32 nLuxTransOutEnd;     /* Display: Transition Outdoor, Accuracy:U22.10 Range:[0, 4294967295 (4*1024*1024*1024)] */
    AX_U32 nLuxOutdoorStart;    /* Display: Outdoor, Accuracy:U22.10 Range:[0, 4294967295 (4*1024*1024*1024)] */
    AX_U32 nLuxOutdoorEnd;      /* Display: Outdoor, Accuracy:U22.10 Range:[0, 4294967295 (4*1024*1024*1024)] */
    AX_U32 nLuxBrightStart;     /* Display: Bright, Accuracy:U22.10 Range:[0, 4294967295 (4*1024*1024*1024)] */
    AX_U32 nLuxBrightEnd;       /* Display: Bright, Accuracy:U22.10 Range:[0, 4294967295 (4*1024*1024*1024)] */
    AX_U32 nLuxVeryBrightStart; /* Display: VeryBright, Accuracy:U22.10 Range:[0, 4294967295 (4*1024*1024*1024)] */

    /* Gray Zone CCT Split Threshold */
    AX_U16 nCctMinInner;    /* Display: Inner Min CCt, Accuracy:U14 Range:[0, 16000] */
    AX_U16 nCctMaxInner;    /* Display: Inner Max CCt, Accuracy:U14 Range:[0, 16000] */
    AX_U16 nCctMinOuter;    /* Display: Outer Min CCt, Accuracy:U14 Range:[0, 16000] */
    AX_U16 nCctMaxOuter;    /* Display: Outer Max CCt, Accuracy:U14 Range:[0, 16000] */
    AX_U16 nCctSplitHtoA;   /* Display: H to A, Accuracy:U14 Range:[0, 16000] */
    AX_U16 nCctSplitAtoF;   /* Display: A to F, Accuracy:U14 Range:[0, 16000] */
    AX_U16 nCctSplitFtoD5;  /* Display: F to D5, Accuracy:U14 Range:[0, 16000] */
    AX_U16 nCctSplitD5toD6; /* Display: D5 to D6, Accuracy:U14 Range:[0, 16000] */
    AX_U16 nCctSplitD6toS;  /* Display: D6 to Shade, Accuracy:U14 Range:[0, 16000] */

    /* Grid Weight Params */
    AX_U8  nGridWeightEnable;                                   /* Display: Enable, Accuracy: U8 Range: [0x0, 0x1] */
    AX_U8  nGridWeightRow;                                      /* Display: Grid Row, Accuracy: U8 Range: [0x1, 0x1B] */
    AX_U8  nGridWeightColumn;                                   /* Display: Grid Column, Accuracy: U8 Range: [0x1, 0x24] */
    AX_U16 nGridWeightTable[AX_ISP_AWB_GRID_WEIGHT_ROW_MAX]
                           [AX_ISP_AWB_GRID_WEIGHT_COLUMN_MAX]; /* Accuracy: U1.10 Range: [0x0, 0x400] */

    /* Lux Weights of Gray Zones and Extra Illuminations */
    AX_U16 nGrayZoneLuxWeight[AX_ISP_AWB_PLANCKIAN_ZONE_NUM]
                             [AX_ISP_AWB_LUX_TYPE_NUM];  /* Accuracy:U10 Range:[0, 1000] */
    AX_U16 nExtIlllumLuxWeight[AX_ISP_AWB_EXT_ILLUM_NUM_MAX]
                              [AX_ISP_AWB_LUX_TYPE_NUM]; /* Accuracy:U10 Range:[0, 1000] */

    /* Luma Weight*/
    AX_U8  nLumaWeightNum;                                  /* Accuracy:U6 Range:[0, 32]*/
    AX_U32 nLumaSplitList[AX_ISP_AWB_LUMA_WEIGHT_MAX_NUM];  /* Accuracy:U8.10 Range:[0, 262143 (256 *1024)] */
    AX_U16 nLumaWeightList[AX_ISP_AWB_LUX_TYPE_NUM]
                          [AX_ISP_AWB_LUMA_WEIGHT_MAX_NUM]; /* Accuracy:U1.10 Range:[0, 1024] */

    /* Mix Light*/
    AX_U8  bMixLightEn;                                           /* Display: Mix Light Enable, Accuracy:U1 Range:[0, 1], 1: Enable Mix Light Weight & CCM Saturation Discount, 0: Disable */
    AX_U16 nMixLightProba_0_CctStd[AX_ISP_AWB_LUX_TYPE_NUM];      /* Display: Proba 0% CCT Std, Accuracy:U14 Range:[0, 9999], Proba=0 if CCT Std below this Thresh */
    AX_U16 nMixLightProba_100_CctStd[AX_ISP_AWB_LUX_TYPE_NUM];    /* Display: Proba 100% CCT Std, Accuracy:U14 Range:[0, 9999], Proba=100 if CCT Std above this Thresh */
    AX_U8  nMixLightProba_100_SatDiscnt[AX_ISP_AWB_LUX_TYPE_NUM]; /* Display: Proba 100% CCM Saturation Discount, Accuracy: U8 Range:[0, 100], CCM Saturation Discount When Proba=100 */

    AX_U8  nMixLightKneeNum;                                      /* Accuracy:U6 Range:[0, 32], Weight LUT: Size */
    AX_U16 nMixLightKneeCctList[AX_ISP_AWB_MIXLIGHT_CCT_MAX_NUM]; /* Display: CCT List, Accuracy:U14 Range:[0, 16000], Weight LUT: Key */
    AX_U16 nMixLightKneeWtList[AX_ISP_AWB_LUX_TYPE_NUM][AX_ISP_AWB_MIXLIGHT_CCT_MAX_NUM];
                                                                  /* Display: Weight, Accuracy:U1.10 Range:[0, 1024], Weight LUT: Value */

    /* Dominant Params */
    AX_ISP_3A_AWB_DOMINANT_T tDomParamList[AX_ISP_AWB_DOMINANT_ZONE_NUM];

    AX_U16 nTmpoStabTriggerAvgBlkWt; /* Display: Tmpo Stab Trigger Weight, Accuracy:U10 Range:[0, 1000] */

    /* Planckian Locus Project in High Lux Scene */
    AX_U8  nPlanckianLocusProjEn;      /* Display: Enable, Accuracy:U6 Range:[0, 1] Enable or Disable*/
    AX_U32 nPlanckianLocusNotProjLux;  /* Display: Not Project Lux, Accuracy:U22.10 Range:[0, 4294967295 (4*1024*1024*1024)] */
    AX_U32 nPlanckianLocusFullProjLux; /* Display: Full Project Lux, Accuracy:U22.10 Range:[0, 4294967295 (4*1024*1024*1024)] */

    /* Spatial Predictor Params */
    AX_U8  nSpatialEn;                                       /* Display: Enable, Accuracy:U8 Range:[0, 1] Enable or Disable*/
    AX_U8  nSpatialSegmetNum;                                /* Display: SegNums, Accuracy:U8  Range:[0, 8] */
    AX_U32 nSpatialStartLux[AX_ISP_AWB_SPATIAL_SEG_MAX_NUM]; /* Accuracy:U22.10 Range:[0, 4294967295 (4*1024*1024*1024)] */
    AX_U32 nSpatialEndLux[AX_ISP_AWB_SPATIAL_SEG_MAX_NUM];   /* Accuracy:U22.10 Range:[0, 4294967295 (4*1024*1024*1024)] */
    AX_U32 nSpatialRg[AX_ISP_AWB_SPATIAL_SEG_MAX_NUM];       /* Accuracy:U4.20 Range:[0, 16777215 (16*1024*1024)] */
    AX_U32 nSpatialBg[AX_ISP_AWB_SPATIAL_SEG_MAX_NUM];       /* Accuracy:U4.20 Range:[0, 16777215 (16*1024*1024)] */

    /* Fusion Params */
    AX_U16 nFusionGrayZoneConfid_0_AvgBlkWeight;   /* Display: Gray Zone Confidence 0% Average Weight, Accuracy:U10 Range:[0, 1000] */
    AX_U16 nFusionGrayZoneConfid_100_AvgBlkWeight; /* Display: Gray Zone Confidence 100% Average Weight, Accuracy:U10 Range:[0, 1000] */
    AX_U32 nFusionSpatialConfid_0_Lux;             /* Display: Spatial Confidence 0% Lux, Accuracy:U22.10 Range:[0, 4294967295 (4*1024*1024*1024)] */
    AX_U32 nFusionSpatialConfid_100_Lux;           /* Display: Spatial Confidence 100% Lux, Accuracy:U22.10 Range:[0, 4294967295 (4*1024*1024*1024)] */
    AX_U16 nFusionWeightGrayZone;                  /* Display: Gray Zone Weight, Accuracy:U1.10 Range:[0, 1024] */
    AX_U16 nFusionWeightSpatial;                   /* Display: Spatial Preditor Weight, Accuracy:U1.10 Range:[0, 1024] */

    /* Preference Params */
    AX_U8  nPreferEn;                                        /* Display: Enable, Accuracy:U8 Range:[0, 1] Enable or Disable*/
    AX_U8  nPreferCctNum;                                    /* Accuracy:U8 Range:[0, 64] */
    AX_U16 nPreferSrcCctList[AX_ISP_AWB_PREFER_CCT_MAX_NUM]; /* Display: Source Cct (LUT Input), Accuracy:U14 Range:[0, 16000] */
    AX_U16 nPreferDstCct[AX_ISP_AWB_LUX_TYPE_NUM][AX_ISP_AWB_PREFER_CCT_MAX_NUM];  /* Display: Destination Cct (LUT Output), Accuracy:U14 Range:[0, 16000] */
    AX_S32 nPreferGrShift[AX_ISP_AWB_LUX_TYPE_NUM][AX_ISP_AWB_PREFER_CCT_MAX_NUM]; /* Display: Green Shift (LUT Output), Accuracy:S4.20 Range:[-16777215,
                                                                                      16777215 (16*1024*1024)] */

    /* Green Cut Params */
    AX_U8  nGreenCutEn;                                     /* Display: Enable, Accuracy:U8 Range:[0, 1] Enable or Disable*/
    AX_U8  nGreenCutLuxListNum;                             /* Display: Lux List Nums, Accuracy:U8 Range:[0, 8] */
    AX_U8  nGreenCutCctListNum;                             /* Display: CCT List Nums, Accuracy:U8 Range:[0, 8] */
    // AX_U32 nGreenCutConfid_0_Lx;                            /* Accuracy:U22.10 Range:[0, 4294967295 (4*1024*1024*1024)] */
    // AX_U32 nGreenCutConfid_100_Lx;                          /* Accuracy:U22.10 Range:[0, 4294967295 (4*1024*1024*1024)] */
    AX_U32 nGreenCutLuxList[AX_ISP_AWB_GC_LUX_MAX_NUM];     /* Display: Lux, Accuracy:U22.10 Range:[0, 4294967295 (4*1024*1024*1024)] */
    AX_U16 nGreenCutWeight[AX_ISP_AWB_GC_LUX_MAX_NUM];      /* Display: Weight of Cut Area, Accuracy: U1.10 Range: [0x0, 0x400] */
    AX_U32 nGreenCutBreakAngle[AX_ISP_AWB_GC_LUX_MAX_NUM];  /* Display: Length of Break Angle, Accuracy:U4.20 Range:[0, 16777215 (16*1024*1024)] */
    AX_S32 nGreenCutOffsetRg[AX_ISP_AWB_GC_LUX_MAX_NUM];    /* Display: Offset Rg, Accuracy:S4.20 Range:[-16777215, 16777215 (16*1024*1024)] */
    AX_S32 nGreenCutOffsetBg[AX_ISP_AWB_GC_LUX_MAX_NUM];    /* Display: Offset Bg, Accuracy:S4.20 Range:[-16777215, 16777215 (16*1024*1024)] */
    AX_U32 nGreenCutCctList[AX_ISP_AWB_GC_CCT_MAX_NUM];     /* Accuracy:U22.10 Range:[0, 4294967295 (4*1024*1024*1024)] */
    AX_U16 nGreenCutCctDiscount[AX_ISP_AWB_GC_CCT_MAX_NUM]; /* Accuracy: U1.10 Range: [0x0, 0x400] */

    /* Multi Camera Sync Params */
    AX_U8 nMultiCamSyncMode; /* Display: Multi Camera Sync Mode, Accuracy:U2 Range:[0, 3] */
} AX_ISP_IQ_AWB_TUNING_T;

typedef struct {
    AX_U64              nSeqNum;    /* Accuracy:U64.0, Range: [0, 2^64-1] */
    AX_U8               nblkRowNum; /* Accuracy:U7 Range:[0, 54] */
    AX_U8               nblkColNum; /* Accuracy:U7 Range:[0, 72] */
    AX_ISP_IQ_AWB_PNT_T tStats[AX_ISP_AWB_GRID_NUM_MAX];

    AX_U16 nCct;                    /* Accuracy:U14 Range:[0, 16000] */
    AX_U32 nLux;                    /* Accuracy:U22.10 Range:[0, 4294967295 (4*1024*1024*1024)] */
    AX_U8  nLuxTypeInd;             /* Accuracy:U8 Range:[0, 64] */
    AX_U16 nCctStd;                 /* Accuracy:U14 Range:[0, 9999] */
    AX_U16 nMixLightProba;          /* Accuracy:U10 Range:[0, 1000] Current Mix Light Probability  */
    AX_U8  nSatDiscount;            /* Accuracy:U8 Range:[0, 100] Current Saturation Discount  */

    /* Stat Info of each Planckian Zone and Extra Illumination.
     *   Element[0~23]: Stat Info of each Planckian Zone.
     *   Element[23~ ]: Stat Info of each Extra Illumination.
     */
    AX_U16 nGrayZoneCnt[AX_ISP_AWB_ZONE_MAX_NUM];            /* Accuracy:U12 Range:[0, 4096] */
    AX_U16 nGrayZoneLuxWeight[AX_ISP_AWB_ZONE_MAX_NUM];      /* Accuracy:U10 Range:[0, 1000] */
    AX_U32 nGrayZoneLuxWeightSum[AX_ISP_AWB_ZONE_MAX_NUM];   /* Accuracy:U32 Range:[0, 4294967295] */
    AX_U32 nGrayZoneFinalWeightSum[AX_ISP_AWB_ZONE_MAX_NUM]; /* Accuracy:U32 Range:[0, 4294967295] */
    AX_U32 nGrayZoneLumaSum[AX_ISP_AWB_ZONE_MAX_NUM];        /* Accuracy:U32 Range:[0, 4294967295] */

    /* Fusion Status */
    AX_U16              nGrayZoneBlkWeightAvg; /* Accuracy:U10 Range:[0, 1000] */
    AX_U16              nGrayZoneConfid;       /* Accuracy:U1.10 Range:[0, 1024] */
    AX_U16              nSpatialConfid;        /* Accuracy:U1.10 Range:[0, 1024] */
    AX_U16              nGrayZoneFusionRatio;  /* Accuracy:U1.10 Range:[0, 1024] */
    AX_U16              nSpatialFusionRatio;   /* Accuracy:U1.10 Range:[0, 1024] */
    AX_ISP_IQ_AWB_PNT_T tGrayZonePoint;
    AX_ISP_IQ_AWB_PNT_T tSpatialPoint;
    AX_ISP_IQ_AWB_PNT_T tFusionPoint;

    /* Classic Gray World */
    AX_ISP_IQ_AWB_PNT_T tGrayWorldPoint;

    /* Green Cut Status*/
    AX_U32 nGcBreakAngle;        /* Accuracy:U4.20 Range:[0, 16777215 (16*1024*1024)] */
    AX_S32 nGcOffsetRg;          /* Accuracy:S4.20 Range:[-16777215, 16777215 (16*1024*1024)] */
    AX_S32 nGcOffsetBg;          /* Accuracy:S4.20 Range:[-16777215, 16777215 (16*1024*1024)] */
    AX_U16 nGreenCutCnt;         /* Accuracy:U13 Range:[0, 4096] */
    AX_U16 nGreenCutFinalWeight; /* Accuracy:U1.10 Range:[0, 1024] */

    /* MLC Status*/
    AX_U16 nMLC1stWeight; /* Accuracy:U1.10 Range:[0, 1024] */
    AX_U16 nMLC2ndWeight; /* Accuracy:U1.10 Range:[0, 1024] */
    // AX_U16 nMLCTotalWeight; /* Accuracy:U1.10 Range:[0, 1024] */

    /* Dominant Status */
    AX_U16 nDomCntH;         /* Accuracy:U13 Range:[0, 4096] */
    AX_U16 nDomCntA;         /* Accuracy:U13 Range:[0, 4096] */
    AX_U16 nDomCntF;         /* Accuracy:U13 Range:[0, 4096] */
    AX_U16 nDomCntD;         /* Accuracy:U13 Range:[0, 4096] */
    AX_U16 nDom2MinorRatioH; /* Accuracy:U1.10 Range:[0, 1024] */
    AX_U16 nDom2MinorRatioA; /* Accuracy:U1.10 Range:[0, 1024] */
    AX_U16 nDom2MinorRatioF; /* Accuracy:U1.10 Range:[0, 1024] */
    AX_U16 nDom2MinorRatioD; /* Accuracy:U1.10 Range:[0, 1024] */
    AX_U16 nDom2AllRatioH;   /* Accuracy:U1.10 Range:[0, 1024] */
    AX_U16 nDom2AllRatioA;   /* Accuracy:U1.10 Range:[0, 1024] */
    AX_U16 nDom2AllRatioF;   /* Accuracy:U1.10 Range:[0, 1024] */
    AX_U16 nDom2AllRatioD;   /* Accuracy:U1.10 Range:[0, 1024] */
    AX_U8  nDomActiveH;      /* Accuracy:U1 Range:[0, 1] */
    AX_U8  nDomActiveA;      /* Accuracy:U1 Range:[0, 1] */
    AX_U8  nDomActiveF;      /* Accuracy:U1 Range:[0, 1] */
    AX_U8  nDomActiveD;      /* Accuracy:U1 Range:[0, 1] */

    /* Luma Counter */
    AX_U16 nLumaWeight[AX_ISP_AWB_LUMA_WEIGHT_MAX_NUM + 1]; /* Accuracy:U1.10 Range:[0, 1024] Current Luma Weight*/
    AX_U16 nLumaCount[AX_ISP_AWB_LUMA_WEIGHT_MAX_NUM + 1];  /* Accuracy:U12 Range:[0, 4000] Current Luma Count*/
} AX_ISP_IQ_AWB_ALG_STATUS_T;

typedef struct {
    AX_U8 nLogLevel;  /* Display: Log Level, AXAWB_LOG_EMERG:0,AXAWB_LOG_ALERT:1,AXAE_LOG_CRIT:2,AXAWB_LOG_ERROR:3, AXAWB_LOG_WARN:4,
                         AXAWB_LOG_NOTICE:5, AXAWB_LOG_INFO:6,AXAWB_LOG_DBG:7*/
    AX_U8 nLogTarget; /* Display: Log Target, AXAWB_LOG_TARGET_STDERR:1, AXAWB_LOG_TARGET_SYSLOG:2 */
    AX_U8 nAlgoPrintInterval; /* Display: Algo Print Interval, Accuracy:U8 Range:[0, 100]   How many frames to print at intervals. If set to 0, do not
                                 print */
    AX_U8 nStatisticsPrintInterval; /* Display: Statistics Print Interval, Accuracy:U8 Range:[0, 100]   How many frames to print at intervals. If set to 0,
                                       do not print */
} AX_ISP_IQ_AWB_LOG_T;

typedef struct {
    AX_CHAR szName[AX_ISP_AWB_ILLUM_NAME_LEN_MAX]; /* Display: lightSourceName */
    AX_U32  nColorTemperature; /* Display: colorTemp, Accuracy:U32 Range:[1000, 15000] */
    AX_S32  nGreenShift;       /* Display: greenShift, Accuracy:S4.20 Range:[-16777215, 16777215 (16*1024*1024)] */
} AX_ISP_IQ_AWB_LIGHTSOURCE_T;

typedef struct {
    AX_U8                       nLightSourceIndex; /*Accuracy:U8 Range:[0, 14] */
    AX_ISP_IQ_AWB_LIGHTSOURCE_T tLightSource[AX_ISP_AWB_MANUAL_ILLUM_NUM_MAX];
} AX_ISP_IQ_AWB_MANUAL_LIGHTSOURCE_T;

typedef struct {
    AX_U8                nManualMode; /* Display: manualMode, Accuracy:U8  Range:[0, 1] 0: manual gain mode; 1: light source mode*/
    AX_ISP_IQ_AWB_GAIN_T tGain;
    AX_ISP_IQ_AWB_MANUAL_LIGHTSOURCE_T tManualLightSource;
} AX_ISP_IQ_AWB_MANUAL_T;

typedef struct {
    AX_U8                      nEnable;
    AX_ISP_IQ_AWB_MANUAL_T     tManualParam;
    AX_ISP_IQ_AWB_CALIB_T      tCalibParam;
    AX_ISP_IQ_AWB_TUNING_T     tTuningParam;
    AX_ISP_IQ_AWB_LOG_T        tLogParam;
} AX_ISP_IQ_AWB_PARAM_T;

typedef struct {
    AX_ISP_IQ_AWB_GAIN_T       tGainStatus;
    AX_ISP_IQ_AWB_ALG_STATUS_T tAlgoStatus;
} AX_ISP_IQ_AWB_STATUS_T;

typedef struct {
    AX_U32 nRGainRatio; /* Accuracy:U4.20 Range:[0, 16777215 (16*1024*1024)] */
    AX_U32 nBGainRatio; /* Accuracy:U4.20 Range:[0, 16777215 (16*1024*1024)] */
} AX_ISP_IQ_AWB_SYNC_RATIO_T;

////////////////////////////////////////////////////////////////////////////////////
//  AE ALG Param
////////////////////////////////////////////////////////////////////////////////////
#define AX_ISP_IQ_AE_LUX_MAX_PAIR_NUM (10)
#define AX_ISP_IQ_AE_AFD_TFLICKER_100HZ (1000000.0 / 100.0) /* predefined flicker period value for ECM module, uint:us */
#define AX_ISP_IQ_AE_AFD_TFLICKER_120HZ (1000000.0 / 120.0) /* predefined flicker period value for ECM module, uint:us */
#define AX_ISP_IQ_AE_GRID_WEIGHT_ROW_MAX (54)
#define AX_ISP_IQ_AE_GRID_WEIGHT_COLUMN_MAX (72)

// flicker
typedef enum AX_ISP_IQ_AE_INIT_ANTI_FLICKER_MODE_S {
    AX_ISP_IQ_AE_AFD_INIT_DISABLE = 0,
    AX_ISP_IQ_AE_AFD_INIT_100HZ   = 1,
    AX_ISP_IQ_AE_AFD_INIT_120HZ   = 2
} AX_ISP_IQ_AE_INIT_ANTI_FLICKER_MODE_T;

#define AX_ISP_IQ_AE_AFD_MAX_NUM 12

typedef struct {
    AX_U8  nSize;                                /* Accuracy: U8 Range: [0x0, 0xC] */
    AX_U32 sznRefList[AX_ISP_IQ_AE_AFD_MAX_NUM]; /* Display: RefVal, <lux value> Accuracy: U22.10; lux range: [0x0, 0x3D090000] */
    AX_U32 sznAntiFlickerToleranceList[AX_ISP_IQ_AE_AFD_MAX_NUM]; /* Display: Anti Flicker Tolerance, Uints: us; Accuracy:U32 Range:[0x0, 0x208d] */
} AX_ISP_IQ_AE_AFD_TOLERANCE_CURVE_T;

typedef struct {
    AX_U8  nSize;                                /* Accuracy: U8 Range: [0x0, 0xC] */
    AX_U32 sznRefList[AX_ISP_IQ_AE_AFD_MAX_NUM]; /* Display: RefVal, <lux value> Accuracy: U22.10; lux range: [0x0, 0x3D090000] */
    AX_U16 sznFlickerValidThList[AX_ISP_IQ_AE_AFD_MAX_NUM];  /* Display: Flicker Valid Th, Accuracy: U6.10, Range: [1, 65535] */
    AX_U16 sznFlickerTrigTimeList[AX_ISP_IQ_AE_AFD_MAX_NUM]; /* Display: Flicker Trig Time, Accuracy: U16, Range: [1, 65535] */
} AX_ISP_IQ_AE_AFD_CURVE_T;

typedef struct {
    AX_U8 nAntiFlickerMode; /* Display: Anti Flicker Mode, Accuracy: U2, 0: Disable; 1: Enable Manual Anti-flicker; 2: Enable Auto Flicker Det; 3: Enable Anto Anti-flicker */
    AX_U8                              nFlickerPeriod;             /* Display: FlickerPeriod, 0: 100HZ, 1: 120HZ */
    AX_U8                              nOverExpMode;               /* Display: Over Exp Mode, 0: ANTI PRIOR,1: LUMA PRIOR */
    AX_U8                              nUnderExpMode;              /* Display: Under Exp Mode, 0: ANTI PRIOR,1: LUMA PRIOR */
    AX_U8                              nFlickerValidNum;           /* Display: Flicker Vaild Num, Accuracy: U8, Range: [0, 255]*/
    AX_U8                              nSkipTh;                    /* Display: Skip Th, Accuracy: U8,  Range: [0, 255]*/
    AX_S16                             nUpSlopeTh;                 /* Display: Up Slope Th, Accuracy: S8.10 Range: [-262144, 262144] */
    AX_S16                             nDownSlopeTh;               /* Display: Down Slope Th, Accuracy: S8.10 Range: [-262144, 262144] */
    AX_ISP_IQ_AE_AFD_CURVE_T           tFlickerParamCurve;
    AX_ISP_IQ_AE_AFD_TOLERANCE_CURVE_T tAntiFlickerToleranceCurve; /* Uints: us. Accuracy:U32 Range:[0x0, 0x208d] */
} AX_ISP_IQ_AE_AFD_PARAMS_T;

typedef struct _AX_ISP_IQ_AE_AFD_STATUS_FUNC_T_ {
    AX_S32 (*pfnAeSetFlickerStatus)(AX_U8 pipe, AX_U8 initAfdStatus);
    AX_S32 (*pfnAeGetFlickerStatus)(AX_U8 pipe, AX_U8 *initAfdStatus);
} AX_ISP_IQ_AE_AFD_STATUS_FUNC_T;

#define AX_ISP_IQ_AE_OBJECT_TARGET_MAX_NUM 12

typedef struct {
    AX_U8  nSize;                                          /* Accuracy: U8 Range: [0x0, 0xC] */
    AX_U32 sznRefList[AX_ISP_IQ_AE_OBJECT_TARGET_MAX_NUM]; /* Display: Ref List, <lux value> if nVehicleTargetMode = 1, use lux range;
                                                            * Accuracy: U22.10
                                                            * lux range: [0x0, 0x3D090000]*/
    AX_U32 sznObjectTargetList[AX_ISP_IQ_AE_OBJECT_TARGET_MAX_NUM]; /* Display: Object Target, Accuracy: U8.10  Range: [0x0, 0x3FC00] */
} AX_ISP_IQ_AE_OBJECT_TARGET_CURVE_T;

#define AX_ISP_IQ_AE_HDR_RATIO_LIST_MAX_NUM (10)

typedef struct {
    AX_U8  nListSize;                                         /* Accuracy: U8 Range: [0x0, 0xA] */
    AX_U32 nRefList[AX_ISP_IQ_AE_HDR_RATIO_LIST_MAX_NUM];     /* Display: RefVal, Accuracy: U22.10  Range: [0x0, 0x3D090000] */
    AX_U32 nSatLumaList[AX_ISP_IQ_AE_HDR_RATIO_LIST_MAX_NUM]; /* Display: Short Frame Saturated Luma, Accuracy: U8.10  Range: [0x0, 0x3FC00] */
    AX_U32 nMinRatioList[AX_ISP_IQ_AE_HDR_RATIO_LIST_MAX_NUM];       /* Display: Min Ratio, Accuracy: U7.10  Range: [nHdrRatioMin, nHdrRatioMax] */
    AX_U32 nMaxRatioList[AX_ISP_IQ_AE_HDR_RATIO_LIST_MAX_NUM];       /* Display: Max Ratio, Accuracy: U7.10  Range: [nHdrRatioMin, nHdrRatioMax] */
} AX_ISP_IQ_AE_HDR_RATIO_PARAM_CURVE_T;

typedef struct {
    AX_U32                               nShortNonSatAreaPercent; /* Display: Short Frame Not-Satureted Area Percent, Accuracy: U7.20  Range: [0x0, 0x6400000] */
    AX_U32                               nTolerance;              /* Display: Tolerance, Accuracy: U7.20  Range: [0x0, 0x6400000] */
    AX_U8                                nConvergeCntFrameNum;    /* Display: Converge Count Frame Num, Accuracy: U8 Range: [0x0, 0xA] */
    AX_U16                               nDampRatio;              /* Display: DampRatio, Accuracy: U0.10 Range: [0x0, 0x400] */
    AX_ISP_IQ_AE_HDR_RATIO_PARAM_CURVE_T tHdrRatioParamCurve;
} AX_ISP_IQ_AE_HDR_RATIO_STRATEGY_PARAM_T;

typedef struct {
    AX_U8  nListSize;                                         /* Accuracy: U8 Range: [0x0, 0xA] */
    AX_U32 nRefList[AX_ISP_IQ_AE_HDR_RATIO_LIST_MAX_NUM];     /* Display: RefVal, Accuracy: U22.10  Range: [0x0, 0x3D090000] */
    AX_U32 nSatLumaList[AX_ISP_IQ_AE_HDR_RATIO_LIST_MAX_NUM]; /* Display: Short Frame Saturated Luma, Accuracy: U8.10  Range: [0x0, 0x3FC00] */
} AX_ISP_IQ_AE_HDR_RATIO_EXTEND_PARAM_CURVE_T;

typedef struct {
    AX_U32 nCommonHdrRatio;         /* Display: Common Hdr Ratio, Accuracy: U7.10  Range: [nHdrRatioMin, nHdrRatioMax] */
    AX_U32 nExtendHdrRatio;         /* Display: Extend Hdr Ratio, Accuracy: U7.10  Range: [nHdrRatioMin, nHdrRatioMax] */
    AX_U32 nHdrRatioTh;             /* Display: Hdr Ratio Th, Accuracy: U7.10  Range: [nHdrRatioMin, nHdrRatioMax] */
    AX_U32 nShortNonSatAreaPercent; /* Display: Short Frame Not-saturated Area Percent, Accuracy: U7.20  Range: [0x0, 0x6400000] */
    AX_U32 nTolerance;              /* Display: Tolerance, Accuracy: U7.20  Range: [0x0, 0x6400000] */
    AX_U8  nConvergeCntFrameNum;    /* Display: Converge Count Frame Num, Accuracy: U8 Range: [0x0, 0xA] */
    AX_U16 nDampRatio;              /* Display: DampRatio, Accuracy: U0.10 Range: [0x0, 0x400] */
    AX_ISP_IQ_AE_HDR_RATIO_EXTEND_PARAM_CURVE_T tHdrRatioExtendParamCurve;
} AX_ISP_IQ_AE_HDR_RATIO_EXTEND_PARAM_T;

#define AX_ISP_IQ_AE_HDR_GAIN_COMP_LIST_MAX_NUM (10)
typedef struct{
    AX_U8  nSize;                                                       /* Accuracy: U8 Range: [0x0, 0xA] */
    AX_U32 nRefList[AX_ISP_IQ_AE_HDR_GAIN_COMP_LIST_MAX_NUM];           /* Display: Ref Gain, Accuracy: U22.10,
                                                                         * range: [nTotalGainMin, nTotalGainMax] */
    AX_U16 nGainCompRatioList[AX_ISP_IQ_AE_HDR_GAIN_COMP_LIST_MAX_NUM]; /* Display: Hdr Gain Comp Ratio, Accuracy: U1.15,
                                                                         * Range: [0x0, 0x8000] */
} AX_ISP_IQ_AE_HDR_GAIN_COMP_LIST_T;

typedef struct {
    AX_U8                                   nHdrMode;       /* Display: Hdr Mode, 0: fixed mode; 1: Dynamic mode; 2: Dynamic Extend */
    AX_ISP_IQ_AE_HDR_GAIN_COMP_LIST_T       tHdrGainCompRatioList;
    AX_ISP_IQ_AE_HDR_RATIO_STRATEGY_PARAM_T tRatioStrategyParam;
    AX_ISP_IQ_AE_HDR_RATIO_EXTEND_PARAM_T   tHdrRatioExtendParam;
    AX_U32                                  nFixedHdrRatio; /* Display: Fixed Hdr Ratio, Accuracy: U7.10  Range: [nHdrRatioMin, nHdrRatioMax] */
} AX_ISP_IQ_AE_HDR_RATIO_T;

typedef struct {
    AX_U32 nBigStepFactor;             /* Display: Big Step Factor, Accuracy: U4.20  Range: [0x0, 0xA00000], 6 decimal places  */
    AX_U32 nSmallStepFactor;           /* Display: Small Step Factor, Accuracy: U4.20  Range: [0x0, 0xA00000], 6 decimal places  */
    AX_U32 nLumaDiffOverThresh;        /* Display: Luma Diff Over Thresh, Accuracy: U8.10  Range: [0x0, 0x3FC00]   */
    AX_U32 nLumaDiffUnderThresh;       /* Display: Luma Diff Under Thresh, Accuracy: U8.10  Range: [0x0, 0x3FC00]   */
    AX_U32 nLumaSpeedThresh;           /* Display: Luma Speed Thresh, Accuracy: U8.10  Range: [0x0, 0x3FC00]   */
    AX_U32 nSpeedDownFactor;           /* Display: Speed Down Factor, Accuracy: U4.20  Range: [0x0, 0xA00000], 6 decimal places  */
    AX_U32 nMinUserPwmDuty;            /* Display: Min User Pwm Duty, Accuracy: U7.10  Range: [0x0, 0x19000]   */
    AX_U32 nMaxUserPwmDuty;            /* Display: Max User Pwm Duty, Accuracy: U7.10  Range: [0x0, 0x19000]   */
    AX_U32 nOpenPwmDuty;               /* Display: Open Pwm Duty, Accuracy: U7.10  Range: [0x0, 0x19000]   */
    AX_U32 nConvergeLumaDiffTolerance; /* Display: Converge Luma Diff Tolerance, Accuracy: U7.20  Range: [0x0, 0x6400000] */
    AX_U32 nConvergeFrameCntThresh;    /* Display: Converge Frame Cnt Thresh, Accuracy: U32 Range: [0x0, 0x64] */
} AX_ISP_IQ_AE_DCIRIS_PARAMS_T;

#define AX_ISP_IQ_AE_SPARSE_SLOW_SHUTTER_MAX_NUM 5

typedef struct {
    AX_U32 nNodeNum;                                           /* Accuracy: U32  Range: [0x0, 0x5]*/
    AX_U32 nFpsList[AX_ISP_IQ_AE_SPARSE_SLOW_SHUTTER_MAX_NUM]; /* Accuracy: U8.10  Range: [nSnsSlowShutterModeFpsMin,
                                                                  nSnsSlowShutterModeFpsMax] */
} AX_ISP_IQ_AE_SPARSE_MODE_PARAM_T;

typedef struct {
    AX_U8 nFrameRateMode;         /* Display: Frame Rate Mode, 0: FIX FRAME RATE MODE; 1: SLOW SHUTTER MODE */
    AX_U8 nFpsIncreaseDelayFrame; /* Display: Fps Increase Delay Frame, Accuracy: U8 Range: [0x0, 0xA] */
} AX_ISP_IQ_AE_SLOW_SHUTTER_PARAM_T;

typedef struct {
    AX_U8                        nIrisType; /* Display: Aperture Type, Accuracy: U8 0: FIXED TYPE; 1: DC-IRIS; 2: P-IRIS  */
    AX_ISP_IQ_AE_DCIRIS_PARAMS_T tDcIrisParam;
} AX_ISP_IQ_AE_IRIS_PARAMS_T;

#define AX_ISP_IQ_AE_LUMA_WEIGHT_MAX_NUM (64)

typedef struct {
    AX_U8  nEnable;                                          /* Display: Luma Weight Enable, 0: disable luma weight 1: enable luma weight */
    AX_U32 nLumaWeightNum;                                   /* Accuracy: U32  Range: [0x0, 0x40]*/
    AX_U32 nLumaSplitList[AX_ISP_IQ_AE_LUMA_WEIGHT_MAX_NUM]; /* Display: Luma Split, Accuracy: U8.10  Range: [0x0, 0x3FC00] */
    AX_U16 nWeightList[AX_ISP_IQ_AE_LUMA_WEIGHT_MAX_NUM];    /* Display: Weight, Accuracy: U1.10 Range: [0x0, 0x400] */
} AX_ISP_IQ_AE_LUMA_WEIGHT_PARAM_T;

#define AX_ISP_IQ_AE_OBJECT_SCORE_LEVEL (8)
#define AX_ISP_IQ_AE_OBJECT_DISTANCE_LEVEL (8)
#define AX_ISP_IQ_AE_OBJECT_LUMA_DIFF_LEVEL (8)
#define AX_ISP_IQ_AE_DETECT_OBJECT_MAX_NUM (10)
#define AX_ISP_IQ_AE_OBJECT_TYPE_MAX (2)

typedef enum AX_ISP_IQ_AE_DETECT_OBJECT_CATEGORY_S {
    AX_ISP_IQ_AE_FACE_ROI    = 0,
    AX_ISP_IQ_AE_BODY_ROI    = 1,
    AX_ISP_IQ_AE_VEHICLE_ROI = 2,
    AX_ISP_IQ_AE_CYCLE_ROI   = 3,
    AX_ISP_IQ_AE_PLATE_ROI   = 4
} AX_ISP_IQ_AE_DETECT_OBJECT_CATEGORY_T;

typedef struct {
    AX_U8  nSize;                                                /* Accuracy: U8 Range: [0x0, 0x5] */
    AX_U32 nObjectLumaDiff[AX_ISP_IQ_AE_OBJECT_LUMA_DIFF_LEVEL]; /* Display: Object Luma Diff, the luma diff between Object weighted meanluma and
                                                                     background weighted meanluma. Accuracy: U9.10
                                                                     Range: [0x0, 0x4 0000]*/
    AX_U32 nObjectWeight[AX_ISP_IQ_AE_OBJECT_LUMA_DIFF_LEVEL];   /* Display: Object Weight, the weight of Object meanluma counted into frame
                                                                     meanluma; Accuracy: U1.10 Range: [0x0, 0x400] */
} AX_ISP_IQ_AE_OBJECT_WEIGHT_LIST_T;

typedef struct {
    AX_U8                              nEnable;                     /* Display: Enable, 0: disable Object ae 1: enable Object ae */
    AX_U32                             nObjectTarget;               /* Display: Object Target, Accuracy: U8.10  Range: [0x0, 0x3FC00] */
    AX_U32                             nObjectTargetMode;           /* Display: Object Target Mode, 0: fixed; 1: lux */
    AX_ISP_IQ_AE_OBJECT_TARGET_CURVE_T tObjectTargetCurve;
    AX_U16 nObjectScoreList[AX_ISP_IQ_AE_OBJECT_SCORE_LEVEL];       /* Object score level list:order from low to high.
                                                                        Accuracy: U1.10 Range: [0x0, 0x400] */
    AX_U16 nObjectScoreWeightList[AX_ISP_IQ_AE_OBJECT_SCORE_LEVEL]; /* Weight table based on score level: order from low
                                                                        to high. Accuracy: U1.10 Range: [0x0, 0x400] */
    AX_U16 nObjectDistanceList[AX_ISP_IQ_AE_OBJECT_DISTANCE_LEVEL]; /* Object distance between image center and
                                                                        Object center;order from low to high. Accuracy:
                                                                        U1.10 Range: [0x0, 0x400]*/
    AX_U16 nObjectDistanceWeightList[AX_ISP_IQ_AE_OBJECT_DISTANCE_LEVEL]; /* Distance weight table: for calculating
                                                                              Object score:order from high to low.
                                                                              Accuracy: U1.10 Range: [0x0, 0x400] */
    AX_U16 nObjectTargetWeight; /* Display: Object Target Weight, the weight of Object target counted into frame target; Accuracy: U1.10 Range: [0x0,
                                    0x400] */
    AX_U16 nObjectRoiFactor;   /* Display: Object Roi Factor, the factor of Object roi counted for Object luma; Accuracy: U1.10 Range: [0x0, 0x400] */
    AX_U32 nNoObjectFrameTh;   /* Display: No Object Frame Th,  the frame th of lost object. Accuracy: U32 Range: [0x0, 0x12C] */
    AX_U32 nToNormalFrameTh;   /* Display: To Normal Frame Th,  the frame th of blending vehicleAE to normal AE. Accuracy: U32 Range: [0x0, 0x12C] */
    AX_U32 nWithObjectFrameTh; /* Display: With Object Frame Th,  the frame th of with object. Accuracy: U32 Range: [0x0, 0x12C] */
    AX_U32 nToObjectAeFrameTh; /* Display: To Object AE Frame Th, the frame th of blending normal AE to vehicleAE. Accuracy: U32 Range: [0x0, 0x12C] */
    AX_ISP_IQ_AE_OBJECT_WEIGHT_LIST_T tObjectWeightList;
    AX_U16 nObjectWeightDampRatio; /* Display: Object Weight Damp Ratio, the damp ratio of Object weight; Accuracy: U1.10 Range: [0x0, 0x400] */
    AX_U16 nToleranceAdjustRatio; /* Display: Tolerance Adjust Ratio, the adjust ratio of ae algo tolerance in Object AE enable.  Accuracy: U4.10 Range:
                                     [0x400, 0x2800] */
    AX_U16 nNoObjectDampRatio;    /* Display: No Object Damp Ratio, the damp ratio of no object; Accuracy: U1.10 Range: [0x0, 0x400] */
} AX_ISP_IQ_AE_OBJECT_TYPE_PARAM_T;

typedef struct {
    AX_U8                            nObjectPriorityMode; /* Display: Priority Mode, 0: default; 1: vehicle priority; 2: face priority. */
    AX_ISP_IQ_AE_OBJECT_TYPE_PARAM_T tObjectTypeParam[AX_ISP_IQ_AE_OBJECT_TYPE_MAX];
} AX_ISP_IQ_AE_OBJECT_UI_PARAM_T;

typedef struct {
    AX_U16 nObjectStartX; /* Object ROI start point. Accuracy: U1.10 Range: [0x0, 0x400]*/
    AX_U16 nObjectStartY; /* Object ROI start point. Accuracy: U1.10 Range: [0x0, 0x400]*/
    AX_U16 nObjectWidth;  /* Object ROI width. Accuracy: U1.10 Range: [0x0, 0x400]*/
    AX_U16 nObjectHeight; /* Object ROI height. Accuracy: U1.10 Range: [0x0, 0x400]*/
} AX_ISP_IQ_AE_OBJECT_RECT_INPUT_PARAM_T;

typedef struct {
    AX_U32 nObjectNum; /* Numbers of detected Object. Accuracy: U32 Range: [0x0, 0xA] */
    AX_U32 nObjectID[AX_ISP_IQ_AE_DETECT_OBJECT_MAX_NUM];       /* ID of each Object. Accuracy: U32 */
    AX_U32 nObjectCategory[AX_ISP_IQ_AE_DETECT_OBJECT_MAX_NUM]; /* 0: face; 1: body; 2: vehicle; 3: cycle; 4: plate. */
    AX_U16 nObjectConfidence[AX_ISP_IQ_AE_DETECT_OBJECT_MAX_NUM]; /* Confidence of each object. Accuracy: U1.10 Range:
                                                                     [0x0, 0x400]*/
    AX_ISP_IQ_AE_OBJECT_RECT_INPUT_PARAM_T
    tObjectInfos[AX_ISP_IQ_AE_DETECT_OBJECT_MAX_NUM];             /* Input Information of each Object */
} AX_ISP_IQ_AE_DETECT_OBJECT_PARAM_T;

#define AX_ISP_IQ_AE_DYNAMIC_RANGE_DET_PARAM_NUM_MAX (12)

typedef struct {
    AX_U32 nListSize;                                              /* Accuracy: U32  Range: [0x0, 0xC]*/
    AX_U32 nRefList[AX_ISP_IQ_AE_DYNAMIC_RANGE_DET_PARAM_NUM_MAX]; /* Display: Ref Val, Accuracy: U22.10; lux range: [0x0, 0x3D090000]. */
    AX_U32 nDarkRegionStart[AX_ISP_IQ_AE_DYNAMIC_RANGE_DET_PARAM_NUM_MAX];  /* Display: Dark Region Start, Accuracy: U8.10  Range: [0x0, 0x3FC00] */
    AX_U32 nDarkRegionEnd[AX_ISP_IQ_AE_DYNAMIC_RANGE_DET_PARAM_NUM_MAX];    /* Display: Dark Region End, Accuracy: U8.10  Range: [0x0, 0x3FC00] */
    AX_U32 nLightRegionStart[AX_ISP_IQ_AE_DYNAMIC_RANGE_DET_PARAM_NUM_MAX]; /* Display: Light Region Start, Accuracy: U8.10  Range: [0x0, 0x3FC00] */
    AX_U32 nLightRegionEnd[AX_ISP_IQ_AE_DYNAMIC_RANGE_DET_PARAM_NUM_MAX];   /* Display: Light Region End, Accuracy: U8.10  Range: [0x0, 0x3FC00] */
    AX_U32 nThdDarkRegion[AX_ISP_IQ_AE_DYNAMIC_RANGE_DET_PARAM_NUM_MAX];    /* Display: Dark Region Th, Accuracy: U7.10  Range: [0x0, 0x19000]*/
    AX_U32 nThdUnSatLightRegion[AX_ISP_IQ_AE_DYNAMIC_RANGE_DET_PARAM_NUM_MAX]; /* Display: Light Region UnSat Th, Accuracy: U7.10  Range: [0x0, 0x19000]*/
    AX_U32 nThdSatLightRegion[AX_ISP_IQ_AE_DYNAMIC_RANGE_DET_PARAM_NUM_MAX]; /* Display: Light Region Sat Th, Accuracy: U7.10  Range: [0x0, 0x19000]*/
} AX_ISP_IQ_AE_DYNAMIC_RANGE_DET_INFO_T;

typedef struct {
    AX_U8                                 nEnable; /* Display: Enable, 0: disable dynamic range detect; 1: enable dynamic range detect */
    AX_ISP_IQ_AE_DYNAMIC_RANGE_DET_INFO_T tDynamicRangeDetInfo;
    AX_U32 nSwitchFrameTh;                         /* Display: Switch Frame Th, SDR/HDR mode switch frame th. Accuracy: U32 Range: [0x0, 0x12C] */
} AX_ISP_IQ_AE_DYNAMIC_RANGE_DET_PARAM_T;

#define AX_ISP_IQ_AE_APEX_BV_MAXGROUP (32)
#define AX_ISP_IQ_AE_BV_COMP_MAX_NUM (15)

typedef struct {
    AX_U32 nAperture; /* Accuracy: U22.10 Range: [0x400, 0xFFFFFFFF] */
    AX_U32 nShutter;  /* Display: Shutter, Uints: us. Accuracy:U32
                        * if nFrameRateMode = 0, Range: [tSnsShutterLimit.nMin, tSnsShutterLimit.nMax]
                        * if nFrameRateMode = 1, Range: [tSnsSlowShutterModeShutterLimit.nMin,
                        * tSnsSlowShutterModeShutterLimit.nMax] */
    AX_U32 nISO;      /* Display: ISO, Accuracy:U32 Range:[nTotalGainMin * 100 / 1024, nTotalGainMax * 100 / 1024] */
} AX_ISP_IQ_AE_APEX_LINE_TABLE_T;

typedef struct {
    AX_U8                          nReinit;               /* Display: Reinit, Accuracy: U8  Range: [0x0, 0x1] */
    AX_U8                          nDelayFrame;           /* Display: DelayFrame, Accuracy: U8  Range: [0x0, 0x8] */
    AX_U8                          nTimeDecayFrameNum;    /* Display: TimeDecayFrameNum, Accuracy: U8  Range: [0x0, 0x8] */
    AX_U16                         nTimeDecayCoeff;       /* Display: TimeDecayCoeff, Accuracy: U1.8  Range: [0x0, 0x100] */
    AX_U8                          nUseInitExpSetting;    /* Display: UseInitExpSetting, Accuracy: U1  Range: [0x0, 0x1] */
    AX_ISP_IQ_AE_APEX_LINE_TABLE_T tInitExpSetting;
    AX_U16                         nTolerance;            /* Display: Tolerance, Accuracy: U1.10  Range: [0x0, 0x400] */
    AX_U16                         nConvergenceRate;      /* Display: ConvergenceRate, Accuracy: U1.15  Range: [0x0, 0x8000] */
    AX_S16                         nProgramShift;         /* Display: ProgramShift, Accuracy: S8.6  Range: [-0x3FFF, 0x3FFF] */
    AX_U16                         nBoundVar;             /* Display: BoundVar, Accuracy: U8.8  Range: [0x0, 0xFFFF] */
} AX_ISP_IQ_AE_APEX_CONFIG_T;

typedef struct {
    AX_U16                         uCalibLineNum; /* Accuracy: U16  Range: [0x0, 0x20] */
    AX_ISP_IQ_AE_APEX_LINE_TABLE_T tCalibExpLines[AX_ISP_IQ_AE_APEX_BV_MAXGROUP];
    AX_ISP_IQ_AE_APEX_CONFIG_T     tConfigSetting;
} AX_ISP_IQ_AE_APEX_PARAM_T;

#define AE_ISP_IQ_AE_TARGET_MAX_NUM (10)
typedef struct{
    AX_U8      nSize;                                 /* Accuracy: U8 Range: [0x0, 0xA] */
    AX_U32     nRefList[AE_ISP_IQ_AE_TARGET_MAX_NUM]; /* Display: Ref, <gain value/lux value> if nTargetMode = 1, use gain range; if nTargetMode = 2,use lux range;
                                                       * Accuracy: U22.10
                                                       * gain range: nTotalGainMin, nTotalGainMax]
                                                       * lux range: [0x0, 0x3D090000]*/
    AX_U32     nTargetList[AE_ISP_IQ_AE_TARGET_MAX_NUM]; /* Display: Target, Accuracy: U8.10  Range: [0x0, 0x3FC00] */
} AX_ISP_IQ_AE_TARGET_CURVE_T;

typedef struct {
    AX_U8 nTargetMode;         /* Display: Target Mode, 0: fixed; 1: gain; 2: lux */
    AX_U32 nTarget;            /* Display: Target, Accuracy: U8.10  Range: [0x0, 0x3FC00] */
    AX_ISP_IQ_AE_TARGET_CURVE_T tAeTargetCurve;
} AX_ISP_IQ_AE_TARGET_PARAM_T;

#define AX_ISP_IQ_AE_KN_MAX_NUM (10)

typedef struct {
    AX_U8  nSize;
    AX_U64 nExpValList[AX_ISP_IQ_AE_KN_MAX_NUM];   /* Display: ExpVal, Accuracy: U36.10
                                   * Range: min = tIspDgainLimit.nMin * tSnsAgainLimit.nMin * tSnsDgainLimit.nMin *
                                   * tSnsShutterLimit.nMin               Range: max = tIspDgainLimit.nMax * tSnsAgainLimit.nMax *
                                   * tSnsDgainLimit.nMax * tSnsShutterLimit.nMax * nDcgRatio */
    AX_U32 nMeanlumaList[AX_ISP_IQ_AE_KN_MAX_NUM]; /* Display: Meanluma, Accuracy: U8.10  Range: [0x0, 0x3FC00]  */
    AX_U32 nLuxList[AX_ISP_IQ_AE_KN_MAX_NUM];      /* Display: Lux, Accuracy: U22.10 Range: [0x0, 0x3D090000] */
    AX_U32 nKn;                                    /* Display: Kn, Accuracy: U24    Range: [0x0, 0x989680] */
} AX_ISP_IQ_AE_KN_CALIB_INPUT_T;

typedef struct {
    AX_U32 nToConvergedFrameTh; /* Display: ToConvergedFrameTh, Accuracy: U32 Range: [0x0, 0xA]*/
    AX_U32 nAgainLcg2HcgTh;    /* Display: AgainLcg2HcgTh, Accuracy: U22.10 Range: [tSnsAgainLimit.nMin, tSnsAgainLimit.nMax] */
    AX_U32 nAgainHcg2LcgTh;    /* Display: AgainHcg2LcgTh, Accuracy: U22.10 Range: [tSnsAgainLimit.nMin, tSnsAgainLimit.nMax] */
    AX_U32 nAgainLcg2HcgRatio; /* Display: AgainLcg2HcgRatio, Accuracy: U10.10 Range: [0x400, 0x2800] */
    AX_U32 nAgainHcg2LcgRatio; /* Display: AgainHcg2LcgRatio, Accuracy: U10.10 Range: [0x400, 0x2800] */

    AX_U8 nCompensationMode; /* Display: Compensation Mode, Accuracy: U8 Range: [0x0, 0x2] 0:Again Compensatition, Gain Compemsation 
                                Prior ISPgain-Again-Dgain; 1:Dgain Compensatition, Gain Compemsation Prior Again-Dgain-ISPgain; 2:Isp
                                Dgain Compensatition, Gain Compemsation Prior Again-ISPgain-Dgain */

    AX_U32 nMaxIspGain;      /* Display: MaxIspGain, Accuracy:U22.10 Range:[tIspDgainLimit.nMin, tIspDgainLimit.nMax] */
    AX_U32 nMinIspGain;      /* Display: MinIspGain, Accuracy:U22.10 Range:[tIspDgainLimit.nMin, tIspDgainLimit.nMax] */
    AX_U32 nMaxUserDgain;    /* Display: MaxUserDgain, Accuracy:U22.10 Range:[tSnsDgainLimit.nMin, tSnsDgainLimit.nMax] */
    AX_U32 nMinUserDgain;    /* Display: MinUserDgain, Accuracy:U22.10 Range:[tSnsDgainLimit.nMin, tSnsDgainLimit.nMax] */
    AX_U32 nMaxUserTotalAgain;  /* Display: MaxUserTotalAgain, Accuracy:U22.10 Range:[nSnsTotalAGainMin, nSnsTotalAGainMax] */
    AX_U32 nMinUserTotalAgain;  /* Display: MinUserTotalAgain, Accuracy:U22.10 Range:[nSnsTotalAGainMin, nSnsTotalAGainMax] */
    AX_U32 nMaxUserSysGain;     /* Display: MaxUserSysGain, Accuracy:U22.10 Range:[nTotalGainMin, nTotalGainMax] */
    AX_U32 nMinUserSysGain;     /* Display: MinUserSysGain, Accuracy:U22.10 Range:[nTotalGainMin, nTotalGainMax] */

    AX_U32 nMaxShutter;         /* Display: MaxInteTime, Uints: us. Accuracy:U32
                                 * if nFrameRateMode = 0, Range: [tSnsShutterLimit.nMin, tSnsShutterLimit.nMax]
                                 * if nFrameRateMode = 1, Range: [tSnsSlowShutterModeShutterLimit.nMin,
                                 * tSnsSlowShutterModeShutterLimit.nMax] */
    AX_U32 nMinShutter;         /* Display: MinInteTime, Uints: us. Accuracy:U32 Range:[tSnsShutterLimit.nMin, tSnsShutterLimit.nMax] */

    AX_U8  nPositionWeightMode; /* Display: Mode, Accuracy: U8    Range: [0x0, 0x2] 0:Close  1:GridWeightMode  2:RoiWeightMode   */
    AX_U16 nRoiStartX;          /* Display: ROI(x,y,w,h), Accuracy: U1.10 Range: [0x0, 0x400] */
    AX_U16 nRoiStartY;          /* Display: ROI(x,y,w,h), Accuracy: U1.10 Range: [0x0, 0x400] */
    AX_U16 nRoiWidth;           /* Display: ROI(x,y,w,h), Accuracy: U1.10 Range: [0x0, 0x400] */
    AX_U16 nRoiHeight;          /* Display: ROI(x,y,w,h), Accuracy: U1.10 Range: [0x0, 0x400] */
    AX_U16 nWeightRoi;          /* Display: Weight1, Accuracy: U1.10 Range: [0x0, 0x400] */
    AX_U16 nWeightBackgnd;      /* Display: Weight2, Accuracy: U1.10 Range: [0x0, 0x400] */
    AX_U8  nGridWeightRow;      /* Display: Grid Row, Accuracy: U8    Range: [0x1, 0x36]  */
    AX_U8  nGridWeightColumn;   /* Display: Grid Column, Accuracy: U8    Range: [0x1, 0x48]  */
    AX_U16 nGridWeightTable[AX_ISP_IQ_AE_GRID_WEIGHT_ROW_MAX]
                           [AX_ISP_IQ_AE_GRID_WEIGHT_COLUMN_MAX]; /* Accuracy: U1.10 Range: [0x0, 0x400] */

    AX_U32                             nStrategyMode; /* Display: Strategy Mode, 0: SHUTTER_PRIOR; 1:GAIN_PRIOR; 2:AE ROUTE */
    AX_ISP_IQ_AE_KN_CALIB_INPUT_T      tKnParam;
    AX_ISP_IQ_AE_TARGET_PARAM_T        tTargetParam;
    AX_ISP_IQ_AE_AFD_PARAMS_T          tIqAFDParams;

    AX_ISP_IQ_AE_OBJECT_UI_PARAM_T tObjectUIParam;

    AX_ISP_IQ_AE_HDR_RATIO_T tAeHdrRatio;
    AX_U8  nMultiCamSyncMode;  /* Display: MultiCamSyncMode, 0: INDEPEND MODE; 1: MASTER SLAVE MODE; 2: SPLIT HDR MODE; 3: SPLICE MODE  */
    AX_U32 nMultiCamSyncRatio; /* Display: MultiCamSyncRatio, Accuracy: U7.20  Range: [0x0, 0x8000000] */
    AX_ISP_IQ_AE_SLOW_SHUTTER_PARAM_T      tSlowShutterParam;
    AX_ISP_IQ_AE_IRIS_PARAMS_T             tIrisParam;
    AX_ISP_IQ_AE_LUMA_WEIGHT_PARAM_T       tLumaWeightParam;
    AX_ISP_IQ_AE_DYNAMIC_RANGE_DET_PARAM_T tDynamicRangeDetParam;
    AX_ISP_IQ_AE_APEX_PARAM_T              tApexParam;
} AX_ISP_IQ_AE_ALG_CONFIG_T;

typedef struct {
    AX_U32 nIspGain;           /* Display: IspGain, Accuracy: U22.10 Range: [tIspDgainLimit.nMin, tIspDgainLimit.nMax] */
    AX_U32 nAGain;             /* Display: Again, Accuracy: U22.10 Range: [tSnsAgainLimit.nMin, tSnsAgainLimit.nMax] */
    AX_U32 nDgain;             /* Display: DGain, Accuracy: U22.10 Range: [tSnsDgainLimit.nMin, tSnsDgainLimit.nMax] */
    AX_U8  nHcgLcg;            /* Display: Mode, HCG:0, LCG:1 , Not Support: 2*/
    AX_U32 nSnsTotalAGain;     /* Display: Sensor Total AGain, Accuracy: U22.10 Range: [nSnsTotalAGainMin, nSnsTotalAGainMax] */
    AX_U32 nSysTotalGain;      /* Display: System Total Gain, Accuracy: U22.10 Range: [nTotalGainMin, nTotalGainMax] */
    AX_U32 nShutter;           /* Display: IntegrationTime(us), Uints: us. Accuracy: U32
                                * if nFrameRateMode = 0, Range: [tSnsShutterLimit.nMin, tSnsShutterLimit.nMax]
                                * if nFrameRateMode = 1, Range: [tSnsSlowShutterModeShutterLimit.nMin,
                                * tSnsSlowShutterModeShutterLimit.nMax] */

    AX_U32 nIrisPwmDuty;       /* Display: Iris Pwm Duty, Accuracy: U7.10  Range: [0x0, 0x19000] */
    AX_U32 nPos;               /* Display: Pos, Accuracy: U10    Range: [0x0, 0x3FF] */
    AX_U32 nHdrRealRatioLtoS;  /* Display: HdrRealRatioLtoS, Accuracy: U7.10  Range: [0x400, 0x1FC00] */
    AX_U32 nHdrRealRatioStoVS; /* Display: HdrRealRatioStoVS, Accuracy: U7.10  Range: [0x400, 0x1FC00] */

    /* Below Unused on Tool, first defined, not nsed */
    AX_U32 nShortAgain;   /* Accuracy: U22.10 Range: [tShortAgainLimit.nMin, tShortAgainLimit.nMax] */
    AX_U32 nShortDgain;   /* Accuracy: U22.10 Range: [tShortDgainLimit.nMin, tShortDgainLimit.nMax] */
    AX_U32 nShortShutter; /* Uints: us. Accuracy: U32 Range: [tShortShutterLimit.nMin, tShortShutterLimit.nMax] */
    AX_U32 nVsAgain;      /* Accuracy: U22.10 Range: [tVsAgainLimit.nMin, tVsAgainLimit.nMax] */
    AX_U32 nVsDgain;      /* Accuracy: U22.10 Range: [tVsDgainLimit.nMin, tVsDgainLimit.nMax] */
    AX_U32 nVsShutter;    /* Uints: us. Accuracy: U32 Range: [tVsShutterLimit.nMin, tVsShutterLimit.nMax] */
    AX_U32 nHdrRatio;     /* Accuracy: U7.10  Range: [0x400, 0x1FC00] */

    AX_U32 nHdrMaxShutterHwLimit; /* Accuracy: U32 Range [0x0,0xFFFF FFFF]*/
    AX_U32 nRealMaxShutter;       /* Accuracy: U32 Range [0x0,0xFFFF FFFF]*/
} AX_ISP_IQ_AE_EXP_SETTING_T;

typedef struct {
    AX_U64 nSeqNum;               /* Accuracy:U64.0, Range: [0, 2^64-1] */
    AX_U32 nMeanLuma;             /* Display: MeanLuma, Accuracy: U8.10  Range: [0x0, 0x3FC00]  */
    AX_U32 nWeightedMeanLuma;     /* Display: Weighted MeanLuma, Accuracy: U8.10  Range: [0x0, 0x3FC00]  */
    AX_S16 nBV;                   /* Display: BV, Accuracy: S8.7  Range: [-0x7FFF, 0x7FFF] */
    AX_U32 nLux;                  /* Display: Lux, Accuracy: U22.10 Range: [0x0, 0x3D090000] */
    AX_U64 nExpVal;               /* Display: ExpVal, Accuracy: U36.10
                                   * Range: min = tIspDgainLimit.nMin * tSnsAgainLimit.nMin * tSnsDgainLimit.nMin *
                                   * tSnsShutterLimit.nMin               Range: max = tIspDgainLimit.nMax * tSnsAgainLimit.nMax *
                                   * tSnsDgainLimit.nMax * tSnsShutterLimit.nMax * nDcgRatio */
    AX_U32 nFps;                  /* Display: Fps, Accuracy: U8.10 Range: [0x0,0x19000]*/
    AX_U32 nFinalTarget;          /* Display: Final Target, Accuracy: U8.10  Range: [0x0, 0x3FC00] */
    AX_U32 nExposureTarget;       /* Display: Exposure Target, Accuracy: U8.10  Range: [0x0, 0x3FC00] */
    AX_U32 nMinRatio;             /* Display: Suggest Min Ratio, Accuracy: U7.10  Range: [nHdrRatioMin, nHdrRatioMax] */
    AX_U32 nMaxRatio;             /* Display: Suggest Max Ratio, Accuracy: U7.10  Range: [nHdrRatioMin, nHdrRatioMax] */
    AX_U32 nShortSatLuma;         /* Display: Short Frame Saturated Luma, Accuracy: U8.10  Range: [0x0, 0x3FC00] */
    AX_U8  bIsConverge;           /* Display: Converge Status 0: No 1; Yes*/
    AX_U32 nAntiFlickerTolerance; /* Display: AntiFlicker Tolerance(us), Uints: us; Accuracy:U32 Range:[0x0, 0x208d] */
    AX_U16 nFlickerValidTh;       /* Display: Flicker Valid Th, Accuracy: U6.10, Range: [1, 65535] */
    AX_U16 nFlickerTrigTime;      /* Display: Flicker Trig Time, Accuracy: U16, Range: [1, 65535] */
    AX_U8  nOutPutHdrMode;        /* Display: OutPut Hdr Mode, Accuracy: U8, 1: LINEAR; 2:HDR. The result of dynamic range detection. */
} AX_ISP_IQ_AE_ALG_STATUS_T;

typedef struct {
    AX_U8                      nEnable;
    AX_ISP_IQ_AE_EXP_SETTING_T tExpManual;
    AX_ISP_IQ_AE_ALG_CONFIG_T  tAeAlgAuto;
    AX_U32 nLogLevel;  /* AXAE_LOG_EMERG:0,AXAE_LOG_ALERT:1,AXAE_LOG_CRIT:2,AXAE_LOG_ERROR:3, AXAE_LOG_WARN:4,
                          AXAE_LOG_NOTICE:5, AXAE_LOG_INFO:6,AXAE_LOG_DBG:7*/
    AX_U32 nLogTarget; /* AXAE_LOG_TARGET_STDERR:1, AXAE_LOG_TARGET_SYSLOG:2 */
} AX_ISP_IQ_AE_PARAM_T;

typedef struct {
    AX_ISP_IQ_AE_ALG_STATUS_T  tAlgStatus;
    AX_ISP_IQ_AE_EXP_SETTING_T tExpStatus;
} AX_ISP_IQ_AE_STATUS_T;

typedef struct {
    AX_U8 scanStatus; /*Accuracy:U6 Range:[0, 1] SCANRUNING = 0, SCANFINISH = 1*/
} AX_ISP_IQ_AE_SCAN_STATUS;

typedef struct {
    AX_U32 nMax;
    AX_U32 nMin;
    AX_U32 nStep;
} AX_ISP_IQ_AE_UNIT_T;

typedef struct {
    AX_U8  nGainMode;         /* 0: AGain_Only, 1: AGain_DGain_Separate, 2: AGain_DGain_Combined */
    AX_U8  nDcgEn;            /* 1: support LCG/HCG switch, 0: Not Support */
    AX_U32 nDcgRatio;         /* Accuracy: U10.10 Range: [0x400, 0x2800] */
    AX_U64 nExpValLimitMin;   /* Accuracy: U36.10 Range: [0x0, 400000000000], Max/Min gain Depend on Sensor Spec */
    AX_U64 nExpValLimitMax;   /* Accuracy: U36.10 Range: [0x0, 400000000000], Max/Min gain Depend on Sensor Spec */
    AX_U32 nTotalGainMin;     /* Accuracy: U22.10 Range: [0x400, 0xFFFFFFFF], Max/Min gain Depend on Sensor Spec */
    AX_U32 nTotalGainMax;     /* Accuracy: U22.10 Range: [0x400, 0xFFFFFFFF], Max/Min gain Depend on Sensor Spec */
    AX_U32 nHdrRatioMin;      /* Accuracy: U7.10  Range: [0x400, 0x1FC00] */
    AX_U32 nHdrRatioMax;      /* Accuracy: U7.10  Range: [0x400, 0x1FC00] */
    AX_U32 nSnsTotalAGainMin; /* Accuracy: U22.10 Range: [0x400, 0xFFFFFFFF], Max/Min gain Depend on Sensor Spec */
    AX_U32 nSnsTotalAGainMax; /* Accuracy: U22.10 Range: [0x400, 0xFFFFFFFF], Max/Min gain Depend on Sensor Spec */

    AX_ISP_IQ_AE_UNIT_T tIspDgainLimit; /* Accuracy: U22.10 Range: [0x400, 0xFFFFFFFF]  */
    AX_ISP_IQ_AE_UNIT_T tSnsShutterLimit; /* Uints: us. Accuracy: U32 Range: [0x0, 0xFFFFFFFF], Max/Min IntTime Depends on FPS */
    AX_ISP_IQ_AE_UNIT_T tSnsAgainLimit;   /* Accuracy: U22.10 Range: [0x400, 0xFFFFFFFF], Max/Min gain Depend on Sensor Spec */
    AX_ISP_IQ_AE_UNIT_T tSnsDgainLimit;   /* Accuracy: U22.10 Range: [0x400, 0xFFFFFFFF], Max/Min gain Depend on Sensor Spec */

    AX_ISP_IQ_AE_UNIT_T tSnsSlowShutterModeShutterLimit; /* Uints: us. Accuracy: U32 Range: [0x0, 0xFFFFFFFF], Max/Min
                                                            IntTime Depends on FPS */

    /*first defined, temp not used*/
    AX_ISP_IQ_AE_UNIT_T tShortAgainLimit;   /* Accuracy: U22.10 Range: [0x400, 0xFFFFFFFF], Max/Min gain Depend on Sensor Spec */
    AX_ISP_IQ_AE_UNIT_T tShortDgainLimit;   /* Accuracy: U22.10 Range: [0x400, 0xFFFFFFFF], Max/Min gain Depend on Sensor Spec */
    AX_ISP_IQ_AE_UNIT_T tShortShutterLimit; /* Uints: us. Accuracy: U32 Range: [0x0, 0xFFFFFFFF], Max/Min IntTime Depends on FPS */
    AX_ISP_IQ_AE_UNIT_T tVsAgainLimit;      /* Accuracy: U22.10 Range: [0x400, 0xFFFFFFFF], Max/Min gain Depend on Sensor Spec */
    AX_ISP_IQ_AE_UNIT_T tVsDgainLimit;      /* Accuracy: U22.10 Range: [0x400, 0xFFFFFFFF], Max/Min gain Depend on Sensor Spec */
    AX_ISP_IQ_AE_UNIT_T tVsShutterLimit;    /* Uints: us. Accuracy: U32 Range: [0x0, 0xFFFFFFFF], Max/Min IntTime Depends on FPS */

} AX_ISP_IQ_AE_EXP_HW_LIMIT_T;


typedef struct {
    AX_U32 nAeSyncRatio; /* Accuracy:U4.20 Range:[0, 16777215 (16*1024*1024)] */
} AX_ISP_IQ_AE_SYNC_RATIO_T;

typedef struct {
    AX_U8 nValidPipeCnt;

    AX_U8 nAeLogLevel;
    AX_U8 nAeLogTarget;
    AX_U8 nLogLevelExtern;
    AX_U8 nLogTargetExtern;
} AX_ISP_IQ_AE_AFD_LOG_T;


////////////////////////////////////////////////////////////////////////////////////
//  AF ALG Param
////////////////////////////////////////////////////////////////////////////////////

#define AX_ISP_AF_FV_SCAN_MAX_NUM           (2048)

typedef struct {
    AX_S32 nDefaultDistance;    /*Accuracy:S15 Range:[-32768,32767]*/
    AX_U8  nStepFactor;         /*Accuracy:U8 Range:[1,256]*/
    AX_S32 nProbStepCoff;       /*Accuracy:S15 Range:[-32768,32767]*/
    AX_U8  nMaxProbeStep;       /*Accuracy:U8 Range:[1,256]*/
    AX_U8  nMinProbeStep;       /*Accuracy:U8 Range:[1,256]*/
    AX_S64 nFvDiffOverThresh;   /*Accuracy:S32 Range:[-2147483648,2147483647]*/
    AX_S64 nFvDiffMiddleThresh; /*Accuracy:S32 Range:[-2147483648,2147483647]*/
    AX_S64 nFvDiffUnderThresh;  /*Accuracy:S32 Range:[-2147483648,2147483647]*/
    AX_U32 nHeightDistanceCoff; /* Accuracy:U10.5 Range: [0, 1048575] */
    AX_U32 nMiddleDistanceCoff; /* Accuracy:U10.5 Range: [0, 1048575] */
    AX_U32 nUnderDistanceCoff;  /* Accuracy:U10.5 Range: [0, 1048575] */
    AX_U32 nProportionalCoff;   /* Accuracy:U10.5 Range: [0, 1048575] */
    AX_U32 nIntegralCoff;       /* Accuracy:U10.5 Range: [0, 1048575] */
    AX_U32 nDifferentialCoff;   /* Accuracy:U10.5 Range: [0, 1048575] */
} AX_ISP_CAF_ZOOM_TRACKING_T;

typedef struct {
    AX_U32 nMuX;     /* Accuracy:U6.10 Range:[0, 20480] */
    AX_U32 nMuY;     /* Accuracy:U6.10 Range:[0, 20480] */
    AX_U32 nSigmaX;  /* Accuracy:U6.10 Range:[10, 10240] */
    AX_U32 nSigmaY;  /* Accuracy:U6.10 Range:[10, 10240] */
    AX_U32 nCoeffV1; /* Accuracy:U6.10 Range:[10, 1024] */
    AX_U32 nCoeffV2; /* Accuracy:U6.10 Range:[10, 1024] */
    AX_U32 nCoeffH1; /* Accuracy:U6.10 Range:[10, 1024] */
    AX_U32 nCoeffH2; /* Accuracy:U6.10 Range:[10, 1024] */
} AX_ISP_CAF_WEIGHT_T;

typedef struct {
    /* Settings of Scan */
    AX_U8  nScanType;       /* Accuracy:U1 Range: [0, 1] */
    AX_U32 nGlobalScanStep; /* Accuracy:U10 Range: [0, 1023] */

    /* Tuning Params ClimbHill Search */
    AX_U32 nSearchDownFrameTh;       /* Accuracy:U8 Range: [0, 255] */
    AX_U32 nSearchDownSerialSlopeTh; /* Accuracy:U10.5 Range: [0, 1048575] */
    AX_U32 nSearchDownSingleSlopeTh; /* Accuracy:U10.5 Range: [0, 1048575] */
    AX_U32 nSearchUpFrameTh;         /* Accuracy:U8 Range: [0, 255] */
    AX_U32 nSearchUpSerialSlopeTh;   /* Accuracy:U10.5 Range: [0, 1048575] */
    AX_U32 nSearchUpSingleSlopeTh;   /* Accuracy:U10.5 Range: [0, 1048575] */

    AX_U32 nSearchBigStepUpSlopeTh;  /* Accuracy:U10.5 Range: [0, 1048575] */
    AX_U32 nSearchBigStep;           /* Accuracy:U10 Range: [0, 1023] */
    AX_U32 nSearchSmallStep;         /* Accuracy:U10 Range: [0, 1023] */
    AX_U32 nSearchProbeStep;         /* Accuracy:U10 Range: [0, 1023] */
    AX_U32 nSearchProbeFrameTh;      /* Accuracy:U8 Range: [0, 255] */

    /* Continuous AF Enable */
    AX_U8  bContinuousAfEn;                           /* Accuracy:U1 Range: [0, 1] */
    AX_U32 nSceneChangeLumaTh;                        /* Accuracy:U12.0 Range: [0, 4095] */
    AX_U32 nSceneSettledLumaTh;                       /* Accuracy:U12.0 Range: [0, 4095] */
    AX_U32 nSceneSlowChangeLumaTh;                    /* Accuracy:U12.0 Range: [0, 4095] */
    AX_U32 nSceneFvChangeLumaTh;                      /* Accuracy:U12.0 Range: [0, 4095] */
    AX_U32 nSceneFvChangeRatioTh;                     /* Accuracy:U1.10 Range: [0, 1024] */

    AX_U32                     nSpotlightZoomRatioTh; /*Accuracy:U7.10 Range:[0, 4194303]*/
    AX_ISP_CAF_WEIGHT_T        tWeight;
    AX_ISP_CAF_ZOOM_TRACKING_T tZoomTracking;
} AX_ISP_IQ_CAF_PARAM_T;

typedef struct {
    AX_S32 zoomPos; /*Accuracy:S21.10 Range:[-169,1331]*/
} AX_ISP_CAF_ZOOM_POS;

typedef struct {
    AX_S32 focusPos; /*Accuracy:S21.10 Range:[-280,1500]*/
} AX_ISP_CAF_FOCUS_POS;

typedef struct {
    AX_U32 zoomRatio; /*Accuracy:U5.10 Range:[1*1024,30*1024]*/
} AX_ISP_CAF_ZOOM_RATIO;

typedef struct {
    AX_U8 scanStatus; /*Accuracy:U6 Range:[0, 2] SCANIDLE=0, SCANRUNING=1, SCANFINISH=2*/
} AX_ISP_CAF_SCAN_STATUS;

typedef struct {
    AX_S32 focusPos; /*Accuracy:S21.10 Range:[-280,1500]*/
    AX_U64 fv;       /*Accuracy: U36.10 Range:[0x0,0xFFFF FFFF FFFF FFFF]*/
} AX_FV_FOCUS_INFO;

typedef struct {
    AX_U32           dataNum; /* Accuracy:U14 Range: [0, 2048] */
    AX_FV_FOCUS_INFO fvfposInfo[AX_ISP_AF_FV_SCAN_MAX_NUM];
    AX_FV_FOCUS_INFO fvFocusPosPeak;
} AX_ISP_CAF_SCAN_FV_INFO;

typedef enum {
    AX_ISP_IQ_AF_CHANNEL_V1 = 0,
    AX_ISP_IQ_AF_CHANNEL_V2 = 1,
    AX_ISP_IQ_AF_CHANNEL_H1 = 2,
    AX_ISP_IQ_AF_CHANNEL_H2 = 3
} AX_ISP_IQ_AF_CHANNEL_T;

// function pointer for sw led control
typedef struct {
    AX_S32 (*pfnSoftLedControl)(AX_F32 fLux);
} AX_ISP_AE_REGISTER_FUNCS_T;

////////////////////////////////////////////////////////////////////////////////////
//  sensor 3a default Param
////////////////////////////////////////////////////////////////////////////////////

typedef struct _AX_SENSOR_3A_DEFAULT_PARAM_T_ {
    AX_ISP_IQ_AE_PARAM_T        *ptAeDftParam;
    AX_ISP_IQ_AWB_PARAM_T       *ptAwbDftParam;
} AX_SENSOR_3A_DEFAULT_PARAM_T;

#ifdef __cplusplus
}
#endif

#endif //_AX_ISP_3A_STRUCT_H_
