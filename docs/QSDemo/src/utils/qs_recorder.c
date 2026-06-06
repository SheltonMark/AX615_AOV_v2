/**************************************************************************************************
 *
 * Copyright (c) 2019-2025 Axera Semiconductor Co., Ltd. All Rights Reserved.
 *
 * This source file is the property of Axera Semiconductor Co., Ltd. and
 * may not be copied or distributed in any isomorphic form without the prior
 * written consent of Axera Semiconductor Co., Ltd.
 *
 **************************************************************************************************/

#include <sys/statfs.h>
#include <sys/types.h>
#include <sys/select.h>
#include <sys/stat.h>
#include <stdlib.h>
#include <dirent.h>
#include <pthread.h>
#include <unistd.h>
#include "qs_recorder.h"
#include "qs_utils.h"
#include "AXRingFifo.h"
#include "ax_sys_api.h"
#include "ax_gzipd_api.h"
#include "audio_config.h"
#include "qsdemo.h"

#define MAX_SDCARD_RETRY_TIMES (20)
#define MAX_RECORD_SNS_COUNT   (2)
#define MAX_PATH               (256)

static const AX_S32 g_nMaxFileSize = 100*1024*1024;
static AX_S32 g_nMaxRecodFileCount = MAX_RECORD_FILE_COUNT;
static AX_BOOL g_bSdOnlineMode = AX_TRUE;//config

static pthread_mutex_t g_mutex_wakup_record[MAX_RECORD_SNS_COUNT] = {PTHREAD_MUTEX_INITIALIZER, PTHREAD_MUTEX_INITIALIZER};
static pthread_cond_t g_cond_wakup_record[MAX_RECORD_SNS_COUNT] = {PTHREAD_COND_INITIALIZER, PTHREAD_COND_INITIALIZER};
static AX_BOOL g_bWakeupRecord[MAX_RECORD_SNS_COUNT] = {AX_TRUE, AX_TRUE};

static pthread_mutex_t g_mutex_copy_result = PTHREAD_MUTEX_INITIALIZER;
static pthread_mutex_t g_mutex_sd_load = PTHREAD_MUTEX_INITIALIZER;

static AX_S32 g_nCamCount = 1;
AX_BOOL g_bWorking = AX_FALSE;
AX_BOOL g_bSdMonitorWorking = AX_FALSE;
static AX_BOOL g_bRecordInited = AX_FALSE;
static AX_CHAR g_szTFCardRoot[] = "/tfcard/qsdemo";

extern AX_S32 g_nRunningTimes;

pthread_t g_CopyResultTid = 0;
pthread_t g_MonitorSDTid = 0;

static AX_BOOL g_bSDCardReady = AX_FALSE;

typedef struct _RECODER_FILE_INFO {
    AX_CHAR  **szVideoFile;
    AX_CHAR  **szAudioFile;
    AX_CHAR* szBaseDir;
    AX_U32   nCurVideoFileSize;
    AX_U32   nCurAudioFileSize;
    AX_U32   nCurFileIdx;
    AX_S32   nCamID;
    AX_RINGFIFO_HANDLE hVideoFifo;
    AX_RINGFIFO_HANDLE hAudioFifo;
    pthread_t tid;
}RECODER_FILE_INFO_T;

RECODER_FILE_INFO_T  g_videoRecordFile[MAX_RECORD_SNS_COUNT];

int compare(const void* a, const void* b) {
    const AX_CHAR* const* lhs = (const AX_CHAR* const*)a;
    const AX_CHAR* const* rhs = (const AX_CHAR* const*)b;
    return strcmp(*lhs, *rhs);
}

AX_BOOL QS_MountSDCard() {
    FILE * fp;
#if defined(AX615_EMMC)
    fp = popen("if [ -e '/dev/mmcblk1p1' ]; then mkdir -p /tfcard; mount /dev/mmcblk1p1 /tfcard; fi","r");
#else
    fp = popen("if [ -e '/dev/mmcblk0p1' ]; then mkdir -p /tfcard; mount /dev/mmcblk0p1 /tfcard; fi","r");
#endif

    if(fp == NULL){
        return AX_FALSE;
    }
    pclose(fp);
    return AX_TRUE;
}

AX_BOOL QS_CheckSDMounted() {
    // /dev/mmcblk1p1 /tfcard vfat rw,relatime,fmask=0022,dmask=0022,codepage=437,iocharset=iso8859-1,shortname=mixed,errors=remount-ro 0 0
    FILE * fp;
    fd_set read_fds;
    struct timeval timeout;
    int fd = 0;
    int ret = 0;

    AX_CHAR buf[256];
    memset(buf, 0, sizeof(buf));

#if defined(AX615_EMMC)
    fp = popen("cat /proc/mounts | grep '/dev/mmcblk1p1 '","r");
#else
    fp = popen("cat /proc/mounts | grep '/dev/mmcblk0p1 '","r");
#endif

    if(fp == NULL){
        return AX_FALSE;
    }

    fd = fileno(fp);
    FD_ZERO(&read_fds);
    FD_SET(fd, &read_fds);

    timeout.tv_sec = 1;
    timeout.tv_usec = 0;

    ret = select(fd + 1, &read_fds, NULL, NULL, &timeout);
    if (ret > 0 && FD_ISSET(fd, &read_fds)) {
        AX_S32 len = fread(buf, 1, 255, fp);
        if (len <= 14) {
            pclose(fp);
            return AX_FALSE;
        }
        pclose(fp);
        return AX_TRUE;
    } else {
        pclose(fp);
        return AX_FALSE;
    }
}

static AX_VOID *monitor_sd_thread_func(AX_VOID *param) {
    QS_SetThreadName("qs_sdmonitor");
    AX_S32  nLastStatus = -1;
    AX_S32  nCheckRetry = 0;
    struct stat buffer;

    while (g_bSdMonitorWorking) {
        AX_S32 nDevExist = 0;

#if defined(AX615_EMMC)
        nDevExist = stat("/dev/mmcblk1p1", &buffer) == 0 ? 1 : 0;
#else
        nDevExist = stat("/dev/mmcblk0p1", &buffer) == 0 ? 1 : 0;
#endif
        if (nLastStatus != nDevExist) {
            if (nDevExist) {
                if (!QS_CheckSDMounted()) {
                    // mount
                    QS_MountSDCard();
                }

                if (QS_CheckSDMounted()) {
                    g_bSDCardReady = AX_TRUE;

                    ALOGI("sd card mounted");
                } else {
                    ALOGI("sd card mounted failed");
                }
            } else {
                g_bSDCardReady = AX_FALSE;

                if (nLastStatus == -1) {
                    // no sd card
                } else {
                    // umount
                    QS_RunCmd("umount -l /tfcard", NULL, 0);

                    ALOGI("sd card removed");
                }
            }
            nLastStatus = nDevExist;

            if (nDevExist != (AX_S32)QS_CheckSDMounted()) {
                nCheckRetry = MAX_SDCARD_RETRY_TIMES;
            } else {
                nCheckRetry = 0;
            }
        } else if (nCheckRetry > 0) {
            ALOGI("sd card retry(%d)", MAX_SDCARD_RETRY_TIMES - nCheckRetry + 1);

            if (nDevExist && !QS_CheckSDMounted()) {
                // mount
                QS_MountSDCard();

                if (QS_CheckSDMounted()) {
                    g_bSDCardReady = AX_TRUE;

                    ALOGI("sd card mounted");
                }
            } else if (!nDevExist && QS_CheckSDMounted()) {
                // umount
                QS_RunCmd("umount -l /tfcard", NULL, 0);

                ALOGI("sd card removed");
            }

            if (nDevExist == (AX_S32)QS_CheckSDMounted()) {
                nCheckRetry = 0;
            } else {
                nCheckRetry --;
            }
        }

        MSSleep(200);
    }

    return NULL;
}

