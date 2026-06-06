#pragma once
#include <memory>
#include "runtime/i_media_runtime.hpp"

namespace aov::media {

std::unique_ptr<IMediaRuntime> CreateAx615MediaRuntime();

}  // namespace aov::media
