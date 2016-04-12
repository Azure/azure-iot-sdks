#Copyright (c) Microsoft. All rights reserved.
#Licensed under the MIT license. See LICENSE file in the project root for full license information.

#this is CMakeLists.txt for iotdm_dmlist_unittests
cmake_minimum_required(VERSION 2.8)

set(theseTestsName iotdm_dmlist_unittests)

set(${theseTestsName}_cpp_files
    ${theseTestsName}.cpp
)

set(${theseTestsName}_c_files
    ../../lwm2m_objects/lwm2m_object_list.c
    ${SHARED_UTIL_SRC_FOLDER}/gballoc.c
    ${SHARED_UTIL_SRC_FOLDER}/list.c
    ${LOCK_C_FILE}
    ${LIBLWM2M_DIR}/platforms/${CMAKE_SYSTEM_NAME}/platform.c
)

set(${theseTestsName}_h_files
)

build_test_artifacts(${theseTestsName} ON)
