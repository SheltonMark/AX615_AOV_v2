# libmedia_demo

`libmedia_demo` is a board-side smoke tool for validating `libmedia.a` before
integrating app/packet/storage/cloud.

It verifies:

- AX615 media runtime init/start/stop.
- OS04D10 single sensor pipeline.
- VENC bare stream callback.
- Raw `.h264/.h265` file output.
- Runtime video config apply.
- AOV sleep/wake API re-entry.

## Build

On the compile server:

```sh
cd ~/AOV_AX615/code_v2
sh libmedia/Build/build_ax615.sh
sh test/libmedia_demo/build_ax615.sh
```

Clean and rebuild:

```sh
sh test/libmedia_demo/build_ax615.sh clean
sh test/libmedia_demo/build_ax615.sh
```

Print the full compiler/linker command:

```sh
VERBOSE=1 sh test/libmedia_demo/build_ax615.sh
```

Output:

```text
test/libmedia_demo/build_ax615/libmedia_demo
```

If link fails with missing SDK symbols, first confirm `Depend/ax_sdk/arm_uclibc`
contains the matching SDK library. Board-specific additions can be passed with
`EXTRA_LIBS`, for example:

```sh
EXTRA_LIBS="-lxxx" sh test/libmedia_demo/build_ax615.sh
```

## Run

Copy the binary and `run_libmedia_demo.sh` to the AX615 board, then run the
qsdemo-aligned launcher:

```sh
chmod +x ./run_libmedia_demo.sh ./libmedia_demo
./run_libmedia_demo.sh -x 3 -o /tmp/libmedia_demo.h265
```

This script mirrors the known-working qsdemo OS04D10 profile:

```text
case_id=3
aiisp_mode=0
gdc_enable=1
jenc_enable=1
vin_cap_num=0
link_mode=3,1
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
```

The launcher also boots the RISC-V firmware if needed and applies the same
kernel/vm debug settings used by qsdemo.

You can also run `libmedia_demo` directly:

```sh
./libmedia_demo \
  --case-id 3 \
  --aiisp-mode 0 \
  --gdc 1 \
  --jenc 1 \
  --link-mode 3,1 \
  --sns-ctrl-mode 3 \
  --codec h265 \
  --width 1920 \
  --height 1080 \
  --fps 15 \
  --bitrate 1024 \
  --gop 60 \
  --seconds 20 \
  --output /tmp/libmedia_demo.h265
```

Expected logs:

```text
[OK] RegisterStreamCallback
[OK] Init
[OK] Start
[frame] seq=... pts=... size=... key=...
[done] frames=... bytes=... output=/tmp/libmedia_demo.h265
```

Validate the stream:

```sh
ffplay /tmp/libmedia_demo.h265
```

For H.264:

```sh
./libmedia_demo --codec h264 --output /tmp/libmedia_demo.h264
ffplay /tmp/libmedia_demo.h264
```

## Interactive Debug

```sh
./libmedia_demo --interactive --output /tmp/libmedia_debug.h265
```

Commands:

```text
status
set bitrate 512
set fps 10
set gop 30
set codec h264
set resolution 1280 720
key
sleep
wake
quit
```

Debug order:

1. Confirm `Init` and `Start` return OK.
2. Confirm frames are printed and output file size grows.
3. Confirm output file can be played.
4. Change bitrate/fps/gop and check stream continues.
5. Change resolution or codec and check the stream restarts.
6. Run `sleep`, then `wake`, and confirm frames resume.

This demo intentionally bypasses `app/packet`. If this demo cannot output video,
debug `libmedia`, SDK, sensor, IQ, or board environment first. If this demo works
but app does not, debug `libmedia -> packet -> app` integration next.

## QSDemo Parity Notes

The demo now accepts the important qsdemo `run.sh` fields so logs can be compared
one-to-one with a working `./run.sh -x 3` run. At the moment, only fields already
present in the public `libmedia` config are applied directly:

```text
sensor=OS04D10
codec
resolution
fps
bitrate
gop
fps_aov
iq_bin_path
```

These fields are collected and printed, but require `libmedia` adapter support
before they can affect AX SDK setup:

```text
aiisp_mode
gdc_enable
jenc_enable
vin_cap_num
link_mode
sleep_time
chn_frame_mode
sd_mode
tisp_in_aov
audio_in_aov
venc_change_rc_policy
gop_in_aov
sns_ctrl_mode
bw_limit_level
ivps_sharpen
interpolation_resolution
aiisp_model_path
```

When debugging no-video issues, compare these logs first:

```text
[qsdemo-profile] ...
[OK] Init
[OK] Start
[frame] ...
```

If qsdemo can output video but `libmedia_demo` cannot, the next area to inspect
is `libmedia/Src/adapter/ax615`: OS04D10 sensor profile, MIPI/VIN/ISP start
order, AIISP/TISP selection, GDC/JENC/link mode, and RISC-V AOV control.
