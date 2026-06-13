#include <atomic>
#include <chrono>
#include <csignal>
#include <cstdint>
#include <cstdlib>
#include <fstream>
#include <iostream>
#include <memory>
#include <sstream>
#include <string>
#include <thread>

#include "common/media_config.hpp"
#include "common/media_status.hpp"
#include "common/media_types.hpp"
#include "runtime/i_media_runtime.hpp"
#include "video/i_video_stream_service.hpp"

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
        case aov::media::VideoCodec::H264:
            return "h264";
        case aov::media::VideoCodec::H265:
            return "h265";
        case aov::media::VideoCodec::JPEG:
            return "jpeg";
    }
    return "unknown";
}

aov::media::VideoCodec ParseCodec(const std::string& value) {
    if (value == "h264" || value == "H264") {
        return aov::media::VideoCodec::H264;
    }
    if (value == "h265" || value == "H265" || value == "hevc" || value == "HEVC") {
        return aov::media::VideoCodec::H265;
    }
    return aov::media::VideoCodec::H265;
}

struct PairInt {
    int first {0};
    int second {0};
};

PairInt ParsePairInt(const std::string& value, const PairInt fallback) {
    PairInt parsed = fallback;
    const std::size_t comma = value.find(',');
    if (comma == std::string::npos) {
        parsed.first = std::atoi(value.c_str());
        return parsed;
    }
    parsed.first = std::atoi(value.substr(0, comma).c_str());
    parsed.second = std::atoi(value.substr(comma + 1).c_str());
    return parsed;
}

struct DemoConfig {
    int width {1920};
    int height {1080};
    int fps {15};
    int fps_aov {1};
    int bitrate_kbps {1024};
    int gop {60};
    int gop_in_aov {10};
    int chn_id {0};
    int run_seconds {20};
    int aiisp_mode {0};
    int gdc_enable {1};
    int jenc_enable {1};
    int vin_cap_num {0};
    int sleep_time_ms {3000};
    int chn_frame_mode {1};
    int sd_mode {1};
    int tisp_in_aov {0};
    int audio_in_aov {0};
    int sns_ctrl_mode {3};
    int ivps_sharpen {-1};
    int case_id {3};
    PairInt link_mode {3, 1};
    PairInt venc_change_rc_policy {0, 1};
    std::string bw_limit_level {"0,4,0,0"};
    std::string interpolation_resolution {"0,0"};
    std::string iq_bin_path {"/opt/etc/os04d10_sdr_vs.bin"};  // Use QSDemo's QS bin for better AWB
    std::string aiisp_model_path {"/opt/etc/models/aiisp"};
    bool interactive {false};
    std::string output_path {"/tmp/libmedia_demo.h265"};
    aov::media::VideoCodec codec {aov::media::VideoCodec::H265};
};

