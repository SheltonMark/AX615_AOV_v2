# code_v2 实施方案（v1 · 待评审）

> 本文档是进入正式编码前的实施规划，供评审。约定：
> - **stub / 业务逻辑层严格 TDD**（先写测试，红→绿→重构，目标 80% 覆盖）；
> - **adapter / SDK 真机层只做集成冒烟**（依赖真机与 AX SDK，不强求单测覆盖）；
> - **Packet.cpp 抽算法重写**（不裸搬大华 Kylin 框架）。
> 若实现与本文冲突，先改本文，再改代码。优先级高于个人临时写法。

---

## 1. 现状盘点（事实，已逐一核对）

### 1.1 已落地且扎实
- 目录骨架完整（app / libmedia / libsys / Depend / products / tests / tools 全建好），`CMakeLists.txt` 产出 `ax615_aov_device_stubs` 静态库，**纯 host 编译、零 SDK 依赖**。
- 16 个 `i_*.hpp` 抽象接口 + 16 个 `*_stub.cpp` 占位实现，三层命名空间清晰：`aov::app` / `aov::media` / `aov::sys`。
- **`AovOrchestratorStub` 已是真状态机**：`ResolveRuntimeState()` 将「配置模式 × 电量 × 预览态」决议为运行态，含低电保护覆盖、智能不休眠 70/80 降级恢复。不是空壳。
- 数据契约冻结完整：`WorkModePolicy` / `BatteryStatus` / `WakeupEvent` / `DetectResultSummary` / `PreviewRequest` / `AlarmEvent` / `AovRuntimeContext`（app），`MediaProfile` / `MediaStatusCode`（media），`ModemHeartbeatState` / `NetworkState` / `SysStatusCode`（sys）。

### 1.2 空白（目录在，无文件）
- `app/entry`（入口 / bootstrap / 依赖装配）— **当前没有 main，stub 串不起来**。
- `libmedia/adapter/ax615`、`libmedia/pipeline`、`libmedia/interface`、`libmedia/rtsp`。
- `libsys/os`、`libsys/include`。
- `tests/{unit,integration,mock}` — **零测试**（违背 80% 覆盖要求，阶段 0 必须补）。
- `products/ax615_default/{factory,schema,profile}` — 无配置文件。

### 1.3 现有 stub 成熟度
- `app/core`：真状态机（见上）。
- `libmedia/media_runtime_stub`：内存态翻转（`low_power_mode` / `interval_sec` / `record_fps`），参数校验到位。
- 其余 stub：返回 `Ok` / 翻转内部 bool 的占位级，足够编译与装配，不含真行为。

---

## 2. 外部可复用资产评估（已读源码核对）

### 2.1 QSDemo（`A:\ax615_sdk_clean\...\app\demo\QSDemo`）
真实链路参考，**行为参考不照搬**。关键事实：

| 能力 | 位置 | 复用方式 |
|---|---|---|
| AOV 状态机 / sleep / wake / fps 切换 | `qsdemo.c`（5041 行，巨函数） | 抽调用顺序到 adapter，**禁止拆几刀搬运** |
| sensor 唤醒出帧（TCM） | `qsdemo.c:539-583` WAKEUP 分支 | 直接参考，已含 `ax_tcm_sns_wakeup` 修复 |
| 媒体链 VIN→IVPS→VENC/JENC/SKEL | `src/com/qs_common_*.c` | `libmedia/adapter/ax615` 吸收调用序 |
| 录像 / SD 卡管理 | `utils/qs_recorder.c`（C 接口，1400 行） | adapter 内封装，对上出 `IMediaRecordService` |
| RTSP | `rtsp/AXRtspServer.cpp`（242 行，live555） | `libmedia/rtsp` adapter 封装 |
| OSD / 字库 | `src/osd/` | `libmedia/osd` adapter 吸收 |

**已知坑（必须正视）**：QSDemo 自带分析文档（`QSDemo_AOV_Analysis.md` 第 11 节）指出——当前 OS04D10 板级路径 wake 后**曾经不出帧**导致 SKEL/VENC/RTSP 断流。源码 `qsdemo.c:539-583` 已落了推荐修复（wake 后显式 `ax_tcm_sns_info_set` + `ax_tcm_sns_wakeup`）。**阶段 1 第一件事就是真机验证这个修复是否生效**，它决定整条 AOV 主链路成败。

