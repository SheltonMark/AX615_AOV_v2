#pragma once

#include "app/core/aov_types.hpp"
#include "app/core/core_state.hpp"

namespace aov::app {

class IAovOrchestrator {
public:
    virtual ~IAovOrchestrator() = default;

    virtual AovStatusCode Init(const WorkModePolicy& policy) = 0;
    virtual AovStatusCode Start() = 0;
    virtual AovStatusCode Stop() = 0;

    virtual AovStatusCode OnBatteryStatusChanged(const BatteryStatus& status) = 0;
    virtual AovStatusCode OnWakeupEvent(const WakeupEvent& event) = 0;
    virtual AovStatusCode OnDetectResult(const DetectResultSummary& result) = 0;
    virtual AovStatusCode OnPreviewRequest(const PreviewRequest& request) = 0;
    virtual AovStatusCode OnPreviewSessionClosed(const std::string& session_id) = 0;
    virtual AovStatusCode OnModuleDrainStateChanged(const core::ModuleDrainState& state) = 0;
    virtual AovStatusCode OnIdleDebounceExpired(std::uint64_t timestamp_ms) = 0;

    virtual RuntimeWorkState GetRuntimeWorkState() const = 0;
    virtual WorkModePolicy GetCurrentPolicy() const = 0;
    virtual AovRuntimeContext GetRuntimeContext() const = 0;
};

} // namespace aov::app
