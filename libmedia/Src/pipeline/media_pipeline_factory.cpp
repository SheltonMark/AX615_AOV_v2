/**
 * media_pipeline_factory.cpp
 * 工厂实现：创建并注入 AX615 adapter 实例
 */

#include "media_pipeline_factory.hpp"
#include "media_pipeline_manager.hpp"

#if defined(LIBMEDIA_BUILD_AX615_ADAPTERS)
#include "../adapter/ax615/audio/ax_audio_capture_adapter.hpp"
#include "../adapter/ax615/audio/ax_audio_play_adapter.hpp"
#include "../adapter/ax615/detect/ax_detect_adapter.hpp"
#include "../adapter/ax615/isp/ax_isp_adapter.hpp"
#include "../adapter/ax615/ivps/ax_ivps_adapter.hpp"
#include "../adapter/ax615/jenc/ax_jenc_adapter.hpp"
#include "../adapter/ax615/osd/ax_osd_adapter.hpp"
#include "../adapter/ax615/power/ax_aov_power_adapter.hpp"
#include "../adapter/ax615/sys/ax_link_manager.hpp"
#include "../adapter/ax615/sys/ax_sys_adapter.hpp"
#include "../adapter/ax615/venc/ax_venc_adapter.hpp"
#include "../adapter/ax615/vin/ax_vin_adapter.hpp"
#endif

namespace aov::media {

std::unique_ptr<IMediaRuntime> CreateAx615MediaRuntime() {
#if defined(LIBMEDIA_BUILD_AX615_ADAPTERS)
    return std::make_unique<MediaPipelineManager>(
        std::make_unique<ax615::AxSysAdapter>(),
        std::make_unique<ax615::AxVinAdapter>(),
        std::make_unique<ax615::AxIspAdapter>(),
        std::make_unique<ax615::AxIvpsAdapter>(),
        std::make_unique<ax615::AxVencAdapter>(),
        std::make_unique<ax615::AxJencAdapter>(),
        std::make_unique<ax615::AxLinkManager>(),
        std::make_unique<ax615::AxAovPowerAdapter>(),
        std::make_unique<ax615::AxAudioCaptureAdapter>(),
        std::make_unique<ax615::AxAudioPlayAdapter>(),
        std::make_unique<ax615::AxDetectAdapter>(),
        std::make_unique<ax615::AxOsdAdapter>());
#else
    return std::make_unique<MediaPipelineManager>();
#endif
}

}  // namespace aov::media