AX_S32 QS_MonitorSDCardStart() {
    if (g_bSdOnlineMode) {
        if (g_MonitorSDTid == 0) {
            g_bSdMonitorWorking = AX_TRUE;

            pthread_create(&g_MonitorSDTid, NULL, monitor_sd_thread_func, NULL);
            if (g_MonitorSDTid != 0) {
                return 0;
            } else {
                return -1;
            }
        }
    } else {
        QS_InstallSdDevice();
    }
    return 0;
}

AX_S32  QS_MonitorSDCardStop() {
    ALOGI("QS_MonitorSDCardStop ++");
    g_bSdMonitorWorking = AX_FALSE;

    if (g_MonitorSDTid != 0) {
        pthread_join(g_MonitorSDTid, NULL);
        g_MonitorSDTid = 0;
    }
    ALOGI("QS_MonitorSDCardStop --");
    return 0;
}

AX_BOOL QS_IsSDCardReady() {
    return g_bSDCardReady;
}

AX_BOOL QS_CheckDisk(AX_U32 nMinLeftSize) {
#ifndef __RTT_OS__
    struct statfs diskInfo;
    if (statfs("/tfcard/qsdemo", &diskInfo) == -1)  {
        return AX_TRUE;
    }
    AX_U64 blocksize = diskInfo.f_bsize;
    AX_U64 availableDisk = diskInfo.f_bavail * blocksize;
    AX_U64 freeDisk = diskInfo.f_bfree * blocksize;
    AX_U64 totalDisk = diskInfo.f_blocks * blocksize;
    ALOGI("QS_CheckDisk: current available: %llu MB, free: %llu MB, totoal: %llu MB", availableDisk>>20, freeDisk>>20, totalDisk>>20);
    if (availableDisk < nMinLeftSize) {
        return AX_FALSE;
    }
#endif
    return AX_TRUE;
}

AX_BOOL QS_CheckAndSweepDisk(AX_S32 nCamId) {
    AX_CHAR command[256] = {0};
    AX_S32 i = 0;
    AX_S32 idx = 0;
    AX_S32 nSize = 0;
    AX_S32 nCurIdx = g_videoRecordFile[nCamId].nCurFileIdx;
    if (nCurIdx > g_nMaxRecodFileCount ) {
        i = nCurIdx - g_nMaxRecodFileCount - 1;
    }

    AX_U32 nMinLeftSize = g_nMaxFileSize*MAX_RECORD_SNS_COUNT;

    do {
        if (QS_CheckDisk(nMinLeftSize)) {
            return AX_TRUE;
        }

        for (; i <= nCurIdx; i++) {
            AX_BOOL bSwept = AX_FALSE;
            idx = i % g_nMaxRecodFileCount;
            if (strlen(g_videoRecordFile[nCamId].szVideoFile[idx])) {
                nSize = sprintf(command, "rm -f %s", g_videoRecordFile[nCamId].szVideoFile[idx]);
                command[nSize] = 0;
                QS_RunCmd(command, NULL, 0);
                g_videoRecordFile[nCamId].szVideoFile[idx][0] = 0;
                bSwept = AX_TRUE;
            }
#ifdef QSDEMO_AUDIO_SUPPORT
            if (strlen(g_videoRecordFile[nCamId].szAudioFile[idx])) {
                nSize = sprintf(command, "rm -f %s", g_videoRecordFile[nCamId].szAudioFile[idx]);
                command[nSize] = 0;
                QS_RunCmd(command, NULL, 0);
                g_videoRecordFile[nCamId].szAudioFile[idx][0] = 0;
                bSwept = AX_TRUE;
            }
#endif

            if (bSwept) {
                break;
            }
        }
        usleep(10000);
    } while(g_bWorking);

    return AX_FALSE;
}

AX_BOOL QS_SweepDiskQs(AX_S32 nRunTimes) {
    AX_CHAR command[300] = {0};
    AX_U32 nMinLeftSize = 100*1024*1024;
    AX_S32 nCount = 0;

    if (QS_CheckDisk(nMinLeftSize)) {
        return AX_TRUE;
    }

    ALOGI("QS_SweepDiskQs: clear result folder ++");
    nCount = sprintf(command, "rm -rf /tfcard/qsdemo/qs");
    if (nCount > 0 && nCount < 300) {
        command[nCount] = 0;
        QS_RunCmd(command, NULL, 0);
    }
    ALOGI("QS_SweepDiskQs: clear result folder --");

    return AX_TRUE;
}

