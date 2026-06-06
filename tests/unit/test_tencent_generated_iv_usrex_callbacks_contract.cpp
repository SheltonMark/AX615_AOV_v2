#include <iostream>
#include <cstdint>
#include <string>

extern "C" {
#include "../../app/cloud/TencentCloud_SDK/tools/iv_usrex.h"
#include "../../app/cloud/tencent/tencent_generated_callback_dispatcher.h"
}

namespace {

struct SeenEvents {
    int properties {0};
    int actions {0};
    std::string last_property_id;
    std::string last_property_value;
    std::string last_action_id;
};

int Fail(const char* message) {
    std::cerr << message << "\n";
    return 1;
}

void OnProperty(const char* property_id,
                const char* value,
                std::uint64_t,
                std::uint64_t,
                void* user) {
    auto* seen = static_cast<SeenEvents*>(user);
    ++seen->properties;
    seen->last_property_id = property_id ? property_id : "";
    seen->last_property_value = value ? value : "";
}

void OnAction(const char* action_id,
              const char*,
              const char*,
              std::uint64_t,
              void* user) {
    auto* seen = static_cast<SeenEvents*>(user);
    ++seen->actions;
    seen->last_action_id = action_id ? action_id : "";
}

} // namespace

int main() {
    aov_tencent_generated_clear();

    TYPE_DEF_TEMPLATE_ARRAY fps[MAX_ARRAY_JSON_STR_LEN + 1] = "{\"fps\":15}";
    if (iv_usrcb_ProWritable_VideoFPS(fps) != 0) {
        return Fail("VideoFPS callback must accept valid payload");
    }

    TYPE_DEF_TEMPLATE_BOOL cloud_storage = 1;
    if (iv_usrcb_ProWritable_CloudStorageSwitch(&cloud_storage) != 0) {
        return Fail("CloudStorageSwitch callback must accept bool payload");
    }

    if (iv_usrcb_Action_Reboot(nullptr) != 0) {
        return Fail("Reboot action callback must enqueue action");
    }

    SeenEvents seen;
    const int drained = aov_tencent_generated_drain(&OnProperty, &OnAction, &seen);
    if (drained != 3 ||
        seen.properties != 2 ||
        seen.actions != 1 ||
        seen.last_property_id != "CloudStorageSwitch" ||
        seen.last_property_value != "true" ||
        seen.last_action_id != "Reboot") {
        return Fail("generated iv_usrex callbacks must enqueue property/action events");
    }

    std::cout << "tencent generated iv_usrex callbacks contract passed\n";
    return 0;
}
