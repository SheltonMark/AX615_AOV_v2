# libmedia 架构设计与落地规划

基于 AOV_设计文档.html、code_v2_实施方案_v1.md、QSDemo_源码分析文档.md

**目标**：先收口 libmedia 可编译骨架，再按 QSDemo 迁移 AX615 媒体链实现；码流链路统一采用“裸编码帧 → app/packet 分发 → 业务消费者处理”的方式，避免在 libmedia 或 packet 中引入私有帧头/尾。

---

## 一、核心设计原则（3条铁律）

### 1.1 依赖倒置 + SDK隔离

```
app/core         → 只调用 libmedia/interface/ 接口
libmedia/        → 业务编排，不含AX SDK头文件
adapter/ax615/   → 唯一可以 #include AX SDK 的地方
```

### 1.2 职责边界

| 层 | 职责 | 禁止 |
|---|---|---|
| libmedia/interface/ | 纯抽象接口（C++虚函数） | 包含任何SDK类型 |
| libmedia/pipeline/ | 媒体链调度（VIN→ISP→IVPS→VENC绑定） | 直接调用AX API |
| adapter/ax615/ | 封装AX615 SDK，吸收QSDemo实现 | 业务状态机、硬编码配置 |

### 1.3 数据契约

所有跨层传值使用 `libmedia/common/media_types.hpp` 定义的POD结构体，禁止暴露：
- AX SDK句柄（如 `AX_VIN_PIPE_ATTR_T`）
- 大华框架类型（如 `CPacket*`）
- app层业务类型（如 `ConfiguredWorkMode`）

### 1.4 裸码流边界

`libmedia` 只输出 VENC/JENC 产生的裸编码帧，不添加私有帧头/尾；`app/packet` 只负责缓存、引用计数、跨线程分发和背压控制，也不再添加或剥离私有帧头/尾。

业务消费者按用途处理 Packet 分发出的裸帧：
- 云端预览 / 云存：直接消费裸码流，必要时只做云 SDK 要求的发送封装。
- SD 卡录像：在 `app/storage/dhfs` 内部执行 PS/PES 封装，再写 SD 卡文件。
- 告警抓图：按 JPEG 或事件元数据独立上报，不反向污染 libmedia 和 packet 的帧格式。

---

## 二、目录结构（最终版）

```
libmedia/
├── Build/                              ← CMake 构建输出目录（不纳入版本控制）
│
├── Include/                            ← 对外暴露的公共头文件（app层只包含此目录）
│   ├── media_types.hpp                 ← 公共POD类型（无SDK依赖）
│   ├── i_media_runtime.hpp             ← 媒体链总入口
│   ├── i_detect_service.hpp            ← 检测服务接口
│   ├── i_stream_capture_service.hpp    ← 码流捕获与转发接口
│   └── i_osd_service.hpp               ← OSD叠加接口
│
└── Src/                                ← 内部实现（app层不可直接包含）
    │
    ├── adapter/ax615/                  ← AX615 SDK封装（从QSDemo移植）
    │   ├── sys/
    │   │   ├── ax_sys_adapter.hpp/cpp  ← AX_SYS_Init / CMPool配置
    │   │   └── ax_link_manager.hpp/cpp ← AX_SYS_Link封装
    │   ├── vin/
    │   │   ├── ax_vin_adapter.hpp/cpp  ← VIN/MIPI/Sensor
    │   │   ├── sensor_registry.hpp/cpp ← Sensor配置注册表（单例）
    │   │   └── sensors/
    │   │       └── os04d10_profile.cpp ← OS04D10具体配置
    │   ├── isp/
    │   │   └── ax_isp_adapter.hpp/cpp  ← ISP + 3A算法库注册
    │   ├── ivps/
    │   │   └── ax_ivps_adapter.hpp/cpp ← IVPS缩放/OSD
    │   ├── venc/
    │   │   └── ax_venc_adapter.hpp/cpp ← VENC编码 + 码流回调线程
    │   ├── jenc/
    │   │   └── ax_jenc_adapter.hpp/cpp ← JENC抓图（JPEG）
    │   └── rtsp/
    │       └── ax_rtsp_adapter.hpp/cpp ← RTSP服务（live555封装）
    │
    ├── audio/                          ← 音频处理（AENC）
    │   └── audio_encoder.hpp/cpp       ← 音频编码（AX_AENC）
    │
    ├── video/                          ← 视频处理（非adapter层）
    │   ├── video_encode.hpp/cpp        ← 视频编码管理（调用adapter/venc）
    │   └── video_utils.hpp/cpp         ← 帧格式转换、分辨率计算等工具
    │
    ├── isp/                            ← ISP业务层（不含AX SDK，调用adapter）
    │   └── isp_manager.hpp/cpp         ← ISP参数管理、场景切换
    │
    ├── pipeline/
    │   ├── media_pipeline_manager.hpp/cpp  ← 媒体链编排（持有adapter实例）
    │   └── media_pipeline_factory.hpp/cpp  ← 工厂（注入AX615 adapter）
    │
    ├── detect/
    │   └── detect_service.hpp/cpp      ← 检测服务实现（调用adapter/ax615/skel）
    │
    ├── capture/
    │   └── stream_capture_service.hpp/cpp  ← 码流捕获服务（回调给app层）
    │
    └── osd/
        └── osd_service.hpp/cpp         ← OSD服务（叠框、文本、状态）
```

