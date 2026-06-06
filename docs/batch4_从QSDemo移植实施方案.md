# Batch4: 从QSDemo移植实施方案（修订版）

> 基于QSDemo源码分析，将核心逻辑移植到code_v2现有架构中。
> 参考文档：`docs/QSDemo_源码分析文档.md`

---

## 一、架构原则

### 依赖倒置 + 适配器模式

业务逻辑只依赖**抽象接口**，SDK实现被隔离在 `adapter/芯片型号/` 目录下。
未来换主控（如ax620）只需新增 `adapter/ax620/` 目录，业务层零改动。

```
libmedia/
├── interface/               ← 纯抽象接口（不含任何SDK头文件）
│   ├── i_sys_adapter.hpp    ← ISysAdapter
│   ├── i_vin_adapter.hpp    ← IVinAdapter
│   ├── i_isp_adapter.hpp    ← IIspAdapter
│   ├── i_ivps_adapter.hpp   ← IIvpsAdapter
│   ├── i_venc_adapter.hpp   ← IVencAdapter
│   ├── i_audio_adapter.hpp  ← IAudioAdapter
│   └── i_rtsp_adapter.hpp   ← IRtspAdapter
│
├── adapter/
│   ├── ax615/               ← AX615 SDK具体实现
│   │   ├── sys/ax_sys_adapter.hpp/cpp
│   │   ├── vin/ax_vin_adapter.hpp/cpp
│   │   ├── isp/ax_isp_adapter.hpp/cpp
│   │   ├── ivps/ax_ivps_adapter.hpp/cpp
│   │   ├── venc/ax_venc_adapter.hpp/cpp
│   │   ├── audio/ax_audio_adapter.hpp/cpp
│   │   └── rtsp/ax_rtsp_adapter.hpp/cpp
│   └── ax620/               ← 未来新增，零改动业务层
│       └── ...
│
├── pipeline/                ← 业务编排，只依赖interface，不include AX SDK
│   ├── i_media_pipeline.hpp
│   └── ax_media_pipeline.hpp/cpp   ← 工厂+装配，持有adapter实例
│
├── common/
│   └── media_types.hpp      ← 公共POD类型，无SDK依赖
│
└── CMakeLists.txt
```

### 关键约束

- `interface/` 目录下所有文件：**禁止 include 任何 AX SDK 头文件**
- `pipeline/` 目录：**禁止 include 任何 AX SDK 头文件**（通过接口指针操作）
- `adapter/ax615/` 目录：可以自由 include AX SDK，SDK细节全部封装在此
- `common/media_types.hpp`：只包含POD结构体和枚举，无外部依赖

---

## 二、公共类型（common/media_types.hpp）

定义跨模块共享的POD类型，所有接口均使用这些类型，**不暴露AX SDK类型**：

```cpp
// 分辨率
struct Resolution { int width; int height; };

// 帧率模式
enum class FrameRateMode { Fixed, Interval };
struct FrameRateConfig {
    FrameRateMode mode;
    int fps;              // Fixed模式使用
    int interval_sec;     // Interval模式使用（低功耗间隔录像）
};

// 编码类型
enum class CodecType { H264, H265, MJPEG };

// 码率控制
enum class RcMode { CBR, VBR, FIXQP };

// 码流帧
struct StreamFrame {
    uint8_t* data;
    size_t   size;
    uint64_t pts_ms;
    bool     is_key_frame;
};

// VIN通道配置
struct VinChannelConfig {
    int        pipe_id;
    Resolution sensor_res;
    int        fps;
    std::string sensor_type;   // "os04a10", "sc450ai" 等
    std::string iq_bin_path;
};

// IVPS通道配置
struct IvpsChannelConfig {
    int        grp_id;
    int        chn_id;
    Resolution out_res;
    int        fps;
};

// VENC通道配置
struct VencChannelConfig {
    int        chn_id;
    CodecType  codec;
    RcMode     rc_mode;
    Resolution res;
    int        fps;
    int        bitrate_kbps;  // CBR/VBR使用
    int        qp;            // FIXQP使用
};

// CMPool块配置
struct CmPoolBlockConfig {
    uint64_t blk_size;
    uint32_t blk_cnt;
    uint64_t meta_size = 512;
    bool     cached    = false;
};

// 检测结果
struct DetectObject {
    enum class Type { Human, Pet, Vehicle, Unknown };
    Type  type;
    float confidence;
    int   x, y, w, h;
};
struct DetectResultSummary {
    bool  has_human;
    bool  has_pet;
    bool  has_vehicle;
    int   total_count;
    std::vector<DetectObject> objects;
};
```

