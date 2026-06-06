# QSDemo 源码分析文档

> 用于回溯参考，分析 QSDemo 各模块逻辑，供 libmedia 实施方案提取使用

---

## 一、整体架构

QSDemo 是爱芯 AX615 平台的快速启动 Demo，核心链路：

```
VIN (摄像头输入)
  → ISP (图像信号处理)
  → IVPS (图像处理/缩放/裁剪)
  → VENC (视频编码 H264/H265/MJPEG)
  → 输出 (RTSP/文件)
```

### 主要源文件
| 文件 | 职责 |
|------|------|
| `qsdemo.h` | 全局结构体定义，SAMPLE_ENTRY_T，管道配置 |
| `com/qs_common_sys.c` | 系统初始化（内存池、LOG） |
| `com/qs_common_vin.c` | VIN 初始化、VI detect、sensor 配置 |
| `com/qs_common_isp.c` | ISP 初始化、线程启动 |
| `com/qs_common_ivps.c` | IVPS 初始化、filter 配置 |
| `com/qs_common_venc.c` | VENC 初始化、编码参数配置 |
| `vin_config.h` | sensor 类型枚举、VIN pipe 配置表 |

---

## 二、核心数据结构

### SAMPLE_ENTRY_T（全局上下文）
```c
// 来自 qsdemo.h
typedef struct {
    AX_U32 nPipeNum;           // pipe 数量
    SAMPLE_PIPE_T pipes[MAX_PIPE_NUM];
    AX_BOOL bEnableRtsp;
    // ...
} SAMPLE_ENTRY_T;

typedef struct {
    AX_U32 nVinPipe;
    AX_U32 nIvpsChn;
    AX_U32 nVencChn;
    AX_U32 nWidth;
    AX_U32 nHeight;
    AX_PAYLOAD_TYPE_E ePayloadType;  // PT_H264 / PT_H265 / PT_MJPEG
    // ...
} SAMPLE_PIPE_T;
```

---

## 三、各模块详细分析

### 3.1 SYS 模块（qs_common_sys.c）

**初始化流程：**
```c
COMMON_SYS_Init(COMMON_SYS_ARGS_T *pArgs)
  → AX_SYS_Init()                    // 系统初始化
  → AX_POOL_SetConfig(poolCfg)       // 配置内存池（CMM）
  → AX_POOL_Init()                   // 创建内存池
```

**关键参数：**
- `nPoolCfgCnt`：内存池数量
- 每个 pool 包含：`nBlkSize`（帧大小）、`nBlkCnt`（帧缓冲数）、`CacheMode`

**反初始化：**
```c
COMMON_SYS_DeInit()
  → AX_POOL_DeInit()
  → AX_SYS_Deinit()
```

---

### 3.2 VIN 模块（qs_common_vin.c）

**这是最复杂的模块，包含 VI detect 逻辑。**

#### 3.2.1 初始化流程
```c
COMMON_VIN_StartVin(COMMON_VIN_ARGS_T *pArgs)
  → COMMON_VIN_SensorClkInit()       // sensor 时钟初始化
  → AX_MIPI_RX_Init()               // MIPI RX 初始化
  → AX_VIN_Init()                   // VIN 模块初始化
  → COMMON_VIN_CreateDev()          // 创建 VIN device
  → COMMON_VIN_OpenSensor()         // 打开 sensor（加载 .so 驱动）
  → COMMON_VIN_CreatePipe()         // 创建 pipe
  → COMMON_VIN_StartPipe()          // 启动 pipe
  → AX_VIN_EnableDev()             // 使能 device
```

#### 3.2.2 VI Detect（动态 sensor 检测）

QSDemo 中 VI detect 的本质：**根据接入的 sensor 类型动态选择配置**

```c
// vin_config.h 中定义 sensor 配置表
typedef struct {
    SAMPLE_SNS_TYPE_E eSnsType;     // sensor 类型枚举
    AX_SNS_ATTR_T tSnsAttr;         // 分辨率、帧率、HDR mode
    AX_VIN_DEV_ATTR_T tDevAttr;     // VIN device 属性
    AX_VIN_PIPE_ATTR_T tPipeAttr;   // VIN pipe 属性
    AX_VIN_CHN_ATTR_T tChnAttr[3]; // 3个通道属性
    AX_MIPI_RX_ATTR_T tMipiAttr;    // MIPI 参数（lane数、速率）
} COMMON_VIN_SENSOR_CONFIG_T;
```

