#pragma once

#include <cstdint>
#include <string>

namespace aov::app::storage::dhfs {

enum class DhfsStatusCode {
    Ok,
    InvalidArgument,
    InvalidState,
    OpenFailed,
    WriteFailed,
    CloseFailed,
    InternalError,
};

struct DhfsWriterConfig {
    std::string root_dir {"/mnt/sdcard/aov_record"};
    std::uint32_t channel {0};
    bool enable_ps_pes {true};
};

struct DhfsWriterStats {
    std::uint64_t written_frames {0};
    std::uint64_t written_bytes {0};
    bool opened {false};
    bool flushed {true};
};

struct DhfsResult {
    DhfsStatusCode code {DhfsStatusCode::Ok};
    std::string message;

    static DhfsResult Ok() {
        return {};
    }

    static DhfsResult Error(DhfsStatusCode status, std::string msg) {
        DhfsResult result;
        result.code = status;
        result.message = std::move(msg);
        return result;
    }

    bool ok() const {
        return code == DhfsStatusCode::Ok;
    }
};

} // namespace aov::app::storage::dhfs
