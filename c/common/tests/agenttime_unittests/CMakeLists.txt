#Copyright (c) Microsoft. All rights reserved.
#Licensed under the MIT license. See LICENSE file in the project root for full license information.

#this is CMakeLists.txt for agenttime_unittests
cmake_minimum_required(VERSION 3.0)

set(agenttime_unittests_cpp_files
agenttime_unittests.cpp
)

set(agenttime_unittests_c_files
../../adapters/agenttime.c
)


set(agenttime_unittests_h_files
)

include_directories(${MICROMOCK_INC_FOLDER} ${TESTRUNNERSWITCHER_INC_FOLDER} ${CTEST_INC_FOLDER} $ENV{VCInstallDir}UnitTest/include)
link_directories($ENV{VCInstallDir}UnitTest/lib)

IF(WIN32)
	#windows needs this define
	add_definitions(-D_CRT_SECURE_NO_WARNINGS)
	add_definitions(-DCPP_UNITTEST)
ENDIF(WIN32)


add_library(
agenttime_unittests SHARED ${agenttime_unittests_cpp_files} ${agenttime_unittests_h_files} ${agenttime_unittests_c_files}
)

target_link_libraries(agenttime_unittests micromock ctest)