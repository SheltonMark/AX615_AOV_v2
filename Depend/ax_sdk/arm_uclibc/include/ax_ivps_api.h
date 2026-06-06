/**************************************************************************************************
 *
 * Copyright (c) 2019-2025 Axera Semiconductor Co., Ltd. All Rights Reserved.
 *
 * This source file is the property of Axera Semiconductor Co., Ltd. and
 * may not be copied or distributed in any isomorphic form without the prior
 * written consent of Axera Semiconductor Co., Ltd.
 *
 **************************************************************************************************/

#ifndef _AX_IVPS_API_H_
#define _AX_IVPS_API_H_
#include "ax_base_type.h"
#include "ax_global_type.h"
#include "ax_ivps_type.h"
#include "ax_ivps_iq_api.h"

#ifdef __cplusplus
extern "C"
{
#endif

    /***************************************************************************************************************/
    /*                                                   PIPELINE                                                  */
    /***************************************************************************************************************/
    AX_S32 AX_IVPS_Init(AX_VOID);
    AX_S32 AX_IVPS_Deinit(AX_VOID);
    AX_S32 AX_IVPS_CreateGrp(IVPS_GRP IvpsGrp, const AX_IVPS_GRP_ATTR_T *ptGrpAttr);
    AX_S32 AX_IVPS_CreateGrpEx(IVPS_GRP *IvpsGrp, const AX_IVPS_GRP_ATTR_T *ptGrpAttr);
    AX_S32 AX_IVPS_DestroyGrp(IVPS_GRP IvpsGrp);
    AX_S32 AX_IVPS_SetPipelineAttr(IVPS_GRP IvpsGrp, AX_IVPS_PIPELINE_ATTR_T *ptPipelineAttr);
    AX_S32 AX_IVPS_GetPipelineAttr(IVPS_GRP IvpsGrp, AX_IVPS_PIPELINE_ATTR_T *ptPipelineAttr);
    AX_S32 AX_IVPS_StartGrp(IVPS_GRP IvpsGrp);
    AX_S32 AX_IVPS_StopGrp(IVPS_GRP IvpsGrp);
    AX_S32 AX_IVPS_EnableChn(IVPS_GRP IvpsGrp, IVPS_CHN IvpsChn);
    AX_S32 AX_IVPS_DisableChn(IVPS_GRP IvpsGrp, IVPS_CHN IvpsChn);
    AX_S32 AX_IVPS_SendFrame(IVPS_GRP IvpsGrp, const AX_VIDEO_FRAME_T *ptFrame, AX_S32 nMilliSec);
    AX_S32 AX_IVPS_GetChnFrame(IVPS_GRP IvpsGrp, IVPS_CHN IvpsChn, AX_VIDEO_FRAME_T *ptFrame, AX_S32 nMilliSec);
    AX_S32 AX_IVPS_ReleaseChnFrame(IVPS_GRP IvpsGrp, IVPS_CHN IvpsChn, AX_VIDEO_FRAME_T *ptFrame);
    AX_S32 AX_IVPS_GetChnFd(IVPS_GRP IvpsGrp, IVPS_CHN IvpsChn);
    AX_S32 AX_IVPS_GetDebugFifoFrame(IVPS_GRP IvpsGrp, AX_VIDEO_FRAME_T *ptFrame);
    AX_S32 AX_IVPS_ReleaseDebugFifoFrame(IVPS_GRP IvpsGrp, AX_VIDEO_FRAME_T *ptFrame);
    AX_S32 AX_IVPS_CloseAllFd(AX_VOID);

    AX_S32 AX_IVPS_SetGrpLDCAttr(IVPS_GRP IvpsGrp, IVPS_FILTER IvpsFilter,
                                 const AX_IVPS_LDC_ATTR_T *ptLDCAttr);
    AX_S32 AX_IVPS_GetGrpLDCAttr(IVPS_GRP IvpsGrp, IVPS_FILTER IvpsFilter,
                                 AX_IVPS_LDC_ATTR_T *ptLDCAttr);
    AX_S32 AX_IVPS_SetChnLDCAttr(IVPS_GRP IvpsGrp, IVPS_CHN IvpsChn, IVPS_FILTER IvpsFilter,
                                 const AX_IVPS_LDC_ATTR_T *ptLDCAttr);
    AX_S32 AX_IVPS_GetChnLDCAttr(IVPS_GRP IvpsGrp, IVPS_CHN IvpsChn, IVPS_FILTER IvpsFilter,
                                 AX_IVPS_LDC_ATTR_T *ptLDCAttr);

    AX_S32 AX_IVPS_SetGrpPoolAttr(IVPS_GRP IvpsGrp, const AX_IVPS_POOL_ATTR_T *ptPoolAttr);
    AX_S32 AX_IVPS_SetChnPoolAttr(IVPS_GRP IvpsGrp, IVPS_CHN IvpsChn, const AX_IVPS_POOL_ATTR_T *ptPoolAttr);

    AX_S32 AX_IVPS_SetGrpCrop(IVPS_GRP IvpsGrp, const AX_IVPS_CROP_INFO_T *ptCropInfo);
    AX_S32 AX_IVPS_GetGrpCrop(IVPS_GRP IvpsGrp, AX_IVPS_CROP_INFO_T *ptCropInfo);
    AX_S32 AX_IVPS_SetChnAttr(IVPS_GRP IvpsGrp, IVPS_CHN IvpsChn, IVPS_FILTER IvpsFilter,
							  const AX_IVPS_CHN_ATTR_T *ptChnAttr);
    AX_S32 AX_IVPS_GetChnAttr(IVPS_GRP IvpsGrp, IVPS_CHN IvpsChn, IVPS_FILTER IvpsFilter,
							  AX_IVPS_CHN_ATTR_T *ptChnAttr);

    AX_S32 AX_IVPS_PointQueryDst2SrcRatio(IVPS_GRP IvpsGrp, const AX_IVPS_POINT_RATIO_T *ptDst, AX_IVPS_POINT_RATIO_T *ptSrc);
    AX_S32 AX_IVPS_GdcPointQuerySrc2Dst(AX_IVPS_SIZE_T tInputFrame,
                const AX_IVPS_GDC_CFG_T *ptCfg, const AX_IVPS_POINT_NICE_T *ptSrc, AX_IVPS_POINT_NICE_T *ptDst);
    /***************************************************************************************************************/
    /*                                               REGION                                                        */
    /***************************************************************************************************************/
    /*
    Create region
    @return : return the region handle created
    */
    IVPS_RGN_HANDLE AX_IVPS_RGN_Create(AX_VOID);
    /*
    Destroy created region
    @param - [IN]  hRegion: specifies the region handle created by AX_IVPS_RGN_Create
    */
    AX_S32 AX_IVPS_RGN_Destroy(IVPS_RGN_HANDLE hRegion);
    /*
    Attach region to IVPS channel
    @param - [IN]  hRegion: specifies the region handle created by AX_IVPS_RGN_Create
    @param - [IN]  IvpsGrp:  specifies the group to attach.
    @param - [IN]  IvpsFilter:  specifies the filter of the group to attach.
    */
    AX_S32 AX_IVPS_RGN_AttachToFilter(IVPS_RGN_HANDLE hRegion, IVPS_GRP IvpsGrp, IVPS_FILTER_ID IvpsFilter);
    /*
    Detach region from IVPS channel
    @param - [IN]  hRegion: specifies the region handle created by AX_IVPS_RGN_Create
    @param - [IN]  IvpsGrp:  specifies the group to detach.
    @param - [IN]  IvpsFilter:  specifies the filter of the group to detach.
    */
    AX_S32 AX_IVPS_RGN_DetachFromFilter(IVPS_RGN_HANDLE hRegion, IVPS_GRP IvpsGrp, IVPS_FILTER_ID IvpsFilter);
    AX_S32 AX_IVPS_RGN_Update(IVPS_RGN_HANDLE hRegion, const AX_IVPS_RGN_DISP_GROUP_T *ptDisp);

    AX_S32 AX_IVPS_RGN_PaletteTableSet(const AX_IVPS_PALETTE_TABLE_T *ptPaletteTable);
    AX_S32 AX_IVPS_RGN_PaletteTableGet(AX_IVPS_PALETTE_TABLE_T *ptPaletteTable);
    AX_S32 AX_IVPS_RGN_LumaSumSet(IVPS_GRP IvpsGrp, IVPS_CHN IvpsChn, IVPS_FILTER IvpsFilter,
                      AX_U32 nNum, const AX_IVPS_RECT_T tRect[]);
    AX_S32 AX_IVPS_RGN_LumaSumGet(IVPS_GRP IvpsGrp, IVPS_CHN IvpsChn, IVPS_FILTER IvpsFilter,
                      AX_U32 nNum, AX_U8 *pLumaSum);
    /***************************************************************************************************************/
    /*                                                   SYNC API                                                  */
    /***************************************************************************************************************/
    /***************************************************************************************************************/
    /*                                                   TDP (no use)                                              */
    /***************************************************************************************************************/
    AX_S32 AX_IVPS_CropResizeTdp(const AX_VIDEO_FRAME_T *ptSrc, AX_VIDEO_FRAME_T *ptDst,
                                 const AX_IVPS_CROP_RESIZE_ATTR_T *ptAttr);
    AX_S32 AX_IVPS_CscTdp(const AX_VIDEO_FRAME_T *ptSrc, AX_VIDEO_FRAME_T *ptDst);

    /***************************************************************************************************************/
    /*                                                   VPP                                                       */
    /***************************************************************************************************************/
    AX_S32 AX_IVPS_CropResizeVpp(const AX_VIDEO_FRAME_T *ptSrc, AX_VIDEO_FRAME_T *ptDst,
                                 const AX_IVPS_CROP_RESIZE_ATTR_T *ptAttr);
    AX_S32 AX_IVPS_CropResizeV2Vpp(const AX_VIDEO_FRAME_T *ptSrc, AX_U8 nCropNum, const AX_IVPS_RECT_T tBox[],
                                   AX_VIDEO_FRAME_T ptDst[], const AX_IVPS_CROP_RESIZE_ATTR_T *ptAttr);
    AX_S32 AX_IVPS_SetScaleCoefLevelVpp(const AX_IVPS_SCALE_RANGE_T *ptScaleRange, const AX_IVPS_SCALE_COEF_LEVEL_T *ptCoefLevel);
    AX_S32 AX_IVPS_GetScaleCoefLevelVpp(const AX_IVPS_SCALE_RANGE_T *ptScaleRange, AX_IVPS_SCALE_COEF_LEVEL_T *ptCoefLevel);

    /***************************************************************************************************************/
    /*                                                   CPU                                                       */
    /***************************************************************************************************************/
    AX_S32 AX_IVPS_DrawLine(const AX_IVPS_RGN_CANVAS_INFO_T *ptCanvas, AX_IVPS_GDI_ATTR_T tAttr,
                            const AX_IVPS_POINT_T tPoint[], AX_U32 nPointNum);
    AX_S32 AX_IVPS_DrawPolygon(const AX_IVPS_RGN_CANVAS_INFO_T *ptCanvas, AX_IVPS_GDI_ATTR_T tAttr,
                            const AX_IVPS_POINT_T tPoint[], AX_U32 nPointNum);
    AX_S32 AX_IVPS_DrawRect(const AX_IVPS_RGN_CANVAS_INFO_T *ptCanvas, AX_IVPS_GDI_ATTR_T tAttr,
                            AX_IVPS_RECT_T tRect);

    /***************************************************************************************************************/
    /*                                                   GDC                                                       */
    /***************************************************************************************************************/
    AX_S32 AX_IVPS_Dewarp(const AX_VIDEO_FRAME_T *ptSrc, AX_VIDEO_FRAME_T *ptDst, const AX_IVPS_DEWARP_ATTR_T *ptAttr);

    /***************************************************************************************************************/
    /*                                                   VGP VPP                                                   */
    /***************************************************************************************************************/
	AX_S32 AX_IVPS_Csc(AX_IVPS_ENGINE_E eEngine, const AX_VIDEO_FRAME_T *ptSrc, AX_VIDEO_FRAME_T *ptDst, const AX_CSC_PARAM_T *ptParam);
	AX_S32 AX_IVPS_AlphaBlending(AX_IVPS_ENGINE_E eEngine, const AX_VIDEO_FRAME_T *ptSrc, const AX_VIDEO_FRAME_T *ptOverlay,
                                 const AX_IVPS_POINT_T tOffset, AX_U8 nAlpha, AX_VIDEO_FRAME_T *ptDst);
    AX_S32 AX_IVPS_DrawOsd(AX_IVPS_ENGINE_E eEngine, const AX_VIDEO_FRAME_T *ptSrc, const AX_OSD_BMP_ATTR_T arrBmp[], AX_U32 nNum);
    AX_S32 AX_IVPS_DrawMosaic(AX_IVPS_ENGINE_E eEngine, const AX_VIDEO_FRAME_T *ptSrc, AX_IVPS_RGN_MOSAIC_T tMosaic[], AX_U32 nNum);
	AX_S32 AX_IVPS_ROILumaSum(AX_IVPS_ENGINE_E eEngine, const AX_VIDEO_FRAME_T *ptSrc, AX_U32 nNum, AX_IVPS_RECT_T tRect[], AX_U32 *pLumaSum);


#ifdef __cplusplus
}
#endif
#endif /* _AX_IVPS_API_H_ */
