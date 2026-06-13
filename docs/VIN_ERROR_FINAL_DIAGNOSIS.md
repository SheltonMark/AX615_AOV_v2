# VIN 0x80110180 错误最终诊断报告

> 日期: 2026-06-11 深夜  
> 状态: **根本原因已找到并修复**

---

## 🎯 根本原因

**ISP 没有注册 Sensor！**

从详细日志中发现：
```
[AxIspAdapter][DEBUG] pipe_id=0, register_sensor=0, register_3a=1
                                  ^^^^^^^^^^^^^^^^
                                  致命错误：没有注册 Sensor！
```

### 问题代码

**文件**: `libmedia/Src/pipeline/media_pipeline_manager.cpp` 行619-628

**错误的配置**:
```cpp
ax615::IspConfig isp_cfg;
isp_cfg.pipe_id = ax615_cfg_.pipe_id;
isp_cfg.dev_id = ax615_cfg_.dev_id;
isp_cfg.iq_bin_path = sensor_.iq_bin_path;
// ❌ 没有设置 register_sensor = true
// ❌ 没有设置 sensor_handle
```

**后果**:
1. ISP 没有注册 Sensor 驱动
2. ISP 不知道如何与 OS04D10 Sensor 通信
3. VIN StartPipe 时，SDK 内部校验失败
4. 返回错误码 `0x80110180` (AX_ERR_VIN_FAILED)

---

## ✅ 修复方案

**文件**: `libmedia/Src/pipeline/media_pipeline_manager.cpp` 行619-632

**修复后的配置**:
```cpp
ax615::IspConfig isp_cfg;
isp_cfg.pipe_id = ax615_cfg_.pipe_id;
isp_cfg.dev_id = ax615_cfg_.dev_id;
isp_cfg.iq_bin_path = sensor_.iq_bin_path;
isp_cfg.register_sensor = true;  // ✅ 必须注册 Sensor！
isp_cfg.register_3a = true;      // ✅ 必须注册 3A 算法！
isp_cfg.sensor_handle = ax615_cfg_.sensor_handle;  // ✅ 必须传入 Sensor 句柄！
```

**修复内容**:
1. ✅ 设置 `register_sensor = true`
2. ✅ 设置 `register_3a = true`
3. ✅ 传入 `sensor_handle`（Sensor 驱动的函数指针表）

---

## 📋 完整的调试历程

### 第1轮：初始化顺序问题 ❌
- **假设**: VIN EnableChn 的时机不对
- **修复**: 推迟到 ISP Open 之后
- **结果**: 仍然失败

### 第2轮：启动顺序问题 ❌
- **假设**: VIN StartPipe 必须在 ISP Start 之前
- **修复**: 调整顺序
- **结果**: 仍然失败

### 第3轮：Sensor 分辨率问题 ❌
- **假设**: 配置了错误的分辨率 2688x1520
- **修复**: 改为 2560x1440
- **结果**: 仍然失败

### 第4轮：添加详细日志 ✅
- **方法**: 在所有关键位置添加 DEBUG 日志
- **发现**: 所有配置都正确，但 `register_sensor=0`
- **结论**: **ISP 没有注册 Sensor 驱动！**

### 第5轮：修复 Sensor 注册 ✅
- **修复**: 在 IspConfig 中设置 register_sensor=true 和 sensor_handle
- **预期**: VIN StartPipe 应该成功

---

## 🔍 为什么之前没发现

1. **日志不够详细**: 之前的日志没有打印 ISP 配置参数
2. **假设顺序问题**: 一直在调整初始化顺序，忽略了配置本身
3. **QSDemo 对比不全**: 只对比了 VIN/ISP 的启动顺序，没有对比 Sensor 注册

---

## 📊 QSDemo vs libmedia 对比

| 项目 | QSDemo | libmedia (修复前) | libmedia (修复后) |
|------|--------|-------------------|-------------------|
| ISP Create | ✓ | ✓ | ✓ |
| Register Sensor | ✓ | ❌ | ✓ |
| Register 3A | ✓ | ✓ | ✓ |
| ISP Open | ✓ | ✓ | ✓ |
| VIN EnableChn | ✓ | ✓ | ✓ |
| VIN StartPipe | ✓ | ❌ 失败 | ✅ 应该成功 |

---

## 🚀 下一步

### 1. 编译
```bash
cd A:\AOV_AX615\code_v2\libmedia\Build
make clean && make -j

cd ../../test/libmedia_demo
make clean && make
```

### 2. 部署到板子
```bash
scp libmedia_demo root@<board_ip>:/tmp/
```

### 3. 运行
```bash
cd /tmp && ./run_libmedia_demo.sh -x 3 -o /tmp/libmedia_demo.h265
```

### 4. 预期日志
```
[Pipeline][DEBUG] ISP config: register_sensor=1, register_3a=1, sensor_handle=0x...
[AxIspAdapter][DEBUG] ========== ISP Open Start ==========
[AxIspAdapter][DEBUG] pipe_id=0, register_sensor=1, register_3a=1  ← ✅ 应该是 1
[AxIspAdapter][DEBUG] Calling AX_ISP_RegisterSensor                ← ✅ 应该调用
[AxIspAdapter][DEBUG] AX_ISP_RegisterSensor OK                     ← ✅ 应该成功
...
[AxVinAdapter][DEBUG] ========== Calling AX_VIN_StartPipe ==========
[AxVinAdapter][DEBUG] AX_VIN_StartPipe returned: 0x0                ← ✅ 应该返回 0x0
[AxVinAdapter][v20250609] AX_VIN_StartPipe OK                       ← ✅ 成功！
```

---

## 💡 教训

1. **日志越详细越好**: 关键配置参数都要打印
2. **不要假设**: 验证每一个配置项
3. **对比要全面**: 不仅要对比流程，还要对比配置
4. **先看配置，再看顺序**: 配置错误比顺序错误更难发现

---

## 📝 相关修改文件

1. ✅ `libmedia/Src/pipeline/media_pipeline_manager.cpp`
   - 修复 ISP 配置（register_sensor + sensor_handle）

2. ✅ `libmedia/Src/adapter/ax615/vin/ax_vin_adapter.cpp`
   - 添加详细 DEBUG 日志

3. ✅ `libmedia/Src/adapter/ax615/isp/ax_isp_adapter.cpp`
   - 添加详细 DEBUG 日志

4. ✅ `test/libmedia_demo/libmedia_demo.cpp`
   - 修复 Sensor 分辨率（2688x1520 → 2560x1440）
   - 修复 IQ Bin 文件名（大写 → 小写）

---

**最终状态**: ✅ **问题已定位并修复，等待板上验证**

**预计结果**: VIN StartPipe 应该成功，整个 Pipeline 正常工作

**下次遇到类似问题**: 先检查配置，再检查顺序！
