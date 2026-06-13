# QSDemo 完整初始化流程分析

## 配置参数（run.sh -x 3）

### 关键参数
```bash
case_id=3                    # OS04D10 单路传感器
aiisp_mode=0                 # TISP 模式
gdc_enable=1                 # 启用 GDC
link_mode=3,1                # VIN->IVPS: OFFLINE, IVPS->VENC: LOW_MEM
chn_frame_mode=1             # Ring buffer 模式
sleep_time=3000              # AOV 周期 3 秒
gop_in_aov=10               # AOV 模式下 GOP=10
sns_ctrl_mode=3              # Sensor 控制：reg + gpio
bw_limit_level=0,4,0,0      # 带宽限制：Normal(0,4), AOV(0,0)
```

### 传感器配置（OS04D10）
```
分辨率: 2560x1440
帧率: 30fps (normal), 1fps (AOV)
接口: MIPI 2-Lane, 720Mbps
工作模式: AX_VIN_DEV_ONLINE
Pipe模式: AX_VIN_PIPE_NORMAL_MODE1
```

---

## QSDemo 初始化流程（完整序列）

### 阶段 0: 系统准备
**位置**: `qsdemo.c` main 函数开始

```c
// 1. 信号处理
signal(SIGPIPE, SIG_IGN);
signal(SIGINT, SigInt);
signal(SIGTSTP, SigStop);

// 2. 参数解析
// -x 3 → case_id = QS_CASE_OS04D10_SINGLE
// -g 3,1 → VIN->IVPS: OFFLINE, IVPS->VENC: LOW_MEM
// -k 3000 → AOV 周期 3 秒
```

---

### 阶段 1: 系统层初始化（SYS + POOL）
**位置**: `qsdemo.c` 调用 `QS_COMMON_SYS_Init()`
**对应文件**: `qs_common_sys.c`

```c
// 1.1 AX_SYS_Init
AX_SYS_Init();

// 1.2 AX_POOL_Exit (清理之前的 pool)
AX_POOL_Exit();

// 1.3 配置 Common Pool
// 根据 sensor 配置计算 pool 大小
AX_POOL_SetConfig(&tCommPoolConfig);

// 1.4 AX_POOL_Init
AX_POOL_Init();

// 返回状态: SYS + POOL 已就绪
```

**关键点**:
- 必须先 `AX_POOL_Exit()` 再 `SetConfig`，否则会冲突
- Pool 配置依赖 sensor 的分辨率和格式

---

### 阶段 2: VIN 初始化（设备和管道创建）
**位置**: `qs_common_cam.c` → `QS_COMMON_VIN_Create()`
**时机**: 在 `QS_COMMON_CAM_Open()` 内部，ISP Init **之前**

```c
// 2.1 AX_VIN_Init
AX_VIN_Init();

// 2.2 AX_MIPI_RX_Init
AX_MIPI_RX_Init();

// 2.3 配置并启动 MIPI
AX_MIPI_RX_SetAttr(rx_dev, &mipi_attr);
AX_MIPI_RX_Start(rx_dev);

// 2.4 创建 VIN Device
AX_VIN_CreateDev(dev_id, &dev_attr);
AX_VIN_SetDevAttr(dev_id, &dev_attr);

// 2.5 绑定 Device 到 Pipe
AX_VIN_SetDevBindPipe(dev_id, &bind_pipe);

// 2.6 绑定 Device 到 MIPI
AX_VIN_SetDevBindMipi(dev_id, rx_dev);

// 2.7 创建 Pipe
AX_VIN_CreatePipe(pipe_id, &pipe_attr);
AX_VIN_SetPipeAttr(pipe_id, &pipe_attr);

// 2.8 设置 Pipe Frame Source (NORMAL_MODE1 必须)
AX_VIN_SetPipeFrameSource(pipe_id, AX_VIN_FRAME_SOURCE_ID_IFE, AX_VIN_FRAME_SOURCE_TYPE_DEV);

// ⚠️ 注意: 此时 Channel 还没有配置和使能！
```

**关键点**:
- VIN Create 只做**设备和管道的创建**，不涉及 Channel 使能
- `SetPipeFrameSource` 是 NORMAL_MODE1 的必须步骤
- MIPI 必须先于 VIN Device 启动

---

