#!/bin/bash
# Copyright (c) Microsoft. All rights reserved. Licensed under the MIT 
# license. See LICENSE file in the project root for full license 
# information.

# Tested on RPi2 debian verion 7.8

install_root="$HOME" 
build_root=$(cd "$(dirname "$0")/.." && pwd) 
cd $build_root

# ----------------------------------------------------------------------------- 
# -- helper subroutines 
# -----------------------------------------------------------------------------
checkExists() {
    if hash $1 2>/dev/null;
    then
        return 1
    else
        echo "$1" not found. Please make sure that "$1" is installed and available in the path.
        exit 1
    fi
}
# ----------------------------------------------------------------------------- 
# -- Check for environment pre-requisites. This script requires -- that 
# the following programs work: -- curl build-essential(g++,gcc,make) 
# cmake git 
# -----------------------------------------------------------------------------
checkExists curl 
checkExists g++ 
checkExists gcc 
checkExists make 
checkExists cmake 
checkExists git

# ----------------------------------------------------------------------------- 
# -- Check for RPiTools directory.
# -----------------------------------------------------------------------------
if [ ! -d "$install_root/RPiTools" ];
then
   echo ---------- Raspberry Pi tool-chain absent ----------
   exit 1
fi

# ----------------------------------------------------------------------------- 
# -- Set environment variable
# -----------------------------------------------------------------------------
cd $install_root/RPiTools/tools/arm-bcm2708/gcc-linaro-arm-linux-gnueabihf-raspbian-x64/arm-linux-gnueabihf 
export RPI_ROOT=$(pwd)

# ----------------------------------------------------------------------------- 
# -- Create toolchain-rpi.cmake 
# -----------------------------------------------------------------------------
echo ---------- Creating toolchain cmake file ---------- 
FILE="$build_root/c/build_all/linux/toolchain-rpi.cmake" 

/bin/cat <<EOM >$FILE
INCLUDE(CMakeForceCompiler) 

SET(CMAKE_SYSTEM_NAME Linux) # this one is important 
SET(CMAKE_SYSTEM_VERSION 1) # this one not so much

# this is the location of the amd64 toolchain targeting the Raspberry Pi
SET(CMAKE_C_COMPILER ${RPI_ROOT}/../bin/arm-linux-gnueabihf-gcc)

# this is the file system root of the target
SET(CMAKE_FIND_ROOT_PATH ${RPI_ROOT})

# search for programs in the build host directories
SET(CMAKE_FIND_ROOT_PATH_MODE_PROGRAM NEVER)

# for libraries and headers in the target directories
SET(CMAKE_FIND_ROOT_PATH_MODE_LIBRARY ONLY) 
SET(CMAKE_FIND_ROOT_PATH_MODE_INCLUDE ONLY) 
EOM

# ----------------------------------------------------------------------------- 
# -- Modify c/iothub_client/samples/iothub_client_sample_http/iothub_client_sample_http.c 
# -----------------------------------------------------------------------------
echo ---------- Modifying c/iothub_client/samples/iothub_client_sample_http/iothub_client_sample_http.c file ---------- 
cd $build_root 
sed -i 's/\[device connection string\]/'$IOTHUB_DEVICE_CONN_STR'/g' c/iothub_client/samples/iothub_client_sample_http/iothub_client_sample_http.c

# ----------------------------------------------------------------------------- 
# -- Build the SDK 
# -----------------------------------------------------------------------------
echo ---------- Building the SDK by executing build.sh script ---------- 
cd $build_root/c/build_all/linux 
./build.sh --toolchain-file toolchain-rpi.cmake --skip-unittests --no-amqp --no-mqtt -cl --sysroot=$RPI_ROOT 
[ $? -eq 0 ] || exit $?
