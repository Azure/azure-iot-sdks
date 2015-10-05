#!/bin/bash

set -e

proton_repo="https://github.com/dcristoloveanu/qpid-proton.git"
proton_branch="0.9-IoTClient"
build_root=

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
    mkdir $build_root
    git clone -b $proton_branch $proton_repo $build_root
}

build ()
{
    if [ ! -d $build_root/build ]
    then
        mkdir $build_root/build
    fi

    push_dir $build_root/build

    cmake .. -DCMAKE_INSTALL_PREFIX="$install_root" -DSYSINSTALL_BINDINGS=ON
    make
    make install

    pop_dir
}

process_args $*
echo "Source: $build_root"
echo "Install: $install_root"
sync_proton
build