---

## 三、抽象接口层（interface/）

所有接口继承自公共基类，不含任何SDK类型。

### 3.1 ISysAdapter

```cpp
class ISysAdapter {
public:
    virtual ~ISysAdapter() = default;
    virtual bool Init(const std::vector<CmPoolBlockConfig>& pool_cfg) = 0;
    virtual void Deinit() = 0;
    virtual bool SetLogLevel(int level) = 0;
};
```

### 3.2 IVinAdapter

```cpp
class IVinAdapter {
public:
    virtual ~IVinAdapter() = default;
    virtual bool Open(const VinChannelConfig& cfg) = 0;
    virtual void Close() = 0;
    virtual bool Start() = 0;
    virtual void Stop() = 0;
};
```

### 3.3 IIspAdapter

```cpp
class IIspAdapter {
public:
    virtual ~IIspAdapter() = default;
    virtual bool Open(int pipe_id, const std::string& iq_bin_path) = 0;
    virtual void Close() = 0;
    virtual bool Start() = 0;
    virtual void Stop() = 0;
    virtual bool SetScene(const std::string& scene) = 0;
};
```

### 3.4 IIvpsAdapter

```cpp
class IIvpsAdapter {
public:
    virtual ~IIvpsAdapter() = default;
    virtual bool Open(const IvpsChannelConfig& cfg) = 0;
    virtual void Close() = 0;
    virtual bool Start() = 0;
    virtual void Stop() = 0;
};
```

### 3.5 IVencAdapter

```cpp
using FrameCallback = std::function<void(const StreamFrame&)>;

class IVencAdapter {
public:
    virtual ~IVencAdapter() = default;
    virtual bool Open(const VencChannelConfig& cfg) = 0;
    virtual void Close() = 0;
    virtual bool Start() = 0;
    virtual void Stop() = 0;
    virtual bool SetFrameCallback(FrameCallback cb) = 0;
    virtual bool RequestIdr() = 0;
    virtual bool ApplyFrameRate(const FrameRateConfig& fr) = 0;
};
```

### 3.6 IRtspAdapter

```cpp
class IRtspAdapter {
public:
    virtual ~IRtspAdapter() = default;
    virtual bool Start(int port) = 0;
    virtual void Stop() = 0;
    virtual bool AddStream(const std::string& path, int venc_chn) = 0;
    virtual void RemoveStream(const std::string& path) = 0;
};
```

---

## 四、AX615适配器实现（adapter/ax615/）

各适配器继承对应接口，封装AX SDK调用。

### 4.1 SYS适配器（参考 qs_common_sys.c）

**文件**：`adapter/ax615/sys/ax_sys_adapter.hpp/cpp`

```cpp
// 关键实现点（来自 qs_common_sys.c: SYS_CaseInit）
bool AxSysAdapter::Init(const std::vector<CmPoolBlockConfig>& pool_cfg) {
    // 1. AX_SYS_Init()
    // 2. 按pool_cfg填充 AX_POOL_FLOORPLAN_T
    // 3. AX_POOL_SetConfig() + AX_POOL_Init()
    // 4. AX_LOG_SetLevel()
}

// QSDemo默认Pool配置（供pipeline_manager使用）：
// Pool[0]: 3840*2160*3/2, 3帧, NONCACHE  → 主码流buffer
// Pool[1]: 1920*1080*3/2, 3帧            → 副码流buffer
// Pool[2]: 752*576*3/2,   4帧            → ISP输出buffer
```

### 4.2 VIN适配器（参考 qs_common_vin.c）

**文件**：`adapter/ax615/vin/ax_vin_adapter.hpp/cpp`

**Sensor注册表机制**（替代switch-case）：

```cpp
// adapter/ax615/vin/sensor_profile.hpp
struct SensorProfile {
    std::string name;                      // "os04a10"
    AX_SNS_ATTR_T sns_attr;
    AX_VIN_DEV_ATTR_T dev_attr;
    AX_VIN_PIPE_ATTR_T pipe_attr;
    std::string default_iq_bin;
    std::function<void(int pipe_id)> load_lib;  // 加载sensor库
};

// 注册os04a10
// adapter/ax615/vin/profiles/os04a10_profile.cpp
SensorProfile make_os04a10_profile();

// SensorRegistry单例
class SensorRegistry {
    std::map<std::string, SensorProfile> profiles_;
public:
    static SensorRegistry& Instance();
    void Register(const SensorProfile& p);
    const SensorProfile* Find(const std::string& name) const;
};
```

