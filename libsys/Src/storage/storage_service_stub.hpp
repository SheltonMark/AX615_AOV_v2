#pragma once

#include "storage/i_storage_service.hpp"

namespace aov::sys {

class StorageServiceStub final : public IStorageService {
public:
    StorageServiceStub() = default;
    ~StorageServiceStub() override = default;

    SdCardStatus GetSdStatus() const override;
    SysStatusCode MountSdCard() override;
    SysStatusCode UnmountSdCard() override;
    SysStatusCode FormatSdCard() override;
    SdCapacity GetCapacity() const override;
    std::string GetRecordRootPath() const override;
    SysStatusCode EnsureDirectory(const std::string& path) override;
    int GetFreeSpaceMb() const override;

    bool IsMounted() const;
    void SetSdStatus(SdCardStatus status);
    void SetMountResult(SysStatusCode status);

private:
    SdCardStatus status_ {SdCardStatus::Mounted};
    SysStatusCode mount_result_ {SysStatusCode::Ok};
    std::string root_path_ {"/tfcard"};
    SdCapacity capacity_ {8192, 1024};
};

}  // namespace aov::sys