**detect 流程（简化）：**
```c
// 1. 枚举所有支持的 sensor 类型
for each eSnsType in supportedSensorList:
    // 2. 尝试打开对应的 sensor .so
    handle = dlopen(snsLibPath, RTLD_LAZY)
    
    // 3. 调用 sensor probe 接口
    pfnSnsProbe = dlsym(handle, "AX_sns_probe")
    ret = pfnSnsProbe(devId)
    
    // 4. probe 成功则使用该配置
    if ret == 0:
        gSnsType = eSnsType
        break
```

**实际使用的 sensor（AX615 场景）：**
- SC200AI（主摄，2MP，MIPI 2-lane）
- SC500AI（可选，5MP）
- OS04A10（可选，4MP，HDR）

#### 3.2.3 Sensor 驱动加载
```c
// sensor .so 路径规则
snprintf(libPath, sizeof(libPath), 
    "/opt/lib/sns/libsns_%s.so", snsName);

// 获取操作函数表
AX_SENSOR_REGISTER_FUNC_T *pfnSns = dlsym(handle, "gSnsFuncs");
```

#### 3.2.4 关键 AX API
```c
AX_MIPI_RX_SetAttr(devId, &tMipiAttr)  // 设置 MIPI 参数
AX_VIN_CreateDev(devId, &tDevAttr)      // 创建 VIN device
AX_VIN_SetDevAttr(devId, &tDevAttr)
AX_VIN_EnableDev(devId)
AX_VIN_CreatePipe(pipeId, &tPipeAttr)  // 创建 pipe
AX_VIN_SetPipeAttr(pipeId, &tPipeAttr)
AX_VIN_SetChnAttr(pipeId, chnId, &tChnAttr)  // 设置通道（输出格式/分辨率）
AX_VIN_EnablePipe(pipeId)
AX_VIN_EnableChn(pipeId, chnId)
```

---

### 3.3 ISP 模块（qs_common_isp.c）

**ISP 运行在独立线程中。**

#### 初始化流程
```c
COMMON_ISP_Init(AX_U32 pipe, COMMON_ISP_ARGS_T *pArgs)
  → AX_ISP_Init()                    // ISP 初始化
  → AX_ISP_Open(pipe)               // 打开 ISP pipe
  → AX_ISP_SetModuleStatus()        // 使能各子模块（AE/AWB/AF等）
  → AX_ISP_RegisterAlgo(pipe, ...)  // 注册 3A 算法库
  → AX_ISP_LoadBinParams(pipe, binPath)  // 加载 IQ tuning 参数
```

#### ISP 线程
```c
void *ISP_Run_Thread(void *arg) {
    while (!bExit) {
        AX_ISP_Run(pipeId);  // 驱动 ISP 帧处理（阻塞直到一帧完成）
    }
}
// 线程需要在 VIN enable 之后启动
```

#### 关键配置
- **IQ bin 路径**：`/opt/etc/isp_params/{snsName}/isp_param.bin`
- **3A 算法库**：`/opt/lib/3a/libax_ae.so`, `libax_awb.so`
- **ISP pipe** 与 **VIN pipe** 一一对应

---

### 3.4 IVPS 模块（qs_common_ivps.c）

**IVPS = Image Video Processing Subsystem，负责缩放、裁剪、格式转换。**

#### 初始化流程
```c
COMMON_IVPS_Init(COMMON_IVPS_ARGS_T *pArgs)
  → AX_IVPS_Init()                          // 模块初始化
  → AX_IVPS_CreateGrp(grpId, &tGrpAttr)    // 创建处理组
  → AX_IVPS_SetPipelineAttr(grpId, &tPipeline)  // 设置 pipeline（filter链）
  → AX_IVPS_EnableChn(grpId, chnId)        // 使能输出通道
  → AX_IVPS_StartGrp(grpId)               // 启动
```

#### Pipeline 配置（Filter 链）
```c
AX_IVPS_PIPELINE_ATTR_T tPipeline;
tPipeline.nOutChnNum = 2;  // 支持多路输出

// Filter 0: 输入 filter（resize/crop）
tPipeline.tFilter[0][0].bEngage = AX_TRUE;
tPipeline.tFilter[0][0].eEngine = AX_IVPS_ENGINE_TDP;  // TDP/VPP/VGP
tPipeline.tFilter[0][0].tFRC.fSrcFrameRate = 30;
tPipeline.tFilter[0][0].tFRC.fDstFrameRate = 30;

// Filter 1: 通道0输出 filter（指定输出分辨率）
tPipeline.tFilter[1][0].bEngage = AX_TRUE;
tPipeline.tFilter[1][0].nDstPicWidth = 1920;
tPipeline.tFilter[1][0].nDstPicHeight = 1080;
tPipeline.tFilter[1][0].eDstPicFormat = AX_FORMAT_YUV420_SEMIPLANAR;

// Filter 2: 通道1输出 filter（第二路）
tPipeline.tFilter[2][0].bEngage = AX_TRUE;
tPipeline.tFilter[2][0].nDstPicWidth = 720;
tPipeline.tFilter[2][0].nDstPicHeight = 576;
```

