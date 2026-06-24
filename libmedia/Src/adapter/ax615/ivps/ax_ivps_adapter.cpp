#include "ax_ivps_adapter.hpp"

#include <algorithm>
#include <cstdio>
#include <cstring>

extern "C" {
#include "ax_pool_type.h"
#include "ax_sys_api.h"
}

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
        std::fprintf(stderr, "[AxIvpsAdapter][v20260611] Invalid channel count: %zu\n", cfg.channels.size());
        return false;
    }

    cfg_ = cfg;

    std::fprintf(stderr, "[AxIvpsAdapter][v20260611] Calling AX_IVPS_Init\n");
    AX_S32 ret = AX_IVPS_Init();
    if (ret != AX_SUCCESS) {
        std::fprintf(stderr, "[AxIvpsAdapter][v20260611] AX_IVPS_Init failed: 0x%x\n", ret);
        return false;
    }
    ivps_initialized_ = true;

    std::fprintf(stderr, "[AxIvpsAdapter][v20260611] Creating IVPS group %d\n", cfg_.grp_id);
    AX_IVPS_GRP_ATTR_T grp_attr;
    std::memset(&grp_attr, 0, sizeof(grp_attr));
    grp_attr.nInFifoDepth = cfg_.in_fifo_depth;
    grp_attr.ePipeline = AX_IVPS_PIPELINE_DEFAULT;

    ret = AX_IVPS_CreateGrp(cfg_.grp_id, &grp_attr);
    if (ret != AX_SUCCESS) {
        std::fprintf(stderr, "[AxIvpsAdapter][v20260611] AX_IVPS_CreateGrp failed: 0x%x\n", ret);
        Close();
        return false;
    }
    group_created_ = true;

    std::fprintf(stderr, "[AxIvpsAdapter][v20260611] Setting pipeline attributes\n");
    if (!SetPipelineAttr()) {
        std::fprintf(stderr, "[AxIvpsAdapter][v20260611] SetPipelineAttr failed\n");
        Close();
        return false;
    }

    // NOTE: QSDemo and official samples do NOT call AX_IVPS_EnableChn
    // Channels are enabled automatically when AX_IVPS_StartGrp is called
    std::fprintf(stderr, "[AxIvpsAdapter][v20260611] Skipping EnableChannels (not needed)\n");

    opened_ = true;
    std::fprintf(stderr, "[AxIvpsAdapter][v20260611] IVPS Open completed successfully\n");
    return true;
}