---

## 三、公共类型定义（common/media_types.hpp）

```cpp
namespace aov::media {

// ========== 状态码（禁止暴露AX错误码） ==========
enum class MediaStatusCode {
    Ok,
    InvalidArgument,
    InvalidState,
    Busy,
    Timeout,
    Unsupported,
    NoResource,
    InternalError,
};

// ========== 媒体规格 ==========
struct MediaProfile {
    int  record_fps       {15};    // 常电录像帧率
    int  interval_sec     {1};     // 低功耗取帧间隔（1/3/5/15）
    bool low_power_mode   {false};
};

// ========== 检测结果（事实，不含决策） ==========
struct DetectResultSummary {
    bool     has_person   {false};
    bool     has_pet      {false};
    bool     has_vehicle  {false};
    int      object_count {0};
    uint64_t frame_id     {0};
    uint64_t timestamp_ms {0};
};

// ========== 编码配置 ==========
enum class VideoCodec { H264, H265, JPEG };
enum class RcMode { CBR, VBR };

// ========== 裸编码帧 ==========
// data 指向 VENC/JENC 输出的原始 H264/H265/JPEG 数据，不包含私有帧头/尾。
// 帧内存生命周期由 libmedia 回调约定保证；app/packet 如需异步分发，必须复制或接管引用。
struct StreamFrame {
    int            chn_id      {0};
    VideoCodec     codec       {VideoCodec::H264};
    const uint8_t* data        {nullptr};
    uint32_t       size        {0};
    uint64_t       pts_ms      {0};
    uint64_t       sequence    {0};
    bool           is_keyframe {false};
};

struct VencConfig {
    int         chn_id  {0};
    VideoCodec  codec   {VideoCodec::H264};
    int         width   {1920};
    int         height  {1080};
    int         fps     {15};
    RcMode      rc_mode {RcMode::CBR};
    int         bitrate {4096};  // kbps
    int         gop     {30};
};

// ========== Sensor配置 ==========
struct SensorConfig {
    std::string sensor_name;       // "os04d10"
    int         pipe_id     {0};
    int         width       {2688};
    int         height      {1520};
    int         fps         {25};
    std::string iq_bin_path;       // "/opt/etc/os04d10.bin"
};

// ========== CMPool配置 ==========
struct CmPoolBlockConfig {
    uint64_t blk_size  {0};
    uint32_t blk_cnt   {0};
    uint64_t meta_size {512};
    bool     cached    {false};
};

struct CmPoolConfig {
    std::vector<CmPoolBlockConfig> pools;

    // 工厂方法：AX615默认Pool布局（从QSDemo移植）
    static CmPoolConfig DefaultAx615() {
        CmPoolConfig cfg;
        // Pool[0]: 主码流 2688×1520 NV12
        cfg.pools.push_back({2688ULL*1520*3/2, 3, 512, false});
        // Pool[1]: 副码流 1920×1080 NV12
        cfg.pools.push_back({1920ULL*1080*3/2, 3, 512, false});
        // Pool[2]: ISP输出 752×576 NV12
        cfg.pools.push_back({752ULL*576*3/2, 4, 512, false});
        return cfg;
    }
};

}  // namespace aov::media
```

---

## 四、核心接口定义（interface/）

### 4.1 IMediaRuntime（媒体链总入口）

**文件**：`interface/i_media_runtime.hpp`

```cpp
namespace aov::media {

class IMediaRuntime {
public:
    virtual ~IMediaRuntime() = default;

    // 初始化媒体链（VIN/ISP/IVPS/VENC全流程）
    virtual MediaStatusCode Init(const SensorConfig& sensor,
                                 const CmPoolConfig& pool) = 0;

    // 启动/停止媒体链
    virtual MediaStatusCode Start() = 0;
    virtual void Stop() = 0;

    // 低功耗/常电模式切换（对应QSDemo的UpdateSnsFps）
    virtual MediaStatusCode EnterLowPowerMode(int interval_sec) = 0;
    virtual MediaStatusCode ResumeNormalMode() = 0;

    // 录像规格切换
    virtual MediaStatusCode ApplyLowPowerRecordProfile(int interval_sec) = 0;
    virtual MediaStatusCode ApplyNormalRecordProfile(int fps) = 0;

    // 请求IDR帧
    virtual MediaStatusCode RequestIdr() = 0;

    // 获取子服务
    virtual IDetectService*        GetDetectService() = 0;
    virtual IStreamCaptureService* GetStreamCaptureService() = 0;
    virtual IOsdService*           GetOsdService() = 0;
};

}  // namespace aov::media
```

