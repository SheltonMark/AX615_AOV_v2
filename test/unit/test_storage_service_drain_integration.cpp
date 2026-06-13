#include <iostream>
#include <memory>

#include "../../app/core/storage_core_drain_mapper.hpp"
#include "../../app/storage/dhfs/dhfs_writer_stub.hpp"
#include "../../app/storage/storage_service_stub.hpp"

namespace {

int Fail(const char* message) {
    std::cerr << message << "\n";
    return 1;
}

} // namespace

int main() {
    using namespace aov::app::core;
    using namespace aov::app::storage;

    StorageServiceStub storage(
        std::unique_ptr<dhfs::IDhfsWriter>(new dhfs::DhfsWriterStub()));
    if (!storage.Init(StorageConfig {}).ok()) {
        return Fail("storage init must succeed");
    }

    if (!storage.StartRecord().ok()) {
        return Fail("storage start record must succeed");
    }
    auto drain = BuildLocalRecordDrainState(storage.GetStats());
    if (drain.local_record_closed_flushed_synced) {
        return Fail("storage must block local drain while recording");
    }

    if (!storage.FlushAndClose().ok()) {
        return Fail("storage flush and close must succeed");
    }
    drain = BuildLocalRecordDrainState(storage.GetStats());
    if (!drain.local_record_closed_flushed_synced) {
        return Fail("storage must report local drain after flush and close");
    }

    std::cout << "storage service drain integration passed\n";
    return 0;
}
