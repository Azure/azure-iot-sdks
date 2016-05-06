#!/bin/bash

set -e

release_folder="debian_release"
repo_orig="apt-get-orig"
repo_vivid="apt-get-vivid"
repo_wily="apt-get-wily"
repo_jessie="apt-get-jessie"

cd $release_folder
if [ -d $repo_vivid ]; then
        rm -rf $repo_vivid
fi

if [ -d $repo_wily ]; then
        rm -rf $repo_wily
fi

if [ -d $repo_jessie ]; then
        rm -rf $repo_jessie
fi

mkdir $repo_wily
mkdir $repo_vivid
mkdir $repo_jessie

# Fix up shared-Util
echo "Copying Azure IoT Sdk"
cp -r ~/$release_folder/$repo_orig/azure-iot-sdks ~/$release_folder/$repo_vivid/azure-iot-sdks
cp -r ~/$release_folder/$repo_orig/azure-iot-sdks ~/$release_folder/$repo_wily/azure-iot-sdks
cp -r ~/$release_folder/$repo_orig/azure-iot-sdks ~/$release_folder/$repo_jessie/azure-iot-sdks

echo "Copying Azure C Shared Util"
cp -r ~/$release_folder/$repo_orig/azure-c-shared-utility ~/$release_folder/$repo_vivid/azure-c-shared-utility
cp -r ~/$release_folder/$repo_orig/azure-c-shared-utility ~/$release_folder/$repo_wily/azure-c-shared-utility
cp -r ~/$release_folder/$repo_orig/azure-c-shared-utility ~/$release_folder/$repo_jessie/azure-c-shared-utility

echo "Copying Azure uamqp"
cp -r ~/$release_folder/$repo_orig/azure-uamqp-c ~/$release_folder/$repo_vivid/azure-uamqp-c
cp -r ~/$release_folder/$repo_orig/azure-uamqp-c ~/$release_folder/$repo_wily/azure-uamqp-c
cp -r ~/$release_folder/$repo_orig/azure-uamqp-c ~/$release_folder/$repo_jessie/azure-uamqp-c

echo "Copying Azure umqtt"
cp -r ~/$release_folder/$repo_orig/azure-umqtt-c ~/$release_folder/$repo_vivid/azure-umqtt-c
cp -r ~/$release_folder/$repo_orig/azure-umqtt-c ~/$release_folder/$repo_wily/azure-umqtt-c
cp -r ~/$release_folder/$repo_orig/azure-umqtt-c ~/$release_folder/$repo_jessie/azure-umqtt-c

echo "Creating Shared Util Vivid zip"
mkdir ~/$release_folder/$repo_vivid/azure-c-shared-utility_repo
mv ~/$release_folder/$repo_vivid/azure-c-shared-utility ~/$release_folder/$repo_vivid/azure-c-shared-utility_repo/azure-c-shared-util-0.1.0
tar czf ~/$release_folder/azure-c-shared-util-0.1.0.tar.gz ~/$release_folder/$repo_vivid/azure-c-shared-utility_repo/azure-c-shared-util-0.1.0
mv ~/$release_folder/azure-c-shared-util-0.1.0.tar.gz ~/$release_folder/$repo_vivid/azure-c-shared-utility_repo/azure-c-shared-util-0.1.0.tar.gz

echo "Creating Shared Util Wily Zip"
mkdir ~/$release_folder/$repo_wily/azure-c-shared-utility_repo
mv ~/$release_folder/$repo_wily/azure-c-shared-utility ~/$release_folder/$repo_wily/azure-c-shared-utility_repo/azure-c-shared-util-0.1.0.0
tar -czf ~/$release_folder/azure-c-shared-util-0.1.0.0.tar.gz ~/$release_folder/$repo_wily/azure-c-shared-utility_repo/azure-c-shared-util-0.1.0.0
mv ~/$release_folder/azure-c-shared-util-0.1.0.0.tar.gz ~/$release_folder/$repo_wily/azure-c-shared-utility_repo/azure-c-shared-util-0.1.0.0.tar.gz

echo "Creating Shared Util Jessie Zip"
mkdir ~/$release_folder/$repo_jessie/azure-c-shared-utility_repo
mv ~/$release_folder/$repo_jessie/azure-c-shared-utility ~/$release_folder/$repo_jessie/azure-c-shared-utility_repo/azure-c-shared-util-0.1.0.0
tar -czf ~/$release_folder/azure-c-shared-util-0.1.0.0.tar.gz ~/$release_folder/$repo_jessie/azure-c-shared-utility_repo/azure-c-shared-util-0.1.0.0
mv ~/$release_folder/azure-c-shared-util-0.1.0.0.tar.gz ~/$release_folder/$repo_jessie/azure-c-shared-utility_repo/azure-c-shared-util-0.1.0.0.tar.gz

