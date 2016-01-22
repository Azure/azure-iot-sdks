#Copyright (c) Microsoft. All rights reserved.
#Licensed under the MIT license. See LICENSE file in the project root for full license information.

cmake_minimum_required(VERSION 2.8.11)
project(azure_iot_sdks)

#the following variables are project-wide and can be used with cmake-gui
option(use_amqp "set use_amqp to ON if amqp is to be used, set to OFF to not use amqp" ON)
option(use_http "set use_http to ON if http is to be used, set to OFF to not use http" ON)
option(use_mqtt "set use_mqtt to ON if mqtt is to be used, set to OFF to not use mqtt" ON)
option(run_e2e_tests "set run_e2e_tests to ON to run e2e tests (default is OFF) [if possible, they are always build]" OFF)
option(run_longhaul_tests "set run_longhaul_tests to ON to run longhaul tests (default is OFF)[if possible, they are always build]" OFF)
option(skip_unittests "set skip_unittests to ON to skip unittests (default is OFF)[if possible, they are always build]" OFF)
option(compileOption_C "passes a string to the command line of the C compiler" OFF)
option(compileOption_CXX "passes a string to the command line of the C++ compiler" OFF)
SET(PROTON_C_INCLUDES "" CACHE STRING "directory where proton-c include files resides. Not needed if the proton_build.sh file is successfully run")
SET(PROTON_LIB "qpid-proton" CACHE STRING "name of the qpid c library. This can be a fully qualified path or simply the name of the lib")

#Use solution folders. 
set_property(GLOBAL PROPERTY USE_FOLDERS ON)

if(IN_OPENWRT)
	ADD_DEFINITIONS("$ENV{TARGET_LDFLAGS}" "$ENV{TARGET_CPPFLAGS}" "$ENV{TARGET_CFLAGS}")
	INCLUDE_DIRECTORIES("$ENV{TOOLCHAIN_DIR}/usr/include" "$ENV{TARGET_LDFLAGS}" "$ENV{TARGET_CPPFLAGS}" "$ENV{TARGET_CFLAGS}")
endif()

add_subdirectory(azure-c-shared-utility/c)
add_subdirectory(azure-umqtt-c)

enable_testing()

#if any compiler has a command line switch called "OFF" then it will need special care
if(NOT "${compileOption_C}" STREQUAL "OFF")
	set(CMAKE_C_FLAGS "${compileOption_C} ${CMAKE_C_FLAGS}")
endif()

if(NOT "${compileOption_CXX}" STREQUAL "OFF")
	set(CMAKE_CXX_FLAGS "${compileOption_CXX} ${CMAKE_CXX_FLAGS}")
endif()


#this project uses several other projects that are build not by these CMakeFiles
#this project also targets several OSes

#this function takes care of three things:
#1. add the includes of proton
#2. link qpid-proton(d).lib
#3. copying some shared libraries(.dll or .so) to the location of the output executable
function(includeProton)
	if(WIN32)
		include_directories($ENV{proton_path}/qpid-proton/proton-c/include)
	elseif( NOT "${PROTON_C_INCLUDES}" STREQUAL "" )
		include_directories(${PROTON_C_INCLUDES})
	endif()
endfunction(includeProton)

function(linkProton whatExecutableIsBuilding)
	includeProton()
	if(WIN32)
		if( CMAKE_SIZEOF_VOID_P EQUAL 4 )
			target_link_libraries(${whatExecutableIsBuilding} $ENV{proton_path}/qpid-proton/build/proton-c/Debug/qpid-protond.lib)
			file(COPY $ENV{proton_path}/qpid-proton/build/proton-c/Debug/qpid-protond.dll DESTINATION ${CMAKE_CURRENT_BINARY_DIR}/Debug)
		else()
			target_link_libraries(${whatExecutableIsBuilding} $ENV{proton_path}/qpid-proton/build_x64/proton-c/Debug/qpid-protond.lib)
			file(COPY $ENV{proton_path}/qpid-proton/build_x64/proton-c/Debug/qpid-protond.dll DESTINATION ${CMAKE_CURRENT_BINARY_DIR}/Debug)
		endif()
	else()
		target_link_libraries(${whatExecutableIsBuilding} ${PROTON_LIB})
	endif()
endfunction(linkProton)

function(includeMqtt)
	include_directories(${MQTT_INC_FOLDER})
endfunction(includeMqtt)

