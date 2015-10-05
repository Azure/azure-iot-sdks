#!/bin/bash

if [ -z $1 ]
then
    code_dir="$HOME/qpid-proton"
else
    code_dir="$1"
fi

#don't delete the wrong stuff!!
if [ -d $code_dir ] && [ -d "$code_dir/proton-c" ] && [ -f "$code_dir/build/install_manifest.txt" ]
then
    if [ -w $(head --lines=1 "$code_dir/build/install_manifest.txt") ]
    then
        for delfile in $(cat "$code_dir/build/install_manifest.txt")
        do
            rm --verbose --force $delfile
            rmdir --parents --ignore-fail-on-non-empty $(dirname $delfile) 2> /dev/null
        done

        echo -n "removing $code_dir..."
        rm --recursive --force $code_dir
        [ $? -eq 0 ] && echo "done."
    else
        echo "You don't have rights to uninstall Proton (try 'sudo'?)"
    fi
else
    echo "Didn't find Proton source code at $code_dir"
fi
