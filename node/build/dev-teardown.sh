#!/bin/sh

# Copyright (c) Microsoft. All rights reserved.
# Licensed under the MIT license. See LICENSE file in the project root for full license information.

node_root=$(cd "$(dirname "$0")/.." && pwd)

cd $node_root/build/tools
echo "-- Removing links for `pwd` --"
npm rm azure-iothub

cd $node_root/../tools/iothub-explorer
echo "-- Removing links for `pwd` --"
npm rm azure-iothub
npm rm azure-iot-device
npm rm azure-iot-common

cd $node_root/e2etests
echo "\n-- Removing links for `pwd` --"
npm rm azure-iothub
npm rm azure-iot-device-mqtt
npm rm azure-iot-device-http
npm rm azure-iot-device-amqp
npm rm azure-iot-device
npm rm azure-iot-common

cd $node_root/device/transport/mqtt
echo "-- Removing links for `pwd` --"
npm rm
npm rm azure-iothub
npm rm azure-iot-device
npm rm azure-iot-common

cd $node_root/device/transport/http
echo "-- Removing links for `pwd` --"
npm rm
npm rm azure-iothub
npm rm azure-iot-device
npm rm azure-iot-common
npm rm azure-iot-http-base

cd $node_root/device/transport/amqp
echo "-- Removing links for `pwd` --"
npm rm
npm rm azure-iothub
npm rm azure-iot-device
npm rm azure-iot-common
npm rm azure-iot-amqp-base

cd $node_root/service
echo "-- Removing links for `pwd` --"
npm rm
npm rm azure-iot-http-base
npm rm azure-iot-amqp-base
npm rm azure-iot-common

cd $node_root/device/core
echo "-- Removing links for `pwd` --"
npm rm
npm rm azure-iot-common
npm rm azure-iot-http-base

cd $node_root/common/transport/http
echo "-- Removing links for `pwd` --"
npm rm
npm rm azure-iot-common

cd $node_root/common/transport/amqp
echo "-- Removing links for `pwd` --"
npm rm
npm rm azure-iot-common

cd $node_root/common/core
echo "-- Removing links for `pwd` --"
npm rm
