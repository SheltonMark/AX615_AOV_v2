/**************************************************************************************************
 *
 * Copyright (c) 2019-2025 Axera Semiconductor Co., Ltd. All Rights Reserved.
 *
 * This source file is the property of Axera Semiconductor Co., Ltd. and
 * may not be copied or distributed in any isomorphic form without the prior
 * written consent of Axera Semiconductor Co., Ltd.
 *
 **************************************************************************************************/

#ifndef __AX_AUTOKIT_API_H__
#define __AX_AUTOKIT_API_H__

#include "ax_base_type.h"
#include "ax_isp_api_ext.h"

#ifdef __cplusplus
extern "C"
{
#endif

AX_S32 AX_ISP_AUTOKIT_Init(AX_U8 nPipeId);
AX_S32 AX_ISP_AUTOKIT_Deinit(AX_U8 nPipeId);
AX_S32 AX_ISP_AUTOKIT_Ctrl(AX_U8 nPipeId, AX_ISP_AUTOKIT_CTRL_CMD_E eCtrlCmd, AX_VOID *pDataIn, AX_VOID *pDataOut);
AX_S32 AX_ISP_AUTOKIT_Process(AX_U8 nPipeId, AX_ISP_AUTOKIT_INFO_T *pDataIn, AX_VOID *pManualParamInOut, AX_U32 *pUpdate);

#ifdef __cplusplus
}
#endif

#endif  // __AX_AUTOKIT_API_H__
