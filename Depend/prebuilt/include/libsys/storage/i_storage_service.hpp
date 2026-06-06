#pragma once

#include "common/sys_types.hpp"

#include <string>

namespace aov::sys {

enum class SdCardStatus {
    NotDetected,
    NotMounted,
    Mounted,
    Formatting,
    Error,
};

struct SdCapacity {
    int total_mb {0};
    int free_mb {0};
};

class IStorageService {
public:
    virtual ~IStorageService() = default;

    virtual SdCardStatus GetSdStatus() const = 0;
    virtual SysStatusCode MountSdCard() = 0;
    virtual SysStatusCode UnmountSdCard() = 0;
    virtual SysStatusCode FormatSdCard() = 0;
    virtual SdCapacity GetCapacity() const = 0;
    virtual std::string GetRecordRootPath() const = 0;
    virtual SysStatusCode EnsureDirectory(const std::string& path) = 0;
    virtual int GetFreeSpaceMb() const = 0;
};

}  // namespace aov::sys
