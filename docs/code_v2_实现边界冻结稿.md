# code_v2 实现边界冻结稿

## 1. 文档目的

这份文档用于冻结 `code_v2` 在进入正式编码前的模块边界、依赖方向和实现禁区。

目标不是把所有细节一次定死，而是解决三个问题：

- 后续代码该放在哪一层
- 哪些能力允许直接依赖，哪些不允许
- QSDemo 与旧版 `code/` 应该如何利用，而不是如何照搬

本文件优先级高于后续个人习惯和临时写法。若实现与本文件冲突，应先改设计，再改代码。

---

## 2. 当前共识

### 2.1 三套东西的定位

- `QSDemo`
  - 真实链路参考
  - 重点参考 AOV 状态机、检测触发、媒体链恢复、低功耗切换、录像/RTSP/OSD 的真实调用顺序
- 旧版 `code/`
  - 过渡性分层参考
  - 重点参考命名方式、接口抽象方向、宿主侧/板端差异处理思路
- `code_v2`
  - 正式产品化主线
  - 后续新功能、新接口、新目录都应优先落在这里

### 2.2 总体原则

- `app` 管业务编排，不直接落 AX SDK 细节
- `libmedia` 管媒体链，不承载业务策略
- `libsys` 管系统与硬件，不承载云业务协议
- 4G 模组链路属于 `libsys/modem`
- 云交互协议属于 `app/cloud`
- AX SDK 直接调用只能出现在 `libmedia/adapter/ax615` 和 `libsys` 的实现侧

---

## 3. 模块边界矩阵

## 3.1 `app/core`

**职责**
- AOV 主状态机
- 工作模式流转
- 唤醒/休眠决策
- 协调 `app/cloud`、`app/preview`、`app/record`、`app/alarm`
- 协调 `libsys` 与 `libmedia`

**允许依赖**
- `app/config`
- `app/cloud`
- `app/preview`
- `app/record`
- `app/alarm`
- `libsys` 对外接口
- `libmedia` 对外接口

**禁止事项**
- 禁止直接包含 AX SDK 头文件
- 禁止直接操作 GPIO / I2C / PWM / TCM
- 禁止直接写 RTSP / VENC / VIN 控制细节
- 禁止直接处理云端协议报文细节

## 3.2 `app/cloud`

**职责**
- 云端业务协议接入
- App 请求路由
- 状态上报 / 告警上报 / 配置同步
- 预览请求编排
- 与 `libsys/modem` 协作完成远程唤醒业务闭环

**允许依赖**
- `app/core`
- `app/config`
- `libsys/modem` 对外接口

**禁止事项**
- 禁止直接拉 GPIO 唤醒脚
- 禁止直接实现 4G 模组驱动细节
- 禁止直接控制媒体链

**边界说明**
- `app/cloud` 负责“云上发生了什么、业务要怎么响应”
- `libsys/modem` 负责“4G 模组怎么收发、怎么触发硬件唤醒”

## 3.3 `app/config`

**职责**
- 装载默认配置
- 管理运行时配置
- 校验配置合法性
- 将配置分发给 `app/core` / `libsys` / `libmedia`

**允许依赖**
- `products/` 配置文件
- `libsys/storage` 对外接口

**禁止事项**
- 禁止直接改媒体链内部对象
- 禁止直接改 GPIO / 4G / 云台
- 禁止承载业务状态机

## 3.4 `app/preview`

**职责**
- 预览会话生命周期
- App 预览请求与媒体预览能力对接
- 预览开始/停止与 AOV 状态协同

**允许依赖**
- `app/core`
- `app/cloud`
- `libmedia/preview` 对外接口

**禁止事项**
- 禁止自己决定系统休眠策略
- 禁止直接操作 AX VENC / RTSP 细节

## 3.5 `app/record`

**职责**
- 录像策略
- 事件录像片段管理
- 录像索引与元数据管理

**允许依赖**
- `app/core`
- `libmedia/record` 对外接口
- `libsys/storage` 对外接口

**禁止事项**
- 禁止直接操作底层存储挂载
- 禁止直接写编码器细节

