#pragma once

#include "app/config/media_config.hpp"
#include "app/packet/packet_types.hpp"
#include "iv_cm.h"

namespace aov::app::cloud::tencent {

struct TencentVencPackResult {
    bool ok {false};
    iv_cm_venc_pack_s pack {};
};

struct TencentAvDataInfoResult {
    bool ok {false};
    iv_cm_av_data_info_s info {};
};

class TencentStreamAdapter {
public:
    TencentVencPackResult ToVencPack(const aov::app::packet::PacketFrame& frame) const;
    TencentAvDataInfoResult ToAvDataInfo(
        const aov::app::config::MediaEncodeConfig& config) const;

private:
    iv_cm_venc_type_e MapVideoCodec(aov::media::VideoCodec codec) const;
    iv_cm_venc_type_e MapVideoCodec(aov::app::config::VideoEncodeType codec) const;
    iv_cm_aenc_type_e MapAudioCodec(aov::app::config::AudioEncodeType codec) const;
    iv_cm_aenc_sample_rate_e MapSampleRate(int sample_rate_khz) const;
    iv_cm_aenc_bit_width_e MapBitWidth(int bit_depth) const;
};

} // namespace aov::app::cloud::tencent
