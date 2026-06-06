# AX615 uclibc 交叉编译链配置
# 用法: cmake -DCMAKE_TOOLCHAIN_FILE=cmake/toolchain_ax615.cmake ..

set(CMAKE_SYSTEM_NAME Linux)
set(CMAKE_SYSTEM_PROCESSOR arm)

set(TOOLCHAIN_ROOT
    "/home/mashuntao/ax615_sdk_clean/toolchain/ax615_uclibc_toolchain/V4_20250701/arm-rel-linux-uclibcgnueabihf")

set(CMAKE_C_COMPILER   "${TOOLCHAIN_ROOT}/bin/arm-rel-linux-uclibcgnueabihf-gcc")
set(CMAKE_CXX_COMPILER "${TOOLCHAIN_ROOT}/bin/arm-rel-linux-uclibcgnueabihf-g++")
set(CMAKE_STRIP        "${TOOLCHAIN_ROOT}/bin/arm-rel-linux-uclibcgnueabihf-strip")
set(CMAKE_AR           "${TOOLCHAIN_ROOT}/bin/arm-rel-linux-uclibcgnueabihf-ar")
set(CMAKE_RANLIB       "${TOOLCHAIN_ROOT}/bin/arm-rel-linux-uclibcgnueabihf-ranlib")

# SDK 路径(在 Linux 服务器上访问)
set(AX615_SDK_ROOT
    "/home/mashuntao/ax615_sdk_clean/02.SDK/AX615_SDK_V2.0.0_P5_20260326181014_NO68"
    CACHE PATH "AX615 SDK root")

set(AX615_SDK_INC "${AX615_SDK_ROOT}/msp/out/arm_uclibc/include")
set(AX615_SDK_LIB "${AX615_SDK_ROOT}/msp/out/arm_uclibc/lib")
set(AX615_THIRD_PARTY "${AX615_SDK_ROOT}/third-party")

# 让 cmake 在 sysroot 内查找库/头，不混用 host 系统路径
set(CMAKE_FIND_ROOT_PATH "${TOOLCHAIN_ROOT}")
set(CMAKE_FIND_ROOT_PATH_MODE_PROGRAM NEVER)
set(CMAKE_FIND_ROOT_PATH_MODE_LIBRARY ONLY)
set(CMAKE_FIND_ROOT_PATH_MODE_INCLUDE ONLY)
