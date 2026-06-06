# code_v2 正式目录树

## 1. 设计目标

`code_v2` 作为产品化版本，目标不是继续堆叠 QSDemo 适配代码，而是形成 **长期可维护、可替换芯片 SDK、可复用量产业务能力** 的设备端工程结构。

设计原则：

- `app` 负责业务编排、云交互、配置中心
- `libmedia` 负责媒体链能力，并通过 `adapter` 隔离 SDK
- `libsys` 负责系统、低功耗、硬件控制、网络/4G 模组、电池、云台等基础能力
- `Depend` 统一管理 SDK、第三方库、预编译依赖
- 默认配置与产品 SKU 配置独立存放，支持后续 App/云端下发覆盖
- 旧版 `code/` 保留，作为 QSDemo 行为和接口映射参考

---

## 2. 正式目录树

```text
code_v2/
├─ app/
│  ├─ entry/
│  ├─ core/
│  ├─ cloud/
│  ├─ config/
│  ├─ alarm/
│  ├─ record/
│  ├─ preview/
│  └─ common/
│
├─ libmedia/
│  ├─ include/
│  ├─ interface/
│  ├─ adapter/
│  │  └─ ax615/
│  ├─ pipeline/
│  ├─ preview/
│  ├─ record/
│  ├─ detect/
│  ├─ osd/
│  ├─ rtsp/
│  └─ common/
│
├─ libsys/
│  ├─ include/
│  ├─ power/
│  ├─ device/
│  ├─ network/
│  ├─ storage/
│  ├─ os/
│  └─ common/
│
├─ Depend/
│  ├─ ax_sdk/
│  ├─ third_party/
│  ├─ prebuilt/
│  └─ toolchain/
│
├─ products/
│  ├─ common/
│  └─ ax615_default/
│     ├─ factory/
│     ├─ schema/
│     └─ profile/
│
├─ tests/
│  ├─ unit/
│  ├─ integration/
│  └─ mock/
│
├─ tools/
│  ├─ build/
│  ├─ deploy/
│  ├─ pack/
│  └─ debug/
│
├─ docs/
│
└─ CMakeLists.txt
```

---

## 3. 顶层模块职责

### 3.1 `app/`

设备产品业务层，负责：

- AOV 主状态机与业务编排
- 云端交互与 App 请求路由
- 远程预览唤醒链路编排
- 配置管理与参数下发
- 报警策略与录像策略
- 预览会话管理
- 调用 `libmedia` / `libsys` 执行动作

### 3.2 `libmedia/`

媒体库，负责：

- camera / vin / ivps / venc / jenc / rtsp
- 检测取帧与编码输出
- OSD
- 录像媒体输出
- 预览媒体输出
- 一层 `adapter/ax615` 隔离 AX SDK

说明：
- `QSDemo/src/com`
- `QSDemo/src/osd`
后续都可以作为 `libmedia` 的参考来源。

### 3.3 `libsys/`

系统与硬件基础库，负责：

- 低功耗控制：sleep / wakeup timer / wake event
- battery / work mode
- network / keepalive 基础能力
- 4G 模组管理：链路状态、GPIO 唤醒脚、模组在线状态
- gpio / i2c / pwm / motor / led
- mount / file / time / thread / reboot
- 本地设备硬件控制

说明：
- **低功耗控制不放在 app，也不全放在 libmedia**
- `app` 决定“何时睡 / 何时醒”
- `libsys` 执行系统级睡眠唤醒动作，并承接 4G 模组 / GPIO 拉脚唤醒
- `libmedia` 执行媒体链相关的低功耗动作（fps、cap frame、preview、idr 等）

### 3.4 `Depend/`

统一管理依赖，负责：

- AX SDK 头文件与库
- 第三方库
- 预编译二进制
- toolchain 说明与封装

目标：避免业务层和构建脚本到处散落 SDK 依赖路径。

### 3.5 `products/`

产品配置层，负责：

- 工厂默认配置
- 产品 SKU 差异配置
- 配置 schema
- 运行 profile

这里放的是“默认值与模板”，不是运行时逻辑。

---

## 4. 二级目录建议

### 4.1 `app/`

- `entry/`
  - 程序入口、bootstrap、依赖装配
- `core/`
  - `AovService`、`DeviceService`、主业务编排
