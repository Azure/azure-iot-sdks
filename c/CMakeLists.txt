#Copyright (c) Microsoft. All rights reserved.
#Licensed under the MIT license. See LICENSE file in the project root for full license information.

cmake_minimum_required(VERSION 3.0)
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
	else()
		#nothing to do here, it is assumed qpid-proton has been installed and headers and libraries are in the system
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
		target_link_libraries(${whatExecutableIsBuilding} qpid-proton)
	endif()
endfunction(linkProton)

function(includePaho)
#function body to be similar to includeProton
endfunction(includePaho)

function(linkPaho whatExecutableIsBuilding)
	includePaho()
	if(WIN32)
		target_link_libraries(${whatExecutableIsBuilding} "$ENV{paho_path}/org.eclipse.paho.mqtt.c/Windows Build/paho-mqtt3cs/Debug/paho-mqtt3cs.lib")
		file(COPY "$ENV{paho_path}/org.eclipse.paho.mqtt.c/Windows Build/paho-mqtt3cs/Debug/paho-mqtt3cs.dll" DESTINATION ${CMAKE_CURRENT_BINARY_DIR}/Debug)
		file(COPY "$ENV{openssldir}/bin/libeay32.dll" DESTINATION ${CMAKE_CURRENT_BINARY_DIR}/Debug)
		file(COPY "$ENV{openssldir}/bin/ssleay32.dll" DESTINATION ${CMAKE_CURRENT_BINARY_DIR}/Debug)
	else()
		target_link_libraries(${whatExecutableIsBuilding} paho-mqtt3cs)
	endif()
endfunction(linkPaho)

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


macro(compileAsC99)
  if (CMAKE_VERSION VERSION_LESS "3.1")
    if (CMAKE_C_COMPILER_ID STREQUAL "GNU")
      set (CMAKE_C_FLAGS "--std=c99 ${CMAKE_C_FLAGS}")
	  set (CMAKE_CXX_FLAGS "--std=c++11 ${CMAKE_CXX_FLAGS}")
    endif()
  else()
    set (CMAKE_C_STANDARD 99)
	set (CMAKE_CXX_STANDARD 11)
  endif()
endmacro(compileAsC99)


function(windows_unittests_add_dll whatIsBuilding)	
	link_directories(${whatIsBuilding}_dll $ENV{VCInstallDir}UnitTest/lib)
	
	add_library(${whatIsBuilding}_dll SHARED 
		${${whatIsBuilding}_cpp_files} 
		${${whatIsBuilding}_h_files} 
		${${whatIsBuilding}_c_files}
	)
	
	target_include_directories(${whatIsBuilding}_dll PUBLIC ${common_include_directories} $ENV{VCInstallDir}UnitTest/include)
	target_compile_definitions(${whatIsBuilding}_dll PUBLIC -DCPP_UNITTEST)
	target_link_libraries(${whatIsBuilding}_dll micromock_cpp_unittest ctest ${ARGN})
endfunction()

function(windows_unittests_add_lib whatIsBuilding)	
	link_directories(${whatIsBuilding}_lib $ENV{VCInstallDir}UnitTest/lib)
	
	add_library(${whatIsBuilding}_lib STATIC 
		${${whatIsBuilding}_cpp_files} 
		${${whatIsBuilding}_h_files} 
		${${whatIsBuilding}_c_files}
	)
	
	target_include_directories(${whatIsBuilding}_lib PUBLIC ${common_include_directories})
	target_compile_definitions(${whatIsBuilding}_lib PUBLIC -DUSE_CTEST)
	target_link_libraries(${whatIsBuilding}_lib micromock_ctest ctest ${ARGN})
endfunction()

function(windows_unittests_add_exe whatIsBuilding)
	add_executable(${whatIsBuilding}_exe
		${${whatIsBuilding}_cpp_files} 
		${${whatIsBuilding}_h_files} 
		${${whatIsBuilding}_c_files}
		${CMAKE_CURRENT_LIST_DIR}/main.c
	)
	target_compile_definitions(${whatIsBuilding}_exe PUBLIC -DUSE_CTEST)
	target_include_directories(${whatIsBuilding}_exe PUBLIC ${common_include_directories})
	target_link_libraries(${whatIsBuilding}_exe micromock_ctest ctest ${ARGN})
	add_test(NAME ${whatIsBuilding} COMMAND ${whatIsBuilding}_exe)
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
	set(common_include_directories ${MICROMOCK_INC_FOLDER} ${TESTRUNNERSWITCHER_INC_FOLDER} ${CTEST_INC_FOLDER} ${SAL_INC_FOLDER} ${COMMON_INC_FOLDER} ${COMMON_SRC_FOLDER})
	if(WIN32)				
	else()
		include_directories(${common_include_directories})
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

function(set_platform_files)
if(WIN32)
	set(LOCK_C_FILE ${CMAKE_CURRENT_LIST_DIR}/common/adapters/lock_c11.c PARENT_SCOPE)
  if (WINCE)
  	set(HTTP_C_FILE ${CMAKE_CURRENT_LIST_DIR}/common/adapters/httpapi_wince.cpp PARENT_SCOPE)
	  set(THREAD_C_FILE ${CMAKE_CURRENT_LIST_DIR}/common/adapters/threadapi_win32.c PARENT_SCOPE)
  else()
  	set(HTTP_C_FILE ${CMAKE_CURRENT_LIST_DIR}/common/adapters/httpapi_winhttp.c PARENT_SCOPE)
	set(THREAD_C_FILE ${CMAKE_CURRENT_LIST_DIR}/common/adapters/threadapi_c11.c PARENT_SCOPE)
  endif()
else()
	set(LOCK_C_FILE ${CMAKE_CURRENT_LIST_DIR}/common/adapters/lock_pthreads.c PARENT_SCOPE)
	set(THREAD_C_FILE ${CMAKE_CURRENT_LIST_DIR}/common/adapters/threadapi_pthreads.c PARENT_SCOPE)
	set(HTTP_C_FILE ${CMAKE_CURRENT_LIST_DIR}/common/adapters/httpapi_curl.c PARENT_SCOPE)
endif()
endfunction(set_platform_files)

if (WINCE)
  set(SDKKEY "HKEY_LOCAL_MACHINE\\SOFTWARE\\Microsoft\\Windows CE Tools\\SDKs\\${CMAKE_GENERATOR_PLATFORM}")

	get_filename_component(wincesdkdir
		"[${SDKKEY};]"
		ABSOLUTE)
    
	include_directories(${wincesdkdir}/Sdk/Crt/Include/Stl)
	include_directories(${CMAKE_CURRENT_LIST_DIR}/common/inc/windowsce)
  
  add_definitions(-D_CRTDBG_FILE_STDERR=stderr)
  
endif()

set_platform_files()

add_subdirectory(testtools)

add_subdirectory(common)

add_subdirectory(iothub_client)
add_subdirectory(serializer)