### 阶段 3: ISP 初始化（Create + Register + Open）
**位置**: `qs_common_cam.c` → `QS_COMMON_ISP_Init()`
**时机**: VIN Create **之后**，VIN StartChn **之前**

```c
// 3.1 打开 Sensor 时钟
AX_ISP_OpenSnsClk(clk_idx, clk_rate);  // 24MHz

// 3.2 创建 ISP Pipe
AX_ISP_Create(pipe_id);

// 3.3 注册 AutoKit 库
AX_ISP_ALG_RegisterAutoKitLib(pipe_id, &stAutoKitLib);

// 3.4 注册 Sensor
AX_ISP_RegisterSensor(pipe_id, &sensor_handle);

// 3.5 注册 3A 算法库
// AE (自动曝光)
AX_ISP_ALG_RegAeLibCallback(pipe_id, &ae_funcs);
// AWB (自动白平衡)
AX_ISP_ALG_RegAwbLibCallback(pipe_id, &awb_funcs);

// 3.6 加载 IQ Bin 文件
AX_ISP_LoadBinParams(pipe_id, "/opt/etc/os04d10_sdr.bin");

// 3.7 打开 ISP
AX_ISP_Open(pipe_id);

// 3.8 设置带宽限制（如果需要）
if (bw_limit_level[0] != -1) {
    AX_AIISP_SetBwLimitLevel(pipe_id, &bw_limit);
}

// 返回状态: ISP 已 Create + Open，但尚未 Start
```

**关键点**:
- **ISP Open 必须在 VIN StartChn 之前完成**
- ISP 此时只是"准备就绪"，还没有真正运行
- IQ Bin 文件路径要正确：`/opt/etc/os04d10_sdr.bin`

---

### 阶段 4: VIN Channel 配置和使能
**位置**: `qs_common_cam.c` → `QS_COMMON_VIN_StartChn()`
**时机**: ISP Open **之后**，VIN StartPipe **之前**

```c
// 4.1 遍历所有 Channel（0-3）
for (chn = 0; chn < AX_VIN_CHN_ID_MAX; chn++) {
    // 4.2 设置 Channel 属性
    AX_VIN_SetChnAttr(pipe_id, chn, &chn_attr[chn]);
    
    // 4.3 如果 Channel 启用
    if (chn_enabled[chn] == AX_TRUE) {
        // 4.4 设置 Frame Mode (Ring/Off)
        AX_VIN_SetChnFrameMode(pipe_id, chn, chn_frame_mode);
        
        // 4.5 使能 Channel
        AX_VIN_EnableChn(pipe_id, chn);
    }
}

// 返回状态: VIN Channel 已配置并使能
```

**关键点**:
- **所有 4 个 Channel 都要调用 SetChnAttr**（即使不启用）
- **EnableChn 必须在 ISP Open 之后**
- 只有启用的 Channel 才调用 EnableChn

---

### 阶段 5: VIN StartPipe
**位置**: `qs_common_cam.c` → `__common_cam_open()` 行315
**时机**: VIN StartChn **之后**，ISP Start **之前**

```c
// 5.1 启动 VIN Pipe
AX_VIN_StartPipe(pipe_id);

// 返回状态: VIN Pipe 已启动，等待 ISP 启动
```

**关键点**:
- **VIN StartPipe 必须在 ISP Start 之前**
- 此时 VIN 已经可以接收数据，但还没有送给 ISP

---

### 阶段 6: ISP Start
**位置**: `qs_common_cam.c` → `__common_cam_open()` 行322
**时机**: VIN StartPipe **之后**，VIN EnableDev **之前**

```c
// 6.1 启动 ISP
AX_ISP_Start(pipe_id);

// 返回状态: ISP 开始运行 3A 算法
```

**关键点**:
- **ISP Start 必须在 VIN StartPipe 之后**
- ISP 此时开始处理图像数据

---

### 阶段 7: VIN EnableDev
**位置**: `qs_common_cam.c` → `QS_COMMON_VIN_StartDev()` 行339
**时机**: ISP Start **之后**，ISP StreamOn **之前**

```c
// 7.1 如果是 OFFLINE 模式，设置 Dump 属性
if (dev_attr.eDevMode == AX_VIN_DEV_OFFLINE) {
    AX_VIN_DUMP_ATTR_T dump_attr;
    dump_attr.bEnable = AX_TRUE;
    dump_attr.nDepth = 3;
    AX_VIN_SetDevDumpAttr(dev_id, AX_VIN_DUMP_QUEUE_TYPE_DEV, &dump_attr);
}

// 7.2 使能 VIN Device
AX_VIN_EnableDev(dev_id);

// 返回状态: VIN Device 开始采集数据
```

