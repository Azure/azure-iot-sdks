#Copyright (c) Microsoft. All rights reserved.
#Licensed under the MIT license. See LICENSE file in the project root for full license information.

#this is CMakeLists.txt for serializer_e2etests
cmake_minimum_required(VERSION 3.0)

compileAsC99()

set(serializer_e2etests_cpp_files
serializer_e2etests.cpp
)

set(serializer_e2etests_c_files
)

set(serializer_e2etests_h_files
MacroE2EModelAction.h
)

include_directories(${IOTHUB_TEST_INC_FOLDER} ${IOTHUB_CLIENT_INC_FOLDER} ${MICROMOCK_INC_FOLDER} ${TESTRUNNERSWITCHER_INC_FOLDER} ${CTEST_INC_FOLDER} $ENV{VCInstallDir}UnitTest/include)
link_directories($ENV{VCInstallDir}UnitTest/lib)

IF(WIN32)
	#windows needs this define
	add_definitions(-D_CRT_SECURE_NO_WARNINGS)
	add_definitions(-DCPP_UNITTEST -DGB_MEASURE_MEMORY_FOR_THIS -DGB_DEBUG_ALLOC)
ENDIF(WIN32)


add_library(
serializer_e2etests SHARED ${serializer_e2etests_cpp_files} ${serializer_e2etests_h_files} ${serializer_e2etests_files}
)
linkProton(serializer_e2etests)
linkPaho(serializer_e2etests)

target_link_libraries(serializer_e2etests serializer micromock ctest iothub_client_http_transport iothub_client iothub_client_amqp_transport iothub_test common
winhttp.lib
)
