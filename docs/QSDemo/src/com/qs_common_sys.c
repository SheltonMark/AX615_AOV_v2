/**************************************************************************************************
 *
 * Copyright (c) 2019-2025 Axera Semiconductor Co., Ltd. All Rights Reserved.
 *
 * This source file is the property of Axera Semiconductor Co., Ltd. and
 * may not be copied or distributed in any isomorphic form without the prior
 * written consent of Axera Semiconductor Co., Ltd.
 *
 **************************************************************************************************/

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include "ax_isp_api.h"
#include "ax_gzipd_api.h"
#include "qs_common_sys.h"
#include "qs_common_cam.h"
#include "ax_buffer_tool.h"
#include "ax_global_type.h"
#include "qs_log.h"

AX_U32 QS_COMMON_SYS_AddToPlan(AX_POOL_FLOORPLAN_T *pPoolFloorPlan, AX_U32 nCfgCnt, AX_POOL_CONFIG_T *pPoolConfig)
{
    AX_U32 i, done = 0;
    AX_POOL_CONFIG_T *pPC;

    for (i = 0; i < nCfgCnt; i++) {
        pPC = &pPoolFloorPlan->CommPool[i];
        if (pPC->BlkSize == pPoolConfig->BlkSize) {
            pPC->BlkCnt += pPoolConfig->BlkCnt;
            done = 1;
        }
    }

    if (!done) {
        pPoolFloorPlan->CommPool[i] = *pPoolConfig;
        nCfgCnt += 1;
    }

    return nCfgCnt;
}

AX_S32 QS_COMMON_SYS_CalcPool(QS_COMMON_SYS_POOL_CFG_T *pPoolCfg, AX_U32 nCommPoolCnt, AX_POOL_FLOORPLAN_T *pPoolFloorPlan)
{
    AX_S32 i, nCfgCnt = 0;
    AX_FRAME_COMPRESS_INFO_T tCompressInfo = {0};
    AX_POOL_CONFIG_T tPoolConfig = {
        .AlignSize = 4096,
        .MetaSize  = 1024,
        .CacheMode = AX_POOL_CACHE_MODE_NONCACHE,
        .PartitionName = "anonymous"
    };

    for (i = 0; i < nCommPoolCnt; i++) {
        tCompressInfo.enCompressMode = pPoolCfg->enCompressMode;
        tCompressInfo.u32CompressLevel = pPoolCfg->u32CompressLevel;
        tPoolConfig.BlkSize = AX_VIN_GetBufferSize(pPoolCfg->nHeight, pPoolCfg->nWidthStride, pPoolCfg->nFmt, &tCompressInfo, 0, pPoolCfg->ubufMode);
        tPoolConfig.BlkCnt  = pPoolCfg->nBlkCnt;
        nCfgCnt = QS_COMMON_SYS_AddToPlan(pPoolFloorPlan, nCfgCnt, &tPoolConfig);
        pPoolCfg += 1;
    }

    return 0;
}

AX_S32 QS_COMMON_SYS_Init(QS_COMMON_SYS_ARGS_T *pCommonArgs)
{
    AX_S32 axRet = 0;
    AX_POOL_FLOORPLAN_T tPoolFloorPlan = {0};

    axRet = AX_SYS_Init();
    if (0 != axRet) {
        QS_COMM_SYS_PRT("AX_SYS_Init failed, ret=0x%x.\n", axRet);
        return -1;
    }
#ifndef __RTT_OS__
    axRet = AX_GZIPD_Init();
    if (axRet) {
        QS_COMM_SYS_PRT("gzipd init fail\n");
        return axRet;
    }
#endif

    /* Release last Pool */
    axRet = AX_POOL_Exit();
    if (0 != axRet) {
        QS_COMM_SYS_PRT("AX_POOL_Exit fail!!Error Code:0x%X\n", axRet);
    }

    /* Calc Pool BlkSize/BlkCnt */
    axRet = QS_COMMON_SYS_CalcPool(pCommonArgs->pPoolCfg, pCommonArgs->nPoolCfgCnt, &tPoolFloorPlan);
    if (0 != axRet) {
        QS_COMM_SYS_PRT("QS_COMMON_SYS_CalcPool failed, ret=0x%x.\n", axRet);
        return -1;
    }

    for (AX_U32 k = 0; k < pCommonArgs->nPoolCfgCnt; k++) {
        QS_COMM_SYS_PRT("Comm Pool[%d]: (%dx%dx%d)_mode_%d_FBC_%d_%dx%d, BlkSize:%lld, MetaSize:%lld\n",
              k,
              pCommonArgs->pPoolCfg[k].nWidth, pCommonArgs->pPoolCfg[k].nHeight, pCommonArgs->pPoolCfg[k].nWidthStride,
              pCommonArgs->pPoolCfg[k].ubufMode,
              pCommonArgs->pPoolCfg[k].enCompressMode, pCommonArgs->pPoolCfg[k].u32CompressLevel,
              tPoolFloorPlan.CommPool[k].BlkCnt,
              tPoolFloorPlan.CommPool[k].BlkSize,
              tPoolFloorPlan.CommPool[k].MetaSize);
    }

    axRet = AX_POOL_SetConfig(&tPoolFloorPlan);
    if (0 != axRet) {
        QS_COMM_SYS_PRT("AX_POOL_SetConfig fail!Error Code:0x%X\n", axRet);
        return -1;
    }

    axRet = AX_POOL_Init();
    if (0 != axRet) {
        QS_COMM_SYS_PRT("AX_POOL_Init fail!!Error Code:0x%X\n", axRet);
        return -1;
    }

    return 0;
}

