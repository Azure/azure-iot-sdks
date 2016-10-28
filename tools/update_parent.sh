#!/bin/bash

# Copyright (c) Microsoft. All rights reserved.
# Licensed under the MIT license. See LICENSE file in the project root for full license information.
#
set -eou pipefail

function currentdir_reponame() {
    local reponame=$(git remote show origin -n | grep h.URL | sed 's/.*:*\///;s/.git$//')
    if [[ -z "$reponame" ]]; then
        echo "Unable to get the repository name in: function currentdir_reponame()"
        exit -1
    fi
    echo "$reponame"
}

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

function checkout_develop()
{
    echo "git rev-parse --verify --quiet develop"
    if [[ $? -eq 0 ]]; then
       run_git_cmd "git pull origin develop"
    else
       run_git_cmd "git checkout -b develop origin/develop"
    fi
}

function update_parent_repo() {
    echo "      <***> Inside: function update_parent_repo($1, $2) <***>"
    if [[ "$#" -ne 2 || -z "$1" || -z "$2" ]] ; then
        echo "Submodule name and path are Required."
        echo "Usage: $(basename $0) SubmoduleName SubmodulePath"
        exit -1
    fi
   
    local name="$1"
    local dir=$(basename $2)
    
    pushd ../
    parent_reponame=$(currentdir_reponame)
    branch_name="UpdateSubmoduleReferenceFor_${name}_In_${parent_reponame}"
    echo "      <***> TopicBranchName: $branch_name"

    checkout_develop
    run_git_cmd "git checkout -b $branch_name"
    run_git_cmd "git add -A"
  
    commit_msg="UpdateSubmoduleReference:For_${name}_In_${parent_reponame}"
    echo "      <***> CommitMessage: $commit_msg"

    run_git_cmd "git commit -m $commit_msg"
    #run_git_cmd "git push --set-upstream origin $branch_name"
    popd
}

if [[ "$#" -ne 3 || -z "$1" || -z "$2" || -z "$3" ]] ; then
    echo "CommitId and Source Repo Are Required."
    echo "Usage: $(basename $0) CommitId SourceRepo"
    exit -1
fi

if [[ ! $(git rev-parse --is-inside-work-tree 2> /dev/null) ]]; then
    echo "Current folder not a git repository!"
    exit -1
fi

subdir="$1"
commit_id=$2
src_repo=$3

pushd "$subdir"
reponame=$(currentdir_reponame)

if [[ "$reponame" == "$src_repo" ]]; then
    echo "      <***> Found matching repo name instance <***>"
    checkout_develop 
    run_git_cmd "git pull origin $commit_id"
    update_parent_repo "$reponame" "$subdir"
    echo "           <*><*><*>update parent passed<*><*><*>"
fi