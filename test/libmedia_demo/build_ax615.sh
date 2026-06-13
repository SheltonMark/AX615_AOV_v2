#!/bin/sh
set -eu

if [ "${VERBOSE:-0}" = "1" ]; then
    set -x
fi

SCRIPT_DIR=$(CDPATH= cd -- "$(dirname -- "$0")" && pwd)
CODE_ROOT=$(CDPATH= cd -- "${SCRIPT_DIR}/../.." && pwd)
OUT_DIR="${SCRIPT_DIR}/build_ax615"

TC_PREFIX=${TC_PREFIX:-arm-rel-linux-uclibcgnueabihf}
CXX=${CXX:-${TC_PREFIX}-g++}
AR=${AR:-${TC_PREFIX}-ar}

LIBMEDIA_A=${LIBMEDIA_A:-"${CODE_ROOT}/Depend/prebuilt/lib/libmedia.a"}
SDK_INC=${SDK_INC:-"${CODE_ROOT}/Depend/ax_sdk/arm_uclibc/include"}
SDK_LIB=${SDK_LIB:-"${CODE_ROOT}/Depend/ax_sdk/arm_uclibc/lib"}
THIRD_PARTY_INC=${THIRD_PARTY_INC:-"${CODE_ROOT}/Depend/third_party/include"}
THIRD_PARTY_LIB=${THIRD_PARTY_LIB:-"${CODE_ROOT}/Depend/third_party/lib"}
LIBMEDIA_INC=${LIBMEDIA_INC:-"${CODE_ROOT}/Depend/prebuilt/include/libmedia"}

case "${1:-build}" in
    clean)
        echo "[clean] removing ${OUT_DIR}"
        rm -rf "${OUT_DIR}"
        echo "libmedia demo clean done"
        exit 0
        ;;
    build)
        ;;
    *)
        echo "usage: $0 [build|clean]" >&2
        exit 2
        ;;
esac

echo "[build] libmedia demo"
echo "  CXX=${CXX}"
echo "  LIBMEDIA_A=${LIBMEDIA_A}"
echo "  LIBMEDIA_INC=${LIBMEDIA_INC}"
echo "  SDK_INC=${SDK_INC}"
echo "  SDK_LIB=${SDK_LIB}"
echo "  THIRD_PARTY_INC=${THIRD_PARTY_INC}"
echo "  THIRD_PARTY_LIB=${THIRD_PARTY_LIB}"

if [ ! -f "${LIBMEDIA_A}" ]; then
    echo "missing ${LIBMEDIA_A}; run libmedia/Build/build_ax615.sh first" >&2
    exit 1
fi

mkdir -p "${OUT_DIR}"

SDK_LIB_NAMES="
    ax_sys
    ax_mipi
    ax_nt_ctrl
    ax_proton
    ax_ivps
    ax_venc
    ax_audio
    ax_tcm
    ax_engine
    ax_skel
    ax_ive
    ax_autokit
    ax_ae
    ax_awb
    ax_af
    ax_audio_3a
    isp_proton_algo
    loadbin_isp
    gen_bin
    sns_os04d10
"

THIRD_PARTY_LIB_NAMES="
    tinyalsa
    samplerate
"

SDK_LIBS=""
for lib_name in ${SDK_LIB_NAMES}; do
    if [ -f "${SDK_LIB}/lib${lib_name}.a" ] || [ -f "${SDK_LIB}/lib${lib_name}.so" ]; then
        echo "  link sdk: -l${lib_name}"
        SDK_LIBS="${SDK_LIBS} -l${lib_name}"
    else
        echo "warning: SDK library lib${lib_name}.a/.so not found, skip" >&2
    fi
done

for lib_name in ${THIRD_PARTY_LIB_NAMES}; do
    if [ -f "${THIRD_PARTY_LIB}/lib${lib_name}.a" ] || [ -f "${THIRD_PARTY_LIB}/lib${lib_name}.so" ]; then
        echo "  link third_party: -l${lib_name}"
        SDK_LIBS="${SDK_LIBS} -l${lib_name}"
    else
        echo "warning: third-party library lib${lib_name}.a/.so not found, skip" >&2
    fi
done

echo "[build] compiling ${SCRIPT_DIR}/libmedia_demo.cpp"
"${CXX}" -std=c++17 -O2 -Wall -Wextra \
    -I"${LIBMEDIA_INC}" \
    -I"${SDK_INC}" \
    -I"${THIRD_PARTY_INC}" \
    "${SCRIPT_DIR}/libmedia_demo.cpp" \
    -Wl,--start-group \
    "${LIBMEDIA_A}" \
    -L"${SDK_LIB}" \
    -L"${THIRD_PARTY_LIB}" \
    -Wl,-rpath-link,"${SDK_LIB}" \
    -Wl,-rpath-link,"${THIRD_PARTY_LIB}" \
    ${SDK_LIBS} \
    ${EXTRA_LIBS:-} \
    -Wl,--end-group \
    -lpthread -ldl -lm \
    -o "${OUT_DIR}/libmedia_demo"

echo "[build] output:"
ls -lh "${OUT_DIR}/libmedia_demo"

# Copy to shared directory for board deployment
SHARED_DIR="/share/mashuntao"
if [ -d "${SHARED_DIR}" ]; then
    cp "${OUT_DIR}/libmedia_demo" "${SHARED_DIR}/libmedia_demo"
    chmod +x "${SHARED_DIR}/libmedia_demo"
    echo "[deploy] copied to ${SHARED_DIR}/libmedia_demo"
fi

