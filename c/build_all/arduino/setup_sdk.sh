#!/bin/bash

set -e

sdk_repo="http://download.linino.org/linino-utils/OpenWrt-SDK-ar71xx-for-linux-x86_64-gcc-4.6-linaro_uClibc-0.9.33.2-32bit.tar.bz2"
sdk_repo_x86="http://download.linino.org/linino-utils/OpenWrt-SDK-ar71xx-for-linux-x86_64-gcc-4.6-linaro_uClibc-0.9.33.2-32bit.tar.bz2"
sdk_repo_x64="http://download.linino.org/linino-utils/OpenWrt-SDK-ar71xx-for-linux-x86_64-gcc-4.6-linaro_uClibc-0.9.33.2-64bit.tar.bz2"
install_root="$HOME"
openwrt_folder="openwrt"
openwrt_sdk_folder="sdk"
working_dir=$PWD
package_dir="package"

MACHINE_TYPE=`uname -m`

usage ()
{
    echo 'setup_sdk.sh [options]'
    echo 'options'
    echo ' -d, --installdir   destination root directory for OpenWRT SDK installation'
    echo '                    (default: $HOME)'
    echo ' -s, --sdk          url of the prebuilt OpenWRT SDK archive'
    echo '                    (default: http://download.linino.org/linino-utils/OpenWrt-SDK-ar71xx-for-linux-x86_64-gcc-4.6-linaro_uClibc-0.9.33.2-32bit.tar.bz2)'
    exit 1
}

process_args ()
{
    while [ ! -z "$1" ] && [ ! -z "$2" ]
    do
        if [ "$1" == "-d" ] || [ "$1" == "--installdir" ]
        then
          install_root="$2"
        elif [ "$1" == "-s" ] || [ "$1" == "--sdk" ]
        then
          sdk_repo="$2"
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

  if [ "$1" == "-s" ] || [ "$1" == "--sdk" ]
  then
    wget $sdk_repo -O openwrtsdk.tar.bz2
  elif [ ${MACHINE_TYPE} == 'x86_64' ]
  then
    wget $sdk_repo_x64 -O openwrtsdk.tar.bz2
  else
    wget $sdk_repo_x86 -O openwrtsdk.tar.bz2
  fi

  tar xvf openwrtsdk.tar.bz2

  # get folders only
  folders=(*/)

  # assuming the only and first file is the name of the extracted archive
  extractedFolder=${folders[0]}

  mv $extractedFolder $openwrt_sdk_folder

  patch_sdk
}

patch_sdk()
{
  #removing cmake from the toolchain
  mv "$install_root/$openwrt_folder/$openwrt_sdk_folder/staging_dir/host/bin/cmake" "$install_root/$openwrt_folder/$openwrt_sdk_folder/staging_dir/host/bin/cmake.old"
}

process_args $*
echo "Installing OpenWRT SDK to $install_root/$openwrt_folder/$openwrt_sdk_folder"

download_sdk

echo "Finished installing the OpenWRT SDK"
