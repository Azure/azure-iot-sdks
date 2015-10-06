#Copyright (c) Microsoft. All rights reserved.
#Licensed under the MIT license. See LICENSE file in the project root for full license information.

#this is CMakeLists.txt for iothubtransporthttp_unittests
cmake_minimum_required(VERSION 3.0)

set(iothubtransporthttp_unittests_cpp_files
iothubtransporthttp_unittests.cpp
)

set(iothubtransporthttp_unittests_c_files
../../src/iothubtransporthttp.c

../../../common/src/crt_abstractions.c
)

set(iothubtransporthttp_unittests_h_files
)

include_directories(${COMMON_SRC_FOLDER} ${MICROMOCK_INC_FOLDER} ${TESTRUNNERSWITCHER_INC_FOLDER} ${CTEST_INC_FOLDER} $ENV{VCInstallDir}UnitTest/include)
link_directories($ENV{VCInstallDir}UnitTest/lib)

IF(WIN32)
	#windows needs this define
	add_definitions(-D_CRT_SECURE_NO_WARNINGS)
	add_definitions(-DCPP_UNITTEST -DGB_MEASURE_MEMORY_FOR_THIS -DGB_DEBUG_ALLOC)
ENDIF(WIN32)

add_library(
iothubtransporthttp_unittests SHARED ${iothubtransporthttp_unittests_cpp_files} ${iothubtransporthttp_unittests_h_files} ${iothubtransporthttp_unittests_c_files}
)

target_link_libraries(iothubtransporthttp_unittests micromock ctest)