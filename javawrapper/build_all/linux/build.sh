#!/bin/bash
# Copyright (c) Microsoft. All rights reserved.
# Licensed under the MIT license. See LICENSE file in the project root for full license information.

build_root=$(cd "$(dirname "$0")/../../.." && pwd)
cd $build_root

# instruct C builder to include javawrapper library and to skip tests

./c/build_all/linux/build.sh --build-javawrapper --skip-unittests #-x
[ $? -eq 0 ] || exit $?

echo compile javawrapper and samples

source ./javawrapper/build_all/linux/exportLibPath.sh

cd javawrapper/device/
mvn install
cp ./../../c/cmake/iotsdk_linux/javawrapper/libiothub_client_java.so ./samples/direct_call_of_wrapped_functions/target
cp ./../../c/cmake/iotsdk_linux/javawrapper/libiothub_client_java.so ./samples/send-receive-sample/target
cp ./../../c/cmake/iotsdk_linux/javawrapper/libiothub_client_java.so ./samples/send-receive-sample-x509/target
cp ./../../c/cmake/iotsdk_linux/javawrapper/libiothub_client_java.so ./samples/send-event-sample/target
cp ./../../c/cmake/iotsdk_linux/javawrapper/libiothub_client_java.so ./samples/send-serialized-event/target
cp ./../../c/cmake/iotsdk_linux/javawrapper/libiothub_client_mock.so ./test
cd $build_root









