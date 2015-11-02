#Copyright (c) Microsoft. All rights reserved.
#Licensed under the MIT license. See LICENSE file in the project root for full license information.

#this is CMakeLists for ctest.

compileAsC99()

set(ctest_c_files
./src/ctest.c
)

set(ctest_h_files
./inc/ctest.h
./inc/ctest_macros.h
)

#these are the include folders
#the following "set" statetement exports across the project a global variable called CTEST_INC_FOLDER that expands to whatever needs to included when using ctest library
set(CTEST_INC_FOLDER ${CMAKE_CURRENT_LIST_DIR}/inc CACHE INTERNAL "this is what needs to be included if using ctest lib" FORCE)

include_directories(${CTEST_INC_FOLDER})

IF(WIN32)
	#windows needs this define
	add_definitions(-D_CRT_SECURE_NO_WARNINGS)
ENDIF(WIN32)

add_library(
ctest ${ctest_c_files} ${ctest_h_files}
)

add_subdirectory(unittests)