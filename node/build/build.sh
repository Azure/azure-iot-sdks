#!/bin/bash

# Copyright (c) Microsoft. All rights reserved.
# Licensed under the MIT license. See LICENSE file in the project root for full license information.

min_output=
integration_tests=
e2e_tests=
npm_command=

node_root=$(cd "$(dirname "$0")/.." && pwd)
cd $node_root

usage ()
{
    echo "Lint code and run tests."
    echo "build.sh [options]"
    echo "options"
    echo " --min                 minimize display output"
    echo " --integration-tests   run integration tests too (unit tests always run)"
    echo " --e2e-tests           run end-to-end tests too (unit tests always run)"
    exit 1
}

process_args ()
{
    min_output=0
    integration_tests=0
    e2e_tests=0

    for arg in $*
    do
        case "$arg" in
            "--min" ) min_output=1;;
            "--integration-tests" ) integration_tests=1;;
            "--e2e-tests" ) e2e_tests=1;;
            * ) usage;;
        esac
    done

    case "$min_output$integration_tests" in
        "00" ) npm_command="npm -s test";;
        "01" ) npm_command="npm -s run lint && npm -s run alltest";;
        "10" ) npm_command="npm -s run lint && npm -s run unittest-min";;
        "11" ) npm_command="npm -s run ci";;
    esac
}

lint_and_test ()
{
    cd "$1"
    pwd
    eval $npm_command
}

create_test_device()
{
    export IOTHUB_X509_DEVICE_ID=x509device-node-$RANDOM
    node $node_root/build/tools/create_device_certs.js --connectionString $IOTHUB_CONNECTION_STRING --deviceId $IOTHUB_X509_DEVICE_ID
    export IOTHUB_X509_CERTIFICATE=$node_root/$IOTHUB_X509_DEVICE_ID-cert.pem
    export IOTHUB_X509_KEY=$node_root/$IOTHUB_X509_DEVICE_ID-key.pem
}

delete_test_device()
{
    node $node_root/../tools/iothub-explorer/iothub-explorer.js $IOTHUB_CONNECTION_STRING delete $IOTHUB_X509_DEVICE_ID
    rm $IOTHUB_X509_CERTIFICATE
    rm $IOTHUB_X509_KEY
}

cleanup_and_exit()
{
    delete_test_device
    exit $1
}

process_args $*

echo ""
echo "-- create test device --"
create_test_device

echo ""
if [ $integration_tests -eq 0 ]
then
    echo "-- Linting and running unit tests --"
else
    echo "-- Linting and running unit + integration tests --"
fi
echo ""

lint_and_test $node_root/common/core
[ $? -eq 0 ] || cleanup_and_exit $?

lint_and_test $node_root/common/transport/amqp
[ $? -eq 0 ] || cleanup_and_exit $?

lint_and_test $node_root/common/transport/http
[ $? -eq 0 ] || cleanup_and_exit $?

lint_and_test $node_root/common/transport/mqtt
[ $? -eq 0 ] || cleanup_and_exit $?

lint_and_test $node_root/device/core
[ $? -eq 0 ] || cleanup_and_exit $?

lint_and_test $node_root/device/transport/amqp
[ $? -eq 0 ] || cleanup_and_exit $?

lint_and_test $node_root/device/transport/amqp-ws
[ $? -eq 0 ] || cleanup_and_exit $?

lint_and_test $node_root/device/transport/http
[ $? -eq 0 ] || cleanup_and_exit $?

lint_and_test $node_root/device/transport/mqtt
[ $? -eq 0 ] || cleanup_and_exit $?

lint_and_test $node_root/service
[ $? -eq 0 ] || cleanup_and_exit $?

if [ $e2e_tests -eq 1 ]
then
    lint_and_test $node_root/e2etests
    [ $? -eq 0 ] || cleanup_and_exit $?
fi

cd $node_root/../tools/iothub-explorer
npm -s test

cleanup_and_exit $?
