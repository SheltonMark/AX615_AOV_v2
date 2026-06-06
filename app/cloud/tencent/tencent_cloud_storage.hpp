#pragma once

#include <cstdint>
#include <string>

#include "app/cloud/cloud_types.hpp"
#include "app/cloud/tencent/tencent_cloud_types.hpp"
#include "iv_cm.h"
#include "iv_cs.h"

namespace aov::app::cloud::tencent {

struct TencentCloudStorageSession {
    int event_id {0};
    iv_cs_chn_e channel {CS_SINGLE_CH};
    std::uint32_t event_start_time_s {0};
    std::uint32_t stream_start_time_s {0};
    std::string info;
};

struct TencentCloudStorageStats {
    bool active {false};
    int active_event_id {0};
    iv_cs_chn_e active_channel {CS_SINGLE_CH};
    std::uint64_t sent_video_frames {0};
    std::uint64_t sent_video_bytes {0};
    std::uint64_t dropped_frames {0};
    std::uint64_t sdk_errors {0};
    std::uint64_t last_video_pts_ms {0};
    std::uint32_t last_video_sequence {0};
    int last_sdk_error {0};
    aov::app::cloud::CloudStorageResult last_result {
        aov::app::cloud::CloudStorageResult::None};
};

class TencentCloudStorage final {
public:
    TencentCloudStatusCode StartEvent(const TencentCloudStorageSession& session);
    TencentCloudResult FinishEvent(aov::app::cloud::CloudStorageResult result);
    TencentCloudResult SendVideoPack(const iv_cm_venc_pack_s& pack);

    TencentCloudStorageStats GetStats() const;

private:
    TencentCloudStorageSession session_ {};
    TencentCloudStorageStats stats_ {};
};

} // namespace aov::app::cloud::tencent
