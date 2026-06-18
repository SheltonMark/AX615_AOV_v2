# Packet Demo - 测试 libmedia → PacketService 链路

## 功能说明

验证 AOV 项目中 **libmedia 编码输出 → PacketService 分发** 的完整链路。

### 测试链路
```
AX_VENC → libmedia (RegisterStreamCallback) → PacketService.PublishFrame →
多消费者队列 (Storage/CloudStorage) → 文件输出
```

### 验证目标
1. ✅ **链路通畅**: StreamFrame 能正确传递到 PacketService
2. ✅ **多路分发**: 多个消费者能同时接收相同数据
3. ✅ **数据一致性**: 各消费者输出文件完全一致
4. ✅ **无丢帧**: 正常码率下 `dropped_frames == 0`
5. ✅ **数据正确**: 输出 H.265 文件能正常播放

---

## 编译

### 前置条件
```bash
# 1. 确保 libmedia.a 已编译
cd ~/AOV_AX615/code_v2/libmedia/Build
./build_ax615.sh

# 检查输出
ls -lh ~/AOV_AX615/code_v2/Depend/prebuilt/lib/libmedia.a
```

### 编译 packet_demo
```bash
cd ~/AOV_AX615/code_v2/test/packet_demo
./build_ax615.sh
```

**编译说明**：
- 链接 `libmedia.a`（已包含 VENC GetStream 线程）
- 链接 `app/packet/*.cpp`（PacketService 源码）
- 使用 ARM 交叉编译工具链

---

## 运行测试

### 基础测试（10秒，默认参数）
```bash
# 板子上
cd /tmp
./packet_demo

# 或使用脚本
./run_packet_demo.sh
```

### 自定义参数
```bash
./run_packet_demo.sh \
    --width 1920 \
    --height 1080 \
    --fps 15 \
    --bitrate 1024 \
    --gop 60 \
    --seconds 10
```

### 参数说明
| 参数 | 默认值 | 说明 |
|------|--------|------|
| `--width` | 1920 | 分辨率宽度 |
| `--height` | 1080 | 分辨率高度 |
| `--fps` | 15 | 帧率 |
| `--bitrate` | 1024 | 码率 (kbps) |
| `--gop` | 60 | GOP 大小 |
| `--seconds` | 10 | 运行时长（秒）|
| `--output-storage` | /tmp/packet_storage.h265 | 存储输出路径 |
| `--output-cloud` | /tmp/packet_cloud.h265 | 云端输出路径 |

---

## 验证结果

### 1. 控制台输出示例
```
========== 初始化 PacketService ==========
[OK] PacketService 初始化完成

========== 订阅消费者 ==========
[OK] 订阅 Storage 消费者
[OK] 订阅 CloudStorage 消费者

========== 注册码流回调 ==========
[OK] RegisterStreamCallback

========== 启动媒体管道 ==========
[OK] Runtime Init
[OK] Runtime Start

========== 开始采集（10秒）==========
[Storage] 帧#1 size=45231 keyframe=1 pts=0ms
[CloudStorage] 帧#1 size=45231 keyframe=1 pts=0ms
[进度] 1s 发布=15 Storage=15 Cloud=15 错误=0
[进度] 2s 发布=30 Storage=30 Cloud=30 错误=0
...

========== 验证结果 ==========
【PacketService 统计】
  发布帧数: 150
  投递帧数: 300        # 2个消费者 × 150帧
  丢弃帧数: 0
  订阅者数: 2

【消费者统计】
  Storage 收到: 150 帧, 3845120 bytes
  CloudStorage 收到: 150 帧, 3845120 bytes

【一致性检查】
  帧数一致: ✅
  字节一致: ✅
  发布一致: ✅
  无丢帧: ✅
  总体结果: ✅ 通过
```

### 2. 文件验证
```bash
# 板子上检查文件
ls -lh /tmp/packet_*.h265

# 拷贝到 PC
scp root@192.168.x.x:/tmp/packet_storage.h265 .
scp root@192.168.x.x:/tmp/packet_cloud.h265 .

# 验证文件一致性
md5sum packet_storage.h265 packet_cloud.h265
# 输出示例:
# a1b2c3d4... packet_storage.h265
# a1b2c3d4... packet_cloud.h265  # MD5 应该一致

# 播放验证
ffplay packet_storage.h265
```

### 3. 通过标准
- ✅ `published_frames == delivered_frames`
- ✅ `dropped_frames == 0`
- ✅ `storage_count == cloud_count`
- ✅ 两个文件 MD5 一致
- ✅ ffplay 能正常播放

---

## 常见问题

### Q1: 编译失败 - 找不到 libmedia.a
```
missing .../Depend/prebuilt/lib/libmedia.a
```
**解决**：先编译 libmedia
```bash
cd ~/AOV_AX615/code_v2/libmedia/Build
./build_ax615.sh
```

### Q2: 运行时段错误
**原因**：libmedia.a 可能是 x86 版本，不是 ARM 版本

**解决**：
```bash
# 清理并重新编译 libmedia
cd ~/AOV_AX615/code_v2/libmedia/Build
./build_ax615.sh clean
./build_ax615.sh

# 重新编译 packet_demo
cd ~/AOV_AX615/code_v2/test/packet_demo
./build_ax615.sh clean
./build_ax615.sh
```

### Q3: dropped_frames > 0
**原因**：消费者处理太慢或内存池不足

**调试**：
- 检查 CPU 占用
- 增大 `PacketConfig.max_bytes`
- 减少 fps 或 bitrate

### Q4: 两个文件 MD5 不一致
**原因**：PacketService 分发逻辑有问题

**调试**：
- 检查 `published_frames` vs `delivered_frames`
- 查看是否有 `PublishFrame failed` 错误

---

## 文件结构
```
test/packet_demo/
├── main.cpp              # 测试程序
├── build_ax615.sh        # 编译脚本
├── run_packet_demo.sh    # 运行脚本
├── README.md             # 本文档
└── build_ax615/          # 编译输出目录
    └── packet_demo       # 可执行文件
```

---

## 下一步

测试通过后，可以：
1. 集成到 `app/` 的主程序中
2. 添加更多消费者（CloudLive、Preview 等）
3. 测试背压场景（慢消费者）
4. 添加性能监控（CPU/内存）