AX_BOOL QS_ListFile(AX_S32 nCamIdx) {
    DIR *dp;
    struct dirent *dirp;
    AX_CHAR command[300] = {0};
    AX_S32 nSize = 0;
    AX_S32 i = 0;

    g_videoRecordFile[nCamIdx].nCurFileIdx = 0;
    g_videoRecordFile[nCamIdx].nCurVideoFileSize = 0;
    g_videoRecordFile[nCamIdx].nCurFileIdx = 0;
    for (i = 0; i < g_nMaxRecodFileCount; i++) {
        memset(g_videoRecordFile[nCamIdx].szVideoFile[i], 0, MAX_PATH * sizeof(char));
        memset(g_videoRecordFile[nCamIdx].szAudioFile[i], 0, MAX_PATH * sizeof(char));
    }

    if ((dp = opendir(g_videoRecordFile[nCamIdx].szBaseDir)) == NULL) {
        ALOGE("can't open %s", g_videoRecordFile[nCamIdx].szBaseDir);
        return -1;
    }
    AX_U32 nCurVideoFileIdx = 0;
#ifdef QSDEMO_AUDIO_SUPPORT
    AX_U32  nCurAudioFileIdx = 0;
#endif
    while((dirp = readdir(dp)) != NULL) {
        if (dirp->d_type == 8) {
            if (strstr(dirp->d_name, ".264") || strstr(dirp->d_name, ".265")) {
                if (nCurVideoFileIdx < g_nMaxRecodFileCount) {
                    sprintf(g_videoRecordFile[nCamIdx].szVideoFile[nCurVideoFileIdx], "%s/%s",g_videoRecordFile[nCamIdx].szBaseDir, dirp->d_name);
                    nCurVideoFileIdx ++;
                } else{
                    // delete video file
                    nSize = sprintf(command, "rm -f %s/%s", g_videoRecordFile[nCamIdx].szBaseDir, dirp->d_name);
                    command[nSize] = 0;
                    QS_RunCmd(command, NULL, 0);
                }
            } else if (strstr(dirp->d_name, ".aac") || strstr(dirp->d_name, ".g711") || strstr(dirp->d_name, ".pcm")) {
#ifdef QSDEMO_AUDIO_SUPPORT
                if (nCurAudioFileIdx < g_nMaxRecodFileCount) {
                    sprintf(g_videoRecordFile[nCamIdx].szAudioFile[nCurAudioFileIdx], "%s/%s",g_videoRecordFile[nCamIdx].szBaseDir, dirp->d_name);
                    nCurAudioFileIdx ++;
                } else{
                    // delete audio file
                    nSize = sprintf(command, "rm -f %s/%s", g_videoRecordFile[nCamIdx].szBaseDir, dirp->d_name);
                    command[nSize] = 0;
                    QS_RunCmd(command, NULL, 0);
                }
#endif
            }
        }
    }
    closedir(dp);

#ifdef QSDEMO_AUDIO_SUPPORT
    QS_SAMPLE_AUDIO_ENCODER_INFO_T stEncoderInfo;
    AX_S32 s32Ret = QS_COMMON_AUDIO_GetEncoderInfo(&stEncoderInfo);

    if (!s32Ret
        && stEncoderInfo.bEnable) {
        // video & audio file not equal, remove all file
        if (nCurVideoFileIdx != nCurAudioFileIdx) {
            ALOGD("sns[%d] delete all files, as video file[%d] != audio file[%d]", nCamIdx, nCurVideoFileIdx, nCurAudioFileIdx);

            AX_S32 nSize = sprintf(command, "rm %s/* -rf", g_videoRecordFile[nCamIdx].szBaseDir);
            command[nSize] = 0;
            QS_RunCmd(command, NULL, 0);
            QS_RunCmd("sync", NULL, 0);

            nCurVideoFileIdx = 0;
            nCurAudioFileIdx = 0;

            for (i = 0; i < g_nMaxRecodFileCount; i++) {
                memset(g_videoRecordFile[nCamIdx].szVideoFile[i], 0, MAX_PATH * sizeof(char));
                memset(g_videoRecordFile[nCamIdx].szAudioFile[i], 0, MAX_PATH * sizeof(char));
            }
        }
    } else if (nCurAudioFileIdx != 0) {
        ALOGD("sns[%d] delete all audio files, as audio disabled", nCamIdx);

        // remove audio file
        AX_S32 nSize = sprintf(command, "rm %s/*.aac -rf", g_videoRecordFile[nCamIdx].szBaseDir);
        command[nSize] = 0;
        QS_RunCmd(command, NULL, 0);
        nSize = sprintf(command, "rm %s/*.g711* -rf", g_videoRecordFile[nCamIdx].szBaseDir);
        command[nSize] = 0;
        QS_RunCmd(command, NULL, 0);
        nSize = sprintf(command, "rm %s/*.pcm -rf", g_videoRecordFile[nCamIdx].szBaseDir);
        command[nSize] = 0;
        QS_RunCmd(command, NULL, 0);
        QS_RunCmd("sync", NULL, 0);

        nCurAudioFileIdx = 0;
        for (i = 0; i < g_nMaxRecodFileCount; i++) {
            memset(g_videoRecordFile[nCamIdx].szAudioFile[i], 0, MAX_PATH * sizeof(char));
        }
    }
#endif

    if (nCurVideoFileIdx > 0) {
        qsort(g_videoRecordFile[nCamIdx].szVideoFile, nCurVideoFileIdx, sizeof(char*), compare);

        struct stat st = {0};
        AX_U32 nLastVideoFileIdx = nCurVideoFileIdx - 1;
        if (stat(g_videoRecordFile[nCamIdx].szVideoFile[nLastVideoFileIdx], &st) == 0) {
            g_videoRecordFile[nCamIdx].nCurVideoFileSize = st.st_size;
        }
    }

    g_videoRecordFile[nCamIdx].nCurFileIdx = nCurVideoFileIdx;

#ifdef QSDEMO_AUDIO_SUPPORT
    if (nCurAudioFileIdx > 0) {
        qsort(g_videoRecordFile[nCamIdx].szAudioFile, nCurAudioFileIdx, sizeof(char*), compare);
    }
#endif

    return 0;
}

static AX_U32 QS_GetFileName(AX_S32 nCamIdx) {
    AX_CHAR command[256] = {0};
    AX_S32 nSize = 0;
    AX_U32 nIdx = g_videoRecordFile[nCamIdx].nCurFileIdx % g_nMaxRecodFileCount;
    time_t t;
    struct tm tm;

    t = time(NULL);
    localtime_r(&t, &tm);

    if (strlen(g_videoRecordFile[nCamIdx].szVideoFile[nIdx]) != 0) {
        nSize = sprintf(command, "rm -f %s", g_videoRecordFile[nCamIdx].szVideoFile[nIdx]);
        command[nSize] = 0;
        QS_RunCmd(command, NULL, 0);
        ALOGI("sns[%d] delete video file: %s", nCamIdx, g_videoRecordFile[nCamIdx].szVideoFile[nIdx]);
    }

    g_videoRecordFile[nCamIdx].nCurVideoFileSize = 0;

    nSize = sprintf(g_videoRecordFile[nCamIdx].szVideoFile[nIdx], "%s/%04d-%02d-%02d-%02d%02d%02d.264",
            g_videoRecordFile[nCamIdx].szBaseDir,
            tm.tm_year + 1900, tm.tm_mon + 1, tm.tm_mday, tm.tm_hour, tm.tm_min, tm.tm_sec);

    g_videoRecordFile[nCamIdx].szVideoFile[nIdx][nSize] = 0;

#ifdef QSDEMO_AUDIO_SUPPORT
    if (strlen(g_videoRecordFile[nCamIdx].szAudioFile[nIdx]) != 0) {
        nSize = sprintf(command, "rm -f %s", g_videoRecordFile[nCamIdx].szAudioFile[nIdx]);
        command[nSize] = 0;
        QS_RunCmd(command, NULL, 0);
        ALOGI("sns[%d] delete audio file: %s", nCamIdx, g_videoRecordFile[nCamIdx].szAudioFile[nIdx]);
    }

    g_videoRecordFile[nCamIdx].nCurAudioFileSize = 0;

    const AX_CHAR* audio_ext = NULL;
    QS_SAMPLE_AUDIO_ENCODER_INFO_T stEncoderInfo;
    AX_S32 s32Ret = QS_COMMON_AUDIO_GetEncoderInfo(&stEncoderInfo);

    if (!s32Ret
        && stEncoderInfo.bEnable) {
        if (stEncoderInfo.ePt == PT_G711A) {
            audio_ext = "g711a";
        } else if (stEncoderInfo.ePt == PT_G711U) {
            audio_ext = "g711u";
        } else if (stEncoderInfo.ePt == PT_AAC) {
            audio_ext = "aac";
        } else {
            audio_ext = "pcm";
        }

        nSize = sprintf(g_videoRecordFile[nCamIdx].szAudioFile[nIdx], "%s/%04d-%02d-%02d-%02d%02d%02d.%s",
                g_videoRecordFile[nCamIdx].szBaseDir,
                tm.tm_year + 1900, tm.tm_mon + 1, tm.tm_mday, tm.tm_hour, tm.tm_min, tm.tm_sec, audio_ext);

        g_videoRecordFile[nCamIdx].szAudioFile[nIdx][nSize] = 0;
    } else {
        memset(g_videoRecordFile[nCamIdx].szAudioFile[nIdx], 0x00, MAX_PATH * sizeof(char));
    }
#endif

    return nIdx;
}

