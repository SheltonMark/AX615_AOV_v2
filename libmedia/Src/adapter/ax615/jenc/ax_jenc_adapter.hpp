#pragma once

#include "common/media_types.hpp"

#include <functional>
#include <memory>
#include <vector>

extern "C" {
#include "ax_venc_api.h"
}

namespace aov::media::ax615 {

struct JencChannelConfig {
    int chn_id = 8;
    int width = 1920;
    int height = 1080;
    float sensor_fps = 15.0F;
    float capture_fps = 1.0F;
    int quality = 60;
    AX_LINK_MODE_E link_mode = AX_LINK_MODE;
    AX_MEMORY_SOURCE_E mem_source = AX_MEMORY_SOURCE_CMM;
    int in_fifo_depth = 1;
    int out_fifo_depth = 1;
    uint32_t stream_buf_size = 0;
    int get_stream_timeout_ms = 200;
};

using JencFrameCallback = std::function<void(const StreamFrame& frame)>;

class AxJencAdapter {
public:
    AxJencAdapter() = default;
    ~AxJencAdapter();

    AxJencAdapter(const AxJencAdapter&) = delete;
    AxJencAdapter& operator=(const AxJencAdapter&) = delete;

    MediaStatusCode Open(const JencChannelConfig& cfg);
    void Close(VENC_CHN chn_id);
    void CloseAll();

    void SetFrameCallback(VENC_CHN chn_id, JencFrameCallback cb);
    MediaStatusCode CaptureOnce(VENC_CHN chn_id);
    MediaStatusCode SetQuality(VENC_CHN chn_id, AX_U32 quality);

    bool IsOpen(VENC_CHN chn_id) const;

private:
    struct ChannelContext {
        JencChannelConfig cfg;
        JencFrameCallback callback;
        bool opened = false;
    };

    MediaStatusCode EnsureModuleInit();
    void MaybeModuleDeinit();
    ChannelContext* FindChannel(VENC_CHN chn_id);
    const ChannelContext* FindChannel(VENC_CHN chn_id) const;
    bool FillChannelAttr(const JencChannelConfig& cfg, AX_VENC_CHN_ATTR_T& attr) const;
    StreamFrame ToStreamFrame(const ChannelContext& ctx, const AX_VENC_STREAM_T& stream) const;

    bool module_initialized_ = false;
    std::vector<std::unique_ptr<ChannelContext>> channels_;
};

}  // namespace aov::media::ax615
