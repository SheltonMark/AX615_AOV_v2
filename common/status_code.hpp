#pragma once

namespace aov::common {

enum class StatusCode {
    Ok,
    InvalidArgument,
    InvalidState,
    NotFound,
    Busy,
    Timeout,
    InternalError,
    Unsupported,
};

} // namespace aov::common