### 4.2 IDetectService（检测服务）

**文件**：`interface/i_detect_service.hpp`

```cpp
namespace aov::media {

using DetectCallback = std::function<void(const DetectResultSummary&)>;

class IDetectService {
public:
    virtual ~IDetectService() = default;

    // 单次检测（阻塞，返回结果）
    virtual MediaStatusCode RunOnceDetect(DetectResultSummary& result) = 0;

    // 注册回调（异步检测）
    virtual void RegisterDetectCallback(DetectCallback cb) = 0;

    // 启动/停止持续检测
    virtual MediaStatusCode StartDetect() = 0;
    virtual void StopDetect() = 0;
};

}  // namespace aov::media
```

### 4.3 IStreamCaptureService（码流捕获服务）

**文件**：`interface/i_stream_capture_service.hpp`

```cpp
namespace aov::media {

// 码流回调函数：libmedia获取到码流后回调给app层
// app层负责决定如何处理：存储到SD卡、转发到云端、RTSP推流等
using StreamCallback = std::function<void(int chn_id, const StreamFrame& frame)>;

class IStreamCaptureService {
public:
    virtual ~IStreamCaptureService() = default;

    // 注册码流回调（支持多通道）
    virtual MediaStatusCode RegisterStreamCallback(int chn_id, StreamCallback cb) = 0;

    // 启动/停止码流捕获
    virtual MediaStatusCode StartCapture(int chn_id) = 0;
    virtual void StopCapture(int chn_id) = 0;

    // 请求关键帧（用于录像切片、快速预览等）
    virtual MediaStatusCode RequestKeyFrame(int chn_id) = 0;

    // 查询捕获状态
    virtual bool IsCapturing(int chn_id) const = 0;
};

}  // namespace aov::media
```

### 4.4 IOsdService（OSD叠加）

**文件**：`interface/i_osd_service.hpp`

```cpp
namespace aov::media {

enum class RuntimeWorkState;  // 前向声明（来自app/core）
struct BatteryStatus;          // 前向声明（来自app/core）

class IOsdService {
public:
    virtual ~IOsdService() = default;

    // 更新检测框
    virtual void UpdateDetectOverlay(const DetectResultSummary& result) = 0;

    // 更新模式状态叠加
    virtual void UpdateModeOverlay(RuntimeWorkState state) = 0;

    // 更新电量状态叠加
    virtual void UpdateBatteryOverlay(const BatteryStatus& battery) = 0;

    // 清除所有叠加
    virtual void ClearAllOverlay() = 0;
};

}  // namespace aov::media
```

---

## 五、adapter/ax615 实现要点（从QSDemo移植）

### 5.1 SYS适配器（qs_common_sys.c）

**文件**：`adapter/ax615/sys/ax_sys_adapter.hpp`

```cpp
class AxSysAdapter {
public:
    bool Init();
    bool ConfigCmPool(const CmPoolConfig& cfg);
    void Deinit();

private:
    // 内部调用：
    // - AX_SYS_Init()
    // - AX_POOL_SetConfig() + AX_POOL_Init()
};
```

**关键点**（QSDemo qs_common_sys.c:38-80）：
- Pool配置必须在 `AX_SYS_Init` 之后、VIN初始化之前
- MetaSize固定512字节（QSDemo经验值）
- CacheMode = NONCACHE（主码流/副码流）

### 5.2 VIN适配器（qs_common_vin.c）

**文件**：`adapter/ax615/vin/ax_vin_adapter.hpp`

```cpp
class AxVinAdapter {
public:
    bool Open(const SensorConfig& cfg);
    void Close();
    bool SetFrameRate(int fps);  // 低功耗切换用

private:
    // 内部调用顺序（严格按QSDemo）：
    // 1. AX_MIPI_RX_SetAttr()
    // 2. AX_VIN_CreateDev() + AX_VIN_SetDevAttr()
    // 3. AX_VIN_EnableDev()
    // 4. AX_VIN_CreatePipe() + AX_VIN_SetPipeAttr()
    // 5. AX_VIN_SetChnAttr() (通道0/1/2)
    // 6. AX_VIN_EnablePipe() + AX_VIN_EnableChn()
};
```

**Sensor注册表**（替代QSDemo的switch-case）：

