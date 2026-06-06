#pragma once

#include "app/cloud/tencent/tencent_cloud_types.hpp"

namespace aov::app::cloud::tencent {

class TencentSdkAdapter {
public:
    TencentCloudResult InitSystem(const TencentCloudInitConfig& config);
    TencentCloudResult InitDataModel();
    TencentCloudResult InitAvTransmission(const TencentCloudInitConfig& config);
    TencentCloudResult ExitAvTransmission();
    TencentCloudResult ExitDataModel();
    TencentCloudResult ExitSystem();
    TencentCloudResult ReportProperty(const char* property_key);
    TencentCloudResult ReportEvent(const char* event_name);

    bool IsSystemReady() const { return system_ready_; }
    bool IsDataModelReady() const { return data_model_ready_; }
    bool IsAvTransmissionReady() const { return av_ready_; }

private:
    bool system_ready_ {false};
    bool data_model_ready_ {false};
    bool av_ready_ {false};
};

} // namespace aov::app::cloud::tencent
