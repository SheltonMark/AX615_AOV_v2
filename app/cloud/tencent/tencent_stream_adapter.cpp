#include "app/cloud/tencent/tencent_stream_adapter.hpp"

namespace aov::app::cloud::tencent {

TencentVencPackResult TencentStreamAdapter::ToVencPack(
    const aov::app::packet::PacketFrame& frame) const {
    TencentVencPackResult result;
    if (!frame.data() || frame.size() == 0) {
        return result;
    }

    result.ok = true;
    result.pack.pu8Addr = const_cast<std::uint8_t*>(frame.data());
    result.pack.u32Len = frame.size();
    result.pack.u64PTS = frame.pts_ms;
    result.pack.eFrameType = frame.is_keyframe ? IV_CM_FRAME_TYPE_I : IV_CM_FRAME_TYPE_P;
    result.pack.u32Seq = static_cast<std::uint32_t>(frame.sequence);
    return result;
}

TencentAvDataInfoResult TencentStreamAdapter::ToAvDataInfo(
    const aov::app::config::MediaEncodeConfig& config) const {
    TencentAvDataInfoResult result;
    if (config.main_stream.width <= 0 ||
        config.main_stream.height <= 0 ||
        config.main_stream.fps_normal <= 0) {
        return result;
    }

    result.ok = true;
    result.info.eAudioType = MapAudioCodec(config.audio.codec);
    result.info.u32AudioCodecOption = IV_CM_AAC_TYPE_NONE;
    result.info.eAudioMode = IV_CM_AENC_MODE_MONO;
    result.info.eAudioBitWidth = MapBitWidth(config.audio.bit_depth);
    result.info.eAudioSampleRate = MapSampleRate(config.audio.sample_rate_khz);
    result.info.u32SampleNumPerFrame = 0;
    result.info.eVideoType = MapVideoCodec(config.main_stream.codec);
    result.info.u32VideoWidth = static_cast<std::uint32_t>(config.main_stream.width);
    result.info.u32VideoHeight = static_cast<std::uint32_t>(config.main_stream.height);
    result.info.u32Framerate = static_cast<std::uint32_t>(config.main_stream.fps_normal);
    return result;
}

iv_cm_venc_type_e TencentStreamAdapter::MapVideoCodec(
    const aov::media::VideoCodec codec) const {
    switch (codec) {
        case aov::media::VideoCodec::H264:
            return IV_CM_VENC_TYPE_H264;
        case aov::media::VideoCodec::H265:
            return IV_CM_VENC_TYPE_H265;
        case aov::media::VideoCodec::JPEG:
            return IV_CM_VENC_TYPE_MJPG;
    }
    return IV_CM_VENC_TYPE_H264;
}

iv_cm_venc_type_e TencentStreamAdapter::MapVideoCodec(
    const aov::app::config::VideoEncodeType codec) const {
    switch (codec) {
        case aov::app::config::VideoEncodeType::H264:
            return IV_CM_VENC_TYPE_H264;
        case aov::app::config::VideoEncodeType::H265:
            return IV_CM_VENC_TYPE_H265;
    }
    return IV_CM_VENC_TYPE_H264;
}

iv_cm_aenc_type_e TencentStreamAdapter::MapAudioCodec(
    const aov::app::config::AudioEncodeType codec) const {
    switch (codec) {
        case aov::app::config::AudioEncodeType::G711A:
            return IV_CM_AENC_TYPE_G711A;
        case aov::app::config::AudioEncodeType::G711U:
            return IV_CM_AENC_TYPE_G711U;
        case aov::app::config::AudioEncodeType::AAC:
            return IV_CM_AENC_TYPE_AAC;
    }
    return IV_CM_AENC_TYPE_G711A;
}

iv_cm_aenc_sample_rate_e TencentStreamAdapter::MapSampleRate(
    const int sample_rate_khz) const {
    switch (sample_rate_khz) {
        case 8:
            return IV_CM_AENC_SAMPLE_RATE_8000;
        case 16:
            return IV_CM_AENC_SAMPLE_RATE_16000;
        case 32:
            return IV_CM_AENC_SAMPLE_RATE_32000;
        case 44:
            return IV_CM_AENC_SAMPLE_RATE_44100;
        case 48:
            return IV_CM_AENC_SAMPLE_RATE_48000;
        default:
            return IV_CM_AENC_SAMPLE_RATE_8000;
    }
}

iv_cm_aenc_bit_width_e TencentStreamAdapter::MapBitWidth(const int bit_depth) const {
    switch (bit_depth) {
        case 8:
            return IV_CM_AENC_BIT_WIDTH_8;
        case 16:
            return IV_CM_AENC_BIT_WIDTH_16;
        case 24:
            return IV_CM_AENC_BIT_WIDTH_24;
        default:
            return IV_CM_AENC_BIT_WIDTH_16;
    }
}

} // namespace aov::app::cloud::tencent
