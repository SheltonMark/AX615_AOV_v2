#include "ax_sys_adapter.hpp"

#include "ax_buffer_tool.h"
#include "ax_vin_api.h"
#include <cstdio>
#include <cstring>
#include "ax_engine_api.h"

namespace aov::media::ax615 {

AxSysAdapter::~AxSysAdapter() {
    Deinit();
}

MediaStatusCode AxSysAdapter::Init() {
    if (initialized_) {
        return MediaStatusCode::Ok;
    }

    std::fprintf(stderr, "[AxSysAdapter][v20260611-fix1] Init() called\n");

    AX_S32 ret = AX_SYS_Init();
    if (ret != AX_SUCCESS) {
        std::fprintf(stderr, "[AxSysAdapter] AX_SYS_Init failed: 0x%x\n", ret);
        return MediaStatusCode::InternalError;
    }
    std::fprintf(stderr, "[AxSysAdapter][v20260611-fix1] AX_SYS_Init OK\n");

    // Release any previous pool config immediately after SYS_Init (like QSDemo does)
    ret = AX_POOL_Exit();
    std::fprintf(stderr, "[AxSysAdapter][v20260611-fix1] AX_POOL_Exit after SYS_Init, ret=0x%x\n", ret);

    initialized_ = true;
    return MediaStatusCode::Ok;
}

MediaStatusCode AxSysAdapter::InitNpu()
{
    std::fprintf(stderr, "[AxSysAdapter] Initializing NPU (AX_ENGINE_Init)\n");

    AX_ENGINE_NPU_ATTR_T attr;
    std::memset(&attr, 0, sizeof(attr));
    attr.eHardMode = AX_ENGINE_VIRTUAL_NPU_ENABLE;

#ifndef __RTT_OS__
    AX_S32 ret = AX_ENGINE_Init(&attr);
    if (ret != AX_SUCCESS) {
        std::fprintf(stderr, "[AxSysAdapter] AX_ENGINE_Init failed: 0x%x\n", ret);
        return MediaStatusCode::InternalError;
    }
#endif

    std::fprintf(stderr, "[AxSysAdapter] NPU initialized successfully\n");
    return MediaStatusCode::Ok;
}


MediaStatusCode AxSysAdapter::ConfigCmPool(const std::vector<PoolBlockCfg>& pools) {
    if (!initialized_) {
        return MediaStatusCode::InvalidState;
    }

    std::memset(&pool_plan_, 0, sizeof(pool_plan_));
    MediaStatusCode status = CalcPool(pools, &pool_plan_);
    if (status != MediaStatusCode::Ok) {
        return status;
    }

    AX_S32 ret = AX_POOL_SetConfig(&pool_plan_);
    if (ret != AX_SUCCESS) {
        std::fprintf(stderr, "[AxSysAdapter] AX_POOL_SetConfig failed: 0x%x\n", ret);
        return MediaStatusCode::InternalError;
    }

    ret = AX_POOL_Init();
    if (ret != AX_SUCCESS) {
        std::fprintf(stderr, "[AxSysAdapter] AX_POOL_Init failed: 0x%x\n", ret);
        return MediaStatusCode::InternalError;
    }

    return MediaStatusCode::Ok;
}

MediaStatusCode AxSysAdapter::ConfigCmPool(const CmPoolConfig& pools) {
    if (!initialized_) {
        return MediaStatusCode::InvalidState;
    }
    if (pools.pools.empty() || pools.pools.size() > AX_MAX_COMM_POOLS) {
        return MediaStatusCode::InvalidArgument;
    }

    std::fprintf(stderr, "[AxSysAdapter][v20260611-fix1] ConfigCmPool called, pool count=%zu\n", pools.pools.size());

    std::memset(&pool_plan_, 0, sizeof(pool_plan_));
    AX_U32 cfg_count = 0;
    for (const auto& pool : pools.pools) {
        if (pool.blk_size == 0 || pool.blk_count == 0) {
            return MediaStatusCode::InvalidArgument;
        }
        AX_POOL_CONFIG_T cfg;
        std::memset(&cfg, 0, sizeof(cfg));
        cfg.AlignSize = 4096;
        cfg.MetaSize = static_cast<AX_U32>(pool.meta_size);
        cfg.CacheMode = pool.cache_mode == PoolCacheMode::Cached ? AX_POOL_CACHE_MODE_CACHED
                                                                 : AX_POOL_CACHE_MODE_NONCACHE;
        cfg.BlkSize = static_cast<AX_U32>(pool.blk_size);
        cfg.BlkCnt = pool.blk_count;
        std::strncpy(reinterpret_cast<char*>(cfg.PartitionName), "anonymous", AX_MAX_PARTITION_NAME_LEN - 1);
        cfg_count = AddToPlan(&pool_plan_, cfg_count, cfg);
    }

    std::fprintf(stderr, "[AxSysAdapter][v20260611-fix1] Calling AX_POOL_SetConfig\n");
    AX_S32 ret = AX_POOL_SetConfig(&pool_plan_);
    if (ret != AX_SUCCESS) {
        std::fprintf(stderr, "[AxSysAdapter] AX_POOL_SetConfig failed: 0x%x\n", ret);
        return MediaStatusCode::InternalError;
    }
    std::fprintf(stderr, "[AxSysAdapter][v20260611-fix1] AX_POOL_SetConfig OK\n");

    std::fprintf(stderr, "[AxSysAdapter][v20260611-fix1] Calling AX_POOL_Init\n");
    ret = AX_POOL_Init();
    if (ret != AX_SUCCESS) {
        std::fprintf(stderr, "[AxSysAdapter][v20260611-fix1] AX_POOL_Init failed: 0x%x\n", ret);
        return MediaStatusCode::InternalError;
    }
    std::fprintf(stderr, "[AxSysAdapter][v20260611-fix1] AX_POOL_Init OK\n");

    return MediaStatusCode::Ok;
}

MediaStatusCode AxSysAdapter::ConfigVinPrivatePool(const std::vector<PoolBlockCfg>& pools) {
    if (!initialized_) {
        return MediaStatusCode::InvalidState;
    }
    if (pools.empty() || pools.size() > AX_MAX_COMM_POOLS) {
        return MediaStatusCode::InvalidArgument;
    }

    std::fprintf(stderr, "[AxSysAdapter][v20260611-fix2] ConfigVinPrivatePool called, pool count=%zu\n", pools.size());

    // CRITICAL: This is QSDemo's QS_COMMON_CAM_PrivPoolInit (qs_common_cam.c lines 86-109)
    // VIN needs private RAW buffer pools configured via AX_VIN_SetPoolAttr BEFORE VIN operations
    // Without this, AX_VIN_StartPipe fails with 0x80110180 because it cannot allocate internal buffers

    AX_POOL_FLOORPLAN_T vin_pool_plan;
    std::memset(&vin_pool_plan, 0, sizeof(vin_pool_plan));

    MediaStatusCode status = CalcPool(pools, &vin_pool_plan);
    if (status != MediaStatusCode::Ok) {
        std::fprintf(stderr, "[AxSysAdapter][v20260611-fix2] CalcPool failed\n");
        return status;
    }

    std::fprintf(stderr, "[AxSysAdapter][v20260611-fix2] Calling AX_VIN_SetPoolAttr\n");
    AX_S32 ret = AX_VIN_SetPoolAttr(&vin_pool_plan);
    if (ret != AX_SUCCESS) {
        std::fprintf(stderr, "[AxSysAdapter] AX_VIN_SetPoolAttr failed: 0x%x\n", ret);
        return MediaStatusCode::InternalError;
    }
    std::fprintf(stderr, "[AxSysAdapter][v20260611-fix2] AX_VIN_SetPoolAttr OK\n");

    return MediaStatusCode::Ok;
}

void AxSysAdapter::Deinit() {
    if (!initialized_) {
        return;
    }

    AX_POOL_Exit();
    AX_SYS_Deinit();
    initialized_ = false;
    std::memset(&pool_plan_, 0, sizeof(pool_plan_));
}

int AxSysAdapter::GetBoardId(char* chip_type) {
    if (!chip_type) {
        return -1;
    }

    FILE* fp = std::fopen("/proc/ax_proc/chip_type", "r");
    if (!fp) {
        return -1;
    }

    constexpr size_t kMaxChipTypeLen = 64;
    char buffer[kMaxChipTypeLen] = {};
    size_t n = std::fread(buffer, 1, kMaxChipTypeLen - 1, fp);
    std::fclose(fp);
    if (n == 0) {
        return -1;
    }

    std::strncpy(chip_type, buffer, kMaxChipTypeLen - 1);
    return 0;
}

AX_POOL AxSysAdapter::CreatePool(AX_U32 frame_size, AX_U32 depth, const char* name) {
    AX_POOL_CONFIG_T cfg;
    std::memset(&cfg, 0, sizeof(cfg));
    cfg.AlignSize = 64;
    cfg.MetaSize = 128;
    cfg.BlkCnt = depth == 0 ? 1 : depth;
    cfg.BlkSize = frame_size;
    cfg.CacheMode = AX_POOL_CACHE_MODE_NONCACHE;
    std::strncpy(reinterpret_cast<char*>(cfg.PartitionName), "anonymous", AX_MAX_PARTITION_NAME_LEN - 1);
    if (name) {
        std::strncpy(reinterpret_cast<char*>(cfg.PoolName), name, AX_MAX_POOL_NAME_LEN - 1);
    }
    return AX_POOL_CreatePool(&cfg);
}

void AxSysAdapter::DestroyPool(AX_POOL pool_id) {
    if (pool_id != AX_INVALID_POOLID) {
        AX_POOL_DestroyPool(pool_id);
    }
}

AX_U32 AxSysAdapter::AddToPlan(AX_POOL_FLOORPLAN_T* plan, AX_U32 cnt, const AX_POOL_CONFIG_T& cfg) {
    if (!plan || cnt >= AX_MAX_COMM_POOLS) {
        return cnt;
    }

    for (AX_U32 i = 0; i < cnt; ++i) {
        if (plan->CommPool[i].BlkSize == cfg.BlkSize) {
            plan->CommPool[i].BlkCnt += cfg.BlkCnt;
            return cnt;
        }
    }

    plan->CommPool[cnt] = cfg;
    return cnt + 1;
}

MediaStatusCode AxSysAdapter::CalcPool(const std::vector<PoolBlockCfg>& pools,
                                       AX_POOL_FLOORPLAN_T* out_plan) {
    if (!out_plan) {
        return MediaStatusCode::InvalidArgument;
    }
    if (pools.size() > AX_MAX_COMM_POOLS) {
        return MediaStatusCode::InvalidArgument;
    }

    AX_U32 cfg_count = 0;
    for (const auto& pool : pools) {
        AX_FRAME_COMPRESS_INFO_T compress_info;
        std::memset(&compress_info, 0, sizeof(compress_info));
        compress_info.enCompressMode = pool.enCompressMode;
        compress_info.u32CompressLevel = pool.u32CompressLevel;

        AX_POOL_CONFIG_T cfg;
        std::memset(&cfg, 0, sizeof(cfg));
        cfg.AlignSize = 4096;
        cfg.MetaSize = 1024;
        cfg.CacheMode = AX_POOL_CACHE_MODE_NONCACHE;
        std::strncpy(reinterpret_cast<char*>(cfg.PartitionName), "anonymous", AX_MAX_PARTITION_NAME_LEN - 1);
        cfg.BlkSize = AX_VIN_GetBufferSize(pool.nHeight,
                                           pool.nWidthStride,
                                           pool.nFmt,
                                           &compress_info,
                                           0,
                                           pool.ubufMode);
        cfg.BlkCnt = pool.nBlkCnt;
        cfg_count = AddToPlan(out_plan, cfg_count, cfg);
    }

    return MediaStatusCode::Ok;
}

}  // namespace aov::media::ax615
