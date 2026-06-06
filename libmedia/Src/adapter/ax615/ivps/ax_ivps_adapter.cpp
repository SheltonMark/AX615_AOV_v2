#include "ax_ivps_adapter.hpp"

#include <algorithm>
#include <cstring>

namespace aov::media::ax615 {
namespace {

constexpr AX_U8 kMaxOutChannels = AX_IVPS_MAX_OUTCHN_NUM;
constexpr IVPS_RGN_HANDLE kInvalidRegionHandle = static_cast<IVPS_RGN_HANDLE>(-1);

AX_U16 EffectiveStride(const IvpsChannelConfig& ch)
{
    return ch.stride == 0 ? ch.width : ch.stride;
}

}  // namespace

AxIvpsAdapter::~AxIvpsAdapter()
{
    Close();
}

bool AxIvpsAdapter::Open(const IvpsGroupConfig& cfg)
{
    if (opened_) {
        return true;
    }

    if (cfg.channels.empty() || cfg.channels.size() > kMaxOutChannels) {
        return false;
    }

    cfg_ = cfg;

    AX_S32 ret = AX_IVPS_Init();
    if (ret != AX_SUCCESS) {
        return false;
    }
    ivps_initialized_ = true;

    AX_IVPS_GRP_ATTR_T grp_attr;
    std::memset(&grp_attr, 0, sizeof(grp_attr));
    grp_attr.nInFifoDepth = cfg_.in_fifo_depth;
    grp_attr.ePipeline = AX_IVPS_PIPELINE_DEFAULT;

    ret = AX_IVPS_CreateGrp(cfg_.grp_id, &grp_attr);
    if (ret != AX_SUCCESS) {
        Close();
        return false;
    }
    group_created_ = true;

    if (!SetPipelineAttr() || !EnableChannels()) {
        Close();
        return false;
    }

    opened_ = true;
    return true;
}

bool AxIvpsAdapter::Start()
{
    if (!opened_ || running_) {
        return opened_ && running_;
    }

    const AX_S32 ret = AX_IVPS_StartGrp(cfg_.grp_id);
    if (ret != AX_SUCCESS) {
        return false;
    }

    running_ = true;
    return true;
}

void AxIvpsAdapter::Stop()
{
    if (!running_) {
        return;
    }

    AX_IVPS_StopGrp(cfg_.grp_id);
    running_ = false;
}

void AxIvpsAdapter::Close()
{
    Stop();
    DisableChannels();

    if (group_created_) {
        AX_IVPS_DestroyGrp(cfg_.grp_id);
        group_created_ = false;
    }

    if (ivps_initialized_) {
        AX_IVPS_Deinit();
        ivps_initialized_ = false;
    }

    opened_ = false;
    cfg_ = {};
}

IVPS_RGN_HANDLE AxIvpsAdapter::CreateRegion(IVPS_FILTER_ID filter, const AX_IVPS_RGN_DISP_GROUP_T& disp)
{
    if (!opened_) {
        return kInvalidRegionHandle;
    }

    const IVPS_RGN_HANDLE handle = AX_IVPS_RGN_Create();
    if (handle == kInvalidRegionHandle) {
        return kInvalidRegionHandle;
    }

    AX_S32 ret = AX_IVPS_RGN_AttachToFilter(handle, cfg_.grp_id, filter);
    if (ret != AX_SUCCESS) {
        AX_IVPS_RGN_Destroy(handle);
        return kInvalidRegionHandle;
    }

    ret = AX_IVPS_RGN_Update(handle, &disp);
    if (ret != AX_SUCCESS) {
        AX_IVPS_RGN_DetachFromFilter(handle, cfg_.grp_id, filter);
        AX_IVPS_RGN_Destroy(handle);
        return kInvalidRegionHandle;
    }

    return handle;
}

bool AxIvpsAdapter::UpdateRegion(IVPS_RGN_HANDLE handle, const AX_IVPS_RGN_DISP_GROUP_T& disp)
{
    if (!opened_ || handle == kInvalidRegionHandle) {
        return false;
    }

    return AX_IVPS_RGN_Update(handle, &disp) == AX_SUCCESS;
}

void AxIvpsAdapter::DestroyRegion(IVPS_RGN_HANDLE handle, IVPS_FILTER_ID filter)
{
    if (handle == kInvalidRegionHandle) {
        return;
    }

    if (opened_) {
        AX_IVPS_RGN_DetachFromFilter(handle, cfg_.grp_id, filter);
    }
    AX_IVPS_RGN_Destroy(handle);
}

bool AxIvpsAdapter::SetPipelineAttr()
{
    AX_IVPS_PIPELINE_ATTR_T attr;
    std::memset(&attr, 0, sizeof(attr));

    attr.nOutChnNum = static_cast<AX_U8>(cfg_.channels.size());
    attr.nInDebugFifoDepth = cfg_.in_debug_fifo_depth;
    attr.eFRCMode = cfg_.frc_mode;

    AX_U8 max_chn_id = 0;
    for (const auto& ch : cfg_.channels) {
        if (ch.chn_id < 0 || ch.chn_id >= kMaxOutChannels) {
            return false;
        }

        max_chn_id = std::max(max_chn_id, static_cast<AX_U8>(ch.chn_id));
        attr.nOutFifoDepth[ch.chn_id] = ch.out_fifo_depth;

        AX_IVPS_FILTER_T& filter = attr.tFilter[ch.chn_id + 1][0];
        filter.bEngage = AX_TRUE;
        filter.eEngine = ch.engine;
        filter.tFRC.fSrcFrameRate = static_cast<AX_F32>(ch.src_fps);
        filter.tFRC.fDstFrameRate = static_cast<AX_F32>(ch.dst_fps);
        filter.nDstPicWidth = ch.width;
        filter.nDstPicHeight = ch.height;
        filter.nDstPicStride = EffectiveStride(ch);
        filter.eDstPicFormat = ch.format;
        filter.eSclType = AX_IVPS_SCL_TYPE_BILINEAR;
        filter.tAspectRatio.eMode = AX_IVPS_ASPECT_RATIO_STRETCH;
    }

    attr.nOutChnNum = static_cast<AX_U8>(max_chn_id + 1);

    return AX_IVPS_SetPipelineAttr(cfg_.grp_id, &attr) == AX_SUCCESS;
}

bool AxIvpsAdapter::EnableChannels()
{
    for (const auto& ch : cfg_.channels) {
        const AX_S32 ret = AX_IVPS_EnableChn(cfg_.grp_id, ch.chn_id);
        if (ret != AX_SUCCESS) {
            DisableChannels();
            return false;
        }
        enabled_channels_.push_back(ch.chn_id);
    }

    return true;
}

void AxIvpsAdapter::DisableChannels()
{
    for (auto it = enabled_channels_.rbegin(); it != enabled_channels_.rend(); ++it) {
        AX_IVPS_DisableChn(cfg_.grp_id, *it);
    }
    enabled_channels_.clear();
}

}  // namespace aov::media::ax615
