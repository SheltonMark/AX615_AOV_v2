#include "storage_service_stub.hpp"

namespace aov::sys {

SdCardStatus StorageServiceStub::GetSdStatus() const {
    return status_;
}

SysStatusCode StorageServiceStub::MountSdCard() {
    if (mount_result_ != SysStatusCode::Ok) {
        return mount_result_;
    }
    if (status_ == SdCardStatus::Error ||
        status_ == SdCardStatus::NotDetected ||
        status_ == SdCardStatus::Formatting) {
        return SysStatusCode::InvalidState;
    }
    status_ = SdCardStatus::Mounted;
    return SysStatusCode::Ok;
}

SysStatusCode StorageServiceStub::UnmountSdCard() {
    status_ = SdCardStatus::NotMounted;
    return SysStatusCode::Ok;
}

SysStatusCode StorageServiceStub::FormatSdCard() {
    if (status_ == SdCardStatus::NotDetected) {
        return SysStatusCode::InvalidState;
    }
    status_ = SdCardStatus::Mounted;
    capacity_ = {8192, 8192};
    return SysStatusCode::Ok;
}

SdCapacity StorageServiceStub::GetCapacity() const {
    return capacity_;
}

std::string StorageServiceStub::GetRecordRootPath() const {
    return root_path_;
}

SysStatusCode StorageServiceStub::EnsureDirectory(const std::string&) {
    return status_ == SdCardStatus::Mounted ? SysStatusCode::Ok : SysStatusCode::InvalidState;
}

int StorageServiceStub::GetFreeSpaceMb() const {
    return status_ == SdCardStatus::Mounted ? capacity_.free_mb : 0;
}

bool StorageServiceStub::IsMounted() const {
    return status_ == SdCardStatus::Mounted;
}

void StorageServiceStub::SetSdStatus(const SdCardStatus status) {
    status_ = status;
}

void StorageServiceStub::SetMountResult(const SysStatusCode status) {
    mount_result_ = status;
}

}  // namespace aov::sys
