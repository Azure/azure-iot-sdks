#Copyright (c) Microsoft. All rights reserved.
#Licensed under the MIT license. See LICENSE file in the project root for full license information.

#this is CMakeLists.txt for simple_sample_with_strings sample

compileAsC99()

set(simple_sample_with_strings_c_files
    simple_sample_with_strings.c
)

set(simple_sample_with_strings_h_files
	simple_sample_with_strings.h
)

IF(WIN32)
	#windows needs this define
	add_definitions(-D_CRT_SECURE_NO_WARNINGS)
ENDIF(WIN32)

include_directories(. ${SERIALIZER_INC_FOLDER})

add_executable(simple_sample_with_strings ${simple_sample_with_strings_c_files} ${simple_sample_with_strings_h_files})
target_link_libraries(simple_sample_with_strings serializer)

linkSharedUtil(simple_sample_with_strings)

