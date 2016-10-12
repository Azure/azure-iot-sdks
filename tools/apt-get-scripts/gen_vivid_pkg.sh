#!/bin/bash

set -e

# Name of the location to update repo
repo_dir="debian_release/apt-get-vivid"

# Fix up shared-Util
echo "Preparing azure-c-shared-utility vivid package"
cd ~/$repo_dir/c-utility_repo/azure-c-shared-util-0.1.0
dh_make -y -l -c mit -e aziotsdklinux@outlook.com -f ../azure-c-shared-util-0.1.0.tar.gz
rm -r debian
cp -r build_all/packaging/linux/debian ./
cp debian/changelog-vivid debian/changelog
dpkg-buildpackage -us -uc


# Release Iot-Sdk
echo "Preparing azure-iot-sdk vivid package"
cd ~/$repo_dir/azure-iot-sdks/azure-iot-sdks-0.1.0
dh_make -y -l -c mit -e aziotsdklinux@outlook.com -f ../azure-iot-sdks-0.1.0.tar.gz
rm -r debian
cp -r build_all/packaging/linux/debian ./
cp debian/changelog-vivid debian/changelog
dpkg-buildpackage -us -uc


# Release uAmqp
echo "Preparing azure-uamqp-c vivid package"
cd ~/$repo_dir/uamqp_repo/azure-uamqp-c-0.1.0
dh_make -y -l -c mit -e aziotsdklinux@outlook.com -f ../azure-uamqp-c-0.1.0.tar.gz
rm -r debian
cp -r build_all/packaging/linux/debian ./
cp debian/changelog-vivid debian/changelog
dpkg-buildpackage -us -uc


# Release uMqtt
echo "Preparing azure-umqtt-c vivid package"
cd ~/$repo_dir/umqtt_repo/azure-umqtt-c-0.1.0
dh_make -y -l -c mit -e aziotsdklinux@outlook.com -f ../azure-umqtt-c-0.1.0.tar.gz
rm -r debian
cp -r build_all/packaging/linux/debian ./
cp debian/changelog-vivid debian/changelog
dpkg-buildpackage -us -uc

