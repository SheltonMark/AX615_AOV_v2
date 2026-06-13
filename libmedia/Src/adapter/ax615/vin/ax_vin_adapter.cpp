#include "ax_vin_adapter.hpp"

#include "ax_buffer_tool.h"
#include <cstdio>
#include <cstring>

namespace aov::media::ax615 {

AxVinAdapter::~AxVinAdapter() {
    Close();
}

bool AxVinAdapter::Open(const VinOpenOptions& options) {
    const SensorProfile* profile = SensorRegistry::Instance().Find(options.sensor_name);
    if (!profile) {
        std::fprintf(stderr, "[AxVinAdapter] sensor not registered: %s\n", options.sensor_name.c_str());
        return false;
    }
    chn_frame_mode_ = options.chn_frame_mode;
    return Open(*profile, options.input_mode, options.lane_combo);
}

bool AxVinAdapter::Open(const SensorProfile& profile,
                        AX_INPUT_MODE_E input_mode,
                        AX_LANE_COMBO_MODE_E lane_combo) {
    std::fprintf(stderr, "[AxVinAdapter][v20260611] Open: sensor=%s, pipe_id=%d, dev_id=%d\n",
                 profile.sensor_name.c_str(), profile.pipe_id, profile.dev_id);
    if (open_) {
        std::fprintf(stderr, "[AxVinAdapter][v20260611] Already open\n");
        return true;
    }
    if (profile.sensor_name.empty() || profile.chn_attrs.empty()) {
        std::fprintf(stderr, "[AxVinAdapter][v20260611] Invalid profile\n");
        return false;
    }

    std::fprintf(stderr, "[AxVinAdapter][v20260611] Calling AX_VIN_Init\n");
    AX_S32 ret = AX_VIN_Init();
    if (ret != AX_SUCCESS) {
        std::fprintf(stderr, "[AxVinAdapter] AX_VIN_Init failed: 0x%x\n", ret);
        return false;
    }
    vin_initialized_ = true;

    // CRITICAL FIX: QSDemo calls AX_VIN_SetLowMemMode(0) after AX_VIN_Init
    // (see qs_common_cam.c lines 138-146). OS04D10 uses nLowMemMode=0 (normal mode)
    // which corresponds to AX_VIN_PIPE_NORMAL_MODE0
    std::fprintf(stderr, "[AxVinAdapter][v20260611] Calling AX_VIN_SetLowMemMode(0)\n");
    ret = AX_VIN_SetLowMemMode(0);
    if (ret != AX_SUCCESS) {
        std::fprintf(stderr, "[AxVinAdapter] AX_VIN_SetLowMemMode(0) failed: 0x%x\n", ret);
        Close();
        return false;
    }
    std::fprintf(stderr, "[AxVinAdapter][v20260611] AX_VIN_SetLowMemMode(0) OK\n");

    // CRITICAL FIX: Configure VIN private pool (QSDemo's QS_COMMON_CAM_PrivPoolInit)
    // Must be called AFTER AX_VIN_Init but BEFORE AX_VIN_CreateDev
    // This sets up RAW buffer pools that VIN pipe needs for internal frame buffering
    std::fprintf(stderr, "[AxVinAdapter][v20260611] Configuring VIN private pool\n");
    {
        AX_POOL_FLOORPLAN_T tPoolFloorPlan;
        std::memset(&tPoolFloorPlan, 0, sizeof(tPoolFloorPlan));

        // Match QSDemo's gtPrivatePoolSingleOs04d10 from vin_config.c:
        // {2560, 1440, 2560, AX_FORMAT_BAYER_RAW_10BPP_PACKED, 4, 2, 4, 0}
        AX_FRAME_COMPRESS_INFO_T tCompressInfo;
        std::memset(&tCompressInfo, 0, sizeof(tCompressInfo));
        tCompressInfo.enCompressMode = static_cast<AX_COMPRESS_MODE_E>(2);
        tCompressInfo.u32CompressLevel = 4;

        AX_POOL_CONFIG_T pool_cfg;
        std::memset(&pool_cfg, 0, sizeof(pool_cfg));
        pool_cfg.MetaSize = 1024;
        pool_cfg.AlignSize = 4096;
        pool_cfg.CacheMode = AX_POOL_CACHE_MODE_NONCACHE;
        std::strncpy(reinterpret_cast<char*>(pool_cfg.PartitionName), "anonymous", AX_MAX_PARTITION_NAME_LEN - 1);

        // Calculate buffer size using AX_VIN_GetBufferSize
        pool_cfg.BlkSize = AX_VIN_GetBufferSize(1440, 2560, AX_FORMAT_BAYER_RAW_10BPP_PACKED, &tCompressInfo, 0, 0);
        pool_cfg.BlkCnt = 4;

        tPoolFloorPlan.CommPool[0] = pool_cfg;

        ret = AX_VIN_SetPoolAttr(&tPoolFloorPlan);
        if (ret != AX_SUCCESS) {
            std::fprintf(stderr, "[AxVinAdapter] AX_VIN_SetPoolAttr failed: 0x%x\n", ret);
            Close();
            return false;
        }
        std::fprintf(stderr, "[AxVinAdapter][v20260611] AX_VIN_SetPoolAttr OK (BlkSize=%llu, BlkCnt=%u)\n",
                     pool_cfg.BlkSize, pool_cfg.BlkCnt);
    }

    std::fprintf(stderr, "[AxVinAdapter][v20260611] Calling AX_MIPI_RX_Init\n");
    ret = AX_MIPI_RX_Init();
    if (ret != AX_SUCCESS) {
        std::fprintf(stderr, "[AxVinAdapter] AX_MIPI_RX_Init failed: 0x%x\n", ret);
        Close();
        return false;
    }
    mipi_initialized_ = true;
    profile_ = profile;

    std::fprintf(stderr, "[AxVinAdapter][v20260611] Starting MIPI\n");
    if (!StartMipi(profile_, input_mode, lane_combo)) {
        std::fprintf(stderr, "[AxVinAdapter][v20260611] StartMipi failed\n");
        Close();
        return false;
    }
    std::fprintf(stderr, "[AxVinAdapter][v20260611] Creating device\n");
    if (!CreateDevice(profile_)) {
        std::fprintf(stderr, "[AxVinAdapter][v20260611] CreateDevice failed\n");
        Close();
        return false;
    }
    std::fprintf(stderr, "[AxVinAdapter][v20260611] Creating pipe and channels\n");
    if (!CreatePipeAndChannels(profile_)) {
        std::fprintf(stderr, "[AxVinAdapter][v20260611] CreatePipeAndChannels failed\n");
        Close();
        return false;
    }

    open_ = true;
    std::fprintf(stderr, "[AxVinAdapter][v20260611] Open completed successfully\n");
    return true;
}

bool AxVinAdapter::Enable() {
    if (!open_ || enabled_) {
        return open_;
    }

    std::fprintf(stderr, "[AxVinAdapter][v20260611] Enable: dev_id=%d, pipe_id=%d, dev_mode=%d\n",
                 profile_.dev_id, profile_.pipe_id, profile_.dev_attr.eDevMode);

    // QSDemo re-sets channel attributes before StartPipe (in QS_COMMON_VIN_StartChn)!
    // CRITICAL: This is when channels are actually ENABLED (after ISP Init)
    std::fprintf(stderr, "[AxVinAdapter][DEBUG] ========== Enable() Start ==========\n");
    std::fprintf(stderr, "[AxVinAdapter][DEBUG] profile_.chn_attrs.size=%zu, enabled_channels_.size=%zu\n",
                 profile_.chn_attrs.size(), enabled_channels_.size());

    std::fprintf(stderr, "[AxVinAdapter][v20260611] Re-setting channel attributes and enabling channels\n");
    for (size_t i = 0; i < profile_.chn_attrs.size() && i < enabled_channels_.size(); ++i) {
        AX_VIN_CHN_ID_E chn = enabled_channels_[i];
        AX_VIN_CHN_ATTR_T chn_attr = profile_.chn_attrs[i];

        std::fprintf(stderr, "[AxVinAdapter][DEBUG] Channel %zu: chn_id=%d, size=%dx%d, stride=%d, format=%d, depth=%d\n",
                     i, chn, chn_attr.nWidth, chn_attr.nHeight, chn_attr.nWidthStride,
                     chn_attr.eImgFormat, chn_attr.nDepth);

        AX_S32 ret = AX_VIN_SetChnAttr(profile_.pipe_id, chn, &chn_attr);
        if (ret != AX_SUCCESS) {
            std::fprintf(stderr, "[AxVinAdapter] Re-set AX_VIN_SetChnAttr[%zu] failed: 0x%x\n", i, ret);
            return false;
        }
        std::fprintf(stderr, "[AxVinAdapter][DEBUG] Channel %zu: SetChnAttr OK\n", i);

        AX_VIN_FRAME_MODE_E frame_mode = static_cast<AX_VIN_FRAME_MODE_E>(chn_frame_mode_);
        ret = AX_VIN_SetChnFrameMode(profile_.pipe_id, chn, frame_mode);
        if (ret != AX_SUCCESS) {
            std::fprintf(stderr, "[AxVinAdapter] Re-set AX_VIN_SetChnFrameMode[%zu] failed: 0x%x\n", i, ret);
            return false;
        }
        std::fprintf(stderr, "[AxVinAdapter][DEBUG] Channel %zu: SetChnFrameMode=%d OK\n", i, frame_mode);

        // NOW enable the channel (QSDemo does this in QS_COMMON_VIN_StartChn, AFTER ISP Init)
        std::fprintf(stderr, "[AxVinAdapter][v20260611] Channel %zu: EnableChn (after ISP Init)\n", i);
        ret = AX_VIN_EnableChn(profile_.pipe_id, chn);
        if (ret != AX_SUCCESS) {
            std::fprintf(stderr, "[AxVinAdapter] AX_VIN_EnableChn[%zu] failed: 0x%x\n", i, ret);
            return false;
        }
        std::fprintf(stderr, "[AxVinAdapter][DEBUG] Channel %zu: EnableChn OK\n", i);
    }
    std::fprintf(stderr, "[AxVinAdapter][DEBUG] All channels enabled\n");

    // QSDemo calls AX_VIN_StartPipe BEFORE AX_VIN_EnableDev!
    // Add detailed diagnostics before StartPipe
    std::fprintf(stderr, "[AxVinAdapter][DEBUG] ========== Pre-StartPipe Diagnostics ==========\n");
    std::fprintf(stderr, "[AxVinAdapter][v20260611] Pre-StartPipe diagnostics:\n");
    std::fprintf(stderr, "[AxVinAdapter][v20260611]   pipe_id=%d, dev_id=%d\n", profile_.pipe_id, profile_.dev_id);
    std::fprintf(stderr, "[AxVinAdapter][v20260611]   dev_mode=%d, pipe_work_mode=%d\n",
                 profile_.dev_attr.eDevMode, profile_.pipe_attr.ePipeWorkMode);
    std::fprintf(stderr, "[AxVinAdapter][v20260611]   pipe_region=%dx%d, bAiIspEnable=%d\n",
                 profile_.pipe_attr.tPipeImgRgn.nWidth, profile_.pipe_attr.tPipeImgRgn.nHeight,
                 profile_.pipe_attr.bAiIspEnable);
    std::fprintf(stderr, "[AxVinAdapter][v20260611]   enabled_channels=%zu, frame_mode=%d\n",
                 enabled_channels_.size(), chn_frame_mode_);

    // Verify VIN pipe state before StartPipe
    AX_VIN_PIPE_ATTR_T check_pipe_attr;
    AX_S32 check_ret = AX_VIN_GetPipeAttr(profile_.pipe_id, &check_pipe_attr);
    std::fprintf(stderr, "[AxVinAdapter][v20260611]   GetPipeAttr check: ret=0x%x\n", check_ret);
    if (check_ret == AX_SUCCESS) {
        std::fprintf(stderr, "[AxVinAdapter][DEBUG]   Pipe: work_mode=%d, region=%dx%d, format=%d, bayer=%d\n",
                     check_pipe_attr.ePipeWorkMode,
                     check_pipe_attr.tPipeImgRgn.nWidth, check_pipe_attr.tPipeImgRgn.nHeight,
                     check_pipe_attr.ePixelFmt, check_pipe_attr.eBayerPattern);
        std::fprintf(stderr, "[AxVinAdapter][DEBUG]   Pipe: sns_mode=%d, compress=%d/%d\n",
                     check_pipe_attr.eSnsMode,
                     check_pipe_attr.tCompressInfo.enCompressMode,
                     check_pipe_attr.tCompressInfo.u32CompressLevel);
    }

    // Verify VIN device state
    AX_VIN_DEV_ATTR_T check_dev_attr;
    check_ret = AX_VIN_GetDevAttr(profile_.dev_id, &check_dev_attr);
    std::fprintf(stderr, "[AxVinAdapter][v20260611]   GetDevAttr check: ret=0x%x\n", check_ret);
    if (check_ret == AX_SUCCESS) {
        std::fprintf(stderr, "[AxVinAdapter][DEBUG]   Dev: mode=%d, work_mode=%d, intf_type=%d\n",
                     check_dev_attr.eDevMode, check_dev_attr.eDevWorkMode, check_dev_attr.eSnsIntfType);
        std::fprintf(stderr, "[AxVinAdapter][DEBUG]   Dev: region[0]=%dx%d, format=%d, bayer=%d\n",
                     check_dev_attr.tDevImgRgn[0].nWidth, check_dev_attr.tDevImgRgn[0].nHeight,
                     check_dev_attr.ePixelFmt, check_dev_attr.eBayerPattern);
        std::fprintf(stderr, "[AxVinAdapter][DEBUG]   Dev: sns_mode=%d, output_mode=%d\n",
                     check_dev_attr.eSnsMode, check_dev_attr.eSnsOutputMode);
    }

    // Check channel status
    for (size_t i = 0; i < enabled_channels_.size(); ++i) {
        AX_VIN_CHN_ATTR_T check_chn_attr;
        check_ret = AX_VIN_GetChnAttr(profile_.pipe_id, enabled_channels_[i], &check_chn_attr);
        std::fprintf(stderr, "[AxVinAdapter][DEBUG]   GetChnAttr[%zu] ret=0x%x", i, check_ret);
        if (check_ret == AX_SUCCESS) {
            std::fprintf(stderr, ", size=%dx%d, stride=%d, format=%d",
                         check_chn_attr.nWidth, check_chn_attr.nHeight,
                         check_chn_attr.nWidthStride, check_chn_attr.eImgFormat);
        }
        std::fprintf(stderr, "\n");
    }

    std::fprintf(stderr, "[AxVinAdapter][DEBUG] ========== Calling AX_VIN_StartPipe ==========\n");
    std::fprintf(stderr, "[AxVinAdapter][v20260611] Calling AX_VIN_StartPipe\n");
    AX_S32 ret = AX_VIN_StartPipe(profile_.pipe_id);
    std::fprintf(stderr, "[AxVinAdapter][DEBUG] AX_VIN_StartPipe returned: 0x%x\n", ret);

    if (ret != AX_SUCCESS) {
        std::fprintf(stderr, "[AxVinAdapter] AX_VIN_StartPipe failed: 0x%x\n", ret);
        std::fprintf(stderr, "[AxVinAdapter] Error details: pipe_id=%d, dev_id=%d, dev_mode=%d, work_mode=%d\n",
                     profile_.pipe_id, profile_.dev_id, profile_.dev_attr.eDevMode, profile_.pipe_attr.ePipeWorkMode);
        std::fprintf(stderr, "[AxVinAdapter][DEBUG] ========== FAILURE ANALYSIS ==========\n");
        std::fprintf(stderr, "[AxVinAdapter][DEBUG] Possible causes:\n");
        std::fprintf(stderr, "[AxVinAdapter][DEBUG]   1. ISP not properly initialized\n");
        std::fprintf(stderr, "[AxVinAdapter][DEBUG]   2. Pipe/Device configuration mismatch\n");
        std::fprintf(stderr, "[AxVinAdapter][DEBUG]   3. Sensor not responding\n");
        std::fprintf(stderr, "[AxVinAdapter][DEBUG]   4. MIPI not configured correctly\n");
        return false;
    }
    pipe_started_ = true;
    std::fprintf(stderr, "[AxVinAdapter][v20260611] AX_VIN_StartPipe OK\n");

    enabled_ = true;
    std::fprintf(stderr, "[AxVinAdapter][v20260611] Enable completed (StartPipe done, EnableDev pending)\n");
    return true;
}

bool AxVinAdapter::StartDev() {
    if (!pipe_started_) {
        std::fprintf(stderr, "[AxVinAdapter] StartDev: pipe not started yet\n");
        return false;
    }
    if (dev_enabled_) {
        std::fprintf(stderr, "[AxVinAdapter][v20260611] StartDev: dev already enabled\n");
        return true;
    }

    std::fprintf(stderr, "[AxVinAdapter][v20260611] ========== StartDev (after ISP Start) ==========\n");

    // Set dump attribute for OFFLINE mode (like QSDemo does)
    if (profile_.dev_attr.eDevMode == AX_VIN_DEV_OFFLINE) {
        std::fprintf(stderr, "[AxVinAdapter][v20260611] Setting dump attr for OFFLINE mode\n");
        AX_VIN_DUMP_ATTR_T dump_attr;
        std::memset(&dump_attr, 0, sizeof(dump_attr));
        dump_attr.bEnable = AX_TRUE;
        dump_attr.nDepth = 3;
        AX_S32 ret = AX_VIN_SetDevDumpAttr(profile_.dev_id, AX_VIN_DUMP_QUEUE_TYPE_DEV, &dump_attr);
        if (ret != AX_SUCCESS) {
            std::fprintf(stderr, "[AxVinAdapter] AX_VIN_SetDevDumpAttr failed: 0x%x\n", ret);
            return false;
        }
    }

    std::fprintf(stderr, "[AxVinAdapter][v20260611] Calling AX_VIN_EnableDev\n");
    AX_S32 ret = AX_VIN_EnableDev(profile_.dev_id);
    if (ret != AX_SUCCESS) {
        std::fprintf(stderr, "[AxVinAdapter] AX_VIN_EnableDev failed: 0x%x\n", ret);
        return false;
    }
    dev_enabled_ = true;

    std::fprintf(stderr, "[AxVinAdapter][v20260611] StartDev completed successfully\n");
    return true;
}

void AxVinAdapter::Disable() {
    std::fprintf(stderr, "[AxVinAdapter][DEBUG] Disable called, enabled=%d, pipe_started=%d, dev_enabled=%d\n",
                 enabled_, pipe_started_, dev_enabled_);
    if (!enabled_) {
        return;
    }
    if (pipe_started_) {
        std::fprintf(stderr, "[AxVinAdapter][DEBUG] Calling AX_VIN_StopPipe\n");
        AX_VIN_StopPipe(profile_.pipe_id);
        pipe_started_ = false;
    }
    if (dev_enabled_) {
        std::fprintf(stderr, "[AxVinAdapter][DEBUG] Calling AX_VIN_DisableDev\n");
        AX_VIN_DisableDev(profile_.dev_id);
        dev_enabled_ = false;
    }
    enabled_ = false;
    std::fprintf(stderr, "[AxVinAdapter][DEBUG] Disable completed\n");
}

void AxVinAdapter::Close() {
    std::fprintf(stderr, "[AxVinAdapter][DEBUG] Close called\n");
    Disable();
    std::fprintf(stderr, "[AxVinAdapter][DEBUG] Calling DestroyPipeAndChannels\n");
    DestroyPipeAndChannels();
    std::fprintf(stderr, "[AxVinAdapter][DEBUG] Calling DestroyDevice\n");
    DestroyDevice();
    std::fprintf(stderr, "[AxVinAdapter][DEBUG] Calling StopMipi\n");
    StopMipi();

    if (mipi_initialized_) {
        std::fprintf(stderr, "[AxVinAdapter][DEBUG] Calling AX_MIPI_RX_DeInit\n");
        AX_MIPI_RX_DeInit();
        mipi_initialized_ = false;
    }

    if (vin_initialized_) {
        std::fprintf(stderr, "[AxVinAdapter][DEBUG] Calling AX_VIN_Deinit\n");
        AX_VIN_Deinit();
        vin_initialized_ = false;
    }

    open_ = false;
    profile_ = {};
    std::fprintf(stderr, "[AxVinAdapter][DEBUG] Close completed\n");
}

bool AxVinAdapter::StartMipi(const SensorProfile& profile,
                             AX_INPUT_MODE_E input_mode,
                             AX_LANE_COMBO_MODE_E lane_combo) {
    std::fprintf(stderr, "[AxVinAdapter][DEBUG] ========== StartMipi ==========\n");
    std::fprintf(stderr, "[AxVinAdapter][v20260611] SetLaneCombo=%d, rx=%d, lane_num=%d, data_rate=%d, lane_map={%d,%d,%d,%d}, clk={%d,%d}\n",
                 lane_combo, profile.rx_dev, profile.mipi_attr.eLaneNum, profile.mipi_attr.nDataRate,
                 profile.mipi_attr.nDataLaneMap[0], profile.mipi_attr.nDataLaneMap[1],
                 profile.mipi_attr.nDataLaneMap[2], profile.mipi_attr.nDataLaneMap[3],
                 profile.mipi_attr.nClkLane[0], profile.mipi_attr.nClkLane[1]);
    std::fprintf(stderr, "[AxVinAdapter][DEBUG] input_mode=%d, phy_mode=%d\n",
                 input_mode, profile.mipi_attr.ePhyMode);

    AX_MIPI_RX_DEV_T mipi_dev;
    std::memset(&mipi_dev, 0, sizeof(mipi_dev));
    mipi_dev.eInputMode = input_mode;
    mipi_dev.tMipiAttr = profile.mipi_attr;

    AX_MIPI_RX_SetLaneCombo(lane_combo);

    AX_S32 ret = AX_MIPI_RX_SetAttr(profile.rx_dev, &mipi_dev);
    if (ret != AX_SUCCESS) {
        std::fprintf(stderr, "[AxVinAdapter] AX_MIPI_RX_SetAttr failed: 0x%x\n", ret);
        return false;
    }
    std::fprintf(stderr, "[AxVinAdapter][DEBUG] AX_MIPI_RX_SetAttr OK\n");

    ret = AX_MIPI_RX_Reset(profile.rx_dev);
    if (ret != AX_SUCCESS) {
        std::fprintf(stderr, "[AxVinAdapter] AX_MIPI_RX_Reset failed: 0x%x\n", ret);
        return false;
    }
    std::fprintf(stderr, "[AxVinAdapter][DEBUG] AX_MIPI_RX_Reset OK\n");

    ret = AX_MIPI_RX_Start(profile.rx_dev);
    if (ret != AX_SUCCESS) {
        std::fprintf(stderr, "[AxVinAdapter] AX_MIPI_RX_Start failed: 0x%x\n", ret);
        return false;
    }
    std::fprintf(stderr, "[AxVinAdapter][DEBUG] AX_MIPI_RX_Start OK\n");

    mipi_started_ = true;
    std::fprintf(stderr, "[AxVinAdapter][DEBUG] ========== StartMipi Complete ==========\n");
    return true;
}

void AxVinAdapter::StopMipi() {
    if (mipi_started_) {
        AX_MIPI_RX_Stop(profile_.rx_dev);
        mipi_started_ = false;
    }
}

bool AxVinAdapter::CreateDevice(const SensorProfile& profile) {
    std::fprintf(stderr, "[AxVinAdapter][DEBUG] ========== CreateDevice ==========\n");
    std::fprintf(stderr, "[AxVinAdapter][DEBUG] dev_id=%d, mode=%d, work_mode=%d, intf_type=%d\n",
                 profile.dev_id, profile.dev_attr.eDevMode,
                 profile.dev_attr.eDevWorkMode, profile.dev_attr.eSnsIntfType);
    std::fprintf(stderr, "[AxVinAdapter][DEBUG] Dev region[0]=%dx%d, format=%d, bayer=%d\n",
                 profile.dev_attr.tDevImgRgn[0].nWidth, profile.dev_attr.tDevImgRgn[0].nHeight,
                 profile.dev_attr.ePixelFmt, profile.dev_attr.eBayerPattern);
    std::fprintf(stderr, "[AxVinAdapter][DEBUG] sns_mode=%d, output_mode=%d, compress=%d/%d\n",
                 profile.dev_attr.eSnsMode, profile.dev_attr.eSnsOutputMode,
                 profile.dev_attr.tCompressInfo.enCompressMode,
                 profile.dev_attr.tCompressInfo.u32CompressLevel);

    AX_VIN_DEV_ATTR_T dev_attr = profile.dev_attr;
    AX_S32 ret = AX_VIN_CreateDev(profile.dev_id, &dev_attr);
    if (ret != AX_SUCCESS) {
        std::fprintf(stderr, "[AxVinAdapter] AX_VIN_CreateDev failed: 0x%x\n", ret);
        return false;
    }
    std::fprintf(stderr, "[AxVinAdapter][DEBUG] AX_VIN_CreateDev OK\n");
    dev_created_ = true;

    ret = AX_VIN_SetDevAttr(profile.dev_id, &dev_attr);
    if (ret != AX_SUCCESS) {
        std::fprintf(stderr, "[AxVinAdapter] AX_VIN_SetDevAttr failed: 0x%x\n", ret);
        return false;
    }
    std::fprintf(stderr, "[AxVinAdapter][DEBUG] AX_VIN_SetDevAttr OK\n");

    // Bind device to pipe (critical!)
    AX_VIN_DEV_BIND_PIPE_T bind_pipe;
    std::memset(&bind_pipe, 0, sizeof(bind_pipe));
    bind_pipe.nNum = 1;
    bind_pipe.nPipeId[0] = profile.pipe_id;
    bind_pipe.nHDRSel[0] = 0;  // SDR mode

    std::fprintf(stderr, "[AxVinAdapter][v20260611] Binding dev %d to pipe %d\n",
                 profile.dev_id, profile.pipe_id);
    ret = AX_VIN_SetDevBindPipe(profile.dev_id, &bind_pipe);
    if (ret != AX_SUCCESS) {
        std::fprintf(stderr, "[AxVinAdapter] AX_VIN_SetDevBindPipe failed: 0x%x\n", ret);
        return false;
    }
    std::fprintf(stderr, "[AxVinAdapter][DEBUG] AX_VIN_SetDevBindPipe OK\n");

    // Bind device to MIPI
    std::fprintf(stderr, "[AxVinAdapter][v20260611] Binding dev %d to MIPI rx_dev %d\n",
                 profile.dev_id, profile.rx_dev);
    ret = AX_VIN_SetDevBindMipi(profile.dev_id, profile.rx_dev);
    if (ret != AX_SUCCESS) {
        std::fprintf(stderr, "[AxVinAdapter] AX_VIN_SetDevBindMipi failed: 0x%x\n", ret);
        return false;
    }
    std::fprintf(stderr, "[AxVinAdapter][DEBUG] AX_VIN_SetDevBindMipi OK\n");
    std::fprintf(stderr, "[AxVinAdapter][DEBUG] ========== CreateDevice Complete ==========\n");

    return true;
}

void AxVinAdapter::DestroyDevice() {
    if (dev_created_) {
        AX_VIN_DestroyDev(profile_.dev_id);
        dev_created_ = false;
    }
}

bool AxVinAdapter::CreatePipeAndChannels(const SensorProfile& profile) {
    AX_VIN_PIPE_ATTR_T pipe_attr = profile.pipe_attr;
    std::fprintf(stderr, "[AxVinAdapter][v20260611] CreatePipe: pipe_id=%d, work_mode=%d\n",
                 profile.pipe_id, pipe_attr.ePipeWorkMode);
    AX_S32 ret = AX_VIN_CreatePipe(profile.pipe_id, &pipe_attr);
    if (ret != AX_SUCCESS) {
        std::fprintf(stderr, "[AxVinAdapter] AX_VIN_CreatePipe failed: 0x%x\n", ret);
        return false;
    }
    pipe_created_ = true;

    std::fprintf(stderr, "[AxVinAdapter][v20260611] SetPipeAttr\n");
    ret = AX_VIN_SetPipeAttr(profile.pipe_id, &pipe_attr);
    if (ret != AX_SUCCESS) {
        std::fprintf(stderr, "[AxVinAdapter] AX_VIN_SetPipeAttr failed: 0x%x\n", ret);
        return false;
    }

    // Set pipe frame source based on pipe mode (like QSDemo does)
    // For NORMAL_MODE1 with DEV input, use IFE from DEV
    if (pipe_attr.ePipeWorkMode == AX_VIN_PIPE_NORMAL_MODE1) {
        std::fprintf(stderr, "[AxVinAdapter][v20260611] SetPipeFrameSource: IFE from DEV\n");
        ret = AX_VIN_SetPipeFrameSource(profile.pipe_id, AX_VIN_FRAME_SOURCE_ID_IFE, AX_VIN_FRAME_SOURCE_TYPE_DEV);
        if (ret != AX_SUCCESS) {
            std::fprintf(stderr, "[AxVinAdapter] AX_VIN_SetPipeFrameSource failed: 0x%x\n", ret);
            return false;
        }
    }

    std::fprintf(stderr, "[AxVinAdapter][v20260611] Configuring %zu channels, chn_frame_mode=%d\n",
                 profile.chn_attrs.size(), chn_frame_mode_);
    for (size_t i = 0; i < profile.chn_attrs.size(); ++i) {
        if (i >= AX_VIN_CHN_ID_MAX) {
            return false;
        }
        AX_VIN_CHN_ID_E chn = static_cast<AX_VIN_CHN_ID_E>(i);
        AX_VIN_CHN_ATTR_T chn_attr = profile.chn_attrs[i];

        std::fprintf(stderr, "[AxVinAdapter][v20260611] Channel %zu: SetChnAttr %dx%d\n",
                     i, chn_attr.nWidth, chn_attr.nHeight);
        ret = AX_VIN_SetChnAttr(profile.pipe_id, chn, &chn_attr);
        if (ret != AX_SUCCESS) {
            std::fprintf(stderr, "[AxVinAdapter] AX_VIN_SetChnAttr[%zu] failed: 0x%x\n", i, ret);
            return false;
        }

        // Set frame mode (but DON'T EnableChn yet - that must happen AFTER ISP Init!)
        AX_VIN_FRAME_MODE_E frame_mode = static_cast<AX_VIN_FRAME_MODE_E>(chn_frame_mode_);
        std::fprintf(stderr, "[AxVinAdapter][v20260611] Channel %zu: SetChnFrameMode=%d\n", i, frame_mode);
        ret = AX_VIN_SetChnFrameMode(profile.pipe_id, chn, frame_mode);
        if (ret != AX_SUCCESS) {
            std::fprintf(stderr, "[AxVinAdapter] AX_VIN_SetChnFrameMode[%zu] failed: 0x%x\n", i, ret);
            return false;
        }

        // NOTE: EnableChn is deferred to Enable() function, after ISP Init (matching QSDemo sequence)
        // QSDemo: ISP_Init -> VIN_StartChn(EnableChn) -> VIN_StartPipe
        // Store channel info for later enabling
        enabled_channels_.push_back(chn);
    }
    std::fprintf(stderr, "[AxVinAdapter][v20260611] CreatePipeAndChannels completed\n");
    return true;
}

void AxVinAdapter::DestroyPipeAndChannels() {
    for (auto it = enabled_channels_.rbegin(); it != enabled_channels_.rend(); ++it) {
        AX_VIN_DisableChn(profile_.pipe_id, *it);
    }
    enabled_channels_.clear();

    if (pipe_created_) {
        AX_VIN_DestroyPipe(profile_.pipe_id);
        pipe_created_ = false;
    }
}

}  // namespace aov::media::ax615