```cpp
// 关键实现点（来自 qs_common_vin.c）
bool AxVinAdapter::Open(const VinChannelConfig& cfg) {
    // 1. 从SensorRegistry查找profile
    // 2. AX_MIPI_RX_Init() + AX_MIPI_RX_SetAttr()
    // 3. AX_VIN_Init() + AX_VIN_CreateDev()
    // 4. AX_VIN_SetDevAttr() + AX_VIN_EnableDev()
    // 5. AX_VIN_CreatePipe() + AX_VIN_SetPipeAttr()
    // 6. AX_VIN_EnablePipe() + AX_VIN_EnableChn()
}
```

### 4.3 ISP适配器（参考 qs_common_isp.c）

**文件**：`adapter/ax615/isp/ax_isp_adapter.hpp/cpp`

```cpp
// 关键实现点（来自 qs_common_isp.c）
bool AxIspAdapter::Open(int pipe_id, const std::string& iq_bin_path) {
    // 1. AX_ISP_Open(pipe_id)
    // 2. 注册AE库: AX_ISP_ALG_AeRegister()
    // 3. 注册AWB库: AX_ISP_ALG_AwbRegister()
    // 4. AX_ISP_LoadBinParams(iq_bin_path.c_str())
    // 5. AX_ISP_SetAeParam() / AX_ISP_SetAwbParam()
}

bool AxIspAdapter::Start() {
    // AX_ISP_Start(pipe_id_) → 启动ISP线程
}
```

### 4.4 IVPS适配器（参考 qs_common_ivps.c）

**文件**：`adapter/ax615/ivps/ax_ivps_adapter.hpp/cpp`

```cpp
// 关键实现点（来自 qs_common_ivps.c）
bool AxIvpsAdapter::Open(const IvpsChannelConfig& cfg) {
    // 1. AX_IVPS_Init()
    // 2. AX_IVPS_CreateGrp() + AX_IVPS_SetGrpAttr()
    // 3. AX_IVPS_SetChnAttr() per channel
    // 4. AX_IVPS_EnableGrp() + AX_IVPS_EnableChn()
}
```

### 4.5 VENC适配器（参考 qs_common_venc.c）

**文件**：`adapter/ax615/venc/ax_venc_adapter.hpp/cpp`

```cpp
// 关键实现点（来自 qs_common_venc.c）
bool AxVencAdapter::Open(const VencChannelConfig& cfg) {
    // 1. AX_VENC_Init()
    // 2. 按codec/rc_mode填充 AX_VENC_CHN_ATTR_T
    //    - H264/CBR: stRcAttr.stH264Cbr
    //    - H264/VBR: stRcAttr.stH264Vbr
    //    - H265/CBR: stRcAttr.stH265Cbr
    //    - MJPEG:    stRcAttr.stMjpegCbr
    // 3. AX_VENC_CreateChn()
}

bool AxVencAdapter::Start() {
    // 1. AX_VENC_StartRecvFrame()
    // 2. 启动回调线程（替代QSDemo的轮询）：
    //    while(running_) {
    //        AX_VENC_GetStream(chn_id_, &stream, -1);
    //        if(callback_) callback_(to_StreamFrame(stream));
    //        AX_VENC_ReleaseStream(chn_id_, &stream);
    //    }
}

bool AxVencAdapter::ApplyFrameRate(const FrameRateConfig& fr) {
    // Fixed模式: AX_VENC_SetRcParam() 更新帧率
    // Interval模式（低功耗）: 配合VIN帧率降低，1/interval_sec fps
}
```

### 4.6 RTSP适配器（参考 QSDemo/src/rtsp/）

**文件**：`adapter/ax615/rtsp/ax_rtsp_adapter.hpp/cpp`

```cpp
// 封装 AXRtspServer + AXLiveServerMediaSession
bool AxRtspAdapter::Start(int port) {
    // 创建 RTSPServer，监听port
}
bool AxRtspAdapter::AddStream(const std::string& path, int venc_chn) {
    // 创建 AXLiveServerMediaSession，绑定venc_chn码流
}
```

