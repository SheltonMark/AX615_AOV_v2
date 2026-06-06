#pragma once

#include "common/media_types.hpp"

#include <atomic>
#include <functional>
#include <memory>
#include <thread>
#include <vector>

extern "C" {
#include "ax_venc_api.h"
}

namespace aov::media::ax615 {

struct VencChannelConfig {
    int chn_id = 0;
    uint32_t width = 1920;
    uint32_t height = 1080;
    uint32_t fps = 25;
    uint32_t bitrate_kbps = 4096;
    uint32_t gop = 50;
    VideoCodec codec = VideoCodec::H264;
    RcMode rc_mode = RcMode::CBR;
    AX_LINK_MODE_E link_mode = AX_LINK_MODE;
    AX_MEMORY_SOURCE_E mem_source = AX_MEMORY_SOURCE_CMM;
    int in_fifo_depth = 1;
    int out_fifo_depth = 4;
    uint32_t stream_buf_size = 0;
    int get_stream_timeout_ms = 500;
};

using VencFrameCallback = std::function<void(const StreamFrame& frame)>;

class AxVencAdapter {
public:
    AxVencAdapter() = default;
    ~AxVencAdapter();

    AxVencAdapter(const AxVencAdapter&) = delete;
    AxVencAdapter& operator=(const AxVencAdapter&) = delete;

    MediaStatusCode CreateChannel(const VencChannelConfig& cfg);
    void DestroyChannel(VENC_CHN chn_id);

    void SetFrameCallback(VENC_CHN chn_id, VencFrameCallback cb);

    MediaStatusCode Start(VENC_CHN chn_id);
    void Stop(VENC_CHN chn_id);

    MediaStatusCode RequestKeyFrame(VENC_CHN chn_id);
    MediaStatusCode SetBitrate(VENC_CHN chn_id, AX_U32 kbps);

    bool IsCreated(VENC_CHN chn_id) const;
    bool IsRunning(VENC_CHN chn_id) const;

private:
    struct ChannelContext {
        VencChannelConfig cfg;
        VencFrameCallback callback;
        std::atomic_bool running {false};
        bool created = false;
        bool receiving = false;
        std::thread stream_thread;
    };

    MediaStatusCode EnsureModuleInit();
    void MaybeModuleDeinit();
    ChannelContext* FindChannel(VENC_CHN chn_id);
    const ChannelContext* FindChannel(VENC_CHN chn_id) const;
    bool FillChannelAttr(const VencChannelConfig& cfg, AX_VENC_CHN_ATTR_T& attr) const;
    StreamFrame ToStreamFrame(const ChannelContext& ctx, const AX_VENC_STREAM_T& stream) const;
    void StreamLoop(ChannelContext& ctx);

    bool module_initialized_ = false;
    std::vector<std::unique_ptr<ChannelContext>> channels_;
};

}  // namespace aov::media::ax615
