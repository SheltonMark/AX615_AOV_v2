#!/bin/sh
app=qsdemo

# (a) aiisp mode
# 0: TISP
# 1: AIISP default
# 2: Manual AIISP
# 3: Manual TISP
# 4: Auto AIISP
aiisp_mode=0

# (b) enable gdc
# 0: disable
# 1: enable
gdc_enable=0

# (c) Reboot Interval from main lanch
# unit sencond
# 0: reboot if no body found 5fps
# >0: reboot if timeout n seconds
# need insert sd card
reboot_interval=0

# (d) sdr/hdr switch times
# 0: no switch
# >0: switch N times, interval 60s
sdrhdr_switch_test=0

# (e) if enable jenc
# 1: enable
# 0: disable and not init jenc
jenc_enable=1

# (f) set aov vin cap num when wakeup
# 0: not set, default 1fps
# 1~3: cap num
# 4: for test, random switch between 1~3
vin_cap_num=0

# (g) link mode (vin-->ivps),(ivps-->venc)
# 0:FLOW_MIN
# 1:SW_LOW_MEM
# 2:SW_LOW_LATENCY
# 3:SW_OFFLINE
# 4:HW_ONLINE
link_mode=3,1

# (i) audio flag
# 0: disable
# 1: enable audio capture
# 2: enable audio play
# 3: enable audio capture and play
audio_enable=0

# (j) rtsp enable
# 0: disable
# 1: enable
rtsp_enable=1

# (k) aov one cycle total time
# unit ms
sleep_time=1000

# (l) vin channel frame mode
# 0: off
# 1: ring
chn_frame_mode=1

# (m) sd offline mode
# 0: offline mode
# 1: online mode
sd_mode=1

# (n) gdc online vpp test
# 0: disable
# 1: enable
gdconlinevpp_test=0

# (o) use tisp in aov (1fps)
# 0: disable
# 1: enable
tisp_in_aov=0

# (p) store audio capture in aov
# 0: disable
# 1: enable
audio_in_aov=0

# (q) change venc rc policy in aov (policy,value)
# -1: no change
# 0: change bitrate (ratio)
# 1: change gop
venc_change_rc_policy=0,1.0

# (r) max record file count
max_record_file_count=5

# (s) venc gop in aov
gop_in_aov=10

# (t) ezoom switch info (x,y,w,h,cx,cy)
ezoom_info=422,278,922,518,-78,-4

# (u) slow shutter
# -1: use default setting
# 0 : off
# 1 : on
slow_shutter=-1

# (v) ezoom test
# 0 : off
# 1 : on
ezoom_test=0

# (w) ae manual shutter (us)
# -1 : default
# >0 : manual
ae_manual_shutter=-1

# (x) case id
# 0: sc438hai single
# 1: sc533hai single
# 2: mis20s1 single
# 3: os04d10 single
# 12: dummy single
# 13: sl465sl single
# 200: sc438hai dual
# 202: mis20s1 dual
# 203: os04d10 dual
# 213: sl465sl dual
case_id=0

# (y) light info
# format: enable,pipeId,light_Id,Sensitivity,mode,ctrl_mode,(NORMAL/AOV)[level]/[level,duty_pct,freq_hz]/[duration_ms,interval_ms]
# enable: 0-1
# light_Id: gpio_num/pwm_num
# Sensitivity 1-100
# mode:0:const_mode(not support) 1:pwm_mode 2:strobe_mode(not support)
# ctrl_mode:0:always_on 1:delay_on_instant_off 2:delay_on_delay_off
# level:0-1000
# duty_pct:0-100
# freq_hz:1-100
# pwm_mode(always_on)    ep: 1,0,11,80,1,0,100,80,100,100,20,100
# pwm_mode(delay_on_instant_off)    ep: 1,0,11,80,1,1,100,80,100,100,20,100
# pwm_mode(delay_on_delay_off)    ep: 1,0,11,80,1,2,100,80,100,100,20,100
light_info=0,0,11,80,1,0,100,80,100,100,20,100

