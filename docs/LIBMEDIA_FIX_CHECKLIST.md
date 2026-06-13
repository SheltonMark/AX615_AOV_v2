# libmedia 修改清单（基于 QSDemo 对齐）

> 基于 QSDEMO_INIT_SEQUENCE.md 和 QSDemo_源码分析文档.md 的综合分析

---

## 当前问题总结

**错误**: `AX_VIN_StartPipe failed: 0x80110180`

**根因**: libmedia 的初始化顺序与 QSDemo 不一致，导致 SDK 内部状态机检查失败

---

## 修改清单

### ✅ 已完成的修改

1. **VIN Adapter - 推迟 EnableChn**
   - ✅ `CreatePipeAndChannels()` 中移除了 `AX_VIN_EnableChn()`
   - ✅ `Enable()` 中添加了 `AX_VIN_EnableChn()`

2. **MediaPipelineManager - 调整启动顺序**
   - ✅ VIN Enable 在 ISP Start 之前
   - ✅ 添加了详细日志

---

## ❌ 待修复的问题

### 问题 1: 链接建立时机可能不对

**QSDemo 顺序**:
```
后台线程:
  1. VENC Init + CreateChn (不 Start)
  2. IVPS Init + CreateGrp (不 Start)
  3. Link Init (建立链接)
  
主线程:
  4. Camera Open (VIN+ISP 启动)
  5. IVPS Start
  6. VENC Start
```

**libmedia 当前**:
```
1. IVPS Open (可能包含 Start?)
2. VENC CreateChannel
3. Link (建立链接)
4. VIN Enable
5. ISP Start
```

**需要确认**:
- [ ] `ivps_->Open()` 是否调用了 `AX_IVPS_StartGrp()`?
- [ ] 如果调用了，需要拆分成 `Configure()` 和 `Start()`
- [ ] VENC 的 `CreateChannel()` 是否调用了 `AX_VENC_StartRecvFrame()`?
- [ ] 如果调用了，需要拆分成 `Create()` 和 `Start()`

---

### 问题 2: IVPS 启动时机

**QSDemo**: IVPS Start 在 Camera 完全启动后
**libmedia**: 需要确认 IVPS Start 的位置

**检查文件**: `libmedia/Src/adapter/ax615/ivps/ax_ivps_adapter.cpp`

**需要做**:
```cpp
// Open() - 只配置，不启动
bool AxIvpsAdapter::Open(config) {
    AX_IVPS_Init();
    AX_IVPS_CreateGrp(...);
    AX_IVPS_SetPipelineAttr(...);
    // ❌ 不要在这里调用 AX_IVPS_StartGrp()
    return true;
}

// Start() - 真正启动
bool AxIvpsAdapter::Start() {
    AX_IVPS_StartGrp(grp_id_);  // ✅ 在这里启动
    return true;
}
```

---

### 问题 3: VENC 启动时机

**QSDemo**: VENC Start 在 IVPS Start 之后
**libmedia**: 需要确认 VENC Start 的位置

**检查文件**: `libmedia/Src/adapter/ax615/venc/ax_venc_adapter.cpp`

**需要做**:
```cpp
// CreateChannel() - 只创建通道
MediaStatusCode AxVencAdapter::CreateChannel(config) {
    AX_VENC_CreateChn(chn_id, &chn_attr);
    AX_VENC_SetRcParam(chn_id, &rc_param);
    AX_VENC_SetGopMode(chn_id, &gop_attr);
    // ❌ 不要在这里调用 AX_VENC_StartRecvFrame()
    return MediaStatusCode::Ok;
}

// Start() - 启动接收帧
MediaStatusCode AxVencAdapter::Start(chn_id) {
    AX_VENC_StartRecvFrame(chn_id, NULL);  // ✅ 在这里启动
    return MediaStatusCode::Ok;
}
```

---

### 问题 4: MediaPipelineManager::Start() 顺序

**当前可能的顺序**:
```cpp
1. 建立链接
2. VIN Enable
3. ISP Start
4. ISP StreamOn
5. IVPS Start (?)
6. VENC Start (?)
```

