#include "libmedia/Src/adapter/ax615/vin/sensor_registry.hpp"

#include <cstring>
#include <vector>

int main() {
    using namespace aov::media::ax615;

    SensorProfile profile;
    profile.sensor_name = "mock_sensor";
    profile.driver_lib_name = "libsns_mock.so";
    profile.driver_obj_name = "gSnsmockObj";
    profile.quick_start_obj_name = "gSnsmockObjQs";
    profile.default_iq_path = "/opt/etc/iqfiles/mock";
    profile.dev_id = 0;
    profile.pipe_id = 0;
    profile.rx_dev = 0;
    profile.sns_clk_idx = 0;

    SensorRegistry registry;
    if (!registry.Register(profile)) {
        return 1;
    }
    if (!registry.Has("mock_sensor")) {
        return 2;
    }

    const SensorProfile* found = registry.Find("mock_sensor");
    if (!found) {
        return 3;
    }
    if (found->driver_lib_name != "libsns_mock.so") {
        return 4;
    }

    std::vector<std::string> names = registry.ListSensorNames();
    bool seen = false;
    for (const auto& name : names) {
        if (name == "mock_sensor") {
            seen = true;
        }
    }
    return seen ? 0 : 5;
}