- `cloud/`
  - 设备与云 / App 的交互逻辑
  - 业务状态同步、命令下发、预览请求、参数配置同步
  - 与 `libsys/modem` 协同完成远程唤醒与模组状态回传
- `config/`
  - 默认配置装载、运行配置、持久化配置、配置应用分发
- `alarm/`
  - 报警类型与策略（人形 / 宠物 / 其他）
- `record/`
  - 录像策略、索引、事件片段管理
- `preview/`
  - App 预览会话与唤醒逻辑
- `common/`
  - 公共模型、错误码、工具

### 4.2 `libmedia/`

- `include/`
  - 对外公开接口
- `interface/`
  - 纯抽象接口定义
- `adapter/ax615/`
  - AX615 SDK 适配层
- `pipeline/`
  - 媒体链装配
- `preview/`
  - 预览输出
- `record/`
  - 编码输出、录像数据通路
- `detect/`
  - 检测取帧与输入输出
- `osd/`
  - 叠加能力
- `rtsp/`
  - 预览流输出
- `common/`
  - 媒体公共结构

### 4.3 `libsys/`

- `include/`
  - 对外公开接口
- `power/`
  - sleep、wake、timer、电池、工作模式
- `device/`
  - gpio / i2c / pwm / motor / led / 云台
- `network/`
  - 网络状态、联网保活基础能力
- `modem/`
  - 4G 模组管理、模组链路状态、GPIO 唤醒脚控制、模组在线状态采集
- `storage/`
  - mount、空间检查、目录、文件系统基础封装
- `os/`
  - 线程、锁、时间、事件、进程、重启
- `common/`
  - 公共结构与工具

---

## 5. 关键边界约束

### 5.1 cloud 放在哪里

建议放在 `app/cloud`，不单独做成顶层 `libcloud`：

- cloud 是产品业务接入面
- 不属于通用底层库
- 不应该直接调用 SDK
- 必须由 `app` 路由到 `libmedia` / `libsys`
- 4G 模组链路、GPIO 拉脚唤醒、模组心跳不放在 `app/cloud`，而放在 `libsys/modem`

### 5.2 低功耗控制放在哪里

建议拆成三层：

- `app`
  - 决策何时进入睡眠、何时唤醒、何时恢复常电
- `libsys/power + libsys/modem`
  - 执行 `sleep / wakeup timer / wake event / battery / gpio wake / 4G wake event`
- `libmedia`
  - 执行媒体链相关的低功耗切换：fps、cap frame、preview、idr、detect frame path

### 5.3 配置项如何生效

配置流建议为：

1. `products/.../factory` 提供默认配置
2. `app/config` 读取默认配置并叠加本地持久化配置
3. 云端 / App 下发新配置到 `app/cloud`
4. `app/config` 校验并更新运行配置
5. `app` 按配置类型分发给：
   - `libmedia`：码流、分辨率、OSD、检测参数等
   - `libsys`：工作模式、电量阈值、云台、补光灯、设备控制、4G 模组行为等
6. 必要时持久化到本地配置存储

### 5.4 App 远程预览唤醒链路

建议链路为：

1. 手机 App 发起预览请求到云端
2. 云端向设备侧 4G 模组下发唤醒指令
3. `libsys/modem` 接收模组事件，并通过 GPIO 拉脚唤醒主控
4. `app/core` 感知唤醒原因，协调 `libsys/power` 与 `libmedia` 恢复常电媒体链
5. `app/preview` 建立预览会话，`app/cloud` 同步会话状态到云端 / App

### 5.5 4G 心跳与状态反馈链路

建议链路为：

1. 设备侧 4G 模组周期性向云端主动上报心跳
2. 云端维护设备在线状态、基础状态与唤醒可达性
3. 云端把设备状态反馈给手机 App
4. `app/cloud` 可按需消费模组在线状态、业务状态与唤醒结果

---

## 6. 与旧版 `code/` 的关系

- `code/` 保留，不删除
- 用作：
  - QSDemo 行为参考
  - 旧版架构参考
  - AX615 / QSDemo 接口映射参考
- `code_v2/` 作为正式产品化版本推进

---

## 7. 当前阶段建议

当前阶段建议先做：

- 固化 `code_v2` 目录边界
- 固化大模块职责
- 输出大模块交互图
- 之后再开始逐步落代码骨架

不建议当前阶段继续细拆到过多类和过多子模块，以免再次形成过度设计。