AX_S32 QS_InstallSdDevice() {
    AX_CHAR szResult[64] = {0};
    AX_S32 nRet = 0;

    // insmod
    nRet = QS_RunCmd("lsmod | grep sdhci_axera", szResult, 64);
    if (nRet == 0 && strlen(szResult) > 0) {
        // ALOGI("sd device had insmod");
    } else {
        QS_RunCmd("insmod /soc/ko/sdhci-axera.ko > /dev/null", NULL, 0);
        QS_RunCmd("sync", NULL, 0);
        // ALOGI("sd device insmod");
    }

    // mount
    memset(szResult, 0, sizeof(szResult));
    nRet = QS_RunCmd("mount | grep \"/tfcard\" | awk '{print $3}'", szResult, 64);
    if (nRet != 0 || strlen(szResult) <= 0) {
        QS_MountSDCard();
        // ALOGI("sd card mount");
    } else {
        // ALOGI("sd card had mounted");
    }

    return nRet;
}

AX_S32 QS_RemoveSdDevice(FILE **f_video, FILE **f_audio) {

    if (f_video && *f_video) {
        fclose(*f_video);
        *f_video = NULL;
    }

    if (f_audio && *f_audio) {
        fclose(*f_audio);
        *f_audio = NULL;
    }

    AX_CHAR szResult[64] = {0};
    AX_S32 nRet = 0;

    nRet = QS_RunCmd("mount | grep \"/tfcard\" | awk '{print $3}'", szResult, 64);
    if (nRet == 0 && strlen(szResult) > 0) {
        QS_RunCmd("umount -l /tfcard", NULL, 0);
        QS_RunCmd("sync", NULL, 0);
        // ALOGI("sd card umount");
    } else {
        // ALOGI("sd card had umount");
    }

    QS_RunCmd("rmmod sdhci-axera.ko > /dev/null", NULL, 0);
    QS_RunCmd("sync", NULL, 0);
    ALOGI("sd device umount and removed");
    return 0;
}