---

## 五、Pipeline编排层（pipeline/）

**文件**：`pipeline/ax_media_pipeline.hpp/cpp`

Pipeline层是工厂+调度，只持有接口指针，通过 `AX_SYS_Link` 绑定模块：

```cpp
class AxMediaPipeline : public IMediaPipeline {
    // 持有接口指针，不持有AX SDK类型
    std::unique_ptr<ISysAdapter>  sys_;
    std::unique_ptr<IVinAdapter>  vin_;
    std::unique_ptr<IIspAdapter>  isp_;
    std::unique_ptr<IIvpsAdapter> ivps_;
    std::unique_ptr<IVencAdapter> venc_main_;   // 主码流
    std::unique_ptr<IVencAdapter> venc_sub_;    // 子码流（预览）
    std::unique_ptr<IRtspAdapter> rtsp_;

public:
    bool Init(const PipelineConfig& cfg);
    void Start();
    void Stop();
    void Deinit();

    // 模式切换
    void ApplyLowPowerProfile(int interval_sec);
    void ApplyNormalProfile(int fps = 15);
    void RequestIdr();

private:
    void LinkModules();    // AX_SYS_Link: VIN→ISP→IVPS→VENC
    void UnlinkModules();
};
```

**链路绑定顺序**（参考 qsdemo.c 主流程）：

```
VIN Pipe → ISP → IVPS Grp → IVPS Chn0 → VENC(主码流)
                           → IVPS Chn1 → VENC(子码流/RTSP)
```

**工厂函数**（注入AX615实现）：

```cpp
// pipeline/pipeline_factory.hpp
std::unique_ptr<IMediaPipeline> CreateAx615Pipeline(const PipelineConfig& cfg);
// 内部创建 AxSysAdapter, AxVinAdapter 等，注入到 AxMediaPipeline
```

---

## 六、实施步骤

| 步骤 | 文件 | 说明 |
|------|------|------|
| 1 | `common/media_types.hpp` | 更新POD类型 |
| 2 | `interface/i_sys_adapter.hpp` | 已完成 |
| 3 | `interface/i_vin_adapter.hpp` | 已完成 |
| 4 | `interface/i_isp_adapter.hpp` | 已完成 |
| 5 | `interface/i_ivps_adapter.hpp` | 已完成 |
| 6 | `interface/i_venc_adapter.hpp` | 已完成 |
| 7 | `interface/i_audio_adapter.hpp` | 新建 |
| 8 | `interface/i_rtsp_adapter.hpp` | 新建 |
| 9 | `adapter/ax615/sys/ax_sys_adapter.hpp/cpp` | 已有骨架，按方案补全 |
| 10 | `adapter/ax615/vin/sensor_profile.hpp` | 新建SensorProfile结构 |
| 11 | `adapter/ax615/vin/sensor_registry.hpp/cpp` | 新建注册表单例 |
| 12 | `adapter/ax615/vin/profiles/os04a10_profile.cpp` | os04a10 profile |
| 13 | `adapter/ax615/vin/ax_vin_adapter.hpp/cpp` | VIN适配器实现 |
| 14 | `adapter/ax615/isp/ax_isp_adapter.hpp/cpp` | ISP适配器实现 |
| 15 | `adapter/ax615/ivps/ax_ivps_adapter.hpp/cpp` | IVPS适配器实现 |
| 16 | `adapter/ax615/venc/ax_venc_adapter.hpp/cpp` | VENC适配器实现 |
| 17 | `adapter/ax615/rtsp/ax_rtsp_adapter.hpp/cpp` | RTSP适配器实现 |
| 18 | `pipeline/ax_media_pipeline.hpp/cpp` | Pipeline编排 |
| 19 | `pipeline/pipeline_factory.hpp/cpp` | 工厂函数 |
| 20 | `CMakeLists.txt` | 更新构建规则 |

---

## 七、与上层app的边界

- `app/` 层只调用 `IMediaPipeline` 接口，通过 `CreateAx615Pipeline()` 获取实例
- `app/` 层不直接接触任何 AX SDK 头文件
- `app/core` 通过 `IMediaRuntime` 控制媒体链（录像/检测/预览）
- SDK升级或换芯片：只需替换 `adapter/ax615/` 下的实现文件
