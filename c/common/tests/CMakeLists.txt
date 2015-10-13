#Copyright (c) Microsoft. All rights reserved.
#Licensed under the MIT license. See LICENSE file in the project root for full license information.

#this is CMakeLists.txt for the folder tests of common
add_subdirectory(agenttime_unittests)
add_subdirectory(base64_unittests)
add_subdirectory(buffer_unittests)
add_subdirectory(crtabstractions_unittests)
add_subdirectory(doublylinkedlist_unittests)
add_subdirectory(gballoc_unittests)
add_subdirectory(gballoc_without_init_unittests)
add_subdirectory(hmacsha256_unittests)

if(${use_http})
	add_subdirectory(httpapiex_unittests)
	add_subdirectory(httpapiexsas_unittests)
	add_subdirectory(httpheaders_unittests)
endif()

add_subdirectory(lock_unittests)
add_subdirectory(map_unittests)
add_subdirectory(sastoken_unittests)
add_subdirectory(string_tokenizer_unittests)
add_subdirectory(strings_unittests)
add_subdirectory(urlencode_unittests)
add_subdirectory(vector_unittests)