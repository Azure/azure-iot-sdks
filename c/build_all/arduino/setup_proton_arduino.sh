#!/bin/bash

set -e

proton_repo="https://github.com/dcristoloveanu/qpid-proton"
proton_branch="0.9-IoTClient"
build_root=

arduino_cmake_repo="https://github.com/queezythegreat/arduino-cmake"
arduino_cmake_branch="master"
arduino_install_root="$HOME"
arduino_install_dir="arduino-cmake"

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
    echo 'build_proton.sh [options]'
    echo 'options'
    echo ' -s, --source    destination directory for proton source'
    echo '                 (default: $HOME/qpid-proton)'
    echo ' -i, --install   destination root directory for proton installation'
    echo '                 (default: $HOME)'
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

sync_proton ()
{
    echo Azure IoT SDK has a dependency on apache qpid-proton-c
    echo https://github.com/apache/qpid-proton/blob/master/LICENSE

    read -p "Do you want to install the component (y/n)?" input_var
    if [ "$input_var" == "y" ] || [ "$input_var" == "Y" ]
    then
        echo "preparing qpid proton-c"
    else
        exit 1
    fi

    rm $build_root -r -f
    cd /tmp
    curl -L $proton_repo/archive/$proton_branch.zip -o proton.zip
    mkdir proton
    unzip proton.zip -d proton
    mv ./proton/qpid-proton-$proton_branch/ $build_root/
    rm proton.zip
    rm -rf proton
}

sync_cmake()
{
    if [ -d $arduino_install_root/$arduino_install_dir ]
    then
	echo Arduino-Cmake already installed
        return 1
    fi

    echo Arduino Yun has a dependency on cmake

    read -p "Do you want to install the component (y/n)?" input_var
    if [ "$input_var" == "y" ] || [ "$input_var" == "Y" ]
    then
        echo "preparing cmake"
    else
        exit 1
    fi

    cd /tmp
    curl -L $arduino_cmake_repo/archive/$arduino_cmake_branch.zip -o arduino_cmake.zip
    unzip arduino_cmake.zip -d arduino_cmake
    rm arduino_cmake.zip

    mv arduino_cmake/arduino-cmake-master $arduino_install_root
    cd $arduino_install_root
    mv arduino-cmake-master $arduino_install_dir
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

build_cmake()
{
	echo Building Arduino-Cmake
	cd $arduino_install_root/$arduino_install_dir
	mkdir build
	cd build
	cmake ..
	make
}

build ()
{
    if [ ! -d $build_root/build ]
    then
        mkdir $build_root/build
    fi

    push_dir $build_root/build

    #cmake .. -DCMAKE_TOOLCHAIN_FILE=~/arduino-cmake/cmake/ArduinoToolchain.cmake -DCMAKE_INSTALL_PREFIX="$install_root" -DSYSINSTALL_BINDINGS=ON -DCMAKE_PREFIX_PATH=$TC/include/openssl -DCMAKE_PREFIX_PATH=$TC/lib -DCMAKE_PREFIX_PATH=$TC/include/uuid
    make V=s package/openwrt-qpid-proton/compile
    #make
    make install

    pop_dir
}

process_args $*
echo "Source: $build_root"
echo "Install: $install_root"
#sync_proton
#sync_cmake
#build_cmake

#build_openwrt
build
