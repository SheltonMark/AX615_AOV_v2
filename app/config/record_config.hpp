#pragma once

#include <cstdint>

#include "alarm_config.hpp"

namespace aov::app::config {

enum class SdCardStatus {
    Normal,
    NotDetected,
    NotFormatted,
    Formatting,
    Error,
};

enum class RecordType {
    Aov,
    Normal,
    Alarm,
};

struct SdStorageConfig {
    bool record_enabled {true};
    bool loop_record {true};
    int min_capacity_gb {8};
    int max_capacity_gb {512};
};

struct RecordScheduleConfig {
    ArmSchedule schedule;
    RecordType default_record_type {RecordType::Normal};
    int alarm_post_record_sec {5};
};

struct CloudStorageConfig {
    bool enabled {false};
    RecordType record_type {RecordType::Alarm};
};

struct StorageRuntimeStatus {
    SdCardStatus sd_status {SdCardStatus::NotDetected};
    std::uint64_t total_bytes {0};
    std::uint64_t free_bytes {0};
    int format_progress {0};
    bool aov_ddr_batch_write_enabled {true};
};

struct RecordConfig {
    SdStorageConfig sd;
    RecordScheduleConfig schedule;
    CloudStorageConfig cloud_storage;
};

} // namespace aov::app::config
