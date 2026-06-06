#pragma once

#include <cstdint>
#include <string>

#include "app/cloud/tencent/tencent_cloud_storage.hpp"
#include "app/cloud/tencent/tencent_live_stream.hpp"
#include "app/cloud/tencent/tencent_stream_adapter.hpp"
#include "app/packet/media_packet_router.hpp"

namespace aov::app::cloud {

struct CloudPacketSinkStats {
    std::uint64_t received_frames {0};
    std::uint64_t converted_video_frames {0};
    std::uint64_t dropped_frames {0};
    iv_cm_venc_pack_s last_video_pack {};
};

struct CloudPacketSinkResult {
    bool success {true};
    std::string message;

    static CloudPacketSinkResult Ok() {
        return {};
    }

    static CloudPacketSinkResult Error(std::string reason) {
        CloudPacketSinkResult result;
        result.success = false;
        result.message = std::move(reason);
        return result;
    }

    bool ok() const {
        return success;
    }
};

class CloudPacketSink final {
public:
    CloudPacketSink() = default;
    explicit CloudPacketSink(tencent::TencentLiveStream* live_stream);
    CloudPacketSink(tencent::TencentLiveStream* live_stream,
                    tencent::TencentCloudStorage* cloud_storage);

    CloudPacketSinkResult OnPacketFrame(const aov::app::packet::PacketFrame& frame);
    CloudPacketSinkStats GetStats() const;
    void Reset();

private:
    tencent::TencentStreamAdapter stream_adapter_ {};
    tencent::TencentLiveStream* live_stream_ {nullptr};
    tencent::TencentCloudStorage* cloud_storage_ {nullptr};
    CloudPacketSinkStats stats_ {};
};

CloudPacketSinkResult BindCloudToPacket(aov::app::packet::MediaPacketRouter& router,
                                        CloudPacketSink& sink);

} // namespace aov::app::cloud
