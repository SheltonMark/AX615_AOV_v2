#pragma once
/**
 * ax_link_manager.hpp
 * AX615 模块链路管理 - 封装 AX_SYS_Link / AX_SYS_UnLink
 * 唯一可以 #include AX SDK 的地方
 */
#include <vector>
#include <cstdint>

// AX SDK 前向（仅在 .cpp 中完整包含）
struct AX_MOD_INFO_S;

namespace aov::media::ax615 {

/// 模块标识（映射到 AX_MOD_ID_E，不暴露 SDK 类型）
enum class AxModId : int {
    VIN  = 6,
    ISP  = 7,
    IVPS = 8,
    VENC = 9,
    JENC = 10,
    NPU  = 15,
};

struct AxModPort {
    AxModId mod;
    int     dev;   ///< 设备/组 ID
    int     chn;   ///< 通道/端口 ID
};

/// 单条链路记录
struct AxLink {
    AxModPort src;
    AxModPort dst;
};

/**
 * AxLinkManager
 * 管理所有 AX_SYS_Link 链路，支持批量建立/销毁
 */
class AxLinkManager {
public:
    AxLinkManager() = default;
    ~AxLinkManager();

    // 禁止拷贝
    AxLinkManager(const AxLinkManager&)            = delete;
    AxLinkManager& operator=(const AxLinkManager&) = delete;

    /// 建立一条链路（VIN→IVPS, IVPS→VENC 等）
    bool Link(const AxModPort& src, const AxModPort& dst);

    /// 销毁一条链路
    bool UnLink(const AxModPort& src, const AxModPort& dst);

    /// 销毁所有已建立的链路（析构时自动调用）
    void UnLinkAll();

    const std::vector<AxLink>& GetLinks() const { return links_; }

private:
    std::vector<AxLink> links_;
};

}  // namespace aov::media::ax615
