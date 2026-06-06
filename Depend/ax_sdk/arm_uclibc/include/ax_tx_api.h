/**************************************************************************************************
 *
 * Copyright (c) 2019-2025 Axera Semiconductor Co., Ltd. All Rights Reserved.
 *
 * This source file is the property of Axera Semiconductor Co., Ltd. and
 * may not be copied or distributed in any isomorphic form without the prior
 * written consent of Axera Semiconductor Co., Ltd.
 *
 **************************************************************************************************/

#ifndef _AX_TX_API_H_
#define _AX_TX_API_H_

#include "ax_tx_common.h"

#ifdef __cplusplus
extern "C"
{
#endif

AX_S32 AX_TX_Init(AX_VOID);
AX_S32 AX_TX_Deinit(AX_VOID);
AX_S32 AX_TX_Reset(AX_VOID);

AX_S32 AX_TX_CLK_SetClkEn(CLK_SRC_EN_S *pstClkEn);
AX_S32 AX_TX_CLK_GetClkEn(CLK_SRC_EN_S *pstClkEn);
AX_S32 AX_TX_CLK_SetClkSel(CLK_SRC_SEL_S *pstClkSel);
AX_S32 AX_TX_CLK_GetClkSel(CLK_SRC_SEL_S *pstClkSel);
AX_S32 AX_TX_CLK_SetBtClk(BT_IN_CLK_S *pstBtClk);
AX_S32 AX_TX_CLK_GetBtClk(BT_IN_CLK_S *pstBtClk);
AX_S32 AX_TX_CLK_SetAudMclk(AUD_MCLK_FREQ_E enFreq);
AX_S32 AX_TX_CLK_GetClkStatus(CLK_STATUS_S *pstClkStatus);

AX_S32 AX_TX_VI_SwitchWorkMode(const VI_WORK_PARAM_S *pstWorkParam);
AX_S32 AX_TX_VI_QueryWorkMode(VI_WORK_PARAM_S *pstWorkParam);
AX_S32 AX_TX_VI_SetDevAttr(const VI_DEV_ATTR_S *pstDevAttr);
AX_S32 AX_TX_VI_GetDevAttr(VI_DEV_ATTR_S *pstDevAttr);
AX_S32 AX_TX_VI_EnableDev(AX_VOID);
AX_S32 AX_TX_VI_DisableDev(AX_VOID);
AX_S32 AX_TX_VI_SetDataLine(AX_U8 *pu8DataLine);
AX_S32 AX_TX_VI_GetBtStatus(VI_BT_STATUS_S *pstBtParam);

AX_S32 AX_TX_VO_SetVideoParam(VO_VIDEO_PARAM_S *pstVideoParam);
AX_S32 AX_TX_VO_GetVideoParam(VO_VIDEO_PARAM_S *pstVideoParam);
AX_S32 AX_TX_VO_SetVideoExtParam(VO_VIDEO_EXT_PARAM_S *pstExtParam);
AX_S32 AX_TX_VO_GetVideoExtParam(VO_VIDEO_EXT_PARAM_S *pstExtParam);
AX_S32 AX_TX_VO_SetVideoEn(AX_BOOL bVideoEn);
AX_S32 AX_TX_VO_GetVideoEn(AX_BOOL *pbVideoEn);
AX_S32 AX_TX_VO_SetAudioParam(VO_AUDIO_PARAM_S *pstAudioParam);
AX_S32 AX_TX_VO_GetAudioParam(VO_AUDIO_PARAM_S *pstAudioParam);
AX_S32 AX_TX_VO_SetAudioExtParam(VO_AUDIO_EXT_PARAM_S *pstAudioExtParam);
AX_S32 AX_TX_VO_GetAudioExtParam(VO_AUDIO_EXT_PARAM_S *pstAudioExtParam);
AX_S32 AX_TX_VO_SetAudioPubAttr(VO_AUDIO_PUB_ATTR_S *pstPubAttr);
AX_S32 AX_TX_VO_GetAudioPubAttr(VO_AUDIO_PUB_ATTR_S *pstPubAttr);
AX_S32 AX_TX_VO_SetRs485Param(VO_RS485_PARAM_S *pstRs485Param);
AX_S32 AX_TX_VO_GetRs485Param(VO_RS485_PARAM_S *pstRs485Param);
AX_S32 AX_TX_VO_GetRs485TxbufCnt(AX_U32 *pu32TxbufCnt);
AX_S32 AX_TX_VO_GetRs485RxbufCnt(AX_U32 *pu32RxbufCnt);
AX_S32 AX_TX_VO_SendRs485Data(VO_RS485_DATA_S *pstTxdata);
AX_S32 AX_TX_VO_RecvRs485Data(VO_RS485_DATA_S *pstRxdata);
AX_S32 AX_TX_VO_SwitchRs485Position(VO_RS485_POSITION_E enVoRs485Position);
AX_S32 AX_TX_VO_ResetRs485(AX_VOID);
AX_S32 AX_TX_VO_SetSignalParam(VO_SIGNAL_PARAM_S *pstSigParam);
AX_S32 AX_TX_VO_GetSignalParam(VO_SIGNAL_PARAM_S *pstSigParam);
AX_S32 AX_TX_VO_SetBufferConfigParam(VO_BUFFER_CONFIG_PARAM_S *pstBufParam);
AX_S32 AX_TX_VO_GetBufferConfigParam(VO_BUFFER_CONFIG_PARAM_S *pstBufParam);
AX_S32 AX_TX_VO_SetDigitalCoaxialPnSeq(AX_VOID);

#ifdef __cplusplus
}
#endif

#endif /* _AX_TX_API_H_ */

