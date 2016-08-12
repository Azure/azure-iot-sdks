#!/bin/bash
# Copyright (c) Microsoft. All rights reserved.
# Licensed under the MIT license. See LICENSE file in the project root for full license information.

build_root=$(cd "$(dirname "$0")/.." && pwd)
cd $build_root/node

# Set up links in the npm cache to ensure we're exercising all the code in
# the repo, rather than downloading released versions of our packages from
# npm.
build/dev-setup.sh
[ $? -eq 0 ] || exit $?

# Lint all JavaScript code and run unit + integration tests
build/build.sh --min --integration-tests --e2e-tests
[ $? -eq 0 ] || exit $?

# The 'npm link' commands in this script create symlinks to tracked repo
# files from ignored locations (under ./node_modules). This means a call to
# 'git clean -xdf' will delete tracked files from the repo's working
# directory. To avoid any complications, we'll unlink everything before
# exiting.
build/dev-teardown.sh
[ $? -eq 0 ] || exit $?
