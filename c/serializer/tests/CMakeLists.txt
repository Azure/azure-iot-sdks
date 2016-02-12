#Copyright (c) Microsoft. All rights reserved.
#Licensed under the MIT license. See LICENSE file in the project root for full license information.

cmake_minimum_required(VERSION 2.8.11)

#this is CMakeLists for serializer e2e folder
add_subdirectory(agentmacros_unittests)
add_subdirectory(agenttypesystem_unittests)
add_subdirectory(codefirst_cpp_unittests)
add_subdirectory(codefirst_unittests)
add_subdirectory(codefirst_withstructs_cpp_unittests)
add_subdirectory(codefirst_withstructs_unittests)
add_subdirectory(commanddecoder_unittests)
add_subdirectory(datamarshaller_unittests)
add_subdirectory(datapublisher_unittests)
add_subdirectory(dataserializer_unittests)
add_subdirectory(iotdevice_unittests)
add_subdirectory(jsondecoder_unittests)
add_subdirectory(jsonencoder_unittests)
add_subdirectory(multitree_unittests)
add_subdirectory(schema_unittests)
add_subdirectory(schemalib_unittests)
add_subdirectory(schemalib_without_init_unittests)
add_subdirectory(schemaserializer_unittests)

if(${use_amqp} AND ${use_http} AND ${run_e2e_tests})
	add_subdirectory(serializer_e2etests)
endif()