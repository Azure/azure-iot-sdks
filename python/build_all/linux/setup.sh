#!/bin/bash
# Copyright (c) Microsoft. All rights reserved.
# Licensed under the MIT license. See LICENSE file in the project root for full license information.

# This script updates a fresh Ubuntu installation with all the dependent
# components necessary to use the IoT Client SDK for C and Python.

build_root=$(cd "$(dirname "$0")/../../.." && pwd)
cd $build_root

# instruct C setup to install all dependent libraries

./c/build_all/linux/setup.sh 
[ $? -eq 0 ] || exit $?

PYTHON_VERSION=2.7

process_args()
{
    save_next_arg=0
    
    for arg in $*
    do
      if [ $save_next_arg == 1 ]
      then
        PYTHON_VERSION="$arg"
        save_next_arg=0
      else
        case "$arg" in
          "--python-version" ) save_next_arg=1;;
          * ) ;;
        esac
      fi
    done
}

process_args $*

scriptdir=$(cd "$(dirname "$0")" && pwd)
deps="python${PYTHON_VERSION}-dev libboost-python-dev"

deps_install ()
{
    sudo apt-get update
    sudo apt-get install -y $deps
}

deps_install

