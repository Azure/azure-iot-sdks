#!/bin/bash

build_root=$(cd "$(dirname "$0")/../.." && pwd)
dockerdir=$build_root/build/node_docker

# Build and start all containers
docker-compose -f $dockerdir/docker-compose.yml down

docker-compose -f $dockerdir/docker-compose.yml ps -q | xargs docker rm -f
docker images -q nodebuild | xargs docker rmi

exit 1
