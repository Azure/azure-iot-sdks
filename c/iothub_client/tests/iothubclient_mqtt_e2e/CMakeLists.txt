#Copyright (c) Microsoft. All rights reserved.
#Licensed under the MIT license. See LICENSE file in the project root for full license information.

#this is CMakeLists.txt for iothubclient_mqtt_e2e
cmake_minimum_required(VERSION 2.8.11)

if(NOT (${use_amqp} AND ${use_mqtt}))
	message(FATAL_ERROR "iothubclient_mqtt_e2e being generated without AMQP and MQTT support")
endif()

compileAsC11()
set(theseTestsName iothubclient_mqtt_e2e)

set(${theseTestsName}_cpp_files
	${theseTestsName}.cpp
)

set(${theseTestsName}_nuget_cpp_files
	${theseTestsName}.cpp
)

set(${theseTestsName}_c_files
)

set(${theseTestsName}_h_files
)

include_directories(${IOTHUB_TEST_INC_FOLDER})
include_directories(${IOTHUB_SERVICE_CLIENT_INC_FOLDER})

build_test_artifacts(${theseTestsName} ON)

if(WIN32)
	if(TARGET ${theseTestsName}_dll)
		target_link_libraries(${theseTestsName}_dll
			iothub_test
			iothub_client
			iothub_client_mqtt_transport
			iothub_service_client
			aziotsharedutil
			iothub_client_mqtt_transport
			rpcrt4
		)
		linkUAMQP(${theseTestsName}_dll)
		linkMqttLibrary(${theseTestsName}_dll)
	endif()
	
	if(TARGET ${theseTestsName}_exe)
		target_link_libraries(${theseTestsName}_exe
			iothub_test
			iothub_client
			iothub_client_mqtt_transport
			iothub_service_client
			aziotsharedutil
			iothub_client_mqtt_transport
			rpcrt4
		)
		linkUAMQP(${theseTestsName}_exe)
		linkMqttLibrary(${theseTestsName}_exe)
	endif()
    	
	if(TARGET ${theseTestsName}_nuget_exe)
		target_link_libraries(${theseTestsName}_nuget_exe
			iothub_test
			iothub_service_client
			rpcrt4
        )
    endif()
else()
	if(TARGET ${theseTestsName}_exe)
		target_link_libraries(${theseTestsName}_exe
			iothub_test
			iothub_client
			iothub_client_mqtt_transport
			iothub_service_client
			aziotsharedutil
			iothub_client_mqtt_transport
		)
		target_link_libraries(${theseTestsName}_exe pthread uuid)
		linkUAMQP(${theseTestsName}_exe)
		linkMqttLibrary(${theseTestsName}_exe)
	endif()
endif()
