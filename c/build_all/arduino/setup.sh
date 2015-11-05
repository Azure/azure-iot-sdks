#!/bin/bash

set -e

sdk_repo="http://download.linino.org/linino-utils/OpenWrt-SDK-ar71xx-for-linux-x86_64-gcc-4.6-linaro_uClibc-0.9.33.2-32bit.tar.bz2"

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
    echo ' -d, --installdir   destination root directory for OpenWRT SDK installation'
    echo '                 (default: $HOME)'
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

download_sdk()
{
  if [ -d  "$install_root/$openwrt_folder/$openwrt_sdk_folder" ]
	then
    # SDK folder already exists ask to reinstall SDK
    read -p "The OpenWRT SDK ($install_root/$openwrt_folder/$openwrt_sdk_folder) already exists. Do you want to delete and reinstall? (Y/N)" input_var
    if [ ! "$input_var" == "y" ] && [ ! "$input_var" == "Y" ]
    then
      return
    fi
	fi

  read -p "Download and installation of the OpenWRT SDK may take some time. Do you want to continue? (Y/N)" input_var
  if [ ! "$input_var" == "y" ] && [ ! "$input_var" == "Y" ]
  then
    exit 1
  fi

	echo "Downloading OpenWRT SDK to $install_root/$openwrt_folder"

  cd $install_root

	if [ ! -d  $openwrt_folder ]
	then
		mkdir $openwrt_folder
	fi

  # Download the SDK
  cd $openwrt_folder

  if [ -d $openwrt_sdk_folder ]
  then
    rm -r -f $openwrt_sdk_folder
  fi

  wget $sdk_repo -O openwrtsdk.tar.bz2

  tar xvf openwrtsdk.tar.bz2

  # get folders only
  folders=(*/)

  # assuming the only and first file is the name of the extracted archive
  extractedFolder=${folders[0]}

  mv $extractedFolder $openwrt_sdk_folder

  echo Patching SDK CMake
  patch_sdk

  echo "Finished installing the OpenWRT SDK"
}

patch_sdk()
{
  #removing cmake from the toolchain
  mv "$install_root/$openwrt_folder/$openwrt_sdk_folder/staging_dir/host/bin/cmake" "$install_root/$openwrt_folder/$openwrt_sdk_folder/staging_dir/host/bin/cmake.old"
}

setup_proton()
{
  cd $working_dir
  ./build_proton.sh -s "$install_root/$openwrt_folder/$openwrt_sdk_folder"
}

setup_iot()
{
  cd "$install_root/$openwrt_folder/$openwrt_sdk_folder/$package_dir"

  if [ ! -d $iot_dir ]
  then
    mkdir $iot_dir
  fi

  cd $iot_dir

  if [ -d "src" ]
  then
    echo Updating repo
    # src dir exists so update the repo
    cd "src"

    git pull
  else
    echo Cloning repo
    # src doesn't exists so clone
    git clone $iot_repo
    mv $iot_dir src

    # Patching the build files
    cp "$working_dir/$iot_makefile" ./Makefile
  fi
}

build_iot()
{
    echo "Building IoT"
    cd "$install_root/$openwrt_folder/$openwrt_sdk_folder"
    make V=s "package/$iot_dir/compile"
}

process_args $*
echo "Install Dir: $install_root/$openwrt_folder/$openwrt_sdk_folder"

download_sdk
setup_proton
setup_iot
build_iot