function(linkMqttLibrary whatExecutableIsBuilding)
	includeMqtt()
	target_link_libraries(${whatExecutableIsBuilding} umqtt)
endfunction(linkMqttLibrary)

function(includeHttp)
#function body to be similar to includeProton
endfunction(includeHttp)

function(linkHttp whatExecutableIsBuilding)
	includeHttp()
	if(WIN32)
		if(WINCE)
			  target_link_libraries(${whatExecutableIsBuilding} crypt32.lib)
		  target_link_libraries(${whatExecutableIsBuilding} ws2.lib)
		else()
			target_link_libraries(${whatExecutableIsBuilding} winhttp.lib)
		endif()
	else()
		target_link_libraries(${whatExecutableIsBuilding} curl)
	endif()
endfunction(linkHttp)

function(linkSharedUtil whatIsBuilding)
	target_link_libraries(${whatIsBuilding} aziotsharedutil)
endfunction(linkSharedUtil)

macro(compileAsC99)
  if (CMAKE_VERSION VERSION_LESS "3.1")
    if (CMAKE_C_COMPILER_ID STREQUAL "GNU")
      set (CMAKE_C_FLAGS "--std=c99 ${CMAKE_C_FLAGS}")
	if (NOT IN_OPENWRT)
	  set (CMAKE_CXX_FLAGS "--std=c++11 ${CMAKE_CXX_FLAGS}")
	endif()
    endif()
  else()
    set (CMAKE_C_STANDARD 99)
	set (CMAKE_CXX_STANDARD 11)
  endif()
endmacro(compileAsC99)

macro(compileAsC11)
  if (CMAKE_VERSION VERSION_LESS "3.1")
    if (CMAKE_C_COMPILER_ID STREQUAL "GNU")
      set (CMAKE_C_FLAGS "--std=c11 ${CMAKE_C_FLAGS}")
      set (CMAKE_C_FLAGS "-D_POSIX_C_SOURCE=200112L ${CMAKE_C_FLAGS}")
      set (CMAKE_CXX_FLAGS "--std=c++11 ${CMAKE_CXX_FLAGS}")
    endif()
  else()
    set (CMAKE_C_STANDARD 11)
    set (CMAKE_CXX_STANDARD 11)
  endif()
endmacro(compileAsC11)

function(windows_unittests_add_dll whatIsBuilding)	
	link_directories(${whatIsBuilding}_dll ${MICROMOCK_UNITTEST_LIB_DIR} ${CTEST_LIB_DIR} ${SHARED_UTIL_LIB_DIR} $ENV{VCInstallDir}UnitTest/lib)
	
	add_library(${whatIsBuilding}_dll SHARED 
		${${whatIsBuilding}_cpp_files} 
		${${whatIsBuilding}_h_files} 
		${${whatIsBuilding}_c_files}
	)
	
	#Add UnitTests to their own folder
    set_target_properties(${whatIsBuilding}_dll
               PROPERTIES
               FOLDER "UnitTests") 
			   
	target_include_directories(${whatIsBuilding}_dll PUBLIC ${sharedutil_include_directories} $ENV{VCInstallDir}UnitTest/include)
	target_compile_definitions(${whatIsBuilding}_dll PUBLIC -DCPP_UNITTEST)
	target_link_libraries(${whatIsBuilding}_dll micromock_cpp_unittest ctest ${ARGN})
endfunction()

function(windows_unittests_add_exe whatIsBuilding)
    link_directories(${whatIsBuilding}_exe ${MICROMOCK_UNITTEST_LIB_DIR} ${CTEST_LIB_DIR} ${SHARED_UTIL_LIB_DIR} $ENV{VCInstallDir}UnitTest/lib)
	
	add_executable(${whatIsBuilding}_exe
		${${whatIsBuilding}_cpp_files} 
		${${whatIsBuilding}_h_files} 
		${${whatIsBuilding}_c_files}
		${CMAKE_CURRENT_LIST_DIR}/main.c
	)
	
	#Add UnitTests to their own folder
    set_target_properties(${whatIsBuilding}_exe
               PROPERTIES
               FOLDER "UnitTests")
	
	target_compile_definitions(${whatIsBuilding}_exe PUBLIC -DUSE_CTEST)
	target_include_directories(${whatIsBuilding}_exe PUBLIC ${sharedutil_include_directories})
	target_link_libraries(${whatIsBuilding}_exe micromock_ctest ctest ${ARGN})
	add_test(NAME ${whatIsBuilding} COMMAND ${whatIsBuilding}_exe)
