#!/bin/bash

# Copyright (c) Microsoft. All rights reserved.
# Licensed under the MIT license. See LICENSE file in the project root for full license information.

script_dir=$(cd "$(dirname "$0")" && pwd)
build_root=${script_dir}

pushd ~/emscripten

make

cp emwrapper.js $build_root

popd
