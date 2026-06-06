/**************************************************************************************************
 *
 * Copyright (c) 2019-2025 Axera Semiconductor Co., Ltd. All Rights Reserved.
 *
 * This source file is the property of Axera Semiconductor Co., Ltd. and
 * may not be copied or distributed in any isomorphic form without the prior
 * written consent of Axera Semiconductor Co., Ltd.
 *
 **************************************************************************************************/

#include "qs_utils.h"
#include <stdlib.h>
#include <stdarg.h>
#include "qs_log.h"
#include "ax_sys_api.h"
#include "ax_gzipd_api.h"
#include "qs_global_type.h"

AX_VOID MSSleep(AX_U32 nSleep) {
    usleep(nSleep * 1000);
}

AX_S32 QS_GetSingleSnsPipeId(AX_S32 nDefault) {
    AX_CHAR szResult[64] = {0};
    AX_S32 nRet = 0;
    AX_S32 nValue = 0;
    nRet = QS_RunCmd("/soc/scripts/ax_env.sh print qs_single_sns_pipe_id  | awk -F '=' '{print $2}'", szResult, 64);
    if (nRet == 0 && strlen(szResult) > 0) {
        nValue = atoi(szResult);
        /* rx0:0 rx1:1 */
        if (nValue == 0 || nValue == 1) {
            return nValue;
        } else {
            ALOGI("invalid single_sns_pipe_id: %s, use default: %d", szResult, nDefault);
            return nDefault;
        }
    } else {
        return nDefault;
    }
}

AX_S32 QS_RunCmd(AX_CHAR* cmd, AX_CHAR *result, AX_S32 size) {
    if (!cmd) {
        return -1;
    }

    FILE * fp = NULL;
#ifndef __RTT_OS__
    fp = popen(cmd, "r");
#endif
    if(fp == NULL){
        return -1;
    }
    if (result && size > 0) {
        AX_S32 ret = fread(result, 1, size - 1, fp);
        if (ret <= 0) {
#ifndef __RTT_OS__
            pclose(fp);
#endif
            return -1;
        }
    }
#ifndef __RTT_OS__
    pclose(fp);
#endif
    return 0;
}

AX_S32  QS_SetSnsSwitch(AX_S32 nSnsId) {
    AX_CHAR szCmd[256] = {0};
    sprintf(szCmd, "/soc/scripts/ax_env.sh set qs_sns_switch %d", nSnsId);
    return QS_RunCmd(szCmd, NULL, 0);
}

AX_S32  QS_GetSnsSwitch(AX_S32 nDefault) {
    AX_CHAR szResult[64] = {0};
    AX_S32 nRet = 0;
    AX_S32 nValue = 0;
    nRet = QS_RunCmd("/soc/scripts/ax_env.sh print qs_sns_switch | awk -F '=' '{print $2}'", szResult, 64);
    if (nRet == 0 && strlen(szResult) > 0) {
        nValue = atoi(szResult);
        if (nValue == 1 || nValue == 2) {
            return nValue;
        } else {
            ALOGI("invalid qs_sns_switch value: %s, use default: %d", szResult, nDefault);
            return nDefault;
        }
    } else {
        return nDefault;
    }
}

AX_S32  QS_GetSnsHdrMode(AX_S32 nDefault) {
    AX_CHAR szResult[64] = {0};
    AX_S32 nRet = 0;
    AX_S32 nValue = 0;
    nRet = QS_RunCmd("/soc/scripts/ax_env.sh print qs_sns_hdrmode | awk -F '=' '{print $2}'", szResult, 64);
    if (nRet == 0 && strlen(szResult) > 0) {
        nValue = atoi(szResult);
        if (nValue == 1 || nValue == 2 || nValue == 5) {
            return nValue;
        } else {
            ALOGI("invalid qs_sns_hdrmode value: %s, use default: %d", szResult, nDefault);
            return nDefault;
        }
    } else {
        return nDefault;
    }
}

const AX_CHAR* GetFlagFilePath(E_FLAG_FILE eFlag) {
    switch(eFlag) {
    case E_FLAG_FILE_TIMES:
    case E_FLAG_FILE_REBOOT:
    case E_FLAG_FILE_AOV:
        return "/customer";
    default:
        return "";
    }
}

const AX_CHAR* GetFlagFile(E_FLAG_FILE eFlag) {
    switch(eFlag) {
    case E_FLAG_FILE_TIMES:
        return "/customer/qs_times";
    case E_FLAG_FILE_REBOOT:
        return "/customer/reboot";
    case E_FLAG_FILE_AOV:
        return "/customer/aov";
    default:
        return "";
    }
}

const AX_CHAR* QS_GetFileExt(const AX_CHAR* sFile) {
    if (!sFile) {
        return NULL;
    }

    const AX_CHAR* dot = strrchr(sFile, '.');

    if (dot && dot != sFile) {
        return dot + 1;
    }

    return NULL;
}

AX_VOID* QS_Serialize(AX_S32* out_size, ...) {
    va_list args;
    va_start(args, out_size);

    size_t total_size = 0;
    va_list size_args;
    va_copy(size_args, args);

    void* arg;
    size_t size;

    arg = va_arg(size_args, void*);
    while (arg != NULL) {
        size = va_arg(size_args, size_t);
        total_size += size;
        arg = va_arg(size_args, void*);
    }
    va_end(size_args);

    void* buffer = malloc(total_size);
    if (!buffer) {
        va_end(args);
        return NULL;
    }

    char* ptr = (char*)buffer;
    arg = va_arg(args, void*);
    while (arg != NULL) {
        size = va_arg(args, size_t);
        memcpy(ptr, arg, size);
        ptr += size;
        arg = va_arg(args, void*);
    }

    va_end(args);

    *out_size = (AX_S32)total_size;

    return buffer;
}

AX_VOID QS_Deserialize(AX_VOID* buffer, ...) {
    va_list args;
    va_start(args, buffer);

    char* ptr = (char*)buffer;
    void* arg;
    size_t size;

    arg = va_arg(args, void*);
    while (arg != NULL) {
        size = va_arg(args, size_t);
        memcpy(arg, ptr, size);
        ptr += size;
        arg = va_arg(args, void*);
    }

    va_end(args);
}

AX_S32 QS_SetThreadName(const char* name)
{
#ifdef __RTT_OS__
    rt_thread_t thread = rt_thread_self();
    rt_strncpy(thread->name, name, RT_NAME_MAX);
#else
    prctl(PR_SET_NAME, name);
#endif
    return 0;
}
