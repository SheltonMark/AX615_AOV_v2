#pragma once

#include "common/media_config.hpp"
#include "common/media_status.hpp"
#include "common/media_types.hpp"

namespace aov::media {

class IAudioStreamService;
class IDetectService;
class IIspService;
class IOsdService;
class IVideoStreamService;

class IMediaRuntime {
public:
    virtual ~IMediaRuntime() = default;

    virtual MediaStatusCode Init(const MediaRuntimeInitConfig& config) = 0;
    virtual MediaStatusCode Init(const SensorConfig& sensor, const CmPoolConfig& pool) = 0;
    virtual MediaStatusCode Start() = 0;
    virtual void Stop() = 0;

    virtual MediaStatusCode ApplyVideoConfig(const VideoEncodeGroupConfig& config) = 0;
    virtual MediaStatusCode ApplyAudioConfig(const AudioEncodeApplyConfig& config) = 0;
    virtual MediaStatusCode ApplyImageConfig(const IspImageConfig& config) = 0;
    virtual MediaStatusCode ApplyDetectConfig(const DetectApplyConfig& config) = 0;
    virtual MediaStatusCode ApplyOsdConfig(const OsdApplyConfig& config) = 0;

    virtual MediaStatusCode PrepareForAovSleep(const AovSleepMediaConfig& config) = 0;
    virtual MediaStatusCode RestoreFromAovSleep(const WakeupMediaProfile& profile) = 0;
    virtual MediaStatusCode EnterLowPowerMode(const LowPowerRecordProfile& profile) = 0;
    virtual MediaStatusCode ResumeNormalMode() = 0;

    virtual MediaStatusCode CaptureSnapshot(const SnapshotRequest& request, SnapshotResult& result) = 0;

    virtual IIspService* GetIspService() = 0;
    virtual IVideoStreamService* GetVideoStreamService() = 0;
    virtual IAudioStreamService* GetAudioStreamService() = 0;
    virtual IDetectService* GetDetectService() = 0;
    virtual IOsdService* GetOsdService() = 0;

    virtual bool IsRunning() const = 0;
    virtual MediaRuntimeStatus GetRuntimeStatus() const = 0;
};

}  // namespace aov::media