endfunction()

function(windows_unittests_add_lib whatIsBuilding)	
	link_directories(${whatIsBuilding}_lib ${MICROMOCK_UNITTEST_LIB_DIR} ${CTEST_LIB_DIR} ${SHARED_UTIL_LIB_DIR} $ENV{VCInstallDir}UnitTest/lib)
	
	add_library(${whatIsBuilding}_lib STATIC 
		${${whatIsBuilding}_cpp_files} 
		${${whatIsBuilding}_h_files} 
		${${whatIsBuilding}_c_files}
	)
	
	    #Add UnitTests to their own folder
    set_target_properties(${whatIsBuilding}_lib
               PROPERTIES
               FOLDER "UnitTests")
			   
	target_include_directories(${whatIsBuilding}_lib PUBLIC ${sharedutil_include_directories})
	target_compile_definitions(${whatIsBuilding}_lib PUBLIC -DUSE_CTEST)
	target_link_libraries(${whatIsBuilding}_lib micromock_ctest ctest ${ARGN})
endfunction()

function(build_test_artifacts whatIsBuilding use_gballoc)
	
	#the first argument is what is building
	#the second argument is whether the tests should be build with gballoc #defines or not
	#the following arguments are a list of libraries to link with
	
	if(${use_gballoc})
		add_definitions(-DGB_MEASURE_MEMORY_FOR_THIS -DGB_DEBUG_ALLOC)
	else()	
	endif()
	
	#setting #defines
	if(WIN32)
		add_definitions(-D_CRT_SECURE_NO_WARNINGS)		
	else()
	endif()
	
	#setting includes
	set(sharedutil_include_directories ${MICROMOCK_INC_FOLDER} ${TESTRUNNERSWITCHER_INC_FOLDER} ${CTEST_INC_FOLDER} ${SAL_INC_FOLDER} ${SHARED_UTIL_INC_FOLDER} ${SHARED_UTIL_SRC_FOLDER})
	if(WIN32)				
	else()
		include_directories(${sharedutil_include_directories})
	endif()

	#setting output type
	if(WIN32)
		#to disable running e2e or longhaul or unittests tests for windows, we build the the same thing as "static library" so it is not picked up by visual studio
		if(
			(("${whatIsBuilding}" MATCHES ".*e2e.*") AND NOT ${run_e2e_tests}) OR
			(("${whatIsBuilding}" MATCHES ".*longhaul.*") AND NOT ${run_longhaul_tests}) OR
			(("${whatIsBuilding}" MATCHES ".*unittests.*") AND ${skip_unittests})
		)
			windows_unittests_add_lib(${whatIsBuilding} ${ARGN})
		else()
			windows_unittests_add_exe(${whatIsBuilding} ${ARGN})
			windows_unittests_add_dll(${whatIsBuilding} ${ARGN})
		endif()
	else()
		if(
			(("${whatIsBuilding}" MATCHES ".*e2e.*") AND NOT ${run_e2e_tests}) OR
			(("${whatIsBuilding}" MATCHES ".*longhaul.*") AND NOT ${run_longhaul_tests}) OR
			(("${whatIsBuilding}" MATCHES ".*unittests.*") AND ${skip_unittests})
		)
			windows_unittests_add_lib(${whatIsBuilding} ${ARGN})
		else()
			windows_unittests_add_exe(${whatIsBuilding} ${ARGN})
		endif()
	endif()
endfunction(build_test_artifacts)

set(SHARED_UTIL_ADAPTER_FOLDER "${CMAKE_CURRENT_LIST_DIR}/azure-c-shared-utility/c/adapters")
if(WIN32)
	set(LOCK_C_FILE ${SHARED_UTIL_ADAPTER_FOLDER}/lock_c11.c)
	set(THREAD_C_FILE ${SHARED_UTIL_ADAPTER_FOLDER}/threadapi_c11.c)
else()
	set(LOCK_C_FILE ${SHARED_UTIL_ADAPTER_FOLDER}/lock_pthreads.c)
	set(THREAD_C_FILE ${SHARED_UTIL_ADAPTER_FOLDER}/threadapi_pthreads.c)
endif()

add_subdirectory(common)

add_subdirectory(iothub_client)
add_subdirectory(serializer)