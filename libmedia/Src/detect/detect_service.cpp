#include "detect_service.hpp"

#include <chrono>
#include <cstring>

#ifdef LIBMEDIA_BUILD_AX615_ADAPTERS
#include "ax_ivps_api.h"
#include "ax_sys_api.h"
#endif

namespace aov::media {

MediaStatusCode DetectService::ApplyDetectConfig(const DetectApplyConfig& config) {
    if (config.pipe_id < 0 || config.ivps_grp_id < 0 || config.ivps_chn_id < 0) {
        return MediaStatusCode::InvalidArgument;
    }
    for (const auto& rule : config.rules) {
        if (rule.pipe_id < 0 || rule.ivps_grp_id < 0 || rule.ivps_chn_id < 0 ||
            rule.sensitivity < 0 || rule.sensitivity > 100) {
            return MediaStatusCode::InvalidArgument;
        }
    }
    config_ = config;
    return MediaStatusCode::Ok;
}

MediaStatusCode DetectService::RunOnceDetect(DetectResultSummary& result) {
    result = {};
    return run_once_handler_ ? run_once_handler_(result) : MediaStatusCode::Ok;
}

MediaStatusCode DetectService::StartDetect() {
    if (running_.exchange(true)) {
        return MediaStatusCode::Ok;
    }

#ifdef LIBMEDIA_BUILD_AX615_ADAPTERS
    MediaStatusCode status = InitSkel();
    if (status != MediaStatusCode::Ok) {
        running_ = false;
        return status;
    }
#endif

    thread_ = std::thread(&DetectService::DetectLoop, this);
    return MediaStatusCode::Ok;
}

void DetectService::StopDetect() {
    if (!running_.exchange(false)) {
        return;
    }
    if (thread_.joinable()) {
        thread_.join();
    }

#ifdef LIBMEDIA_BUILD_AX615_ADAPTERS
    DeinitSkel();
#endif
}

void DetectService::DetectLoop() {
#ifdef LIBMEDIA_BUILD_AX615_ADAPTERS
    // 使用真实的 SKEL 检测（参考 sample_vin_ivps_skel_venc_rtsp）
    while (running_) {
        MediaStatusCode status = GetFrameAndDetect();
        if (status != MediaStatusCode::Ok) {
            // 获取帧失败或超时，短暂休眠后重试
            std::this_thread::sleep_for(std::chrono::milliseconds(10));
        }
    }
#else
    // Stub 实现（用于非 AX615 平台）
    while (running_) {
        if (callback_) {
            DetectResultSummary result;
            const MediaStatusCode status = RunOnceDetect(result);
            if (status == MediaStatusCode::Ok) {
                callback_(result);
            }
        }
        std::this_thread::sleep_for(std::chrono::milliseconds(100));
    }
#endif
}

void DetectService::SetRunOnceHandler(std::function<MediaStatusCode(DetectResultSummary&)> handler) {
    run_once_handler_ = std::move(handler);
}

#ifdef LIBMEDIA_BUILD_AX615_ADAPTERS

MediaStatusCode DetectService::InitSkel() {
    // 初始化 SKEL（参考 sample_vin_ivps_skel_venc_rtsp.c:870）
    AX_SKEL_INIT_PARAM_T init_param;
    std::memset(&init_param, 0, sizeof(init_param));
    init_param.pStrModelDeploymentPath = "/opt/etc/skelModels";

    AX_S32 ret = AX_SKEL_Init(&init_param);
    if (ret != 0) {
        return MediaStatusCode::InternalError;
    }

    // 创建 SKEL handle
    AX_SKEL_HANDLE_PARAM_T handle_param;
    std::memset(&handle_param, 0, sizeof(handle_param));
    handle_param.ePPL = AX_SKEL_PPL_HVCP;  // HVCP pipeline
    handle_param.nFrameDepth = 1;
    handle_param.nFrameCacheDepth = 1;
    handle_param.nIoDepth = 0;
    handle_param.nWidth = 1920;   // TODO: 从 config_ 获取分辨率
    handle_param.nHeight = 1080;

    ret = AX_SKEL_Create(&handle_param, &skel_handle_);
    if (ret != 0) {
        AX_SKEL_DeInit();
        return MediaStatusCode::InternalError;
    }

    // 注册结果回调（参考 sample_vin_ivps_skel_venc_rtsp.c:891）
    ret = AX_SKEL_RegisterResultCallback(skel_handle_, SkelResultCallback, this);
    if (ret != 0) {
        AX_SKEL_Destroy(skel_handle_);
        AX_SKEL_DeInit();
        skel_handle_ = nullptr;
        return MediaStatusCode::InternalError;
    }

    return MediaStatusCode::Ok;
}

void DetectService::DeinitSkel() {
    if (skel_handle_) {
        AX_SKEL_Destroy(skel_handle_);
        skel_handle_ = nullptr;
    }
    AX_SKEL_DeInit();
}

MediaStatusCode DetectService::GetFrameAndDetect() {
    // 从 IVPS 获取帧（参考 sample_vin_ivps_skel_venc_rtsp.c:959）
    AX_VIDEO_FRAME_T video_frame;
    std::memset(&video_frame, 0, sizeof(video_frame));
    AX_S32 ret = AX_IVPS_GetChnFrame(config_.ivps_grp_id, config_.ivps_chn_id, &video_frame, 100);

    if (ret != 0) {
        return MediaStatusCode::Timeout;
    }

    // 填充虚拟地址和物理地址
    video_frame.u64VirAddr[0] = (AX_ULONG)AX_POOL_GetBlockVirAddr(video_frame.u32BlkId[0]);
    video_frame.u64PhyAddr[0] = AX_POOL_Handle2PhysAddr(video_frame.u32BlkId[0]);
    video_frame.u32FrameSize = video_frame.u32PicStride[0] * video_frame.u32Height * 3 / 2;

    // 发送帧到 SKEL 进行检测
    if (skel_handle_) {
        AX_SKEL_FRAME_T skel_frame;
        std::memset(&skel_frame, 0, sizeof(skel_frame));
        skel_frame.nFrameId = frame_id_++;
        skel_frame.stFrame = video_frame;

        ret = AX_SKEL_SendFrame(skel_handle_, &skel_frame, 0);
    }

    // 释放帧
    AX_IVPS_ReleaseChnFrame(config_.ivps_grp_id, config_.ivps_chn_id, &video_frame);

    return (ret == 0) ? MediaStatusCode::Ok : MediaStatusCode::InternalError;
}

void DetectService::SkelResultCallback(AX_SKEL_HANDLE handle, AX_SKEL_RESULT_T* result, void* user_data) {
    (void)handle;

    if (!result || !user_data) {
        return;
    }

    DetectService* service = static_cast<DetectService*>(user_data);

    // 解析 SKEL 结果（参考 sample_vin_ivps_skel_venc_rtsp.c:806）
    DetectResultSummary summary;
    summary.frame_id = result->nFrameId;
    summary.timestamp_ms = std::chrono::duration_cast<std::chrono::milliseconds>(
        std::chrono::system_clock::now().time_since_epoch()).count();

    // 初始化检测结果
    summary.has_human = false;
    summary.has_vehicle = false;
    summary.object_count = 0;

    // 解析检测到的对象
    if (result->pstObjectItems && result->nObjectSize > 0) {
        summary.object_count = static_cast<int>(result->nObjectSize);

        // 遍历检测到的对象，判断类型
        for (AX_U32 i = 0; i < result->nObjectSize; i++) {
            const AX_SKEL_OBJECT_ITEM_T& obj = result->pstObjectItems[i];

            // 根据类别字符串判断对象类型
            if (obj.pstrObjectCategory) {
                if (std::strcmp(obj.pstrObjectCategory, "person") == 0 ||
                    std::strcmp(obj.pstrObjectCategory, "body") == 0) {
                    summary.has_human = true;
                } else if (std::strcmp(obj.pstrObjectCategory, "vehicle") == 0 ||
                           std::strcmp(obj.pstrObjectCategory, "car") == 0) {
                    summary.has_vehicle = true;
                }
            }
        }
    }

    // 回调到 app 层
    if (service->callback_) {
        service->callback_(summary);
    }
}

#endif

}  // namespace aov::media