```cpp
// adapter/ax615/vin/sensor_registry.hpp
struct SensorProfile {
    std::string name;
    AX_SNS_ATTR_T sns_attr;        // ← 这里可以用AX类型
    AX_VIN_DEV_ATTR_T dev_attr;
    AX_VIN_PIPE_ATTR_T pipe_attr;
    AX_MIPI_RX_ATTR_T mipi_attr;
    std::string default_iq_bin;
};

class SensorRegistry {
public:
    static SensorRegistry& Instance();
    void Register(const SensorProfile& profile);
    const SensorProfile* Find(const std::string& name) const;
private:
    std::map<std::string, SensorProfile> profiles_;
};
```

**使用方式**：

```cpp
// adapter/ax615/vin/sensors/os04d10_profile.cpp
static bool registered = []() {
    SensorProfile profile;
    profile.name = "os04d10";
    profile.sns_attr = { /* 从QSDemo gOs04d10SnsAttr 移植 */ };
    profile.dev_attr = { /* 从QSDemo gOs04d10DevAttr 移植 */ };
    // ...
    SensorRegistry::Instance().Register(profile);
    return true;
}();
```

### 5.3 ISP适配器（qs_common_isp.c）

**文件**：`adapter/ax615/isp/ax_isp_adapter.hpp`

```cpp
class AxIspAdapter {
public:
    bool Open(int pipe_id, const std::string& iq_bin_path);
    bool Start();  // 启动ISP线程
    void Stop();
    void Close();

private:
    // 内部调用顺序（严格按QSDemo）：
    // 1. AX_ISP_Open(pipe_id)
    // 2. AX_ISP_RegisterAeLibCallback()   ← 注册AE算法库
    // 3. AX_ISP_RegisterAwbLibCallback()  ← 注册AWB算法库
    // 4. AX_ISP_LoadBinParams(iq_bin_path.c_str())
    // 5. pthread_create(&isp_thread_, IspRunThread, ...)

    static void* IspRunThread(void* arg) {
        while (running) {
            AX_ISP_Run(pipe_id);  // 阻塞调用
        }
    }

    pthread_t isp_thread_;
    bool running_{false};
};
```

### 5.4 VENC适配器（qs_common_venc.c）

**文件**：`adapter/ax615/venc/ax_venc_adapter.hpp`

```cpp
using StreamCallback = std::function<void(const StreamFrame&)>;

class AxVencAdapter {
public:
    bool CreateChannel(const VencConfig& cfg);
    bool Start(int chn_id);
    void Stop(int chn_id);
    void DestroyChannel(int chn_id);

    // 注册码流回调（替代QSDemo的轮询GetStream）
    void RegisterStreamCallback(int chn_id, StreamCallback cb);

    // 请求IDR帧
    bool RequestIdr(int chn_id);

    // 动态调整帧率
    bool SetFrameRate(int chn_id, int fps);

private:
    // 内部调用顺序（严格按QSDemo）：
    // 1. AX_VENC_Init()
    // 2. 根据VencConfig填充 AX_VENC_CHN_ATTR_T
    //    - H264/CBR: stRcAttr.stH264Cbr
    //    - H264/VBR: stRcAttr.stH264Vbr
    //    - H265/CBR: stRcAttr.stH265Cbr
    // 3. AX_VENC_CreateChn()
    // 4. AX_VENC_StartRecvFrame()
    // 5. pthread_create(&stream_thread_, StreamGetThread, ...)

    static void* StreamGetThread(void* arg) {
        while (running) {
            AX_VENC_GetStream(chn_id, &stream, -1);
            if (callback) callback(ToStreamFrame(stream));
            AX_VENC_ReleaseStream(chn_id, &stream);
        }
    }

    std::map<int, StreamCallback> callbacks_;
    std::map<int, pthread_t> stream_threads_;
};
```

### 5.5 IVPS适配器（qs_common_ivps.c）

**文件**：`adapter/ax615/ivps/ax_ivps_adapter.hpp`

```cpp
class AxIvpsAdapter {
public:
    bool CreateGroup(int grp_id, int in_w, int in_h);
    bool EnableChannel(int grp_id, int chn_id, int out_w, int out_h);
    bool Start(int grp_id);
    void Stop(int grp_id);

private:
    // 内部调用顺序（严格按QSDemo）：
    // 1. AX_IVPS_Init()
    // 2. AX_IVPS_CreateGrp()
    // 3. AX_IVPS_SetPipelineAttr()  ← 设置filter链（缩放）
    // 4. AX_IVPS_EnableChn()
    // 5. AX_IVPS_StartGrp()
};
```

### 5.6 Link Manager（AX_SYS_Link封装）

**文件**：`adapter/ax615/sys/ax_link_manager.hpp`

```cpp
struct ModLink {
    int src_mod, src_dev, src_chn;
    int dst_mod, dst_dev, dst_chn;
};

class AxLinkManager {
public:
    bool Link(const ModLink& link);
    void UnlinkAll();

private:
    // 内部调用 AX_SYS_Link()
    std::vector<ModLink> links_;
};
```