# (z) sns aov ctrl mode
# -1: default
# 0: not init
# 1: reg
# 2: gpio
# 3: both
sns_ctrl_mode=-1

# (A) bw limit level format: npu0(normal),npu1(normal),npu0(aov),npu1(aov)
# -1: default(please all value set -1)
# 0: AX_AIISP_BW_LIMIT_DISABLE
# 1: AX_AIISP_BW_LIMIT_LEVEL1
# 2: AX_AIISP_BW_LIMIT_LEVEL2
# 3: AX_AIISP_BW_LIMIT_LEVEL3
# 4: AX_AIISP_BW_LIMIT_LEVEL4
bw_limit_level=0,4,0,0

# (B) ivps sharpen
# -1: default
# 0: disable
# 1: enable
ivps_sharpen=-1

# (C) interpolation resolution(only main chan) : w,h
# 0:  disable
# >0: ep(2M-->3M): 2304,1296;(2M-->4M): 2560,1440; (4M-->5M): 2880,1620
interpolation_resolution=0,0

function usage() {
  echo "USAGE: $0 -x <case-id>"
  echo " e.g.: $0 -x 3              -- start with os04d10 single"
  echo " ---------------------------------------------------------------------------------------------------------------------------"
  echo " Command details:"
  echo "   x: case id."
  echo "      0: sc438hai single(DEFAULT)"
  echo "      1: sc533hai single"
  echo "      2: mis20s1 single"
  echo "      3: os04d10 single"
  echo "      7: sc235hai single"
  echo "     12: dummy single"
  echo "     13: sl465sl single"
  echo "    200: sc438hai dual"
  echo "    202: mis20s1 dual"
  echo "    203: os04d10 dual"
  echo "    207: sc235hai dual"
  echo "    213: sl465sl dual"
  exit 1;
}

while getopts "x:h" opt
do
   case "$opt" in
      x ) case_id="$OPTARG" ;;
      h ) usage ;;
   esac
done

# update AE user log level
# echo ulog 42 5 > /proc/ax_proc/logctl
# sed -i 's/\(AE[[:space:]]\+42[[:space:]]\+\)[0-9]/\15/' /var/log/AXSyslog/ax_syslog.conf

if [ ! -f "/proc/ax_proc/riscv/boot" ]; then
   echo "insmod ricv"
   insmod /soc/ko/ax_riscv_module.ko
   echo /opt/data/rtthread.bin >/proc/ax_proc/riscv/boot
fi

# echo 0 > /sys/module/printk/parameters/console_suspend

# disable hsk for vin
# echo ife_itp_hsk=0 > /proc/ax_proc/vin/command

# disable kernel printk
echo 0 > /proc/sys/kernel/printk

# adjust vm param to update tfcard writing data
echo 50 > /proc/sys/vm/dirty_writeback_centisecs
echo 100 > /proc/sys/vm/dirty_expire_centisecs
echo 1 > /proc/sys/vm/dirty_background_ratio
echo 1 > /proc/sys/vm/dirty_ratio
echo 1 > /proc/sys/vm/overcommit_memory
echo 896 > /proc/sys/vm/min_free_kbytes

# launch
${app} -a ${aiisp_mode} -b ${gdc_enable} -c ${reboot_interval} -d ${sdrhdr_switch_test} -e ${jenc_enable} -f ${vin_cap_num} -g ${link_mode} -i ${audio_enable} -j ${rtsp_enable} -k ${sleep_time} -l ${chn_frame_mode} -m ${sd_mode} -n ${gdconlinevpp_test} -o ${tisp_in_aov} -p ${audio_in_aov} -q ${venc_change_rc_policy} -r ${max_record_file_count} -s ${gop_in_aov} -t ${ezoom_info} -u ${slow_shutter} -v ${ezoom_test} -w ${ae_manual_shutter} -x ${case_id} -y ${light_info} -z ${sns_ctrl_mode} -A ${bw_limit_level} -B ${ivps_sharpen} -C ${interpolation_resolution} &