#### 数据链路绑定（IVPS 输入源）
```c
// 将 VIN 输出绑定到 IVPS 输入
AX_MOD_INFO_T srcMod = {AX_ID_VIN, pipeId, chnId};
AX_MOD_INFO_T dstMod = {AX_ID_IVPS, grpId, 0};
AX_SYS_Link(&srcMod, &dstMod);  // 硬件链路，零拷贝
```

---

### 3.5 VENC 模块（qs_common_venc.c）

#### 初始化流程
```c
COMMON_VENC_Init(COMMON_VENC_ARGS_T *pArgs)
  → AX_VENC_Init(&tVencModAttr)         // 模块初始化
  → AX_VENC_CreateChn(chnId, &tChnAttr) // 创建编码通道
  → AX_VENC_StartRecvFrame(chnId, NULL) // 开始接收帧
```

#### 编码通道配置
```c
AX_VENC_CHN_ATTR_T tChnAttr = {0};

// 图像属性
tChnAttr.stVencAttr.u32PicWidth = 1920;
tChnAttr.stVencAttr.u32PicHeight = 1080;
tChnAttr.stVencAttr.enType = PT_H264;  // PT_H265 / PT_MJPEG

// H264 参数
tChnAttr.stRcAttr.enRcMode = AX_VENC_RC_MODE_H264CBR;
tChnAttr.stRcAttr.stH264Cbr.u32BitRate = 4096;  // kbps
tChnAttr.stRcAttr.stH264Cbr.u32Gop = 60;

// 输入图像格式
tChnAttr.stVencAttr.enMemSource = AX_MEMORY_SOURCE_CMM;
```

#### 码流获取线程
```c
void *VENC_GetStream_Thread(void *arg) {
    AX_VENC_STREAM_T tStream = {0};
    while (!bExit) {
        ret = AX_VENC_GetStream(chnId, &tStream, -1);  // 阻塞等待
        if (ret == 0) {
            // 处理码流数据
            processStream(&tStream);
            AX_VENC_ReleaseStream(chnId, &tStream);    // 必须释放
        }
    }
}
```

#### IVPS → VENC 绑定
```c
AX_MOD_INFO_T srcMod = {AX_ID_IVPS, grpId, chnId};
AX_MOD_INFO_T dstMod = {AX_ID_VENC, 0, vencChnId};
AX_SYS_Link(&srcMod, &dstMod);
```

---

## 四、完整初始化顺序

```
1. AX_SYS_Init() + AX_POOL_Init()          ← SYS
2. AX_MIPI_RX_Init() + AX_VIN_Init()       ← VIN 前置
3. VIN detect sensor（probe .so）
4. AX_VIN_CreateDev/Pipe/Chn()             ← VIN 配置
5. AX_ISP_Init() + AX_ISP_Open()           ← ISP
6. AX_ISP_LoadBinParams()                   ← ISP IQ
7. AX_IVPS_Init() + CreateGrp/Pipeline     ← IVPS
8. AX_VENC_Init() + CreateChn()            ← VENC
9. AX_SYS_Link (VIN→IVPS, IVPS→VENC)      ← 绑定链路
10. AX_VIN_EnableDev/Pipe/Chn()            ← 启动 VIN
11. ISP_Run_Thread 启动                     ← ISP 线程
12. VENC_GetStream_Thread 启动             ← 取流线程
```

---

## 五、反初始化顺序（逆序）

```
1. 停止取流线程
2. 停止 ISP 线程
3. AX_VIN_DisableDev/Pipe/Chn()
4. AX_SYS_UnLink (解除所有绑定)
5. AX_VENC_StopRecvFrame() + DestroyChn()
6. AX_IVPS_StopGrp() + DestroyGrp()
7. AX_ISP_Close() + DeInit()
8. AX_VIN_DestroyDev/Pipe()
9. AX_POOL_DeInit() + AX_SYS_Deinit()
```

---

## 六、关键常量与限制

| 参数 | 值 | 说明 |
|------|-----|------|
| MAX_PIPE_NUM | 4 | 最大 VIN pipe 数 |
| MAX_CHN_NUM | 3 | 每个 pipe 最大通道数 |
| IVPS_GRP_NUM | 最多8组 | AX615 硬件限制 |
| VENC_CHN_NUM | 最多16路 | AX615 硬件限制 |
| 内存池 Block | 按帧大小计算 | W*H*1.5 (NV12) |
