#Copyright (c) Microsoft. All rights reserved.
#Licensed under the MIT license. See LICENSE file in the project root for full license information.

cmake_minimum_required(VERSION 2.8.11)

if(NOT ${use_mqtt})
    message(FATAL_ERROR "iothubclient_mqtt_dt_e2e being generated without MQTT support")
endif()

compileAsC99()
set(theseTestsName iothubclient_mqtt_dt_e2e)

set(${theseTestsName}_test_files
    ${theseTestsName}.cpp
    ../common_dt_e2e/iothubclient_common_dt_e2e.cpp
)

set(${theseTestsName}_cpp_files
    ${theseTestsName}.cpp
    ../common_dt_e2e/iothubclient_common_dt_e2e.cpp
)

set(${theseTestsName}_c_files
    ../../../certs/certs.c
    ../../../parson/parson.c
)

set(${theseTestsName}_h_files
    ../../../certs/certs.h
    ../../../parson/parson.h
    ../common_dt_e2e/iothubclient_common_dt_e2e.h
)

include_directories(../common_dt_e2e)
include_directories(../../../parson/)
include_directories(../../../certs/)
include_directories(${IOTHUB_TEST_INC_FOLDER})
include_directories(${IOTHUB_SERVICE_CLIENT_INC_FOLDER})

file(COPY suppressions.supp DESTINATION ${CMAKE_CURRENT_BINARY_DIR})
build_test_artifacts(${theseTestsName} ON VALGRIND_SUPPRESSIONS_FILE suppressions.supp)

if(WIN32)
    if(MSVC)
        #CPP compiler on windows likes to complain about unused local function removed (C4505)
        #C compiler doesn't like to complain about the same thing
        set (CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} /wd4505")
        set_source_files_properties(../../../parson/parson.c PROPERTIES COMPILE_FLAGS "/wd4244 /wd4232")
    endif()

    if(TARGET ${theseTestsName}_dll)
        target_link_libraries(${theseTestsName}_dll
            aziotsharedutil
            iothub_test
            iothub_client
            iothub_service_client
            iothub_client_mqtt_transport
            rpcrt4
        )
        linkMqttLibrary(${theseTestsName}_dll)
    endif()

    if(TARGET ${theseTestsName}_exe)
        target_link_libraries(${theseTestsName}_exe
            aziotsharedutil
            iothub_test
            iothub_client
            iothub_service_client
            iothub_client_mqtt_transport
            rpcrt4
        )
        linkMqttLibrary(${theseTestsName}_exe)
    endif()
else()
    if(TARGET ${theseTestsName}_exe)
        target_link_libraries(${theseTestsName}_exe
            aziotsharedutil
            iothub_test
            iothub_client
            iothub_client_mqtt_transport
            iothub_service_client
            aziotsharedutil
            iothub_client_mqtt_transport
        )
        target_link_libraries(${theseTestsName}_exe pthread uuid)
        linkMqttLibrary(${theseTestsName}_exe)
    endif()
endif()
