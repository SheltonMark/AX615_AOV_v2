#include "ax_jenc_adapter.hpp"

#include <cstring>

namespace aov::media::ax615 {
namespace {

AX_U32 DefaultJpegBufSize(const JencChannelConfig& cfg)
{
    const AX_U32 raw_size = cfg.width * cfg.height * 3 / 2;
    return raw_size == 0 ? (1024 * 1024) : raw_size;
}

}  // namespace

AxJencAdapter::~AxJencAdapter()
{
    CloseAll();
}

MediaStatusCode AxJencAdapter::Open(const JencChannelConfig& cfg)
{
    if (cfg.chn_id < 0 || cfg.chn_id >= AX_MAX_VENC_CHN_NUM || cfg.width == 0 || cfg.height == 0) {
        return MediaStatusCode::InvalidArgument;
    }
    if (cfg.quality < 1 || cfg.quality > 99) {
        return MediaStatusCode::InvalidArgument;
    }

    ChannelContext* existing = FindChannel(cfg.chn_id);
    if (existing && existing->opened) {
        return MediaStatusCode::Busy;
    }

    const MediaStatusCode init_status = EnsureModuleInit();
    if (init_status != MediaStatusCode::Ok) {
        return init_status;
    }

    AX_VENC_CHN_ATTR_T attr;
    if (!FillChannelAttr(cfg, attr)) {
        return MediaStatusCode::InvalidArgument;
    }

    AX_S32 ret = AX_VENC_CreateChn(cfg.chn_id, &attr);
    if (ret != AX_SUCCESS) {
        return MediaStatusCode::InternalError;
    }

    AX_VENC_JPEG_PARAM_T jpeg_param;
    std::memset(&jpeg_param, 0, sizeof(jpeg_param));
    ret = AX_VENC_GetJpegParam(cfg.chn_id, &jpeg_param);
    if (ret != AX_SUCCESS) {
        AX_VENC_DestroyChn(cfg.chn_id);
        return MediaStatusCode::InternalError;
    }

    jpeg_param.u32Qfactor = cfg.quality;
    ret = AX_VENC_SetJpegParam(cfg.chn_id, &jpeg_param);
    if (ret != AX_SUCCESS) {
        AX_VENC_DestroyChn(cfg.chn_id);
        return MediaStatusCode::InternalError;
    }

    if (existing) {
        existing->cfg = cfg;
        existing->opened = true;
    } else {
        auto ctx = std::make_unique<ChannelContext>();
        ctx->cfg = cfg;
        ctx->opened = true;
        channels_.push_back(std::move(ctx));
    }

    return MediaStatusCode::Ok;
}

void AxJencAdapter::Close(VENC_CHN chn_id)
{
    for (auto it = channels_.begin(); it != channels_.end(); ++it) {
        if (!*it || (*it)->cfg.chn_id != chn_id) {
            continue;
        }

        if ((*it)->opened) {
            AX_VENC_StopRecvFrame(chn_id);
            AX_VENC_DestroyChn(chn_id);
        }
        channels_.erase(it);
        MaybeModuleDeinit();
        return;
    }
}

void AxJencAdapter::CloseAll()
{
    while (!channels_.empty()) {
        Close(channels_.back()->cfg.chn_id);
    }
    MaybeModuleDeinit();
}

void AxJencAdapter::SetFrameCallback(VENC_CHN chn_id, JencFrameCallback cb)
{
    ChannelContext* ctx = FindChannel(chn_id);
    if (ctx) {
        ctx->callback = std::move(cb);
        return;
    }

    auto pending = std::make_unique<ChannelContext>();
    pending->cfg.chn_id = chn_id;
    pending->callback = std::move(cb);
    channels_.push_back(std::move(pending));
}

MediaStatusCode AxJencAdapter::CaptureOnce(VENC_CHN chn_id)
{
    ChannelContext* ctx = FindChannel(chn_id);
    if (!ctx || !ctx->opened) {
        return MediaStatusCode::InvalidState;
    }

    AX_VENC_RECV_PIC_PARAM_T recv_param;
    std::memset(&recv_param, 0, sizeof(recv_param));
    recv_param.s32RecvPicNum = 1;

    AX_S32 ret = AX_VENC_StartRecvFrame(chn_id, &recv_param);
    if (ret != AX_SUCCESS) {
        return MediaStatusCode::InternalError;
    }

    AX_VENC_STREAM_T stream;
    std::memset(&stream, 0, sizeof(stream));
    ret = AX_VENC_GetStream(chn_id, &stream, ctx->cfg.get_stream_timeout_ms);
    if (ret != AX_SUCCESS) {
        AX_VENC_StopRecvFrame(chn_id);
        return ret == AX_ERR_VENC_TIMEOUT ? MediaStatusCode::Timeout : MediaStatusCode::InternalError;
    }

    if (stream.stPack.pu8Addr && stream.stPack.u32Len > 0 && ctx->callback) {
        const StreamFrame frame = ToStreamFrame(*ctx, stream);
        ctx->callback(frame);
    }

    const AX_S32 release_ret = AX_VENC_ReleaseStream(chn_id, &stream);
    AX_VENC_StopRecvFrame(chn_id);
    return release_ret == AX_SUCCESS ? MediaStatusCode::Ok : MediaStatusCode::InternalError;
}

MediaStatusCode AxJencAdapter::SetQuality(VENC_CHN chn_id, AX_U32 quality)
{
    ChannelContext* ctx = FindChannel(chn_id);
    if (!ctx || !ctx->opened) {
        return MediaStatusCode::InvalidState;
    }
    if (quality < 1 || quality > 99) {
        return MediaStatusCode::InvalidArgument;
    }

    AX_VENC_JPEG_PARAM_T jpeg_param;
    std::memset(&jpeg_param, 0, sizeof(jpeg_param));
    AX_S32 ret = AX_VENC_GetJpegParam(chn_id, &jpeg_param);
    if (ret != AX_SUCCESS) {
        return MediaStatusCode::InternalError;
    }

    jpeg_param.u32Qfactor = quality;
    ret = AX_VENC_SetJpegParam(chn_id, &jpeg_param);
    if (ret != AX_SUCCESS) {
        return MediaStatusCode::InternalError;
    }

    ctx->cfg.quality = quality;
    return MediaStatusCode::Ok;
}

bool AxJencAdapter::IsOpen(VENC_CHN chn_id) const
{
    const ChannelContext* ctx = FindChannel(chn_id);
    return ctx && ctx->opened;
}

MediaStatusCode AxJencAdapter::EnsureModuleInit()
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

void AxJencAdapter::MaybeModuleDeinit()
{
    if (!module_initialized_) {
        return;
    }
    for (const auto& ctx : channels_) {
        if (ctx && ctx->opened) {
            return;
        }
    }

    AX_VENC_Deinit();
    module_initialized_ = false;
}

AxJencAdapter::ChannelContext* AxJencAdapter::FindChannel(VENC_CHN chn_id)
{
    for (auto& ctx : channels_) {
        if (ctx && ctx->cfg.chn_id == chn_id) {
            return ctx.get();
        }
    }
    return nullptr;
}

const AxJencAdapter::ChannelContext* AxJencAdapter::FindChannel(VENC_CHN chn_id) const
{
    for (const auto& ctx : channels_) {
        if (ctx && ctx->cfg.chn_id == chn_id) {
            return ctx.get();
        }
    }
    return nullptr;
}

bool AxJencAdapter::FillChannelAttr(const JencChannelConfig& cfg, AX_VENC_CHN_ATTR_T& attr) const
{
    std::memset(&attr, 0, sizeof(attr));

    attr.stVencAttr.enType = PT_JPEG;
    attr.stVencAttr.u32MaxPicWidth = cfg.width;
    attr.stVencAttr.u32MaxPicHeight = cfg.height;
    attr.stVencAttr.u32PicWidthSrc = cfg.width;
    attr.stVencAttr.u32PicHeightSrc = cfg.height;
    attr.stVencAttr.u32BufSize = cfg.stream_buf_size == 0 ? DefaultJpegBufSize(cfg) : cfg.stream_buf_size;
    attr.stVencAttr.enMemSource = cfg.mem_source;
    attr.stVencAttr.enLinkMode = cfg.link_mode;
    attr.stVencAttr.u8InFifoDepth = cfg.in_fifo_depth;
    attr.stVencAttr.u8OutFifoDepth = cfg.out_fifo_depth;
    attr.stVencAttr.s32StopWaitTime = -1;
    attr.stRcAttr.stFrameRate.fSrcFrameRate = cfg.sensor_fps;
    attr.stRcAttr.stFrameRate.fDstFrameRate = cfg.capture_fps;
    return true;
}

StreamFrame AxJencAdapter::ToStreamFrame(const ChannelContext& ctx, const AX_VENC_STREAM_T& stream) const
{
    StreamFrame frame;
    frame.chn_id = ctx.cfg.chn_id;
    frame.codec = VideoCodec::JPEG;
    frame.data = stream.stPack.pu8Addr;
    frame.size = stream.stPack.u32Len;
    frame.pts_ms = stream.stPack.u64PTS / 1000;
    frame.sequence = stream.stPack.u64SeqNum;
    frame.is_keyframe = true;
    return frame;
}

}  // namespace aov::media::ax615
