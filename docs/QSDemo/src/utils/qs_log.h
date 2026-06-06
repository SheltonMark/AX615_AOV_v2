/**************************************************************************************************
 *
 * Copyright (c) 2019-2025 Axera Semiconductor Co., Ltd. All Rights Reserved.
 *
 * This source file is the property of Axera Semiconductor Co., Ltd. and
 * may not be copied or distributed in any isomorphic form without the prior
 * written consent of Axera Semiconductor Co., Ltd.
 *
 **************************************************************************************************/

#ifndef _QSCOMMON_H__
#define _QSCOMMON_H__

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <signal.h>
#include <errno.h>
#include <unistd.h>
#include "ax_global_type.h"
#include "qs_timer.h"

#define IS_AX615Q  (AX_SYS_GetChipType() == AX615QV200 || AX_SYS_GetChipType() == AX615QV201)

#define	APP_LOG_LEVEL_EMERG    0    /* system is unusable */
#define	APP_LOG_LEVEL_ALERT    1    /* action must be taken immediately */
#define	APP_LOG_LEVEL_CRIT     2    /* critical conditions */
#define	APP_LOG_LEVEL_ERR      3    /* error conditions */
#define	APP_LOG_LEVEL_WARN     4    /* warning conditions */
#define	APP_LOG_LEVEL_NOTICE   5    /* normal but significant condition */
#define	APP_LOG_LEVEL_INFO     6    /* informational */
#define	APP_LOG_LEVEL_DEBUG    7    /* debug-level messages */

#define MACRO_BLACK "\033[1;30;30m"
#define MACRO_RED "\033[1;30;31m"
#define MACRO_GREEN "\033[1;30;32m"
#define MACRO_YELLOW "\033[1;30;33m"
#define MACRO_BLUE "\033[1;30;34m"
#define MACRO_PURPLE "\033[1;30;35m"
#define MACRO_WHITE "\033[1;30;37m"
#define MACRO_END "\033[0m"

extern int ax_app_log_level;

#define APPLOG(level, fmt, ...) \
    do {  \
        if ((level) <= ax_app_log_level) { \
            switch(level) { \
            case APP_LOG_LEVEL_CRIT: \
                printf(MACRO_PURPLE "[%12llu][qsdemo] " fmt MACRO_END "\n",  GetTickCountPts(), ##__VA_ARGS__); \
                break; \
            case APP_LOG_LEVEL_ERR: \
                printf(MACRO_RED "[%12llu][qsdemo] " fmt MACRO_END "\n", GetTickCountPts(), ##__VA_ARGS__); \
                break; \
            case APP_LOG_LEVEL_WARN: \
                printf(MACRO_YELLOW "[%12llu][qsdemo] " fmt MACRO_END "\n", GetTickCountPts(), ##__VA_ARGS__); \
                break; \
            case APP_LOG_LEVEL_INFO: \
                printf(MACRO_GREEN "[%12llu][qsdemo] " fmt MACRO_END "\n",  GetTickCountPts(), ##__VA_ARGS__); \
                break; \
            default: \
                break; \
            } \
        } \
    } while (0)


// Used for printing at critical time points.
#define ALOGD(fmt, ...) APPLOG(APP_LOG_LEVEL_CRIT, fmt, ##__VA_ARGS__)
#define ALOGW(fmt, ...) APPLOG(APP_LOG_LEVEL_WARN, fmt, ##__VA_ARGS__)
#define ALOGI(fmt, ...) APPLOG(APP_LOG_LEVEL_INFO, fmt, ##__VA_ARGS__)
#define ALOGE(fmt, ...) APPLOG(APP_LOG_LEVEL_ERR, fmt, ##__VA_ARGS__)


void    LogInit();
void    LogDeInit();
AX_BOOL LogOpen();
AX_VOID LogToFile(const AX_CHAR *pFmt, ...);
AX_VOID LogClose();

#endif // _QSCOMMON_H__