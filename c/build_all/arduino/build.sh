#!/bin/bash

set -e

sdk_root="$HOME"
openwrt_folder="openwrt"
openwrt_sdk_folder="sdk"
working_dir=$PWD
iot_dir="azure-iot-sdks"

usage ()
{
    echo 'build.sh [options]'
    echo 'options'
    echo ' -d, --sdkroot   OpenWRT SDK root directory'
    echo '                 (default: $HOME)'
    exit 1
}

process_args ()
{
    while [ ! -z "$1" ] && [ ! -z "$2" ]
    do
        if [ "$1" == "-d" ] || [ "$1" == "--sdkroot" ]
        then
            sdk_root="$2"
        else
            usage
        fi

        shift
        shift
    done
}

build_iot()
{
    echo "Building IoT"
    rm -d -f -r $sdk_root/$openwrt_folder/$openwrt_sdk_folder/package/$iot_dir
    mkdir $sdk_root/$openwrt_folder/$openwrt_sdk_folder/package/$iot_dir
    mkdir $sdk_root/$openwrt_folder/$openwrt_sdk_folder/package/$iot_dir/src
    cp -f "$working_dir/Makefile.iot" "$sdk_root/$openwrt_folder/$openwrt_sdk_folder/package/$iot_dir/Makefile"
    cd "$working_dir/../../.."
    cp -R -f . "$sdk_root/$openwrt_folder/$openwrt_sdk_folder/package/$iot_dir/src"
    cd "$sdk_root/$openwrt_folder/$openwrt_sdk_folder"
    make V=s "package/$iot_dir/compile"
}

process_args $*

build_iot