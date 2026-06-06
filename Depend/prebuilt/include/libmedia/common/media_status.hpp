#pragma once

#include <string>

namespace aov::media {

enum class MediaStatusCode {
    Ok,
    InvalidArgument,
    InvalidState,
    Busy,
    Timeout,
    Unsupported,
    NoResource,
    NotFound,
    PermissionDenied,
    SdkError,
    InternalError,
};

struct MediaStatus {
    MediaStatusCode code {MediaStatusCode::Ok};
    int sdk_error {0};
    std::string message;

    bool ok() const { return code == MediaStatusCode::Ok; }
};

}  // namespace aov::media
