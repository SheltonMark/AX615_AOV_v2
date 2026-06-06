#include "app/cloud/tencent/tencent_sdk_adapter.hpp"

#include <cstdio>
#include <cstring>

#if defined(AOV_ENABLE_TENCENT_SDK)
#include "iv_av.h"
#include "iv_dm.h"
#include "iv_sys.h"

extern "C" iv_sys_os_impl_t os_impl;
#endif

namespace aov::app::cloud::tencent {

TencentCloudResult TencentSdkAdapter::InitSystem(const TencentCloudInitConfig& config) {
#if defined(AOV_ENABLE_TENCENT_SDK)
    iv_sys_device_info device_info {};
    device_info.product_id = config.product_id.c_str();
    device_info.device_name = config.device_name.c_str();
    device_info.region = nullptr;
    device_info.product_secret = nullptr;
    device_info.device_key = config.device_secret.c_str();

    iv_sys_init_parm_s parm {};
    std::snprintf(parm.sys_cache_path,
                  sizeof(parm.sys_cache_path),
                  "%s",
                  config.sys_cache_path.c_str());
    std::snprintf(parm.sys_store_path,
                  sizeof(parm.sys_store_path),
                  "%s",
                  config.sys_store_path.c_str());
    parm.dev_info_path = config.dev_info_path.c_str();
    parm.device_info = &device_info;
    parm.command_timeout = config.command_timeout_ms;
    parm.keep_alive_ms = config.keep_alive_ms;
    parm.mqtt_ping_interval_ms = config.mqtt_ping_interval_ms;
    parm.auto_connect_enable = 1;
    parm.mqtt_recv_buf_max_size = 2048;
    parm.mqtt_write_buf_max_size = 2048;
    parm.max_channel_num = config.max_channel_num;
    parm.comm_use_http = 0;
    parm.p2p_use_proxy = 0;
    parm.os_impl_cb = os_impl;

    const int ret = iv_sys_init(&parm);
    if (ret != 0) {
        return TencentCloudResult::Error(TencentCloudStatusCode::SdkError,
                                         ret,
                                         "iv_sys_init failed");
    }
#else
    (void)config;
#endif
    system_ready_ = true;
    return TencentCloudResult::Ok();
}

TencentCloudResult TencentSdkAdapter::InitDataModel() {
#if defined(AOV_ENABLE_TENCENT_SDK)
    iv_dm_init_parm_s parm {};
    const int ret = iv_dm_init(&parm);
    if (ret != 0) {
        return TencentCloudResult::Error(TencentCloudStatusCode::SdkError,
                                         ret,
                                         "iv_dm_init failed");
    }
#endif
    data_model_ready_ = true;
    return TencentCloudResult::Ok();
}

TencentCloudResult TencentSdkAdapter::InitAvTransmission(
    const TencentCloudInitConfig& config) {
    if (!system_ready_) {
        return TencentCloudResult::Error(TencentCloudStatusCode::InvalidState,
                                         0,
                                         "Tencent system is not initialized");
    }
    if (av_ready_) {
        return TencentCloudResult::Ok();
    }

#if defined(AOV_ENABLE_TENCENT_SDK)
    iv_avt_init_parm_s parm {};
    parm.max_frame_size = 512;
    parm.max_connect_num = config.max_channel_num == 0 ? 1 : config.max_channel_num;
    parm.p2p_init_params.sender_interval_ms = 10;
    parm.p2p_init_params.mtu_size = 1200;
    parm.p2p_keep_alive.time_inter_s = 10;
    parm.p2p_keep_alive.max_attempt_num = 3;

    const int ret = iv_avt_init(&parm);
    if (ret != 0) {
        return TencentCloudResult::Error(TencentCloudStatusCode::SdkError,
                                         ret,
                                         "iv_avt_init failed");
    }
#else
    (void)config;
#endif

    av_ready_ = true;
    return TencentCloudResult::Ok();
}

TencentCloudResult TencentSdkAdapter::ExitAvTransmission() {
    if (!av_ready_) {
        return TencentCloudResult::Ok();
    }

#if defined(AOV_ENABLE_TENCENT_SDK)
    const int ret = iv_avt_exit();
    if (ret != 0) {
        return TencentCloudResult::Error(TencentCloudStatusCode::SdkError,
                                         ret,
                                         "iv_avt_exit failed");
    }
#endif

    av_ready_ = false;
    return TencentCloudResult::Ok();
}

TencentCloudResult TencentSdkAdapter::ExitDataModel() {
#if defined(AOV_ENABLE_TENCENT_SDK)
    const int ret = iv_dm_exit();
    if (ret != 0) {
        return TencentCloudResult::Error(TencentCloudStatusCode::SdkError,
                                         ret,
                                         "iv_dm_exit failed");
    }
#endif
    data_model_ready_ = false;
    return TencentCloudResult::Ok();
}

TencentCloudResult TencentSdkAdapter::ExitSystem() {
#if defined(AOV_ENABLE_TENCENT_SDK)
    const int ret = iv_sys_exit();
    if (ret != 0) {
        return TencentCloudResult::Error(TencentCloudStatusCode::SdkError,
                                         ret,
                                         "iv_sys_exit failed");
    }
#endif
    system_ready_ = false;
    return TencentCloudResult::Ok();
}

TencentCloudResult TencentSdkAdapter::ReportProperty(const char* property_key) {
    if (!property_key) {
        return TencentCloudResult::Error(TencentCloudStatusCode::InvalidArgument,
                                         0,
                                         "property key is null");
    }
#if defined(AOV_ENABLE_TENCENT_SDK)
    const int ret = iv_dm_property_report(property_key, nullptr, nullptr);
    if (ret != 0) {
        return TencentCloudResult::Error(TencentCloudStatusCode::SdkError,
                                         ret,
                                         "iv_dm_property_report failed");
    }
#endif
    return TencentCloudResult::Ok();
}

TencentCloudResult TencentSdkAdapter::ReportEvent(const char* event_name) {
    if (!event_name) {
        return TencentCloudResult::Error(TencentCloudStatusCode::InvalidArgument,
                                         0,
                                         "event name is null");
    }
#if defined(AOV_ENABLE_TENCENT_SDK)
    const int ret = iv_dm_event_report(event_name);
    if (ret != 0) {
        return TencentCloudResult::Error(TencentCloudStatusCode::SdkError,
                                         ret,
                                         "iv_dm_event_report failed");
    }
#endif
    return TencentCloudResult::Ok();
}

} // namespace aov::app::cloud::tencent
