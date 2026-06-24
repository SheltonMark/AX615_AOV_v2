#include "app/core/aov_app_runtime.hpp"

#include <string>

#include "libmedia/Include/detect/i_detect_service.hpp"
#include "libmedia/Include/video/i_video_stream_service.hpp"

namespace aov::app::core {

AovAppRuntime::AovAppRuntime(config::ConfigServiceStub& config,
                             IAovOrchestrator& orchestrator,
                             ICloudService& cloud,
                             AlarmServiceStub& alarm,
                             packet::IPacketService& packet,
                             storage::IStorageService& storage)
    : config_(config),
      orchestrator_(orchestrator),
      cloud_(cloud),
      alarm_(alarm),
      packet_(packet),
      storage_(storage),
      router_(packet),
      cloud_command_bridge_(command_queue_) {
}

AovAppRuntime::AovAppRuntime(config::ConfigServiceStub& config,
                             IAovOrchestrator& orchestrator,
                             ICloudService& cloud,
                             AlarmServiceStub& alarm,
                             packet::IPacketService& packet,
                             storage::IStorageService& storage,
                             aov::media::IMediaRuntime& media_runtime)
    : config_(config),
      orchestrator_(orchestrator),
      cloud_(cloud),
      alarm_(alarm),
      packet_(packet),
      storage_(storage),
      media_runtime_(&media_runtime),
      router_(packet),
      cloud_command_bridge_(command_queue_) {
}

AovStatusCode AovAppRuntime::Init() {
    if (status_.initialized) {
        return AovStatusCode::Ok;
    }

    AovStatusCode status = ToAovStatus(config_.LoadFactoryConfig());
    if (status != AovStatusCode::Ok) {
        return status;
    }

    status = orchestrator_.Init(config_.GetActiveWorkModePolicy());
    if (status != AovStatusCode::Ok) {
        return status;
    }

    status = ToAovStatus(packet_.Init(packet::PacketConfig {}));
    if (status != AovStatusCode::Ok) {
        return status;
    }

    status = ToAovStatus(storage_.Init(storage::StorageConfig {}));
    if (status != AovStatusCode::Ok) {
        packet_.Deinit();
        return status;
    }

    status = ToAovStatus(storage::BindStorageToPacket(router_, storage_));
    if (status != AovStatusCode::Ok) {
        storage_.Deinit();
        packet_.Deinit();
        return status;
    }

    const auto cloud_bind_result = cloud::BindCloudToPacket(router_, cloud_packet_sink_);
    if (!cloud_bind_result.ok()) {
        storage_.Deinit();
        packet_.Deinit();
        return AovStatusCode::InvalidState;
    }

    status = BindMediaToPacket();
    if (status != AovStatusCode::Ok) {
        storage_.Deinit();
        packet_.Deinit();
        return status;
    }

    // 注册 libmedia 检测回调
    if (media_runtime_ != nullptr) {
        auto* detect_service = media_runtime_->GetDetectService();
        if (detect_service != nullptr) {
            detect_service->RegisterDetectCallback(
                [this](const aov::media::DetectResultSummary& result) {
                    // 将 libmedia 的检测结果转发到 app 层
                    aov::app::DetectResultSummary app_result;
                    app_result.has_person = result.has_human;  // libmedia 用 has_human
                    app_result.has_pet = false;  // libmedia 没有 pet 检测
                    app_result.has_vehicle = result.has_vehicle;
                    app_result.object_count = result.object_count;
                    app_result.frame_id = result.frame_id;
                    app_result.timestamp_ms = result.timestamp_ms;

                    // 触发 app 层的检测处理
                    this->OnDetectResult(app_result);
                }
            );
        }
    }

    (void)alarm_;
    status_.initialized = true;
    return AovStatusCode::Ok;
}

AovStatusCode AovAppRuntime::Start() {
    if (!status_.initialized) {
        const AovStatusCode status = Init();
        if (status != AovStatusCode::Ok) {
            return status;
        }
    }
    if (status_.started) {
        return AovStatusCode::Ok;
    }

    AovStatusCode status = cloud_.Start();
    if (status != AovStatusCode::Ok) {
        return status;
    }

    status = orchestrator_.Start();
    if (status != AovStatusCode::Ok) {
        cloud_.Stop();
        return status;
    }

    status = cloud_.ReportDeviceState();
    if (status != AovStatusCode::Ok) {
        orchestrator_.Stop();
        cloud_.Stop();
        return status;
    }

    status_.started = true;
    return AovStatusCode::Ok;
}

AovStatusCode AovAppRuntime::Stop() {
    if (status_.started) {
        storage_.FlushAndClose();
        orchestrator_.Stop();
        cloud_.Stop();
    }

    storage_.Deinit();
    packet_.Deinit();
    status_.started = false;
    status_.initialized = false;
    return AovStatusCode::Ok;
}

AovStatusCode AovAppRuntime::OnWakeupEvent(const WakeupEvent& event) {
    if (!status_.initialized) {
        return AovStatusCode::InvalidState;
    }

    // 通知 orchestrator 处理唤醒事件（生成决策计划）
    AovStatusCode status = orchestrator_.OnWakeupEvent(event);
    if (status != AovStatusCode::Ok) {
        return status;
    }

    // 获取决策计划（从 orchestrator 的内部状态推断）
    // 注意：这里简化处理，实际应该从 orchestrator 返回 plan
    // 但为了不改接口，我们根据唤醒源判断

    if (event.source == WakeupSource::Timer) {
        // 定时器唤醒 → 进入低功耗模式（1fps 录像 + 检测）
        if (media_runtime_ != nullptr) {
            aov::media::WakeupMediaProfile profile;
            profile.reason = aov::media::WakeupReason::Timer;
            profile.start_video = true;              // 启动视频编码
            profile.start_audio = false;
            profile.start_detect_confirm = true;     // ✅ 关键：启动检测模块
            profile.request_idr = true;

            // 低功耗录像配置
            profile.low_power_record.record_fps = 1;      // 1fps 录像
            profile.low_power_record.normal_fps = 15;     // 常电帧率
            profile.low_power_record.target_frame_count = 1;  // 每次唤醒采集1帧
            profile.low_power_record.gop = 2;

            status = ToAovStatus(media_runtime_->RestoreFromAovSleep(profile));
            if (status != AovStatusCode::Ok) {
                return status;
            }
        }

        // 启动间隔录像
        status = ToAovStatus(storage_.StartRecord(storage::RecordMode::Interval));
        if (status != AovStatusCode::Ok) {
            return status;
        }

    } else if (event.source == WakeupSource::AppRemote) {
        // App预览唤醒 → 恢复常电模式
        if (media_runtime_ != nullptr) {
            aov::media::WakeupMediaProfile profile;
            profile.reason = aov::media::WakeupReason::App;
            profile.start_video = true;
            profile.start_audio = true;
            profile.start_detect_confirm = false;  // App 预览不需要检测确认
            profile.request_idr = true;

            status = ToAovStatus(media_runtime_->RestoreFromAovSleep(profile));
            if (status != AovStatusCode::Ok) {
                return status;
            }
        }

        // 启动事件录像
        status = ToAovStatus(storage_.StartRecord(storage::RecordMode::Event));
        if (status != AovStatusCode::Ok) {
            return status;
        }
    }

    return AovStatusCode::Ok;
}

AovStatusCode AovAppRuntime::OnDetectResult(const DetectResultSummary& result) {
    if (!status_.initialized) {
        return AovStatusCode::InvalidState;
    }

    // 通知 orchestrator 处理检测结果
    AovStatusCode status = orchestrator_.OnDetectResult(result);
    if (status != AovStatusCode::Ok) {
        return status;
    }

    // 判断是否检测到目标
    const bool has_target = result.has_person || result.has_pet || result.has_vehicle;

    if (has_target) {
        // 检测到目标 → libmedia 应该已经自动切换到常电（方案B）
        // 这里只需要：
        // 1. 切换录像模式：间隔录像 → 事件录像
        // 2. 启动云存
        // 3. 触发告警

        // ① 切换录像模式
        status = ToAovStatus(storage_.StopRecord());  // 停止间隔录像
        if (status != AovStatusCode::Ok) {
            // 继续执行，不中断
        }

        status = ToAovStatus(storage_.StartRecord(storage::RecordMode::Event));  // 启动事件录像
        if (status != AovStatusCode::Ok) {
            return status;
        }

        // ② 评估告警
        const AlarmEvent event = alarm_.EvaluateDetectResult(result,
                                                             config_.GetActiveConfig().alarm,
                                                             orchestrator_.GetRuntimeWorkState(),
                                                             orchestrator_.GetRuntimeContext().battery);

        if (event.type != AlarmType::Unknown) {
            // ③ 启动云存
            if (event.need_cloud_report || event.need_record) {
                status = cloud_.StartCloudStorage("detect-" + std::to_string(event.timestamp_ms));
                if (status != AovStatusCode::Ok) {
                    return status;
                }
            }

            // ④ 上报告警
            if (event.need_cloud_report) {
                status = cloud_.ReportAlarm(event);
                if (status != AovStatusCode::Ok) {
                    return status;
                }
            }
        }
    }

    return AovStatusCode::Ok;
}

AovStatusCode AovAppRuntime::ApplyPendingConfig() {
    if (!status_.initialized) {
        return AovStatusCode::InvalidState;
    }
    if (media_runtime_ == nullptr) {
        return AovStatusCode::InvalidState;
    }

    CoreApplyCoordinator coordinator(*media_runtime_, storage_);
    const CoreApplyResult apply_result = coordinator.ApplyDeviceConfig(config_.GetDesiredConfig());
    if (!apply_result.ok()) {
        return AovStatusCode::InternalError;
    }

    AovStatusCode status = ToAovStatus(config_.MarkApplied());
    if (status != AovStatusCode::Ok) {
        return status;
    }
    status = ToAovStatus(config_.Persist());
    if (status != AovStatusCode::Ok) {
        return status;
    }

    return cloud_.ReportDeviceState();
}

cloud::CloudActionMapResult AovAppRuntime::SubmitCloudAction(
    const cloud::CloudActionRequest& request) {
    return cloud_command_bridge_.SubmitCloudAction(request);
}

CoreCommandQueueResult AovAppRuntime::PopCoreCommand(CoreCommand& out) {
    return command_queue_.Pop(out);
}

std::size_t AovAppRuntime::GetPendingCoreCommandCount() const {
    return command_queue_.PendingCount();
}

CoreCommandExecResult AovAppRuntime::ExecuteCoreCommand(
    const CoreCommand& command,
    aov::sys::IStorageService* sys_storage_service,
    aov::sys::IDeviceControlService* device_control_service) {
    if (!status_.initialized) {
        return CoreCommandExecResult::Error(CoreCommandExecStatusCode::Failed,
                                            "app.core.runtime",
                                            "runtime is not initialized");
    }
    if (media_runtime_ == nullptr) {
        return CoreCommandExecResult::Error(CoreCommandExecStatusCode::Unsupported,
                                            "libmedia.runtime",
                                            "core command execution needs media runtime");
    }

    if (sys_storage_service != nullptr && device_control_service != nullptr) {
        CoreCommandExecutor executor(*media_runtime_,
                                     config_,
                                     storage_,
                                     *sys_storage_service,
                                     *device_control_service);
        return executor.Execute(command);
    }
    if (sys_storage_service != nullptr) {
        CoreCommandExecutor executor(*media_runtime_,
                                     config_,
                                     storage_,
                                     *sys_storage_service);
        return executor.Execute(command);
    }

    CoreCommandExecutor executor(*media_runtime_, config_, storage_);
    return executor.Execute(command);
}

CoreCommandExecResult AovAppRuntime::DrainCoreCommands(
    const std::size_t max_count,
    aov::sys::IStorageService* sys_storage_service,
    aov::sys::IDeviceControlService* device_control_service) {
    if (max_count == 0) {
        return CoreCommandExecResult::Ok();
    }

    std::size_t executed = 0;
    CoreCommand command;
    while (executed < max_count) {
        const auto pop_result = command_queue_.Pop(command);
        if (pop_result.code == CoreCommandQueueStatusCode::Empty) {
            return CoreCommandExecResult::Ok();
        }
        if (!pop_result.ok()) {
            return CoreCommandExecResult::Error(CoreCommandExecStatusCode::Failed,
                                                "app.core.command_queue",
                                                pop_result.message);
        }

        const auto exec_result = ExecuteCoreCommand(command,
                                                    sys_storage_service,
                                                    device_control_service);
        if (!exec_result.ok()) {
            return exec_result;
        }
        ++executed;
    }
    return CoreCommandExecResult::Ok();
}

AovAppRuntimeStatus AovAppRuntime::GetStatus() const {
    return status_;
}

AovStatusCode AovAppRuntime::ToAovStatus(const config::ConfigResult& result) {
    return result.ok() ? AovStatusCode::Ok : AovStatusCode::InvalidArgument;
}

AovStatusCode AovAppRuntime::ToAovStatus(const packet::PacketResult& result) {
    if (result.ok()) {
        return AovStatusCode::Ok;
    }
    if (result.code == packet::PacketStatusCode::InvalidArgument) {
        return AovStatusCode::InvalidArgument;
    }
    if (result.code == packet::PacketStatusCode::InvalidState) {
        return AovStatusCode::InvalidState;
    }
    if (result.code == packet::PacketStatusCode::Backpressure) {
        return AovStatusCode::Busy;
    }
    return AovStatusCode::InternalError;
}

AovStatusCode AovAppRuntime::ToAovStatus(const storage::StorageResult& result) {
    if (result.ok()) {
        return AovStatusCode::Ok;
    }
    if (result.code == storage::StorageStatusCode::InvalidArgument) {
        return AovStatusCode::InvalidArgument;
    }
    if (result.code == storage::StorageStatusCode::InvalidState) {
        return AovStatusCode::InvalidState;
    }
    if (result.code == storage::StorageStatusCode::NotMounted) {
        return AovStatusCode::NotFound;
    }
    if (result.code == storage::StorageStatusCode::NoSpace) {
        return AovStatusCode::Busy;
    }
    return AovStatusCode::InternalError;
}

AovStatusCode AovAppRuntime::ToAovStatus(const aov::media::MediaStatusCode status) {
    if (status == aov::media::MediaStatusCode::Ok) {
        return AovStatusCode::Ok;
    }
    if (status == aov::media::MediaStatusCode::InvalidArgument) {
        return AovStatusCode::InvalidArgument;
    }
    if (status == aov::media::MediaStatusCode::InvalidState) {
        return AovStatusCode::InvalidState;
    }
    if (status == aov::media::MediaStatusCode::Busy) {
        return AovStatusCode::Busy;
    }
    if (status == aov::media::MediaStatusCode::NotFound) {
        return AovStatusCode::NotFound;
    }
    return AovStatusCode::InternalError;
}

AovStatusCode AovAppRuntime::BindMediaToPacket() {
    if (media_runtime_ == nullptr) {
        return AovStatusCode::Ok;
    }

    aov::media::IVideoStreamService* video = media_runtime_->GetVideoStreamService();
    if (video == nullptr) {
        return AovStatusCode::InvalidState;
    }

    for (const int chn_id : {0, 1}) {
        const auto status = video->RegisterStreamCallback(
            chn_id,
            [this](int, const aov::media::StreamFrame& frame) {
                (void)router_.OnMediaFrame(frame);
            });
        const AovStatusCode mapped = ToAovStatus(status);
        if (mapped != AovStatusCode::Ok) {
            return mapped;
        }
    }
    return AovStatusCode::Ok;
}

void AovAppRuntime::CheckAndReportDrainState() {
    // 检查各模块的 Drain 状态
    ModuleDrainState state;

    // 1. 检查本地录像是否完成（关闭、刷新、同步）
    // TODO: storage_ 需要提供 IsDrained() 或类似接口
    state.local_record_closed_flushed_synced = true;  // 暂时默认为 true

    // 2. 检查云端存储是否完成
    // TODO: cloud_ 需要提供 IsUploadQueueEmpty() 接口
    state.cloud_storage_finished = true;  // 暂时默认为 true

    // 3. 检查配置是否持久化完成
    // TODO: config_ 需要提供 IsPersisted() 接口
    state.config_persisted = true;  // 暂时默认为 true

    // 4. idle_debounce_expired 由 Orchestrator 管理，这里不设置
    state.idle_debounce_expired = false;

    // 上报给 Orchestrator
    orchestrator_.OnModuleDrainStateChanged(state);
}

} // namespace aov::app::core

