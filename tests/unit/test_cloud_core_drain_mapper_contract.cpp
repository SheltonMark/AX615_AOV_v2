#include <iostream>

#include "../../app/cloud/cloud_service_stub.hpp"
#include "../../app/core/cloud_core_drain_mapper.hpp"

namespace {

int Fail(const char* message) {
    std::cerr << message << "\n";
    return 1;
}

} // namespace

int main() {
    using namespace aov::app;
    using namespace aov::app::cloud;
    using namespace aov::app::core;

    CloudRuntimeStatus status;
    status.cloud_storage_running = true;
    status.cloud_storage_result = CloudStorageResult::None;

    auto drain = BuildCloudStorageDrainState(status);
    if (drain.cloud_storage_finished) {
        return Fail("running cloud storage must block cloud drain");
    }

    status.cloud_storage_running = false;
    status.cloud_storage_result = CloudStorageResult::None;
    drain = BuildCloudStorageDrainState(status);
    if (drain.cloud_storage_finished) {
        return Fail("cloud storage without terminal result must block cloud drain");
    }

    status.cloud_storage_result = CloudStorageResult::Success;
    drain = BuildCloudStorageDrainState(status);
    if (!drain.cloud_storage_finished) {
        return Fail("successful cloud storage must satisfy cloud drain");
    }

    status.cloud_storage_result = CloudStorageResult::ExplicitFailure;
    drain = BuildCloudStorageDrainState(status);
    if (!drain.cloud_storage_finished) {
        return Fail("explicit cloud storage failure must satisfy cloud drain");
    }

    status.cloud_storage_running = true;
    status.cloud_storage_result = CloudStorageResult::Success;
    drain = BuildCloudStorageDrainState(status);
    if (drain.cloud_storage_finished) {
        return Fail("running cloud storage must block drain even if previous result was success");
    }

    CloudServiceStub cloud;
    cloud.StartCloudStorage("alarm-1");
    drain = BuildCloudStorageDrainState(cloud.GetRuntimeStatus());
    if (drain.cloud_storage_finished) {
        return Fail("cloud service running cloud storage must block drain");
    }

    cloud.FinishCloudStorage(CloudStorageResult::ExplicitFailure);
    drain = BuildCloudStorageDrainState(cloud.GetRuntimeStatus());
    if (!drain.cloud_storage_finished) {
        return Fail("cloud service explicit failure must satisfy drain");
    }

    std::cout << "cloud core drain mapper contract passed\n";
    return 0;
}
