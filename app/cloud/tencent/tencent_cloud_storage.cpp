#include "app/cloud/tencent/tencent_cloud_storage.hpp"

#if defined(AOV_ENABLE_TENCENT_SDK)
#include "iv_cs.h"
#endif

namespace aov::app::cloud::tencent {

TencentCloudStatusCode TencentCloudStorage::StartEvent(
    const TencentCloudStorageSession& session) {
    if (session.event_id <= 0) {
        return TencentCloudStatusCode::InvalidArgument;
    }
    if (stats_.active) {
        return TencentCloudStatusCode::InvalidState;
    }

#if defined(AOV_ENABLE_TENCENT_SDK)
    const int ret = iv_cs_event_start_ext(session.channel,
                                          session.event_id,
                                          session.info.c_str(),
                                          session.event_start_time_s,
                                          session.stream_start_time_s);
    if (ret != 0) {
        stats_.last_sdk_error = ret;
        ++stats_.sdk_errors;
        return TencentCloudStatusCode::SdkError;
    }
#endif

    session_ = session;
    stats_.active = true;
    stats_.active_event_id = session.event_id;
    stats_.active_channel = session.channel;
    stats_.last_result = aov::app::cloud::CloudStorageResult::None;
    return TencentCloudStatusCode::Ok;
}

TencentCloudResult TencentCloudStorage::FinishEvent(
    const aov::app::cloud::CloudStorageResult result) {
    if (result == aov::app::cloud::CloudStorageResult::None) {
        return TencentCloudResult::Error(TencentCloudStatusCode::InvalidArgument,
                                         0,
                                         "cloud storage finish result is none");
    }
    if (!stats_.active) {
        stats_.last_result = result;
        return TencentCloudResult::Ok();
    }

#if defined(AOV_ENABLE_TENCENT_SDK)
    const int ret = iv_cs_event_stop_ext(session_.channel,
                                         session_.event_id,
                                         session_.info.c_str(),
                                         session_.event_start_time_s,
                                         session_.stream_start_time_s);
    if (ret != 0) {
        stats_.last_sdk_error = ret;
        ++stats_.sdk_errors;
        return TencentCloudResult::Error(TencentCloudStatusCode::SdkError,
                                         ret,
                                         "iv_cs_event_stop_ext failed");
    }
#endif

    stats_.active = false;
    stats_.last_result = result;
    return TencentCloudResult::Ok();
}

TencentCloudResult TencentCloudStorage::SendVideoPack(const iv_cm_venc_pack_s& pack) {
    if (!stats_.active) {
        ++stats_.dropped_frames;
        if (stats_.last_result == aov::app::cloud::CloudStorageResult::None) {
            return TencentCloudResult::Error(TencentCloudStatusCode::InvalidState,
                                             0,
                                             "cloud storage session is inactive");
        }
        return TencentCloudResult::Ok();
    }
    if (!pack.pu8Addr || pack.u32Len == 0) {
        ++stats_.dropped_frames;
        return TencentCloudResult::Error(TencentCloudStatusCode::InvalidArgument,
                                         0,
                                         "cloud storage video pack is empty");
    }

#if defined(AOV_ENABLE_TENCENT_SDK)
    iv_cm_venc_pack_s sdk_pack = pack;
    const int ret = iv_cs_push_stream(session_.channel,
                                      IV_CM_STREAM_TYPE_VIDEO,
                                      &sdk_pack);
    if (ret != 0) {
        stats_.last_sdk_error = ret;
        ++stats_.sdk_errors;
        return TencentCloudResult::Error(TencentCloudStatusCode::SdkError,
                                         ret,
                                         "iv_cs_push_stream video failed");
    }
#endif

    ++stats_.sent_video_frames;
    stats_.sent_video_bytes += pack.u32Len;
    stats_.last_video_pts_ms = pack.u64PTS;
    stats_.last_video_sequence = pack.u32Seq;
    return TencentCloudResult::Ok();
}

TencentCloudStorageStats TencentCloudStorage::GetStats() const {
    return stats_;
}

} // namespace aov::app::cloud::tencent
