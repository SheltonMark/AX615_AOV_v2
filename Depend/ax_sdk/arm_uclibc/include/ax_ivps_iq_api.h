/**************************************************************************************************
 *
 * Copyright (c) 2019-2025 Axera Semiconductor Co., Ltd. All Rights Reserved.
 *
 * This source file is the property of Axera Semiconductor Co., Ltd. and
 * may not be copied or distributed in any isomorphic form without the prior
 * written consent of Axera Semiconductor Co., Ltd.
 *
 **************************************************************************************************/

#ifndef __AX_IVPS_IQ_API_H__
#define __AX_IVPS_IQ_API_H__

#include "ax_base_type.h"
#include "ax_global_type.h"
#include "ax_ivps_type.h"

#ifdef __cplusplus
extern "C"
{
#endif

#define AX_IVPS_SHP_GAIN_SIZE 33

typedef struct {
    AX_U16                           nUdGainLut[AX_IVPS_SHP_GAIN_SIZE];         /* Accuracy: U8.4, Range: [0, 4095], set undirectional gain w.r.t. texture strength*/
    AX_U8                            nUdGainLutAnchor;                          /* Accuracy: U3.0, Range: [0, 7], smaller value means more anchor points in flat/weak texture regions*/
    AX_U16                           nDirGainLut[AX_IVPS_SHP_GAIN_SIZE];        /* Accuracy: U8.4, Range: [0, 4095], set directional gain w.r.t. edge strength*/
    AX_U8                            nDirGainLutAnchor;                         /* Accuracy: U3.0, Range: [0, 7], smaller value means more anchor points in flat/weak texture regions*/
    AX_U16                           nUdFreq;                                   /* Accuracy: U5.5, Range: [0, 1023], larger value means higher undirectional frequecny will be enhanced*/
    AX_U16                           nDirFreq;                                  /* Accuracy: U5.5, Range: [0, 1023], larger value means higher directional frequecny will be enhanced*/
    AX_U8                            nEdgeStr;                                  /* Accuracy: U4.0, Range: [0, 15], larger value means to use more directional filter's result*/
    AX_U16                           nEdgeThr;                                  /* Accuracy: U8.4, Range: [0, 4095], larger value means less pixels will be considered as edge pixels*/
    AX_U8                            nEdgeSmoothThinRatio;                      /* Accuracy: U4.0, Range: [0, 15], larger value means to make the edge much thinner*/
    AX_U8                            nOvershoot;                                /* Accuracy: U0.7, Range: [0, 127], larger value means stronger overshoot*/
    AX_U8                            nUndershoot;                               /* Accuracy: U0.7, Range: [0, 127], larger value means stronger undershoot*/
    AX_U8                            nDetailRegionThr;                          /* Accuracy: U8.0, Range: [0, 255], larger value means more pixels will be considered as detail region*/
    AX_S8                            nDetailOvershootAdj;                       /* Accuracy: S0.7, Range: [-127, 127], larger value means stronger overshoot for detail region*/
    AX_S8                            nDetailUndershootAdj;                      /* Accuracy: S0.7, Range: [-127, 127], larger value means stronger undershoot for detail region*/
    AX_U8                            nShootReleaseRatio;                        /* Accuracy: U1.4, Range: [0, 16], larger value means shoot suppression tends to ignore more weak texture/detail region*/
    AX_U8                            nShootSupRange;                            /* Accuracy: U8.0, Range: [0, 255], larger value means shoot suppression tends to control more edges*/
    AX_U8                            nShootSupStr;                              /* Accuracy: U0.7, Range: [0, 127], larger value means stronger shoot suppression on edges*/
    AX_U16                           nShpLimit;                                 /* Accuracy: U5.5, Range: [0, 1023], smaller value means more restriction on the pixel value change, and may reduce the number of white/black points*/
    AX_U8                            nLumaGainLutNeg[AX_IVPS_SHP_GAIN_SIZE];    /* Accuracy: U1.5, Range: [0, 63], adjust sharpen gain for negative hf based on image brightness*/
    AX_U8                            nLumaGainLutPos[AX_IVPS_SHP_GAIN_SIZE];    /* Accuracy: U1.5, Range: [0, 63], adjust sharpen gain for positive hf based on image brightness*/
    AX_U8                            nNrStr;                                    /* Accuracy: U1.5, Range: [0, 32], larger value means stronger noise reduction*/
    AX_U8                            nNrUdTextureSens;                          /* Accuracy: U0.8, Range: [0, 255], larger value means image has more undirectional textures and apply less noise reduction*/
    AX_U8                            nNrUdTextureThr;                           /* Accuracy: U0.8, Range: [0, 255], larger value means less pixels will be considered as real texture and apply more noise reduction*/
    AX_U16                           nNrUdLimit;                                /* Accuracy: U8.2, Range: [0, 1023], larger value means less restriction on undirectional noise reduction*/
    AX_U8                            nUdCoring;                                 /* Accuracy: U3.3, Range: [0, 63], larger value means stronger coring for undirectional details*/
    AX_U8                            nNrDirEdgeSens;                            /* Accuracy: U0.8, Range: [0, 255], larger value means image has more directional edges and apply less noise reduction*/
    AX_U8                            nNrDirEdgeThr;                             /* Accuracy: U4.4, Range: [0, 255], larger value means less pixels will be considered as real edge and apply more noise reduction*/
    AX_U16                           nNrDirLimit;                               /* Accuracy: U8.2, Range: [0, 1023], larger value means less restriction on directional noise reduction*/
    AX_U8                            nDirCoring;                                /* Accuracy: U3.3, Range: [0, 63], larger value means stronger coring for directional details*/
} AX_IVPS_IQ_SHARPEN_MANUAL_T;

typedef struct {
    AX_U8 nSharpenEn;
    AX_IVPS_IQ_SHARPEN_MANUAL_T tManualParam;
} AX_IVPS_IQ_SHARPEN_PARAM_T;

/************************************************************************************
 *  SHARPEN IQ Auto Ext Param
 ************************************************************************************/
#define AX_IVPS_REF_AUTOTBL_SHARPEN_EXPRESS_NUM (12)

typedef struct {
    AX_U8                            nRefMode;                                                                          /* choose ref mode, Accuracy: U1.0 Range: [0, 1], 0:use lux as ref, 1:use gain as ref */
    AX_U8                            nParamGrpNum;                                                                      /* Accuracy: U8.0 Range: [0, 12] */
    AX_U32                           nRefVal[AX_IVPS_REF_AUTOTBL_SHARPEN_EXPRESS_NUM];                                  /* Gain: Accuracy: U22.10 Range: [0x400, 0xFFFFFFFF]; Lux: Accuracy: U22.10 Range: [0, 0xFFFFFFFF] */
    AX_U16                           nUdGainLut[AX_IVPS_REF_AUTOTBL_SHARPEN_EXPRESS_NUM][AX_IVPS_SHP_GAIN_SIZE];        /* Accuracy: U8.4, Range: [0, 4095], set undirectional gain w.r.t. texture strength*/
    AX_U8                            nUdGainLutAnchor[AX_IVPS_REF_AUTOTBL_SHARPEN_EXPRESS_NUM];                         /* Accuracy: U3.0, Range: [0, 7], smaller value means more anchor points in flat/weak texture regions*/
    AX_U16                           nDirGainLut[AX_IVPS_REF_AUTOTBL_SHARPEN_EXPRESS_NUM][AX_IVPS_SHP_GAIN_SIZE];       /* Accuracy: U8.4, Range: [0, 4095], set directional gain w.r.t. edge strength*/
    AX_U8                            nDirGainLutAnchor[AX_IVPS_REF_AUTOTBL_SHARPEN_EXPRESS_NUM];                        /* Accuracy: U3.0, Range: [0, 7], smaller value means more anchor points in flat/weak texture regions*/
    AX_U16                           nUdFreq[AX_IVPS_REF_AUTOTBL_SHARPEN_EXPRESS_NUM];                                  /* Accuracy: U5.5, Range: [0, 1023], larger value means higher undirectional frequecny will be enhanced*/
    AX_U16                           nDirFreq[AX_IVPS_REF_AUTOTBL_SHARPEN_EXPRESS_NUM];                                 /* Accuracy: U5.5, Range: [0, 1023], larger value means higher directional frequecny will be enhanced*/
    AX_U8                            nEdgeStr[AX_IVPS_REF_AUTOTBL_SHARPEN_EXPRESS_NUM];                                 /* Accuracy: U4.0, Range: [0, 15], larger value means to use more directional filter's result*/
    AX_U16                           nEdgeThr[AX_IVPS_REF_AUTOTBL_SHARPEN_EXPRESS_NUM];                                 /* Accuracy: U8.4, Range: [0, 4095], larger value means less pixels will be considered as edge pixels*/
    AX_U8                            nEdgeSmoothThinRatio[AX_IVPS_REF_AUTOTBL_SHARPEN_EXPRESS_NUM];                     /* Accuracy: U4.0, Range: [0, 15], larger value means to make the edge much thinner*/
    AX_U8                            nOvershoot[AX_IVPS_REF_AUTOTBL_SHARPEN_EXPRESS_NUM];                               /* Accuracy: U0.7, Range: [0, 127], larger value means stronger overshoot*/
    AX_U8                            nUndershoot[AX_IVPS_REF_AUTOTBL_SHARPEN_EXPRESS_NUM];                              /* Accuracy: U0.7, Range: [0, 127], larger value means stronger undershoot*/
    AX_U8                            nDetailRegionThr[AX_IVPS_REF_AUTOTBL_SHARPEN_EXPRESS_NUM];                         /* Accuracy: U8.0, Range: [0, 255], larger value means more pixels will be considered as detail region*/
    AX_S8                            nDetailOvershootAdj[AX_IVPS_REF_AUTOTBL_SHARPEN_EXPRESS_NUM];                      /* Accuracy: S0.7, Range: [-127, 127], larger value means stronger overshoot for detail region*/
    AX_S8                            nDetailUndershootAdj[AX_IVPS_REF_AUTOTBL_SHARPEN_EXPRESS_NUM];                     /* Accuracy: S0.7, Range: [-127, 127], larger value means stronger undershoot for detail region*/
    AX_U8                            nShootReleaseRatio[AX_IVPS_REF_AUTOTBL_SHARPEN_EXPRESS_NUM];                       /* Accuracy: U1.4, Range: [0, 16], larger value means shoot suppression tends to ignore more weak texture/detail region*/
    AX_U8                            nShootSupRange[AX_IVPS_REF_AUTOTBL_SHARPEN_EXPRESS_NUM];                           /* Accuracy: U8.0, Range: [0, 255], larger value means shoot suppression tends to control more edges*/
    AX_U8                            nShootSupStr[AX_IVPS_REF_AUTOTBL_SHARPEN_EXPRESS_NUM];                             /* Accuracy: U0.7, Range: [0, 127], larger value means stronger shoot suppression on edges*/
    AX_U16                           nShpLimit[AX_IVPS_REF_AUTOTBL_SHARPEN_EXPRESS_NUM];                                /* Accuracy: U5.5, Range: [0, 1023], smaller value means more restriction on the pixel value change, and may reduce the number of white/black points*/
    AX_U8                            nLumaGainLutNeg[AX_IVPS_REF_AUTOTBL_SHARPEN_EXPRESS_NUM][AX_IVPS_SHP_GAIN_SIZE];   /* Accuracy: U1.5, Range: [0, 63], adjust sharpen gain for negative hf based on image brightness*/
    AX_U8                            nLumaGainLutPos[AX_IVPS_REF_AUTOTBL_SHARPEN_EXPRESS_NUM][AX_IVPS_SHP_GAIN_SIZE];   /* Accuracy: U1.5, Range: [0, 63], adjust sharpen gain for positive hf based on image brightness*/
    AX_U8                            nNrStr[AX_IVPS_REF_AUTOTBL_SHARPEN_EXPRESS_NUM];                                   /* Accuracy: U1.5, Range: [0, 32], larger value means stronger noise reduction*/
    AX_U8                            nNrUdTextureSens[AX_IVPS_REF_AUTOTBL_SHARPEN_EXPRESS_NUM];                         /* Accuracy: U0.8, Range: [0, 255], larger value means image has more undirectional textures and apply less noise reduction*/
    AX_U8                            nNrUdTextureThr[AX_IVPS_REF_AUTOTBL_SHARPEN_EXPRESS_NUM];                          /* Accuracy: U0.8, Range: [0, 255], larger value means less pixels will be considered as real texture and apply more noise reduction*/
    AX_U16                           nNrUdLimit[AX_IVPS_REF_AUTOTBL_SHARPEN_EXPRESS_NUM];                               /* Accuracy: U8.2, Range: [0, 1023], larger value means less restriction on undirectional noise reduction*/
    AX_U8                            nUdCoring[AX_IVPS_REF_AUTOTBL_SHARPEN_EXPRESS_NUM];                                /* Accuracy: U3.3, Range: [0, 63], larger value means stronger coring for undirectional details*/
    AX_U8                            nNrDirEdgeSens[AX_IVPS_REF_AUTOTBL_SHARPEN_EXPRESS_NUM];                           /* Accuracy: U0.8, Range: [0, 255], larger value means image has more directional edges and apply less noise reduction*/
    AX_U8                            nNrDirEdgeThr[AX_IVPS_REF_AUTOTBL_SHARPEN_EXPRESS_NUM];                            /* Accuracy: U4.4, Range: [0, 255], larger value means less pixels will be considered as real edge and apply more noise reduction*/
    AX_U16                           nNrDirLimit[AX_IVPS_REF_AUTOTBL_SHARPEN_EXPRESS_NUM];                              /* Accuracy: U8.2, Range: [0, 1023], larger value means less restriction on directional noise reduction*/
    AX_U8                            nDirCoring[AX_IVPS_REF_AUTOTBL_SHARPEN_EXPRESS_NUM];                               /* Accuracy: U3.3, Range: [0, 63], larger value means stronger coring for directional details*/
} AX_IVPS_IQ_SHARPEN_AUTO_T;

typedef struct {
    AX_U8                            nSharpenEn;     /* sharpen enable, Accuracy: U1.0 Range: [0, 1], 0: Disable, 1: Enable */
    AX_U8                            nAutoMode;      /* for ref auto or manual adjust mode, Accuracy: U1.0 Range:[0, 1], 0: manual, 1:auto, default:1 */
    AX_IVPS_IQ_SHARPEN_MANUAL_T      tManualParam;
    AX_IVPS_IQ_SHARPEN_AUTO_T        tAutoParam;
} AX_IVPS_IQ_SHARPEN_PARAM_EX_T;
/************************************************************************************
 *  IVPS IQ API
 ************************************************************************************/
AX_S32 AX_IVPS_LoadBinParams(IVPS_GRP IvpsGrp, const AX_CHAR *pFileName);
AX_S32 AX_IVPS_IQ_SetSharpenParam(IVPS_GRP IvpsGrp, AX_IVPS_IQ_SHARPEN_PARAM_T *pIvpsSharpenParam);
AX_S32 AX_IVPS_IQ_GetSharpenParam(IVPS_GRP IvpsGrp, AX_IVPS_IQ_SHARPEN_PARAM_T *pIvpsSharpenParam);
AX_S32 AX_IVPS_IQ_SetSharpenParamEx(IVPS_GRP IvpsGrp, AX_IVPS_IQ_SHARPEN_PARAM_EX_T *pIvpsSharpenParam);
AX_S32 AX_IVPS_IQ_GetSharpenParamEx(IVPS_GRP IvpsGrp, AX_IVPS_IQ_SHARPEN_PARAM_EX_T *pIvpsSharpenParam);

#ifdef __cplusplus
}
#endif
#endif  //_AX_IVPS_IQ_API_H_