## 3.6 `app/alarm`

**职责**
- 报警类型策略
- 检测结果到报警事件的转换
- 报警事件与录像/上报联动

**允许依赖**
- `app/core`
- `app/cloud`
- `app/record`
- `libmedia/detect` 对外接口

**禁止事项**
- 禁止直接实现检测模型逻辑
- 禁止直接实现媒体抓帧逻辑

## 3.7 `libmedia`

**职责**
- camera / vin / ivps / venc / jenc / rtsp
- detect frame path
- OSD
- preview / record 的媒体输出
- 媒体级低功耗切换：fps / cap frame / idr / preview 恢复

**允许依赖**
- `libmedia/adapter/ax615`
- `libsys` 提供的少量系统能力接口（如时间、文件、线程），仅在必要时使用

**禁止事项**
- 禁止承载云协议
- 禁止承载业务状态机
- 禁止自行决定是否进入 AOV / 常电模式

**边界说明**
- `libmedia` 只回答“媒体链怎么做”
- 不回答“现在业务上应不应该做”

## 3.8 `libmedia/adapter/ax615`

**职责**
- AX615 媒体 SDK 隔离
- 吸收 `QSDemo/src/com`、`QSDemo/src/osd` 中可复用的实现经验
- 对上层输出稳定接口

**允许依赖**
- AX615 SDK
- QSDemo 行为参考

**禁止事项**
- 禁止把业务状态机继续塞回来
- 禁止把路径、产品默认配置硬编码进适配层

## 3.9 `libsys/power`

**职责**
- 系统级 sleep / wakeup timer / wake event
- battery / work mode
- 唤醒原因采集

**允许依赖**
- SoC 低功耗相关接口
- `libsys/os`
- `libsys/device`

**禁止事项**
- 禁止承载云协议
- 禁止承载媒体恢复细节

## 3.10 `libsys/modem`

**职责**
- 4G 模组链路状态管理
- 4G 模组心跳承接
- 模组在线状态采集
- GPIO 拉脚唤醒主控
- 模组事件与 `app/cloud` / `app/core` 对接

**允许依赖**
- `libsys/device`
- `libsys/network`
- `libsys/os`

**禁止事项**
- 禁止承载云端业务协议解释
- 禁止直接控制媒体链恢复顺序

**边界说明**
- `libsys/modem` 负责“模组和硬件行为”
- `app/cloud` 负责“云消息和业务含义”

## 3.11 `libsys/device`

**职责**
- gpio / i2c / pwm / motor / led / 云台
- 通用硬件外设控制

**禁止事项**
- 禁止承载业务状态
- 禁止直接做云交互

## 3.12 `libsys/network`

**职责**
- 网络在线状态
- 基础联网能力
- 为 `libsys/modem` 和 `app/cloud` 提供链路状态支撑

**禁止事项**
- 禁止承载 App/云协议语义

## 3.13 `libsys/storage`

**职责**
- 挂载、目录、空间检查、文件系统基础能力
- 屏蔽 `/tfcard`、`/mnt/sdcard` 这类路径差异

**禁止事项**
- 禁止承载录像业务策略
- 禁止直接感知“这是报警录像还是普通录像”

## 3.14 `products/`

**职责**
- 工厂默认配置
- SKU 差异配置
- 配置 schema / profile

**禁止事项**
- 禁止写运行时逻辑
- 禁止在这里放 C/C++ 业务代码

---

## 4. 依赖方向冻结

允许的主要依赖方向如下：

```text
products -> app/config

app/core -> app/cloud / app/config / app/preview / app/record / app/alarm
app/core -> libsys / libmedia

app/cloud -> libsys/modem
app/preview -> libmedia/preview
app/record -> libmedia/record + libsys/storage
app/alarm -> libmedia/detect + app/cloud + app/record

libmedia -> libmedia/adapter/ax615
libsys/* -> 各自实现层
```

禁止反向依赖：

- `libmedia` 不能依赖 `app/*`
- `libsys` 不能依赖 `app/*`
- `libmedia` 不能直接依赖 `app/cloud`
- `libsys/modem` 不能解释云协议业务含义
- `app/*` 不能直接 include AX SDK 头文件