**应该改成（严格对齐 QSDemo）**:
```cpp
MediaStatusCode MediaPipelineManager::Start() {
    // ========== 阶段1: 建立链接（在 Camera 启动前）==========
    std::fprintf(stderr, "[Pipeline] Establishing links\n");
    for (const auto& link : link_plan_) {
        if (!link_->Link(...)) {
            return MediaStatusCode::InternalError;
        }
    }
    std::fprintf(stderr, "[Pipeline] All links established\n");
    
    // ========== 阶段2: 启动 Camera (VIN + ISP) ==========
    std::fprintf(stderr, "[Pipeline] Starting Camera (VIN + ISP)\n");
    
    // 2.1 VIN Enable (EnableChn → StartPipe → EnableDev)
    if (!vin_->Enable()) {
        std::fprintf(stderr, "[Pipeline] vin_->Enable() failed\n");
        return MediaStatusCode::InternalError;
    }
    std::fprintf(stderr, "[Pipeline] VIN Enable OK\n");
    
    // 2.2 ISP Start
    if (!isp_->Start()) {
        std::fprintf(stderr, "[Pipeline] isp_->Start() failed\n");
        return MediaStatusCode::InternalError;
    }
    
    // 2.3 ISP StreamOn
    if (!isp_->StreamOn()) {
        std::fprintf(stderr, "[Pipeline] isp_->StreamOn() failed\n");
        return MediaStatusCode::InternalError;
    }
    std::fprintf(stderr, "[Pipeline] Camera started successfully\n");
    
    // ========== 阶段3: 启动后端处理 (IVPS + VENC) ==========
    std::fprintf(stderr, "[Pipeline] Starting backend processors\n");
    
    // 3.1 IVPS Start
    if (!ivps_->Start()) {
        std::fprintf(stderr, "[Pipeline] ivps_->Start() failed\n");
        return MediaStatusCode::InternalError;
    }
    
    // 3.2 VENC Start (所有通道)
    for (const auto& ch : ax615_cfg_.video_channels) {
        MediaStatusCode status = venc_->Start(ch.venc_chn_id);
        if (status != MediaStatusCode::Ok) {
            std::fprintf(stderr, "[Pipeline] venc_->Start(%d) failed\n", ch.venc_chn_id);
            return status;
        }
    }
    
    std::fprintf(stderr, "[Pipeline] All modules started successfully\n");
    return MediaStatusCode::Ok;
}
```

---

## 修改步骤（按优先级）

### Step 1: 检查当前代码状态 ⏰ 10分钟

**文件**: 
- `libmedia/Src/adapter/ax615/ivps/ax_ivps_adapter.cpp`
- `libmedia/Src/adapter/ax615/venc/ax_venc_adapter.cpp`

**检查项**:
```bash
# 搜索 AX_IVPS_StartGrp 在哪里调用
grep -n "AX_IVPS_StartGrp" libmedia/Src/adapter/ax615/ivps/*.cpp

# 搜索 AX_VENC_StartRecvFrame 在哪里调用
grep -n "AX_VENC_StartRecvFrame" libmedia/Src/adapter/ax615/venc/*.cpp
```

**判断**:
- 如果在 `Open()` 中调用 → 需要拆分
- 如果在 `Start()` 中调用 → 无需改动

---

### Step 2: 拆分 IVPS Open 和 Start ⏰ 30分钟

**如果 `Open()` 中有 `StartGrp`**，修改：

```cpp
// ax_ivps_adapter.hpp
class AxIvpsAdapter {
public:
    bool Open(const IvpsConfig& cfg);  // 只配置
    bool Start();                       // 启动
    bool IsStarted() const { return started_; }
    
private:
    bool started_ = false;
};

// ax_ivps_adapter.cpp
bool AxIvpsAdapter::Open(const IvpsConfig& cfg) {
    AX_IVPS_Init();
    AX_IVPS_CreateGrp(grp_id_, &grp_attr);
    AX_IVPS_SetPipelineAttr(grp_id_, &pipeline_attr);
    // 配置通道，但不启动
    opened_ = true;
    return true;
}

bool AxIvpsAdapter::Start() {
    if (!opened_ || started_) {
        return opened_;
    }
    AX_S32 ret = AX_IVPS_StartGrp(grp_id_);
    if (ret != AX_SUCCESS) {
        std::fprintf(stderr, "[AxIvpsAdapter] AX_IVPS_StartGrp failed: 0x%x\n", ret);
        return false;
    }
    started_ = true;
    std::fprintf(stderr, "[AxIvpsAdapter] IVPS Group %d started\n", grp_id_);
    return true;
}
```

