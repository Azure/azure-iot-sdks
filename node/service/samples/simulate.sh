#!/bin/bash
# Copyright (c) Microsoft. All rights reserved.
# Licensed under the MIT license. See LICENSE file in the project root for full license information.

set -e

current_path=$(cd "$(dirname "$0")" && pwd)
device_creds_path="$current_path"/devicecreds.txt
sample_path="$current_path"/cmake/iotdm_client/samples/iotdm_simple_sample/iotdm_simple_sample

# check if devicecreds.txt exists
if [ ! -f "$device_creds_path" ];
then
    echo devicecreds.txt was not found. Please generate devices by running generate_devices.js first.
    exit 1
fi

# check if the sample binary exists
if [ ! -f "$sample_path" ];
then
    echo Device management simple sample binary was not found. Please run setup.sh first to build the sample.
    exit 1
fi

echo In order to simulate devices please run the following commands individually from separate terminals for each device.
echo

while read c;
do
    echo "\"$sample_path\"" "\"$c\""
    echo
done <"$device_creds_path"