bool AxIvpsAdapter::Start()
{
    if (!opened_ || running_) {
        return opened_ && running_;
    }

    std::fprintf(stderr, "[AxIvpsAdapter][v20260611] Starting IVPS group %d\n", cfg_.grp_id);
    const AX_S32 ret = AX_IVPS_StartGrp(cfg_.grp_id);
    if (ret != AX_SUCCESS) {
        std::fprintf(stderr, "[AxIvpsAdapter] AX_IVPS_StartGrp(%d) failed: 0x%x\n", cfg_.grp_id, ret);
        return false;
    }

    // Verify IVPS is actually running by reading back the pipeline attr
    AX_IVPS_PIPELINE_ATTR_T verify_attr;
    std::memset(&verify_attr, 0, sizeof(verify_attr));
    AX_S32 verify_ret = AX_IVPS_GetPipelineAttr(cfg_.grp_id, &verify_attr);
    if (verify_ret == AX_SUCCESS) {
        std::fprintf(stderr, "[AxIvpsAdapter][DEBUG] IVPS verification: nOutChnNum=%d, eFRCMode=%d\n",
                     verify_attr.nOutChnNum, verify_attr.eFRCMode);
        std::fprintf(stderr, "[AxIvpsAdapter][DEBUG]   tFilter[0][0].bEngage=%d, eEngine=%d, %dx%d\n",
                     verify_attr.tFilter[0][0].bEngage, verify_attr.tFilter[0][0].eEngine,
                     verify_attr.tFilter[0][0].nDstPicWidth, verify_attr.tFilter[0][0].nDstPicHeight);
        std::fprintf(stderr, "[AxIvpsAdapter][DEBUG]   tFilter[1][0].bEngage=%d, eEngine=%d, %dx%d\n",
                     verify_attr.tFilter[1][0].bEngage, verify_attr.tFilter[1][0].eEngine,
                     verify_attr.tFilter[1][0].nDstPicWidth, verify_attr.tFilter[1][0].nDstPicHeight);
        for (int i = 0; i < verify_attr.nOutChnNum; i++) {
            std::fprintf(stderr, "[AxIvpsAdapter][DEBUG]   nOutFifoDepth[%d]=%d\n", i, verify_attr.nOutFifoDepth[i]);
        }
    } else {
        std::fprintf(stderr, "[AxIvpsAdapter][ERROR] AX_IVPS_GetPipelineAttr failed: 0x%x\n", verify_ret);
    }

    running_ = true;
    std::fprintf(stderr, "[AxIvpsAdapter][v20260611] IVPS group %d started successfully\n", cfg_.grp_id);
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

    // CRITICAL: Configure group filter (tFilter[0][0]) to receive VIN input
    // SDK sample uses [0][0], NOT [0][1]! This was the bug!
    std::fprintf(stderr, "[AxIvpsAdapter][v20260611] Configuring group filter (tFilter[0][0])\n");
    AX_IVPS_FILTER_T& group_filter = attr.tFilter[0][0];  // CRITICAL: [0][0] not [0][1]!
    group_filter.bEngage = AX_TRUE;
    group_filter.eEngine = AX_IVPS_ENGINE_GDC;  // Use GDC engine (SDK sample uses GDC)
    group_filter.nDstPicWidth = 2560;   // Match VIN output
    group_filter.nDstPicHeight = 1440;
    group_filter.nDstPicStride = 2560;
    group_filter.eDstPicFormat = AX_FORMAT_YUV420_SEMIPLANAR;
    group_filter.bCrop = AX_FALSE;
    std::fprintf(stderr, "[AxIvpsAdapter][v20260611] Group filter: %dx%d, engine=%d\n",
                 group_filter.nDstPicWidth, group_filter.nDstPicHeight, group_filter.eEngine);

    AX_U8 max_chn_id = 0;
    for (const auto& ch : cfg_.channels) {
        if (ch.chn_id < 0 || ch.chn_id >= kMaxOutChannels) {
            std::fprintf(stderr, "[AxIvpsAdapter][v20260611] Invalid channel id: %d\n", ch.chn_id);
            return false;
        }

        std::fprintf(stderr, "[AxIvpsAdapter][v20260611] Configuring channel %d: %dx%d, stride=%d, fps=%d->%d\n",
                     ch.chn_id, ch.width, ch.height, EffectiveStride(ch), ch.src_fps, ch.dst_fps);

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
    std::fprintf(stderr, "[AxIvpsAdapter][v20260611] Calling AX_IVPS_SetPipelineAttr, nOutChnNum=%d\n", attr.nOutChnNum);

    AX_S32 ret = AX_IVPS_SetPipelineAttr(cfg_.grp_id, &attr);
    if (ret != AX_SUCCESS) {
        std::fprintf(stderr, "[AxIvpsAdapter][v20260611] AX_IVPS_SetPipelineAttr failed: 0x%x\n", ret);
        return false;
    }
    return true;
}

bool AxIvpsAdapter::EnableChannels()
{
    for (const auto& ch : cfg_.channels) {
        std::fprintf(stderr, "[AxIvpsAdapter][v20260611] Enabling channel %d\n", ch.chn_id);
        const AX_S32 ret = AX_IVPS_EnableChn(cfg_.grp_id, ch.chn_id);
        if (ret != AX_SUCCESS) {
            std::fprintf(stderr, "[AxIvpsAdapter][v20260611] AX_IVPS_EnableChn(grp=%d, chn=%d) failed: 0x%x\n",
                         cfg_.grp_id, ch.chn_id, ret);
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

bool AxIvpsAdapter::GetChnFrame(int chn_id, AX_VIDEO_FRAME_T& frame, int timeout_ms)
{
    if (!running_) {
        return false;
    }

    AX_S32 ret = AX_IVPS_GetChnFrame(cfg_.grp_id, chn_id, &frame, timeout_ms);
    if (ret != AX_SUCCESS) {
        return false;
    }

    // 填充虚拟地址和物理地址
    frame.u64VirAddr[0] = (AX_ULONG)AX_POOL_GetBlockVirAddr(frame.u32BlkId[0]);
    frame.u64PhyAddr[0] = AX_POOL_Handle2PhysAddr(frame.u32BlkId[0]);
    frame.u32FrameSize = frame.u32PicStride[0] * frame.u32Height * 3 / 2;

    return true;
}

bool AxIvpsAdapter::ReleaseChnFrame(int chn_id, const AX_VIDEO_FRAME_T& frame)
{
    if (!running_) {
        return false;
    }

    // 需要 const_cast，因为 SDK 接口不是 const
    AX_VIDEO_FRAME_T* mutable_frame = const_cast<AX_VIDEO_FRAME_T*>(&frame);
    AX_S32 ret = AX_IVPS_ReleaseChnFrame(cfg_.grp_id, chn_id, mutable_frame);
    return ret == AX_SUCCESS;
}

}  // namespace aov::media::ax615
