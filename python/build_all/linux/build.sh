#!/bin/bash
# Copyright (c) Microsoft. All rights reserved.
# Licensed under the MIT license. See LICENSE file in the project root for full license information.

build_root=$(cd "$(dirname "$0")/../../.." && pwd)
cd $build_root

# instruct C builder to include python library and to skip tests

./c/build_all/linux/build.sh --build-python --skip-unittests --skip-e2e-tests #-x
[ $? -eq 0 ] || exit $?

echo copy iothub_client library to sample folder
cp ~/cmake/python/iothub_client.so ./python/device/samples/iothub_client.so
cp ~/cmake/python/iothub_client_mock.so ./python/device/tests/iothub_client_mock.so