AX_S32 QS_loadSdFile(RECODER_FILE_INFO_T *pRecFileInfo, FILE **f_video, FILE **f_audio, AX_U32 sdResetFlag, AX_S32 timeoutMs) {
    AX_U64 u64InPts = GetTickCountPts();
    AX_U64 u64CurPts = u64InPts;
    do {
        do {
            if (g_bSdOnlineMode) {
                if (QS_IsSDCardReady()) {
                    break;
                }
            } else {
                AX_CHAR szResult[64] = {0};
                AX_S32 nRet = 0;

                nRet = QS_RunCmd("mount | grep \"/tfcard\" | awk '{print $3}'", szResult, 64);
                if (nRet == 0 && strlen(szResult) > 0) {
                    break;
                } else {
                    QS_InstallSdDevice();
                }
            }
            u64CurPts = GetTickCountPts();
            if (timeoutMs > 0 && (u64CurPts - u64InPts >= (AX_U64)timeoutMs * 1000)) {
                return -1;
            }
            MSSleep(50);
        } while (g_bWorking);

        if (!g_bWorking) {
            return -1;
        }

        ALOGI("sd device installed and mounted");
        // check timeout
        u64CurPts = GetTickCountPts();
        if (timeoutMs > 0 && (u64CurPts - u64InPts >= (AX_U64)timeoutMs * 1000)) {
            return -1;
        }
        // first in or other reason rm base dir, go to mkdir and rm all files
        if(sdResetFlag == 1 || access(g_videoRecordFile[pRecFileInfo->nCamID].szBaseDir, F_OK) != 0) {
            __sync_synchronize();
            if (access(g_videoRecordFile[pRecFileInfo->nCamID].szBaseDir, F_OK) != 0) {
                AX_CHAR command[256] = {0};
                // create dir
                AX_S32 nSize = sprintf(command, "mkdir -p %s", g_videoRecordFile[pRecFileInfo->nCamID].szBaseDir);
                command[nSize] = 0;

                QS_RunCmd(command, NULL, 0);
                QS_RunCmd("sync", NULL, 0);
                if (access(g_videoRecordFile[pRecFileInfo->nCamID].szBaseDir, F_OK) == 0) {
                    ALOGI("sns[%d] create dir %s success",pRecFileInfo->nCamID, g_videoRecordFile[pRecFileInfo->nCamID].szBaseDir);
                } else {
                    MSSleep(500);
                    __sync_synchronize();
                    continue;
                }
            }

            __sync_synchronize();
            if (!g_bWorking) {
                return -1;
            }

            // read max list files and rm all other files
            QS_ListFile(pRecFileInfo->nCamID);

            // if minleft, rm all exist file(TODO:SD card 64G cost 9 seconds)
            // QS_CheckAndSweepDisk(pRecFileInfo->nCamID);

            AX_U32 nIdx = QS_GetFileName(pRecFileInfo->nCamID);
            *f_video = fopen(g_videoRecordFile[pRecFileInfo->nCamID].szVideoFile[nIdx], "wb");
            ALOGI("sns[%d] create video file: %s, ret=%d", pRecFileInfo->nCamID, g_videoRecordFile[pRecFileInfo->nCamID].szVideoFile[nIdx], errno);

            if (!(*f_video)) {
                MSSleep(500);
                continue;
            }

        #ifdef QSDEMO_AUDIO_SUPPORT
            if (strlen(g_videoRecordFile[pRecFileInfo->nCamID].szAudioFile[nIdx]) != 0) {
                *f_audio = fopen(g_videoRecordFile[pRecFileInfo->nCamID].szAudioFile[nIdx], "wb");
                ALOGI("sns[%d] create audio file: %s, ret=%d", pRecFileInfo->nCamID, g_videoRecordFile[pRecFileInfo->nCamID].szAudioFile[nIdx], errno);

                if (!(*f_audio)) {
                    fclose(*f_video);
                    *f_video = NULL;
                    MSSleep(500);
                    continue;
                }
            }
        #endif
        } else { // app rmmode
            AX_U32 nIdx = 0;
            if (g_videoRecordFile[pRecFileInfo->nCamID].nCurFileIdx > 0) {
                nIdx = (g_videoRecordFile[pRecFileInfo->nCamID].nCurFileIdx - 1) % g_nMaxRecodFileCount;
            }

            // read dir file list compare g_videoRecordFile list
            if (strlen(g_videoRecordFile[pRecFileInfo->nCamID].szVideoFile[nIdx]) != 0) {
                if (access(g_videoRecordFile[pRecFileInfo->nCamID].szVideoFile[nIdx], F_OK) == 0 && *f_video == NULL) {
                    *f_video = fopen(g_videoRecordFile[pRecFileInfo->nCamID].szVideoFile[nIdx], "ab");

                    if (!(*f_video)) {
                        return -1;
                    }

                    if (g_videoRecordFile[pRecFileInfo->nCamID].nCurVideoFileSize == 0) {
                        struct stat st = {0};
                        if (stat(g_videoRecordFile[pRecFileInfo->nCamID].szVideoFile[nIdx], &st) == 0) {
                            g_videoRecordFile[pRecFileInfo->nCamID].nCurVideoFileSize = st.st_size;
                        }
                    }
                } else {
                    //TODO: no exist file, update list file or next create file?
                }
            } else {
                ALOGI("sns[%d] video file name is NULL", pRecFileInfo->nCamID);
                return -1;
            }
        #ifdef QSDEMO_AUDIO_SUPPORT
            if (strlen(g_videoRecordFile[pRecFileInfo->nCamID].szAudioFile[nIdx]) != 0) {
                if (access(g_videoRecordFile[pRecFileInfo->nCamID].szAudioFile[nIdx], F_OK) == 0 && *f_audio == NULL) {
                    *f_audio = fopen(g_videoRecordFile[pRecFileInfo->nCamID].szAudioFile[nIdx], "ab");
                    // ALOGI("sns[%d] open audio file: %s, ret=%d", pRecFileInfo->nCamID, g_videoRecordFile[pRecFileInfo->nCamID].szAudioFile[nIdx], errno);

                    if (!(*f_audio)) {
                        fclose(*f_video);
                        *f_video = NULL;
                        return -1;
                    }
                } else {
                    //TODO: no exist file, update list file or next create file?
                }
            } else {
                QS_SAMPLE_AUDIO_ENCODER_INFO_T stEncoderInfo;

                AX_S32 s32Ret = QS_COMMON_AUDIO_GetEncoderInfo(&stEncoderInfo);
                if (!s32Ret && stEncoderInfo.bEnable) {
                    ALOGI("sns[%d] audio file name is NULL", pRecFileInfo->nCamID);
                    return -1;
                }
            }
        #endif
        }
    } while(0);

    return 0;
}
static AX_VOID *record_thread_func(AX_VOID *param) {

    RECODER_FILE_INFO_T * pRecFileInfo = (RECODER_FILE_INFO_T *)param;
    AX_RINGFIFO_ELEMENT_T di;
    FILE *f_video = NULL;
    FILE *f_audio = NULL;
    AX_U32 len = 0;
    AX_U32 wlen = 0;
    AX_CHAR name[64] = {0};
    sprintf(name, "qs_record%d", pRecFileInfo->nCamID);
    AX_U32 aFinishFlag = 0;

    QS_SetThreadName(name);
wait_sd:
    //before here, make sure sd ko ready
    if (QS_loadSdFile(pRecFileInfo, &f_video, &f_audio, 1, -1) == -1) {
        return NULL;
    }
#define CHECK_SDCARD_READY() do { \
            if (!QS_IsSDCardReady()) { \
                update_sleep_condition(pRecFileInfo->nCamID == 0 ? CONDITION_MASK_RECORD0 : CONDITION_MASK_RECORD1, true);\
                goto wait_sd; \
            } \
        } while(0)

    if (!g_bSdOnlineMode) {
        pthread_mutex_lock(&g_mutex_sd_load);
        QS_RemoveSdDevice(&f_video, &f_audio);
        pthread_mutex_unlock(&g_mutex_sd_load);
    }
    while (g_bWorking || AX_RingFifo_Size(pRecFileInfo->hVideoFifo) > 0 || AX_RingFifo_Size(pRecFileInfo->hAudioFifo) > 0) {
        if (g_bWorking
            && AX_RingFifo_Size(pRecFileInfo->hVideoFifo) < (AX_RingFifo_Capacity(pRecFileInfo->hVideoFifo) * 2 / 3)
            && AX_RingFifo_Size(pRecFileInfo->hAudioFifo) < (AX_RingFifo_Capacity(pRecFileInfo->hAudioFifo) * 2 / 3)) {

            usleep(100000);
            continue;
        }
        if (g_bSdOnlineMode) {
            CHECK_SDCARD_READY();
        }
        //wait for wakeup
        pthread_mutex_lock(&g_mutex_wakup_record[pRecFileInfo->nCamID]);
        while (!g_bWakeupRecord[pRecFileInfo->nCamID]) {
            pthread_cond_wait(&g_cond_wakup_record[pRecFileInfo->nCamID], &g_mutex_wakup_record[pRecFileInfo->nCamID]);
        }
        pthread_mutex_unlock(&g_mutex_wakup_record[pRecFileInfo->nCamID]);
        update_sleep_condition(pRecFileInfo->nCamID == 0 ? CONDITION_MASK_RECORD0 : CONDITION_MASK_RECORD1, false);

        ALOGI("sns[%d] write record to sd +++", pRecFileInfo->nCamID);
        // mount sd card and check mount ok, TODO: maybe cost much time
        static AX_U64 u64SdInPts = 0;
        u64SdInPts = GetTickCountPts();
        if (!g_bSdOnlineMode) {
            pthread_mutex_lock(&g_mutex_sd_load);
            QS_InstallSdDevice();

            if(0 != QS_loadSdFile(pRecFileInfo, &f_video, &f_audio, 0, 1000)) {
                update_sleep_condition(pRecFileInfo->nCamID == 0 ? CONDITION_MASK_RECORD0 : CONDITION_MASK_RECORD1, true);
                ALOGE("sns[%d] sd insmod and reload timeout %d", pRecFileInfo->nCamID, 1000);

                pthread_mutex_unlock(&g_mutex_sd_load);
                ALOGI("sns[%d] write record to sd ---", pRecFileInfo->nCamID);
                goto wait_sd;
            }
        }

        aFinishFlag = 1;
        do {
#ifdef QSDEMO_AUDIO_SUPPORT
            memset(&di, 0, sizeof(di));
            if (AX_RingFifo_Size(pRecFileInfo->hAudioFifo) > 0
                && AX_RingFifo_Get(pRecFileInfo->hAudioFifo, &di) == 0) {
                len = di.data[0].len + di.data[1].len;
                wlen = 0;

                if (f_audio) {
                    wlen = fwrite(di.data[0].buf, 1, di.data[0].len, f_audio);
                    if (di.data[1].len) {
                        wlen += fwrite(di.data[1].buf, 1, di.data[1].len, f_audio);
                    }
                    if (wlen != len) {
                        ALOGE("Audio sns[%d] write data to file failed, %u -> %u", pRecFileInfo->nCamID, len, wlen);
                        {
                            fflush(f_audio);
                            fsync(fileno(f_audio));
                            fclose(f_audio);
                            f_audio = NULL;
                            pRecFileInfo->nCurAudioFileSize += wlen;
                            AX_RingFifo_Pop(pRecFileInfo->hAudioFifo);

                            if (f_video) {
                                fflush(f_video);
                                fsync(fileno(f_video));
                                fclose(f_video);
                                f_video = NULL;
                            }
                            ALOGI("sns[%d] write record to sd ---", pRecFileInfo->nCamID);
                            update_sleep_condition(pRecFileInfo->nCamID == 0 ? CONDITION_MASK_RECORD0 : CONDITION_MASK_RECORD1, true);

                            if (!g_bSdOnlineMode) {
                                pthread_mutex_unlock(&g_mutex_sd_load);
                            }
                            goto wait_sd;
                        }
                    }
                }

                pRecFileInfo->nCurAudioFileSize += wlen;
                AX_RingFifo_Pop(pRecFileInfo->hAudioFifo);
                aFinishFlag = 0;
            } else {
                aFinishFlag = 1;
            }
#endif

            memset(&di, 0, sizeof(di));
            if (AX_RingFifo_Get(pRecFileInfo->hVideoFifo, &di) == 0) {
                // ALOGI("[rec%d][get] pts=%llu, ifrm=%d", pRecFileInfo->nCamID, di.nPts, di.bIFrame);

                len = di.data[0].len + di.data[1].len;
                wlen = 0;
                if (di.bIFrame && (pRecFileInfo->nCurVideoFileSize + len) >= g_nMaxFileSize) {
                    if (f_video) {
                        fflush(f_video);
                        fsync(fileno(f_video));
                        fclose(f_video);
                        f_video = NULL;
                    }

#ifdef QSDEMO_AUDIO_SUPPORT
                    if (f_audio) {
                        fflush(f_audio);
                        fsync(fileno(f_audio));
                        fclose(f_audio);
                        f_audio = NULL;
                    }
#endif
                    g_videoRecordFile[pRecFileInfo->nCamID].nCurFileIdx ++;

                    //TODO:statfs SD card 64G cost 9 seconds
                    // QS_CheckAndSweepDisk(pRecFileInfo->nCamID);

                    AX_U32 nIdx = QS_GetFileName(pRecFileInfo->nCamID);
                    f_video = fopen(g_videoRecordFile[pRecFileInfo->nCamID].szVideoFile[nIdx], "wb");
                    ALOGI("sns[%d] create video file: %s", pRecFileInfo->nCamID, g_videoRecordFile[pRecFileInfo->nCamID].szVideoFile[nIdx]);

                    if (!f_video) {
                        ALOGI("sns[%d] write record to sd ---", pRecFileInfo->nCamID);
                        update_sleep_condition(pRecFileInfo->nCamID == 0 ? CONDITION_MASK_RECORD0 : CONDITION_MASK_RECORD1, true);

                        if (!g_bSdOnlineMode) {
                            pthread_mutex_unlock(&g_mutex_sd_load);
                        }
                        goto wait_sd;
                    }

#ifdef QSDEMO_AUDIO_SUPPORT
                    if (strlen(g_videoRecordFile[pRecFileInfo->nCamID].szAudioFile[nIdx]) != 0) {
                        f_audio = fopen(g_videoRecordFile[pRecFileInfo->nCamID].szAudioFile[nIdx], "wb");
                        ALOGI("sns[%d] create audio file: %s", pRecFileInfo->nCamID, g_videoRecordFile[pRecFileInfo->nCamID].szAudioFile[nIdx]);

                        if (!f_audio) {
                            fclose(f_video);
                            f_video = NULL;
                            ALOGI("sns[%d] write record to sd ---", pRecFileInfo->nCamID);
                            update_sleep_condition(pRecFileInfo->nCamID == 0 ? CONDITION_MASK_RECORD0 : CONDITION_MASK_RECORD1, true);

                            if (!g_bSdOnlineMode) {
                                pthread_mutex_unlock(&g_mutex_sd_load);
                            }
                            goto wait_sd;
                        }
                    }
#endif
                }

                if (pRecFileInfo->nCurVideoFileSize == 0 && !di.bIFrame) {
                    AX_RingFifo_Pop(pRecFileInfo->hVideoFifo);
                    continue;
                }

                if (f_video) {
                    wlen = fwrite(di.data[0].buf, 1, di.data[0].len, f_video);
                    if (di.data[1].len) {
                        wlen += fwrite(di.data[1].buf, 1, di.data[1].len, f_video);
                    }
                    if (wlen != len) {
                        ALOGE("Video sns[%d] write data to file failed, %u -> %u", pRecFileInfo->nCamID, len, wlen);
                        {
                            fflush(f_video);
                            fsync(fileno(f_video));
                            fclose(f_video);
                            f_video = NULL;
                            pRecFileInfo->nCurVideoFileSize += wlen;
                            AX_RingFifo_Pop(pRecFileInfo->hVideoFifo);
#ifdef QSDEMO_AUDIO_SUPPORT
                            if (f_audio) {
                                fflush(f_audio);
                                fsync(fileno(f_audio));
                                fclose(f_audio);
                                f_audio = NULL;
                            }
#endif
                            ALOGI("sns[%d] write record to sd ---", pRecFileInfo->nCamID);
                            update_sleep_condition(pRecFileInfo->nCamID == 0 ? CONDITION_MASK_RECORD0 : CONDITION_MASK_RECORD1, true);

                            if (!g_bSdOnlineMode) {
                                pthread_mutex_unlock(&g_mutex_sd_load);
                            }
                            goto wait_sd;
                        }
                    }
                }

                pRecFileInfo->nCurVideoFileSize += wlen;
                AX_RingFifo_Pop(pRecFileInfo->hVideoFifo);
            } else {
                if (aFinishFlag) {
                    break;
                } else {
                    continue;
                }
            }
        } while(g_bWorking);

    if (f_video) {
        fflush(f_video);
        fsync(fileno(f_video));
    }
#ifdef QSDEMO_AUDIO_SUPPORT
        if (f_audio) {
            fflush(f_audio);
            fsync(fileno(f_audio));
        }
#endif
        if (!g_bSdOnlineMode) {
            QS_RemoveSdDevice(&f_video, &f_audio);
            pthread_mutex_unlock(&g_mutex_sd_load);
        }
        ALOGI("sns[%d] write record to sd --- (cost %llu)", pRecFileInfo->nCamID, GetTickCountPts() - u64SdInPts);
        update_sleep_condition(pRecFileInfo->nCamID == 0 ? CONDITION_MASK_RECORD0 : CONDITION_MASK_RECORD1, true);

    }

    if (f_video) {
        fflush(f_video);
        fsync(fileno(f_video));
        fclose(f_video);
    }
    if (f_audio) {
        fflush(f_audio);
        fsync(fileno(f_audio));
        fclose(f_audio);
    }

    return NULL;
}

