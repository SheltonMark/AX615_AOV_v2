/**************************************************************************************************
 *
 * Copyright (c) 2019-2025 Axera Semiconductor Co., Ltd. All Rights Reserved.
 *
 * This source file is the property of Axera Semiconductor Co., Ltd. and
 * may not be copied or distributed in any isomorphic form without the prior
 * written consent of Axera Semiconductor Co., Ltd.
 *
 **************************************************************************************************/

#ifndef _QSRECORDER_H__
#define _QSRECORDER_H__

#define MAX_RECORD_FILE_COUNT  (5)

#include "qs_log.h"
#include "ax_global_type.h"

#ifndef __RTT_OS__
AX_S32  QS_InstallSdDevice();
AX_S32  QS_MonitorSDCardStart();
AX_S32  QS_MonitorSDCardStop();
AX_BOOL QS_IsSDCardReady();

AX_S32  QS_VideoRecorderInit(AX_S32 nCamCount, AX_U32 nVideoRingBufSize, AX_U32 nAudioRingBufSize, AX_S32 nMaxRecodFileCount, AX_BOOL bSdOnlieMode);
AX_S32  QS_VideoRecorderDeinit();
AX_S32  QS_VideoRecorderStart();
AX_S32  QS_VideoRecorderStop();
AX_S32  QS_SaveVideo(AX_S32 nCamIdx, AX_U8 *pData, AX_S32 nSize, AX_U64 nPts, AX_BOOL bIFrame, AX_BOOL bFlush);
AX_S32  QS_SaveAudio(AX_S32 nCamIdx, AX_U8 *pData, AX_S32 nSize, AX_U64 nPts, AX_BOOL bFlush);
AX_S32  QS_VideoRecorderWakeup(AX_BOOL bWakeup);

AX_BOOL QS_MountSDCard();
AX_BOOL QS_CheckSDMounted();
AX_BOOL QS_SweepDiskQs(AX_S32 nRunTimes);

AX_BOOL QS_CopyResultFiles(AX_BOOL bReboot, const AX_CHAR* suffix);
AX_VOID QS_StartCopyResultFiles();
AX_VOID QS_StopCopyResultFiles();
AX_S32  QS_ChangeSysLogPath(AX_S32 nRunTimes);
AX_S32  QS_GetSDRuntimePath(AX_CHAR *szPath, AX_S32 nLen, AX_S32 nRunTimes, AX_BOOL bReboot);
AX_BOOL QS_CheckMounted(const char* mntpt, AX_S32 nTimes);
#else
AX_S32  QS_InstallSdDevice()
{
    ALOGW("Not support record in RTT OS\n");
    return -1;
}
AX_S32  QS_MonitorSDCardStart()
{
    ALOGW("Not support record in RTT OS\n");
    return -1;
}
AX_S32  QS_MonitorSDCardStop()
{
    ALOGW("Not support record in RTT OS\n");
    return -1;
}
AX_BOOL QS_IsSDCardReady()
{
    ALOGW("Not support record in RTT OS\n");
    return -1;
}
AX_S32  QS_VideoRecorderInit(AX_S32 nCamCount, AX_U32 nVideoRingBufSize, AX_U32 nAudioRingBufSize, AX_S32 nMaxRecodFileCount, AX_BOOL bSdOnlieMode)
{
    ALOGW("Not support record in RTT OS\n");
    return -1;
}
AX_S32  QS_VideoRecorderDeinit()
{
    ALOGW("Not support record in RTT OS\n");
    return -1;
}
AX_S32  QS_VideoRecorderStart()
{
    ALOGW("Not support record in RTT OS\n");
    return -1;
}
AX_S32  QS_VideoRecorderStop()
{
    ALOGW("Not support record in RTT OS\n");
    return -1;
}
AX_S32  QS_SaveVideo(AX_S32 nCamIdx, AX_U8 *pData, AX_S32 nSize, AX_U64 nPts, AX_BOOL bIFrame, AX_BOOL bFlush)
{
    ALOGW("Not support record in RTT OS\n");
    return -1;
}
AX_S32  QS_SaveAudio(AX_S32 nCamIdx, AX_U8 *pData, AX_S32 nSize, AX_U64 nPts, AX_BOOL bFlush)
{
    ALOGW("Not support record in RTT OS\n");
    return -1;
}
AX_S32  QS_VideoRecorderWakeup(AX_BOOL bWakeup)
{
    ALOGW("Not support record in RTT OS\n");
    return -1;
}

AX_BOOL QS_MountSDCard()
{
    ALOGW("Not support record in RTT OS\n");
    return -1;
}
AX_BOOL QS_CheckSDMounted()
{
    ALOGW("Not support record in RTT OS\n");
    return -1;
}
AX_BOOL QS_SweepDiskQs(AX_S32 nRunTimes)
{
    ALOGW("Not support record in RTT OS\n");
    return -1;
}

AX_BOOL QS_CopyResultFiles(AX_BOOL bReboot, const AX_CHAR* suffix)
{
    ALOGW("Not support record in RTT OS\n");
    return -1;
}
AX_VOID QS_StartCopyResultFiles()
{
    ALOGW("Not support record in RTT OS\n");
    return -1;
}
AX_VOID QS_StopCopyResultFiles()
{
    ALOGW("Not support record in RTT OS\n");
    return -1;
}
AX_S32  QS_ChangeSysLogPath(AX_S32 nRunTimes)
{
    ALOGW("Not support record in RTT OS\n");
    return -1;
}
AX_S32  QS_GetSDRuntimePath(AX_CHAR *szPath, AX_S32 nLen, AX_S32 nRunTimes, AX_BOOL bReboot)
{
    ALOGW("Not support record in RTT OS\n");
    return -1;
}
AX_BOOL QS_CheckMounted(const char* mntpt, AX_S32 nTimes)
{
    ALOGW("Not support record in RTT OS\n");
    return -1;
}
#endif
#endif //_QSRECORDER_H__