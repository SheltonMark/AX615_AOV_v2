#include <iostream>

#include "../../app/cloud/cloud_service_stub.hpp"
#include "../../app/config/device_config.hpp"
#include "../../app/core/core_runtime_facade.hpp"
#include "../../app/storage/dhfs/dhfs_writer_stub.hpp"
#include "../../app/storage/storage_service_stub.hpp"
#include "../../libsys/Src/power/power_service_stub.hpp"
#include "../../libsys/Src/storage/storage_service_stub.hpp"

namespace {

int Fail(const char* message) {
    std::cerr << message << "\n";
    return 1;
}

class FakeMediaRuntime final : public aov::media::IMediaRuntime {
public:
    aov::media::MediaStatusCode Init(const aov::media::MediaRuntimeInitConfig&) override {
        return aov::media::MediaStatusCode::Ok;
    }
    aov::media::MediaStatusCode Init(const aov::media::SensorConfig&,
                                     const aov::media::CmPoolConfig&) override {
        return aov::media::MediaStatusCode::Ok;
    }
    aov::media::MediaStatusCode Start() override {
        return aov::media::MediaStatusCode::Ok;
    }
    void Stop() override {}
    aov::media::MediaStatusCode ApplyVideoConfig(
        const aov::media::VideoEncodeGroupConfig&) override {
        ++apply_count;
        return aov::media::MediaStatusCode::Ok;
    }
    aov::media::MediaStatusCode ApplyAudioConfig(
        const aov::media::AudioEncodeApplyConfig&) override {
        return aov::media::MediaStatusCode::Ok;
    }
    aov::media::MediaStatusCode ApplyImageConfig(
        const aov::media::IspImageConfig&) override {
        return aov::media::MediaStatusCode::Ok;
    }
    aov::media::MediaStatusCode ApplyDetectConfig(
        const aov::media::DetectApplyConfig&) override {
        return aov::media::MediaStatusCode::Ok;
    }
    aov::media::MediaStatusCode ApplyOsdConfig(
        const aov::media::OsdApplyConfig&) override {
        return aov::media::MediaStatusCode::Ok;
    }
    aov::media::MediaStatusCode PrepareForAovSleep(
        const aov::media::AovSleepMediaConfig&) override {
        return aov::media::MediaStatusCode::Ok;
    }
    aov::media::MediaStatusCode RestoreFromAovSleep(
        const aov::media::WakeupMediaProfile&) override {
        return aov::media::MediaStatusCode::Ok;
    }
    aov::media::MediaStatusCode EnterLowPowerMode(
        const aov::media::LowPowerRecordProfile&) override {
        return aov::media::MediaStatusCode::Ok;
    }
    aov::media::MediaStatusCode ResumeNormalMode() override {
        return aov::media::MediaStatusCode::Ok;
    }
    aov::media::MediaStatusCode CaptureSnapshot(const aov::media::SnapshotRequest&,
                                                aov::media::SnapshotResult&) override {
        return aov::media::MediaStatusCode::Ok;
    }
    aov::media::IIspService* GetIspService() override { return nullptr; }
    aov::media::IVideoStreamService* GetVideoStreamService() override { return nullptr; }
    aov::media::IAudioStreamService* GetAudioStreamService() override { return nullptr; }
    aov::media::IDetectService* GetDetectService() override { return nullptr; }
    aov::media::IOsdService* GetOsdService() override { return nullptr; }
    bool IsRunning() const override { return true; }
    aov::media::MediaRuntimeStatus GetRuntimeStatus() const override { return {}; }

    int apply_count {0};
};

} // namespace

int main() {
    using namespace aov::app;
    using namespace aov::app::cloud;
    using namespace aov::app::config;
    using namespace aov::app::core;

    FakeMediaRuntime media;
    aov::app::storage::StorageServiceStub app_storage(
        std::unique_ptr<aov::app::storage::dhfs::IDhfsWriter>(
            new aov::app::storage::dhfs::DhfsWriterStub()));
    CloudServiceStub cloud;
    aov::sys::PowerServiceStub power;
    aov::sys::StorageServiceStub sys_storage;

    CoreRuntimeFacade facade(media, app_storage, cloud, power, sys_storage);

    DeviceConfig config = MakeDefaultDeviceConfig();
    config.work_mode.record_interval_sec = 3;

    auto result = facade.ApplyConfig(config);
    if (!result.ok() ||
        media.apply_count != 1) {
        return Fail("facade must apply config through core apply coordinator");
    }

    if (!app_storage.StartRecord().ok()) {
        return Fail("storage start must succeed after facade apply");
    }
    cloud.StartCloudStorage("alarm-1");

    auto sleep_result = facade.TryEnterAovSleep(config.work_mode, false, true);
    if (sleep_result.ok() ||
        power.HasEnteredSleep()) {
        return Fail("facade must not enter sleep while local/cloud drain is incomplete");
    }

    if (!app_storage.FlushAndClose().ok()) {
        return Fail("storage flush close must succeed");
    }
    cloud.FinishCloudStorage(CloudStorageResult::Success);

    sleep_result = facade.TryEnterAovSleep(config.work_mode, false, true);
    if (sleep_result.ok() ||
        power.HasEnteredSleep()) {
        return Fail("facade must still wait for idle debounce");
    }

    sleep_result = facade.TryEnterAovSleep(config.work_mode, true, true);
    if (!sleep_result.ok() ||
        !power.HasEnteredSleep() ||
        power.GetWakeupIntervalSec() != 3 ||
        !sys_storage.IsMounted()) {
        return Fail("facade must enter AOV sleep when all drain conditions are complete");
    }

    std::cout << "core runtime facade contract passed\n";
    return 0;
}
