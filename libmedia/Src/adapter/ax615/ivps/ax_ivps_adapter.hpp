#pragma once

#include <cstdint>
#include <vector>

extern "C" {
#include "ax_global_type.h"
#include "ax_ivps_api.h"
}

namespace aov::media::ax615 {

struct IvpsChannelConfig {
    int chn_id = 0;
    int width = 1920;
    int height = 1080;
    int stride = 1920;
    AX_IMG_FORMAT_E format = AX_FORMAT_YUV420_SEMIPLANAR;
    int src_fps = 0;
    int dst_fps = 0;
    int out_fifo_depth = 0;
    AX_IVPS_ENGINE_E engine = AX_IVPS_ENGINE_SCL;
};

struct IvpsGroupConfig {
    int grp_id = 0;
    int in_fifo_depth = 2;
    int in_debug_fifo_depth = 0;
    AX_IVPS_FRC_MODE_E frc_mode = AX_IVPS_FRC_RATIO;
    std::vector<IvpsChannelConfig> channels;
};

class AxIvpsAdapter {
public:
    AxIvpsAdapter() = default;
    ~AxIvpsAdapter();

    AxIvpsAdapter(const AxIvpsAdapter&) = delete;
    AxIvpsAdapter& operator=(const AxIvpsAdapter&) = delete;

    bool Open(const IvpsGroupConfig& cfg);
    bool Start();
    void Stop();
    void Close();

    IVPS_RGN_HANDLE CreateRegion(IVPS_FILTER_ID filter, const AX_IVPS_RGN_DISP_GROUP_T& disp);
    bool UpdateRegion(IVPS_RGN_HANDLE handle, const AX_IVPS_RGN_DISP_GROUP_T& disp);
    void DestroyRegion(IVPS_RGN_HANDLE handle, IVPS_FILTER_ID filter);

    bool IsOpen() const { return opened_; }
    bool IsRunning() const { return running_; }
    IVPS_GRP GetGrpId() const { return cfg_.grp_id; }
    int GetChannelCount() const { return static_cast<int>(enabled_channels_.size()); }

private:
    bool SetPipelineAttr();
    bool EnableChannels();
    void DisableChannels();

    IvpsGroupConfig cfg_{};
    bool ivps_initialized_ = false;
    bool group_created_ = false;
    bool opened_ = false;
    bool running_ = false;
    std::vector<IVPS_CHN> enabled_channels_;
};

}  // namespace aov::media::ax615
