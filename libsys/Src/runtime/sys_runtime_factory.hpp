#pragma once

#include "runtime/i_sys_runtime.hpp"

#include <memory>

namespace aov::sys {

std::unique_ptr<ISysRuntime> CreateDefaultSysRuntime();

}  // namespace aov::sys
