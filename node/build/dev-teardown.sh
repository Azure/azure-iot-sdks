#!/bin/sh

# Copyright (c) Microsoft. All rights reserved.
# Licensed under the MIT license. See LICENSE file in the project root for full license information.

node_root=$(cd "$(dirname "$0")/.." && pwd)

cd $node_root/../tools/iothub-explorer
echo "-- Removing links for `pwd` --"
npm rm azure-iothub
npm rm azure-iot-common

cd $node_root/service
echo "-- Removing links for `pwd` --"
npm rm
npm rm azure-iot-common

cd $node_root/device
echo "-- Removing links for `pwd` --"
npm rm
npm rm azure-iot-common

cd $node_root/common
echo "-- Removing links for `pwd` --"
npm rm