### 2.2 Packet 体系（`Z:\cp3prov1.2\Apps\Kylin\System\Packet.cpp` 等，662 行 + packet_manager/impl 头）

**决策演进（已与产品背景对齐，覆盖此前判断）**：

- 此前我判断「不引入 Packet」，依据是分层冻结 + 不拖大华框架 + QSDemo 已自带 `AXRingFifo`。
- 产品背景补充后**修正为：保留 Packet 设计，替换掉 AXRingFifo，码流出编码器后按 cp3 那套 Packet 设计承载。**
- 背景：旧产品要同时供 云端App + Web + NVR 多端，Packet 的原始价值是**多路复用**。AOV 新产品只有云端 App 一个消费者，多路复用动机消失——但 **Packet「码流→封包→攒一批→一次性落盘」的承载/分页范式本身值得沿用**，正好天然契合本项目要的「攒包批量写、少挂载、降功耗」。

**关键边界（cp3 Packet 是两层，只引一层）**：

| 层 | 内容 | code_v2 处置 |
|---|---|---|
| ① 承载/分页缓冲层 | `packet_manager`（伙伴分页内存池）+ `CPacket`（包对象：攒帧、页对齐、`GetBuffer`） | **保留并引入**，与 `AXRingFifo` 同位，**替换它** |
| ② DHAV 帧格式解析层 | `Packet.cpp` 的 `findIframePtsfrom_begin`、`0x44484156`("DHAV")/`0x5a4c4156`("VALZ") 私有帧头解析 | **不移植**——给 NVR/Web 回放用；AX615 走标准 H.264/H.265 裸流，云端 App 用不到 |

**落地原则**：
- 引入①时**剥离大华框架依赖**（`Support.h`/`Mutex.h`/`Object.h`/`MediaStream.h`/`AppConfig.h`/`ConfigManager_new.h`、`#ifdef CP3PROV12` 等），重写为 `libmedia/common` 下无框架依赖的 Packet/PagePool（保留其分页伙伴算法与包语义，去掉 Kylin 耦合）。这与「不裸搬大华框架」的冻结约束不冲突——**保留的是设计与算法，不是框架**。
- 录像写盘：`libmedia/record` 用 Packet 承载编码帧，攒到阈值（容量/时间窗，见阶段 1）整批落盘，挂载/卸载按批次发生。
- 缓冲可调大：Packet 池容量作为配置项（对应你提的「加大缓冲」）。

### 2.3 与 QSDemo 写盘策略的关系（已读源码）
QSDemo 的 `qs_recorder.c` **已经是「攒一段再批量写」**：每帧经 `QS_SaveVideo` 入 `AXRingFifo`（不碰 SD）；写盘线程等 FIFO 攒到 **2/3 容量**才触发（`qs_recorder.c:749-755`）；触发后挂载 SD→整批 `fwrite`→`fsync`→卸载（离线模式 `QS_InstallSdDevice`/`QS_RemoveSdDevice`），并用 `update_sleep_condition(CONDITION_MASK_RECORD)` 与 AOV 休眠协同。

**所以「攒包批量写 + 少挂载」机制 QSDemo 已验证可行**。本项目的差异只是：**把承载单元从 `AXRingFifo` 换成重写后的 Packet**，触发/挂载/休眠协同逻辑沿用 QSDemo 的成熟做法。

---

## 3. 分阶段实施计划

四个阶段，每阶段有明确产出与验收标准。阶段 0 / 部分阶段 2、3 是纯逻辑层（严格 TDD）；阶段 1 与各 adapter 是真机层（集成冒烟）。

### 阶段 0 — 地基补齐（纯 host，严格 TDD）｜预计 0.5–1 天

**目标**：让现有 16 个 stub 串成可运行空回路，并建立测试护栏。全程不碰 SDK。

