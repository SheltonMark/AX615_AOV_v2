#include <cstdint>
#include <iostream>
#include <vector>

#include "../../app/packet/packet_service.hpp"

namespace {

int Fail(const char* message) {
    std::cerr << message << "\n";
    return 1;
}

aov::media::StreamFrame MakeFrame(const std::uint64_t sequence,
                                  const bool keyframe,
                                  const std::uint32_t payload_size = 16) {
    static std::uint8_t payload[4096] {};
    payload[0] = 0x00;
    payload[1] = 0x00;
    payload[2] = 0x00;
    payload[3] = 0x01;
    payload[4] = keyframe ? 0x65 : 0x41;

    aov::media::StreamFrame frame;
    frame.chn_id = 0;
    frame.codec = aov::media::VideoCodec::H264;
    frame.data = payload;
    frame.size = payload_size;
    frame.pts_ms = sequence * 40;
    frame.sequence = sequence;
    frame.is_keyframe = keyframe;
    return frame;
}

} // namespace

int main() {
    using namespace aov::app::packet;

    PacketConfig config;
    config.max_frames = 4;
    config.max_bytes = 4096;
    config.pool_buffer_count = 8;
    config.pool_buffer_size = 4096;
    config.auto_dispatch = false;
    config.drop_non_keyframe_first = true;

    PacketService service;
    if (!service.Init(config).ok()) {
        return Fail("packet service init failed");
    }

    int storage_frames = 0;
    int cloud_frames = 0;
    if (!service.Subscribe(PacketConsumerId::Storage,
                           [&](const PacketFrame& frame) {
                               ++storage_frames;
                               if (frame.size() != 16) {
                                   std::cerr << "storage frame metadata mismatch\n";
                               }
                           }).ok()) {
        return Fail("storage subscribe failed");
    }
    if (!service.Subscribe(PacketConsumerId::CloudLive,
                           [&](const PacketFrame& frame) {
                               ++cloud_frames;
                               if (frame.size() != 16) {
                                   std::cerr << "cloud frame metadata mismatch\n";
                               }
                           }).ok()) {
        return Fail("cloud live subscribe failed");
    }
    if (!service.Subscribe(PacketConsumerId::CloudStorage,
                           [](const PacketFrame&) {}).ok()) {
        return Fail("cloud storage subscribe failed");
    }

    const PacketConsumerConfig storage_default =
        service.GetConsumerConfig(PacketConsumerId::Storage);
    const PacketConsumerConfig live_default =
        service.GetConsumerConfig(PacketConsumerId::CloudLive);
    const PacketConsumerConfig cloud_storage_default =
        service.GetConsumerConfig(PacketConsumerId::CloudStorage);
    if (storage_default.max_frames <= live_default.max_frames ||
        cloud_storage_default.max_frames <= live_default.max_frames ||
        storage_default.drop_non_keyframe_first ||
        !live_default.drop_non_keyframe_first ||
        !cloud_storage_default.drop_non_keyframe_first) {
        return Fail("packet consumer default strategies mismatch");
    }
    service.Unsubscribe(PacketConsumerId::CloudStorage);

    if (!service.PublishFrame(MakeFrame(1, true)).ok()) {
        return Fail("publish first frame failed");
    }
    if (storage_frames != 0 || cloud_frames != 0) {
        return Fail("auto_dispatch=false must not invoke callbacks during publish");
    }
    if (!service.Dispatch(PacketConsumerId::Storage, 1).ok() ||
        !service.Dispatch(PacketConsumerId::CloudLive, 1).ok()) {
        return Fail("dispatch first frame failed");
    }
    if (storage_frames != 1 || cloud_frames != 1) {
        return Fail("both consumers must receive the first frame");
    }

    PacketConsumerConfig tiny_cloud_queue;
    tiny_cloud_queue.max_frames = 1;
    tiny_cloud_queue.max_bytes = 4096;
    tiny_cloud_queue.drop_non_keyframe_first = true;
    tiny_cloud_queue.wait_next_keyframe_after_drop = true;
    if (!service.SetConsumerConfig(PacketConsumerId::CloudLive, tiny_cloud_queue).ok()) {
        return Fail("set cloud consumer config failed");
    }

    std::vector<PacketPressureEventType> pressure_events;
    if (!service.SetPressureCallback(
            [&](const PacketPressureEvent& event) {
                if (event.consumer == PacketConsumerId::CloudLive ||
                    event.consumer == PacketConsumerId::Unknown) {
                    pressure_events.push_back(event.type);
                }
            }).ok()) {
        return Fail("set pressure callback failed");
    }

    if (!service.PublishFrame(MakeFrame(2, true)).ok() ||
        !service.PublishFrame(MakeFrame(3, false)).ok() ||
        !service.PublishFrame(MakeFrame(4, false)).ok()) {
        return Fail("publish backpressure frames failed");
    }
    if (!service.Dispatch(PacketConsumerId::Storage, 8).ok()) {
        return Fail("storage dispatch during cloud backpressure failed");
    }
    if (storage_frames != 4) {
        return Fail("storage must continue receiving while cloud live is backpressured");
    }
    const PacketConsumerStats cloud_stats = service.GetConsumerStats(PacketConsumerId::CloudLive);
    if (cloud_stats.dropped_frames == 0 || !cloud_stats.waiting_keyframe) {
        return Fail("cloud live must drop frames and wait for keyframe after backpressure");
    }
    bool saw_live_overflow = false;
    for (const PacketPressureEventType event : pressure_events) {
        if (event == PacketPressureEventType::ConsumerOverflow) {
            saw_live_overflow = true;
        }
    }
    if (!saw_live_overflow) {
        return Fail("cloud live overflow must notify pressure callback");
    }

    if (!service.PublishFrame(MakeFrame(5, false)).ok()) {
        return Fail("publish non-keyframe while waiting failed");
    }
    if (!service.Dispatch(PacketConsumerId::CloudLive, 8).ok()) {
        return Fail("cloud dispatch while waiting failed");
    }
    if (cloud_frames != 1) {
        return Fail("cloud live must not recover on non-keyframe");
    }

    if (!service.PublishFrame(MakeFrame(6, true)).ok() ||
        !service.Dispatch(PacketConsumerId::CloudLive, 8).ok()) {
        return Fail("cloud live keyframe recovery failed");
    }
    if (cloud_frames != 2 ||
        service.GetConsumerStats(PacketConsumerId::CloudLive).waiting_keyframe) {
        return Fail("cloud live must recover on next keyframe");
    }
    bool saw_live_recover = false;
    for (const PacketPressureEventType event : pressure_events) {
        if (event == PacketPressureEventType::ConsumerRecovered) {
            saw_live_recover = true;
        }
    }
    if (!saw_live_recover) {
        return Fail("cloud live keyframe recovery must notify pressure callback");
    }

    const PacketStats stats = service.GetStats();
    if (stats.published_frames != 6 ||
        stats.dropped_frames == 0 ||
        stats.active_consumers != 2) {
        return Fail("packet service stats mismatch");
    }

    service.Deinit();

    PacketConfig pool_config;
    pool_config.max_frames = 4;
    pool_config.max_bytes = 4096;
    pool_config.pool_buffer_count = 1;
    pool_config.pool_buffer_size = 4096;
    pool_config.auto_dispatch = false;

    PacketService pool_limited_service;
    int pool_storage_frames = 0;
    if (!pool_limited_service.Init(pool_config).ok() ||
        !pool_limited_service.Subscribe(PacketConsumerId::Storage,
                                        [&](const PacketFrame&) {
                                            ++pool_storage_frames;
                                        }).ok()) {
        return Fail("pool limited service init failed");
    }
    bool saw_pool_exhausted = false;
    bool saw_pool_recovered = false;
    pool_limited_service.SetPressureCallback(
        [&](const PacketPressureEvent& event) {
            if (event.type == PacketPressureEventType::PoolExhausted) {
                saw_pool_exhausted = true;
            }
            if (event.type == PacketPressureEventType::PoolRecovered) {
                saw_pool_recovered = true;
            }
        });
    if (!pool_limited_service.PublishFrame(MakeFrame(20, true)).ok()) {
        return Fail("pool limited service must publish first frame");
    }
    if (pool_limited_service.PublishFrame(MakeFrame(21, false)).ok()) {
        return Fail("pool limited service must reject second frame while first is queued");
    }
    if (!pool_limited_service.Dispatch(PacketConsumerId::Storage, 1).ok() ||
        pool_storage_frames != 1) {
        return Fail("pool limited service dispatch failed");
    }
    if (!saw_pool_exhausted || !saw_pool_recovered) {
        return Fail("pool pressure and recovery must notify callback");
    }
    if (!pool_limited_service.PublishFrame(MakeFrame(22, true)).ok()) {
        return Fail("pool limited service must reuse buffer after dispatch");
    }
    pool_limited_service.Deinit();

    PacketService producer_service;
    PacketConfig producer_config;
    producer_config.max_frames = 4;
    producer_config.max_bytes = 4096;
    producer_config.pool_buffer_count = 1;
    producer_config.pool_buffer_size = 64;
    producer_config.auto_dispatch = false;

    int producer_storage_frames = 0;
    std::uint8_t producer_first_byte = 0;
    if (!producer_service.Init(producer_config).ok() ||
        !producer_service.Subscribe(PacketConsumerId::Storage,
                                    [&](const PacketFrame& frame) {
                                        ++producer_storage_frames;
                                        producer_first_byte = frame.data() ? frame.data()[0] : 0;
                                    }).ok()) {
        return Fail("producer buffer service init failed");
    }

    PacketWritableBuffer writable;
    if (!producer_service.HasCapacity(8) ||
        !producer_service.AcquireBuffer(8, writable).ok() ||
        writable.data == nullptr ||
        writable.capacity < 8) {
        return Fail("producer must acquire writable buffer");
    }
    writable.data[0] = 0x12;
    writable.data[1] = 0x34;
    writable.size = 8;

    if (producer_service.HasCapacity(8)) {
        return Fail("pool with one checked-out buffer must report no capacity");
    }

    PacketFrameInfo info;
    info.media_type = PacketMediaType::Video;
    info.chn_id = 0;
    info.codec = aov::media::VideoCodec::H264;
    info.pts_ms = 9000;
    info.sequence = 90;
    info.is_keyframe = true;
    if (!producer_service.PublishBuffer(writable, info).ok()) {
        return Fail("producer must publish writable buffer");
    }
    if (writable.data != nullptr || writable.size != 0) {
        return Fail("PublishBuffer must invalidate writable buffer handle");
    }
    if (!producer_service.Dispatch(PacketConsumerId::Storage, 1).ok() ||
        producer_storage_frames != 1 ||
        producer_first_byte != 0x12) {
        return Fail("producer published buffer must reach storage consumer");
    }

    PacketWritableBuffer abandoned;
    if (!producer_service.AcquireBuffer(8, abandoned).ok()) {
        return Fail("producer must acquire buffer for release check");
    }
    producer_service.ReleaseBuffer(abandoned);
    if (abandoned.data != nullptr || !producer_service.HasCapacity(8)) {
        return Fail("ReleaseBuffer must return capacity");
    }
    const std::string dump = producer_service.DumpStats();
    if (dump.find("published=") == std::string::npos ||
        dump.find("consumer=Storage") == std::string::npos) {
        return Fail("DumpStats must include global and consumer stats");
    }
    producer_service.Deinit();

    PacketService audio_service;
    PacketConfig audio_config;
    audio_config.pool_buffer_count = 2;
    audio_config.pool_buffer_size = 64;
    audio_config.auto_dispatch = false;
    PacketFrame received_audio;
    if (!audio_service.Init(audio_config).ok() ||
        !audio_service.Subscribe(PacketConsumerId::Storage,
                                 [&](const PacketFrame& frame) {
                                     received_audio = frame;
                                 }).ok()) {
        return Fail("audio packet service init failed");
    }
    PacketWritableBuffer audio_buffer;
    if (!audio_service.AcquireBuffer(8, audio_buffer).ok()) {
        return Fail("audio packet acquire failed");
    }
    audio_buffer.data[0] = 0x7f;
    audio_buffer.size = 8;
    PacketFrameInfo audio_info;
    audio_info.media_type = PacketMediaType::Audio;
    audio_info.audio_codec = PacketAudioCodec::G711A;
    audio_info.sample_rate = 8000;
    audio_info.channels = 1;
    audio_info.bit_width = 16;
    audio_info.pts_ms = 100;
    audio_info.sequence = 10;
    if (!audio_service.PublishBuffer(audio_buffer, audio_info).ok() ||
        !audio_service.Dispatch(PacketConsumerId::Storage, 1).ok()) {
        return Fail("audio packet publish failed");
    }
    if (received_audio.media_type != PacketMediaType::Audio ||
        received_audio.audio_codec != PacketAudioCodec::G711A ||
        received_audio.sample_rate != 8000 ||
        received_audio.channels != 1 ||
        received_audio.bit_width != 16) {
        return Fail("audio packet metadata mismatch");
    }
    audio_service.Deinit();

    std::cout << "packet service contract passed\n";
    return 0;
}