AX_S32 QS_COMMON_SYS_DeInit()
{
    AX_S32 nRet = AX_SUCCESS;
    ALOGI("QS_COMMON_SYS_DeInit ++");
    nRet = AX_POOL_Exit();
    if (0 != nRet) {
#ifndef __RTT_OS__
        AX_GZIPD_Deinit();
#endif
        AX_SYS_Deinit();
        ALOGI("QS_COMMON_SYS_DeInit --");
        return nRet;
    }

#ifndef __RTT_OS__
    AX_GZIPD_Deinit();
#endif
    AX_SYS_Deinit();
    ALOGI("QS_COMMON_SYS_DeInit --");
    return 0;
}

AX_S8 QS_COMMON_SYS_GetBoardId(AX_CHAR *chip_type)
{
    FILE *pFile = NULL;
    AX_CHAR temp[BOARD_ID_LEN] = {0};

    pFile = fopen("/proc/ax_proc/chip_type", "r");
    if (pFile) {
        if (fread(temp, BOARD_ID_LEN, 1, pFile) != 1) {
            fclose(pFile);
            return -1;
        }
        fclose(pFile);
    } else {
        QS_COMM_SYS_PRT("fopen /proc/ax_proc/chip_type failed!!!\n");
        return -1;
    }

    strncpy(chip_type, temp, BOARD_ID_LEN * sizeof(AX_CHAR));
    return 0;
}

AX_POOL QS_COMMON_SYS_CreatePool(AX_U32 nFrameSize, AX_U32 nDepth, const AX_CHAR* PoolName) {
    AX_POOL_CONFIG_T stPoolConfig;
    memset(&stPoolConfig, 0, sizeof(AX_POOL_CONFIG_T));
    stPoolConfig.AlignSize = 64;
    stPoolConfig.MetaSize = 128;
    stPoolConfig.BlkCnt = (nDepth == 0) ? 1 : nDepth;
    stPoolConfig.BlkSize = nFrameSize;
    stPoolConfig.CacheMode = AX_POOL_CACHE_MODE_NONCACHE;
    memset(stPoolConfig.PartitionName, 0, sizeof(stPoolConfig.PartitionName));
    strcpy((AX_CHAR *)stPoolConfig.PartitionName, "anonymous");

    if (PoolName) {
        strncpy((AX_CHAR *)stPoolConfig.PoolName, PoolName, AX_MAX_POOL_NAME_LEN - 1);
    }

    return AX_POOL_CreatePool(&stPoolConfig);
}

AX_VOID QS_COMMON_SYS_DestroyPool(AX_POOL PoolId) {
    if (PoolId != AX_INVALID_POOLID) {
        AX_S32 ret = AX_POOL_DestroyPool(PoolId);
        if (ret != AX_SUCCESS) {
            ALOGE("AX_POOL_DestroyPool failed! ret:0x%x", ret);
        }
    }
}

AX_BLK QS_COMMON_SYS_GetBlock(AX_POOL PoolId, AX_U32 nFrameSize, AX_U64 *pPhy, AX_VOID **ppVir) {
    AX_BLK BlockId = AX_POOL_GetBlock(PoolId, nFrameSize, NULL);

    if (BlockId != AX_INVALID_BLOCKID) {
        if (pPhy) {
            *pPhy = (AX_U64)AX_POOL_Handle2PhysAddr(BlockId);
        }
        if (ppVir) {
            *ppVir = (AX_VOID *)AX_POOL_GetBlockVirAddr(BlockId);
        }

        AX_POOL_IncreaseRefCnt(BlockId);
    }

    return BlockId;
}

AX_VOID QS_COMMON_SYS_ReleaseBlock(AX_BLK BlockId) {
    if (BlockId == AX_INVALID_BLOCKID) {
        return;
    }

    AX_POOL_DecreaseRefCnt(BlockId);

    AX_POOL_ReleaseBlock(BlockId);
}
