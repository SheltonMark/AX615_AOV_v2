#!/bin/sh
set -eu

SCRIPT_DIR=$(CDPATH= cd -- "$(dirname -- "$0")" && pwd)
LIBSYS_DIR=$(CDPATH= cd -- "${SCRIPT_DIR}/.." && pwd)
CODE_ROOT=$(CDPATH= cd -- "${LIBSYS_DIR}/.." && pwd)
BUILD_DIR="${SCRIPT_DIR}/cmake_build"
PREBUILT_DIR="${CODE_ROOT}/Depend/prebuilt"
PREBUILT_INCLUDE_DIR="${PREBUILT_DIR}/include/libsys"
PREBUILT_LIB_DIR="${PREBUILT_DIR}/lib"
JOBS=${JOBS:-$(getconf _NPROCESSORS_ONLN 2>/dev/null || echo 4)}

case "${1:-build}" in
    clean)
        if [ -d "${BUILD_DIR}" ]; then
            cmake --build "${BUILD_DIR}" --target clean 2>/dev/null || true
        fi
        rm -rf "${PREBUILT_INCLUDE_DIR}"
        rm -f "${PREBUILT_LIB_DIR}/libsys.a"
        echo "libsys clean done"
        exit 0
        ;;
    distclean)
        rm -rf "${BUILD_DIR}"
        rm -rf "${PREBUILT_INCLUDE_DIR}"
        rm -f "${PREBUILT_LIB_DIR}/libsys.a"
        echo "libsys distclean done"
        exit 0
        ;;
    build)
        ;;
    *)
        echo "usage: $0 [build|clean|distclean]" >&2
        exit 2
        ;;
esac

mkdir -p "${BUILD_DIR}"
cd "${BUILD_DIR}"

cmake "${LIBSYS_DIR}" \
    -DCMAKE_BUILD_TYPE=Release

make sys -j"${JOBS}"

mkdir -p "${PREBUILT_INCLUDE_DIR}" "${PREBUILT_LIB_DIR}"
rm -rf "${PREBUILT_INCLUDE_DIR}"
mkdir -p "${PREBUILT_INCLUDE_DIR}"
cp -R "${LIBSYS_DIR}/include/." "${PREBUILT_INCLUDE_DIR}/"
cp "${BUILD_DIR}/libsys.a" "${PREBUILT_LIB_DIR}/libsys.a"
ls -lh "${PREBUILT_LIB_DIR}/libsys.a"
