# VIN 模块修复总结

## 问题描述

**错误代码**: `0x80110180` (AX_ERR_VIN_FAILED)  
**失败位置**: `AX_VIN_StartPipe` 调用时失败  
**现象**: QSDemo 能正常运行，libmedia_demo 失败

## 根本原因

通过对比 QSDemo 和 libmedia 的初始化序列，发现**关键顺序错误**：

### QSDemo 正确序列（工作）
```
ISP Create/Open → VIN 配置 → [AX_VIN_StartPipe] → [AX_ISP_Start] → AX_VIN_EnableDev → AX_ISP_StreamOn
                                      ↑                    ↑
                                     先执行              后执行
```

### libmedia 原始序列（失败）
```
ISP Create/Open → VIN 配置 → [AX_ISP_Start] → [AX_VIN_StartPipe] ✗ 失败！
                                      ↑                ↑
                                   错误顺序！        在这里失败
```

**SDK 要求**: `AX_VIN_StartPipe` 必须在 `AX_ISP_Start` **之前**调用！

## 修复内容

### 1. 修改启动顺序 (media_pipeline_manager.cpp:730-760)

**修改文件**: `libmedia/Src/pipeline/media_pipeline_manager.cpp`

**修改内容**: 
- 将 `vin_->Enable()` (包含 AX_VIN_StartPipe) 移到 `isp_->Start()` 之前
- 添加详细注释说明原因和参考位置

**关键代码**:
```cpp
// 1. VIN Enable (AX_VIN_StartPipe + AX_VIN_EnableDev) - 必须在 ISP Start 之前！
if (!vin_->Enable()) {
    return MediaStatusCode::InternalError;
}

// 2. ISP Start - 必须在 VIN StartPipe 之后！
if (!isp_->Start()) {
    return MediaStatusCode::InternalError;
}

// 3. ISP StreamOn - 最后启动流
if (!isp_->StreamOn()) {
    return MediaStatusCode::InternalError;
}
```

### 2. 添加诊断信息 (ax_vin_adapter.cpp:106-137)

**修改文件**: `libmedia/Src/adapter/ax615/vin/ax_vin_adapter.cpp`

**修改内容**:
- 在 `AX_VIN_StartPipe` 之前输出详细的配置信息
- 调用 SDK API 验证 VIN 状态
- 失败时输出完整的错误上下文

**诊断输出**:
```cpp
[AxVinAdapter] Pre-StartPipe diagnostics:
  pipe_id=0, dev_id=0
  dev_mode=0, pipe_work_mode=1
  pipe_region=2560x1440, bAiIspEnable=0
  enabled_channels=1, frame_mode=1
  GetPipeAttr check: ret=0x0
  GetDevAttr check: ret=0x0
```

## 配置验证

已对比 OS04D10 传感器配置，libmedia 与 QSDemo 完全一致：

| 配置项 | 值 | 状态 |
|--------|---|------|
| 分辨率 | 2560x1440 | ✓ |
| 帧率 | 30 fps | ✓ |
| 设备模式 | AX_VIN_DEV_ONLINE | ✓ |
| 工作模式 | AX_VIN_PIPE_NORMAL_MODE1 | ✓ |
| MIPI 通道 | 2 Lane, 720Mbps | ✓ |
| 像素格式 | RAW10 PACKED | ✓ |
| Bayer 模式 | RGGB | ✓ |

## 测试步骤

### 1. 编译
```bash
cd A:\AOV_AX615\code_v2
bash rebuild_and_test.sh
```

或手动编译：
```bash
# 编译 libmedia
cd libmedia/Build
make clean && make -j

# 编译 libmedia_demo
cd ../../test/libmedia_demo
make clean && make -j
```

### 2. 部署到板子
将以下文件复制到板子的 `/tmp/` 目录：
- `test/libmedia_demo/libmedia_demo`
- `test/libmedia_demo/run_libmedia_demo.sh`

### 3. 在板子上运行
```bash
cd /tmp
./run_libmedia_demo.sh -x 3 -o /tmp/libmedia_demo.h265
```

### 4. 预期输出
```
[Pipeline] Starting VIN (AX_VIN_StartPipe + AX_VIN_EnableDev)
[AxVinAdapter] Pre-StartPipe diagnostics:
[AxVinAdapter]   pipe_id=0, dev_id=0
[AxVinAdapter]   dev_mode=0, pipe_work_mode=1
[AxVinAdapter]   pipe_region=2560x1440, bAiIspEnable=0
[AxVinAdapter] Calling AX_VIN_StartPipe
[AxVinAdapter] AX_VIN_StartPipe OK          ← 应该成功！
[AxVinAdapter] Calling AX_VIN_EnableDev
[AxVinAdapter] Enable completed successfully
[Pipeline] VIN Enable OK
[Pipeline] Starting ISP (AX_ISP_Start)
[Pipeline] ISP StreamOn (AX_ISP_StreamOn)
[Pipeline] Starting IVPS
[Pipeline] Starting VENC channels
```

## 修改文件清单

1. ✓ `libmedia/Src/pipeline/media_pipeline_manager.cpp` (行 730-760)
   - 修复启动顺序
   - 添加详细注释

2. ✓ `libmedia/Src/adapter/ax615/vin/ax_vin_adapter.cpp` (行 106-137)
   - 添加诊断输出
   - 增强错误报告

3. ✓ `docs/VIN_DEBUG_ANALYSIS.md` (新建)
   - 详细的问题分析文档

4. ✓ `docs/VIN_FIX_SUMMARY.md` (本文件)
   - 修复总结和测试指南

5. ✓ `rebuild_and_test.sh` (新建)
   - 自动化编译脚本

## 参考资料

- **QSDemo 参考**: `docs/QSDemo/src/com/qs_common_cam.c` (行 282-348)
- **ISP 初始化**: `docs/QSDemo/src/com/qs_common_isp.c` (QS_COMMON_ISP_Init)
- **传感器配置**: `docs/QSDemo/src/vin_config.h` (gOs04d10DevAttr, gOs04d10PipeAttr)
- **SDK 路径**: `A:\ax615_sdk_clean\02.SDK\AX615_SDK_V2.0.0_P5_20260326 181014_NO68`

## 技术要点

1. **AX615 SDK 的 VIN-ISP 依赖关系**:
   - ISP 必须先 Create 和 Open (准备就绪)
   - VIN Pipe 必须先 Start (建立管道)
   - 然后才能启动 ISP (开始处理)
   - 最后 StreamOn (开始数据流)

2. **为什么这个顺序很重要**:
   - VIN StartPipe 会建立与 ISP 的连接通道
   - 如果 ISP 已经启动，VIN 无法正确建立连接
   - SDK 内部有状态机检查这个顺序

3. **调试技巧**:
   - 对比工作的参考代码 (QSDemo)
   - 逐步追踪初始化序列
   - 使用 SDK API 查询状态验证每一步
   - 添加详细日志帮助定位问题

## 后续工作

如果此次修复成功，建议：

1. ✓ **完成 VIN 模块单元测试**
   - 测试不同分辨率
   - 测试不同帧率
   - 测试 ONLINE/OFFLINE 模式

2. **验证完整链路**
   - VIN → ISP → IVPS → VENC 数据流
   - 验证视频编码输出
   - 检查帧率和质量

3. **清理调试代码**
   - 保留关键日志
   - 移除过于详细的诊断信息
   - 添加统一的日志级别控制

---

**修复日期**: 2026-06-11  
**版本标识**: v20250609-fix2  
**修复状态**: 待板上验证