---

## 六、pipeline编排层（pipeline/）

**文件**：`pipeline/media_pipeline_manager.hpp`

```cpp
class MediaPipelineManager : public IMediaRuntime {
public:
    // 构造时注入各adapter实例（依赖注入）
    MediaPipelineManager(
        std::unique_ptr<AxSysAdapter> sys,
        std::unique_ptr<AxVinAdapter> vin,
        std::unique_ptr<AxIspAdapter> isp,
        std::unique_ptr<AxIvpsAdapter> ivps,
        std::unique_ptr<AxVencAdapter> venc,
        std::unique_ptr<AxJencAdapter> jenc
    );

    // 实现 IMediaRuntime 接口
    MediaStatusCode Init(const SensorConfig& sensor,
                        const CmPoolConfig& pool) override;
    MediaStatusCode Start() override;
    void Stop() override;
    MediaStatusCode EnterLowPowerMode(int interval_sec) override;
    MediaStatusCode ResumeNormalMode() override;
    // ...

private:
    // 核心方法：按QSDemo顺序启动各模块
    bool StartSequence();

    // 启动顺序（严格按QSDemo qsdemo.c主流程）：
    // 1. sys_->Init() + ConfigCmPool()
    // 2. vin_->Open()
    // 3. isp_->Open() + LoadIQ
    // 4. ivps_->CreateGroup()
    // 5. venc_->CreateChannel()
    // 6. LinkModules()  ← VIN→IVPS, IVPS→VENC
    // 7. vin_->Enable()
    // 8. isp_->Start()  ← 启动ISP线程
    // 9. venc_->Start() ← 启动码流线程

    bool LinkModules();  // 调用 link_mgr_->Link()

    std::unique_ptr<AxSysAdapter> sys_;
    std::unique_ptr<AxVinAdapter> vin_;
    std::unique_ptr<AxIspAdapter> isp_;
    std::unique_ptr<AxIvpsAdapter> ivps_;
    std::unique_ptr<AxVencAdapter> venc_;
    std::unique_ptr<AxJencAdapter> jenc_;
    std::unique_ptr<AxLinkManager> link_mgr_;

    // 子服务（持有指针，不拥有）
    std::unique_ptr<DetectService> detect_service_;
    std::unique_ptr<StreamCaptureService> capture_service_;
    std::unique_ptr<OsdService> osd_service_;
};
```

**工厂函数**：

```cpp
// pipeline/media_pipeline_factory.hpp
std::unique_ptr<IMediaRuntime> CreateAx615MediaRuntime();

// 实现（cpp）：
std::unique_ptr<IMediaRuntime> CreateAx615MediaRuntime() {
    // 创建各adapter实例
    auto sys  = std::make_unique<AxSysAdapter>();
    auto vin  = std::make_unique<AxVinAdapter>();
    auto isp  = std::make_unique<AxIspAdapter>();
    auto ivps = std::make_unique<AxIvpsAdapter>();
    auto venc = std::make_unique<AxVencAdapter>();
    auto jenc = std::make_unique<AxJencAdapter>();

    // 注入到pipeline
    return std::make_unique<MediaPipelineManager>(
        std::move(sys), std::move(vin), std::move(isp),
        std::move(ivps), std::move(venc), std::move(jenc)
    );
}
```

---

## 七、关键决策点（已冻结）

### 7.1 CMPool配置谁负责？

**决策**：pipeline层写死默认配置，不暴露给app层。

```cpp
// app/core 调用：
auto runtime = CreateAx615MediaRuntime();
runtime->Init(sensor_cfg, CmPoolConfig::DefaultAx615());
```

**理由**：CMPool大小与sensor分辨率强相关，app层不该管。

### 7.2 Sensor注册时机？

**决策**：静态初始化（在adapter编译时自动注册）。

```cpp
// adapter/ax615/vin/sensors/os04d10_profile.cpp
static bool registered = []() {
    SensorRegistry::Instance().Register(make_os04d10_profile());
    return true;
}();
```

**理由**：避免显式注册代码散落，编译时自动完成。

### 7.3 码流如何转发给app层？

**决策**：libmedia 通过回调函数将裸编码帧转发给 app 层，app 层统一进入 `app/packet`。`libmedia` 不负责存储、网络传输、PS 封装，也不添加私有帧头/尾。

**理由**：
- 职责分离：libmedia 专注于媒体链管理和码流获取
- `app/packet` 作为码流承载和分发中心，统一处理缓存、引用计数、跨线程分发和背压
- 裸码流在预览、云存、云端转发链路中可以直接使用，避免“添加私有头 → 再剥离私有头”的重复解析
- SD 卡录像是文件格式问题，放在 `app/storage/dhfs` 内部做 PS/PES 封装，不污染 libmedia 和 packet 的帧格式

