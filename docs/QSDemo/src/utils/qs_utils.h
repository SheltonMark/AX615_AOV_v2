/**************************************************************************************************
 *
 * Copyright (c) 2019-2025 Axera Semiconductor Co., Ltd. All Rights Reserved.
 *
 * This source file is the property of Axera Semiconductor Co., Ltd. and
 * may not be copied or distributed in any isomorphic form without the prior
 * written consent of Axera Semiconductor Co., Ltd.
 *
 **************************************************************************************************/

#ifndef _QS_UTILS_H__
#define _QS_UTILS_H__

#include "ax_global_type.h"
#include <math.h>

#ifndef __RTT_OS__
#include <sys/prctl.h>
#endif

#define IS_FPS_EQUAL(float_a, float_b)    ((fabs(float_a - float_b) < (0.01)) ? (1) : (0))

#define QS_SERIALIZE_ARG(var) &(var), sizeof(var)
#define QS_SERIALIZE(out_size, ...) QS_Serialize(out_size, __VA_ARGS__, NULL, 0)
#define QS_DESERIALIZE(buffer, ...) QS_Deserialize(buffer, __VA_ARGS__, NULL, 0)

typedef enum _E_FLAG_FILE {
    E_FLAG_FILE_TIMES = 0,
    E_FLAG_FILE_REBOOT,
    E_FLAG_FILE_AOV
}E_FLAG_FILE;

const AX_CHAR* GetFlagFilePath(E_FLAG_FILE eFlag);
const AX_CHAR* GetFlagFile(E_FLAG_FILE eFlag);
const AX_CHAR* QS_GetFileExt(const AX_CHAR* sFile);

AX_VOID MSSleep(AX_U32 nSleep);
AX_S32  QS_RunCmd(AX_CHAR* cmd, AX_CHAR *result, AX_S32 len);
AX_S32  QS_GetSingleSnsPipeId(AX_S32 nDefault);
AX_S32  QS_SetSnsSwitch(AX_S32 nSnsId);
AX_S32  QS_GetSnsSwitch(AX_S32 nDefault);
AX_S32  QS_GetSnsHdrMode(AX_S32 nDefault);

// QS_Serialize
// param: out size (MUST)
//        arg1, sizoef(arg1),
//        arg2, sizoef(arg2),
//        arg3, sizoef(arg3),
//        arg4, sizoef(arg4),
//        ......,  ......,
//        NULL, 0  (MUST)
// out buffer should free
AX_VOID* QS_Serialize(AX_S32* out_size, ...);

// QS_Deserialize
// param: buffer (MUST)
//        arg1, sizoef(arg1),
//        arg2, sizoef(arg2),
//        arg3, sizoef(arg3),
//        arg4, sizoef(arg4),
//        ......,  ......,
AX_VOID QS_Deserialize(AX_VOID* buffer, ...);

AX_S32  QS_SetThreadName(const char* name);
#endif //_QS_UTILS_H__
