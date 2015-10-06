#Copyright (c) Microsoft. All rights reserved.
#Licensed under the MIT license. See LICENSE file in the project root for full license information.

#this is CMakeLists.txt for base64_unittests
cmake_minimum_required(VERSION 3.0)

set(base64_unittests_cpp_files
base64_unittests.cpp
)

set(base64_unittests_c_files
../../src/base64.c
../../src/strings.c
../../src/buffer.c
)

set(base64_unittests_h_files
)

include_directories(${MICROMOCK_INC_FOLDER} ${TESTRUNNERSWITCHER_INC_FOLDER} ${CTEST_INC_FOLDER} $ENV{VCInstallDir}UnitTest/include)
link_directories($ENV{VCInstallDir}UnitTest/lib)

IF(WIN32)
	#windows needs this define
	add_definitions(-D_CRT_SECURE_NO_WARNINGS)
	add_definitions(-DCPP_UNITTEST)
ENDIF(WIN32)


add_library(
base64_unittests SHARED ${base64_unittests_cpp_files} ${base64_unittests_h_files} ${base64_unittests_c_files}
)

target_link_libraries(base64_unittests micromock ctest)