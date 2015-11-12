#!/bin/bash
# Copyright (c) Microsoft. All rights reserved.
# Licensed under the MIT license. See LICENSE file in the project root for full license information.

build_root=$(cd "$(dirname "$0")/../.." && pwd)
cd $build_root/build/docs

# -----------------------------------------------------------------------------
# -- Generate C API docs
# -----------------------------------------------------------------------------
doxygen
if [ $? -ne 0 ]
then
    echo Generating C API docs failed.
    exit $?
fi
