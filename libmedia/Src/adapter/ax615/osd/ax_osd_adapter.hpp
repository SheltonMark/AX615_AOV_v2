#pragma once

#include "common/media_types.hpp"

#include <vector>

extern "C" {
#include "ax_ivps_api.h"
}

namespace aov::media::ax615 {

struct AxOsdRegionConfig {
    int grp_id = 0;
    int filter_id = 0;
    int x = 0;
    int y = 0;
    int width = 0;
    int height = 0;
};

class AxOsdAdapter {
public:
    AxOsdAdapter() = default;
    ~AxOsdAdapter();

    AxOsdAdapter(const AxOsdAdapter&) = delete;
    AxOsdAdapter& operator=(const AxOsdAdapter&) = delete;

    MediaStatusCode Open() { opened_ = true; return MediaStatusCode::Ok; }
    void Close();
    MediaStatusCode CreateRegion(IVPS_GRP grp_id, IVPS_FILTER_ID filter_id, const AX_IVPS_RGN_DISP_GROUP_T& disp);
    MediaStatusCode UpdateRegion(IVPS_RGN_HANDLE handle, const AX_IVPS_RGN_DISP_GROUP_T& disp);
    MediaStatusCode DestroyRegion(IVPS_RGN_HANDLE handle);
    MediaStatusCode UpdateDetectOverlay(const DetectResultSummary& result);
    MediaStatusCode ClearAll();
    bool IsOpen() const { return opened_; }

private:
    struct Region {
        IVPS_RGN_HANDLE handle = AX_IVPS_INVALID_REGION_HANDLE;
        IVPS_GRP grp_id = 0;
        IVPS_FILTER_ID filter_id = 0;
    };

    Region* FindRegion(IVPS_RGN_HANDLE handle);

    std::vector<Region> regions_;
    bool opened_ = false;
};

}  // namespace aov::media::ax615
