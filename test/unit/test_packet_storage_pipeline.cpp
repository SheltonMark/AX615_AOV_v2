#include <cstdint>
#include <iostream>

#include "../../app/storage/packet_storage_pipeline.hpp"

namespace {

int Fail(const char* message) {
    std::cerr << message << "\n";
    return 1;
}

} // namespace

int main() {
    aov::app::storage::PacketStoragePipeline pipeline;

    aov::app::packet::PacketConfig packet_config;
    packet_config.max_frames = 8;
    packet_config.max_bytes = 1024 * 1024;

    aov::app::storage::StorageConfig storage_config;
    storage_config.mount_point = "/tmp";
    storage_config.record_dir = "aov_storage_smoke";

    auto result = pipeline.Init(packet_config, storage_config);
    if (!result.ok()) {
        return Fail(result.message.c_str());
    }

    const std::uint8_t payload[] = {
        0x00, 0x00, 0x00, 0x01, 0x65, 0x88, 0x84, 0x21
    };

    aov::media::StreamFrame frame;
    frame.chn_id = 0;
    frame.codec = aov::media::VideoCodec::H264;
    frame.data = payload;
    frame.size = sizeof(payload);
    frame.pts_ms = 1234;
    frame.sequence = 1;
    frame.is_keyframe = true;

    const auto packet_result = pipeline.router.OnMediaFrame(frame);
    if (!packet_result.ok()) {
        return Fail(packet_result.message.c_str());
    }

    const auto storage_stats = pipeline.storage_service.GetStats();
    if (storage_stats.received_frames != 1) {
        return Fail("storage did not receive exactly one frame");
    }
    if (storage_stats.written_frames != 1) {
        return Fail("storage did not write exactly one frame");
    }
    if (storage_stats.written_bytes != sizeof(payload)) {
        return Fail("storage written byte count mismatch");
    }

    const auto stop_result = pipeline.storage_service.FlushAndClose();
    if (!stop_result.ok()) {
        return Fail(stop_result.message.c_str());
    }

    const auto final_stats = pipeline.storage_service.GetStats();
    if (!final_stats.flushed || final_stats.recording) {
        return Fail("storage did not flush and stop recording");
    }

    pipeline.Deinit();
    std::cout << "packet storage pipeline smoke passed\n";
    return 0;
}
