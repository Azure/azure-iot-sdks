#!/bin/bash
# Copyright (c) Microsoft. All rights reserved.
# Licensed under the MIT license. See LICENSE file in the project root for full license information.

build_root=$(cd "$(dirname "$0")/../../.." && pwd)
cd $build_root

# instruct C builder to include javawrapper library and to skip tests

./c/build_all/linux/build.sh --build-javawrapper --skip-unittests --skip-e2e-tests #-x
[ $? -eq 0 ] || exit $?

echo compile java samples

cd javawrapper/device/samples/send-event-sample/
mvn install
cp ~/cmake/javawrapper/libiothub_client_java.so ./target
cd $build_root 

cd javawrapper/device/samples/send-receive-sample/
mvn install
cp ~/cmake/javawrapper/libiothub_client_java.so ./target
cd $build_root 

cd javawrapper/device/samples/direct_call_of_wrapped_functions/
mvn install
cp ~/cmake/javawrapper/libiothub_client_java.so ./target
cd $build_root 

cd javawrapper/device/samples/send-serialized-event/
mvn install
cp ~/cmake/javawrapper/libiothub_client_java.so ./target
cd $build_root 

source ./javawrapper/build_all/linux/exportLibPath.sh

cd javawrapper/device/test/
mvn test
cp ~/cmake/javawrapper/libiothub_client_mock.so ./target
cd $build_root 




