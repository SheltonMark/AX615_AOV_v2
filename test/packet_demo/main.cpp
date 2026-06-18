#include <atomic>
#include <chrono>
#include <csignal>
#include <cstdint>
#include <cstdlib>
#include <fstream>
#include <iostream>
#include <memory>
#include <string>
#include <thread>

#include "common/media_config.hpp"
#include "common/media_status.hpp"
#include "common/media_types.hpp"
#include "runtime/i_media_runtime.hpp"
#include "video/i_video_stream_service.hpp"

// Packet 模块（路径相对于 CODE_ROOT/app）
#include "packet/packet_service.hpp"

namespace aov::media {
std::unique_ptr<IMediaRuntime> CreateAx615MediaRuntime();
}

namespace {

std::atomic<bool> g_running {true};

void OnSignal(int) {
    g_running = false;
}

const char* CodecName(const aov::media::VideoCodec codec) {
    switch (codec) {
        case aov::media::VideoCodec::H264: return "h264";
        case aov::media::VideoCodec::H265: return "h265";
        case aov::media::VideoCodec::JPEG: return "jpeg";
    }
    return "unknown";
}

const char* ConsumerName(aov::app::packet::PacketConsumerId id) {
    using aov::app::packet::PacketConsumerId;
    switch (id) {
        case PacketConsumerId::Storage: return "Storage";
        case PacketConsumerId::CloudStorage: return "CloudStorage";
        case PacketConsumerId::CloudLive: return "CloudLive";
        default: return "Unknown";
    }
}

struct DemoConfig {
    int width {1920};
    int height {1080};
    int fps {15};
    int fps_aov {1};
    int bitrate_kbps {1024};
    int gop {60};
    int run_seconds {10};
    std::string output_storage {"/tmp/packet_storage.h265"};
    std::string output_cloud {"/tmp/packet_cloud.h265"};
    aov::media::VideoCodec codec {aov::media::VideoCodec::H265};
};

void PrintUsage(const char* argv0) {
    std::cout
        << "usage: " << argv0 << " [options]\n"
        << "options:\n"
        << "  --width N                default 1920\n"
        << "  --height N               default 1080\n"
        << "  --fps N                  default 15\n"
        << "  --bitrate N              kbps, default 1024\n"
        << "  --gop N                  default 60\n"
        << "  --seconds N              default 10\n"
        << "  --output-storage PATH    default /tmp/packet_storage.h265\n"
        << "  --output-cloud PATH      default /tmp/packet_cloud.h265\n"
        << "  --help\n";
}

bool ParseArgs(int argc, char** argv, DemoConfig& config) {
    for (int i = 1; i < argc; ++i) {
        const std::string arg = argv[i];
        auto require_value = [&](const char* name) -> const char* {
            if (i + 1 >= argc) {
                std::cerr << "missing value for " << name << "\n";
                return nullptr;
            }
            return argv[++i];
        };

        if (arg == "--help" || arg == "-h") {
            PrintUsage(argv[0]);
            return false;
        } else if (arg == "--width") {
            const char* value = require_value("--width");
            if (!value) return false;
            config.width = std::atoi(value);
        } else if (arg == "--height") {
            const char* value = require_value("--height");
            if (!value) return false;
            config.height = std::atoi(value);
        } else if (arg == "--fps") {
            const char* value = require_value("--fps");
            if (!value) return false;
            config.fps = std::atoi(value);
        } else if (arg == "--bitrate") {
            const char* value = require_value("--bitrate");
            if (!value) return false;
            config.bitrate_kbps = std::atoi(value);
        } else if (arg == "--gop") {
            const char* value = require_value("--gop");
            if (!value) return false;
            config.gop = std::atoi(value);
        } else if (arg == "--seconds") {
            const char* value = require_value("--seconds");
            if (!value) return false;
            config.run_seconds = std::atoi(value);
        } else if (arg == "--output-storage") {
            const char* value = require_value("--output-storage");
            if (!value) return false;
            config.output_storage = value;
        } else if (arg == "--output-cloud") {
            const char* value = require_value("--output-cloud");
            if (!value) return false;
            config.output_cloud = value;
        } else {
            std::cerr << "unknown argument: " << arg << "\n";
            PrintUsage(argv[0]);
            return false;
        }
    }
    return true;
}

aov::media::VideoEncodeGroupConfig BuildVideoConfig(const DemoConfig& config) {
    aov::media::VideoEncodeGroupConfig video;
    video.main_stream.profile = aov::media::StreamProfile::Main;
    video.main_stream.chn_id = 0;
    video.main_stream.codec = config.codec;
    video.main_stream.resolution = {config.width, config.height};
    video.main_stream.fps_normal = config.fps;
    video.main_stream.fps_aov = config.fps_aov;
    video.main_stream.rc_mode = aov::media::RcMode::CBR;
    video.main_stream.bitrate_kbps = config.bitrate_kbps;
    video.main_stream.gop = config.gop;
    video.main_stream.enabled = true;
    video.sub_stream.enabled = false;
    return video;
}

aov::media::MediaRuntimeInitConfig BuildRuntimeConfig(const DemoConfig& config) {
    aov::media::MediaRuntimeInitConfig init;
    init.sensor.sensor_id = aov::media::SensorId::OS04D10;
    init.sensor.pipe_id = 0;
    init.sensor.dev_id = 0;
    init.sensor.resolution = {2560, 1440};
    init.sensor.fps = config.fps;
    init.sensor.chn_frame_mode = 1;
    init.sensor.iq_bin_path = "/opt/etc/os04d10_sdr_vs.bin";
    init.pool = aov::media::CmPoolConfig::DefaultAx615();
    init.video = BuildVideoConfig(config);
    init.audio.enabled = false;
    init.detect.enabled = false;
    init.osd.enabled = false;
    return init;
}

bool CheckStatus(const char* action, const aov::media::MediaStatusCode code) {
    if (code == aov::media::MediaStatusCode::Ok) {
        std::cout << "[OK] " << action << "\n";
        return true;
    }
    std::cerr << "[FAIL] " << action << " status=" << static_cast<int>(code) << "\n";
    return false;
}

}  // namespace

