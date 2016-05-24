#Copyright (c) Microsoft. All rights reserved.
#Licensed under the MIT license. See LICENSE file in the project root for full license information.

#this is CMakeLists.txt for iotdm_observe_unittests
cmake_minimum_required(VERSION 2.8)

set(theseTestsName iotdm_dispatchers_unittests)

set(${theseTestsName}_cpp_files
    ${theseTestsName}.cpp
)

set(${theseTestsName}_c_files
    ${SHARED_UTIL_SRC_FOLDER}/gballoc.c
    ${SHARED_UTIL_SRC_FOLDER}/list.c
    ${SHARED_UTIL_SRC_FOLDER}/strings.c
    ${LOCK_C_FILE}    
    ../../../wakaama/core/utils.c
    ../../../wakaama/core/tlv.c
    ../../../wakaama/core/data.c
    ../../../wakaama/core/discover.c
    ../../../wakaama/core/uri.c
    ../../lwm2m_objects/lwm2m_object_list.c
    ../../lwm2m_objects/iotdm_dispatchers.c
    ../../lwm2m_objects/iotdm_dispatcher_list.c
    ${LIBLWM2M_DIR}/platforms/${CMAKE_SYSTEM_NAME}/platform.c
)

set(${theseTestsName}_h_files
)

build_test_artifacts(${theseTestsName} ON)
