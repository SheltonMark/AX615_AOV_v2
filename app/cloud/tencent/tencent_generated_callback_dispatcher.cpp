#include "app/cloud/tencent/tencent_generated_callback_dispatcher.h"

#include <deque>
#include <mutex>
#include <string>

namespace {

enum class EventType {
    Property,
    Action,
};

struct GeneratedEvent {
    EventType type {EventType::Property};
    std::string id;
    std::string value;
    std::string request_id;
    uint64_t cloud_version {0};
    uint64_t recv_ts_ms {0};
};

std::mutex& QueueMutex() {
    static std::mutex mutex;
    return mutex;
}

std::deque<GeneratedEvent>& Queue() {
    static std::deque<GeneratedEvent> queue;
    return queue;
}

bool EmptyId(const char* id) {
    return id == nullptr || id[0] == '\0';
}

} // namespace

extern "C" int aov_tencent_generated_push_property(const char* property_id,
                                                   const char* value,
                                                   const uint64_t cloud_version,
                                                   const uint64_t recv_ts_ms) {
    if (EmptyId(property_id)) {
        return -1;
    }

    GeneratedEvent event;
    event.type = EventType::Property;
    event.id = property_id;
    event.value = value != nullptr ? value : "";
    event.cloud_version = cloud_version;
    event.recv_ts_ms = recv_ts_ms;

    std::lock_guard<std::mutex> lock(QueueMutex());
    Queue().push_back(event);
    return 0;
}

extern "C" int aov_tencent_generated_push_action(const char* action_id,
                                                 const char* params,
                                                 const char* request_id,
                                                 const uint64_t recv_ts_ms) {
    if (EmptyId(action_id)) {
        return -1;
    }

    GeneratedEvent event;
    event.type = EventType::Action;
    event.id = action_id;
    event.value = params != nullptr ? params : "";
    event.request_id = request_id != nullptr ? request_id : "";
    event.recv_ts_ms = recv_ts_ms;

    std::lock_guard<std::mutex> lock(QueueMutex());
    Queue().push_back(event);
    return 0;
}

extern "C" int aov_tencent_generated_drain(
    const aov_tencent_property_callback property_callback,
    const aov_tencent_action_callback action_callback,
    void* user) {
    std::deque<GeneratedEvent> events;
    {
        std::lock_guard<std::mutex> lock(QueueMutex());
        events.swap(Queue());
    }

    int drained = 0;
    for (const auto& event : events) {
        if (event.type == EventType::Property) {
            if (property_callback != nullptr) {
                property_callback(event.id.c_str(),
                                  event.value.c_str(),
                                  event.cloud_version,
                                  event.recv_ts_ms,
                                  user);
            }
        } else if (action_callback != nullptr) {
            action_callback(event.id.c_str(),
                            event.value.c_str(),
                            event.request_id.c_str(),
                            event.recv_ts_ms,
                            user);
        }
        ++drained;
    }

    return drained;
}

extern "C" void aov_tencent_generated_clear(void) {
    std::lock_guard<std::mutex> lock(QueueMutex());
    Queue().clear();
}
