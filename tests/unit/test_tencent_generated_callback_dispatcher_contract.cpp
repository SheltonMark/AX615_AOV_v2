#include <cstdint>
#include <iostream>
#include <string>

#include "../../app/cloud/tencent/tencent_generated_callback_dispatcher.h"

namespace {

struct SeenEvents {
    int properties {0};
    int actions {0};
    std::string last_property_id;
    std::string last_property_value;
    std::string last_action_id;
    std::string last_action_params;
};

int Fail(const char* message) {
    std::cerr << message << "\n";
    return 1;
}

void OnProperty(const char* property_id,
                const char* value,
                std::uint64_t cloud_version,
                std::uint64_t recv_ts_ms,
                void* user) {
    auto* seen = static_cast<SeenEvents*>(user);
    ++seen->properties;
    seen->last_property_id = property_id ? property_id : "";
    seen->last_property_value = value ? value : "";
    if (cloud_version != 3 || recv_ts_ms != 4000) {
        seen->last_property_id = "bad-meta";
    }
}

void OnAction(const char* action_id,
              const char* params,
              const char* request_id,
              std::uint64_t recv_ts_ms,
              void* user) {
    auto* seen = static_cast<SeenEvents*>(user);
    ++seen->actions;
    seen->last_action_id = action_id ? action_id : "";
    seen->last_action_params = params ? params : "";
    if ((request_id == nullptr) || std::string(request_id) != "req-1" || recv_ts_ms != 5000) {
        seen->last_action_id = "bad-meta";
    }
}

} // namespace

int main() {
    aov_tencent_generated_clear();

    if (aov_tencent_generated_push_property(nullptr, "15", 3, 4000) == 0) {
        return Fail("dispatcher must reject null property id");
    }
    if (aov_tencent_generated_push_action(nullptr, "{}", "req-1", 5000) == 0) {
        return Fail("dispatcher must reject null action id");
    }

    if (aov_tencent_generated_push_property("VideoFPS", "15", 3, 4000) != 0 ||
        aov_tencent_generated_push_action("Reboot", "{}", "req-1", 5000) != 0) {
        return Fail("dispatcher must accept valid generated callback events");
    }

    SeenEvents seen;
    const int drained = aov_tencent_generated_drain(&OnProperty, &OnAction, &seen);
    if (drained != 2 ||
        seen.properties != 1 ||
        seen.actions != 1 ||
        seen.last_property_id != "VideoFPS" ||
        seen.last_property_value != "15" ||
        seen.last_action_id != "Reboot" ||
        seen.last_action_params != "{}") {
        return Fail("dispatcher must drain queued generated callback events");
    }

    if (aov_tencent_generated_drain(&OnProperty, &OnAction, &seen) != 0) {
        return Fail("dispatcher queue must be empty after drain");
    }

    std::cout << "tencent generated callback dispatcher contract passed\n";
    return 0;
}
