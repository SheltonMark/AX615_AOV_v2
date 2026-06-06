# Common 回调与线程模型规范

## 1. 目标

统一 `code_v2` 中回调使用方式和线程边界，避免后面出现：

- 回调在哪个线程执行不清楚
- 回调里直接做重逻辑
- 多层回调相互嵌套导致状态混乱

---

## 2. 回调原则

统一原则：

- 底层模块只负责“通知事件发生”
- 回调函数不直接做重业务处理
- 重业务处理必须回到 `app/core` 或对应业务线程

---

## 3. 推荐回调类型

建议统一别名：

```cpp
using VoidCallback = std::function<void()>;

template <typename T>
using EventCallback = std::function<void(const T&)>;
```

适用场景：
- `IPowerService::RegisterWakeupCallback(EventCallback<WakeupTriggeredEvent>)`
- `IDetectService::RegisterDetectCallback(EventCallback<DetectCompletedEvent>)`

---

## 4. 线程归属约束

### 4.1 `libsys`

- 可在模块自己的工作线程/中断转发线程触发回调
- 但必须文档化：回调是否跨线程

### 4.2 `libmedia`

- 检测、预览、录像回调可能来自媒体线程
- 不允许在媒体线程里直接做云交互或复杂业务决策

### 4.3 `app/core`

- 建议有统一事件消费线程
- 所有跨模块事件最终汇聚到该线程决策

---

## 5. 禁止事项

- 禁止在回调里直接阻塞等待网络
- 禁止在回调里直接做复杂磁盘 IO
- 禁止在回调里递归触发新的状态切换
- 禁止默认假设回调一定在主线程

---

## 6. 推荐模型

建议第一阶段采用：

- 底层模块：产生事件
- 事件队列：转交事件
- `app/core`：单线程消费决策

即使后面不实现完整事件总线，设计上也要按这个方向约束。

---

## 7. 下一步落地建议

落代码时优先提供：

- `common/callback.hpp`
- `common/thread_model.md` 或对应注释约定
