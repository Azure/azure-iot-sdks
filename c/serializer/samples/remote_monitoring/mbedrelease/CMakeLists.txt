#Copyright (c) Microsoft. All rights reserved.
#Licensed under the MIT license. See LICENSE file in the project root for full license information.

cmake_minimum_required(VERSION 2.8.11)
project(mbed_remote_monitoring)

### Data
set(sdk_base_path ../../../../..)
set(mbed_repo_name "remote_monitoring")
set(local_repo_path ${CMAKE_CURRENT_BINARY_DIR}/${mbed_repo_name})

# seperate out the file lists so that they can used by the release cmake
include (../mbed/filelist.txt)

### Include CMAKE library for MBED.
if (NOT DEFINED MBED_CMAKE_LIB)
	include(${sdk_base_path}/c/tools/mbed_build/CMakeLists.txt)
endif()

### Main
if(WIN32)
	releaseMBEDRepository()
else()
	message(FATAL_ERROR "Apologies, but the only currently supported platform is Microsoft Windows.")
endif()

