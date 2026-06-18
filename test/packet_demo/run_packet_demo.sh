#!/bin/bash
# run_packet_demo.sh - 板子上运行 packet_demo 的脚本

set -e

DEMO_BIN="./packet_demo"

if [ ! -f "${DEMO_BIN}" ]; then
    echo "错误: 找不到 ${DEMO_BIN}"
    echo "请先运行: ./build_ax615.sh"
    exit 1
fi

echo "=========================================="
echo "  Packet Demo - libmedia → PacketService"
echo "=========================================="
echo ""

# 默认参数
WIDTH=1920
HEIGHT=1080
FPS=15
BITRATE=1024
GOP=60
SECONDS=10
OUTPUT_STORAGE="/tmp/packet_storage.h265"
OUTPUT_CLOUD="/tmp/packet_cloud.h265"

# 解析命令行参数
while [ $# -gt 0 ]; do
    case "$1" in
        --width)
            WIDTH="$2"
            shift 2
            ;;
        --height)
            HEIGHT="$2"
            shift 2
            ;;
        --fps)
            FPS="$2"
            shift 2
            ;;
        --bitrate)
            BITRATE="$2"
            shift 2
            ;;
        --gop)
            GOP="$2"
            shift 2
            ;;
        --seconds)
            SECONDS="$2"
            shift 2
            ;;
        --output-storage)
            OUTPUT_STORAGE="$2"
            shift 2
            ;;
        --output-cloud)
            OUTPUT_CLOUD="$2"
            shift 2
            ;;
        --help|-h)
            echo "用法: $0 [选项]"
            echo ""
            echo "选项:"
            echo "  --width N            宽度 (默认: 1920)"
            echo "  --height N           高度 (默认: 1080)"
            echo "  --fps N              帧率 (默认: 15)"
            echo "  --bitrate N          码率 kbps (默认: 1024)"
            echo "  --gop N              GOP (默认: 60)"
            echo "  --seconds N          运行时长秒 (默认: 10)"
            echo "  --output-storage PATH 存储输出 (默认: /tmp/packet_storage.h265)"
            echo "  --output-cloud PATH   云端输出 (默认: /tmp/packet_cloud.h265)"
            echo "  --help               显示帮助"
            exit 0
            ;;
        *)
            echo "未知参数: $1"
            echo "使用 --help 查看帮助"
            exit 1
            ;;
    esac
done

echo "【运行参数】"
echo "  分辨率: ${WIDTH}x${HEIGHT}"
echo "  帧率: ${FPS} fps"
echo "  码率: ${BITRATE} kbps"
echo "  GOP: ${GOP}"
echo "  时长: ${SECONDS} 秒"
echo "  存储输出: ${OUTPUT_STORAGE}"
echo "  云端输出: ${OUTPUT_CLOUD}"
echo ""

# 删除旧文件
rm -f "${OUTPUT_STORAGE}" "${OUTPUT_CLOUD}"

echo "【开始运行】"
"${DEMO_BIN}" \
    --width "${WIDTH}" \
    --height "${HEIGHT}" \
    --fps "${FPS}" \
    --bitrate "${BITRATE}" \
    --gop "${GOP}" \
    --seconds "${SECONDS}" \
    --output-storage "${OUTPUT_STORAGE}" \
    --output-cloud "${OUTPUT_CLOUD}"

EXIT_CODE=$?

echo ""
echo "=========================================="
if [ ${EXIT_CODE} -eq 0 ]; then
    echo "✅ 测试通过"
else
    echo "❌ 测试失败 (退出码: ${EXIT_CODE})"
fi
echo "=========================================="
echo ""

if [ -f "${OUTPUT_STORAGE}" ] && [ -f "${OUTPUT_CLOUD}" ]; then
    echo "【文件验证】"
    echo "Storage 文件:"
    ls -lh "${OUTPUT_STORAGE}"
    echo ""
    echo "Cloud 文件:"
    ls -lh "${OUTPUT_CLOUD}"
    echo ""

    echo "MD5 校验:"
    md5sum "${OUTPUT_STORAGE}"
    md5sum "${OUTPUT_CLOUD}"
    echo ""

    STORAGE_MD5=$(md5sum "${OUTPUT_STORAGE}" | awk '{print $1}')
    CLOUD_MD5=$(md5sum "${OUTPUT_CLOUD}" | awk '{print $1}')

    if [ "${STORAGE_MD5}" = "${CLOUD_MD5}" ]; then
        echo "✅ 文件一致性验证通过"
    else
        echo "❌ 文件不一致"
    fi
    echo ""

    echo "【播放验证】"
    echo "在 PC 上运行:"
    echo "  scp root@192.168.x.x:${OUTPUT_STORAGE} ."
    echo "  ffplay $(basename ${OUTPUT_STORAGE})"
fi

exit ${EXIT_CODE}
