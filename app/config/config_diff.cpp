#include "config_diff.hpp"

namespace aov::app::config {

namespace {

void AddField(PendingConfigChange& change, const char* field) {
    change.changed_fields.push_back(field);
}

bool SameMainStream(const VideoStreamConfig& lhs, const VideoStreamConfig& rhs) {
    return lhs.codec == rhs.codec &&
           lhs.width == rhs.width &&
           lhs.height == rhs.height &&
           lhs.fps_normal == rhs.fps_normal &&
           lhs.fps_aov == rhs.fps_aov &&
           lhs.bitrate_mode == rhs.bitrate_mode &&
           lhs.bitrate_kbps == rhs.bitrate_kbps &&
           lhs.gop == rhs.gop;
}

} // namespace

PendingConfigChange BuildPendingConfigChange(const DeviceConfig& active,
                                             const DeviceConfig& desired) {
    PendingConfigChange change;
    change.version = desired.version;

    if (active.work_mode.mode != desired.work_mode.mode ||
        active.work_mode.record_interval_sec != desired.work_mode.record_interval_sec ||
        active.work_mode.low_battery_protect_threshold_pct !=
            desired.work_mode.low_battery_protect_threshold_pct) {
        AddField(change, "work_mode");
        change.requires_wakeup_apply = true;
    }

    if (!SameMainStream(active.media.main_stream, desired.media.main_stream) ||
        !SameMainStream(active.media.sub_stream, desired.media.sub_stream)) {
        AddField(change, "media.video");
        change.requires_media_restart = true;
    }

    if (active.record.sd.record_enabled != desired.record.sd.record_enabled ||
        active.record.sd.loop_record != desired.record.sd.loop_record ||
        active.record.schedule.default_record_type != desired.record.schedule.default_record_type ||
        active.record.cloud_storage.enabled != desired.record.cloud_storage.enabled) {
        AddField(change, "record");
    }

    return change;
}

} // namespace aov::app::config
