#!/bin/bash
# Copyright (c) Microsoft. All rights reserved.
# Licensed under the MIT license. See LICENSE file in the project root for full license information.

set -e

# -----------------------------------------------------------------------------
# -- helper subroutines
# -----------------------------------------------------------------------------
checkExists() {
    if hash $1 2>/dev/null;
    then
        return 0
    else
        echo "$1" not found. Please make sure that "$1" is installed and available in the path.
        exit 1
    fi
}

usage()
{
    echo "setup.sh [Azure IoT Hub Connection String]"
    exit 1
}

# -----------------------------------------------------------------------------
# -- Check for environment pre-requisites. This script requires
# -- that the following programs work:
# --     cmake, node, npm
# -----------------------------------------------------------------------------
checkExists cmake
checkExists node
checkExists npm

# check if the connection string was passed
if [ $# -eq 0 ] 
then
    usage
fi
conn_string=$1

#
# This is the path where the C source files live. 
#
build_root=$(cd "$(dirname "$0")/../../../c" && pwd)

#
# This is the path to where this file lives.
#
output_path=$(cd "$(dirname "$0")" && pwd)

#
# This is the destination location where we generate the CMake output files
#
cmake_output_path="$output_path"/cmake

# clean up the output path
rm -r -f "$cmake_output_path"
mkdir -p "$cmake_output_path"
pushd "$cmake_output_path"

# generate makefiles
echo Creating build files...
cmake "$build_root"

# make!
echo Building device client sample...
make --jobs=$(nproc) iotdm_simple_sample

popd

# provision devices
echo Provisioning devices...
pushd $output_path
npm install
node "$output_path"/generate_devices.js "$conn_string"
popd

echo All done!
echo You are now in a folder containing the sample applications.
echo You can run each sample by calling it with the connection string as an argument.

[ $? -eq 0 ] || exit $?
