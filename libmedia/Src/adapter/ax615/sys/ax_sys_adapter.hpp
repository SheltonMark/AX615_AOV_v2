#pragma once
/**
 * ax_sys_adapter.hpp
 * AX615 系统初始化与CMPool配置（从QSDemo qs_common_sys移植）
 * 唯一可以 #include AX SDK 头文件的地方
 */
#include <vector>
#include <cstdint>
#include "common/media_types.hpp"

// AX SDK headers（仅 adapter 层可包含）
#include "ax_base_type.h"
#include "ax_global_type.h"
#include "ax_pool_type.h"
#include "ax_sys_api.h"

namespace aov::media::ax615 {

/// Pool块配置（内部用，对应AX_POOL_CONFIG_T）
struct PoolBlockCfg {
    AX_U32  nWidth        = 0;
    AX_U32  nHeight       = 0;
    AX_U32  nWidthStride  = 0;
    AX_IMG_FORMAT_E nFmt   = AX_FORMAT_YUV420_SEMIPLANAR;
    AX_U32  nBlkCnt       = 3;
    AX_COMPRESS_MODE_E enCompressMode = AX_COMPRESS_MODE_NONE;
    AX_U32  u32CompressLevel = 0;
    AX_U32  ubufMode      = 0;
};

class AxSysAdapter {
public:
    AxSysAdapter() = default;
    ~AxSysAdapter();

    // 禁止拷贝
    AxSysAdapter(const AxSysAdapter&)            = delete;
    AxSysAdapter& operator=(const AxSysAdapter&) = delete;

    /// 初始化 AX_SYS_Init
    MediaStatusCode Init();
    MediaStatusCode ConfigCmPool(const CmPoolConfig& pools);

    /// 计算并配置 CMPool（调用 AX_POOL_SetConfig + AX_POOL_Init）
    MediaStatusCode ConfigCmPool(const std::vector<PoolBlockCfg>& pools);

    /// 反初始化
    void Deinit();

    bool IsInitialized() const { return initialized_; }

    /// 获取板载芯片类型（调用QSDemo QS_COMMON_SYS_GetBoardId）
    int GetBoardId(char* chip_type);

    /// 创建私有内存池
    AX_POOL CreatePool(AX_U32 frame_size, AX_U32 depth, const char* name);
    void    DestroyPool(AX_POOL pool_id);

private:
    AX_U32 AddToPlan(AX_POOL_FLOORPLAN_T* plan, AX_U32 cnt, const AX_POOL_CONFIG_T& cfg);
    MediaStatusCode CalcPool(const std::vector<PoolBlockCfg>& pools,
                             AX_POOL_FLOORPLAN_T* out_plan);

    bool initialized_ = false;
    AX_POOL_FLOORPLAN_T pool_plan_{};
};

}  // namespace aov::media::ax615