**任务**
1. `tests/` 引入 GoogleTest（`Depend/third_party` 放置或 CMake `FetchContent`，离线环境优先 vendored）。
2. `app/entry/main.cpp` + 依赖装配（手写组合根 composition root）：按接口注入各 stub，串出「定时器→core→media→record」的全 stub 回路，可 `./ax615_aov_device` 跑一轮日志。
3. **先写测试**锁住已有真逻辑：
   - `AovOrchestratorStub`：覆盖 `ResolveRuntimeState()` 全分支（PowerSaving→SleepPeriodicDetect；SmartNoSleep 69%/81% 降级恢复；UltraLongStandby→SleepAppWakeOnly；低电保护覆盖一切；预览态强制 AwakeNormal）；`IsValidPolicy` 边界（interval 1/3/5/15 合法、其余非法；阈值 5–30；save<awake）。
   - `MediaRuntimeStub`：profile 翻转与参数校验（interval≤0、fps≤0 报 InvalidArgument）。
4. CMake 增加 `ax615_aov_device`（可执行）+ `ax615_aov_tests`（测试）两个 target，`ctest` 可跑。

**验收**
- `cmake --build` 通过；`ctest` 全绿；orchestrator 状态机分支覆盖 ≥ 80%。
- `main` 能跑出一轮「唤醒→检测(无目标)→回休眠」的 stub 日志。

**风险**：离线环境拉不到 GoogleTest → 预案：vendored 一份到 `Depend/third_party/googletest`，或退化用极简自写断言宏（不推荐，仅兜底）。

---

### 阶段 1 — adapter 接 AX SDK，打通 AOV 主链路（真机，集成冒烟）｜预计 3–4 天

**目标**：定时唤醒→轻量检测→（有目标）恢复常电录像 这条主链路真机跑通。**这是整个项目的关键路径与最大风险点。**

**任务**
1. `Depend/ax_sdk` 接入 AX615 SDK 头与库，CMake 增加交叉编译 toolchain（`tools/build` 放 toolchain.cmake）。
2. `libmedia/adapter/ax615`：实现 `MediaRuntime` 真版，吸收 `qs_common_cam.c` 的 VIN/IVPS/VENC/JENC 启动序；`EnterLowPowerMode`/`ResumeNormalMode` 映射 `UpdateSnsFps(0)` / `UpdateSnsFps(default)`。
3. **攻坚 wake 出帧**：在 `libsys/power` 的 sleep/wake 实现里落 `qsdemo.c:539-583` 的 TCM 唤醒序，真机验证 wake 后 SKEL/VENC 能拿到新帧（这是 QSDemo 已知断流点）。
4. `libmedia/detect`：接 SKEL，`RunOnceDetect()` 输出 `DetectResultSummary`。
5. `libsys/storage`：封装 `qs_recorder.c`（挂载、目录、写盘），对上出 `IStorageService` + 录像落盘。
6. 真机集成冒烟脚本（`tools/debug`）：跑「省电模式 1 秒间隔，无人回睡 / 有人恢复 15fps」。

**验收**
- 真机：定时唤醒后能取到检测帧（wake 不再断流）；检测到人→切 15fps 常电录像→SD 卡有文件；无人→回睡。
- 集成冒烟脚本可重复跑通；关键节点有日志（wake 帧号、work_mode、cap_frame_num）。

**风险（高）**：wake 出帧若真机仍失败 → 按分析文档 7.1 深挖 sensor/VIN 统计；必要时 7.2 兜底（wake 直接恢复，牺牲功耗）。此风险可能单独吃掉 1–2 天。

---

### 阶段 2 — 业务链路补全（逻辑层 TDD + adapter 冒烟）｜预计 2–3 天

**目标**：打通「检测→core→record→alarm→cloud」事件闭环（即设计文档时序图 2），抓图/录像索引/告警绑定上云。

**任务**
1. `app/alarm`：`EvaluateDetectResult()` 真逻辑（检测摘要→AlarmType），`IsAlarmAllowed()` 落保护态裁剪。**严格 TDD**。
2. `app/record`：事件录像片段管理、索引元数据。低功耗间隔 vs 事件 15fps 双规格切换。**逻辑部分 TDD**。
3. `libmedia` 抓图：来自子码流（按你的更正——不是 JENC/主码流，是**子码流**），adapter 封装。
4. `app/cloud`：告警 + 抓图 + 录像索引绑定上报；`ReportDeviceState` / `ReportBatteryStatus`。云协议适配封装（腾讯云优先走适配层）。
5. （按需）若有缓冲需求，重写 `libmedia/common` 的 RingBuffer（Packet 抽算法），否则用 SDK pool。

