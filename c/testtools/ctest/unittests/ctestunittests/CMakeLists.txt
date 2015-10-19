#Copyright (c) Microsoft. All rights reserved.
#Licensed under the MIT license. See LICENSE file in the project root for full license information.

cmake_minimum_required(VERSION 3.0)
#this is unittests for ctest

compileAsC99()

set(ctest_unittests_c_files
assertfailurestests.c
assertsuccesstests.c
ctestunittests.c
simpletestsuiteonetest.c
simpletestsuitetwotests.c
testfunctioncleanuptests.c
testfunctioninitializetests.c
testsuiteinitializecleanuptests.c
)

set(ctest_unittests_cpp_files
simpletestsuiteonetestcpp.cpp
)

set(ctest_unittests_h_files
)

include_directories(${CTEST_INC_FOLDER})

IF(WIN32)
	#windows needs this define
	add_definitions(-D_CRT_SECURE_NO_WARNINGS)
ENDIF(WIN32)

add_executable(
ctest_unittests ${ctest_unittests_c_files} ${ctest_unittests_cpp_files} ${ctest_unittests_h_files}
)

target_link_libraries(ctest_unittests ctest)

if(NOT ${skip_unittests})
	add_test(NAME ctest_unittests COMMAND ctest_unittests)
endif()