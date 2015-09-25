#!/bin/bash
# Copyright (c) Microsoft. All rights reserved.
# Licensed under the MIT license. See LICENSE file in the project root for full license information.

build_root=$(cd "$(dirname "$0")/.." && pwd)

# -- Java --
export JAVA_HOME=/usr/lib/jvm/java-7-openjdk-amd64
cd $build_root/java/device
mvn verify
[ $? -eq 0 ] || exit $?

