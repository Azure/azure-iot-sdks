#Copyright (c) Microsoft. All rights reserved.
#Licensed under the MIT license. See LICENSE file in the project root for full license information.

cmake_minimum_required(VERSION 2.8)

compileAsC99()

set (SAMPLE_NAME iotdm_2channel_sample)

set(LIBIOTDM_DIR ../..)
set(LIBLWM2M_DIR ../../../wakaama)

# this should be replaced with a global project variable...
set(LIBSHAREDUTIL_DIR ../../../azure-c-shared-utility/c)

set(my_c_files
    ${SAMPLE_NAME}.c
    device_object.c
    firmwareupdate_object.c
    lwm2mserver_object.c
    config_object.c
)

set(m_h_files
)

if(WIN32)
set (common_platform_c_files
    ${LIBSHAREDUTIL_DIR}/adapters/lock_c11.c
    ${LIBSHAREDUTIL_DIR}/adapters/platform_win32.c
    ${LIBSHAREDUTIL_DIR}/adapters/socketio_win32.c
    ${LIBSHAREDUTIL_DIR}/adapters/threadapi_c11.c
)
else()
set (common_platform_c_files
    ${LIBSHAREDUTIL_DIR}/adapters/lock_pthreads.c
    ${LIBSHAREDUTIL_DIR}/adapters/platform_linux.c
    ${LIBSHAREDUTIL_DIR}/adapters/socketio_berkeley.c
    ${LIBSHAREDUTIL_DIR}/adapters/threadapi_pthreads.c
)
endif()

set (common_platform_h_files
    ${LIBSHAREDUTIL_DIR}/inc/base64.h
    ${LIBSHAREDUTIL_DIR}/inc/buffer_.h
    ${LIBSHAREDUTIL_DIR}/inc/hmac.h
    ${LIBSHAREDUTIL_DIR}/inc/hmacsha256.h
    ${LIBSHAREDUTIL_DIR}/inc/lock.h
    ${LIBSHAREDUTIL_DIR}/inc/platform.h
    ${LIBSHAREDUTIL_DIR}/inc/sha.h
    ${LIBSHAREDUTIL_DIR}/inc/socketio.h
    ${LIBSHAREDUTIL_DIR}/inc/threadapi.h
)

set(common_c_files ${common_platform_c_files}
    ${LIBSHAREDUTIL_DIR}/adapters/agenttime.c
    ${LIBSHAREDUTIL_DIR}/src/buffer.c
    ${LIBSHAREDUTIL_DIR}/src/base64.c
    ${LIBSHAREDUTIL_DIR}/src/crt_abstractions.c
    ${LIBSHAREDUTIL_DIR}/src/hmac.c
    ${LIBSHAREDUTIL_DIR}/src/hmacsha256.c
    ${LIBSHAREDUTIL_DIR}/src/gballoc.c
    ${LIBSHAREDUTIL_DIR}/src/list.c
    ${LIBSHAREDUTIL_DIR}/src/sastoken.c
    ${LIBSHAREDUTIL_DIR}/src/sha1.c
    ${LIBSHAREDUTIL_DIR}/src/sha224.c
    ${LIBSHAREDUTIL_DIR}/src/sha384-512.c
    ${LIBSHAREDUTIL_DIR}/src/strings.c
    ${LIBSHAREDUTIL_DIR}/src/string_tokenizer.c
    ${LIBSHAREDUTIL_DIR}/src/urlencode.c
    ${LIBSHAREDUTIL_DIR}/src/usha.c
    ${LIBSHAREDUTIL_DIR}/src/xio.c
)

if(WIN32)
set(common_c_files ${common_c_files}
    ${LIBSHAREDUTIL_DIR}/src/tlsio_schannel.c
)
else()
set(common_c_files ${common_c_files}
    ${LIBSHAREDUTIL_DIR}/src/tlsio_openssl.c
)
endif()

set(common_h_files ${common_platform_h_files}
    ${LIBSHAREDUTIL_DIR}/inc/crt_abstractions.h
    ${LIBSHAREDUTIL_DIR}/inc/gballoc.h
    ${LIBSHAREDUTIL_DIR}/inc/sastoken.h
    ${LIBSHAREDUTIL_DIR}/inc/list.h
    ${LIBSHAREDUTIL_DIR}/inc/strings.h
    ${LIBSHAREDUTIL_DIR}/inc/string_tokenizer.h
    ${LIBSHAREDUTIL_DIR}/inc/tlsio_openssl.h
    ${LIBSHAREDUTIL_DIR}/inc/urlencode.h
    ${LIBSHAREDUTIL_DIR}/inc/xio.h
)

if(WIN32)
set(common_h_files ${common_h_files}
    ${LIBSHAREDUTIL_DIR}/inc/tlsio_schannel.h
)
else()
set(common_h_files ${common_h_files}
    ${LIBSHAREDUTIL_DIR}/inc/tlsio_openssl.h
)
endif()

if(WIN32)
    #windows needs this define
    add_definitions(-D_CRT_SECURE_NO_WARNINGS)
    add_definitions(-DGB_MEASURE_MEMORY_FOR_THIS -DGB_DEBUG_ALLOC)
else()
endif(WIN32)

include_directories(.)
include_directories(${LIBIOTDM_DIR}/inc)
include_directories(${LIBLWM2M_DIR}/core)
include_directories(${LIBSHAREDUTIL_DIR}/inc)

add_executable(${SAMPLE_NAME}
    ${common_c_files}
    ${common_h_files}
    ${my_h_files}
    ${my_c_files}
)

target_link_libraries(${SAMPLE_NAME} 
	iotdm_client
	iothub_client 
	iothub_client_http_transport
)

    
if(WIN32)
    source_group("shared-utility-src" FILES ${common_c_files})
    source_group("shared-utility-inc" FILES ${common_h_files})
    target_link_libraries(${SAMPLE_NAME} Secur32 ws2_32)
else()
    target_link_libraries(${SAMPLE_NAME} m pthread ssl crypto)
endif()

linkHttp(${SAMPLE_NAME})

