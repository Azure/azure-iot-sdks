#Copyright (c) Microsoft. All rights reserved.
#Licensed under the MIT license. See LICENSE file in the project root for full license information.

#this is CMakeLists.txt for hmacsha256_unittests
cmake_minimum_required(VERSION 3.0)

set(hmacsha256_unittests_cpp_files
hmacsha256_unittests.cpp
)

set(hmacsha256_unittests_c_files
../../src/hmacsha256.c
../../src/hmac.c
../../src/usha.c
../../src/sha1.c
../../src/sha224.c
../../src/sha384-512.c

../../src/gballoc.c
../../adapters/lock_c11.c
../../src/buffer.c
)

set(hmacsha256_unittests_h_files
)

include_directories(${COMMON_SRC_FOLDER} ${MICROMOCK_INC_FOLDER} ${TESTRUNNERSWITCHER_INC_FOLDER} ${CTEST_INC_FOLDER} $ENV{VCInstallDir}UnitTest/include)
link_directories($ENV{VCInstallDir}UnitTest/lib)

IF(WIN32)
	#windows needs this define
	add_definitions(-D_CRT_SECURE_NO_WARNINGS)
	add_definitions(-DCPP_UNITTEST -DGB_MEASURE_MEMORY_FOR_THIS -DGB_DEBUG_ALLOC)
ENDIF(WIN32)


add_library(
hmacsha256_unittests SHARED ${hmacsha256_unittests_cpp_files} ${hmacsha256_unittests_h_files} ${hmacsha256_unittests_c_files}
)

target_link_libraries(hmacsha256_unittests micromock ctest)