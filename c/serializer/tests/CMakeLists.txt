#Copyright (c) Microsoft. All rights reserved.
#Licensed under the MIT license. See LICENSE file in the project root for full license information.

cmake_minimum_required(VERSION 2.8.11)

#this is CMakeLists for serializer e2e folder
add_subdirectory(agentmacros_ut)
add_subdirectory(agenttypesystem_ut)
add_subdirectory(codefirst_cpp_ut)
add_subdirectory(codefirst_ut)
add_subdirectory(codefirst_withstructs_cpp_ut)
add_subdirectory(codefirst_withstructs_ut)
add_subdirectory(commanddecoder_ut)
add_subdirectory(datamarshaller_ut)
add_subdirectory(datapublisher_ut)
add_subdirectory(dataserializer_ut)
add_subdirectory(iotdevice_ut)
add_subdirectory(jsondecoder_ut)
add_subdirectory(jsonencoder_ut)
add_subdirectory(multitree_ut)
add_subdirectory(schema_ut)
add_subdirectory(schemalib_ut)
add_subdirectory(schemalib_without_init_ut)
add_subdirectory(schemaserializer_ut)

if(${use_amqp} AND ${use_http} AND (${run_e2e_tests} OR ${nuget_e2e_tests}))
	add_subdirectory(serializer_e2e)
endif()