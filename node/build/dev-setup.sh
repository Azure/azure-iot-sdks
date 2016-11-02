#!/bin/sh

# Copyright (c) Microsoft. All rights reserved.
# Licensed under the MIT license. See LICENSE file in the project root for full license information.

node_root=$(cd "$(dirname "$0")/.." && pwd)

cd $node_root/common/core
echo "\n-- Creating links for `pwd` --"
npm link
[ $? -eq 0 ] || exit $?

cd $node_root/common/transport/amqp
echo "\n-- Creating links for `pwd` --"
npm link azure-iot-common
npm link
[ $? -eq 0 ] || exit $?

cd $node_root/common/transport/http
echo "\n-- Creating links for `pwd` --"
npm link azure-iot-common
npm link
[ $? -eq 0 ] || exit $?

cd $node_root/device/core
echo "\n-- Creating links for `pwd` --"
npm link azure-iot-http-base
npm link azure-iot-common
npm link
[ $? -eq 0 ] || exit $?

cd $node_root/service
echo "\n-- Creating links for `pwd` --"
npm link azure-iot-common
npm link azure-iot-amqp-base
npm link azure-iot-http-base
npm link
[ $? -eq 0 ] || exit $?

cd $node_root/device/transport/amqp
echo "\n-- Creating links for `pwd` --"
npm link azure-iot-amqp-base
npm link azure-iot-common
npm link azure-iot-device
npm link azure-iothub
npm link
[ $? -eq 0 ] || exit $?

cd $node_root/device/transport/http
echo "\n-- Creating links for `pwd` --"
npm link azure-iot-http-base
npm link azure-iot-common
npm link azure-iot-device
npm link azure-iothub
npm link
[ $? -eq 0 ] || exit $?

cd $node_root/device/transport/mqtt
echo "\n-- Creating links for `pwd` --"
npm link azure-iot-common
npm link azure-iot-device
npm link azure-iothub
npm link
[ $? -eq 0 ] || exit $?

cd $node_root/e2etests
echo "\n-- Creating links for `pwd` --"
npm link azure-iot-common
npm link azure-iot-device
npm link azure-iot-device-amqp
npm link azure-iot-device-http
npm link azure-iot-device-mqtt
npm link azure-iothub
npm install

cd $node_root/../tools/iothub-explorer
echo "\n-- Creating links for `pwd` --"
npm link azure-iot-common
npm link azure-iot-device
npm link azure-iothub
npm install
[ $? -eq 0 ] || exit $?

cd $node_root/build/tools
echo "\n-- Creating links for `pwd` --"
npm link azure-iothub
npm install