#!/bin/bash

set -e

sdk_repo="http://download.linino.org/linino-utils/OpenWrt-SDK-ar71xx-for-linux-x86_64-gcc-4.6-linaro_uClibc-0.9.33.2-32bit.tar.bz2"

install_root="$HOME"
openwrt_folder="openwrtsdk"

push_dir ()
{
    pushd $1 > /dev/null
    echo "In ${PWD#$build_root/}"
}

pop_dir ()
{
    popd > /dev/null
}

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
            $install_root="$2"
        #elif [ "$1" == "-i" ] || [ "$1" == "--install" ]
        #then
        #    install_root="$2"
        else
            usage
        fi

        shift
        shift
    done

#    if [ ! -z "$1" ] && [ -z "$2" ]
#    then #odd number of arguments
#        usage
#    fi
}

build_openwrt()
{
	echo Downloading LininoIO SDK

	if [ ! -d $install_root/$openwrt_folder ]
	then
		echo "Creating SDK folder"
		mkdir $install_root/$openwrt_folder
	else
		echo "SDK folder exists"
	fi

cd 

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

process_args $*
echo "Install Dir: $install_root"

build_openwrt