---

### 阶段 8: ISP StreamOn
**位置**: `qs_common_cam.c` → `__common_cam_open()` 行348
**时机**: VIN EnableDev **之后**

```c
// 8.1 开启 ISP 数据流
AX_ISP_StreamOn(pipe_id);

// 返回状态: 整个 Camera (VIN+ISP) 已完全启动
```

---

### 阶段 9: IVPS 初始化（后台线程）
**位置**: `qsdemo.c` → `InitVinPostProc()` → `SampleIvpsInit()`
**时机**: 在后台线程中，与 Camera 启动并行

```c
// 9.1 IVPS Init
AX_IVPS_Init();

// 9.2 创建 IVPS Group
AX_IVPS_CreateGrp(ivps_grp, &grp_attr);

// 9.3 设置 Pipeline 属性
AX_IVPS_SetPipelineAttr(ivps_grp, &pipeline_attr);

// 9.4 配置输出 Channel
// Channel 0: 1920x1080 @ 15fps
// (如果有其他 Channel，继续配置)

// 返回状态: IVPS Group 已创建并配置，但尚未启动
```

**关键点**:
- IVPS 在后台线程初始化
- 此时只是配置，还没有 Start

---

### 阶段 10: VENC 初始化（后台线程）
**位置**: `qsdemo.c` → `InitVinPostProc()` → `SampleVencInit()`
**时机**: 在后台线程中，在 IVPS Init 之前

```c
// 10.1 VENC 模块初始化
AX_VENC_MOD_ATTR_T mod_attr;
mod_attr.enVencType = AX_VENC_MULTI_ENCODER;
mod_attr.stModThdAttr.u32TotalThreadNum = 3;
AX_VENC_Init(&mod_attr);

// 10.2 创建 VENC Channel
// Channel 0: H.265, 1920x1080, 15fps, CBR
AX_VENC_CreateChn(chn_id, &chn_attr);

// 10.3 设置 RC 参数
AX_VENC_SetRcParam(chn_id, &rc_param);

// 10.4 设置 GOP 模式
AX_VENC_SetGopMode(chn_id, &gop_mode);

// 返回状态: VENC Channel 已创建，但尚未启动
```

---

### 阶段 11: 建立链接（后台线程）
**位置**: `qsdemo.c` → `InitVinPostProc()` → `SampleLinkInit()`
**时机**: 在 VENC Init 和 IVPS Init **之后**，Camera Open **之前**

```c
// 11.1 配置链接属性
AX_MOD_LINK_ATTR_T link_attr;

// 11.2 VIN -> IVPS 链接（OFFLINE 模式）
link_attr.eDataFlowMode = AX_MOD_DATA_FLOW_SW_OFFLINE;
link_attr.eSrcEngine = AX_ENG_NONE;
link_attr.eDstEngine = AX_ENG_NONE;
link_attr.nRingLineCnt = 0;

AX_MOD_INFO_T src = {AX_ID_VIN, 0, 0};  // VIN grp=0, chn=0
AX_MOD_INFO_T dst = {AX_ID_IVPS, 0, 0}; // IVPS grp=0, chn=0
AX_SYS_Link_V2(&src, &dst, &link_attr);

// 11.3 IVPS -> VENC 链接（LOW_MEM 模式）
link_attr.eDataFlowMode = AX_MOD_DATA_FLOW_SW_LOW_MEM;
link_attr.eSrcEngine = AX_ENG_VPP;
link_attr.eDstEngine = AX_ENG_VENC;
link_attr.nRingLineCnt = 128;

src = {AX_ID_IVPS, 0, 0};  // IVPS grp=0, chn=0
dst = {AX_ID_VENC, 0, 0};  // VENC grp=0, chn=0
AX_SYS_Link_V2(&src, &dst, &link_attr);

// 返回状态: 数据链路已建立
```

**关键点**:
- **链接在所有模块 Init 之后、Camera 启动之前建立**
- VIN->IVPS 使用 OFFLINE 模式（link_mode 第一个参数 = 3）
- IVPS->VENC 使用 LOW_MEM 模式（link_mode 第二个参数 = 1）