---

### Step 3: 拆分 VENC Create 和 Start ⏰ 30分钟

**如果 `CreateChannel()` 中有 `StartRecvFrame`**，修改：

```cpp
// ax_venc_adapter.cpp
MediaStatusCode AxVencAdapter::CreateChannel(const VencChannelConfig& cfg) {
    AX_VENC_CHN_ATTR_T chn_attr;
    // ... 配置 chn_attr
    
    AX_S32 ret = AX_VENC_CreateChn(cfg.chn_id, &chn_attr);
    if (ret != AX_SUCCESS) {
        return MediaStatusCode::InternalError;
    }
    
    // 设置 RC、GOP 等参数
    // ...
    
    // ❌ 移除这里的 AX_VENC_StartRecvFrame()
    
    channels_[cfg.chn_id] = cfg;
    return MediaStatusCode::Ok;
}

MediaStatusCode AxVencAdapter::Start(int chn_id) {
    if (channels_.find(chn_id) == channels_.end()) {
        return MediaStatusCode::InvalidParameter;
    }
    
    if (started_channels_.find(chn_id) != started_channels_.end()) {
        return MediaStatusCode::Ok;  // 已经启动
    }
    
    AX_S32 ret = AX_VENC_StartRecvFrame(chn_id, NULL);
    if (ret != AX_SUCCESS) {
        std::fprintf(stderr, "[AxVencAdapter] AX_VENC_StartRecvFrame(%d) failed: 0x%x\n", chn_id, ret);
        return MediaStatusCode::InternalError;
    }
    
    started_channels_.insert(chn_id);
    std::fprintf(stderr, "[AxVencAdapter] VENC channel %d started\n", chn_id);
    return MediaStatusCode::Ok;
}
```

---

### Step 4: 修改 MediaPipelineManager::Start() ⏰ 20分钟

**文件**: `libmedia/Src/pipeline/media_pipeline_manager.cpp`

**修改内容**: 参考上面"问题 4"的完整代码

**关键点**:
1. 链接在最前面建立
2. Camera (VIN+ISP) 完整启动
3. 然后启动 IVPS
4. 最后启动 VENC

---

### Step 5: 验证修改 ⏰ 10分钟

**编译**:
```bash
cd libmedia/Build
make clean && make -j
```

**检查日志顺序**:
```
[Pipeline] Establishing links
[Pipeline] All links established
[Pipeline] Starting Camera (VIN + ISP)
  [VIN] Re-setting channel attributes
  [VIN] Channel 0: EnableChn (after ISP Init)
  [VIN] Calling AX_VIN_StartPipe
  [VIN] AX_VIN_StartPipe OK          ← 应该成功
  [VIN] Calling AX_VIN_EnableDev
  [VIN] Enable completed successfully
[Pipeline] VIN Enable OK
[Pipeline] isp_->Start() called
[Pipeline] isp_->StreamOn() called
[Pipeline] Camera started successfully
[Pipeline] Starting backend processors
  [AxIvpsAdapter] IVPS Group 0 started
  [AxVencAdapter] VENC channel 0 started
[Pipeline] All modules started successfully
```

---

## 预期结果

修改后，`AX_VIN_StartPipe` 应该返回成功（0x0），整个 Pipeline 正常运行。

如果仍然失败，需要进一步检查：
1. ISP Open 是否真的完成（调用了 AX_ISP_Create + AX_ISP_Open）
2. VIN 的 Pipe Frame Source 是否正确设置
3. Channel 属性是否与 QSDemo 完全一致

---

## 参考文档

- [QSDEMO_INIT_SEQUENCE.md](./QSDEMO_INIT_SEQUENCE.md) - 详细时序
- [QSDemo_源码分析文档.md](./QSDemo_源码分析文档.md) - 模块功能
- QSDemo 源码: `A:\AOV_AX615\code_v2\docs\QSDemo\src\com\`

---

**更新时间**: 2026-06-11
**状态**: 待执行