echo "Creating Iothub Vivid zip"
mv ~/$release_folder/$repo_vivid/azure-iot-sdks/c ~/$release_folder/$repo_vivid/azure-iot-sdks/azure-iot-sdks-0.1.0
tar -czf ~/$release_folder/azure-iot-sdks-0.1.0.tar.gz ~/$release_folder/$repo_vivid/azure-iot-sdks/azure-iot-sdks-0.1.0
mv ~/$release_folder/azure-iot-sdks-0.1.0.tar.gz ~/$release_folder/$repo_vivid/azure-iot-sdks/azure-iot-sdks-0.1.0.tar.gz

echo "Creating Iothub Wily Zip"
mv ~/$release_folder/$repo_wily/azure-iot-sdks/c ~/$release_folder/$repo_wily/azure-iot-sdks/azure-iot-sdks-0.1.0.0
tar -czf ~/$release_folder/azure-iot-sdks-0.1.0.0.tar.gz ~/$release_folder/$repo_wily/azure-iot-sdks/azure-iot-sdks-0.1.0.0
mv ~/$release_folder/azure-iot-sdks-0.1.0.0.tar.gz ~/$release_folder/$repo_wily/azure-iot-sdks/azure-iot-sdks-0.1.0.0.tar.gz

echo "Creating uAmqp Vivid zip"
mkdir ~/$release_folder/$repo_vivid/azure-uamqp-c_repo
mv ~/$release_folder/$repo_vivid/azure-uamqp-c ~/$release_folder/$repo_vivid/azure-uamqp-c_repo/azure-uamqp-c-0.1.0
tar czf ~/$release_folder/azure-uamqp-c-0.1.0.tar.gz ~/$release_folder/$repo_vivid/azure-uamqp-c_repo/azure-uamqp-c-0.1.0
mv ~/$release_folder/azure-uamqp-c-0.1.0.tar.gz ~/$release_folder/$repo_vivid/azure-uamqp-c_repo/azure-uamqp-c-0.1.0.tar.gz

echo "Creating uAmqp Wily Zip"
mkdir ~/$release_folder/$repo_wily/azure-uamqp-c_repo
mv ~/$release_folder/$repo_wily/azure-uamqp-c ~/$release_folder/$repo_wily/azure-uamqp-c_repo/azure-uamqp-c-0.1.0.0
tar czf ~/$release_folder/azure-uamqp-c-0.1.0.0.tar.gz ~/$release_folder/$repo_wily/azure-uamqp-c_repo/azure-uamqp-c-0.1.0.0
mv ~/$release_folder/azure-uamqp-c-0.1.0.0.tar.gz ~/$release_folder/$repo_wily/azure-uamqp-c_repo/azure-uamqp-c-0.1.0.0.tar.gz

echo "Creating uMqtt zip"
mkdir ~/$release_folder/$repo_vivid/azure-umqtt-c_repo
mv ~/$release_folder/$repo_vivid/azure-umqtt-c ~/$release_folder/$repo_vivid/azure-umqtt-c_repo/azure-umqtt-c-0.1.0
tar czf ~/$release_folder/azure-umqtt-c-0.1.0.tar.gz ~/$release_folder/$repo_vivid/azure-umqtt-c_repo/azure-umqtt-c-0.1.0
mv ~/$release_folder/azure-umqtt-c-0.1.0.tar.gz ~/$release_folder/$repo_vivid/azure-umqtt-c_repo/azure-umqtt-c-0.1.0.tar.gz

mkdir ~/$release_folder/$repo_wily/azure-umqtt-c_repo
mv ~/$release_folder/$repo_wily/azure-umqtt-c ~/$release_folder/$repo_wily/azure-umqtt-c_repo/azure-umqtt-c-0.1.0.0
tar czf ~/$release_folder/azure-umqtt-c-0.1.0.0.tar.gz ~/$release_folder/$repo_wily/azure-umqtt-c_repo/azure-umqtt-c-0.1.0.0
mv ~/$release_folder/azure-umqtt-c-0.1.0.0.tar.gz ~/$release_folder/$repo_wily/azure-umqtt-c_repo/azure-umqtt-c-0.1.0.0.tar.gz