---

### 阶段 12: Camera 启动（主线程）
**位置**: `qsdemo.c` → 主流程调用 `QS_COMMON_CAM_Open()`
**时机**: 后台线程完成 IVPS/VENC/Link 初始化之后

```c
// 12.1 等待后台线程完成（如果使用了 AX_FAST_BOOT）
#ifdef AX_FAST_BOOT
pthread_mutex_lock(&g_mtxIvpsInit);
while (!g_bIvpsInited) {
    pthread_cond_wait(&g_condIvpsInit, &g_mtxIvpsInit);
}
pthread_mutex_unlock(&g_mtxIvpsInit);
#endif

// 12.2 调用 QS_COMMON_CAM_Open
// 这里会执行前面的 阶段2-8
QS_COMMON_CAM_Open(&gCams[0], cam_count);

// 返回状态: Camera 已完全启动
```

---

### 阶段 13: 启动数据流（主线程）
**位置**: `qsdemo.c` → 主流程
**时机**: Camera Open **之后**

```c
// 13.1 启动 IVPS
AX_IVPS_StartGrp(ivps_grp);

// 13.2 启动 VENC
AX_VENC_StartRecvFrame(venc_chn_id);

// 13.3 创建 GetStream 线程接收编码数据
pthread_create(&tid, NULL, VencGetStreamProc, ...);

// 返回状态: 整个 Pipeline 开始工作
```

---

## 完整时序图

```
主线程                          后台线程(InitVinPostProc)
  |                                  |
  |--[1] SYS + POOL Init             |
  |                                  |
  |--[启动后台线程]----------------→ |
  |                                  |
  |                                  |--[10] VENC Init (CreateChn)
  |                                  |
  |                                  |--[9] IVPS Init (CreateGrp)
  |                                  |
  |                                  |--[11] Link Init (建立链接)
  |                                  |
  |                                  |--[通知主线程完成]
  |                                  |
  |←-[等待后台完成]------------------|
  |                                  |
  |--[2] VIN Create (Device+Pipe)   |
  |--[3] ISP Init (Create+Open)     |
  |--[4] VIN StartChn (EnableChn)   |
  |--[5] VIN StartPipe               |
  |--[6] ISP Start                   |
  |--[7] VIN EnableDev               |
  |--[8] ISP StreamOn                |
  |                                  |
  |--[13] IVPS Start                 |
  |--[13] VENC Start                 |
  |--[13] 创建 GetStream 线程        |
  |                                  |
  ▼ 整个 Pipeline 运行中              ▼
```

---

## 关键依赖关系

### 严格顺序依赖（必须遵守）

```
1. AX_POOL_Exit → AX_POOL_SetConfig → AX_POOL_Init
   原因: Pool 配置必须在 Init 前设置

2. VIN Create → ISP Create/Open → VIN EnableChn
   原因: VIN Channel 使能需要 ISP 已经就绪

3. VIN EnableChn → VIN StartPipe → ISP Start
   原因: SDK 内部状态机要求

4. ISP Start → VIN EnableDev → ISP StreamOn
   原因: 数据流启动顺序
```

### 并行初始化（可以并发）

```
主线程: SYS/POOL Init
后台线程: VENC Init, IVPS Init, Link Init

但是: Link 建立必须在 Camera Open 之前
```

---

## libmedia 需要修改的关键点

### 问题 1: EnableChn 时机错误
**当前**: VIN Open 时就 EnableChn（在 ISP Init 之前）
**应该**: ISP Open 之后，VIN StartPipe 之前

### 问题 2: ISP Start 时机错误
**当前**: VIN Enable 之前就 ISP Start
**应该**: VIN StartPipe 之后，VIN EnableDev 之前

### 问题 3: 链接建立时机可能错误
**当前**: 在 IVPS/VENC Open 之后立即建立
**应该**: 在所有模块 Init/Create 完成后、Camera 启动前建立

### 问题 4: 启动顺序错误
**当前**: ISP Start → VIN Enable
**应该**: VIN Enable(包含 EnableChn+StartPipe+EnableDev) → ISP Start → ISP StreamOn

---

## 修改建议（基于当前架构）

### Step 1: 修改 VIN Adapter

