#Copyright (c) Microsoft. All rights reserved.
#Licensed under the MIT license. See LICENSE file in the project root for full license information.

#this is CMakeLists for micromock
cmake_minimum_required(VERSION 3.0)

compileAsC99()

set(micromock_cpp_files
./src/micromockcharstararenullterminatedstrings.cpp
./src/micromockexception.cpp
./src/micromocktestmutex.cpp
./src/mockcallrecorder.cpp
./src/mockmethodcallbase.cpp
./src/mockvaluebase.cpp
#./src/timediscretemicromock.cpp
)

set(micromock_h_files
./inc/globalmock.h
./inc/macro_utils.h
./inc/micromock.h
./inc/micromockcallmacros.h
./inc/micromockcharstararenullterminatedstrings.h
./inc/micromockcommon.h
./inc/micromockenumtostring.h
./inc/micromockexception.h
./inc/micromocktestmutex.h
./inc/micromocktestrunnerhooks.h
./inc/mock.h
./inc/mockcallargument.h
./inc/mockcallargumentbase.h
./inc/mockcallcomparer.h
./inc/mockcallrecorder.h
./inc/mockmethodcall.h
./inc/mockmethodcallbase.h
./inc/mockresultvalue.h
./inc/mockvalue.h
./inc/mockvaluebase.h
./inc/nicecallcomparer.h
./inc/runtimemock.h 	
./inc/strictorderedcallcomparer.h
./inc/strictunorderedcallcomparer.h
./inc/threadsafeglobalmock.h
./inc/timediscretemicromock.h
./inc/timediscretemicromockcallmacros.h
)

IF(WIN32)
	#windows needs this define
	add_definitions(-D_CRT_SECURE_NO_WARNINGS)
	
	#windows needs to be build 2 flavor of micromock, one for CTEST reporting, one for CPP_UNITTEST
	add_library(micromock_cpp_unittest ${micromock_cpp_files} ${micromock_h_files})
	target_compile_definitions(micromock_cpp_unittest PUBLIC CPP_UNITTEST)
	target_include_directories(micromock_cpp_unittest PUBLIC $ENV{VCInstallDir}UnitTest/include)
	
	add_library(micromock_ctest ${micromock_cpp_files} ${micromock_h_files})
	target_compile_definitions(micromock_ctest PUBLIC USE_CTEST)
else()
	add_library(micromock_ctest ${micromock_cpp_files} ${micromock_h_files})
	target_compile_definitions(micromock_ctest PUBLIC USE_CTEST)
ENDIF()

#these are the include folders
#the following "set" statetement exports across the project a global variable calledMICROMOCK_INC_FOLDER that expands to whatever needs to included when using micromock library
set(MICROMOCK_INC_FOLDER ${CMAKE_CURRENT_LIST_DIR}/inc CACHE INTERNAL "this is what needs to be included if using micromock lib" FORCE)

include_directories(${TESTRUNNERSWITCHER_INC_FOLDER} ${CTEST_INC_FOLDER} ${SAL_INC_FOLDER} ${MICROMOCK_INC_FOLDER} )





add_subdirectory(unittests)