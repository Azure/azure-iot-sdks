#!/bin/bash

set -e

install_root="$HOME"
openwrt_folder="openwrt"
openwrt_sdk_folder="sdk"
working_dir=$PWD
package_dir="package"

iot_dir="azure-iot-sdks"
iot_repo="https://github.com/Azure/azure-iot-sdks.git"
iot_makefile="Makefile.iot"

usage ()
{
    echo 'setup.sh [options]'
    echo 'options'
    echo ' -d, --installdir   OpenWRT SDK root directory'
    echo '                    (default: $HOME)'
    exit 1
}

process_args ()
{
    while [ ! -z "$1" ] && [ ! -z "$2" ]
    do
        if [ "$1" == "-d" ] || [ "$1" == "--installdir" ]
        then
            install_root="$2"
        else
            usage
        fi

        shift
        shift
    done
}

setup_sdk()
{
  # Check if SDK is installed
  #if [ ! -d "$install_root/$openwrt_folder/$openwrt_sdk_folder" ]
  #then
    # Kickoff the SDK installation since it doesn't exists
    ./setup_sdk.sh -d $install_root
  #fi
}

process_args $*

setup_sdk