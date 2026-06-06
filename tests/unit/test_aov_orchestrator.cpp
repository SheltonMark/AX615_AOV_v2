/**
 * @file test_aov_orchestrator.cpp
 * @brief AovOrchestratorStub 状态机单测（阶段0护栏）
 *
 * TDD 覆盖：
 *  - Init 参数校验
 *  - ResolveRuntimeState：配置模式 × 电量 × 预览态
 *  - 低电保护覆盖逻辑
 *  - SmartNoSleep 70/80 降级恢复
 *  - 状态快照不可变性（GetRuntimeContext 返回副本）
 */

#include <gtest/gtest.h>

#include "app/core/aov_orchestrator_stub.hpp"

using namespace aov::app;

// ────────────────────────────────────────────────────────────
// Helpers
// ────────────────────────────────────────────────────────────

static WorkModePolicy MakePowerSavingPolicy(int interval_sec = 1) {
    WorkModePolicy p;
    p.configured_mode = ConfiguredWorkMode::PowerSaving;
    p.periodic_wakeup_interval_sec = interval_sec;
    p.normal_record_fps = 15;
    p.low_battery_protect_threshold_pct = 10;
    p.smart_to_save_threshold_pct   = 70;
    p.smart_to_awake_threshold_pct  = 80;
    return p;
}

static WorkModePolicy MakeSmartNoSleepPolicy() {
    WorkModePolicy p = MakePowerSavingPolicy();
    p.configured_mode = ConfiguredWorkMode::SmartNoSleep;
    return p;
}

static WorkModePolicy MakeUltraLongStandbyPolicy() {
    WorkModePolicy p = MakePowerSavingPolicy();
    p.configured_mode = ConfiguredWorkMode::UltraLongStandby;
    return p;
}

static BatteryStatus MakeBattery(int pct, bool charging = false) {
    BatteryStatus b;
    b.percent  = pct;
    b.charging = charging;
    b.low_battery_protect = false;
    return b;
}

// ────────────────────────────────────────────────────────────
// Fixture
// ────────────────────────────────────────────────────────────

class OrchestratorTest : public ::testing::Test {
protected:
    AovOrchestratorStub orch_;

    void SetUp() override {
        // 每个用例开始时以 PowerSaving 策略初始化
        ASSERT_EQ(orch_.Init(MakePowerSavingPolicy()), AovStatusCode::Ok);
        ASSERT_EQ(orch_.Start(), AovStatusCode::Ok);
    }
};

// ────────────────────────────────────────────────────────────
// 1. Init / 参数校验
// ────────────────────────────────────────────────────────────

TEST(OrchestratorInitTest, ValidPowerSavingPolicy) {
    AovOrchestratorStub o;
    EXPECT_EQ(o.Init(MakePowerSavingPolicy()), AovStatusCode::Ok);
}

TEST(OrchestratorInitTest, ValidSmartNoSleepPolicy) {
    AovOrchestratorStub o;
    EXPECT_EQ(o.Init(MakeSmartNoSleepPolicy()), AovStatusCode::Ok);
}

TEST(OrchestratorInitTest, ValidUltraLongStandbyPolicy) {
    AovOrchestratorStub o;
    EXPECT_EQ(o.Init(MakeUltraLongStandbyPolicy()), AovStatusCode::Ok);
}

TEST(OrchestratorInitTest, InvalidWakeupInterval_Zero) {
    AovOrchestratorStub o;
    WorkModePolicy p = MakePowerSavingPolicy(0); // interval=0 非法
    // stub 应拒绝 interval<=0
    AovStatusCode r = o.Init(p);
    EXPECT_NE(r, AovStatusCode::Ok);
}

TEST(OrchestratorInitTest, InvalidWakeupInterval_Negative) {
    AovOrchestratorStub o;
    WorkModePolicy p = MakePowerSavingPolicy(-1);
    EXPECT_NE(o.Init(p), AovStatusCode::Ok);
}

TEST(OrchestratorInitTest, DoubleInitReturnsError) {
    AovOrchestratorStub o;
    ASSERT_EQ(o.Init(MakePowerSavingPolicy()), AovStatusCode::Ok);
    EXPECT_NE(o.Init(MakePowerSavingPolicy()), AovStatusCode::Ok);
}

// ────────────────────────────────────────────────────────────
// 2. 低电保护覆盖（任意配置模式下优先生效）
// ────────────────────────────────────────────────────────────

