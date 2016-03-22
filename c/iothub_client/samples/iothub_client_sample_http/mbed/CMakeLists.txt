#Copyright (c) Microsoft. All rights reserved.
#Licensed under the MIT license. See LICENSE file in the project root for full license information.

cmake_minimum_required(VERSION 2.8.11)

set(mbed_project_base "iothub_client_sample_http" CACHE STRING "The item being built")

### This option would be used by build scripts so to release a project directly to the mbed repo sights.  No compiles
### would occur.
###
### To use add -Drelease_the_project:bool=ON
option(release_the_project "Used to just push the project and adjust the appending of _bld" off)

project(${mbed_project_base})

### Data
set(sdk_base_path ../../../../..)

###
### The compilation environment had the normal project base name with an appended _bld  They will be constantly overwritten.
if (release_the_project)
        set(mbed_repo_name ${mbed_project_base})
    else()
        set(mbed_repo_name ${mbed_project_base}_bld)
endif()

set(local_repo_path ${CMAKE_CURRENT_BINARY_DIR}/${mbed_repo_name})
set(compile_mbed_tool ${sdk_base_path}/c/tools/compilembed/bin/debug/compilembed.exe)

set(mbed_platform "FRDM-K64F")

# seperate out the file lists so that they can used by the release cmake
include (${mbed_project_base}_filelist.txt)

### Include CMAKE library for MBED.
if (NOT DEFINED MBED_CMAKE_LIB)
	include(${sdk_base_path}/c/tools/mbed_build/CMakeLists.txt)
endif()

### Main
if(WIN32)
    if (${release_the_project})
	    releaseMBEDRepository()
	else ()
        compileMBEDRepository()
	endif()
else()
	message(FATAL_ERROR "Apologies, but the only currently supported platform is Microsoft Windows.")
endif()

