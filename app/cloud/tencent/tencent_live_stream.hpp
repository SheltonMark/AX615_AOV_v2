#pragma once

#include <cstdint>

#include "app/cloud/tencent/tencent_cloud_types.hpp"
#include "iv_av.h"
#include "iv_cm.h"

namespace aov::app::cloud::tencent {

struct TencentLiveSession {
    std::uint32_t visitor {0};
    std::uint32_t channel {0};
    iv_avt_video_res_type_e video_res_type {IV_AVT_VIDEO_RES_HD};
};

struct TencentLiveStreamStats {
    bool active {false};
    std::uint64_t sent_video_frames {0};
    std::uint64_t sent_video_bytes {0};
    std::uint64_t dropped_frames {0};
    std::uint64_t sdk_errors {0};
    std::uint64_t last_video_pts_ms {0};
    std::uint32_t last_video_sequence {0};
    int last_sdk_error {0};
};

class TencentLiveStream final {
public:
    TencentCloudStatusCode Start(const TencentLiveSession& session);
    TencentCloudResult Stop();
    TencentCloudResult SendVideoPack(const iv_cm_venc_pack_s& pack);

    TencentLiveStreamStats GetStats() const;

private:
    TencentLiveSession session_ {};
    TencentLiveStreamStats stats_ {};
};

} // namespace aov::app::cloud::tencent
