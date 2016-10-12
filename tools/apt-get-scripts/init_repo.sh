#!/bin/bash

set -e

release_folder="debian_release"
repo_orig="apt-get-orig"
repo_vivid="apt-get-vivid"
repo_wily="apt-get-wily"
repo_trusty="apt-get-trusty"

cd $release_folder
if [ -d $repo_vivid ]; then
        rm -rf $repo_vivid
fi

if [ -d $repo_wily ]; then
        rm -rf $repo_wily
fi

if [ -d $repo_trusty ]; then
        rm -rf $repo_trusty
fi

mkdir $repo_wily
mkdir $repo_vivid
mkdir $repo_trusty

# Fix up shared-Util
echo "Copying Azure IoT Sdk"
cp -r ~/$release_folder/$repo_orig/azure-iot-sdks ~/$release_folder/$repo_vivid/azure-iot-sdks
cp -r ~/$release_folder/$repo_orig/azure-iot-sdks ~/$release_folder/$repo_wily/azure-iot-sdks
cp -r ~/$release_folder/$repo_orig/azure-iot-sdks ~/$release_folder/$repo_trusty/azure-iot-sdks

echo "Copying Azure C Shared Util"
cp -r ~/$release_folder/$repo_orig/azure-iot-sdks/c/c-utility ~/$release_folder/$repo_vivid/c-utility
cp -r ~/$release_folder/$repo_orig/azure-iot-sdks/c/c-utility ~/$release_folder/$repo_wily/c-utility
cp -r ~/$release_folder/$repo_orig/azure-iot-sdks/c/c-utility ~/$release_folder/$repo_trusty/c-utility

echo "Copying Azure uamqp"
cp -r ~/$release_folder/$repo_orig/azure-iot-sdks/c/uamqp ~/$release_folder/$repo_vivid/uamqp
cp -r ~/$release_folder/$repo_orig/azure-iot-sdks/c/uamqp ~/$release_folder/$repo_wily/uamqp
cp -r ~/$release_folder/$repo_orig/azure-iot-sdks/c/uamqp ~/$release_folder/$repo_trusty/uamqp

echo "Copying Azure umqtt"
cp -r ~/$release_folder/$repo_orig/azure-iot-sdks/c/umqtt ~/$release_folder/$repo_vivid/umqtt
cp -r ~/$release_folder/$repo_orig/azure-iot-sdks/c/umqtt ~/$release_folder/$repo_wily/umqtt
cp -r ~/$release_folder/$repo_orig/azure-iot-sdks/c/umqtt ~/$release_folder/$repo_trusty/umqtt

echo "Creating Shared Util Vivid zip"
mkdir ~/$release_folder/$repo_vivid/c-utility_repo
mv ~/$release_folder/$repo_vivid/c-utility ~/$release_folder/$repo_vivid/c-utility_repo/azure-c-shared-util-0.1.0
tar czf ~/$release_folder/azure-c-shared-util-0.1.0.tar.gz ~/$release_folder/$repo_vivid/c-utility_repo/azure-c-shared-util-0.1.0
mv ~/$release_folder/azure-c-shared-util-0.1.0.tar.gz ~/$release_folder/$repo_vivid/c-utility_repo/azure-c-shared-util-0.1.0.tar.gz

echo "Creating Shared Util Wily Zip"
mkdir ~/$release_folder/$repo_wily/c-utility_repo
mv ~/$release_folder/$repo_wily/c-utility ~/$release_folder/$repo_wily/c-utility_repo/azure-c-shared-util-0.1.0.0
tar -czf ~/$release_folder/azure-c-shared-util-0.1.0.0.tar.gz ~/$release_folder/$repo_wily/c-utility_repo/azure-c-shared-util-0.1.0.0
mv ~/$release_folder/azure-c-shared-util-0.1.0.0.tar.gz ~/$release_folder/$repo_wily/c-utility_repo/azure-c-shared-util-0.1.0.0.tar.gz

echo "Creating Shared Util Trusty Zip"
mkdir ~/$release_folder/$repo_trusty/c-utility_repo
mv ~/$release_folder/$repo_trusty/c-utility ~/$release_folder/$repo_trusty/c-utility_repo/azure-c-shared-util-0.1.0.0
tar -czf ~/$release_folder/azure-c-shared-util-0.1.0.0.tar.gz ~/$release_folder/$repo_trusty/c-utility_repo/azure-c-shared-util-0.1.0.0
mv ~/$release_folder/azure-c-shared-util-0.1.0.0.tar.gz ~/$release_folder/$repo_trusty/c-utility_repo/azure-c-shared-util-0.1.0.0.tar.gz

echo "Creating Iothub Vivid zip"
mv ~/$release_folder/$repo_vivid/azure-iot-sdks/c ~/$release_folder/$repo_vivid/azure-iot-sdks/azure-iot-sdks-0.1.0
tar -czf ~/$release_folder/azure-iot-sdks-0.1.0.tar.gz ~/$release_folder/$repo_vivid/azure-iot-sdks/azure-iot-sdks-0.1.0
mv ~/$release_folder/azure-iot-sdks-0.1.0.tar.gz ~/$release_folder/$repo_vivid/azure-iot-sdks/azure-iot-sdks-0.1.0.tar.gz