AX_S32 QS_VideoRecorderInit(AX_S32 nCamCount, AX_U32 nVideoRingBufSize, AX_U32 nAudioRingBufSize, AX_S32 nMaxRecodFileCount, AX_BOOL bSdOnlieMode)
{
    AX_S32 i = 0;
    AX_S32 j = 0;
    AX_CHAR szName[64] = {0};
    g_nCamCount = nCamCount;
    g_nMaxRecodFileCount = nMaxRecodFileCount;
    g_bSdOnlineMode = bSdOnlieMode;

    ALOGI("max record file count %d", g_nMaxRecodFileCount);

    for (i = 0; i < nCamCount; i++) {
        g_videoRecordFile[i].nCurFileIdx = 0;
        g_videoRecordFile[i].nCamID = i;
        g_videoRecordFile[i].nCurVideoFileSize = 0;
        g_videoRecordFile[i].szBaseDir = (i == 0) ? "/tfcard/qsdemo/aov/sns0_vr" : "/tfcard/qsdemo/aov/sns1_vr";
        g_videoRecordFile[i].nCurFileIdx = 0;
        g_videoRecordFile[i].tid = 0;
        g_videoRecordFile[i].szVideoFile = (char**)malloc(g_nMaxRecodFileCount*sizeof(char*));
        g_videoRecordFile[i].szAudioFile = (char**)malloc(g_nMaxRecodFileCount*sizeof(char*));
        for(j = 0; j < g_nMaxRecodFileCount; j++) {
            g_videoRecordFile[i].szVideoFile[j] = (char*) malloc(MAX_PATH*sizeof(char));
            g_videoRecordFile[i].szAudioFile[j] = (char*) malloc(MAX_PATH*sizeof(char));
            memset(g_videoRecordFile[i].szVideoFile[j], 0, MAX_PATH*sizeof(char));
            memset(g_videoRecordFile[i].szAudioFile[j], 0, MAX_PATH*sizeof(char));
        }
    }

    for (i = 0; i < nCamCount; i++) {
        sprintf(szName, "VRING%d", i);
        if (AX_RingFifo_Init(&g_videoRecordFile[i].hVideoFifo, NULL, nVideoRingBufSize, szName) != 0) {
            ALOGE("create video fifo failed");
            return -1;
        }
#ifdef QSDEMO_AUDIO_SUPPORT
        sprintf(szName, "ARING%d", i);
        if (AX_RingFifo_Init(&g_videoRecordFile[i].hAudioFifo, NULL, nAudioRingBufSize, szName) != 0) {
            ALOGE("create audio fifo failed");
            return -1;
        }
#endif
    }

    g_bRecordInited = AX_TRUE;
    return 0;
}

