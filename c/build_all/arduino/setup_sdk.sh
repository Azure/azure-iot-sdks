#!/bin/bash

set -e

sdk_repo="https://downloads.openwrt.org/snapshots/trunk/ar71xx/generic/OpenWrt-SDK-ar71xx-generic_gcc-4.8-linaro_musl-1.1.11.Linux-x86_64.tar.bz2"

openwrt_root="$HOME"
openwrt_folder="openwrt"
openwrt_sdk_folder="sdk"

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
    #echo ' -s, --source    destination directory for proton source'
    #echo '                 (default: $HOME/qpid-proton)'
    #echo ' -i, --install   destination root directory for proton installation'
    #echo '                 (default: $HOME)'
    exit 1
}

process_args ()
{
    build_root="$HOME/qpid-proton"
    install_root="$HOME"

    while [ ! -z "$1" ] && [ ! -z "$2" ]
    do
        if [ "$1" == "-s" ] || [ "$1" == "--source" ]
        then
            build_root="$2"
        elif [ "$1" == "-i" ] || [ "$1" == "--install" ]
        then
            install_root="$2"
        else
            usage
        fi

        shift
        shift
    done

    if [ ! -z "$1" ] && [ -z "$2" ]
    then #odd number of arguments
        usage
    fi
}

build_openwrt()
{
	echo Build OpenWRT SDK

	cd $openwrt_root/$openwrt_folder/$openwrt_sdk_folder

	./scripts/feeds update -a
	./scripts/feeds install libopenssl libuuid
	make package/feeds/base/openssl/compile
	make package/feeds/base/util-linux/compile

	echo Copying SDK outputs
#todo: need to somehow find the output folder
	export TC=$openwrt_root/$openwrt_folder/$openwrt_sdk_folder/staging_dir/toolchain-mips_34kc_gcc-4.8-linaro_musl-1.1.11

	    if [ ! -d $TC/include/uuid ]
	    then
		mkdir $TC/include/uuid
	    fi

	cp build_dir/target-mips_34kc_musl-1.1.11/util-linux-2.25.2/libuuid/src/uuid.h $TC/include/uuid
	cp build_dir/target-mips_34kc_musl-1.1.11/util-linux-2.25.2/.libs/libuuid.* $TC/lib
	cp -r -L build_dir/target-mips_34kc_musl-1.1.11/openssl-1.0.2d/include/openssl $TC/include
	cp build_dir/target-mips_34kc_musl-1.1.11/openssl-1.0.2d/libcrypto.so* $TC/lib
	cp build_dir/target-mips_34kc_musl-1.1.11/openssl-1.0.2d/libssl.so* $TC/lib
}

process_args $*
echo "Source: $build_root"
echo "Install: $install_root"

build_openwrt
