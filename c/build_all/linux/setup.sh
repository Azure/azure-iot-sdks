#!/bin/bash

#This script updates a fresh Ubuntu installation with all the dependent
# components necessary to use the IoT Client SDK for C.

repo_name_from_uri()
{
    echo "$1" | sed -e 's|/$||' -e 's|:*/*\.git$||' -e 's|.*[/:]||g'
}

scriptdir=$(cd "$(dirname "$0")" && pwd)
deps="curl libcurl4-openssl-dev uuid-dev uuid g++ make cmake git unzip openjdk-7-jre libssl-dev"
repo="https://github.com/Azure/azure-iot-sdks.git"
repo_name=$(repo_name_from_uri $repo)

push_dir () { pushd $1 > /dev/null; }
pop_dir () { popd $1 > /dev/null; }

repo_exists ()
{
    push_dir "$scriptdir"
    [ "$(git rev-parse --is-inside-work-tree)" == "true" ] || return 1
    origin=$(git config remote.origin.url) || return 1
    actual_name=$(repo_name_from_uri $origin)
    [ "${repo_name,,}" == "${actual_name,,}" ] || return 1
    pop_dir
}

deps_install ()
{
    sudo apt-get update
    sudo apt-get install -y $deps
}

clone_source ()
{
    git clone $repo
}

install_proton_from_source ()
{
    #Skip if already installed
    code_dir="$HOME/qpid-proton"
    if [ -d $code_dir ] && [ -d "$code_dir/proton-c" ] && [ -f "$code_dir/build/install_manifest.txt" ]
    then
        echo "Proton already installed."
        return 0
    fi

    sudo bash c/build_all/linux/build_proton.sh --install /usr
    [ $? -eq 0 ] || return $?
}

deps_install

if repo_exists
then
    echo "Repo $repo_name already cloned"
    push_dir "$(git rev-parse --show-toplevel)"
else
    clone_source || exit 1
    push_dir "$repo_name"
fi

install_proton_from_source || exit 1
pop_dir
