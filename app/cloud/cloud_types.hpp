#pragma once

#include <cstdint>
#include <string>

namespace aov::app::cloud {

enum class CloudPropertyOp {
    Set,
    Get,
    ReportAck,
};

struct CloudPropertyUpdate {
    std::string property_id;
    std::string value;
    CloudPropertyOp op {CloudPropertyOp::Set};
    std::uint64_t cloud_version {0};
    std::uint64_t recv_ts_ms {0};
};

struct CloudActionRequest {
    std::string action_id;
    std::string params;
    std::string request_id;
    std::uint64_t recv_ts_ms {0};
};

struct CloudEventReport {
    std::string event_id;
    std::string payload;
    std::uint64_t utc_sec {0};
};

enum class CloudStorageResult {
    None,
    Success,
    ExplicitFailure,
};

struct CloudRuntimeStatus {
    bool connected {false};
    bool preview_streaming {false};
    bool cloud_storage_running {false};
    CloudStorageResult cloud_storage_result {CloudStorageResult::None};
    std::string active_cloud_storage_id;
};

enum class CloudConfigStatusCode {
    Ok,
    InvalidArgument,
    UnsupportedProperty,
};

struct CloudConfigError {
    CloudConfigStatusCode code {CloudConfigStatusCode::Ok};
    std::string field;
    std::string message;
};

template <typename T>
struct CloudConfigResult {
    T value {};
    CloudConfigError error {};

    static CloudConfigResult Ok(T mapped_value) {
        CloudConfigResult result;
        result.value = mapped_value;
        return result;
    }

    static CloudConfigResult Error(CloudConfigStatusCode status,
                                   std::string field_name,
                                   std::string reason) {
        CloudConfigResult result;
        result.error.code = status;
        result.error.field = std::move(field_name);
        result.error.message = std::move(reason);
        return result;
    }

    bool ok() const {
        return error.code == CloudConfigStatusCode::Ok;
    }
};

} // namespace aov::app::cloud
