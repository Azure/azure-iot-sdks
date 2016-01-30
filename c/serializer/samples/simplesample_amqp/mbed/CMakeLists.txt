#Copyright (c) Microsoft. All rights reserved.
#Licensed under the MIT license. See LICENSE file in the project root for full license information.

cmake_minimum_required(VERSION 2.8.11)
project(mbed_simplesample_amqp)

### Data
set(target_project_name "simplesample_amqp")
set(sdk_base_path ../../../../..)
set(mbed_repo_name "simplesample_amqp_bld")
set(local_repo_path ${CMAKE_CURRENT_BINARY_DIR}/${mbed_repo_name})
set(compile_mbed_tool ${sdk_base_path}/c/tools/compilembed/bin/release/compilembed.exe)
set(mbed_platform "FRDM-K64F")

set(project_files
		${CMAKE_CURRENT_SOURCE_DIR}/main.cpp
		${CMAKE_CURRENT_SOURCE_DIR}/../*.c 
		${CMAKE_CURRENT_SOURCE_DIR}/../*.h
		${CMAKE_CURRENT_SOURCE_DIR}/../../../../certs/certs.h
		${CMAKE_CURRENT_SOURCE_DIR}/../../../../certs/certs.c
	)

set(common_files 
	)
	
# Add the paterns of the filenames to be deleted before pushing local update to remote repo.
set(exclude_files
	)

### Include CMAKE library for MBED.
if (NOT DEFINED MBED_CMAKE_LIB)
	include(${sdk_base_path}/c/tools/mbed_build/CMakeLists.txt)
endif()

### Main
if(WIN32)
	verifyRequiredParameters()
	verifyRequiredTools()
	cloneMBEDRepository()
	updateMBEDSourceFiles()
	pushMBEDRepository()
	compileMBEDProject()
else()
	message(FATAL_ERROR "Apologies, but the only currently supported platform is Microsoft Windows.")
endif()

