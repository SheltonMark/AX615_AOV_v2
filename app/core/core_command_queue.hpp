#pragma once

#include <cstddef>
#include <deque>
#include <string>
#include <utility>

#include "app/core/core_commands.hpp"

namespace aov::app::core {

enum class CoreCommandQueueStatusCode {
    Ok,
    Empty,
    Full,
};

struct CoreCommandQueueResult {
    CoreCommandQueueStatusCode code {CoreCommandQueueStatusCode::Ok};
    std::string message;

    static CoreCommandQueueResult Ok() {
        return {};
    }

    static CoreCommandQueueResult Error(CoreCommandQueueStatusCode status,
                                        std::string reason) {
        CoreCommandQueueResult result;
        result.code = status;
        result.message = std::move(reason);
        return result;
    }

    bool ok() const {
        return code == CoreCommandQueueStatusCode::Ok;
    }
};

class CoreCommandQueue final {
public:
    explicit CoreCommandQueue(std::size_t max_pending = 32);

    CoreCommandQueueResult Push(const CoreCommand& command);
    CoreCommandQueueResult Pop(CoreCommand& out);
    bool HasPending() const;
    std::size_t PendingCount() const;
    void Clear();

private:
    std::size_t max_pending_ {32};
    std::deque<CoreCommand> pending_ {};
};

} // namespace aov::app::core
