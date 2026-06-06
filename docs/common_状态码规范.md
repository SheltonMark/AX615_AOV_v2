# Common 状态码规范

## 1. 目标

统一 `code_v2` 的状态码设计，避免每个模块各定义一套返回值，导致跨层判断混乱。

原则：

- 所有对外接口必须返回显式状态码
- 状态码语义稳定，不能混入日志文本
- 模块级状态码可保留，但需要能映射到统一基础状态码

---

## 2. 基础状态码

建议全局统一基础状态码：

```cpp
enum class StatusCode {
    Ok,
    InvalidArgument,
    InvalidState,
    NotFound,
    Busy,
    Timeout,
    Unsupported,
    NoResource,
    AlreadyExists,
    InternalError,
};
```

说明：
- `Ok`：成功
- `InvalidArgument`：参数非法
- `InvalidState`：当前状态不允许执行
- `NotFound`：对象/配置/会话不存在
- `Busy`：资源正忙
- `Timeout`：超时
- `Unsupported`：当前版本/模式不支持
- `NoResource`：资源不足，如存储空间/句柄不足
- `AlreadyExists`：重复创建
- `InternalError`：未分类内部错误

---

## 3. 模块返回值策略

建议：

- `app/*`
  - 直接使用 `StatusCode`
- `libsys/*`
  - 内部可有模块细分错误，但对上层统一映射为 `StatusCode`
- `libmedia/*`
  - 内部可有 AX615 错误码，但对上层统一映射为 `StatusCode`

即：
- SDK 原生错误码不允许直接穿透到 `app/core`
- 板端细节错误不允许污染业务层接口

---

## 4. 需要保留细节错误时的做法

当上层既要统一返回值，又要保留底层细节时，建议使用：

```cpp
struct ErrorInfo {
    StatusCode code;
    int vendor_code;
    std::string message;
};
```

说明：
- `code`：统一错误码
- `vendor_code`：底层 AX615 / modem / OS 原始错误码
- `message`：调试文本，不用于业务逻辑判断

---

## 5. 接口设计约束

以下接口不建议只返回 `bool`：

- 启停类接口
- 配置类接口
- 状态切换类接口
- 资源申请类接口

统一要求返回：

```cpp
StatusCode
```

查询类可以返回值本身，或：

```cpp
StatusCode + out param
```

---

## 6. 与日志的关系

- 日志用于诊断
- `StatusCode` 用于控制流
- 不能通过解析日志决定业务流程

---

## 7. 下一步落地建议

落代码时优先提供：

- `common/status_code.hpp`
- `common/error_info.hpp`
