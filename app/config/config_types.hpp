#pragma once

#include <cstdint>
#include <string>
#include <utility>
#include <vector>

namespace aov::app::config {

enum class ConfigStatusCode {
    Ok,
    InvalidArgument,
    InvalidState,
    PersistFailed,
    InternalError,
};

enum class ConfigSource {
    FactoryDefault,
    LocalPersisted,
    CloudDesired,
    AppCommand,
    RuntimePolicy,
};

enum class ConfigApplyState {
    Pending,
    Applied,
    Rejected,
    NeedMediaRestart,
    NeedDeviceReboot,
    Failed,
};

struct ConfigVersion {
    std::uint64_t version {0};
    std::uint64_t updated_ts_ms {0};
    ConfigSource source {ConfigSource::FactoryDefault};
};

struct ConfigResult {
    ConfigStatusCode code {ConfigStatusCode::Ok};
    std::string field;
    std::string message;

    static ConfigResult Ok() {
        return {};
    }

    static ConfigResult Error(ConfigStatusCode status,
                              std::string field_name,
                              std::string reason) {
        ConfigResult result;
        result.code = status;
        result.field = std::move(field_name);
        result.message = std::move(reason);
        return result;
    }

    bool ok() const {
        return code == ConfigStatusCode::Ok;
    }
};

struct PendingConfigChange {
    ConfigVersion version;
    std::vector<std::string> changed_fields;
    bool requires_media_restart {false};
    bool requires_device_reboot {false};
    bool requires_wakeup_apply {false};
    bool can_apply_in_sleep {false};
};

} // namespace aov::app::config
