#!/bin/sh
set -eu

SCRIPT_DIR=$(CDPATH= cd -- "$(dirname -- "$0")" && pwd)
LIBMEDIA_DIR=$(CDPATH= cd -- "${SCRIPT_DIR}/.." && pwd)
CODE_ROOT=$(CDPATH= cd -- "${LIBMEDIA_DIR}/.." && pwd)
PREBUILT_DIR="${CODE_ROOT}/Depend/prebuilt"
PREBUILT_INCLUDE_DIR="${PREBUILT_DIR}/include/libmedia"
PREBUILT_LIB_DIR="${PREBUILT_DIR}/lib"

JOBS=${JOBS:-$(getconf _NPROCESSORS_ONLN 2>/dev/null || echo 4)}

cd "${SCRIPT_DIR}"

case "${1:-build}" in
    clean)
        rm -rf \
            CMakeCache.txt \
            CMakeFiles \
            Makefile \
            cmake_install.cmake \
            libmedia.a
        rm -rf "${PREBUILT_INCLUDE_DIR}"
        rm -f "${PREBUILT_LIB_DIR}/libmedia.a"
        echo "libmedia clean done"
        exit 0
        ;;
    build)
        ;;
    *)
        echo "usage: $0 [build|clean]" >&2
        exit 2
        ;;
esac

cmake "${LIBMEDIA_DIR}" \
    -DCMAKE_TOOLCHAIN_FILE="${CODE_ROOT}/cmake/toolchain_ax615.cmake" \
    -DLIBMEDIA_BUILD_AX615_ADAPTERS=ON \
    -DCMAKE_BUILD_TYPE=Release

make media -j"${JOBS}"

mkdir -p "${PREBUILT_INCLUDE_DIR}" "${PREBUILT_LIB_DIR}"
rm -rf "${PREBUILT_INCLUDE_DIR}"
mkdir -p "${PREBUILT_INCLUDE_DIR}"
cp -R "${LIBMEDIA_DIR}/Include/." "${PREBUILT_INCLUDE_DIR}/"
cp "${SCRIPT_DIR}/libmedia.a" "${PREBUILT_LIB_DIR}/libmedia.a"

ls -lh "${PREBUILT_LIB_DIR}/libmedia.a"