int main(int argc, char** argv) {
    DemoConfig config;
    if (!ParseArgs(argc, argv, config)) {
        return 2;
    }

    std::cout << "[packet-demo] 测试 libmedia → PacketService 链路\n";
    std::cout << "[config] " << config.width << "x" << config.height
              << " fps=" << config.fps
              << " bitrate=" << config.bitrate_kbps << " kbps"
              << " gop=" << config.gop
              << " duration=" << config.run_seconds << "s\n";

    std::signal(SIGINT, OnSignal);
    std::signal(SIGTERM, OnSignal);

    // ========== 1. 初始化 PacketService ==========
    std::cout << "\n========== 初始化 PacketService ==========\n";
    aov::app::packet::PacketService packet_svc;
    aov::app::packet::PacketConfig pkt_cfg;
    pkt_cfg.max_frames = 128;
    pkt_cfg.max_bytes = 4 * 1024 * 1024;
    pkt_cfg.pool_buffer_count = 64;
    pkt_cfg.pool_buffer_size = 4 * 1024 * 1024;

    auto pkt_result = packet_svc.Init(pkt_cfg);
    if (!pkt_result.ok()) {
        std::cerr << "[FAIL] PacketService Init: " << pkt_result.message << "\n";
        return 1;
    }
    std::cout << "[OK] PacketService 初始化完成\n";

    // ========== 2. 订阅消费者 ==========
    std::cout << "\n========== 订阅消费者 ==========\n";
    std::atomic<int> storage_count{0};
    std::atomic<int> cloud_count{0};
    std::atomic<uint64_t> storage_bytes{0};
    std::atomic<uint64_t> cloud_bytes{0};

    std::ofstream storage_file(config.output_storage, std::ios::binary | std::ios::trunc);
    std::ofstream cloud_file(config.output_cloud, std::ios::binary | std::ios::trunc);

    if (!storage_file || !cloud_file) {
        std::cerr << "[FAIL] 打开输出文件失败\n";
        return 1;
    }

    // 订阅存储消费者
    pkt_result = packet_svc.Subscribe(
        aov::app::packet::PacketConsumerId::Storage,
        [&](const aov::app::packet::PacketFrame& pkt) {
            storage_file.write(reinterpret_cast<const char*>(pkt.data()), pkt.size());
            storage_count++;
            storage_bytes += pkt.size();

            if (storage_count <= 3 || pkt.is_keyframe) {
                std::cout << "[Storage] 帧#" << storage_count
                          << " size=" << pkt.size()
                          << " keyframe=" << pkt.is_keyframe
                          << " pts=" << pkt.pts_ms << "ms\n";
            }
        });

    if (!pkt_result.ok()) {
        std::cerr << "[FAIL] Subscribe Storage: " << pkt_result.message << "\n";
        return 1;
    }
    std::cout << "[OK] 订阅 Storage 消费者\n";

    // 订阅云存储消费者
    pkt_result = packet_svc.Subscribe(
        aov::app::packet::PacketConsumerId::CloudStorage,
        [&](const aov::app::packet::PacketFrame& pkt) {
            cloud_file.write(reinterpret_cast<const char*>(pkt.data()), pkt.size());
            cloud_count++;
            cloud_bytes += pkt.size();

            if (cloud_count <= 3 || pkt.is_keyframe) {
                std::cout << "[CloudStorage] 帧#" << cloud_count
                          << " size=" << pkt.size()
                          << " keyframe=" << pkt.is_keyframe
                          << " pts=" << pkt.pts_ms << "ms\n";
            }
        });

    if (!pkt_result.ok()) {
        std::cerr << "[FAIL] Subscribe CloudStorage: " << pkt_result.message << "\n";
        return 1;
    }
    std::cout << "[OK] 订阅 CloudStorage 消费者\n";

    // ========== 3. 初始化 libmedia ==========
    std::cout << "\n========== 初始化 libmedia ==========\n";
    auto runtime = aov::media::CreateAx615MediaRuntime();
    if (!runtime) {
        std::cerr << "[FAIL] CreateAx615MediaRuntime\n";
        return 1;
    }

    auto* video = runtime->GetVideoStreamService();
    if (!video) {
        std::cerr << "[FAIL] GetVideoStreamService\n";
        return 1;
    }

    // ========== 4. 注册码流回调 → PacketService ==========
    std::cout << "\n========== 注册码流回调 ==========\n";
    std::atomic<int> publish_count{0};
    std::atomic<int> publish_errors{0};

    if (!CheckStatus("RegisterStreamCallback",
                     video->RegisterStreamCallback(
                         0,
                         [&](int, const aov::media::StreamFrame& frame) {
                             if (frame.data && frame.size > 0) {
                                 // 关键：StreamFrame → PacketService
                                 auto result = packet_svc.PublishFrame(frame);
                                 if (result.ok()) {
                                     publish_count++;
                                 } else {
                                     publish_errors++;
                                     if (publish_errors <= 5) {
                                         std::cerr << "[ERROR] PublishFrame 失败: "
                                                   << result.message << "\n";
                                     }
                                 }
                             }
                         }))) {
        return 1;
    }

    // ========== 5. 启动 runtime ==========
    std::cout << "\n========== 启动媒体管道 ==========\n";
    if (!CheckStatus("Runtime Init", runtime->Init(BuildRuntimeConfig(config)))) {
        return 1;
    }
    if (!CheckStatus("Runtime Start", runtime->Start())) {
        runtime->Stop();
        return 1;
    }

    // ========== 6. 运行测试 ==========
    std::cout << "\n========== 开始采集（" << config.run_seconds << "秒）==========\n";
    const auto begin = std::chrono::steady_clock::now();
    while (g_running) {
        std::this_thread::sleep_for(std::chrono::seconds(1));
        storage_file.flush();
        cloud_file.flush();

        const auto elapsed = std::chrono::duration_cast<std::chrono::seconds>(
            std::chrono::steady_clock::now() - begin).count();

        std::cout << "[进度] " << elapsed << "s"
                  << " 发布=" << publish_count
                  << " Storage=" << storage_count
                  << " Cloud=" << cloud_count
                  << " 错误=" << publish_errors << "\n";

        if (elapsed >= config.run_seconds) {
            break;
        }
    }

    // ========== 7. 停止并统计 ==========
    std::cout << "\n========== 停止采集 ==========\n";
    g_running = false;
    runtime->Stop();
    storage_file.flush();
    cloud_file.flush();
    storage_file.close();
    cloud_file.close();

    // 获取 PacketService 统计
    auto stats = packet_svc.GetStats();

    std::cout << "\n========== 验证结果 ==========\n";
    std::cout << "【PacketService 统计】\n";
    std::cout << "  发布帧数: " << stats.published_frames << "\n";
    std::cout << "  发布字节: " << stats.published_bytes << " bytes\n";
    std::cout << "  投递帧数: " << stats.delivered_frames << "\n";
    std::cout << "  丢弃帧数: " << stats.dropped_frames << "\n";
    std::cout << "  丢弃字节: " << stats.dropped_bytes << " bytes\n";
    std::cout << "  订阅者数: " << stats.subscriber_count << "\n";
    std::cout << "  缓存帧数: " << stats.buffered_frames << "\n";

    std::cout << "\n【消费者统计】\n";
    std::cout << "  Storage 收到: " << storage_count << " 帧, "
              << storage_bytes << " bytes\n";
    std::cout << "  CloudStorage 收到: " << cloud_count << " 帧, "
              << cloud_bytes << " bytes\n";

    std::cout << "\n【一致性检查】\n";
    bool consistency_ok = (storage_count == cloud_count &&
                           storage_bytes == cloud_bytes &&
                           storage_count == static_cast<int>(stats.published_frames));
    std::cout << "  帧数一致: " << (storage_count == cloud_count ? "✅" : "❌") << "\n";
    std::cout << "  字节一致: " << (storage_bytes == cloud_bytes ? "✅" : "❌") << "\n";
    std::cout << "  发布一致: " << (storage_count == static_cast<int>(stats.published_frames) ? "✅" : "❌") << "\n";
    std::cout << "  无丢帧: " << (stats.dropped_frames == 0 ? "✅" : "❌") << "\n";
    std::cout << "  总体结果: " << (consistency_ok && stats.dropped_frames == 0 ? "✅ 通过" : "❌ 失败") << "\n";

    std::cout << "\n【输出文件】\n";
    std::cout << "  Storage: " << config.output_storage << "\n";
    std::cout << "  CloudStorage: " << config.output_cloud << "\n";
    std::cout << "\n【验证命令】\n";
    std::cout << "  md5sum " << config.output_storage << " " << config.output_cloud << "\n";
    std::cout << "  ffplay " << config.output_storage << "\n";

    return (consistency_ok && stats.dropped_frames == 0) ? 0 : 1;
}
