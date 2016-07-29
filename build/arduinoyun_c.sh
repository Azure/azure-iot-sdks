#!/bin/bash
# Copyright (c) Microsoft. All rights reserved.
# Licensed under the MIT license. See LICENSE file in the project root for full license information.

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
# -- Check for environment pre-requisites. This script requires
# -- that the following programs work:
# --     curl uuid g++ make cmake git unzip gawk
# -----------------------------------------------------------------------------
checkExists curl
checkExists uuid
checkExists g++
checkExists make
checkExists cmake
checkExists git
checkExists unzip
checkExists gawk

# -----------------------------------------------------------------------------
# -- Run the ./setup.sh script to install the OpenWRT SDK
# -----------------------------------------------------------------------------
if [ ! -d "$install_root/openwrt/sdk" ];
then
   echo ---------- Installing OpenWRT SDK by executing ./setup.sh script ----------
   cd $build_root/c/build_all/arduino
   printf 'y\ny\n' | ./setup.sh
else
   echo ---------- OpenWRT SDK is already installed ----------
fi

# -----------------------------------------------------------------------------
# -- Modify c/serializer/samples/simplesample_http/simplesample_http.c
# -----------------------------------------------------------------------------
echo ---------- Modifying c/serializer/samples/simplesample_http/simplesample_http.c file ----------
cd $build_root
sed -i 's/\[device connection string\]/'$IOTHUB_DEVICE_CONN_STR'/g' c/serializer/samples/simplesample_http/simplesample_http.c


# -----------------------------------------------------------------------------
# -- Build the SDK
# -----------------------------------------------------------------------------
echo ---------- Building the SDK by executing build.sh script ----------
cd $build_root/c/build_all/arduino
./build.sh
[ $? -eq 0 ] || exit $?
