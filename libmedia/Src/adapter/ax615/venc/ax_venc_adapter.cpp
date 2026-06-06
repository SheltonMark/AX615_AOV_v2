#include "ax_venc_adapter.hpp"

#include <cstring>

namespace aov::media::ax615 {
namespace {

AX_U32 DefaultStreamBufSize(const VencChannelConfig& cfg)
{
    const AX_U32 raw_size = cfg.width * cfg.height * 3 / 2;
    return raw_size == 0 ? (1024 * 1024) : raw_size;
}

bool IsVideoCodec(AX_PAYLOAD_TYPE_E codec)
{
    return codec == PT_H264 || codec == PT_H265;
}

AX_PAYLOAD_TYPE_E ToPayload(VideoCodec codec)
{
    return codec == VideoCodec::H265 ? PT_H265 : PT_H264;
}

AX_VENC_RC_MODE_E ToRcMode(VideoCodec codec, RcMode rc_mode)
{
    if (codec == VideoCodec::H265) {
        switch (rc_mode) {
            case RcMode::VBR:
                return AX_VENC_RC_MODE_H265VBR;
            case RcMode::FIXQP:
                return AX_VENC_RC_MODE_H265FIXQP;
            case RcMode::CBR:
            default:
                return AX_VENC_RC_MODE_H265CBR;
        }
    }
    switch (rc_mode) {
        case RcMode::VBR:
            return AX_VENC_RC_MODE_H264VBR;
        case RcMode::FIXQP:
            return AX_VENC_RC_MODE_H264FIXQP;
        case RcMode::CBR:
        default:
            return AX_VENC_RC_MODE_H264CBR;
    }
}

VideoCodec ToPublicCodec(AX_PAYLOAD_TYPE_E codec)
{
    switch (codec) {
        case PT_H265:
            return VideoCodec::H265;
        case PT_JPEG:
            return VideoCodec::JPEG;
        case PT_H264:
        default:
            return VideoCodec::H264;
    }
}

bool IsKeyFrame(const AX_VENC_PACK_T& pack)
{
    return pack.enCodingType == AX_VENC_INTRA_FRAME || pack.enType == PT_JPEG;
}

void FillCommonCbr(AX_VENC_H264_CBR_T& cbr, const VencChannelConfig& cfg)
{
    std::memset(&cbr, 0, sizeof(cbr));
    cbr.u32Gop = cfg.gop;
    cbr.u32StatTime = 2;
    cbr.u32BitRate = cfg.bitrate_kbps;
    cbr.u32MinQp = 10;
    cbr.u32MaxQp = 51;
    cbr.u32MinIQp = 10;
    cbr.u32MaxIQp = 51;
    cbr.u32MaxIprop = 100;
    cbr.u32MinIprop = 1;
    cbr.s32IntraQpDelta = -2;
    cbr.s32DeBreathQpDelta = -2;
    cbr.u32IdrQpDeltaRange = 10;
    cbr.u32BlockRcSize = 2;
    cbr.s32MinFrameQp = 10;
    cbr.s32MaxFrameQp = 51;
    cbr.s32MinIFrameQp = 10;
    cbr.s32MaxIFrameQp = 51;
    cbr.s32CbQpOffset = -6;
    cbr.s32CrQpOffset = -6;
    cbr.u32MotionSen = 8;
    cbr.u32MotionAlpha = 128;
    cbr.u32StillIBitPercent = 0;
    cbr.bCtuBitsRefEn = AX_FALSE;
    cbr.s32CtuBitsRefQpDeltaRange = 3;
}

void FillCommonVbr(AX_VENC_H264_VBR_T& vbr, const VencChannelConfig& cfg)
{
    std::memset(&vbr, 0, sizeof(vbr));
    vbr.u32Gop = cfg.gop;
    vbr.u32StatTime = 1;
    vbr.u32MaxBitRate = cfg.bitrate_kbps;
    vbr.u32MinQp = 30;
    vbr.u32MaxQp = 51;
    vbr.u32MinIQp = 30;
    vbr.u32MaxIQp = 51;
    vbr.s32IntraQpDelta = -2;
    vbr.s32DeBreathQpDelta = -2;
    vbr.u32IdrQpDeltaRange = 10;
    vbr.u32ChangePos = 85;
    vbr.u32BlockRcSize = 2;
    vbr.s32MinFrameQp = 30;
    vbr.s32MaxFrameQp = 51;
    vbr.s32MinIFrameQp = 30;
    vbr.s32MaxIFrameQp = 51;
    vbr.s32CbQpOffset = -6;
    vbr.s32CrQpOffset = -6;
    vbr.u32MotionSen = 6;
    vbr.u32MotionAlpha = 32;
    vbr.u32StillIBitPercent = 25;
    vbr.bCtuBitsRefEn = AX_FALSE;
    vbr.s32CtuBitsRefQpDeltaRange = 3;
}

void FillCommonFixQp(AX_VENC_H264_FIXQP_T& fix_qp, const VencChannelConfig& cfg)
{
    std::memset(&fix_qp, 0, sizeof(fix_qp));
    fix_qp.u32Gop = cfg.gop;
    fix_qp.u32IQp = 25;
    fix_qp.u32PQp = 30;
    fix_qp.u32BQp = 32;
}

}  // namespace

AxVencAdapter::~AxVencAdapter()
{
    while (!channels_.empty()) {
        DestroyChannel(channels_.back()->cfg.chn_id);
    }
    MaybeModuleDeinit();
}

MediaStatusCode AxVencAdapter::CreateChannel(const VencChannelConfig& cfg)
{
    if (cfg.chn_id < 0 || cfg.chn_id >= AX_MAX_VENC_CHN_NUM || cfg.width == 0 || cfg.height == 0) {
        return MediaStatusCode::InvalidArgument;
    }
    const AX_PAYLOAD_TYPE_E payload = ToPayload(cfg.codec);
    if (!IsVideoCodec(payload)) {
        return MediaStatusCode::Unsupported;
    }
    ChannelContext* existing = FindChannel(cfg.chn_id);
    if (existing && existing->created) {
        return MediaStatusCode::Busy;
    }

    const MediaStatusCode init_status = EnsureModuleInit();
    if (init_status != MediaStatusCode::Ok) {
        return init_status;
    }

    AX_VENC_CHN_ATTR_T attr;
    if (!FillChannelAttr(cfg, attr)) {
        return MediaStatusCode::Unsupported;
    }

    const AX_S32 ret = AX_VENC_CreateChn(cfg.chn_id, &attr);
    if (ret != AX_SUCCESS) {
        return MediaStatusCode::InternalError;
    }

    if (existing) {
        existing->cfg = cfg;
        existing->created = true;
    } else {
        auto ctx = std::make_unique<ChannelContext>();
        ctx->cfg = cfg;
        ctx->created = true;
        channels_.push_back(std::move(ctx));
    }
    return MediaStatusCode::Ok;
}

void AxVencAdapter::DestroyChannel(VENC_CHN chn_id)
{
    for (auto it = channels_.begin(); it != channels_.end(); ++it) {
        if (!*it || (*it)->cfg.chn_id != chn_id) {
            continue;
        }

        Stop(chn_id);
    AX_VENC_DestroyChn(static_cast<VENC_CHN>(chn_id));
        channels_.erase(it);
        MaybeModuleDeinit();
        return;
    }
}

void AxVencAdapter::SetFrameCallback(VENC_CHN chn_id, VencFrameCallback cb)
{
    ChannelContext* ctx = FindChannel(chn_id);
    if (!ctx) {
        auto pending = std::make_unique<ChannelContext>();
        pending->cfg.chn_id = chn_id;
        pending->callback = std::move(cb);
        channels_.push_back(std::move(pending));
        return;
    }

    ctx->callback = std::move(cb);
}

MediaStatusCode AxVencAdapter::Start(VENC_CHN chn_id)
{
    ChannelContext* ctx = FindChannel(chn_id);
    if (!ctx || !ctx->created) {
        return MediaStatusCode::InvalidState;
    }
    if (ctx->running.load()) {
        return MediaStatusCode::Ok;
    }

    AX_VENC_RECV_PIC_PARAM_T recv_param;
    std::memset(&recv_param, 0, sizeof(recv_param));
    recv_param.s32RecvPicNum = -1;

    const AX_S32 ret = AX_VENC_StartRecvFrame(static_cast<VENC_CHN>(chn_id), &recv_param);
    if (ret != AX_SUCCESS) {
        return MediaStatusCode::InternalError;
    }

    ctx->receiving = true;
    ctx->running.store(true);
    ctx->stream_thread = std::thread([this, ctx]() {
        StreamLoop(*ctx);
    });
    return MediaStatusCode::Ok;
}

void AxVencAdapter::Stop(VENC_CHN chn_id)
{
    ChannelContext* ctx = FindChannel(chn_id);
    if (!ctx) {
        return;
    }

    const bool was_running = ctx->running.exchange(false);

    if (ctx->receiving) {
        AX_VENC_StopRecvFrame(static_cast<VENC_CHN>(chn_id));
        ctx->receiving = false;
    }

    if (was_running && ctx->stream_thread.joinable()) {
        ctx->stream_thread.join();
    }
}

MediaStatusCode AxVencAdapter::RequestKeyFrame(VENC_CHN chn_id)
{
    ChannelContext* ctx = FindChannel(chn_id);
    if (!ctx || !ctx->created) {
        return MediaStatusCode::InvalidState;
    }

    return AX_VENC_RequestIDR(static_cast<VENC_CHN>(chn_id), AX_TRUE) == AX_SUCCESS
        ? MediaStatusCode::Ok
        : MediaStatusCode::InternalError;
}

MediaStatusCode AxVencAdapter::SetBitrate(VENC_CHN chn_id, AX_U32 kbps)
{
    ChannelContext* ctx = FindChannel(chn_id);
    if (!ctx || !ctx->created || kbps == 0) {
        return MediaStatusCode::InvalidState;
    }

    AX_VENC_RC_PARAM_T rc_param;
    std::memset(&rc_param, 0, sizeof(rc_param));
    AX_S32 ret = AX_VENC_GetRcParam(static_cast<VENC_CHN>(chn_id), &rc_param);
    if (ret != AX_SUCCESS) {
        return MediaStatusCode::InternalError;
    }

    switch (rc_param.enRcMode) {
        case AX_VENC_RC_MODE_H264CBR:
            rc_param.stH264Cbr.u32BitRate = kbps;
            break;
        case AX_VENC_RC_MODE_H264VBR:
            rc_param.stH264Vbr.u32MaxBitRate = kbps;
            break;
        case AX_VENC_RC_MODE_H265CBR:
            rc_param.stH265Cbr.u32BitRate = kbps;
            break;
        case AX_VENC_RC_MODE_H265VBR:
            rc_param.stH265Vbr.u32MaxBitRate = kbps;
            break;
        default:
            return MediaStatusCode::Unsupported;
    }

    ret = AX_VENC_SetRcParam(static_cast<VENC_CHN>(chn_id), &rc_param);
    if (ret != AX_SUCCESS) {
        return MediaStatusCode::InternalError;
    }

    ctx->cfg.bitrate_kbps = kbps;
    return MediaStatusCode::Ok;
}

bool AxVencAdapter::IsCreated(VENC_CHN chn_id) const
{
    const ChannelContext* ctx = FindChannel(chn_id);
    return ctx && ctx->created;
}

bool AxVencAdapter::IsRunning(VENC_CHN chn_id) const
{
    const ChannelContext* ctx = FindChannel(chn_id);
    return ctx && ctx->running.load();
}

MediaStatusCode AxVencAdapter::EnsureModuleInit()
{
    if (module_initialized_) {
        return MediaStatusCode::Ok;
    }

    AX_VENC_MOD_ATTR_T mod_attr;
    std::memset(&mod_attr, 0, sizeof(mod_attr));
    mod_attr.enVencType = AX_VENC_MULTI_ENCODER;
    mod_attr.stModThdAttr.u32TotalThreadNum = 3;
    mod_attr.stModThdAttr.bExplicitSched = AX_FALSE;

    const AX_S32 ret = AX_VENC_Init(&mod_attr);
    if (ret != AX_SUCCESS) {
        return MediaStatusCode::InternalError;
    }

    module_initialized_ = true;
    return MediaStatusCode::Ok;
}

void AxVencAdapter::MaybeModuleDeinit()
{
    if (!module_initialized_) {
        return;
    }
    for (const auto& ctx : channels_) {
        if (ctx && ctx->created) {
            return;
        }
    }

    AX_VENC_Deinit();
    module_initialized_ = false;
}

AxVencAdapter::ChannelContext* AxVencAdapter::FindChannel(VENC_CHN chn_id)
{
    for (auto& ctx : channels_) {
        if (ctx && ctx->cfg.chn_id == chn_id) {
            return ctx.get();
        }
    }
    return nullptr;
}

const AxVencAdapter::ChannelContext* AxVencAdapter::FindChannel(VENC_CHN chn_id) const
{
    for (const auto& ctx : channels_) {
        if (ctx && ctx->cfg.chn_id == chn_id) {
            return ctx.get();
        }
    }
    return nullptr;
}

bool AxVencAdapter::FillChannelAttr(const VencChannelConfig& cfg, AX_VENC_CHN_ATTR_T& attr) const
{
    std::memset(&attr, 0, sizeof(attr));

    const AX_PAYLOAD_TYPE_E payload = ToPayload(cfg.codec);
    const AX_VENC_RC_MODE_E rc_mode = ToRcMode(cfg.codec, cfg.rc_mode);
    attr.stVencAttr.enType = payload;
    attr.stVencAttr.u32MaxPicWidth = cfg.width;
    attr.stVencAttr.u32MaxPicHeight = cfg.height;
    attr.stVencAttr.u32PicWidthSrc = cfg.width;
    attr.stVencAttr.u32PicHeightSrc = cfg.height;
    attr.stVencAttr.u32BufSize = cfg.stream_buf_size == 0 ? DefaultStreamBufSize(cfg) : cfg.stream_buf_size;
    attr.stVencAttr.enMemSource = cfg.mem_source;
    attr.stVencAttr.enLinkMode = cfg.link_mode;
    attr.stVencAttr.u8InFifoDepth = cfg.in_fifo_depth;
    attr.stVencAttr.u8OutFifoDepth = cfg.out_fifo_depth;
    attr.stVencAttr.s32StopWaitTime = -1;

    attr.stRcAttr.s32FirstFrameStartQp = -1;
    attr.stRcAttr.stFrameRate.fSrcFrameRate = static_cast<AX_F32>(cfg.fps);
    attr.stRcAttr.stFrameRate.fDstFrameRate = static_cast<AX_F32>(cfg.fps);
    attr.stGopAttr.enGopMode = AX_VENC_GOPMODE_NORMALP;

    switch (payload) {
        case PT_H264:
            attr.stVencAttr.enProfile = AX_VENC_H264_HIGH_PROFILE;
            attr.stVencAttr.enLevel = AX_VENC_H264_LEVEL_5_2;
            switch (rc_mode) {
                case AX_VENC_RC_MODE_H264CBR:
                    attr.stRcAttr.enRcMode = AX_VENC_RC_MODE_H264CBR;
                    FillCommonCbr(attr.stRcAttr.stH264Cbr, cfg);
                    return true;
                case AX_VENC_RC_MODE_H264VBR:
                    attr.stRcAttr.enRcMode = AX_VENC_RC_MODE_H264VBR;
                    FillCommonVbr(attr.stRcAttr.stH264Vbr, cfg);
                    return true;
                case AX_VENC_RC_MODE_H264FIXQP:
                    attr.stRcAttr.enRcMode = AX_VENC_RC_MODE_H264FIXQP;
                    FillCommonFixQp(attr.stRcAttr.stH264FixQp, cfg);
                    return true;
                default:
                    return false;
            }
        case PT_H265:
            attr.stVencAttr.enProfile = AX_VENC_HEVC_MAIN_PROFILE;
            attr.stVencAttr.enLevel = AX_VENC_HEVC_LEVEL_6;
            attr.stVencAttr.enTier = AX_VENC_HEVC_MAIN_TIER;
            switch (rc_mode) {
                case AX_VENC_RC_MODE_H265CBR:
                    attr.stRcAttr.enRcMode = AX_VENC_RC_MODE_H265CBR;
                    FillCommonCbr(attr.stRcAttr.stH265Cbr, cfg);
                    return true;
                case AX_VENC_RC_MODE_H265VBR:
                    attr.stRcAttr.enRcMode = AX_VENC_RC_MODE_H265VBR;
                    FillCommonVbr(attr.stRcAttr.stH265Vbr, cfg);
                    return true;
                case AX_VENC_RC_MODE_H265FIXQP:
                    attr.stRcAttr.enRcMode = AX_VENC_RC_MODE_H265FIXQP;
                    FillCommonFixQp(attr.stRcAttr.stH265FixQp, cfg);
                    return true;
                default:
                    return false;
            }
        default:
            return false;
    }
}

StreamFrame AxVencAdapter::ToStreamFrame(const ChannelContext& ctx, const AX_VENC_STREAM_T& stream) const
{
    StreamFrame frame;
    frame.chn_id = ctx.cfg.chn_id;
    frame.codec = ToPublicCodec(stream.stPack.enType);
    frame.data = stream.stPack.pu8Addr;
    frame.size = stream.stPack.u32Len;
    frame.pts_ms = stream.stPack.u64PTS / 1000;
    frame.sequence = stream.stPack.u64SeqNum;
    frame.is_keyframe = IsKeyFrame(stream.stPack);
    return frame;
}

void AxVencAdapter::StreamLoop(ChannelContext& ctx)
{
    while (ctx.running.load()) {
        AX_VENC_STREAM_T stream;
        std::memset(&stream, 0, sizeof(stream));

        const AX_S32 ret = AX_VENC_GetStream(static_cast<VENC_CHN>(ctx.cfg.chn_id), &stream, ctx.cfg.get_stream_timeout_ms);
        if (ret == AX_SUCCESS) {
            if (stream.stPack.pu8Addr && stream.stPack.u32Len > 0 && ctx.callback) {
                const StreamFrame frame = ToStreamFrame(ctx, stream);
                ctx.callback(frame);
            }
            AX_VENC_ReleaseStream(static_cast<VENC_CHN>(ctx.cfg.chn_id), &stream);
        } else if (ret == AX_ERR_VENC_FLOW_END) {
            ctx.running.store(false);
        }
    }
}

}  // namespace aov::media::ax615
