#!/bin/bash

set -e

sdk_repo="http://download.linino.org/linino-utils/OpenWrt-SDK-ar71xx-for-linux-x86_64-gcc-4.6-linaro_uClibc-0.9.33.2-32bit.tar.bz2"

install_root="$HOME"
openwrt_folder="openwrt"
openwrt_sdk_folder="sdk"
working_dir=$PWD

usage ()
{
    echo 'setup_sdk.sh [options]'
    echo 'options'
    echo ' -i, --installdir   destination root directory for OpenWRT SDK installation'
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
        #elif [ "$1" == "-i" ] || [ "$1" == "--install" ]
        #then
        #    install_root="$2"
        else
            usage
        fi

        shift
        shift
    done
}

download_proton()
{
  echo "Downloading Proton"
}

download_sdk()
{
  read -p "The download and installation may take some time. Do you want to continue? (Y/N)" input_var
  if [ ! "$input_var" == "y" ] && [ ! "$input_var" == "Y" ]
  then
    echo "Exiting..."
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

  echo "Finished downloading OpenWRT SDK"

  #echo $(dirname .)

  #for entry in "."/*
  #do
  #  echo "$entry"
  #done

	#cd $openwrt_root/$openwrt_folder/$openwrt_sdk_folder

	#./scripts/feeds update -a
	#./scripts/feeds install libopenssl libuuid
	#make package/feeds/base/openssl/compile
	#make package/feeds/base/util-linux/compile

	#echo Copying SDK outputs
	#todo: need to somehow find the output folder
	#export TC=$openwrt_root/$openwrt_folder/$openwrt_sdk_folder/staging_dir/toolchain-mips_34kc_gcc-4.8-linaro_musl-1.1.11

	#    if [ ! -d $TC/include/uuid ]
	#    then
	#	mkdir $TC/include/uuid
	#    fi

	#cp build_dir/target-mips_34kc_musl-1.1.11/util-linux-2.25.2/libuuid/src/uuid.h $TC/include/uuid
	#cp build_dir/target-mips_34kc_musl-1.1.11/util-linux-2.25.2/.libs/libuuid.* $TC/lib
	#cp -r -L build_dir/target-mips_34kc_musl-1.1.11/openssl-1.0.2d/include/openssl $TC/include
	#cp build_dir/target-mips_34kc_musl-1.1.11/openssl-1.0.2d/libcrypto.so* $TC/lib
	#cp build_dir/target-mips_34kc_musl-1.1.11/openssl-1.0.2d/libssl.so* $TC/lib
}

setup_proton()
{
  cd $working_dir
  ./build_proton.sh -s "$install_root/$openwrt_folder/$openwrt_sdk_folder"
}

process_args $*
echo "Install Dir: $install_root/$openwrt_folder/$openwrt_sdk_folder"

download_sdk
setup_proton
