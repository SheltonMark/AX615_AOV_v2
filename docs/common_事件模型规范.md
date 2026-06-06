# Common 事件模型规范

## 1. 目标

统一 `code_v2` 的事件表达方式，避免不同模块各自发明事件结构，导致状态机输入难以收敛。

---

## 2. 事件分类

建议第一阶段把事件分成四类：

- 电源/系统事件
- 媒体检测事件
- 云/App 业务事件
- 会话/录像事件

---

## 3. 统一事件头

```cpp
struct EventHeader {
    std::uint64_t timestamp_ms {0};
    std::uint32_t sequence {0};
};
```

说明：
- 所有事件都应带时间戳
- 需要跨线程排查时，可用 `sequence` 追踪顺序

---

## 4. 建议事件定义

## 4.1 电源/系统事件

```cpp
struct BatteryStatusChangedEvent {
    EventHeader header;
    BatteryStatus status;
};

struct WakeupTriggeredEvent {
    EventHeader header;
    WakeupEvent wakeup;
};
```

## 4.2 检测事件

```cpp
struct DetectCompletedEvent {
    EventHeader header;
    DetectResultSummary result;
};
```

## 4.3 云/App 事件

```cpp
struct PreviewRequestedEvent {
    EventHeader header;
    PreviewRequest request;
};

struct ConfigUpdatedEvent {
    EventHeader header;
};
```

## 4.4 业务结果事件

```cpp
struct AlarmTriggeredEvent {
    EventHeader header;
    AlarmEvent alarm;
};
```

---

## 5. 事件传递原则

- `libsys` / `libmedia` 只上送事实事件
- `app/core` 消费事实事件并作出决策
- `app/*` 子域消费决策结果或业务事件

即：
- 事件层不直接承载“应该怎么办”的结论
- 决策只放在 `app/core`

---

## 6. 事件命名原则

统一使用过去式或完成式，表达“事实已经发生”：

- `BatteryStatusChangedEvent`
- `WakeupTriggeredEvent`
- `DetectCompletedEvent`
- `PreviewRequestedEvent`
- `AlarmTriggeredEvent`

避免使用含糊命名：

- `PowerEvent`
- `ActionEvent`
- `BusinessEvent`

---

## 7. 下一步落地建议

落代码时优先提供：

- `common/event_header.hpp`
- `app/core/aov_events.hpp`