**验收**
- 逻辑层（alarm 决策、record 策略）单测 ≥ 80%，全绿。
- 真机：检测到目标→抓子码流图→事件录像→告警带图+索引上云→收到回执。

---

### 阶段 3 — 4G 与产品化收尾（真机冒烟）｜预计 2 天

**目标**：远程预览闭环 + 4G 心跳 + 配置下发生效。

**任务**
1. `libsys/modem`：4G 模组在线/心跳采集，GPIO 拉脚唤醒主控（`libsys/device` 配合）。
2. App 远程预览闭环（时序图 3）：cloud→modem→GPIO→core→media→preview→回传。
3. `app/config` + `products/ax615_default/{factory,schema,profile}`：配置 schema 落地，下发→持久化→通知 core→各层生效（时序图 6）。
4. 电量降级/恢复、低电保护进入/退出真机验证（时序图 4、5）。

**验收**
- App 发起预览→设备 4G 唤醒→出预览流→App 可见。
- 心跳稳定上报，App 侧看到在线/电量/模式。
- 改配置（如唤醒间隔 1→5）→设备实际生效。

---

## 4. 测试策略（对应「stub/业务逻辑严格 TDD」）

| 层 | 测试要求 | 框架 |
|---|---|---|
| `app/core` 状态机、`app/alarm` 决策、`app/config` 校验、`app/record` 策略 | **严格 TDD，≥80% 覆盖** | GoogleTest 单测 |
| 各 stub（占位实现） | 行为契约测试（返回码、状态翻转） | GoogleTest 单测 |
| `libmedia/adapter`、`libsys/power/modem` 真机实现 | 集成冒烟（真机脚本），mock SDK 可选 | tools/debug 脚本 + 可选 mock |
| 跨层装配（entry） | 集成测试（全 stub 回路跑通） | GoogleTest integration |

`tests/mock` 放接口 mock（用于 alarm 依赖 detect 等跨层用例）；`tests/unit` 放纯逻辑；`tests/integration` 放装配回路。

---

## 5. 关键依赖与顺序约束

```
阶段0(地基+护栏) ──► 阶段1(主链路,真机) ──► 阶段2(业务闭环) ──► 阶段3(4G+产品化)
        │                    │
        │                    └─ 风险点:wake出帧(可能阻塞1-2天)
        └─ 纯host,无SDK,可立即开始
```

- 阶段 0 不依赖任何外部环境，**建议立即可做**。
- 阶段 1 起需要真机 + AX SDK + 交叉编译链就绪。
- Packet 重写为「按需」支线，不阻塞主线。

---

## 6. 评审确认结果（已定）

| # | 问题 | 结论 |
|---|---|---|
| 1 | Packet 调整 | ✅ **接受**：抽算法重写 + DHAV 不移植 + 主链路优先用 AX SDK pool |
| 2 | 抓图来源 | ✅ **子码流**（设计文档时序图已修正） |
| 3 | GoogleTest 引入 | ✅ **离线 vendored**（见 §8 环境落地） |
| 4 | 交叉编译链 | ✅ 已确认，uclibc 链，**在 192.168.170.201 Linux 服务器编译**（见 §8） |
| 5 | 真机 | ✅ **demo 板已就绪，QSDemo 已跑起来** |
| 6 | 云协议 | ✅ 文档+SDK+产品代码三者齐备（见 §9） |

<!-- APPEND-ENV -->

---

## 8. 环境与依赖落地（已核对真实路径）

### 8.1 构建环境
- **编译在 192.168.170.201 Linux 服务器**（Windows 经 samba 映射 A盘/Z盘访问；A盘=Z盘=同一物理位置，已验证）。
- **不在 Windows 本地编译**；CMake/构建脚本以 Linux 路径为准。

