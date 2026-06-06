#pragma once

#include <cstdint>
#include <string>

namespace aov::app::storage {

enum class StorageStatusCode {
    Ok,
    InvalidArgument,
    InvalidState,
    NotMounted,
    NoSpace,
    WriteFailed,
    InternalError,
};

struct StorageConfig {
    std::string mount_point {"/mnt/sdcard"};
    std::string record_dir {"aov_record"};
    std::uint32_t segment_seconds {60};
    std::uint32_t max_record_mb {4096};
    bool loop_record {true};
};

struct StorageStats {
    std::uint64_t received_frames {0};
    std::uint64_t written_frames {0};
    std::uint64_t dropped_frames {0};
    std::uint64_t written_bytes {0};
    bool recording {false};
    bool flushed {true};
};

struct StorageResult {
    StorageStatusCode code {StorageStatusCode::Ok};
    std::string message;

    static StorageResult Ok() {
        return {};
    }

    static StorageResult Error(StorageStatusCode status, std::string msg) {
        StorageResult result;
        result.code = status;
        result.message = std::move(msg);
        return result;
    }

    bool ok() const {
        return code == StorageStatusCode::Ok;
    }
};

} // namespace aov::app::storage