echo "Creating Iothub Wily Zip"
mv ~/$release_folder/$repo_wily/azure-iot-sdks/c ~/$release_folder/$repo_wily/azure-iot-sdks/azure-iot-sdks-0.1.0.0
tar -czf ~/$release_folder/azure-iot-sdks-0.1.0.0.tar.gz ~/$release_folder/$repo_wily/azure-iot-sdks/azure-iot-sdks-0.1.0.0
mv ~/$release_folder/azure-iot-sdks-0.1.0.0.tar.gz ~/$release_folder/$repo_wily/azure-iot-sdks/azure-iot-sdks-0.1.0.0.tar.gz

echo "Creating Iothub Trusty Zip"
mv ~/$release_folder/$repo_trusty/azure-iot-sdks/c ~/$release_folder/$repo_trusty/azure-iot-sdks/azure-iot-sdks-0.1.0.0
tar -czf ~/$release_folder/azure-iot-sdks-0.1.0.0.tar.gz ~/$release_folder/$repo_trusty/azure-iot-sdks/azure-iot-sdks-0.1.0.0
mv ~/$release_folder/azure-iot-sdks-0.1.0.0.tar.gz ~/$release_folder/$repo_trusty/azure-iot-sdks/azure-iot-sdks-0.1.0.0.tar.gz

echo "Creating uAmqp Vivid zip"
mkdir ~/$release_folder/$repo_vivid/uamqp_repo
mv ~/$release_folder/$repo_vivid/uamqp ~/$release_folder/$repo_vivid/uamqp_repo/azure-uamqp-c-0.1.0
tar czf ~/$release_folder/azure-uamqp-c-0.1.0.tar.gz ~/$release_folder/$repo_vivid/uamqp_repo/azure-uamqp-c-0.1.0
mv ~/$release_folder/azure-uamqp-c-0.1.0.tar.gz ~/$release_folder/$repo_vivid/uamqp_repo/azure-uamqp-c-0.1.0.tar.gz

echo "Creating uAmqp Wily Zip"
mkdir ~/$release_folder/$repo_wily/uamqp_repo
mv ~/$release_folder/$repo_wily/uamqp ~/$release_folder/$repo_wily/uamqp_repo/azure-uamqp-c-0.1.0.0
tar czf ~/$release_folder/azure-uamqp-c-0.1.0.0.tar.gz ~/$release_folder/$repo_wily/uamqp_repo/azure-uamqp-c-0.1.0.0
mv ~/$release_folder/azure-uamqp-c-0.1.0.0.tar.gz ~/$release_folder/$repo_wily/uamqp_repo/azure-uamqp-c-0.1.0.0.tar.gz

echo "Creating uAmqp Trusty Zip"
mkdir ~/$release_folder/$repo_trusty/uamqp_repo
mv ~/$release_folder/$repo_trusty/uamqp ~/$release_folder/$repo_trusty/uamqp_repo/azure-uamqp-c-0.1.0.0
tar czf ~/$release_folder/azure-uamqp-c-0.1.0.0.tar.gz ~/$release_folder/$repo_trusty/uamqp_repo/azure-uamqp-c-0.1.0.0
mv ~/$release_folder/azure-uamqp-c-0.1.0.0.tar.gz ~/$release_folder/$repo_trusty/uamqp_repo/azure-uamqp-c-0.1.0.0.tar.gz

echo "Creating uMqtt Vivid zip"
mkdir ~/$release_folder/$repo_vivid/umqtt_repo
mv ~/$release_folder/$repo_vivid/umqtt ~/$release_folder/$repo_vivid/umqtt_repo/azure-umqtt-c-0.1.0
tar czf ~/$release_folder/azure-umqtt-c-0.1.0.tar.gz ~/$release_folder/$repo_vivid/umqtt_repo/azure-umqtt-c-0.1.0
mv ~/$release_folder/azure-umqtt-c-0.1.0.tar.gz ~/$release_folder/$repo_vivid/umqtt_repo/azure-umqtt-c-0.1.0.tar.gz

echo "Creating uMqtt Wily zip"
mkdir ~/$release_folder/$repo_wily/umqtt_repo
mv ~/$release_folder/$repo_wily/umqtt ~/$release_folder/$repo_wily/umqtt_repo/azure-umqtt-c-0.1.0.0
tar czf ~/$release_folder/azure-umqtt-c-0.1.0.0.tar.gz ~/$release_folder/$repo_wily/umqtt_repo/azure-umqtt-c-0.1.0.0
mv ~/$release_folder/azure-umqtt-c-0.1.0.0.tar.gz ~/$release_folder/$repo_wily/umqtt_repo/azure-umqtt-c-0.1.0.0.tar.gz

echo "Creating uMqtt Trusty zip"
mkdir ~/$release_folder/$repo_trusty/umqtt_repo
mv ~/$release_folder/$repo_trusty/umqtt ~/$release_folder/$repo_trusty/umqtt_repo/azure-umqtt-c-0.1.0.0
tar czf ~/$release_folder/azure-umqtt-c-0.1.0.0.tar.gz ~/$release_folder/$repo_trusty/umqtt_repo/azure-umqtt-c-0.1.0.0
mv ~/$release_folder/azure-umqtt-c-0.1.0.0.tar.gz ~/$release_folder/$repo_trusty/umqtt_repo/azure-umqtt-c-0.1.0.0.tar.gz

