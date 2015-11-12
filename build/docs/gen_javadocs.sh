#!/bin/bash
# Copyright (c) Microsoft. All rights reserved.
# Licensed under the MIT license. See LICENSE file in the project root for full license information.

build_root=$(cd "$(dirname "$0")/../.." && pwd)
cd $build_root/build/docs

# -----------------------------------------------------------------------------
# -- Generate Java docs for Device SDK
# -----------------------------------------------------------------------------
echo Generating Java Device SDK docs... 
cd $build_root/java/device/iothub-java-client
mvn -q javadoc:javadoc
if [ $? -ne 0 ]
then
    echo Generating java docs for iothub-java-client failed.
    exit $?
fi

# Move the generated docs to $build_root/java/device/doc/api_reference
doc_target_dir=$build_root/java/device/doc/api_reference
echo Copying Java API docs to $doc_target_dir
if [ -d $doc_target_dir ]
then
	rm -rf $doc_target_dir
fi
mkdir -p $doc_target_dir
cp -r target/site/apidocs/* $doc_target_dir

# -----------------------------------------------------------------------------
# -- Generate Java docs for Service SDK
# -----------------------------------------------------------------------------
echo Generating Java Service SDK docs... 
cd $build_root/java/service/iothub-service-sdk
mvn -q javadoc:javadoc
if [ $? -ne 0 ]
then
    echo Generating java docs for iothub-service-sdk failed.
    exit $?
fi

# Move the generated docs to $build_root/java/service/doc/api_reference
doc_target_dir=$build_root/java/service/doc/api_reference
echo Copying Java API docs to $doc_target_dir
if [ -d $doc_target_dir ]
then
	rm -rf $doc_target_dir
fi
mkdir -p $doc_target_dir
cp -r target/site/apidocs/* $doc_target_dir
