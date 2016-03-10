#!/bin/bash

#This script updates a fresh openSUSE installation with all the dependent
# components necessary to use the IoT Client SDK for C.

#Make sure temporary Git credentials file is ALWAYS deleted
trap 'creds_delete' EXIT

repo_name_from_uri()
{
    echo "$1" | sed -e 's|/$||' -e 's|:*/*\.git$||' -e 's|.*[/:]||g'
}

scriptdir=$(cd "$(dirname "$0")" && pwd)
deps=" make cmake gcc git libcurl-devel swig doxygen gcc-c++ libopenssl-devel"
repo="https://github.com/Azure/azure-iot-sdks.git"
repo_name=$(repo_name_from_uri $repo)
cred=~/cred.$$
user=
pass=

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

creds_read ()
{
    echo "Enter credentials for $repo:"
    read -p "username: " user
    read -p "password (or access token, for 2FA): " -s pass
    echo -e "\n"
}

creds_create ()
{
    protocol=$(expr "$repo" : '\(.*\):\/\/.*')
    host=$(expr "$repo" : '.*:\/\/\([^/]*\)/.*')

    #NOTE: tabs in the heredoc are intentional; don't replace them with spaces
    git credential-store --file $cred store <<-end-credentials
	protocol=$protocol
	host=$host
	username=$user
	password=$pass

	end-credentials
}

creds_delete ()
{
    rm -f $cred
}

deps_install ()
{
    sudo zypper ref
    sudo zypper --non-interactive in $deps
    sudo gem install rspec simplecov
}

clone_source ()
{
    git -c credential.helper="store --file $cred" -c core.askpass=true clone $repo
}

if ! repo_exists
then
    creds_read #read Git credentials up front, so script can be unattended-ish
fi

deps_install

if repo_exists
then
    echo "Repo $repo_name already cloned"
    push_dir "$(git rev-parse --show-toplevel)"
else
    creds_create
    clone_source || exit 1
    push_dir "$repo_name"
    creds_delete
fi

pop_dir