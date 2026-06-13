#include <cstdint>
#include <iostream>

#include "../../app/cloud/tencent/tencent_cloud_storage.hpp"

namespace {

int Fail(const char* message) {
    std::cerr << message << "\n";
    return 1;
}

iv_cm_venc_pack_s MakePack() {
    static std::uint8_t data[] = {0x00, 0x00, 0x01, 0x65};
    iv_cm_venc_pack_s pack {};
    pack.pu8Addr = data;
    pack.u32Len = sizeof(data);
    pack.u64PTS = 1234;
    pack.u32Seq = 7;
    pack.eFrameType = IV_CM_FRAME_TYPE_I;
    return pack;
}

} // namespace

int main() {
    using namespace aov::app::cloud;
    using namespace aov::app::cloud::tencent;

    TencentCloudStorage storage;
    if (storage.SendVideoPack(MakePack()).ok()) {
        return Fail("cloud storage must reject frames before event start");
    }

    TencentCloudStorageSession session;
    session.event_id = 11;
    session.channel = CS_SINGLE_CH;
    session.event_start_time_s = 100;
    session.stream_start_time_s = 98;
    session.info = "{\"type\":\"human\"}";
    if (storage.StartEvent(session) != TencentCloudStatusCode::Ok) {
        return Fail("cloud storage must start valid event session");
    }
    if (!storage.GetStats().active ||
        storage.GetStats().active_event_id != 11) {
        return Fail("cloud storage stats must mark active event");
    }

    if (!storage.SendVideoPack(MakePack()).ok() ||
        storage.GetStats().sent_video_frames != 1 ||
        storage.GetStats().sent_video_bytes != 4 ||
        storage.GetStats().last_video_pts_ms != 1234) {
        return Fail("cloud storage must accept video frame while active");
    }

    if (storage.FinishEvent(CloudStorageResult::None).ok()) {
        return Fail("cloud storage finish must reject None result");
    }
    if (!storage.FinishEvent(CloudStorageResult::Success).ok() ||
        storage.GetStats().active ||
        storage.GetStats().last_result != CloudStorageResult::Success) {
        return Fail("cloud storage finish must close event and keep result");
    }
    if (!storage.SendVideoPack(MakePack()).ok()) {
        return Fail("cloud storage may drop frame after finish without failing caller");
    }
    if (storage.GetStats().dropped_frames == 0) {
        return Fail("cloud storage must count frame dropped after finish");
    }

    std::cout << "tencent cloud storage contract passed\n";
    return 0;
}
