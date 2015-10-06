#Copyright (c) Microsoft. All rights reserved.
#Licensed under the MIT license. See LICENSE file in the project root for full license information.

#this is CMakeLists.txt for iothub_client_sample_http
set(iothub_client_sample_http_c_files
iothub_client_sample_http.c
./windows/main.c
)

set(iothub_client_sample_http_h_files
iothub_client_sample_http.h
)

IF(WIN32)
	#windows needs this define
	add_definitions(-D_CRT_SECURE_NO_WARNINGS)
ENDIF(WIN32)

include_directories(.)

add_executable(iothub_client_sample_http ${iothub_client_sample_http_c_files} ${iothub_client_sample_http_h_files})
#TODO: debug folder is hard coded and horrible. Should be something like configuration
target_link_libraries(iothub_client_sample_http common iothub_client_http_transport iothub_client Winhttp.lib)