### 8.2 交叉编译工具链（uclibc，ARM）
- Linux 实际路径：`/home/mashuntao/ax615_sdk_clean/toolchain/...`
  对应 Windows 访问：`A:\ax615_sdk_clean\toolchain\`
- 编译器：
  - `ax615_uclibc_toolchain/V4_20250701/arm-rel-linux-uclibcgnueabihf/bin/arm-rel-linux-uclibcgnueabihf-{gcc,g++}`
  - RISC-V（协处理器，按需）：`RISC-V_toolchain/riscv64-elf-gnu-amd64/bin/`
- 落地：`tools/build/toolchain-ax615-uclibc.cmake`（设 `CMAKE_C/CXX_COMPILER`、sysroot），CMake `-DCMAKE_TOOLCHAIN_FILE=` 引用。

### 8.3 AX615 SDK（头 + 库，uclibc 版）
SDK 根：`A:\ax615_sdk_clean\02.SDK\AX615_SDK_V2.0.0_P5_20260326181014_NO68`
- **头文件**：`msp/out/arm_uclibc/include/`（含 `ax_ivps_api.h` / `ax_skel_api.h` / `ax_venc_api.h` 等）
- **库**：`build/out/AX615_nor_uclibc/objs/opt/lib/`（`libax_ivps.a` / `libax_skel.a` / `libax_venc.a` / `libax_proton` / `libax_mipi` / `libax_tcm` …）
- **传感器库**：`libsns_os04d10.a`（本板 sensor）
- **链接清单权威来源**：`app/demo/QSDemo/Makefile.static`（已读，CLIB 段完整列出 ae/awb/proton/autokit/skel/engine/mipi/tcm/venc/ivps/sns_os04d10/audio… 照抄即可）

### 8.4 第三方库（离线 vendored，禁止联网拉取）
- SDK 自带：`<SDK>/third-party/{ffmpeg,fdk-aac,faac,opus,live,opencv,openssl,mp4,tinyalsa,libsamplerate}`（各含 `lib/`）
- 大华侧：`Z:\cp3prov1.2\Depend\Libs\{Platform,XS_SDK}`
- **GoogleTest**：离线放 `Depend/third_party/googletest`（若 SDK/系统无现成，从已有机器拷入；阶段 0 唯一外部依赖）。
- `Depend/ax_sdk` 用软链/路径变量指向 §8.3 的 SDK，**不拷贝整包**。

### 8.5 落地动作（阶段 1 起）
1. 写 `tools/build/toolchain-ax615-uclibc.cmake`。
2. `Depend/ax_sdk/ax_sdk_paths.cmake`：集中定义 SDK include/lib 路径变量（单一真值，禁散落）。
3. adapter 子库 `libmedia_ax615` / `libsys_ax615` 链接真实 `libax_*.a`，host stub 库保持不变可继续跑测试。

---

## 9. 云模块落地（阶段 2/3 依据，已读 PDF + 产品代码）

### 9.1 三份资产
| 资产 | 路径 | 用途 |
|---|---|---|
| 概要设计 PDF | `docs/腾讯云接入模块概要设计说明书 V1.0`（陈魁等，2025-08-21） | 协议/物模型/流程权威 |
| 产品化云代码 | `Z:\cp3prov1.2\Apps\Kylin\comps\Cloud`（79 文件） | `cloud_alarm/snap/live/playBack/PTZ/OTA/msg/storage/IOT` 等，配置与流程参考 |
| 腾讯云 SDK | `comps/Cloud/TencentCloud_SDK/`（v2.4.50） | 实际接入库 |

### 9.2 云模块四传输面（PDF §1.4）
1. **MQTT**：物模型（属性 / 行为 / 普通事件）——设备状态、电量、模式、告警上报走这里。
2. **P2P**：音视频（直播 / 回放 / 对讲）+ 自定义信令（PTZ 实时控制）。
3. **HTTP**：云存（图片云存=智能事件+图片；视频云存=事件+图片+视频）。
4. **配网 + OTA**：二维码 / AP（CoAP 10.10.0.1:5683） / 蓝牙辅助；OTA。

### 9.3 映射到 code_v2 分层（关键约束）
- `app/cloud` 只承载**业务语义**（物模型属性映射、告警/状态上报、预览请求路由），调用腾讯云 SDK 的封装。
- 腾讯云 SDK、P2P、MQTT 客户端等**实现细节收口在适配封装**（建议 `app/cloud/adapter_tencent/` 或 `Depend` 侧），`app/core` 不感知。
- **物模型生成**：PDF §2.1 指出 json 物模型经 python 脚本生成处理文件（`iv_user_func.c` 适配）——此生成物属于云适配层产物，不进 `app/core`。
- **抓图上云**：结合报警事件，按云存套餐状态选接口（PDF §2.2.1）——对应 `app/alarm`→`app/cloud` 链路，抓图来自**子码流**。
- **心跳**：PDF 配网段心跳 5s；设备在线/状态心跳归 `libsys/modem` 采集 + `app/cloud` 上报语义（与时序图 7 一致）。

### 9.4 复用边界提醒
`comps/Cloud` 是大华 Kylin 体系产品代码，**参考其业务流程与配置，不照搬框架**（同 Packet 原则）。腾讯云 SDK 本体可直接链接（它是第三方库，不是大华框架）。

---

## 9.5 状态上报与新鲜度约束（关键业务约束，已与硬件架构对齐）

### 硬件事实
- **主控 AX615**：AOV 休眠，靠定时 / 检测 / 远程唤醒醒来。
- **4G 模组**：自身也休眠，约每 1 分钟醒一次，仅维持「可被云端触达 / 收远程指令」。**模组每次醒不顺带唤醒主控**，只有收到明确远程指令（如 App 预览）才经 GPIO 拉醒主控。
- **电量 MCU**：常驻低功耗，2 路 ADC 采样，经 **I2C 上报给主控 AX615**。MCU 一直在测，但数据上云必须等主控醒着读 I2C + 走云会话。

### 核心结论：App 看到的状态不是实时的
设备状态（电量、工作模式、告警计数）**只能在主控 CPU 唤醒窗口上报**，是「上次唤醒时的快照」。新鲜度分三档：

| 信息 | 维护者 | 对云端新鲜度 |
|---|---|---|
| 在线 / 信号 / 网络 | 4G 模组（自身 ~1 分钟间歇周期） | ~1 分钟级，且本身间歇 |
| **电量** | MCU 常采 → I2C → **主控醒时才上报** | **主控上次唤醒快照** |
| **模式 / 告警** | 主控 | **主控上次唤醒快照** |

- **省电模式**：状态刷新跟随定时唤醒节奏（1/3/5/15 秒那次顺带上报）。
- **超长待机 / 低电保护**：主控不自动醒，状态可能长时间停在最后快照，直到 App 远程唤醒才刷新。

### 对设计的修正与落地
1. **修正设计文档时序图 7**：原文「modem 上报在线、信号、**电量**、网络」——**电量必须从 modem 这条拿掉**。模组不在 `MCU→I2C→主控` 链上，读不到电量。电量只能由主控（`libsys/power` 读 I2C）醒着时经 `app/cloud` 上报。
2. **「模组常电保在线」假设作废**：模组是 ~1 分钟间歇，连「在线」都不是连续的。
3. **数据结构**：`BatteryStatus` 增加 `sampled_at_ms`（采集时刻）；设备状态上云一律带采集时间戳，App 端按「截至 XX:XX」显示，不可呈现为实时。
4. **层映射**（现有接口不改，补实现约定）：
   - 电量 MCU = 常驻协处理器，主控经 **I2C（`libsys/device`）** 读寄存器。
   - 经 `IPowerService::GetBatteryStatus()` 出 `BatteryStatus`。
   - `app/cloud` 只在主控唤醒窗口上报，禁止假装实时心跳。

### 待硬件确认（不写死）
- **低电保护触发路径**：MCU 检测到电量跌破阈值时，能否**主动**中断/GPIO 唤醒主控立即进保护？还是只能等主控下次自醒时才评估？→ 待查原理图。
  - 若 MCU 不能主动唤醒，超长待机下存在「电量已低但主控很久不醒、迟迟不进保护」的电池过放风险，需在硬件确认后决定兜底策略（如模组 1 分钟周期兼做电量看门狗）。

---

## 10. 一句话总结

地基（entry + 测试）当天可补；真正的硬骨头是阶段 1 的 **wake 后出帧**（QSDemo 在这块板子上的已知断流点），它决定 AOV 主链路成败，应作为最高优先级风险对待。其余业务链路在接口冻结到位的前提下，是「填实现 + 写测试」的可控工作。**全员低功耗（主控/模组/MCU 各自间歇）决定了云端状态本质是快照而非实时——这是产品行为的硬约束，须在 App 交互与上报设计中显式体现。**



