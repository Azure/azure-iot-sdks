#Copyright (c) Microsoft. All rights reserved.
#Licensed under the MIT license. See LICENSE file in the project root for full license information.

#this is CMakeLists.txt for serializer_e2e
cmake_minimum_required(VERSION 2.8.11)

if(NOT (${use_amqp} AND  ${use_http}) )
	message(FATAL_ERROR "serializer_e2e being generated without amqp or http support")
endif()

compileAsC99()
set(theseTestsName serializer_e2e)

set(${theseTestsName}_cpp_files
	${theseTestsName}.cpp
)

set(${theseTestsName}_nuget_cpp_files
	${theseTestsName}.cpp
)

set(${theseTestsName}_c_files
)

set(${theseTestsName}_h_files
	MacroE2EModelAction.h
)

include_directories(${IOTHUB_TEST_INC_FOLDER} ${IOTHUB_CLIENT_INC_FOLDER} ${SERIALIZER_INC_FOLDER})
include_directories(${IOTHUB_SERVICE_CLIENT_INC_FOLDER})

build_test_artifacts(${theseTestsName} ON)

if(WIN32)
	if(TARGET ${theseTestsName}_dll)
		target_link_libraries(${theseTestsName}_dll
			serializer
			iothub_test
			iothub_client
			iothub_client_amqp_transport
			iothub_client_http_transport
			iothub_service_client
			aziotsharedutil
			iothub_client_http_transport
		)
		linkSharedUtil(${theseTestsName}_dll)
		linkUAMQP(${theseTestsName}_dll)
		linkHttp(${theseTestsName}_dll)
	endif()

	if(TARGET ${theseTestsName}_exe)
		target_link_libraries(${theseTestsName}_exe
			serializer
			iothub_test
			iothub_client
			iothub_client_amqp_transport
			iothub_client_http_transport
			iothub_service_client
			aziotsharedutil
			iothub_client_http_transport
		)
		linkSharedUtil(${theseTestsName}_exe)
		linkUAMQP(${theseTestsName}_exe)
		linkHttp(${theseTestsName}_exe)
	endif()

	if(TARGET ${theseTestsName}_nuget_exe)
		target_link_libraries(${theseTestsName}_nuget_exe
			serializer
			iothub_test
			iothub_service_client
		)
	endif()
else()
	if(TARGET ${theseTestsName}_exe)
		target_link_libraries(${theseTestsName}_exe
				serializer
				iothub_test
				iothub_client
				iothub_client_amqp_transport
				iothub_client_http_transport
				iothub_service_client
				aziotsharedutil
				iothub_client_http_transport
			)
		target_link_libraries(${theseTestsName}_exe pthread)
		linkSharedUtil(${theseTestsName}_exe)
		linkUAMQP(${theseTestsName}_exe)
		linkHttp(${theseTestsName}_exe)
	endif()
endif()


