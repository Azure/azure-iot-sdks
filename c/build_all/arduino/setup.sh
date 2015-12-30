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

setup_proton()
{
  cd $working_dir
  echo "Setting up qpid-proton"
  ./build_proton.sh -s "$install_root/$openwrt_folder/$openwrt_sdk_folder"
}

setup_iot()
{
  echo "Setting up the Azure IoT SDK"
  cd "$install_root/$openwrt_folder/$openwrt_sdk_folder/$package_dir"

  if [ ! -d $iot_dir ]
  then
    # IoT does not exists, so make it
    mkdir $iot_dir
  fi

  cd $iot_dir

  if [ -d "src" ]
  then
    echo Updating repo
    # src dir exists so update the repo
    cd "src"

    git pull
    git submodule init
    git submodule update
  else
    echo Cloning repo
    # src doesn't exists so clone
    git clone $iot_repo --recursive
    mv $iot_dir src

    # Patching the build files
    cp "$working_dir/$iot_makefile" ./Makefile
  fi

  read -p "Do you want to build the Azure IoT SDK now? (Y/N)" input_var
  if [ "$input_var" == "y" ] || [ "$input_var" == "Y" ]
  then
    cd $working_dir
    ./build.sh -d "$install_root"
  fi
}

process_args $*

setup_sdk
setup_proton
setup_iot
