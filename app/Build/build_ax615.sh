#!/bin/sh
set -eu

SCRIPT_DIR=$(CDPATH= cd -- "$(dirname -- "$0")" && pwd)
APP_DIR=$(CDPATH= cd -- "${SCRIPT_DIR}/.." && pwd)
CODE_ROOT=$(CDPATH= cd -- "${APP_DIR}/.." && pwd)
BUILD_DIR="${SCRIPT_DIR}/cmake_build_ax615"
ACTION=${1:-build}

JOBS=${JOBS:-$(getconf _NPROCESSORS_ONLN 2>/dev/null || echo 4)}

case "${ACTION}" in
    build|clean|distclean)
        ;;
    *)
        echo "Usage: $0 [build|clean|distclean]" >&2
        exit 2
        ;;
esac

if [ "${ACTION}" = "distclean" ]; then
    rm -rf "${BUILD_DIR}" "${SCRIPT_DIR}/Kylin"
    echo "distclean done"
    exit 0
fi

if [ "${ACTION}" = "clean" ]; then
    if [ -d "${BUILD_DIR}" ]; then
        cmake --build "${BUILD_DIR}" --target clean
    fi
    rm -f "${SCRIPT_DIR}/Kylin"
    echo "clean done"
    exit 0
fi

mkdir -p "${BUILD_DIR}"
cd "${BUILD_DIR}"

cmake "${CODE_ROOT}" \
    -DCMAKE_TOOLCHAIN_FILE="${CODE_ROOT}/cmake/toolchain_ax615.cmake" \
    -DBUILD_LIBMEDIA=OFF \
    -DBUILD_APP_STUBS=ON \
    -DAOV_LINK_PREBUILT_DEPS=ON \
    -DCMAKE_BUILD_TYPE=Release

make Kylin -j"${JOBS}"

cp -f "${BUILD_DIR}/Kylin" "${SCRIPT_DIR}/Kylin"
ls -lh "${SCRIPT_DIR}/Kylin"
