#include "ax_audio_play_adapter.hpp"

#include <cstring>

extern "C" {
#include "ax_aac.h"
#include "ax_adec_api.h"
#include "ax_ao_api.h"
}

namespace aov::media::ax615 {
namespace {

AX_PAYLOAD_TYPE_E ToPayload(AudioCodec codec) {
    switch (codec) {
        case AudioCodec::AAC:
            return PT_AAC;
        case AudioCodec::G711U:
            return PT_G711U;
        case AudioCodec::G711A:
        default:
            return PT_G711A;
    }
}

}  // namespace

AxAudioPlayAdapter::~AxAudioPlayAdapter() {
    Close();
}

MediaStatusCode AxAudioPlayAdapter::Open(const AxAudioPlayConfig& cfg) {
    if (opened_) {
        return MediaStatusCode::Ok;
    }
    if (cfg.card_id < 0 || cfg.device_id < 0 || cfg.adec_chn_id < 0 || cfg.sample_rate <= 0 || cfg.channels <= 0) {
        return MediaStatusCode::InvalidArgument;
    }
    cfg_ = cfg;

    AX_S32 ret = AX_ADEC_Init();
    if (ret != AX_SUCCESS) {
        return MediaStatusCode::InternalError;
    }
    adec_initialized_ = true;

    AX_ADEC_AAC_DECODER_ATTR_T aac_attr;
    std::memset(&aac_attr, 0, sizeof(aac_attr));
    aac_attr.enTransType = AX_AAC_TRANS_TYPE_ADTS;

    AX_ADEC_CHN_ATTR_T adec_attr;
    std::memset(&adec_attr, 0, sizeof(adec_attr));
    adec_attr.enType = ToPayload(cfg.codec);
    adec_attr.u32BufSize = 8;
    adec_attr.enMode = AX_ADEC_MODE_PACK;
    adec_attr.enLinkMode = AX_LINK_MODE;
    adec_attr.pValue = cfg.codec == AudioCodec::AAC ? &aac_attr : nullptr;
    ret = AX_ADEC_CreateChn(cfg.adec_chn_id, &adec_attr);
    if (ret != AX_SUCCESS) {
        Close();
        return MediaStatusCode::InternalError;
    }
    adec_created_ = true;

    ret = AX_AO_Init();
    if (ret != AX_SUCCESS) {
        Close();
        return MediaStatusCode::InternalError;
    }
    ao_initialized_ = true;

    AX_AO_ATTR_T ao_attr;
    std::memset(&ao_attr, 0, sizeof(ao_attr));
    ao_attr.enSamplerate = static_cast<AX_AUDIO_SAMPLE_RATE_E>(cfg.sample_rate);
    ao_attr.enBitwidth = AX_AUDIO_BIT_WIDTH_16;
    ao_attr.enSoundmode = cfg.channels > 1 ? AX_AUDIO_SOUND_MODE_STEREO : AX_AUDIO_SOUND_MODE_MONO;
    ao_attr.u32ChnCnt = static_cast<AX_U32>(cfg.channels);
    ao_attr.u32PeriodSize = 1024;
    ao_attr.u32PeriodCount = 8;
    ao_attr.U32Depth = 30;
    ao_attr.enLinkMode = AX_LINK_MODE;
    ret = AX_AO_SetPubAttr(cfg.card_id, cfg.device_id, &ao_attr);
    if (ret != AX_SUCCESS) {
        Close();
        return MediaStatusCode::InternalError;
    }
    ret = AX_AO_EnableDev(cfg.card_id, cfg.device_id);
    if (ret != AX_SUCCESS) {
        Close();
        return MediaStatusCode::InternalError;
    }
    ao_enabled_ = true;

    opened_ = true;
    return MediaStatusCode::Ok;
}

MediaStatusCode AxAudioPlayAdapter::Play(const AudioFrame& frame) {
    if (!opened_) {
        return MediaStatusCode::InvalidState;
    }
    if (!frame.data || frame.size == 0) {
        return MediaStatusCode::InvalidArgument;
    }
    AX_AUDIO_STREAM_T stream;
    std::memset(&stream, 0, sizeof(stream));
    stream.pStream = const_cast<AX_U8*>(frame.data);
    stream.u32Len = frame.size;
    stream.u64TimeStamp = frame.pts_ms * 1000;
    stream.u32Seq = static_cast<AX_U32>(frame.sequence);
    return AX_ADEC_SendStream(cfg_.adec_chn_id, &stream, AX_TRUE) == AX_SUCCESS
        ? MediaStatusCode::Ok
        : MediaStatusCode::InternalError;
}

void AxAudioPlayAdapter::Close() {
    if (ao_enabled_) {
        AX_AO_DisableDev(cfg_.card_id, cfg_.device_id);
        ao_enabled_ = false;
    }
    if (ao_initialized_) {
        AX_AO_DeInit();
        ao_initialized_ = false;
    }
    if (adec_created_) {
        AX_ADEC_DestroyChn(cfg_.adec_chn_id);
        adec_created_ = false;
    }
    if (adec_initialized_) {
        AX_ADEC_DeInit();
        adec_initialized_ = false;
    }
    opened_ = false;
    cfg_ = {};
}

}  // namespace aov::media::ax615
