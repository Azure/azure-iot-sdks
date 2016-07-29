#!/bin/bash
# Copyright (c) Microsoft. All rights reserved.
# Licensed under the MIT license. See LICENSE file in the project root for full license information.

build_root=$(cd "$(dirname "$0")/.." && pwd)
cd $build_root/build/docs

# -----------------------------------------------------------------------------
# -- helper subroutines
# -----------------------------------------------------------------------------
checkExists() {
    if hash $1 2>/dev/null;
    then
        return 1
    else
        echo "$1" not found. Please make sure that "$1" is installed and available in the path.
        exit 1
    fi
}

# -----------------------------------------------------------------------------
# -- Check for environment pre-requisites. This script requires
# -- that the following programs work:
# --     doxygen, git, node, javadoc, mvn, jsdoc
# -----------------------------------------------------------------------------
checkExists git
checkExists node
checkExists npm
checkExists doxygen
checkExists javadoc
checkExists mvn
checkExists jsdoc

# -----------------------------------------------------------------------------
# -- Generate C API docs
# -----------------------------------------------------------------------------
echo Generating C API docs
./gen_cdocs.sh

# -----------------------------------------------------------------------------
# -- Generate JS API docs
# -----------------------------------------------------------------------------
echo Generating Node JS API docs
./gen_jsdocs.sh

# -----------------------------------------------------------------------------
# -- Generate Java API docs
# -----------------------------------------------------------------------------
echo Generating Java API docs
./gen_javadocs.sh
