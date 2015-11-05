#!/bin/bash

set -e

#sh ../linux/build_proton.sh

proton_repo="https://github.com/dcristoloveanu/qpid-proton"
proton_branch="0.9-IoTClient"
sdk_root="$HOME/openwrt/sdk"
install_dir="openwrt-qpid-proton"
package_dir="package"
src_dir="src"
working_dir=$PWD
makefile="Makefile.proton"

usage ()
{
    echo 'build_proton.sh [options]'
    echo 'options'
    echo ' -s, --sdkroot   root of the SDK folder'
    echo '                 (default: $HOME/openwrt/sdk)'
    exit 1
}

process_args ()
{
    while [ ! -z "$1" ]
    do
        if [ "$1" == "-s" ] || [ "$1" == "--sdkroot" ]
        then
            sdk_root="$2"
        else
            usage
        fi

        shift
        shift
    done
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

    cd "$sdk_root/$package_dir"

    if [ -d $install_dir ]
    then
      echo "Removing $install_dir"
      rm -r -f $install_dir
    fi

    mkdir $install_dir

    cd $install_dir

    curl -L $proton_repo/archive/$proton_branch.zip -o proton.zip
    unzip proton.zip

    echo "Moving to $src_dir"
    mv qpid-proton-$proton_branch/ $src_dir/

    rm proton.zip

    echo "Moving Makefile to src directory"
    cp "$working_dir/$makefile" ./Makefile

    echo "Patch Proton-C cmake"
    cat "$working_dir/CMakeList.append" >> "$src_dir/proton-c/CMakeLists.txt"
}

build()
{
  cd "$sdk_root"
  make V=s "package/$install_dir/compile"
}

process_args $*

echo "Install: $sdk_root"
sync_proton

echo "Building"
build
