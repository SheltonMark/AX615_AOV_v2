#include "ax_detect_adapter.hpp"

#include <cstring>

namespace aov::media::ax615 {
namespace {

bool IsBodyObject(const AX_SKEL_OBJECT_ITEM_T& item) {
    return item.pstrObjectCategory && std::strcmp(item.pstrObjectCategory, "body") == 0;
}

bool IsActiveTrack(const AX_SKEL_OBJECT_ITEM_T& item) {
    return item.eTrackState == AX_SKEL_TRACK_STATUS_NEW ||
           item.eTrackState == AX_SKEL_TRACK_STATUS_UPDATE;
}

}  // namespace

AxDetectAdapter::~AxDetectAdapter() {
    Close();
}

MediaStatusCode AxDetectAdapter::Open(const AxDetectConfig& cfg) {
    if (cfg.model_path.empty() || cfg.stream_id < 0 || cfg.frame_depth <= 0 || cfg.width <= 0 || cfg.height <= 0) {
        return MediaStatusCode::InvalidArgument;
    }
    if (opened_) {
        return MediaStatusCode::Ok;
    }

    AX_SKEL_INIT_PARAM_T init_param;
    std::memset(&init_param, 0, sizeof(init_param));
    init_param.pStrModelDeploymentPath = const_cast<AX_CHAR*>(cfg.model_path.c_str());
    AX_S32 ret = AX_SKEL_Init(&init_param);
    if (ret != AX_SUCCESS) {
        return MediaStatusCode::InternalError;
    }
    sdk_initialized_ = true;

    AX_SKEL_COMMON_ENABLE_CONFIG_T track_enable;
    AX_SKEL_COMMON_ENABLE_CONFIG_T push_enable;
    std::memset(&track_enable, 0, sizeof(track_enable));
    std::memset(&push_enable, 0, sizeof(push_enable));
    track_enable.bEnable = cfg.enable_track ? AX_TRUE : AX_FALSE;
    push_enable.bEnable = cfg.enable_push ? AX_TRUE : AX_FALSE;

    AX_SKEL_CONFIG_ITEM_T items[2];
    std::memset(items, 0, sizeof(items));
    items[0].pstrType = const_cast<AX_CHAR*>("track_enable");
    items[0].pstrValue = &track_enable;
    items[0].nValueSize = sizeof(track_enable);
    items[1].pstrType = const_cast<AX_CHAR*>("push_enable");
    items[1].pstrValue = &push_enable;
    items[1].nValueSize = sizeof(push_enable);

    AX_SKEL_CONFIG_T skel_cfg;
    std::memset(&skel_cfg, 0, sizeof(skel_cfg));
    skel_cfg.nSize = 2;
    skel_cfg.pstItems = items;

    AX_SKEL_HANDLE_PARAM_T handle_param;
    std::memset(&handle_param, 0, sizeof(handle_param));
    handle_param.ePPL = AX_SKEL_PPL_PCD;
    handle_param.nFrameDepth = static_cast<AX_U32>(cfg.frame_depth);
    handle_param.nFrameCacheDepth = 0;
    handle_param.nIoDepth = 0;
    handle_param.nWidth = static_cast<AX_U32>(cfg.width);
    handle_param.nHeight = static_cast<AX_U32>(cfg.height);
    handle_param.stConfig = skel_cfg;

    ret = AX_SKEL_Create(&handle_param, &handle_);
    if (ret != AX_SUCCESS) {
        Close();
        return MediaStatusCode::InternalError;
    }

    ret = AX_SKEL_RegisterResultCallback(handle_, &AxDetectAdapter::OnSkelResult, this);
    if (ret != AX_SUCCESS) {
        Close();
        return MediaStatusCode::InternalError;
    }

    cfg_ = cfg;
    opened_ = true;
	std::fprintf(stderr, "[AxDetectAdapter] SKEL Opened successfully\n");
    return MediaStatusCode::Ok;
}

void AxDetectAdapter::Close() {
    if (handle_) {
        AX_SKEL_Destroy(handle_);
        handle_ = nullptr;
    }
    if (sdk_initialized_) {
        AX_SKEL_DeInit();
        sdk_initialized_ = false;
    }
    opened_ = false;
    cfg_ = {};
    callback_ = {};
}

MediaStatusCode AxDetectAdapter::RunOnce(DetectResultSummary& result) {
    result = {};
    return opened_ ? MediaStatusCode::Unsupported : MediaStatusCode::InvalidState;
}

MediaStatusCode AxDetectAdapter::SendFrame(int stream_id, uint64_t frame_id, const AX_VIDEO_FRAME_T& frame, int timeout_ms) {
    if (!opened_ || !handle_) {
        return MediaStatusCode::InvalidState;
    }
    if (stream_id < 0 || timeout_ms < 0) {
        return MediaStatusCode::InvalidArgument;
    }

	static int count = 0; if (++count <= 5) std::fprintf(stderr, "[AxDetectAdapter] SendFrame #%d\n", count);

    AX_SKEL_FRAME_T skel_frame;
    std::memset(&skel_frame, 0, sizeof(skel_frame));
    skel_frame.nStreamId = static_cast<AX_U32>(stream_id);
    skel_frame.nFrameId = frame_id;
    skel_frame.stFrame = frame;

    return AX_SKEL_SendFrame(handle_, &skel_frame, timeout_ms) == AX_SUCCESS
        ? MediaStatusCode::Ok
        : MediaStatusCode::InternalError;
}

void AxDetectAdapter::SetResultCallback(AxDetectCallback cb) {
    callback_ = std::move(cb);
}

void AxDetectAdapter::OnSkelResult(AX_SKEL_HANDLE, AX_SKEL_RESULT_T* result, AX_VOID* user_data) {
	static int count = 0; if (++count <= 5) 
	std::fprintf(stderr, "[AxDetectAdapter] OnSkelResult #%d, obj=%u\n", count, result ? result->nObjectSize : 0);
    if (!result || !user_data) {
        return;
    }
    auto* self = static_cast<AxDetectAdapter*>(user_data);
    if (self->callback_) {
        self->callback_(ToSummary(*result));
    }
}

DetectResultSummary AxDetectAdapter::ToSummary(const AX_SKEL_RESULT_T& result) {
    DetectResultSummary summary;
    summary.frame_id = result.nFrameId;
    summary.object_count = static_cast<int>(result.nObjectSize);
    for (AX_U32 i = 0; i < result.nObjectSize; ++i) {
        const AX_SKEL_OBJECT_ITEM_T& item = result.pstObjectItems[i];
        if (IsBodyObject(item) && IsActiveTrack(item)) {
            summary.has_human = true;
            DetectObject object;
            object.type = DetectTargetType::Human;
            object.confidence = 0;
            summary.objects.push_back(object);
        }
    }
    return summary;
}

}  // namespace aov::media::ax615
