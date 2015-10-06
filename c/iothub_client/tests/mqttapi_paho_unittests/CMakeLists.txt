#Copyright (c) Microsoft. All rights reserved.
#Licensed under the MIT license. See LICENSE file in the project root for full license information.

#this is CMakeLists.txt for mqttapi_paho_unittests
cmake_minimum_required(VERSION 3.0)

set(mqttapi_paho_unittests_cpp_files
mqttapi_paho_unittests.cpp
)

set(mqttapi_paho_unittests_c_files
../../adapters/mqttapi_paho.c
)

set(iothubmessage_unittests_h_files
)

include_directories(${COMMON_SRC_FOLDER} ${MICROMOCK_INC_FOLDER} ${TESTRUNNERSWITCHER_INC_FOLDER} ${CTEST_INC_FOLDER} $ENV{VCInstallDir}UnitTest/include)
link_directories($ENV{VCInstallDir}UnitTest/lib)

IF(WIN32)
	#windows needs this define
	add_definitions(-D_CRT_SECURE_NO_WARNINGS)
	add_definitions(-DCPP_UNITTEST -DGB_MEASURE_MEMORY_FOR_THIS -DGB_DEBUG_ALLOC)
ENDIF(WIN32)

add_library(
mqttapi_paho_unittests SHARED ${mqttapi_paho_unittests_cpp_files} ${mqttapi_paho_unittests_h_files} ${mqttapi_paho_unittests_c_files}
)

target_link_libraries(mqttapi_paho_unittests micromock ctest)