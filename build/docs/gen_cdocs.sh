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

# -----------------------------------------------------------------------------
# -- Fix up the line feeds in the generated C docs so that just line
# -- feed changes aren't flagged as changes by git. We don't care if this
# -- fails though.
# -----------------------------------------------------------------------------
echo Fixing line feeds in generated docs...
node fix-crlf.js
