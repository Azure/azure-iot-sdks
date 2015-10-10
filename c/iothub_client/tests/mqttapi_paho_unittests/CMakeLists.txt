#Copyright (c) Microsoft. All rights reserved.
#Licensed under the MIT license. See LICENSE file in the project root for full license information.

#this is CMakeLists.txt for mqttapi_paho_unittests
cmake_minimum_required(VERSION 3.0)

if(NOT ${use_mqtt})
	message(FATAL_ERROR "mqttapi_paho_unittests being generated without mqtt support")
endif()


compileAsC99()
set(theseTestsName mqttapi_paho_unittests)

set(${theseTestsName}_cpp_files
${theseTestsName}.cpp
)

set(${theseTestsName}_c_files
../../adapters/mqttapi_paho.c
)

set(${theseTestsName}_h_files
)

build_test_artifacts(${theseTestsName} ON)