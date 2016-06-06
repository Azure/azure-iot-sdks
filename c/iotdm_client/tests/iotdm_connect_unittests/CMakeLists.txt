#Copyright (c) Microsoft. All rights reserved.
#Licensed under the MIT license. See LICENSE file in the project root for full license information.

cmake_minimum_required(VERSION 2.8)

set(theseTestsName iotdm_connect_unittests)

set(${theseTestsName}_cpp_files
    ${theseTestsName}.cpp
)

set(${theseTestsName}_c_files
    ${LOCK_C_FILE}	
    ${SHARED_UTIL_SRC_FOLDER}/gballoc.c
    ../../src/iotdm_connect.c
	../../src/dm_create_tlsio.c
    ../../../certs/certs.c
)

set(${theseTestsName}_h_files
    mocks.h
)

include_directories(../../../certs)
include_directories(.)

build_test_artifacts(${theseTestsName} ON)