TEST_F(OrchestratorTest, LowBatteryProtection_OverridesPowerSaving) {
    BatteryStatus b = MakeBattery(5); // 低于 10%
    ASSERT_EQ(orch_.OnBatteryStatusChanged(b), AovStatusCode::Ok);
    EXPECT_EQ(orch_.GetRuntimeWorkState(), RuntimeWorkState::LowBatteryProtection);
}

TEST_F(OrchestratorTest, LowBatteryProtection_OverridesSmartNoSleep) {
    AovOrchestratorStub o;
    ASSERT_EQ(o.Init(MakeSmartNoSleepPolicy()), AovStatusCode::Ok);
    ASSERT_EQ(o.Start(), AovStatusCode::Ok);

    ASSERT_EQ(o.OnBatteryStatusChanged(MakeBattery(3)), AovStatusCode::Ok);
    EXPECT_EQ(o.GetRuntimeWorkState(), RuntimeWorkState::LowBatteryProtection);
}

TEST_F(OrchestratorTest, LowBatteryProtection_RecoverWhenAboveThreshold) {
    // 先触发低电
    ASSERT_EQ(orch_.OnBatteryStatusChanged(MakeBattery(5)), AovStatusCode::Ok);
    ASSERT_EQ(orch_.GetRuntimeWorkState(), RuntimeWorkState::LowBatteryProtection);

    // 充电后电量恢复到安全值
    BatteryStatus b = MakeBattery(50, /*charging=*/true);
    ASSERT_EQ(orch_.OnBatteryStatusChanged(b), AovStatusCode::Ok);
    EXPECT_NE(orch_.GetRuntimeWorkState(), RuntimeWorkState::LowBatteryProtection);
}

// ────────────────────────────────────────────────────────────
// 3. PowerSaving 模式下的状态决议
// ────────────────────────────────────────────────────────────

TEST_F(OrchestratorTest, PowerSaving_NormalBattery_NoPreview_SleepPeriodic) {
    ASSERT_EQ(orch_.OnBatteryStatusChanged(MakeBattery(80)), AovStatusCode::Ok);
    EXPECT_EQ(orch_.GetRuntimeWorkState(), RuntimeWorkState::SleepPeriodicDetect);
}

TEST_F(OrchestratorTest, PowerSaving_PreviewActive_AwakeNormal) {
    ASSERT_EQ(orch_.OnBatteryStatusChanged(MakeBattery(80)), AovStatusCode::Ok);

    PreviewRequest req;
    req.session_id = "sess-001";
    req.user_id    = "user-A";
    req.force_wakeup = true;
    ASSERT_EQ(orch_.OnPreviewRequest(req), AovStatusCode::Ok);

    EXPECT_EQ(orch_.GetRuntimeWorkState(), RuntimeWorkState::AwakeNormal);
}

TEST_F(OrchestratorTest, PowerSaving_PreviewClosed_BackToSleep) {
    ASSERT_EQ(orch_.OnBatteryStatusChanged(MakeBattery(80)), AovStatusCode::Ok);

    PreviewRequest req;
    req.session_id = "sess-002";
    req.user_id    = "user-B";
    ASSERT_EQ(orch_.OnPreviewRequest(req), AovStatusCode::Ok);
    ASSERT_EQ(orch_.GetRuntimeWorkState(), RuntimeWorkState::AwakeNormal);

    ASSERT_EQ(orch_.OnPreviewSessionClosed("sess-002"), AovStatusCode::Ok);
    EXPECT_EQ(orch_.GetRuntimeWorkState(), RuntimeWorkState::SleepPeriodicDetect);
}

// ────────────────────────────────────────────────────────────
// 4. SmartNoSleep 70/80 降级恢复
// ────────────────────────────────────────────────────────────

TEST(OrchestratorSmartTest, SmartNoSleep_AboveAwakeThreshold_AwakeNormal) {
    AovOrchestratorStub o;
    ASSERT_EQ(o.Init(MakeSmartNoSleepPolicy()), AovStatusCode::Ok);
    ASSERT_EQ(o.Start(), AovStatusCode::Ok);

    ASSERT_EQ(o.OnBatteryStatusChanged(MakeBattery(90)), AovStatusCode::Ok);
    EXPECT_EQ(o.GetRuntimeWorkState(), RuntimeWorkState::AwakeNormal);
}

