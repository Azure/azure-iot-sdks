#Copyright (c) Microsoft. All rights reserved.
#Licensed under the MIT license. See LICENSE file in the project root for full license information.

#this is CMakeLists.txt for cpp/device/iothub_client

cmake_minimum_required(VERSION 2.8.11)

project(iothub_client_cpp)

FILE(GLOB_RECURSE iothub_client_cpp_files src/*.cpp)
FILE(GLOB_RECURSE iothub_client_hpp_files inc/*.hpp)

include_directories(src)
include_directories(inc)

add_library(iothub_client_cpp
    ${iothub_client_cpp_files}
    ${iothub_client_hpp_files}
)

set_property(TARGET ${CMAKE_PROJECT_NAME} PROPERTY CXX_STANDARD 11)

add_subdirectory(../../../c ${CMAKE_CURRENT_BINARY_DIR}/iothub_client)
include_directories(../../../c/iothub_client/inc)
include_directories(../../../c/azure-c-shared-utility/inc)

target_link_libraries(${CMAKE_PROJECT_NAME}
	iothub_client
	iothub_client_amqp_transport
	iothub_client_mqtt_transport
	iothub_client_http_transport
	aziotsharedutil
	uamqp
	pthread
	curl
	ssl
	crypto
)

linkSharedUtil(${CMAKE_PROJECT_NAME})
linkUAMQP(${CMAKE_PROJECT_NAME})
linkHttp(${CMAKE_PROJECT_NAME})

add_subdirectory(samples)

install(TARGETS iothub_client_cpp DESTINATION lib)
install(FILES ${iothub_client_hpp_files} DESTINATION include/azureiot)