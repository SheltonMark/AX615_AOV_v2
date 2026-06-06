#include "app/cloud/cloud_action_mapper.hpp"

#include <cstdlib>
#include <map>

namespace aov::app::cloud {

namespace {

using aov::app::core::CoreCommand;
using aov::app::core::CoreCommandType;

CloudActionMapResult Error(const CloudActionRequest& request, const char* message) {
    return CloudActionMapResult::Error(CloudConfigStatusCode::InvalidArgument,
                                       request.action_id,
                                       message);
}

CloudActionMapResult FieldError(const CloudActionRequest& request,
                                const char* suffix,
                                const char* message) {
    return CloudActionMapResult::Error(
        CloudConfigStatusCode::InvalidArgument,
        request.action_id + std::string(".") + suffix,
        message);
}

bool ParseInt(const std::string& value, int& out) {
    char* end = nullptr;
    const long parsed = std::strtol(value.c_str(), &end, 10);
    if (end == value.c_str() || *end != '\0') {
        return false;
    }
    out = static_cast<int>(parsed);
    return true;
}

bool ParseKeyValuePairs(const std::string& value, std::map<std::string, std::string>& out) {
    std::size_t start = 0;
    while (start <= value.size()) {
        const std::size_t end = value.find(';', start);
        const std::string item = value.substr(
            start,
            end == std::string::npos ? std::string::npos : end - start);
        if (!item.empty()) {
            const std::size_t eq = item.find('=');
            if (eq == std::string::npos || eq == 0 || eq + 1 >= item.size()) {
                return false;
            }
            out[item.substr(0, eq)] = item.substr(eq + 1);
        }
        if (end == std::string::npos) {
            break;
        }
        start = end + 1;
    }
    return true;
}

std::string ReadString(const std::map<std::string, std::string>& values,
                       const char* key,
                       const char* default_value) {
    const auto it = values.find(key);
    return it == values.end() ? std::string(default_value) : it->second;
}

bool ReadInt(const std::map<std::string, std::string>& values,
             const char* key,
             int& out,
             const int default_value) {
    const auto it = values.find(key);
    if (it == values.end()) {
        out = default_value;
        return true;
    }
    return ParseInt(it->second, out);
}

CoreCommand MakeBaseCommand(const CloudActionRequest& request, const CoreCommandType type) {
    CoreCommand command;
    command.type = type;
    command.request_id = request.request_id;
    command.recv_ts_ms = request.recv_ts_ms;
    return command;
}

CloudActionMapResult MapFormatStorageMedium(const CloudActionRequest& request,
                                            const std::map<std::string, std::string>& params) {
    CoreCommand command = MakeBaseCommand(request, CoreCommandType::FormatStorageMedium);
    command.storage_medium = ReadString(params, "medium", "sdcard");
    if (command.storage_medium.empty()) {
        return FieldError(request, "medium", "medium must not be empty");
    }
    return CloudActionMapResult::Ok(command);
}

CloudActionMapResult MapOneKeyAlarm(const CloudActionRequest& request,
                                    const std::map<std::string, std::string>& params) {
    CoreCommand command = MakeBaseCommand(request, CoreCommandType::OneKeyAlarm);
    if (!ReadInt(params, "duration", command.duration_sec, 10)) {
        return FieldError(request, "duration", "duration must be integer");
    }
    if (command.duration_sec <= 0 || command.duration_sec > 300) {
        return FieldError(request, "duration", "duration must be in range 1-300");
    }
    return CloudActionMapResult::Ok(command);
}

CloudActionMapResult MapPreviewImageCapture(const CloudActionRequest& request,
                                            const std::map<std::string, std::string>& params) {
    CoreCommand command = MakeBaseCommand(request, CoreCommandType::PreviewImageCapture);
    command.stream_id = ReadString(params, "stream", "main");
    if (command.stream_id.empty()) {
        return FieldError(request, "stream", "stream must not be empty");
    }
    return CloudActionMapResult::Ok(command);
}

CloudActionMapResult MapReboot(const CloudActionRequest& request,
                               const std::map<std::string, std::string>& params) {
    CoreCommand command = MakeBaseCommand(request, CoreCommandType::Reboot);
    if (!ReadInt(params, "delay", command.delay_sec, 0)) {
        return FieldError(request, "delay", "delay must be integer");
    }
    if (command.delay_sec < 0 || command.delay_sec > 3600) {
        return FieldError(request, "delay", "delay must be in range 0-3600");
    }
    return CloudActionMapResult::Ok(command);
}

} // namespace

CloudActionMapResult MapCloudAction(const CloudActionRequest& request) {
    std::map<std::string, std::string> params;
    if (!ParseKeyValuePairs(request.params, params)) {
        return Error(request, "action params must be key=value pairs");
    }

    if (request.action_id == "FormatStorageMedium") {
        return MapFormatStorageMedium(request, params);
    }
    if (request.action_id == "OneKeyAlarm") {
        return MapOneKeyAlarm(request, params);
    }
    if (request.action_id == "PreviewImageCapture") {
        return MapPreviewImageCapture(request, params);
    }
    if (request.action_id == "Reboot") {
        return MapReboot(request, params);
    }
    if (request.action_id == "SetDefaultDevConfigs") {
        return CloudActionMapResult::Ok(
            MakeBaseCommand(request, CoreCommandType::SetDefaultDevConfigs));
    }

    return CloudActionMapResult::Error(CloudConfigStatusCode::UnsupportedProperty,
                                       request.action_id,
                                       "unsupported cloud action");
}

} // namespace aov::app::cloud