---

## 5. QSDemo 与旧版 `code/` 的使用规则

## 5.1 QSDemo 怎么用

QSDemo 只作为“真实链路参考”，优先吸收以下内容：

- `qsdemo.c`
  - AOV 状态流转
  - 检测与休眠/唤醒切换
  - 媒体链恢复时序
- `src/com`
  - 媒体通道组织方式
  - AX615 媒体接口调用顺序
- `src/osd`
  - OSD 能力组织方式
- AOV / TCM / timer / wakeup 相关逻辑
  - 作为 `libsys/power` + `libmedia` 的实现参考

禁止做法：

- 禁止把 `qsdemo.c` 的大函数直接拆几刀后搬进 `code_v2`
- 禁止继续形成“一个文件管所有事情”的组织方式

## 5.2 旧版 `code/` 怎么用

旧版 `code/` 只作为“分层和命名参考”，可以吸收：

- 接口命名
- 宿主/板端双态适配思路
- 分层方向是否合理

禁止做法：

- 禁止把旧版 `code/` 当正式主线继续膨胀
- 禁止为了复用一点代码，把 `code_v2` 再拉回旧架构

---

## 6. 三条关键链路的边界定义

## 6.1 定时唤醒 -> 轻量检测 -> 恢复常电

- `libsys/power`
  - 负责定时唤醒动作与唤醒原因上报
- `libmedia/detect`
  - 负责轻量检测所需取帧与检测输入
- `app/core`
  - 负责根据检测结果决定是否恢复常电
- `libmedia`
  - 负责恢复编码、RTSP、IDR、录像相关媒体动作

## 6.2 App 远程预览唤醒

- 手机 App 先请求云端
- 云端向设备侧 4G 模组下发唤醒指令
- `libsys/modem`
  - 接收模组事件
  - 通过 GPIO 拉脚唤醒主控
- `app/core`
  - 感知唤醒原因，决定进入预览恢复流程
- `libmedia`
  - 恢复预览所需媒体链
- `app/preview`
  - 建立预览会话
- `app/cloud`
  - 回传预览状态给云端 / App

## 6.3 4G 模组主动心跳

- `libsys/modem`
  - 承接模组在线状态与心跳基础能力
- `app/cloud`
  - 负责业务状态整合与上报语义
- 云端
  - 维护设备在线状态
  - 反馈给手机 App

---

## 7. 实现禁区

以下做法在 `code_v2` 中默认禁止：

- 在 `app/*` 中直接调用 AX SDK
- 在 `libmedia` 中写 AOV 业务状态机
- 在 `libsys/modem` 中写云协议 JSON/私有协议业务语义
- 在 `app/cloud` 中直接拉 GPIO 或直接访问 4G 驱动
- 在多个模块重复维护工作模式、电量阈值、录像路径、唤醒原因字段
- 把板端路径常量散落在多个目录中
- 没有接口边界就直接从业务层跳到底层实现

---

## 8. 第一阶段实现顺序建议

在真正写功能前，建议先按下面顺序推进：

1. 冻结对外接口命名
   - `app/core`
   - `app/cloud`
   - `libmedia`
   - `libsys/power`
   - `libsys/modem`
2. 冻结三条关键时序
   - 定时唤醒检测
   - App 远程预览唤醒
   - 4G 心跳状态反馈
3. 再开始写最小骨架
   - 只写接口、事件、状态、返回码
   - 暂不急着落 AX615 具体实现
4. 最后再把 QSDemo 行为逐步映射进去

---

## 9. 后续实现判断标准

以后每次准备写一个新文件时，先回答四个问题：

1. 这个逻辑属于业务决策、媒体能力，还是系统/硬件能力？
2. 这个模块是否需要知道云协议语义？
3. 这个模块是否需要知道 AX SDK 细节？
4. 如果未来换芯片 / 换 4G 模组 / 换云协议，这个文件应不应该跟着一起大改？

如果答案混乱，说明边界选错了，不应立即编码。
