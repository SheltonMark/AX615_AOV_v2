#!/bin/sh

app=${APP:-./libmedia_demo}

# Align default values with the qsdemo run.sh profile that is known to run on
# OS04D10 single-sensor AOV boards.
aiisp_mode=0
gdc_enable=1
jenc_enable=1
vin_cap_num=0
link_mode=3,1
audio_enable=0
sleep_time=3000
chn_frame_mode=1
sd_mode=1
tisp_in_aov=0
audio_in_aov=0
venc_change_rc_policy=0,1.0
gop_in_aov=10
sns_ctrl_mode=3
bw_limit_level=0,4,0,0
ivps_sharpen=-1
interpolation_resolution=0,0
case_id=3

codec=h265
width=1920
height=1080
fps=15
bitrate=1024
gop=60
seconds=120
output=/tmp/libmedia_demo.h265
interactive=0
iq_bin=/opt/etc/os04d10_sdr_vs.bin
aiisp_model=/opt/etc/models/aiisp/OS04D10_SDR_S4-BASE-NOF_1440x2560_10b_HCG_ISP1_A1-2X_00001448915_260114_AX615.axmodel

usage() {
    echo "USAGE: $0 [options]"
    echo "  -x <case-id>       default 3, OS04D10 single"
    echo "  -o <output>        default /tmp/libmedia_demo.h265"
    echo "  -t <seconds>       default 20"
    echo "  -I                 interactive mode"
    echo "  -- <extra args>    pass extra args to libmedia_demo"
    echo
    echo "Example:"
    echo "  $0 -x 3 -o /tmp/os04d10.h265"
    echo "  $0 -I -- --codec h264 --output /tmp/os04d10.h264"
    exit 1
}

while getopts "x:o:t:Ih" opt; do
    case "$opt" in
        x) case_id="$OPTARG" ;;
        o) output="$OPTARG" ;;
        t) seconds="$OPTARG" ;;
        I) interactive=1 ;;
        h) usage ;;
        *) usage ;;
    esac
done
shift $((OPTIND - 1))

if [ "${1:-}" = "--" ]; then
    shift
fi

if [ ! -f "/proc/ax_proc/riscv/boot" ]; then
    echo "insmod riscv"
    insmod /soc/ko/ax_riscv_module.ko
    echo /opt/data/rtthread.bin >/proc/ax_proc/riscv/boot
fi

echo 0 > /proc/sys/kernel/printk

# Keep the same writeback tuning as qsdemo. This is mainly useful when output is
# written to SD/NAND during long AOV debug sessions.
echo 50 > /proc/sys/vm/dirty_writeback_centisecs
echo 100 > /proc/sys/vm/dirty_expire_centisecs
echo 1 > /proc/sys/vm/dirty_background_ratio
echo 1 > /proc/sys/vm/dirty_ratio
echo 1 > /proc/sys/vm/overcommit_memory
echo 896 > /proc/sys/vm/min_free_kbytes

interactive_arg=
if [ "${interactive}" = "1" ]; then
    interactive_arg="--interactive"
fi

exec ${app} \
    --case-id "${case_id}" \
    --aiisp-mode "${aiisp_mode}" \
    --gdc "${gdc_enable}" \
    --jenc "${jenc_enable}" \
    --vin-cap-num "${vin_cap_num}" \
    --link-mode "${link_mode}" \
    --sleep-time-ms "${sleep_time}" \
    --chn-frame-mode "${chn_frame_mode}" \
    --sd-mode "${sd_mode}" \
    --tisp-in-aov "${tisp_in_aov}" \
    --audio-in-aov "${audio_in_aov}" \
    --venc-rc-policy "${venc_change_rc_policy}" \
    --gop-in-aov "${gop_in_aov}" \
    --sns-ctrl-mode "${sns_ctrl_mode}" \
    --bw-limit-level "${bw_limit_level}" \
    --ivps-sharpen "${ivps_sharpen}" \
    --interpolation "${interpolation_resolution}" \
    --iq-bin "${iq_bin}" \
    --aiisp-model "${aiisp_model}" \
    --codec "${codec}" \
    --width "${width}" \
    --height "${height}" \
    --fps "${fps}" \
    --bitrate "${bitrate}" \
    --gop "${gop}" \
    --seconds "${seconds}" \
    --output "${output}" \
    ${interactive_arg} \
    "$@"
