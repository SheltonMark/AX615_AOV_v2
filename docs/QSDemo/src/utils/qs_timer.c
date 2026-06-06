/**************************************************************************************************
 *
 * Copyright (c) 2019-2025 Axera Semiconductor Co., Ltd. All Rights Reserved.
 *
 * This source file is the property of Axera Semiconductor Co., Ltd. and
 * may not be copied or distributed in any isomorphic form without the prior
 * written consent of Axera Semiconductor Co., Ltd.
 *
 **************************************************************************************************/

#include "qs_timer.h"
#include "qs_log.h"
#include <sys/ioctl.h>
#include <sys/mman.h>
#include <sys/time.h>
#include <time.h>
#include <fcntl.h>
#include "ax_sys_api.h"
#include "ax_gzipd_api.h"
#include "qs_global_type.h"

#define MMAP_FILE_NAME          "/dev/mem"
#define PAGE_SIZE               0x1000
#define TIMER_BASE              0x4820000
#define MAP_SIZE                0x100

AX_U64 g_u64RomCodeTime = 0;

AX_S32 wakeup_timer_set(AX_U32 wait_time)
{
    /*
    AX_CHAR command[256] = {0};
    AX_S32 nSize = 0;
    nSize = sprintf(command, "echo %u > /proc/ax_proc/power/wakeup_source/timer32/time", wait_time);
    command[nSize] = 0;
    system(command);
    */

    AX_CHAR str[64] = {0};
    AX_S32 nSize = 0;
    nSize = sprintf(str, "%u", wait_time);
    str[nSize] = 0;

    FILE * fp = fopen("/proc/ax_proc/power/wakeup_source/timer32/time", "w+");
    if (fp) {
        fwrite(str, 1, strlen(str),fp);
        fflush(fp);
        fclose(fp);
    } else {
        ALOGE("open /proc/ax_proc/power/wakeup_source/timer32/time failed, error=%d", errno);
        return -1;
    }

    // AX_VOID *base;
    // AX_S32 fd = open(MMAP_FILE_NAME, O_RDWR | O_SYNC);
    // if (fd == -1) {
    //     ALOGE("wakeup_timer_set: open %s failed", MMAP_FILE_NAME);
    //     return -1;
    // }
    // base = mmap(0, MAP_SIZE, PROT_READ | PROT_WRITE, MAP_SHARED, fd, 0x1000);

    // if (base == MAP_FAILED) {
    //     ALOGE("wakeup_timer_set: mmap failed");
    //     close(fd);
    //     return -1;
    // }

    // *(volatile unsigned int *)(base + 0xFC) = wait_time;

    // munmap(base, MAP_SIZE);
    // close(fd);

	return 0;
}

#ifdef AX_FAST_BOOT
static AX_U64 GetRomcodeTime(AX_VOID) {
    long timer_vaddr = 0;
    int fd = 0;
    AX_U64 romtime = 0;

    fd = open(MMAP_FILE_NAME, O_RDONLY | O_SYNC);
    if (-1 == fd) {
        return 0;
    }

#ifndef __RTT_OS__
    void *temp_vaddr = mmap(0, PAGE_SIZE, PROT_READ, MAP_SHARED, fd, 0);
#else
    void* temp_vaddr = NULL;
#endif
    if (MAP_FAILED == temp_vaddr) {
        close(fd);
        return 0;
    }
    timer_vaddr = (long)temp_vaddr;
    romtime = *(volatile unsigned int *)(timer_vaddr + 0x900);

#ifndef __RTT_OS__
    munmap((void*)timer_vaddr, PAGE_SIZE);
#endif
    close(fd);

    return romtime;
}
#endif