void PrintUsage(const char* argv0) {
    std::cout
        << "usage: " << argv0 << " [options]\n"
        << "options:\n"
        << "  --codec h264|h265        default h265\n"
        << "  --width N                default 1920\n"
        << "  --height N               default 1080\n"
        << "  --fps N                  default 15\n"
        << "  --fps-aov N              default 1\n"
        << "  --bitrate N              kbps, default 1024\n"
        << "  --gop N                  default 60\n"
        << "  --gop-in-aov N           default 10, align qsdemo -s\n"
        << "  --chn N                  default 0\n"
        << "  --seconds N              default 20\n"
        << "  --output PATH            default /tmp/libmedia_demo.h265\n"
        << "  --interactive            read commands from stdin\n"
        << "\nqsdemo-aligned options:\n"
        << "  --case-id N              default 3, OS04D10 single\n"
        << "  --aiisp-mode N           default 0, qsdemo -a\n"
        << "  --gdc N                  default 1, qsdemo -b\n"
        << "  --jenc N                 default 1, qsdemo -e\n"
        << "  --vin-cap-num N          default 0, qsdemo -f\n"
        << "  --link-mode A,B          default 3,1, qsdemo -g\n"
        << "  --sleep-time-ms N        default 3000, qsdemo -k\n"
        << "  --chn-frame-mode N       default 1, qsdemo -l\n"
        << "  --sd-mode N              default 1, qsdemo -m\n"
        << "  --tisp-in-aov N          default 0, qsdemo -o\n"
        << "  --audio-in-aov N         default 0, qsdemo -p\n"
        << "  --venc-rc-policy A,B     default 0,1.0, qsdemo -q\n"
        << "  --sns-ctrl-mode N        default 3, qsdemo -z\n"
        << "  --bw-limit-level VALUE   default 0,4,0,0, qsdemo -A\n"
        << "  --ivps-sharpen N         default -1, qsdemo -B\n"
        << "  --interpolation W,H      default 0,0, qsdemo -C\n"
        << "  --iq-bin PATH            default /opt/etc/OS04D10_SDR.bin\n"
        << "  --aiisp-model PATH       default /opt/etc/models/aiisp\n"
        << "\ninteractive commands:\n"
        << "  status\n"
        << "  set bitrate N\n"
        << "  set fps N\n"
        << "  set gop N\n"
        << "  set codec h264|h265\n"
        << "  set resolution W H\n"
        << "  key\n"
        << "  sleep\n"
        << "  wake\n"
        << "  quit\n";
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
        } else if (arg == "--codec") {
            const char* value = require_value("--codec");
            if (!value) return false;
            config.codec = ParseCodec(value);
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
        } else if (arg == "--fps-aov") {
            const char* value = require_value("--fps-aov");
            if (!value) return false;
            config.fps_aov = std::atoi(value);
        } else if (arg == "--bitrate") {
            const char* value = require_value("--bitrate");
            if (!value) return false;
            config.bitrate_kbps = std::atoi(value);
        } else if (arg == "--gop") {
            const char* value = require_value("--gop");
            if (!value) return false;
            config.gop = std::atoi(value);
        } else if (arg == "--gop-in-aov") {
            const char* value = require_value("--gop-in-aov");
            if (!value) return false;
            config.gop_in_aov = std::atoi(value);
        } else if (arg == "--chn") {
            const char* value = require_value("--chn");
            if (!value) return false;
            config.chn_id = std::atoi(value);
        } else if (arg == "--seconds") {
            const char* value = require_value("--seconds");
            if (!value) return false;
            config.run_seconds = std::atoi(value);
        } else if (arg == "--output") {
            const char* value = require_value("--output");
            if (!value) return false;
            config.output_path = value;
        } else if (arg == "--interactive") {
            config.interactive = true;
        } else if (arg == "--case-id") {
            const char* value = require_value("--case-id");
            if (!value) return false;
            config.case_id = std::atoi(value);
        } else if (arg == "--aiisp-mode") {
            const char* value = require_value("--aiisp-mode");
            if (!value) return false;
            config.aiisp_mode = std::atoi(value);
        } else if (arg == "--gdc") {
            const char* value = require_value("--gdc");
            if (!value) return false;
            config.gdc_enable = std::atoi(value);
        } else if (arg == "--jenc") {
            const char* value = require_value("--jenc");
            if (!value) return false;
            config.jenc_enable = std::atoi(value);
        } else if (arg == "--vin-cap-num") {
            const char* value = require_value("--vin-cap-num");
            if (!value) return false;
            config.vin_cap_num = std::atoi(value);
        } else if (arg == "--link-mode") {
            const char* value = require_value("--link-mode");
            if (!value) return false;
            config.link_mode = ParsePairInt(value, config.link_mode);
        } else if (arg == "--sleep-time-ms") {
            const char* value = require_value("--sleep-time-ms");
            if (!value) return false;
            config.sleep_time_ms = std::atoi(value);
        } else if (arg == "--chn-frame-mode") {
            const char* value = require_value("--chn-frame-mode");
            if (!value) return false;
            config.chn_frame_mode = std::atoi(value);
        } else if (arg == "--sd-mode") {
            const char* value = require_value("--sd-mode");
            if (!value) return false;
            config.sd_mode = std::atoi(value);
        } else if (arg == "--tisp-in-aov") {
            const char* value = require_value("--tisp-in-aov");
            if (!value) return false;
            config.tisp_in_aov = std::atoi(value);
        } else if (arg == "--audio-in-aov") {
            const char* value = require_value("--audio-in-aov");
            if (!value) return false;
            config.audio_in_aov = std::atoi(value);
        } else if (arg == "--venc-rc-policy") {
            const char* value = require_value("--venc-rc-policy");
            if (!value) return false;
            config.venc_change_rc_policy = ParsePairInt(value, config.venc_change_rc_policy);
        } else if (arg == "--sns-ctrl-mode") {
            const char* value = require_value("--sns-ctrl-mode");
            if (!value) return false;
            config.sns_ctrl_mode = std::atoi(value);
        } else if (arg == "--bw-limit-level") {
            const char* value = require_value("--bw-limit-level");
            if (!value) return false;
            config.bw_limit_level = value;
        } else if (arg == "--ivps-sharpen") {
            const char* value = require_value("--ivps-sharpen");
            if (!value) return false;
            config.ivps_sharpen = std::atoi(value);
        } else if (arg == "--interpolation") {
            const char* value = require_value("--interpolation");
            if (!value) return false;
            config.interpolation_resolution = value;
        } else if (arg == "--iq-bin") {
            const char* value = require_value("--iq-bin");
            if (!value) return false;
            config.iq_bin_path = value;
        } else if (arg == "--aiisp-model") {
            const char* value = require_value("--aiisp-model");
            if (!value) return false;
            config.aiisp_model_path = value;
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
    video.main_stream.chn_id = config.chn_id;
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
    init.sensor.resolution = {2560, 1440};  // ✅ OS04D10 正确分辨率（修复：原来错误地设置为 2688x1520）
    init.sensor.fps = config.fps;
    init.sensor.chn_frame_mode = config.chn_frame_mode;
    init.sensor.iq_bin_path = config.iq_bin_path;
    init.pool = aov::media::CmPoolConfig::DefaultAx615();
    init.video = BuildVideoConfig(config);
    init.audio.enabled = false;
    init.detect.enabled = false;
    init.detect.pipe_id = 0;
    init.detect.ivps_grp_id = 0;
    init.detect.ivps_chn_id = 0;  // Use main stream channel for detection
    
    // Add human detection rule
    aov::media::DetectRuleApplyConfig rule;
    rule.type = aov::media::DetectTargetType::Human;
    rule.enabled = true;
    rule.pipe_id = 0;
    rule.ivps_grp_id = 0;
    rule.ivps_chn_id = 0;
    rule.sensitivity = 60;
    rule.static_filter = false;
    init.detect.rules.push_back(rule);
    
    init.osd.enabled = false;
    return init;
}

