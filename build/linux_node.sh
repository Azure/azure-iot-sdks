#!/bin/bash
# Copyright (c) Microsoft. All rights reserved.
# Licensed under the MIT license. See LICENSE file in the project root for full license information.

build_root=$(cd "$(dirname "$0")/.." && pwd)
cd $build_root

# The 'npm link' commands in this script create symlinks to tracked repo
# files from ignored locations (under ./node_modules). This means a call to
# 'git clean -xdf' will delete tracked files from the repo's working directory.
# To avoid any complications, we'll unlink everything before exiting.
unlink ()
{
    cd $build_root/tools/iothub-explorer
    npm rm azure-iothub

    cd $build_root/node/service
    npm rm
    npm rm azure-iot-build
    npm rm azure-iot-common

    cd ../device
    npm rm
    npm rm azure-iot-build
    npm rm azure-iot-common

    cd ../common
    npm rm
    npm rm azure-iot-build

    cd ../build
    npm rm

    exit $1
}

# -- Node.js --
cd $build_root/node

# Use 'npm link ...' commands instead of 'npm install' to refer to local
# folders for azure-iot-* packages, instead of pulling the most recently
# published versions from npmjs.org.

cd build
npm link
./build.sh
[ $? -eq 0 ] || unlink $?

cd ../common
npm link azure-iot-build
npm link
./build.sh lintAndAllTests
[ $? -eq 0 ] || unlink $?

cd ../device
npm link azure-iot-build
npm link azure-iot-common
npm link
./build.sh lintAndAllTests
[ $? -eq 0 ] || unlink $?

cd ../service
npm link azure-iot-build
npm link azure-iot-common
npm link
./build.sh lintAndAllTests
[ $? -eq 0 ] || unlink $?

cd ../../tools/iothub-explorer
npm link azure-iothub
npm install
npm test
[ $? -eq 0 ] || unlink $?

unlink 0