TEST(OrchestratorSmartTest, SmartNoSleep_BelowSaveThreshold_DegradeToSleep) {
    AovOrchestratorStub o;
    ASSERT_EQ(o.Init(MakeSmartNoSleepPolicy()), AovStatusCode::Ok);
    ASSERT_EQ(o.Start(), AovStatusCode::Ok);

    // 电量低于 70%，应降级为省电睡眠
    ASSERT_EQ(o.OnBatteryStatusChanged(MakeBattery(60)), AovStatusCode::Ok);
    EXPECT_EQ(o.GetRuntimeWorkState(), RuntimeWorkState::SleepPeriodicDetect);
}

TEST(OrchestratorSmartTest, SmartNoSleep_Between70And80_StaysInCurrentState) {
    AovOrchestratorStub o;
    ASSERT_EQ(o.Init(MakeSmartNoSleepPolicy()), AovStatusCode::Ok);
    ASSERT_EQ(o.Start(), AovStatusCode::Ok);

    // 先降级
    ASSERT_EQ(o.OnBatteryStatusChanged(MakeBattery(60)), AovStatusCode::Ok);
    ASSERT_EQ(o.GetRuntimeWorkState(), RuntimeWorkState::SleepPeriodicDetect);

    // 充回到 75%（介于 70~80），不应立刻恢复 AwakeNormal
    ASSERT_EQ(o.OnBatteryStatusChanged(MakeBattery(75)), AovStatusCode::Ok);
    EXPECT_EQ(o.GetRuntimeWorkState(), RuntimeWorkState::SleepPeriodicDetect);
}

TEST(OrchestratorSmartTest, SmartNoSleep_RecoverAbove80_BackToAwake) {
    AovOrchestratorStub o;
    ASSERT_EQ(o.Init(MakeSmartNoSleepPolicy()), AovStatusCode::Ok);
    ASSERT_EQ(o.Start(), AovStatusCode::Ok);

    // 先降级
    ASSERT_EQ(o.OnBatteryStatusChanged(MakeBattery(60)), AovStatusCode::Ok);
    ASSERT_EQ(o.GetRuntimeWorkState(), RuntimeWorkState::SleepPeriodicDetect);

    // 充回 > 80%，恢复 AwakeNormal
    ASSERT_EQ(o.OnBatteryStatusChanged(MakeBattery(85)), AovStatusCode::Ok);
    EXPECT_EQ(o.GetRuntimeWorkState(), RuntimeWorkState::AwakeNormal);
}

// ────────────────────────────────────────────────────────────
// 5. UltraLongStandby 模式
// ────────────────────────────────────────────────────────────

TEST(OrchestratorUltraTest, UltraLongStandby_NormalBattery_SleepAppWakeOnly) {
    AovOrchestratorStub o;
    ASSERT_EQ(o.Init(MakeUltraLongStandbyPolicy()), AovStatusCode::Ok);
    ASSERT_EQ(o.Start(), AovStatusCode::Ok);

    ASSERT_EQ(o.OnBatteryStatusChanged(MakeBattery(80)), AovStatusCode::Ok);
    EXPECT_EQ(o.GetRuntimeWorkState(), RuntimeWorkState::SleepAppWakeOnly);
}

// ────────────────────────────────────────────────────────────
// 6. 状态快照不可变性
// ────────────────────────────────────────────────────────────

TEST_F(OrchestratorTest, GetRuntimeContext_ReturnsCopy) {
    ASSERT_EQ(orch_.OnBatteryStatusChanged(MakeBattery(80)), AovStatusCode::Ok);

    AovRuntimeContext ctx1 = orch_.GetRuntimeContext();
    // 修改副本不影响内部状态
    ctx1.battery.percent = 1;

    AovRuntimeContext ctx2 = orch_.GetRuntimeContext();
    EXPECT_EQ(ctx2.battery.percent, 80);
}

// ────────────────────────────────────────────────────────────
// 7. Stop/Start 幂等
// ────────────────────────────────────────────────────────────

TEST(OrchestratorLifecycleTest, StopBeforeStart_NoError) {
    AovOrchestratorStub o;
    ASSERT_EQ(o.Init(MakePowerSavingPolicy()), AovStatusCode::Ok);
    // 未 Start，调 Stop 不应崩溃
    EXPECT_NO_FATAL_FAILURE(o.Stop());
}

TEST(OrchestratorLifecycleTest, StartStopStart_IsAllowed) {
    AovOrchestratorStub o;
    ASSERT_EQ(o.Init(MakePowerSavingPolicy()), AovStatusCode::Ok);
    ASSERT_EQ(o.Start(), AovStatusCode::Ok);
    ASSERT_EQ(o.Stop(),  AovStatusCode::Ok);
    // 重新 Start 应允许
    EXPECT_EQ(o.Start(), AovStatusCode::Ok);
}