void PrintQsdemoProfile(const DemoConfig& config) {
    std::cout
        << "[qsdemo-profile]"
        << " case_id=" << config.case_id
        << " aiisp_mode=" << config.aiisp_mode
        << " gdc=" << config.gdc_enable
        << " jenc=" << config.jenc_enable
        << " vin_cap_num=" << config.vin_cap_num
        << " link_mode=" << config.link_mode.first << "," << config.link_mode.second
        << " sleep_time_ms=" << config.sleep_time_ms
        << " chn_frame_mode=" << config.chn_frame_mode
        << " sd_mode=" << config.sd_mode
        << " tisp_in_aov=" << config.tisp_in_aov
        << " audio_in_aov=" << config.audio_in_aov
        << " venc_rc_policy=" << config.venc_change_rc_policy.first
        << "," << config.venc_change_rc_policy.second
        << " gop_in_aov=" << config.gop_in_aov
        << " sns_ctrl_mode=" << config.sns_ctrl_mode
        << " bw_limit_level=" << config.bw_limit_level
        << " ivps_sharpen=" << config.ivps_sharpen
        << " interpolation=" << config.interpolation_resolution
        << "\n"
        << "[qsdemo-profile] iq_bin=" << config.iq_bin_path
        << " aiisp_model=" << config.aiisp_model_path
        << "\n";

    std::cout
        << "[qsdemo-profile][note] Some qsdemo fields are collected for log parity "
        << "but require libmedia adapter support before they affect SDK setup.\n";
}

bool CheckStatus(const char* action, const aov::media::MediaStatusCode code) {
    if (code == aov::media::MediaStatusCode::Ok) {
        std::cout << "[OK] " << action << "\n";
        return true;
    }
    std::cerr << "[FAIL] " << action << " status=" << static_cast<int>(code) << "\n";
    return false;
}

void PrintRuntimeStatus(aov::media::IMediaRuntime& runtime) {
    const auto status = runtime.GetRuntimeStatus();
    std::cout
        << "[status]"
        << " initialized=" << status.initialized
        << " running=" << status.running
        << " low_power=" << status.low_power
        << " prepared_sleep=" << status.prepared_for_aov_sleep
        << " video_capturing=" << status.video_capturing
        << " frames=" << status.video_frames
        << " last_error=" << static_cast<int>(status.last_error.code)
        << " msg=" << status.last_error.message
        << "\n";
}

bool ApplyVideo(aov::media::IMediaRuntime& runtime, const DemoConfig& config) {
    std::cout
        << "[config] codec=" << CodecName(config.codec)
        << " " << config.width << "x" << config.height
        << " fps=" << config.fps
        << " bitrate=" << config.bitrate_kbps
        << " gop=" << config.gop
        << "\n";
    return CheckStatus("ApplyVideoConfig", runtime.ApplyVideoConfig(BuildVideoConfig(config)));
}

