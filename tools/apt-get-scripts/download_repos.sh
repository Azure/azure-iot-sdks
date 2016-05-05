#!/bin/bash

set -e

repo_dir="apt-get-orig"
release_folder="debian_release"

# Get to the home directory
cd ~

#If the apt-get-project exists then remove it
if [ -d $repo_dir ]; then
        rm -rf $repo_dir
fi

#Make the directory
mkdir $release_folder
cd $release_folder
mkdir $repo_dir
cd "$repo_dir"

git clone -b develop https://github.com/Azure/azure-iot-sdks.git --recursive
git clone -b develop https://github.com/Azure/azure-c-shared-utility.git --recursive
git clone -b develop https://github.com/Azure/azure-uamqp-c.git --recursive
git clone -b develop https://github.com/Azure/azure-umqtt-c.git --recursive
