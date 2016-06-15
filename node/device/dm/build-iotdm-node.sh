#!/bin/bash

# Copyright (c) Microsoft. All rights reserved.
# Licensed under the MIT license. See LICENSE file in the project root for full license information.

script_dir=$(cd "$(dirname "$0")" && pwd)
build_root=${script_dir}
# BKTODO: use environment variable for find enmscripten another way
emscripten_root=/home/bertk/emsdk_portable

extracloptions=-Wtautological-constant-out-of-range-compare

# BKTODO: find better location.  home directory isn't appropriate anymore.
cmakedir=~/emscripten
rm -r -f ${cmakedir}
mkdir ${cmakedir}
pushd ${cmakedir}

cmake \
    -DCMAKE_TOOLCHAIN_FILE=${emscripten_root}/emscripten/master/cmake/Modules/Platform/Emscripten.cmake \
    -DEMSCRIPTEN_ROOT_PATH=${emscripten_root}/emscripten/master/ \
    -G "Unix Makefiles" \
    -DCMAKE_BUILD_TYPE=Debug \
    -Dno_tls:bool=OFF \
    -Duse_openssl:bool=OFF \
    -Duse_wolfssl:bool=OFF \
    -DcompileOption_C:STRING="$extracloptions" \
    -Drun_e2e_tests:BOOL=OFF \
    -Drun_longhaul_tests:BOOL=OFF \
    -Duse_amqp:BOOL=OFF \
    -Duse_http:BOOL=OFF \
    -Duse_mqtt:BOOL=OFF \
    -Dskip_unittests:BOOL=ON \
    -Dbuild_python:STRING=OFF \
    -Dbuild_javawrapper:BOOL=OFF \
    -Duse_openssl:BOOL=OFF \
    -Duse_schannel:BOOL=OFF \
    -Dbuild_edison_sample:BOOL=OFF \
    -Duse_socketio:BOOL=OFF \
    $build_root

# BKTODO: enable mproc builds
make

cp emwrapper.js $build_root

popd
