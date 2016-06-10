cmake_minimum_required(VERSION 2.8)

set(CMAKE_C_FLAGS "${CMAKE_C_FLAGS} -D__STDC_NO_ATOMICS__=1 -Wno-parentheses-equality -Wno-pointer-sign -Wno-tautological-constant-out-of-range-compare -Wno-switch")

set(CMAKE_C_FLAGS "${CMAKE_C_FLAGS}  -s ASSERTIONS=2")

#include the iot sdks CMAKE and link in the iotdm client
add_subdirectory("../../../c" "${CMAKE_CURRENT_BINARY_DIR}/iotsdk-build")

compileAsC99()

set(LIBRARY_NAME emwrapper)
set(IOTDM_ROOT_DIR ../../../c/iotdm_client)
set(SIMPLE_SAMPLE_DIR ${IOTDM_ROOT_DIR}/samples/iotdm_simple_sample)
set(LWM2M_OBJECTS_DIR ${IOTDM_ROOT_DIR}/lwm2m_objects)
set(LIBLWM2M_DIR ../../../c/wakaama)
set(IOTDM_CLIENT_INC_FOLDER ${IOTDM_ROOT_DIR}/inc)
set(IOTDM_CLIENT_SRC_DIR ${IOTDM_ROOT_DIR}/src)

set(${LIBRARY_NAME}_c_files
    ./src/platform_emscripten.c
    ./src/socketio_node.c
    ./src/tlsio_node.c
    ./src/iotdm_nodeapi.c
)

include_directories(${LIBLWM2M_DIR}/core)
include_directories(${LIBLWM2M_DIR}/core/er-coap-13)
include_directories(${IOTDM_CLIENT_INC_FOLDER})
include_directories(${SHARED_UTIL_INC_FOLDER})
include_directories(${IOTHUB_CLIENT_INC_FOLDER})
include_directories(${LWM2M_OBJECTS_DIR})
include_directories(${IOTDM_CLIENT_SRC_DIR})

add_executable(${LIBRARY_NAME} 
    ${${LIBRARY_NAME}_c_files}
)


target_link_libraries(${LIBRARY_NAME}
    iotdm_client)

linkSharedUtil(${LIBRARY_NAME})

    
