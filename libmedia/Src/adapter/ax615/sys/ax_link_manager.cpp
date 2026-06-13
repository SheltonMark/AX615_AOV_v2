#include "ax_link_manager.hpp"

#include "ax_sys_api.h"
#include <algorithm>
#include <cstdio>
#include <cstring>

namespace aov::media::ax615 {

namespace {

AX_MOD_INFO_T ToAxMod(const AxModPort& port) {
    AX_MOD_INFO_T mod;
    mod.enModId = static_cast<AX_MOD_ID_E>(port.mod);
    mod.s32GrpId = port.dev;
    mod.s32ChnId = port.chn;
    return mod;
}

bool SameLink(const AxLink& link, const AxModPort& src, const AxModPort& dst) {
    return link.src.mod == src.mod && link.src.dev == src.dev && link.src.chn == src.chn &&
           link.dst.mod == dst.mod && link.dst.dev == dst.dev && link.dst.chn == dst.chn;
}

}  // namespace

AxLinkManager::~AxLinkManager() {
    std::fprintf(stderr, "[AxLinkManager][DEBUG] Destructor called, links count=%zu\n", links_.size());
    UnLinkAll();
    std::fprintf(stderr, "[AxLinkManager][DEBUG] Destructor completed\n");
}

bool AxLinkManager::Link(const AxModPort& src, const AxModPort& dst) {
    AX_MOD_INFO_T ax_src = ToAxMod(src);
    AX_MOD_INFO_T ax_dst = ToAxMod(dst);

    std::fprintf(stderr, "[AxLinkManager][v20260611] Linking: src(mod=%d, grp=%d, chn=%d) -> dst(mod=%d, grp=%d, chn=%d)\n",
                 ax_src.enModId, ax_src.s32GrpId, ax_src.s32ChnId,
                 ax_dst.enModId, ax_dst.s32GrpId, ax_dst.s32ChnId);

    // Use AX_SYS_Link_V2 with link attributes (like QSDemo does)
    AX_MOD_LINK_ATTR_T link_attr;
    std::memset(&link_attr, 0, sizeof(link_attr));

    // Set link mode based on src/dst modules (matching QSDemo behavior)
    if (ax_src.enModId == AX_ID_VIN && ax_dst.enModId == AX_ID_IVPS) {
        // VIN -> IVPS: Try LOW_MEM mode instead of OFFLINE (QSDemo line 3047-3050)
        // CRITICAL: OFFLINE mode seems to have flow control issues with GDC engine
        link_attr.eDataFlowMode = AX_MOD_DATA_FLOW_SW_LOW_MEM;
        link_attr.eSrcEngine = AX_ENG_NONE;
        link_attr.eDstEngine = AX_ENG_GDC;  // Match IVPS group filter engine
        link_attr.nRingLineCnt = 128;
        std::fprintf(stderr, "[AxLinkManager][v20260611] VIN->IVPS: LOW_MEM mode with GDC engine\n");
    } else if (ax_src.enModId == AX_ID_IVPS && ax_dst.enModId == AX_ID_VENC) {
        // IVPS -> VENC: use LOW_MEM mode (QSDemo line 260)
        link_attr.eDataFlowMode = AX_MOD_DATA_FLOW_SW_LOW_MEM;
        link_attr.eSrcEngine = AX_ENG_VPP;
        link_attr.eDstEngine = AX_ENG_VENC;
        link_attr.nRingLineCnt = 128;
        std::fprintf(stderr, "[AxLinkManager][v20260611] IVPS->VENC: LOW_MEM mode\n");
    } else if (ax_src.enModId == AX_ID_IVPS && ax_dst.enModId == AX_ID_JENC) {
        // IVPS -> JENC: use LOW_MEM mode
        link_attr.eDataFlowMode = AX_MOD_DATA_FLOW_SW_LOW_MEM;
        link_attr.eSrcEngine = AX_ENG_VPP;
        link_attr.eDstEngine = AX_ENG_JENC;
        link_attr.nRingLineCnt = 128;
        std::fprintf(stderr, "[AxLinkManager][v20260611] IVPS->JENC: LOW_MEM mode\n");
    } else {
        // Default: OFFLINE mode
        link_attr.eDataFlowMode = AX_MOD_DATA_FLOW_SW_OFFLINE;
        link_attr.eSrcEngine = AX_ENG_NONE;
        link_attr.eDstEngine = AX_ENG_NONE;
        link_attr.nRingLineCnt = 0;
        std::fprintf(stderr, "[AxLinkManager][v20260611] Using default OFFLINE mode\n");
    }

    AX_S32 ret = AX_SYS_Link_V2(&ax_src, &ax_dst, &link_attr);
    if (ret != AX_SUCCESS) {
        std::fprintf(stderr, "[AxLinkManager][v20260611] AX_SYS_Link_V2 failed: 0x%x\n", ret);
        return false;
    }

    links_.push_back({src, dst});
    std::fprintf(stderr, "[AxLinkManager][v20260611] Link OK\n");
    return true;
}

bool AxLinkManager::UnLink(const AxModPort& src, const AxModPort& dst) {
    AX_MOD_INFO_T ax_src = ToAxMod(src);
    AX_MOD_INFO_T ax_dst = ToAxMod(dst);
    AX_S32 ret = AX_SYS_UnLink(&ax_src, &ax_dst);
    if (ret != AX_SUCCESS) {
        return false;
    }

    links_.erase(std::remove_if(links_.begin(), links_.end(),
                                [&](const AxLink& link) { return SameLink(link, src, dst); }),
                 links_.end());
    return true;
}

void AxLinkManager::UnLinkAll() {
    for (auto it = links_.rbegin(); it != links_.rend(); ++it) {
        AX_MOD_INFO_T ax_src = ToAxMod(it->src);
        AX_MOD_INFO_T ax_dst = ToAxMod(it->dst);
        AX_SYS_UnLink(&ax_src, &ax_dst);
    }
    links_.clear();
}

}  // namespace aov::media::ax615
