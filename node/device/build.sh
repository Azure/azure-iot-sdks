#!/bin/sh
# Copyright (c) Microsoft. All rights reserved.
# Licensed under the MIT license. See LICENSE file in the project root for full license information.

scriptdir="`dirname "$0"`"
jakepath="`pwd`/node_modules/.bin/jake"

cd $scriptdir
if [ ! -f $jakepath ]
then
    echo "This script depends on packages it cannot find. Please run 'npm install' from $scriptdir, then try again."
    echo ""
    exit 1 
fi

$jakepath $*
