#!/bin/bash

set -e

# Name of the location to update repo
repo_dir="apt-get-wily"

# Clean up shared-Util
echo "Preparing to remove azure-c-shared-utility vivid files"
rm ~/$repo_dir/azure-c-shared-utility/azure-c-shared-util*0.1.0.0.*
rm ~/$repo_dir/azure-c-shared-utility/azure-c-shared-util*0.1.0.0-*
rm -r ~/$repo_dir/azure-c-shared-utility/azure-c-shared-util-0.1.0.0/debian


# Release Iot-Sdk
echo "Preparing azure-iot-sdk vivid package"
rm ~/$repo_dir/azure-iot-sdk/azure-iot-sdk*0.1.0.0.*
rm ~/$repo_dir/azure-iot-sdk/azure-iot-sdk-c*0.1.0.1-*
rm -r ~/$repo_dir/azure-iot-sdks/azure-iot-sdks-0.1.0.0/debian


# Release uAmqp
echo "Preparing azure-uamqp-c vivid package"
rm ~/$repo_dir/azure-uamqp-c_repo/azure-uamqp-c*0.1.0.0.*
rm ~/$repo_dir/azure-uamqp-c_repo/azure-uamqp-c*0.1.0.0-*
rm -r ~/$repo_dir/azure-uamqp-c_repo/azure-uamqp-c-0.1.0.0/debian


# Release uMqtt
echo "Preparing to remove azure-umqtt-c vivid files"
rm ~/$repo_dir/azure-umqtt-c_repo/azure-umqtt-c*0.1.0.0.*
rm ~/$repo_dir/azure-umqtt-c_repo/azure-umqtt-c*0.1.0.1-*
rm -r ~/$repo_dir/azure-umqtt-c_repo/azure-umqtt-c-0.1.0.0/debian

