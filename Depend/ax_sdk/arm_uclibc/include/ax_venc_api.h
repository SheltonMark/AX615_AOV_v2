/**************************************************************************************************
 *
 * Copyright (c) 2019-2025 Axera Semiconductor Co., Ltd. All Rights Reserved.
 *
 * This source file is the property of Axera Semiconductor Co., Ltd. and
 * may not be copied or distributed in any isomorphic form without the prior
 * written consent of Axera Semiconductor Co., Ltd.
 *
 **************************************************************************************************/

#ifndef __AX_VENC_API_H__
#define __AX_VENC_API_H__

#include <stdbool.h>

#include "ax_venc_comm.h"


#ifdef __cplusplus
extern "C" {
#endif /* End of #ifdef __cplusplus */

AX_S32 AX_VENC_Init(const AX_VENC_MOD_ATTR_T *pstModAttr);
AX_S32 AX_VENC_Deinit();

AX_S32 AX_VENC_CreateChn(VENC_CHN VeChn, const AX_VENC_CHN_ATTR_T *pstAttr);
AX_S32 AX_VENC_CreateChnEx(VENC_CHN *pVeChn, const AX_VENC_CHN_ATTR_T *pstAttr);
AX_S32 AX_VENC_DestroyChn(VENC_CHN VeChn);

AX_S32 AX_VENC_SendFrame(VENC_CHN VeChn, const AX_VIDEO_FRAME_INFO_T *pstFrame, AX_S32 s32MilliSec);
AX_S32 AX_VENC_SendFrameEx(VENC_CHN VeChn, const AX_USER_FRAME_INFO_T *pstFrame, AX_S32 s32MilliSec);

AX_S32 AX_VENC_SelectGrp(VENC_GRP grpId, AX_CHN_STREAM_STATUS_T *pstChnStrmState, AX_S32 s32MilliSec);
AX_S32 AX_VENC_SelectClearGrp(VENC_GRP grpId);
AX_S32 AX_VENC_SelectGrpAddChn(VENC_GRP grpId, VENC_CHN VeChn);
AX_S32 AX_VENC_SelectGrpDeleteChn(VENC_GRP grpId, VENC_CHN VeChn);
AX_S32 AX_VENC_SelectGrpQuery(VENC_GRP grpId, AX_VENC_SELECT_GRP_PARAM_T *pstGrpInfo);
AX_S32 AX_VENC_GetFd(VENC_CHN VeChn);
AX_S32 AX_VENC_GetStream(VENC_CHN VeChn, AX_VENC_STREAM_T *pstStream, AX_S32 s32MilliSec);
AX_S32 AX_VENC_ReleaseStream(VENC_CHN VeChn, const AX_VENC_STREAM_T *pstStream);
AX_S32 AX_VENC_GetStreamBufInfo(VENC_CHN VeChn, AX_VENC_STREAM_BUF_INFO_T *pstStreamBufInfo);

AX_S32 AX_VENC_StartRecvFrame(VENC_CHN VeChn, const AX_VENC_RECV_PIC_PARAM_T *pstRecvParam);
AX_S32 AX_VENC_StopRecvFrame(VENC_CHN VeChn);
AX_S32 AX_VENC_ResetChn(VENC_CHN VeChn);

AX_S32 AX_VENC_SetRoiAttr(VENC_CHN VeChn, const AX_VENC_ROI_ATTR_T *pstRoiAttr);
AX_S32 AX_VENC_GetRoiAttr(VENC_CHN VeChn, AX_U32 u32Index, AX_VENC_ROI_ATTR_T *pstRoiAttr);

AX_S32 AX_VENC_SetRoiMvAttr(VENC_CHN VeChn, const AX_VENC_ROI_MV_ATTR_T *pstRoiMvAttr);
AX_S32 AX_VENC_GetRoiMvAttr(VENC_CHN VeChn, AX_VENC_ROI_MV_ATTR_T *pstRoiMvAttr);

AX_S32 AX_VENC_EnableNRoiLowFrameRate(VENC_CHN VeChn);
AX_S32 AX_VENC_DisableNRoiLowFrameRate(VENC_CHN VeChn);

AX_S32 AX_VENC_SetOsdAttr(VENC_CHN VeChn, const AX_VENC_OSD_ATTR_T *pstOsdAttr);
AX_S32 AX_VENC_GetOsdAttr(VENC_CHN VeChn, AX_VENC_OSD_ATTR_T *pstOsdAttr);

AX_S32 AX_VENC_SetRcParam(VENC_CHN VeChn, const AX_VENC_RC_PARAM_T *pstRcParam);
AX_S32 AX_VENC_GetRcParam(VENC_CHN VeChn, AX_VENC_RC_PARAM_T *pstRcParam);

AX_S32 AX_VENC_SetVuiParam(VENC_CHN VeChn, const AX_VENC_VUI_PARAM_T *pstVuiParam);
AX_S32 AX_VENC_GetVuiParam(VENC_CHN VeChn, AX_VENC_VUI_PARAM_T *pstVuiParam);

AX_S32 AX_VENC_SetChnAttr(VENC_CHN VeChn, const AX_VENC_CHN_ATTR_T *pstChnAttr);
AX_S32 AX_VENC_GetChnAttr(VENC_CHN VeChn, AX_VENC_CHN_ATTR_T *pstChnAttr);

AX_S32 AX_VENC_SetRateJamStrategy(VENC_CHN VeChn, const AX_VENC_RATE_JAM_CFG_T *pstRateJamParam);
AX_S32 AX_VENC_GetRateJamStrategy(VENC_CHN VeChn, AX_VENC_RATE_JAM_CFG_T *pstRateJamParam);

AX_S32 AX_VENC_SetSuperFrameStrategy(VENC_CHN VeChn, const AX_VENC_SUPERFRAME_CFG_T *pstSuperFrameCfg);
AX_S32 AX_VENC_GetSuperFrameStrategy(VENC_CHN VeChn, AX_VENC_SUPERFRAME_CFG_T *pstSuperFrameCfg);

AX_S32 AX_VENC_SetIntraRefresh(VENC_CHN VeChn, const AX_VENC_INTRA_REFRESH_T *pstIntraRefresh);
AX_S32 AX_VENC_GetIntraRefresh(VENC_CHN VeChn, AX_VENC_INTRA_REFRESH_T *pstIntraRefresh);

AX_S32 AX_VENC_SetUsrData(VENC_CHN VeChn, const AX_VENC_USR_DATA_T *pstUsrData);
AX_S32 AX_VENC_GetUsrData(VENC_CHN VeChn, AX_VENC_USR_DATA_T *pstUsrData);

AX_S32 AX_VENC_SetSliceSplit(VENC_CHN VeChn, const AX_VENC_SLICE_SPLIT_T *pstSliceSplit);
AX_S32 AX_VENC_GetSliceSplit(VENC_CHN VeChn, AX_VENC_SLICE_SPLIT_T *pstSliceSplit);

AX_S32 AX_VENC_RequestIDR(VENC_CHN VeChn, AX_BOOL bInstant);

AX_S32 AX_VENC_QueryStatus(VENC_CHN VeChn, AX_VENC_CHN_STATUS_T *pstStatus);

AX_S32 AX_VENC_SetJpegParam(VENC_CHN VeChn, const AX_VENC_JPEG_PARAM_T *pstJpegParam);
AX_S32 AX_VENC_GetJpegParam(VENC_CHN VeChn, AX_VENC_JPEG_PARAM_T *pstJpegParam);

AX_S32 AX_VENC_JpegEncodeOneFrame(AX_JPEG_ENCODE_ONCE_PARAMS_T *pstJpegParam);

AX_S32 AX_VENC_JpegGetThumbnail(AX_VOID *pRawData, AX_VOID *pThumbData, AX_U32 *pThumbSize);

AX_S32 AX_VENC_EnableSvc(VENC_CHN VeChn, AX_BOOL bEnable);
AX_S32 AX_VENC_GetSvcParam(VENC_CHN VeChn, AX_VENC_SVC_PARAM_T *pstSvcParam);
AX_S32 AX_VENC_SetSvcParam(VENC_CHN VeChn, AX_VENC_SVC_PARAM_T *pstSvcParam);
AX_S32 AX_VENC_GetSvcRegion(VENC_CHN VeChn, AX_VENC_SVC_REGION_T *pstRegionParam);
AX_S32 AX_VENC_SetSvcRegion(VENC_CHN VeChn, AX_VENC_SVC_REGION_T *pstRegionParam);

AX_S32 AX_VENC_WakeUpWithIDR(VENC_CHN VeChn, AX_BOOL bEnable);

AX_S32 AX_VENC_SetStreamCheck(VENC_CHN VeChn, AX_VENC_STREAM_CHECK_TYPE_E enCheckType);
AX_S32 AX_VENC_GetStreamCheck(VENC_CHN VeChn, AX_VENC_STREAM_CHECK_TYPE_E *penCheckType);
AX_S32 AX_VENC_EnableFakeMonochr(VENC_CHN VeChn);
AX_S32 AX_VENC_DisableFakeMonochr(VENC_CHN VeChn);
AX_S32 AX_VENC_SetDblkAttr(VENC_CHN VeChn, const AX_VENC_DBLK_ATTR_T *pstDblkAttr);
AX_S32 AX_VENC_GetDblkAttr(VENC_CHN VeChn, AX_VENC_DBLK_ATTR_T *pstDblkAttr);
AX_S32 AX_VENC_SetCirParam(VENC_CHN VeChn, const AX_VENC_CIR_PARAM_T *pstCirParam);
AX_S32 AX_VENC_GetCirParam(VENC_CHN VeChn, AX_VENC_CIR_PARAM_T *pstCirParam);
AX_S32 AX_VENC_SetH265Sao(VENC_CHN VeChn, const AX_VENC_SAO_PARAM_T *pstSaoParam);
AX_S32 AX_VENC_GetH265Sao(VENC_CHN VeChn, AX_VENC_SAO_PARAM_T *pstSaoParam);

AX_S32 AX_VENC_SetIntraArea(VENC_CHN VeChn, const AX_VENC_INTRA_AREA_T *pstIntraArea);
AX_S32 AX_VENC_GetIntraArea(VENC_CHN VeChn, AX_VENC_INTRA_AREA_T *pstIntraArea);

AX_S32 AX_VENC_SetCuInfoOutputParam(VENC_CHN VeChn, const AX_VENC_CUINFO_PARAM_T *pstCuinfoParam);
AX_S32 AX_VENC_SetDeBreathEffect(VENC_CHN VeChn, const AX_VENC_DEBREATH_EFFECT_T *pstDeBreathEffect);
AX_S32 AX_VENC_GetDeBreathEffect(VENC_CHN VeChn, AX_VENC_DEBREATH_EFFECT_T *pstDeBreathEffect);
AX_S32 AX_VENC_SetDeBlurringEffect(VENC_CHN VeChn, const AX_VENC_DEBLURRING_EFFECT_T *pstDeBlurringEffect);
AX_S32 AX_VENC_GetDeBlurringEffect(VENC_CHN VeChn, AX_VENC_DEBLURRING_EFFECT_T *pstDeBlurringEffect);
AX_S32 AX_VENC_SetFgProtectAttr(VENC_CHN VeChn, const AX_VENC_FG_PROTECT_T *pstFgProtect);
AX_S32 AX_VENC_GetFgProtectAttr(VENC_CHN VeChn, AX_VENC_FG_PROTECT_T *pstFgProtect);

AX_S32 AX_VENC_SetRdoAttr(VENC_CHN VeChn, const AX_VENC_RDO_ATTR_T *pstRdoAttr);
AX_S32 AX_VENC_GetRdoAttr(VENC_CHN VeChn, AX_VENC_RDO_ATTR_T *pstRdoAttr);
AX_S32 AX_VENC_SetSkinDetectAttr(VENC_CHN VeChn, const AX_VENC_SKIN_DETECT_ATTR_T *pstSkinDetectAttr);
AX_S32 AX_VENC_GetSkinDetectAttr(VENC_CHN VeChn, AX_VENC_SKIN_DETECT_ATTR_T *pstSkinDetectAttr);
AX_S32 AX_VENC_SetChromaProtectAttr(VENC_CHN VeChn, const AX_VENC_CHROMA_PROTECT_ATTR_T *pstChrProtAttr);
AX_S32 AX_VENC_GetChromaProtectAttr(VENC_CHN VeChn, AX_VENC_CHROMA_PROTECT_ATTR_T *pstChrProtAttr);
AX_S32 AX_VENC_SetSubjQualRecAttr(VENC_CHN VeChn, const AX_VENC_SUBJ_QUAL_REC_ATTR_T *pstQualRecAttr);
AX_S32 AX_VENC_GetSubjQualRecAttr(VENC_CHN VeChn, AX_VENC_SUBJ_QUAL_REC_ATTR_T *pstQualRecAttr);
AX_S32 AX_VENC_SetPmCtrlAttr(VENC_CHN VeChn, const AX_VENC_PM_CTRL_ATTR_T *pstPmCtrlAttr);
AX_S32 AX_VENC_GetPmCtrlAttr(VENC_CHN VeChn, AX_VENC_PM_CTRL_ATTR_T *pstPmCtrlAttr);
AX_S32 AX_VENC_SetSceneDetectAttr(VENC_CHN VeChn, const AX_VENC_SCENE_DETECT_ATTR_T *pstSceneDetectAttr);
AX_S32 AX_VENC_GetSceneDetectAttr(VENC_CHN VeChn, AX_VENC_SCENE_DETECT_ATTR_T *pstSceneDetectAttr);
AX_S32 AX_VENC_SetSubjSplitAttr(VENC_CHN VeChn, const AX_VENC_SUBJ_SPLIT_ATTR_T *pstSubjSplitAttr);
AX_S32 AX_VENC_GetSubjSplitAttr(VENC_CHN VeChn, AX_VENC_SUBJ_SPLIT_ATTR_T *pstSubjSplitAttr);
AX_S32 AX_VENC_SetInterStatAttr(VENC_CHN VeChn, const AX_VENC_INTER_STAT_ATTR_T *pstInterStatAttr);
AX_S32 AX_VENC_GetInterStatAttr(VENC_CHN VeChn, AX_VENC_INTER_STAT_ATTR_T *pstInterStatAttr);

AX_S32 AX_VENC_SetLowPowerLevel(VENC_CHN VeChn, AX_U32 u32LpLevel);
AX_S32 AX_VENC_GetLowPowerLevel(VENC_CHN VeChn, AX_U32 *pu32LpLevel);

AX_S32 AX_VENC_SetLocalMapParam(VENC_CHN VeChn, const AX_VENC_LOCAL_PARAM_T *pstLocalParam);
AX_S32 AX_VENC_GetLocalMapParam(VENC_CHN VeChn, AX_VENC_LOCAL_PARAM_T *pstLocalParam);

AX_S32 AX_VENC_SetHVSGainParam(VENC_CHN VeChn, const AX_VENC_HVSGAIN_PARAM_T *pstHvsGainParam);
AX_S32 AX_VENC_GetHVSGainParam(VENC_CHN VeChn, AX_VENC_HVSGAIN_PARAM_T *pstHvsGainParam);

AX_S32 AX_VENC_SetModParam(const AX_VENC_MOD_PARAM_T *pstModParam);
AX_S32 AX_VENC_GetModParam(AX_VENC_MOD_PARAM_T *pstModParam);

AX_S32 AX_VENC_SetStrmHeadAttr(VENC_CHN VeChn, const AX_VENC_STRM_HEAD_ATTR_T *pstStrmHeadAttr);
AX_S32 AX_VENC_GetStrmHeadAttr(VENC_CHN VeChn, AX_VENC_STRM_HEAD_ATTR_T *pstStrmHeadAttr);

#ifdef __cplusplus
}
#endif /* End of #ifdef __cplusplus */

#endif /* End of __AX_VENC_API_H__ */
