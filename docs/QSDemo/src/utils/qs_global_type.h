/**************************************************************************************************
 *
 * Copyright (c) 2019-2025 Axera Semiconductor Co., Ltd. All Rights Reserved.
 *
 * This source file is the property of Axera Semiconductor Co., Ltd. and
 * may not be copied or distributed in any isomorphic form without the prior
 * written consent of Axera Semiconductor Co., Ltd.
 *
 **************************************************************************************************/

#ifndef _QS_GLOBAL_TYPE_H_
#define _QS_GLOBAL_TYPE_H_

#include <stdbool.h>
#include "ax_base_type.h"
#include "ax_global_type.h"

#ifdef __RTT_OS__
#include <rtthread.h>
#define USE_STATIC_LIB
#define MIS20S1_SUPPORT
#define QS_SAMPLE_SNS_MIS20S1_SINGLE
#define TCM_SUPPORT

#define RTOS_DEFAULT_STACK_SIZE (1 * 1024 * 1024)
#define OSD_LOGO_STATIC_HEADER_FILE

#ifndef F_OK
#define F_OK    0
#define R_OK    4
#define W_OK    2
#define X_OK    1
#endif

#ifndef CLOCK_BOOTTIME
#define CLOCK_BOOTTIME            7
// #define CLOCK_REALTIME            0
// #define CLOCK_MONOTONIC           1
// #define CLOCK_PROCESS_CPUTIME_ID  2
// #define CLOCK_THREAD_CPUTIME_ID   3
#endif
#else
#include <sys/prctl.h>
#include <dlfcn.h>
#endif

typedef AX_VOID* HANDLE;

#ifndef TRUE
#define TRUE (1)
#endif  // TRUE

#ifndef FALSE
#define FALSE (0)
#endif  // FALSE

#ifndef FREEIF
#define FREEIF(p)     \
    do {              \
        if (!p) {     \
            free(p);  \
            p = NULL; \
        }             \
    } while (0)
#endif

#ifndef ARRAY_SIZE
#define ARRAY_SIZE(x) (sizeof(x) / sizeof(x[0]))
#endif

#ifndef AX_MAX
#define AX_MAX(a, b) (((a) > (b)) ? (a) : (b))
#endif

#ifndef AX_MIN
#define AX_MIN(a, b) (((a) < (b)) ? (a) : (b))
#endif

#ifndef ALIGN_UP
#define ALIGN_UP(x, a) ((((x) + ((a) - 1)) / a) * a)
#endif

#ifndef ALIGN_DOWN
#define ALIGN_DOWN(x, a) (((x) / (a)) * (a))
#endif

#ifndef COMM_ALIGN
#define COMM_ALIGN(x, n) (((x) + (n) - 1) & ~((n) - 1))
#endif

#ifndef DIV_ROUND_UP
#define DIV_ROUND_UP(n, d) (((n) + (d) - 1) / (d))
#endif

#ifndef IS_ALIGN_OF
#define IS_ALIGN_OF(x, a) (((x) & ((a) - 1)) == 0)
#endif

#ifndef HAS_FLAG
#define HAS_FLAG(F, C) ((C) == ((F) & (C)))
#endif

#ifndef ADD_FLAG
#define ADD_FLAG(F, C) F |= C
#endif

#ifndef DEL_FLAG
#define DEL_FLAG(F, C) F &= ~C
#endif

#ifndef SET_FLAG
#define SET_FLAG(F, C) F = C
#endif

#ifndef INFINITE
#define INFINITE ((AX_U32) - 1)
#endif

#ifndef AXQS_INVALID_ID
#define AXQS_INVALID_ID (AX_U32)(-1)
#endif

#define QS_IPC_URL "ipc:///tmp/qs.ipc"

#endif  // _QS_GLOBAL_TYPE_H_
