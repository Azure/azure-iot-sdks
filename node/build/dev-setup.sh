#!/bin/sh

# Copyright (c) Microsoft. All rights reserved.
# Licensed under the MIT license. See LICENSE file in the project root for full license information.

node_root=$(cd "$(dirname "$0")/.." && pwd)
cd $node_root

cd common
echo "\n-- Creating links for `pwd` --"
npm link
[ $? -eq 0 ] || exit $?

cd ../device
echo "\n-- Creating links for `pwd` --"
npm link azure-iot-common
npm link
[ $? -eq 0 ] || exit $?

cd ../service
echo "\n-- Creating links for `pwd` --"
npm link azure-iot-common
npm link
[ $? -eq 0 ] || exit $?

cd ../../tools/iothub-explorer
echo "\n-- Creating links for `pwd` --"
npm link azure-iot-common
npm link azure-iothub
npm install
[ $? -eq 0 ] || exit $?
