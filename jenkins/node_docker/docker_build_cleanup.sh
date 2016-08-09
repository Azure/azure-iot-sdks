#!/bin/bash

build_root=$(cd "$(dirname "$0")/../.." && pwd)
dockerdir=$build_root/jenkins/node_docker

# Stop all containers
docker-compose -f $dockerdir/docker-compose.yml down

# Remove containers
docker-compose -f $dockerdir/docker-compose.yml ps -q | xargs docker rm -f

# Remove temporary images used by containers
docker images -q nodebuild | xargs docker rmi