AX_U64 GetTickCountPts(AX_VOID) {
#ifdef AX_FAST_BOOT
    AX_U64 val = 0;
    void* timer_vaddr = 0;
    int fd = 0;
    AX_U64 romtime = 0;
    AX_U64 romcode_time = 0;

    if (g_u64RomCodeTime == 0) {
        g_u64RomCodeTime = GetRomcodeTime();
    }

    fd = open(MMAP_FILE_NAME, O_RDONLY | O_SYNC);
    if (-1 == fd) {
        return 0;
    }

#ifndef __RTT_OS__
    timer_vaddr = mmap(0, PAGE_SIZE, PROT_READ, MAP_SHARED, fd, TIMER_BASE);
#endif
    if (MAP_FAILED == timer_vaddr) {
        close(fd);
        return 0;
    }

    romtime = g_u64RomCodeTime;
    romcode_time = romtime * 1000 / 32;

    val = *(volatile unsigned long long *)(timer_vaddr + 0x0);
#ifndef __RTT_OS__
    munmap((void*)timer_vaddr, PAGE_SIZE);
#endif
    close(fd);

    return ((val - romtime)/24 + romcode_time);
#else
    //AX_SYS_GetCurPTS(&val);
    struct timespec ts = {0};
    clock_gettime(CLOCK_BOOTTIME, &ts);
    return (AX_U64)ts.tv_sec * 1000000 + (AX_U64)ts.tv_nsec / 1000;
#endif
}


AX_S32 ReadISPTimes(AX_U64 *u64Started, AX_U64 *u64Opended, AX_U64 *u64FrameReady, AX_U64 *u64OutToNext, AX_U64 *u64Fsof, AX_S32 nCamCnt) {
    long timer_vaddr = 0;
    int fd = 0;
    AX_U64 romtime = 0;
    AX_U64 romcode_time = 0;

    fd = open(MMAP_FILE_NAME, O_RDONLY | O_SYNC);
    if (-1 == fd) {
        return 0;
    }

#ifndef __RTT_OS__
    void *temp_vaddr = mmap(0, PAGE_SIZE, PROT_READ, MAP_SHARED, fd, 0);
#else
    void* temp_vaddr = NULL;
#endif
    if (MAP_FAILED == temp_vaddr) {
        close(fd);
        return 0;
    }
    timer_vaddr = (long)temp_vaddr;
    romtime = *(volatile unsigned int *)(timer_vaddr + 0x900);
    romcode_time = romtime * 1000 / 32;

    u64Started[0] = (*(volatile unsigned int *)(timer_vaddr + 0x940) - romtime)/24 + romcode_time;
    u64Opended[0] = (*(volatile unsigned int *)(timer_vaddr + 0x944) - romtime)/24 + romcode_time;
    u64FrameReady[0] = (*(volatile unsigned int *)(timer_vaddr + 0x94c) - romtime)/24 + romcode_time;
    u64OutToNext[0] = (*(volatile unsigned int *)(timer_vaddr + 0x950) - romtime)/24 + romcode_time;
    u64Fsof[0] =  (*(volatile unsigned int *)(timer_vaddr + 0x988) - romtime)/24 + romcode_time;

    if (nCamCnt == 2) {
        u64Started[1] = (*(volatile unsigned int *)(timer_vaddr + 0x978) - romtime)/24 + romcode_time;
        u64Opended[1] = (*(volatile unsigned int *)(timer_vaddr + 0x97c) - romtime)/24 + romcode_time;
        u64FrameReady[1] = (*(volatile unsigned int *)(timer_vaddr + 0x980) - romtime)/24 + romcode_time;
        u64OutToNext[1] = (*(volatile unsigned int *)(timer_vaddr + 0x984) - romtime)/24 + romcode_time;
        u64Fsof[1] =  (*(volatile unsigned int *)(timer_vaddr + 0x98C) - romtime)/24 + romcode_time;
    }
#ifndef __RTT_OS__
    munmap((void*)timer_vaddr, PAGE_SIZE);
#endif
    close(fd);

    return 1;
}

AX_U64 GetClockTime(AX_VOID) {
    struct timespec ts = {0};
    int nError = clock_gettime(CLOCK_MONOTONIC, &ts);

    if (0 == nError) {
        return (AX_U64)ts.tv_sec * 1000 + (AX_U64)ts.tv_nsec / 1000000;
    } else {
        ALOGE("Get clock time fail with code: %d.\n", errno);
        return 0;
    }
}