AX_S32 QS_VideoRecorderDeinit()
{
    ALOGI("QS_VideoRecorderDeinit ++");
    AX_S32 i = 0;
    AX_S32 j = 0;
    if (!g_bRecordInited) {
        return 0;
    }

    g_bRecordInited = AX_FALSE;

    for (i = 0; i < g_nCamCount; i++) {
        if (g_videoRecordFile[i].hVideoFifo) {
            AX_RingFifo_Deinit(g_videoRecordFile[i].hVideoFifo);
            g_videoRecordFile[i].hVideoFifo = NULL;
        }

        if (g_videoRecordFile[i].hAudioFifo) {
            AX_RingFifo_Deinit(g_videoRecordFile[i].hAudioFifo);
            g_videoRecordFile[i].hAudioFifo = NULL;
        }

        for(j = 0; j < g_nMaxRecodFileCount; j++) {
            free(g_videoRecordFile[i].szVideoFile[j]);
            free(g_videoRecordFile[i].szAudioFile[j]);
        }
        free(g_videoRecordFile[i].szVideoFile);
        free(g_videoRecordFile[i].szAudioFile);
    }
    ALOGI("QS_VideoRecorderDeinit --");
    return 0;
}

AX_S32 QS_VideoRecorderStart() {
    AX_S32 i = 0;

    if (!g_bRecordInited || g_bWorking) {
        return 0;
    }
    g_bWorking = AX_TRUE;

    for (i = 0; i < g_nCamCount; i++) {
        // create thread to save data
        pthread_create(&g_videoRecordFile[i].tid, NULL, record_thread_func, &g_videoRecordFile[i]);
    }
    return 0;
}

AX_S32 QS_VideoRecorderStop() {
    ALOGI("QS_VideoRecorderStop ++");
    AX_S32 i = 0;
    if (!g_bRecordInited || !g_bWorking) {
        return 0;
    }
    g_bWorking = AX_FALSE;
    for (i = 0; i < g_nCamCount; i++) {
        // join thread
        if (g_videoRecordFile[i].tid != 0) {
            pthread_join(g_videoRecordFile[i].tid, NULL);
        }
    }
    ALOGI("QS_VideoRecorderStop --");
    return 0;
}

AX_S32 QS_SaveVideo(AX_S32 nCamIdx, AX_U8 *pData, AX_S32 nSize, AX_U64 nPts, AX_BOOL bIFrame, AX_BOOL bFlush)
{
    AX_S32 ret = 0;
    if (nCamIdx >= g_nCamCount) {
        return 0;
    }

    if (!g_bRecordInited) {
        //ALOGE("recorder is not inited!");
        return 0;
    }

    ret = AX_RingFifo_Put(g_videoRecordFile[nCamIdx].hVideoFifo, pData, nSize, NULL, 0, nPts, bIFrame);
    // if (ret == 0) {
    //     ALOGI("[rec%d][put] pts=%llu, ifrm=%d", nCamIdx, nPts, bIFrame);
    // } else {
    //     ALOGI("[rec%d][put] pts=%llu, ifrm=%d, ring is full", nCamIdx, nPts, bIFrame);
    // }

    if (bFlush) {
        while (AX_RingFifo_Size(g_videoRecordFile[nCamIdx].hVideoFifo) != 0) {
            usleep(10000);
        }
    }

    return ret;
}

AX_S32 QS_SaveAudio(AX_S32 nCamIdx, AX_U8 *pData, AX_S32 nSize, AX_U64 nPts, AX_BOOL bFlush)
{
    AX_S32 ret = 0;
    if (nCamIdx >= g_nCamCount) {
        return 0;
    }

    if (!g_bRecordInited) {
        //ALOGE("recorder is not inited!");
        return 0;
    }

    ret = AX_RingFifo_Put(g_videoRecordFile[nCamIdx].hAudioFifo, pData, nSize, NULL, 0, nPts, AX_TRUE);

    if (bFlush) {
        while (AX_RingFifo_Size(g_videoRecordFile[nCamIdx].hAudioFifo) != 0) {
            usleep(10000);
        }
    }

    return ret;
}

AX_S32  QS_VideoRecorderWakeup(AX_BOOL bWakeup) {
    for (AX_S32 i = 0; i < g_nCamCount; i++) {
        pthread_mutex_lock(&g_mutex_wakup_record[i]);
        g_bWakeupRecord[i] = bWakeup;
        if (bWakeup) {
            pthread_cond_signal(&g_cond_wakup_record[i]);
        }
        pthread_mutex_unlock(&g_mutex_wakup_record[i]);
    }

    return 0;
}

AX_BOOL QS_CopyResultFiles(AX_BOOL bReboot, const AX_CHAR* suffix) {

    ALOGI("QS_CopyResultFiles ++");
    AX_U32 insmodFlag = 0;

    if (g_bSdOnlineMode) {
        if (!QS_IsSDCardReady()) {
            ALOGI("QS_CopyResultFiles --, no sd card");
            return -1;
        }
    } else {
        pthread_mutex_lock(&g_mutex_sd_load);
        AX_U32 tryCnt = 20;
        do {
            AX_CHAR szResult[64] = {0};
            AX_S32 nRet = 0;

            nRet = QS_RunCmd("mount | grep \"/tfcard\" | awk '{print $3}'", szResult, 64);
            if (nRet == 0 && strlen(szResult) > 0) {
                break;
            } else {
                QS_InstallSdDevice();
                insmodFlag = 1;
            }
            if (--tryCnt == 0) {
                ALOGI("QS_CopyResultFiles --, no sd card");
                pthread_mutex_unlock(&g_mutex_sd_load);
                return -1;
            }
            MSSleep(50);
        } while (g_bWorking);
    }

    pthread_mutex_lock(&g_mutex_copy_result);

    AX_CHAR command[MAX_PATH] = {0};
    AX_CHAR destRootPath[128] = {0};
    AX_S32 nSize = 0;
    if (bReboot) {
        if (suffix) {
            nSize = sprintf(destRootPath, "%s/qs/%d_%s", g_szTFCardRoot, g_nRunningTimes, suffix);
        } else {
            nSize = sprintf(destRootPath, "%s/qs/%d", g_szTFCardRoot, g_nRunningTimes);
        }
    } else {
        if (suffix) {
            nSize = sprintf(destRootPath, "%s/aov/log/%d_%s", g_szTFCardRoot, g_nRunningTimes, suffix);
        } else {
            nSize = sprintf(destRootPath, "%s/aov/log/%d", g_szTFCardRoot, g_nRunningTimes);
        }
    }
    destRootPath[nSize] = 0;

#define RUN_CMD(s) do {\
        if (s > 0 && s < MAX_PATH) {\
            command[s] = 0;\
            QS_RunCmd(command, NULL, 0);\
        }\
    } while(0)

    nSize = sprintf(command, "mkdir -p %s", g_szTFCardRoot);
    RUN_CMD(nSize);

    if (bReboot) {
        if (g_nRunningTimes == 1) {
            // clear sd dir for 1st run time
            nSize = sprintf(command, "rm -rf %s", g_szTFCardRoot);
            RUN_CMD(nSize);
        } else {
            // check sd space and remove old runtime dir if space is not enough
            // QS_SweepDiskQs(g_nRunningTimes);
        }
    }

    // mkdir output
    nSize = sprintf(command, "mkdir -p %s", destRootPath);
    RUN_CMD(nSize);

    // log
    nSize = sprintf(command, "cp -Lrf /opt/data/AXSyslog %s", destRootPath);
    RUN_CMD(nSize);

    // if (IS_AX615Q) {
    //     nSize = sprintf(command, "cp -Lrf /tmp/data/AXSyslog %s", destRootPath);
    //     RUN_CMD(nSize);
    // } else {
    //     nSize = sprintf(command, "cp -Lrf /customer/data/AXSyslog %s", destRootPath);
    //     RUN_CMD(nSize);
    //     // 630C, delete log, because log will increase and copying time will increase.
    //     nSize = sprintf(command, "/etc/init.d/S02klogd stop");
    //     RUN_CMD(nSize);
    //     nSize = sprintf(command, "rm -rf /customer/data/AXSyslog");
    //     RUN_CMD(nSize);
    //     nSize = sprintf(command, "mkdir -p /customer/data/AXSyslog/kernel");
    //     RUN_CMD(nSize);
    //     nSize = sprintf(command, "/etc/init.d/S01syslogd restart && /etc/init.d/S02klogd start");
    //     RUN_CMD(nSize);
    // }

    if (bReboot) {
        // riscv log
        nSize = sprintf(command, "cat /proc/ax_proc/riscv/log_dump > %s/riscv.log", destRootPath);
        RUN_CMD(nSize);

        // jpeg
        nSize = sprintf(command, "if [ -e '/tmp/jenc0.jpg' ]; then cp -f /tmp/*.jpg %s; fi", destRootPath);
        RUN_CMD(nSize);

        // h264
        nSize = sprintf(command, "if [ -e '/tmp/venc0.264' ]; then cp -f /tmp/*.264 %s; fi", destRootPath);
        RUN_CMD(nSize);
    }

    // sync
    QS_RunCmd("sync", NULL, 0);

    ALOGI("copy result files to %s done", destRootPath);

    pthread_mutex_unlock(&g_mutex_copy_result);

    if (!g_bSdOnlineMode) {
        if (insmodFlag) {
            QS_RemoveSdDevice(NULL, NULL);
        }
        pthread_mutex_unlock(&g_mutex_sd_load);
    }
    ALOGI("QS_CopyResultFiles --");
    return 0;
}

