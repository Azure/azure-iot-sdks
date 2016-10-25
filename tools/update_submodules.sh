#!/bin/bash

#Copyright (c) Microsoft. All rights reserved.
# Licensed under the MIT license. See LICENSE file in the project root for full license information.
set -eou pipefail

function run_git_cmd()
{
    $1
    if [[ $? -ne 0 ]]; then
        echo "<x> Error In: \"$1\""
        exit $?
    else
        echo "<*>git Command Passed: \"$1\""
    fi
}

commit_id=$1
src_repo=$2

if [[ "$#" -ne 2 || -z "$commit_id" || -z "$src_repo" ]] ; then
    echo "CommitId and Source Repo Are Required."
    echo "Usage: $(basename $0) CommitId SourceRepo"
    exit -1
fi

if [[ ! $(git rev-parse --is-inside-work-tree 2> /dev/null) ]]; then
    echo "Current folder not a git repository!"
    exit -1
fi                

#echo "************************************************************************"
#echo "Sync all urls, init, and update all submodules
#echo "Update submodule options: 1) Update to what the superproject pointing to
#echo "                          2) Update to the head/latest of all submodules
#echo "Currently option 1) is supported, later option 2) can be added and/or"
#echo "allow the selection of submodule update method."
#echo "CommitId Options: 1) CommitId is tracked on origin but not merged into develop"
#ech  "                  2) CommitId is already merged into develop"
#echo "Currently option 1) is supported, later option 2) can be added and/or"
#echo "make the CommitId an optional argument."
#echo "************************************************************************"

run_git_cmd "git submodule sync"
run_git_cmd "git submodule update --init --recursive"
git submodule foreach --recursive | tac | sed 's/Entering //' | xargs -n 1 -I% bash -c "update_parent.sh % $*"
git submodule foreach --recursive git status