void CommandLoop(aov::media::IMediaRuntime& runtime, DemoConfig& config) {
    std::string line;
    while (g_running && std::getline(std::cin, line)) {
        std::istringstream iss(line);
        std::string cmd;
        iss >> cmd;
        if (cmd.empty()) {
            continue;
        }
        if (cmd == "quit" || cmd == "exit") {
            g_running = false;
            break;
        }
        if (cmd == "status") {
            PrintRuntimeStatus(runtime);
            continue;
        }
        if (cmd == "key") {
            auto* video = runtime.GetVideoStreamService();
            if (video) {
                CheckStatus("RequestKeyFrame", video->RequestKeyFrame(config.chn_id));
            }
            continue;
        }
        if (cmd == "sleep") {
            aov::media::AovSleepMediaConfig sleep;
            sleep.enable_low_power_wakeup = true;
            sleep.pipe_id = 0;
            CheckStatus("PrepareForAovSleep", runtime.PrepareForAovSleep(sleep));
            PrintRuntimeStatus(runtime);
            continue;
        }
        if (cmd == "wake") {
            aov::media::WakeupMediaProfile wake;
            wake.reason = aov::media::WakeupReason::Manual;
            wake.start_video = true;
            wake.request_idr = true;
            CheckStatus("RestoreFromAovSleep", runtime.RestoreFromAovSleep(wake));
            PrintRuntimeStatus(runtime);
            continue;
        }
        if (cmd == "set") {
            std::string field;
            iss >> field;
            if (field == "bitrate") {
                iss >> config.bitrate_kbps;
            } else if (field == "fps") {
                iss >> config.fps;
            } else if (field == "gop") {
                iss >> config.gop;
            } else if (field == "codec") {
                std::string codec;
                iss >> codec;
                config.codec = ParseCodec(codec);
            } else if (field == "resolution") {
                iss >> config.width >> config.height;
            } else {
                std::cerr << "unknown set field: " << field << "\n";
                continue;
            }
            ApplyVideo(runtime, config);
            continue;
        }
        std::cerr << "unknown command: " << cmd << "\n";
    }
}

}  // namespace

int main(int argc, char** argv) {
    DemoConfig config;
    if (!ParseArgs(argc, argv, config)) {
        return 2;
    }

    PrintQsdemoProfile(config);

    std::signal(SIGINT, OnSignal);
    std::signal(SIGTERM, OnSignal);

    std::ofstream output(config.output_path, std::ios::binary | std::ios::trunc);
    if (!output) {
        std::cerr << "open output failed: " << config.output_path << "\n";
        return 1;
    }

    auto runtime = aov::media::CreateAx615MediaRuntime();
    if (!runtime) {
        std::cerr << "CreateAx615MediaRuntime failed\n";
        return 1;
    }

    std::atomic<std::uint64_t> frames {0};
    std::atomic<std::uint64_t> bytes {0};
    auto* video = runtime->GetVideoStreamService();
    if (!video) {
        std::cerr << "GetVideoStreamService returned null\n";
        return 1;
    }

    if (!CheckStatus("RegisterStreamCallback",
                     video->RegisterStreamCallback(
                         config.chn_id,
                         [&](int, const aov::media::StreamFrame& frame) {
                             if (frame.data && frame.size > 0) {
                                 output.write(reinterpret_cast<const char*>(frame.data), frame.size);
                                 ++frames;
                                 bytes += frame.size;
                                 if (frames.load() <= 5 || frame.is_keyframe ||
                                     frames.load() % 30 == 0) {
                                     std::cout
                                         << "[frame]"
                                         << " seq=" << frame.sequence
                                         << " pts=" << frame.pts_ms
                                         << " size=" << frame.size
                                         << " key=" << frame.is_keyframe
                                         << " codec=" << CodecName(frame.codec)
                                         << "\n";
                                 }
                             }
                         }))) {
        return 1;
    }

    std::cout << "[demo] output=" << config.output_path << "\n";
    if (!CheckStatus("Init", runtime->Init(BuildRuntimeConfig(config)))) {
        return 1;
    }
    if (!CheckStatus("Start", runtime->Start())) {
        runtime->Stop();
        return 1;
    }
    PrintRuntimeStatus(*runtime);

    std::thread command_thread;
    if (config.interactive) {
        command_thread = std::thread([&]() {
            CommandLoop(*runtime, config);
        });
    }

    const auto begin = std::chrono::steady_clock::now();
    while (g_running) {
        std::this_thread::sleep_for(std::chrono::seconds(1));
        output.flush();
        const auto elapsed = std::chrono::duration_cast<std::chrono::seconds>(
            std::chrono::steady_clock::now() - begin).count();
        std::cout
            << "[progress] sec=" << elapsed
            << " frames=" << frames.load()
            << " bytes=" << bytes.load()
            << "\n";
        if (!config.interactive && elapsed >= config.run_seconds) {
            break;
        }
    }

    g_running = false;
    if (command_thread.joinable()) {
        command_thread.join();
    }

    runtime->Stop();
    output.flush();
    output.close();
    PrintRuntimeStatus(*runtime);
    std::cout
        << "[done] frames=" << frames.load()
        << " bytes=" << bytes.load()
        << " output=" << config.output_path
        << "\n";

    return frames.load() > 0 ? 0 : 3;
}
