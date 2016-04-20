#!/bin/bash
# Copyright (c) Microsoft. All rights reserved.
# Licensed under the MIT license. See LICENSE file in the project root for full license information.

build_root=$(cd "$(dirname "$0")/../../.." && pwd)
cd $build_root

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
          "--build-python" ) save_next_arg=1;;
          * ) ;;
        esac
      fi
    done
}

process_args $*

# instruct C builder to include python library and to skip tests

./c/build_all/linux/build.sh --build-python $PYTHON_VERSION --skip-unittests --skip-e2e-tests $*
[ $? -eq 0 ] || exit $?
cd $build_root

echo copy iothub_client library to samples folder
cp ~/cmake/python/src/iothub_client.so ./python/device/samples/iothub_client.so
echo copy iothub_client_mock library to tests folder
cp ~/cmake/python/test/iothub_client_mock.so ./python/device/tests/iothub_client_mock.so

cd $build_root/python/device/tests/
echo "python${PYTHON_VERSION}" iothub_client_ut.py
"python${PYTHON_VERSION}" iothub_client_ut.py
[ $? -eq 0 ] || exit $?
cd $build_root