**链路边界**：

```
libmedia/VENC
  -> StreamFrame(裸 H264/H265/JPEG)
  -> app/packet
      -> app/cloud / app/preview：裸码流或云 SDK 发送封装
      -> app/storage/dhfs：PS/PES 封装 -> 写 SD 卡文件
```

**实现**：

```cpp
// capture/stream_capture_service.hpp
class StreamCaptureService : public IStreamCaptureService {
public:
    MediaStatusCode RegisterStreamCallback(int chn_id, StreamCallback cb) override {
        callbacks_[chn_id] = cb;
        // 向 VENC adapter 注册内部回调；frame 为裸编码帧，不含私有帧头/尾
        venc_adapter_->RegisterStreamCallback(chn_id, 
            [this, chn_id](const StreamFrame& frame) {
                if (callbacks_.count(chn_id)) {
                    callbacks_[chn_id](chn_id, frame);
                }
            });
        return MediaStatusCode::Ok;
    }

private:
    std::map<int, StreamCallback> callbacks_;
    AxVencAdapter* venc_adapter_;
};
```

**app 层使用示例**：

```cpp
// app 层注册回调
auto capture = runtime->GetStreamCaptureService();
capture->RegisterStreamCallback(0, [](int chn_id, const StreamFrame& frame) {
    packet->PushEncodedFrame(chn_id, frame);  // Packet 只承载和分发裸编码帧
});
```

**Packet 消费者职责**：

- `app/cloud` / `app/preview`：从 Packet 取裸码流，按云 SDK 或直播预览接口发送。
- `app/storage/dhfs`：从 Packet 取裸码流，封装 PS/PES 后写入 SD 卡文件，并在结束时执行关闭文件 + 刷缓存 + 强制落盘。
- `app/core`：不直接处理码流，只汇总本地存储、云存、预览等业务状态，决定是否进入空闲防抖和 AOV 休眠。

### 7.4 低功耗切换如何实现？

**决策**：`EnterLowPowerMode(interval_sec)` 调用 `vin_->SetFrameRate(1000/interval_sec)`。

**映射关系**（对应QSDemo的UpdateSnsFps）：
- 1秒1帧 → fps = 1
- 3秒1帧 → fps = 0.33（向下取整为0，实际按定时器触发）
- 5秒1帧 → fps = 0.2
- 15秒1帧 → fps = 0.067

QSDemo参考：qsdemo.c:UpdateSnsFps(int fps) 调用 ax_tcm_sns_wakeup。

---

## 八、与QSDemo的映射清单

| QSDemo文件 | 移植到 | 关键函数/逻辑 |
|---|---|---|
| qs_common_sys.c | adapter/ax615/sys/ax_sys_adapter.cpp | SYS_CaseInit → CMPool配置 |
| qs_common_vin.c | adapter/ax615/vin/ax_vin_adapter.cpp | COMMON_VIN_StartDev → VIN启动序 |
| qs_common_isp.c | adapter/ax615/isp/ax_isp_adapter.cpp | COMMON_ISP_Run → ISP线程 |
| qs_common_venc.c | adapter/ax615/venc/ax_venc_adapter.cpp | SAMPLE_VENC_Start → VENC启动序 |
| qs_common_ivps.c | adapter/ax615/ivps/ax_ivps_adapter.cpp | COMMON_IVPS_Init → IVPS创建序 |
| osd/ax_osd.c | osd/osd_service.cpp + adapter/ax615/ivps/ | OSD区域叠加 |
| VENC码流获取 | capture/stream_capture_service.cpp | 码流回调转发给app层 |
| qsdemo.c:539-583 | adapter/ax615/vin/ 或 libsys/power | TCM唤醒修复（ax_tcm_sns_wakeup） |

---

## 九、实施顺序（按当前 code_v2 现状修正）

### 阶段1：libmedia 骨架收口（优先）

当前 `libmedia` 已经搭了部分目录和接口，但头文件、cpp、命名空间、CMake 仍存在多套草稿混用的问题。下一步先不继续堆 AX SDK 真实实现，先把骨架修到可编译。

1. 填充 `Depend/ax_sdk/arm_uclibc/include` 和 `Depend/ax_sdk/arm_uclibc/lib`，来源为 SDK 的 `msp/out/arm_uclibc/include`、`msp/out/arm_uclibc/lib`。
2. 对齐 `Include/` 和 `Src/` 的真实目录，修正 `CMakeLists.txt` 中旧路径、旧文件名。
3. `CMakeLists.txt` 默认只编译 host 可验证骨架；通过 `LIBMEDIA_BUILD_AX615_ADAPTERS=ON` 开启真实 AX615 adapter 编译，并使用 `Depend/ax_sdk/arm_uclibc` 下的 SDK 头文件和库。
4. 统一命名空间为 `aov::media` / `aov::media::ax615`，清理旧的 `libmedia::adapter::ax615` 写法。
5. 统一码流类型为 `StreamFrame`，语义固定为裸编码帧，不含私有帧头/尾。
6. 对齐 `IMediaRuntime` 与 `MediaPipelineManager` 的方法名、成员变量和生命周期状态。
7. adapter 层先保持接口壳，真实 `AX_*` 调用在阶段2按 QSDemo 迁移。

