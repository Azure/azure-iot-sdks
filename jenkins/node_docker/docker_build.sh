#!/bin/bash

echo ${IOTHUB_DEVICE_ID?Error \$IOTHUB_DEVICE_ID is not defined.}
echo ${IOTHUB_DEVICE_KEY?Error \$IOTHUB_DEVICE_KEY is not defined.}
echo ${IOTHUB_CONNECTION_STRING?Error \$IOTHUB_CONNECTION_STRING is not defined.}
echo ${STORAGE_CONNECTION_STRING?Error \$STORAGE_CONNECTION_STRING is not defined.}

build_root=$(cd "$(dirname "$0")/../.." && pwd)
dockerdir=$build_root/jenkins/node_docker
envdir=$dockerdir/env

mkdir $envdir

for build_container in "aziot-node010" "aziot-node012" "aziot-node4" "aziot-node5" "aziot-node6"
do
    ENV_FILE=$envdir/$build_container.env
    echo IOTHUB_DEVICE_ID=$IOTHUB_DEVICE_ID > $ENV_FILE
    echo IOTHUB_DEVICE_KEY=$IOTHUB_DEVICE_KEY >> $ENV_FILE
    echo IOTHUB_CONNECTION_STRING=$IOTHUB_CONNECTION_STRING >> $ENV_FILE
    echo STORAGE_CONNECTION_STRING=$STORAGE_CONNECTION_STRING >> $ENV_FILE
done

# Build and start all containers
docker-compose -f $dockerdir/docker-compose.yml up --force-recreate


docker-compose  -f $dockerdir/docker-compose.yml ps -q | xargs docker inspect -f '{{ .Config.Image }} finished with ExitCode {{ .State.ExitCode }}'
# Compute exit code: count the number of containers for which the exit code was not 0
BUILD_EXIT_CODE=$(docker-compose -f $dockerdir/docker-compose.yml ps -q | xargs docker inspect -f '{{ .State.ExitCode }}' | grep -v 0 | wc -l)

exit $BUILD_EXIT_CODE
