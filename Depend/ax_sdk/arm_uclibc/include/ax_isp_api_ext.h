/**************************************************************************************************
 *
 * Copyright (c) 2019-2025 Axera Semiconductor Co., Ltd. All Rights Reserved.
 *
 * This source file is the property of Axera Semiconductor Co., Ltd. and
 * may not be copied or distributed in any isomorphic form without the prior
 * written consent of Axera Semiconductor Co., Ltd.
 *
 **************************************************************************************************/

#ifndef __AX_ISP_API_EXT_H__
#define __AX_ISP_API_EXT_H__

#include "ax_base_type.h"
#include "ax_isp_error_code.h"
#include "ax_isp_common.h"
#include "ax_isp_api.h"

#ifdef __cplusplus
extern "C"
{
#endif

#define AX_AUTOKIT_LIB_NAME_SIZE_MAX    20

typedef enum _AX_ISP_AUTOKIT_CTRL_CMD_E_ {
    AX_ISP_AUTOKIT_CTRL_LOAD_PARAM          = 0,
    AX_ISP_AUTOKIT_CTRL_GET_PARAM           = 1,
    AX_ISP_AUTOKIT_CTRL_DEFAULT_PARAM       = 2,
    AX_ISP_AUTOKIT_CTRL_PROC_INFO           = 3,
    AX_ISP_AUTOKIT_CTRL_PROC_STATUS         = 4,
    AX_ISP_AUTOKIT_CTRL_DEFINE0,
    AX_ISP_AUTOKIT_CTRL_DEFINE1,
    AX_ISP_AUTOKIT_CTRL_MAX,
} AX_ISP_AUTOKIT_CTRL_CMD_E;


typedef struct _AX_ISP_AUTOKIT_INFO_T_ {
    AX_U8                   nPipeId;
    AX_ISP_SUB_ID_E         eModId;
    AX_SNS_HDR_MODE_E       eHdrMode;
    AX_ISP_AE_RESULT_T     *ptAeResult;
    AX_ISP_AWB_RESULT_T    *ptAwbResult;
} AX_ISP_AUTOKIT_INFO_T;


typedef struct _AX_ISP_AUTOKIT_LOAD_PARAM_T_ {
    AX_U8                   nPipeId;
    AX_ISP_SUB_ID_E         eModId;
    AX_U32                  nDataSize;
    AX_VOID                *pParam;
} AX_ISP_AUTOKIT_LOAD_PARAM_T;

typedef struct _AX_ISP_AUTOKIT_LIB_T_ {
    AX_S32                  nLibId;
    AX_CHAR                 szLibName[AX_AUTOKIT_LIB_NAME_SIZE_MAX];
} AX_ISP_AUTOKIT_LIB_T;


typedef struct _AX_ISP_AUTOKIT_FUNC_T_ {

    AX_S32(*pfnAutoKit_Init)(AX_U8 nPipeId);
    AX_S32(*pfnAutoKit_Deinit)(AX_U8 nPipeId);
    AX_S32(*pfnAutoKit_Process)(AX_U8 nPipeId, AX_ISP_AUTOKIT_INFO_T *pDataIn, AX_VOID *pManualParamInOut, AX_U32 *pUpdate);
    AX_S32(*pfnAutoKit_Ctrl)(AX_U8 nPipeId, AX_ISP_AUTOKIT_CTRL_CMD_E eCtrlCmd, AX_VOID *pDataIn, AX_VOID *pDataOut);

} AX_ISP_AUTOKIT_FUNC_T;


/************************************************************************************
 *  ISP EXT API
 ************************************************************************************/
AX_S32 AX_ISP_RegisterAutoKitLib(AX_U8 nPipeId, AX_ISP_AUTOKIT_LIB_T *pAutoKitLib, AX_ISP_AUTOKIT_FUNC_T *pAutoKitFuncCb);
AX_S32 AX_ISP_UnRegisterAutoKitLib(AX_U8 nPipeId);


#ifdef __cplusplus
}
#endif

#endif  // __AX_ISP_API_EXT_H__