**验收**：`libmedia` 单独可编译；接口层无 AX SDK 依赖；`StreamFrame` 裸帧契约清晰。

### 阶段2：QSDemo 媒体链迁移到 AX615 adapter

按 QSDemo 已验证顺序迁移，不改变上层接口。

1. `qs_common_sys.c` → `adapter/ax615/sys/ax_sys_adapter.cpp`：`AX_SYS_Init`、CMPool。
2. `vin_config.c/h` + `qs_common_vin.c` → `adapter/ax615/vin/`：Sensor、MIPI、VIN 启动序。
3. `qs_common_isp.c` → `adapter/ax615/isp/`：ISP 初始化、IQ、3A、ISP 线程。
4. `qs_common_ivps.c` → `adapter/ax615/ivps/`：IVPS group、filter、输出通道。
5. `qs_common_venc.c` + `qsdemo.c::VencGetStreamProc` → `adapter/ax615/venc/`：VENC channel、取流线程、`AX_VENC_ReleaseStream`。
6. `qsdemo.c::UpdateSnsFps`、`ax_tcm_sns_wakeup` 相关逻辑单独收口，用于 AOV 低功耗/常电切换。

**验收**：真机上能按 QSDemo 顺序启动媒体链，VENC 裸码流能稳定回调到 libmedia capture 服务。

**当前进度**：
- `adapter/ax615/sys/ax_sys_adapter.*` 已对齐当前 SDK 头文件，完成 `AX_SYS_Init`、`AX_POOL_Exit`、`AX_POOL_SetConfig`、`AX_POOL_Init`、`AX_SYS_Deinit` 最小闭环。
- `adapter/ax615/sys/ax_link_manager.*` 已对齐当前 SDK 头文件，完成 `AX_SYS_Link` / `AX_SYS_UnLink` 封装。
- `adapter/ax615/vin/sensor_registry.*` 已改成可插拔注册表；新增 sensor 只增加 `sensors/<sensor>_profile.cpp` 并注册 `SensorProfile`，不改 `AxVinAdapter` 主流程。
- `adapter/ax615/vin/sensors/os04d10_profile.cpp` 已作为 profile 样例对齐当前 SDK 头文件。
- `adapter/ax615/vin/ax_vin_adapter.*` 已改成通用 `SensorProfile` 消费者，完成 MIPI SetAttr/Reset/Start、VIN Init/CreateDev/SetDevAttr/CreatePipe/SetPipeAttr/SetChnAttr/EnableChn/EnableDev、Disable/Destroy/Deinit 主流程。
- `adapter/ax615/isp/ax_isp_adapter.*` 已对齐当前 SDK，完成 ISP Create/Open/Start/Stop/Close/Destroy、可选 Sensor 注册、AE/AWB 3A callback 注册、IQ bin 加载。
- `adapter/ax615/ivps/ax_ivps_adapter.*` 已对齐当前 SDK，完成 IVPS Init/CreateGrp/SetPipelineAttr/EnableChn/StartGrp/StopGrp/DisableChn/DestroyGrp/Deinit 主流程，并保留 OSD region 的 Create/Update/Destroy 基础封装。
- `adapter/ax615/venc/ax_venc_adapter.*` 已对齐当前 SDK，完成 VENC Init/CreateChn/StartRecvFrame/GetStream/ReleaseStream/StopRecvFrame/DestroyChn/Deinit 主流程；取流线程只把 `AX_VENC_STREAM_T.stPack` 转成 `StreamFrame` 裸编码帧并回调上层，不添加私有帧头/尾。
- `adapter/ax615/jenc/ax_jenc_adapter.*` 已对齐当前 SDK：AX615 上 JPEG 抓图不是独立 `AX_JENC_*` API，而是通过 `AX_VENC_CreateChn(PT_JPEG)`、`AX_VENC_GetJpegParam/SetJpegParam`、`AX_VENC_StartRecvFrame/GetStream/ReleaseStream/StopRecvFrame` 实现；输出 `StreamFrame` 裸 JPEG，作为告警抓图链路输入。
- `adapter/ax615/power/ax_aov_power_adapter.*` 已收口媒体侧 AOV 低功耗切换动作：VIN pipe fps 降/恢复、VENC RC fps/GOP/bitrate 调整、恢复常电时请求 IDR、TCM sensor low-power/normal 信息设置和 `ax_tcm_sns_wakeup`。该 adapter 不决定何时休眠，只执行 `app/core`/`libsys/power` 决策后的媒体侧参数切换。
- `Src/pipeline/media_pipeline_manager.*` 已新增 AX615 主链配置和启动/Link 计划：`ConfigureAx615Pipeline()` 校验 sensor/pipe/IVPS/VENC/JENC 配置，生成 SYS → VIN → ISP → IVPS → VENC/JENC → Link → Start 的顺序计划和 VIN→IVPS、IVPS→VENC/JENC 链路计划；host 路径仍保持不触碰真实 AX 硬件，便于合同测试。
- `MediaPipelineManager` 已在 `LIBMEDIA_BUILD_AX615_ADAPTERS` 条件编译下接入真实 adapter：工厂创建 SYS/VIN/ISP/IVPS/VENC/JENC/Link/AOV power，`Start()` 映射到 Open/Create/Link/Enable/Start/取流回调，`Stop()` 按反序释放，`EnterLowPowerMode()/ResumeNormalMode()` 映射到 `AxAovPowerAdapter`。
- 已通过 `g++ -fsyntax-only` + `Depend/ax_sdk/arm_uclibc/include` 的 SYS/Link/VIN/SensorRegistry/OS04D10 profile/ISP/IVPS/VENC/JENC/AOV power 语法编译验证。

