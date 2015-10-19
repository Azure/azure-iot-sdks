#!/bin/bash
# Copyright (c) Microsoft. All rights reserved.
# Licensed under the MIT license. See LICENSE file in the project root for full license information.

build_root=$(cd "$(dirname "$0")/../.." && pwd)
cd $build_root/node

# -----------------------------------------------------------------------------
# -- Generate device JS docs
# -----------------------------------------------------------------------------
doc_target_dir=$build_root/node/device/doc/api_reference
if [ -d $doc_target_dir ]
then
	rm -rf $doc_target_dir
fi
mkdir -p $doc_target_dir
jsdoc -c ../build/docs/jsdoc-device.conf.json -d $doc_target_dir
