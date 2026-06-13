# VIN Module Debug Analysis - 0x80110180 Error

## Problem Summary

**Error**: `AX_VIN_StartPipe failed: 0x80110180` (AX_ERR_VIN_FAILED)

**Context**:
- QSDemo runs successfully on the board
- libmedia_demo fails at VIN startup
- All initialization steps before StartPipe complete successfully
- Error occurs during pipeline startup phase

## Root Cause

### Critical Sequence Difference

**QSDemo Sequence (WORKING):**
```
1. QS_COMMON_ISP_Init
   - AX_ISP_Create
   - Register Sensor
   - Register 3A (AE/AWB)
   - Load IQ bin
   - AX_ISP_Open ✓

2. QS_COMMON_VIN_StartChn
   - Set channel attributes ✓

3. AX_VIN_StartPipe ✓          ← Must happen BEFORE AX_ISP_Start!

4. AX_ISP_Start ✓               ← Must happen AFTER AX_VIN_StartPipe!

5. QS_COMMON_VIN_StartDev
   - AX_VIN_EnableDev ✓

6. AX_ISP_StreamOn ✓
```

**libmedia Original Sequence (FAILING):**
```
1. ISP Open
   - AX_ISP_Create ✓
   - Register Sensor ✓
   - Register 3A ✓
   - AX_ISP_Open ✓

2. VIN Open
   - Device/Pipe/Channel creation ✓

3. Link establishment ✓

4. AX_ISP_Start ✓               ← TOO EARLY! This is the bug!

5. vin_->Enable()
   - Re-set channel attributes ✓
   - AX_VIN_StartPipe ✗          ← FAILS HERE (0x80110180)
   - AX_VIN_EnableDev (not reached)

6. AX_ISP_StreamOn (not reached)
```

### Key Finding

**The SDK requires AX_VIN_StartPipe to be called BEFORE AX_ISP_Start!**

This is counterintuitive but confirmed by analyzing QSDemo's working sequence in:
- `docs/QSDemo/src/com/qs_common_cam.c` lines 282-348

## Applied Fixes

### 1. Fix Startup Sequence (media_pipeline_manager.cpp)

**File**: `libmedia/Src/pipeline/media_pipeline_manager.cpp`

Changed the order to match QSDemo:
- **Before**: ISP Start → VIN Enable → ISP StreamOn
- **After**: VIN Enable (StartPipe + EnableDev) → ISP Start → ISP StreamOn

```cpp
// 1. AX_VIN_StartPipe (must be BEFORE AX_ISP_Start to avoid 0x80110180 error)
if (!vin_->Enable()) {
    return MediaStatusCode::InternalError;
}

// 2. AX_ISP_Start (must be AFTER AX_VIN_StartPipe)
if (!isp_->Start()) {
    return MediaStatusCode::InternalError;
}

// 3. AX_ISP_StreamOn (start streaming, must be AFTER AX_VIN_EnableDev)
if (!isp_->StreamOn()) {
    return MediaStatusCode::InternalError;
}
```

### 2. Add Detailed Diagnostics (ax_vin_adapter.cpp)

**File**: `libmedia/Src/adapter/ax615/vin/ax_vin_adapter.cpp`

Added comprehensive pre-StartPipe diagnostics to catch any remaining issues:
- Pipe and device configuration verification
- State checks before StartPipe
- Detailed error reporting with context

```cpp
std::fprintf(stderr, "[AxVinAdapter][v20250609] Pre-StartPipe diagnostics:\n");
std::fprintf(stderr, "[AxVinAdapter][v20250609]   pipe_id=%d, dev_id=%d\n", ...);
std::fprintf(stderr, "[AxVinAdapter][v20250609]   dev_mode=%d, pipe_work_mode=%d\n", ...);
// ... more diagnostics
```

## Configuration Verification

### OS04D10 Sensor Configuration Comparison

| Parameter | QSDemo | libmedia | Match |
|-----------|--------|----------|-------|
| Resolution | 2560x1440 | 2560x1440 | ✓ |
| FPS | 30 | 30 | ✓ |
| Dev Mode | AX_VIN_DEV_ONLINE | AX_VIN_DEV_ONLINE | ✓ |
| Pipe Work Mode | AX_VIN_PIPE_NORMAL_MODE1 | AX_VIN_PIPE_NORMAL_MODE1 | ✓ |
| MIPI Lanes | AX_MIPI_DATA_LANE_2 | AX_MIPI_DATA_LANE_2 | ✓ |
| MIPI Data Rate | 720 | 720 | ✓ |
| Pixel Format | AX_FORMAT_BAYER_RAW_10BPP_PACKED | AX_FORMAT_BAYER_RAW_10BPP_PACKED | ✓ |
| Bayer Pattern | AX_BP_RGGB | AX_BP_RGGB | ✓ |
| Pipe Compress | LOSSY, 4 | LOSSY, 4 | ✓ |
| Dev Compress | NONE, 0 | NONE, 0 | ✓ |
| Channel Format | YUV420_SEMIPLANAR | YUV420_SEMIPLANAR | ✓ |

**Conclusion**: Configuration matches QSDemo exactly.

## Expected Outcome

With the sequence fix, the VIN module should now initialize successfully:

```
[Pipeline] Starting VIN (AX_VIN_StartPipe + AX_VIN_EnableDev)
[AxVinAdapter] Pre-StartPipe diagnostics:
[AxVinAdapter]   pipe_id=0, dev_id=0
[AxVinAdapter]   dev_mode=0, pipe_work_mode=1
[AxVinAdapter]   pipe_region=2560x1440, bAiIspEnable=0
[AxVinAdapter] Calling AX_VIN_StartPipe
[AxVinAdapter] AX_VIN_StartPipe OK          ← Should succeed now!
[AxVinAdapter] Calling AX_VIN_EnableDev
[AxVinAdapter] Enable completed successfully
[Pipeline] VIN Enable OK
[Pipeline] Starting ISP (AX_ISP_Start)
[Pipeline] ISP StreamOn (AX_ISP_StreamOn)
```

## Next Steps

1. **Rebuild libmedia**
   ```bash
   cd libmedia/Build
   make clean && make
   ```

2. **Test on board**
   ```bash
   cd /tmp
   ./run_libmedia_demo.sh -x 3 -o /tmp/libmedia_demo.h265
   ```

3. **Verify output**
   - Check that VIN StartPipe succeeds
   - Confirm ISP Start happens after VIN
   - Verify video frames are captured
   - Check output file is valid H.265 stream

## References

- QSDemo: `docs/QSDemo/src/com/qs_common_cam.c` (lines 282-348)
- QSDemo ISP Init: `docs/QSDemo/src/com/qs_common_isp.c` (QS_COMMON_ISP_Init)
- SDK Error Code: 0x80110180 = AX_ERR_VIN_FAILED (generic VIN failure)
- Sensor Config: `docs/QSDemo/src/vin_config.h` (gOs04d10DevAttr, gOs04d10PipeAttr)

---

**Date**: 2026-06-11
**Version**: v20250609-fix2