**新增 Sensor 规则**：
1. 在 `adapter/ax615/vin/sensors/` 新增 `<sensor>_profile.cpp`。
2. 文件内只负责填写 `SensorProfile`：driver so 名、driver symbol、quick-start symbol、默认 IQ 路径、MIPI 参数、SNS 属性、VIN dev/pipe/chn 属性。
3. 通过静态 registrar 调用 `SensorRegistry::Instance().Register(profile)`。
4. 禁止在 `AxVinAdapter` 中写 `if sensor == xxx` 的型号特判；`AxVinAdapter` 只按 `SensorProfile` 执行通用 open/enable/close 流程。
5. 新增 sensor 必须单独通过 `g++ -fsyntax-only` + SDK include 的 profile 编译验证，避免影响其他已注册 sensor。

**阶段2剩余步骤**：
1. 在板端用 `LIBMEDIA_BUILD_AX615_ADAPTERS=ON` 做真实 SDK 链接编译，确认库名、链接顺序、pthread/dl 依赖完整。
2. 真机跑 SYS → VIN → ISP → IVPS → VENC/JENC → Link → Start → 取流回调，重点验证首帧、ReleaseStream、Stop 反序释放和重复启停。
3. 真机联调 AOV 低功耗切换：确认 `aov_pipe_src_fps/aov_pipe_dst_fps`、TCM `cap_frame_num`、VENC GOP/bitrate 策略与当前 sensor/profile 匹配。

**后续阶段提醒**：
- 阶段3：接 `app/packet` 裸流分发，`libmedia -> app/packet -> storage/cloud/preview`。
- 阶段4：接 SD 本地存储和云端消费者，SD 侧在 `app/storage/dhfs` 做 PS/PES 封装，云端直接消费裸码流或云 SDK 封装。

### 阶段3：Packet 裸流分发链路

Packet 是现有模块，优先复用已有队列、缓存、引用计数和跨线程分发能力，但需要把帧格式职责调整为“裸编码帧承载”，不再添加私有帧头/尾。

1. 新增或调整 Packet 输入接口：`PushEncodedFrame(chn_id, StreamFrame)`。
2. Packet 内部保存裸帧数据和必要元信息：通道、编码格式、PTS、序号、关键帧标记。
3. 云端预览、云存、本地存储都作为 Packet 消费者订阅。
4. 验证慢消费者不会阻塞 VENC 取流线程，必要时采用丢帧、限队列或引用计数释放策略。

**验收**：`libmedia -> app/packet -> storage_stub/cloud_stub/preview_stub` 链路可跑；链路中没有私有帧头添加和剥离动作。

### 阶段4：本地存储与云端消费者接入

1. SD 卡录像：`Packet -> app/storage/dhfs -> PS/PES 封装 -> 写 SD 卡文件 -> 关闭文件 + 刷缓存 + 强制落盘`。
2. 云端预览：`Packet -> app/cloud/app/preview -> 裸码流或云 SDK 发送封装 -> 手机 App`。
3. 云存：`Packet -> app/cloud -> 云存 SDK 封装 -> 云端存储 -> 成功或明确失败后回 app/core`。
4. `app/core` 汇总本地存储、云存、预览、控制命令状态，所有业务链完成后进入空闲防抖，再回 AOV 休眠。

**验收**：SD 卡和云端可以并行消费 Packet；本地存储完成、云存成功或明确失败后，core 才允许回 AOV 休眠。
