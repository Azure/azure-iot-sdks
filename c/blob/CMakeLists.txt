#Copyright (c) Microsoft. All rights reserved.
#Licensed under the MIT license. See LICENSE file in the project root for full license information.

cmake_minimum_required(VERSION 2.8.12)

include_directories(${SHARED_UTIL_INC_FOLDER})
include_directories(${UMOCK_C_INC_FOLDER})
include_directories(${BLOB_INC_FOLDER})

set(blob_c_files
./src/blob.c
)

set(blob_h_files
./inc/blob.h
)

set(BLOB_INC_FOLDER ${CMAKE_CURRENT_LIST_DIR}/inc CACHE INTERNAL "this is what needs to be included if using blob lib" FORCE)

add_library(blob
	${blob_c_files} 
	${blob_h_files}
)
linkSharedUtil(blob)
