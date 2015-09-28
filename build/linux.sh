#!/bin/bash
# Copyright (c) Microsoft. All rights reserved.
# Licensed under the MIT license. See LICENSE file in the project root for full license information.

build_root=$(cd "$(dirname "$0")/.." && pwd)
cd $build_root

# -- C --
./c/build_all/linux/build.sh --run-e2e-tests #-x
[ $? -eq 0 ] || exit $?

# -- Node.js --
cd $build_root/node

cd build
npm install
./build.sh
[ $? -eq 0 ] || exit $?

cd ../common
npm install
./build.sh lintAndAllTests
[ $? -eq 0 ] || exit $?

cd ../device
npm install
./build.sh lintAndAllTests
[ $? -eq 0 ] || exit $?

cd ../service
npm install
./build.sh lintAndAllTests
[ $? -eq 0 ] || exit $?

cd ../../tools/iothub-explorer
npm install
npm test
[ $? -eq 0 ] || exit $?
