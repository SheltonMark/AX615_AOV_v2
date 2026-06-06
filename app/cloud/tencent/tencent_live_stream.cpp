#include "app/cloud/tencent/tencent_live_stream.hpp"

#if defined(AOV_ENABLE_TENCENT_SDK)
#include "iv_av.h"
#endif

namespace aov::app::cloud::tencent {

TencentCloudStatusCode TencentLiveStream::Start(const TencentLiveSession& session) {
    if (session.visitor == 0) {
        return TencentCloudStatusCode::InvalidArgument;
    }

    session_ = session;
    stats_.active = true;
    return TencentCloudStatusCode::Ok;
}

TencentCloudResult TencentLiveStream::Stop() {
    if (!stats_.active) {
        return TencentCloudResult::Ok();
    }

#if defined(AOV_ENABLE_TENCENT_SDK)
    const int ret = iv_avt_send_finish_stream(session_.visitor,
                                              session_.channel,
                                              session_.video_res_type);
    if (ret != 0) {
        stats_.last_sdk_error = ret;
        ++stats_.sdk_errors;
        return TencentCloudResult::Error(TencentCloudStatusCode::SdkError,
                                         ret,
                                         "iv_avt_send_finish_stream failed");
    }
#endif

    stats_.active = false;
    return TencentCloudResult::Ok();
}

TencentCloudResult TencentLiveStream::SendVideoPack(const iv_cm_venc_pack_s& pack) {
    if (!stats_.active) {
        ++stats_.dropped_frames;
        return TencentCloudResult::Error(TencentCloudStatusCode::InvalidState,
                                         0,
                                         "live stream session is inactive");
    }
    if (!pack.pu8Addr || pack.u32Len == 0) {
        ++stats_.dropped_frames;
        return TencentCloudResult::Error(TencentCloudStatusCode::InvalidArgument,
                                         0,
                                         "video pack is empty");
    }

#if defined(AOV_ENABLE_TENCENT_SDK)
    iv_cm_venc_pack_s sdk_pack = pack;
    const int ret = iv_avt_send_stream(session_.visitor,
                                       session_.channel,
                                       session_.video_res_type,
                                       IV_AVT_STREAM_TYPE_VIDEO,
                                       &sdk_pack);
    if (ret != 0) {
        stats_.last_sdk_error = ret;
        ++stats_.sdk_errors;
        return TencentCloudResult::Error(TencentCloudStatusCode::SdkError,
                                         ret,
                                         "iv_avt_send_stream video failed");
    }
#endif

    ++stats_.sent_video_frames;
    stats_.sent_video_bytes += pack.u32Len;
    stats_.last_video_pts_ms = pack.u64PTS;
    stats_.last_video_sequence = pack.u32Seq;
    return TencentCloudResult::Ok();
}

TencentLiveStreamStats TencentLiveStream::GetStats() const {
    return stats_;
}

} // namespace aov::app::cloud::tencent