```cpp
// CreatePipeAndChannels() - 只创建，不使能
bool AxVinAdapter::CreatePipeAndChannels(const SensorProfile& profile) {
    // 创建 Pipe
    AX_VIN_CreatePipe(...);
    AX_VIN_SetPipeAttr(...);
    AX_VIN_SetPipeFrameSource(...);
    
    // 配置 Channel 属性，但不 EnableChn
    for (channel) {
        AX_VIN_SetChnAttr(...);
        AX_VIN_SetChnFrameMode(...);
        // ❌ 不调用 AX_VIN_EnableChn
        enabled_channels_.push_back(chn);  // 只记录
    }
}

// Enable() - 使能并启动
bool AxVinAdapter::Enable() {
    // 重新设置属性（QSDemo 也这样做）
    for (channel) {
        AX_VIN_SetChnAttr(...);
        AX_VIN_SetChnFrameMode(...);
        AX_VIN_EnableChn(...);  // ✅ 现在才使能
    }
    
    // 启动 Pipe
    AX_VIN_StartPipe(...);
    
    // 使能 Device
    if (OFFLINE mode) {
        AX_VIN_SetDevDumpAttr(...);
    }
    AX_VIN_EnableDev(...);
}
```

### Step 2: 修改 MediaPipelineManager::Start()

```cpp
MediaStatusCode MediaPipelineManager::Start() {
    // ========== 阶段1: 配置（不启动）==========
    // VIN/ISP 已经在 Open 时配置好
    // IVPS CreateGrp (不 Start)
    // VENC CreateChannel (不 Start)
    
    // ========== 阶段2: 建立链接 ==========
    for (link : link_plan_) {
        link_->Link(...);
    }
    
    // ========== 阶段3: 启动 Camera ==========
    // 3.1 VIN Enable (EnableChn → StartPipe → EnableDev)
    vin_->Enable();
    
    // 3.2 ISP Start
    isp_->Start();
    
    // 3.3 ISP StreamOn
    isp_->StreamOn();
    
    // ========== 阶段4: 启动后端 ==========
    // 4.1 IVPS Start
    ivps_->Start();
    
    // 4.2 VENC Start
    for (channel) {
        venc_->Start(channel);
    }
    
    return MediaStatusCode::Ok;
}
```

---

## 验证清单

启动 libmedia_demo 后，日志应该显示：

```
[Pipeline] ISP Open OK                        ← ISP 已就绪
[Pipeline] All links established              ← 链接已建立
[Pipeline] Starting VIN                       ← 开始启动 VIN
  [VIN] Re-setting channel attributes         ← 重新设置属性
  [VIN] Channel 0: EnableChn                  ← 现在才使能（在 ISP Open 后）
  [VIN] Calling AX_VIN_StartPipe              
  [VIN] AX_VIN_StartPipe OK                   ← ✅ 应该成功
  [VIN] Calling AX_VIN_EnableDev
  [VIN] Enable completed successfully
[Pipeline] VIN Enable OK
[Pipeline] Starting ISP                       ← ISP Start（在 VIN StartPipe 后）
[Pipeline] ISP StreamOn
[Pipeline] Starting IVPS
[Pipeline] Starting VENC
```

---

## 参考文件清单

| 文件 | 关键函数 | 说明 |
|-----|---------|------|
| `qsdemo.c` | `main()` | 主流程，参数解析 |
| `qsdemo.c` | `InitVinPostProc()` | 后台线程初始化 |
| `qs_common_sys.c` | `QS_COMMON_SYS_Init()` | SYS + POOL 初始化 |
| `qs_common_vin.c` | `QS_COMMON_VIN_Create()` | VIN Device/Pipe 创建 |
| `qs_common_vin.c` | `QS_COMMON_VIN_StartChn()` | VIN Channel 使能 |
| `qs_common_vin.c` | `QS_COMMON_VIN_StartDev()` | VIN Device 使能 |
| `qs_common_isp.c` | `QS_COMMON_ISP_Init()` | ISP 初始化 |
| `qs_common_cam.c` | `QS_COMMON_CAM_Open()` | Camera 启动（组合 VIN+ISP）|
| `qsdemo.c` | `SampleLinkInit()` | 链接建立 |
| `qsdemo.c` | `SampleIvpsInit()` | IVPS 初始化 |
| `qsdemo.c` | `SampleVencInit()` | VENC 初始化 |

---

**生成日期**: 2026-06-11  
**适用版本**: AX615 SDK V2.0.0 P5, QSDemo OS04D10 单路配置
