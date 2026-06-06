#include "app/cloud/tencent/tencent_generated_callback_bridge.hpp"

#include <algorithm>
#include <array>

namespace aov::app::cloud::tencent {
namespace {

bool Contains(const std::string& id, const char* const* begin, const char* const* end) {
    return std::find_if(begin, end, [&id](const char* item) {
               return id == item;
           }) != end;
}

bool IsSupportedProperty(const std::string& property_id) {
    static constexpr const char* kProperties[] = {
        "VideoFPS",
        "VideoBitrate",
        "VideoResolution",
        "VideoEncoding",
        "CloudStorageSwitch",
        "StorageRecordMode",
        "HumanDetectConfig",
        "ImageDetectConfig",
        "SoundLightAlarmConfig",
    };
    return Contains(property_id, std::begin(kProperties), std::end(kProperties));
}

bool IsSupportedAction(const std::string& action_id) {
    static constexpr const char* kActions[] = {
        "OneKeyAlarm",
        "FormatStorageMedium",
        "Reboot",
        "PreviewImageCapture",
        "SetDefaultDevConfigs",
    };
    return Contains(action_id, std::begin(kActions), std::end(kActions));
}

template <typename T>
aov::app::cloud::CloudConfigResult<T> Unsupported(const std::string& field,
                                                  const std::string& value) {
    return aov::app::cloud::CloudConfigResult<T>::Error(
        aov::app::cloud::CloudConfigStatusCode::UnsupportedProperty,
        field,
        "unsupported tencent generated callback id: " + value);
}

} // namespace

aov::app::cloud::CloudConfigResult<aov::app::cloud::CloudPropertyUpdate>
TencentGeneratedCallbackBridge::BuildPropertyUpdate(const std::string& property_id,
                                                    const std::string& value,
                                                    const std::uint64_t cloud_version,
                                                    const std::uint64_t recv_ts_ms) const {
    if (!IsSupportedProperty(property_id)) {
        return Unsupported<aov::app::cloud::CloudPropertyUpdate>("property_id", property_id);
    }
    return product_model_bridge_.BuildPropertyUpdate(property_id,
                                                     value,
                                                     cloud_version,
                                                     recv_ts_ms);
}

aov::app::cloud::CloudConfigResult<aov::app::cloud::CloudActionRequest>
TencentGeneratedCallbackBridge::BuildActionRequest(const std::string& action_id,
                                                   const std::string& params,
                                                   const std::string& request_id,
                                                   const std::uint64_t recv_ts_ms) const {
    if (!IsSupportedAction(action_id)) {
        return Unsupported<aov::app::cloud::CloudActionRequest>("action_id", action_id);
    }
    return product_model_bridge_.BuildActionRequest(action_id,
                                                    params,
                                                    request_id,
                                                    recv_ts_ms);
}

} // namespace aov::app::cloud::tencent
