#include "ax_link_manager.hpp"

#include "ax_sys_api.h"
#include <algorithm>

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
    UnLinkAll();
}

bool AxLinkManager::Link(const AxModPort& src, const AxModPort& dst) {
    AX_MOD_INFO_T ax_src = ToAxMod(src);
    AX_MOD_INFO_T ax_dst = ToAxMod(dst);
    AX_S32 ret = AX_SYS_Link(&ax_src, &ax_dst);
    if (ret != AX_SUCCESS) {
        return false;
    }

    links_.push_back({src, dst});
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