AX_VOID * _CopyResultFiles(AX_VOID * para) {
    QS_SetThreadName("qs_copy_result");
    QS_CopyResultFiles(AX_TRUE, NULL);
    return NULL;
}

AX_VOID QS_StartCopyResultFiles() {
    pthread_create(&g_CopyResultTid, NULL, _CopyResultFiles, NULL);
}

AX_VOID QS_StopCopyResultFiles() {
    ALOGI("QS_StopCopyResultFiles ++");
    if (g_CopyResultTid != 0) {
        pthread_join(g_CopyResultTid, NULL);
    }
    ALOGI("QS_StopCopyResultFiles --");
}

AX_S32  QS_ChangeSysLogPath(AX_S32 nRunTimes) {
    // mount /dev/mmcblk0p1 /tfcard
    // sed -i -re "s/AX_SYSLOG_path =.*/AX_SYSLOG_path = \/tfcard\/log /g" /var/log/AXSyslog/ax_syslog.conf
    // sed -i -re "s/user\.\*.*    \/var\/log\/AXSyslog\/kernel\/user.log/user\.\*          \/tfcard\/log\/AXSyslog\/kernel\/user.log/g" /var/log/AXSyslog/syslog.conf
    // mkdir -p /tfcard/log/AXSyslog/kernel
    // mkdir -p /tfcard/log/AXSyslog/syslog
    // /etc/init.d/axklogd stop
    // /etc/init.d/axsyslogd restart
    // /etc/init.d/axklogd start &
    AX_CHAR szCmd[256] = {0};
    AX_S32 nCount = 0;
    AX_CHAR szLogPath[128] = {0};
    AX_S32 nSize = 0;

    nSize = sprintf(szLogPath, "%s/qs/%d/syslog", g_szTFCardRoot, nRunTimes);
    szLogPath[nSize] = 0;

    nCount = sprintf(szCmd, "mkdir -p %s", szLogPath);
    szCmd[nCount] = 0;
    QS_RunCmd(szCmd, NULL, 0);

    nCount = sprintf(szCmd, "sed -i -re 's|AX_SYSLOG_path =.*|AX_SYSLOG_path = %s|g' /etc/ax_syslog.conf", szLogPath);
    szCmd[nCount] = 0;
    QS_RunCmd(szCmd, NULL, 0);

    nCount = sprintf(szCmd, "sed -i -re 's|AX_SYSLOG_filesizemax =.*|AX_SYSLOG_filesizemax = 32|g' /etc/ax_syslog.conf");
    szCmd[nCount] = 0;
    QS_RunCmd(szCmd, NULL, 0);

    nCount = sprintf(szCmd, "sed -i -re 's|AX_SYSLOG_volume =.*|AX_SYSLOG_volume = 1024|g' /etc/ax_syslog.conf");
    szCmd[nCount] = 0;
    QS_RunCmd(szCmd, NULL, 0);

    nCount = sprintf(szCmd, "/etc/init.d/axsyslogd reload");
    szCmd[nCount] = 0;
    system(szCmd);

    ALOGI("syslog path changed to: %s", szLogPath);

    return 0;
}

AX_S32 QS_GetSDRuntimePath(AX_CHAR *szPath, AX_S32 nLen, AX_S32 nRunTimes, AX_BOOL bReboot) {
    if (!szPath || nLen <= 0) {
        return -1;
    }
    AX_CHAR szCmd[256] = {0};

    AX_S32 nSize = 0;
    if (bReboot) {
        nSize = snprintf(szPath, nLen - 1, "%s/qs/%d", g_szTFCardRoot, nRunTimes);
    } else {
        nSize = snprintf(szPath, nLen - 1, "%s/aov/%d", g_szTFCardRoot, nRunTimes);
    }

    szPath[nSize] = 0;

    nSize = sprintf(szCmd, "mkdir -p %s", szPath);
    szCmd[nSize] = 0;
    QS_RunCmd(szCmd, NULL, 0);

    return 0;
}


/* check mount point */
AX_BOOL QS_CheckMounted(const char* mntpt, AX_S32 nTimes) {

    AX_BOOL bRet = AX_FALSE;
    AX_S32 nTimesTmp = nTimes;

    FILE *fp;
    AX_CHAR buf[256];
    memset(buf, 0, sizeof(buf));
    AX_CHAR command[256];
    memset(command, 0, sizeof(command));

    snprintf(command, sizeof(command), "cat /proc/mounts | grep '%s '", mntpt);
    do {
        fp = popen(command, "r");
        if (fp == NULL) {
            /* excute command failed */
            ALOGE("popen failed: %s.", command);
            break;
        }

        if (fgets(buf, sizeof(buf) - 1, fp) != NULL) {
            bRet = AX_TRUE;
        }
        pclose(fp);
        if (bRet) {
            break;
        }
        nTimesTmp --;
        MSSleep(40);
    } while (nTimesTmp > 0);

    ALOGI("retry cat mounts times: %d.", nTimes - nTimesTmp);
    return bRet;
}
