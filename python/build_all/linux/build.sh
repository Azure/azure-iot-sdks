#!/bin/bash
# Copyright (c) Microsoft. All rights reserved.
# Licensed under the MIT license. See LICENSE file in the project root for full license information.

build_root=$(cd "$(dirname "$0")/../../.." && pwd)
cd $build_root

# instruct C builder to include python library and to skip tests

./c/build_all/linux/build.sh --build-python --skip-unittests --skip-e2e-tests $*
[ $? -eq 0 ] || exit $?
cd $build_root

if [[ $* == *--python-3* ]]; then
    echo copy iothub_client library to py3 samples folder
    cp ~/cmake/python/src/iothub_client.so ./python/device/samples/py3/iothub_client.so
    echo copy iothub_client_mock library to tests folder
    cp ~/cmake/python/test/iothub_client_mock.so ./python/device/tests/iothub_client_mock.so
else
    echo copy iothub_client library to samples folder
    cp ~/cmake/python/src/iothub_client.so ./python/device/samples/iothub_client.so
    echo copy iothub_client_mock library to tests folder
    cp ~/cmake/python/test/iothub_client_mock.so ./python/device/tests/iothub_client_mock.so
fi

cd $build_root/python/device/tests/
if [[ $* == *--python-3* ]]; then
python3 iothub_client_ut.py
else
python iothub_client_ut.py
fi
[ $? -eq 0 ] || exit $?
cd $build_root
