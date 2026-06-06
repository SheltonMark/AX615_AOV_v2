#include "ax_audio_capture_adapter.hpp"

#include <cstring>

extern "C" {
#include "ax_aac.h"
#include "ax_aenc_api.h"
#include "ax_ai_api.h"
#include "ax_sys_api.h"
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

AX_AUDIO_SOUND_MODE_E ToSoundMode(int channels) {
    return channels > 1 ? AX_AUDIO_SOUND_MODE_STEREO : AX_AUDIO_SOUND_MODE_MONO;
}

AX_AI_LAYOUT_MODE_E ToLayoutMode(int channels) {
    return channels > 1 ? AX_AI_MIC_MIC : AX_AI_MIC_REF;
}

}  // namespace

AxAudioCaptureAdapter::~AxAudioCaptureAdapter() {
    Close();
}

MediaStatusCode AxAudioCaptureAdapter::Open(const AxAudioCaptureConfig& cfg) {
    if (opened_) {
        return MediaStatusCode::Ok;
    }
    if (cfg.card_id < 0 || cfg.device_id < 0 || cfg.aenc_chn_id < 0 || cfg.sample_rate <= 0 || cfg.channels <= 0) {
        return MediaStatusCode::InvalidArgument;
    }
    cfg_ = cfg;

    AX_S32 ret = AX_AI_Init();
    if (ret != AX_SUCCESS) {
        return MediaStatusCode::InternalError;
    }
    ai_initialized_ = true;

    AX_AI_ATTR_T ai_attr;
    std::memset(&ai_attr, 0, sizeof(ai_attr));
    ai_attr.enSamplerate = static_cast<AX_AUDIO_SAMPLE_RATE_E>(cfg.sample_rate);
    ai_attr.enBitwidth = AX_AUDIO_BIT_WIDTH_16;
    ai_attr.u32ChnCnt = static_cast<AX_U32>(cfg.channels);
    ai_attr.u32PeriodSize = cfg.channels > 1 && cfg.codec == AudioCodec::AAC ? 1280 : 1024;
    ai_attr.u32PeriodCount = 8;
    ai_attr.U32Depth = 30;
    ai_attr.enLinkMode = AX_LINK_MODE;
    ai_attr.enLayoutMode = ToLayoutMode(cfg.channels);
    ret = AX_AI_SetPubAttr(cfg.card_id, cfg.device_id, &ai_attr);
    if (ret != AX_SUCCESS) {
        Close();
        return MediaStatusCode::InternalError;
    }

    ret = AX_AENC_Init();
    if (ret != AX_SUCCESS) {
        Close();
        return MediaStatusCode::InternalError;
    }
    aenc_initialized_ = true;

    AX_AENC_AAC_ENCODER_ATTR_T aac_attr;
    std::memset(&aac_attr, 0, sizeof(aac_attr));
    aac_attr.enAacType = AX_AAC_TYPE_AAC_LC;
    aac_attr.enTransType = AX_AAC_TRANS_TYPE_ADTS;
    aac_attr.enChnMode = cfg.channels > 1 ? AX_AAC_CHANNEL_MODE_2 : AX_AAC_CHANNEL_MODE_1;

    AX_AENC_CHN_ATTR_T aenc_attr;
    std::memset(&aenc_attr, 0, sizeof(aenc_attr));
    aenc_attr.enType = ToPayload(cfg.codec);
    aenc_attr.u32PtNumPerFrm = static_cast<AX_U32>(cfg.sample_rate / 50);
    aenc_attr.u32BufSize = 8;
    aenc_attr.enLinkMode = AX_LINK_MODE;
    aenc_attr.pValue = cfg.codec == AudioCodec::AAC ? &aac_attr : nullptr;

    ret = AX_AENC_CreateChn(cfg.aenc_chn_id, &aenc_attr);
    if (ret != AX_SUCCESS) {
        Close();
        return MediaStatusCode::InternalError;
    }
    aenc_created_ = true;

    AX_MOD_INFO_T src;
    AX_MOD_INFO_T dst;
    std::memset(&src, 0, sizeof(src));
    std::memset(&dst, 0, sizeof(dst));
    src.enModId = AX_ID_AI;
    src.s32GrpId = cfg.card_id;
    src.s32ChnId = cfg.device_id;
    dst.enModId = AX_ID_AENC;
    dst.s32GrpId = 0;
    dst.s32ChnId = cfg.aenc_chn_id;
    AX_SYS_Link(&src, &dst);

    opened_ = true;
    return MediaStatusCode::Ok;
}

MediaStatusCode AxAudioCaptureAdapter::Start() {
    if (!opened_) {
        return MediaStatusCode::InvalidState;
    }
    if (!ai_enabled_) {
        if (AX_AI_EnableDev(cfg_.card_id, cfg_.device_id) != AX_SUCCESS) {
            return MediaStatusCode::InternalError;
        }
        ai_enabled_ = true;
    }
    thread_running_.store(true);
    stream_thread_ = std::thread([this]() { CaptureLoop(); });
    running_ = true;
    return MediaStatusCode::Ok;
}

void AxAudioCaptureAdapter::Stop() {
    thread_running_.store(false);
    if (stream_thread_.joinable()) {
        stream_thread_.join();
    }
    if (ai_enabled_) {
        AX_AI_DisableDev(cfg_.card_id, cfg_.device_id);
        ai_enabled_ = false;
    }
    running_ = false;
}

void AxAudioCaptureAdapter::Close() {
    Stop();
    if (aenc_created_) {
        AX_AENC_DestroyChn(cfg_.aenc_chn_id);
        aenc_created_ = false;
    }
    if (aenc_initialized_) {
        AX_AENC_DeInit();
        aenc_initialized_ = false;
    }
    if (ai_initialized_) {
        AX_AI_DeInit();
        ai_initialized_ = false;
    }
    opened_ = false;
    cfg_ = {};
    callback_ = {};
}

void AxAudioCaptureAdapter::SetFrameCallback(AxAudioFrameCallback cb) {
    callback_ = std::move(cb);
}

void AxAudioCaptureAdapter::CaptureLoop() {
    while (thread_running_.load()) {
        AX_AUDIO_STREAM_T stream;
        std::memset(&stream, 0, sizeof(stream));
        AX_S32 ret = AX_AENC_GetStream(cfg_.aenc_chn_id, &stream, 100);
        if (ret != AX_SUCCESS) {
            continue;
        }
        if (stream.pStream && stream.u32Len > 0 && callback_) {
            AudioFrame frame;
            frame.chn_id = cfg_.aenc_chn_id;
            frame.codec = cfg_.codec;
            frame.data = stream.pStream;
            frame.size = stream.u32Len;
            frame.pts_ms = stream.u64TimeStamp / 1000;
            frame.sequence = stream.u32Seq;
            frame.sample_rate_hz = cfg_.sample_rate;
            frame.channels = cfg_.channels;
            callback_(frame);
        }
        AX_AENC_ReleaseStream(cfg_.aenc_chn_id, &stream);
    }
}

}  // namespace aov::media::ax615
