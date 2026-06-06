#include "ax_osd_adapter.hpp"

namespace aov::media::ax615 {

AxOsdAdapter::~AxOsdAdapter() {
    Close();
}

void AxOsdAdapter::Close() {
    for (auto it = regions_.rbegin(); it != regions_.rend(); ++it) {
        AX_IVPS_RGN_DetachFromFilter(it->handle, it->grp_id, it->filter_id);
        AX_IVPS_RGN_Destroy(it->handle);
    }
    regions_.clear();
    opened_ = false;
}

MediaStatusCode AxOsdAdapter::CreateRegion(IVPS_GRP grp_id, IVPS_FILTER_ID filter_id, const AX_IVPS_RGN_DISP_GROUP_T& disp) {
    if (!opened_) {
        return MediaStatusCode::InvalidState;
    }
    IVPS_RGN_HANDLE handle = AX_IVPS_RGN_Create();
    if (handle == AX_IVPS_INVALID_REGION_HANDLE) {
        return MediaStatusCode::InternalError;
    }
    if (AX_IVPS_RGN_AttachToFilter(handle, grp_id, filter_id) != AX_SUCCESS) {
        AX_IVPS_RGN_Destroy(handle);
        return MediaStatusCode::InternalError;
    }
    if (AX_IVPS_RGN_Update(handle, &disp) != AX_SUCCESS) {
        AX_IVPS_RGN_DetachFromFilter(handle, grp_id, filter_id);
        AX_IVPS_RGN_Destroy(handle);
        return MediaStatusCode::InternalError;
    }
    regions_.push_back({handle, grp_id, filter_id});
    return MediaStatusCode::Ok;
}

MediaStatusCode AxOsdAdapter::UpdateRegion(IVPS_RGN_HANDLE handle, const AX_IVPS_RGN_DISP_GROUP_T& disp) {
    if (!opened_ || !FindRegion(handle)) {
        return MediaStatusCode::InvalidState;
    }
    return AX_IVPS_RGN_Update(handle, &disp) == AX_SUCCESS
        ? MediaStatusCode::Ok
        : MediaStatusCode::InternalError;
}

MediaStatusCode AxOsdAdapter::DestroyRegion(IVPS_RGN_HANDLE handle) {
    if (!opened_) {
        return MediaStatusCode::InvalidState;
    }
    for (auto it = regions_.begin(); it != regions_.end(); ++it) {
        if (it->handle != handle) {
            continue;
        }
        AX_IVPS_RGN_DetachFromFilter(it->handle, it->grp_id, it->filter_id);
        AX_IVPS_RGN_Destroy(it->handle);
        regions_.erase(it);
        return MediaStatusCode::Ok;
    }
    return MediaStatusCode::InvalidArgument;
}

MediaStatusCode AxOsdAdapter::UpdateDetectOverlay(const DetectResultSummary&) {
    return opened_ ? MediaStatusCode::Ok : MediaStatusCode::InvalidState;
}

MediaStatusCode AxOsdAdapter::ClearAll() {
    if (!opened_) {
        return MediaStatusCode::InvalidState;
    }
    for (const auto& region : regions_) {
        AX_IVPS_RGN_DISP_GROUP_T disp{};
        AX_IVPS_RGN_Update(region.handle, &disp);
    }
    return MediaStatusCode::Ok;
}

AxOsdAdapter::Region* AxOsdAdapter::FindRegion(IVPS_RGN_HANDLE handle) {
    for (auto& region : regions_) {
        if (region.handle == handle) {
            return &region;
        }
    }
    return nullptr;
}

}  // namespace aov::media::ax615
