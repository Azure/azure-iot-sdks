#Copyright (c) Microsoft. All rights reserved.
#Licensed under the MIT license. See LICENSE file in the project root for full license information.

cmake_minimum_required(VERSION 3.0)
project(everything)

enable_testing()

#this project uses several other projects that are build not by these CMakeFiles
#this project also targets several OSes

#this function takes care of three things:
#1. add the includes of proton
#2. link qpid-proton(d).lib
#3. copying some shared libraries(.dll or .so) to the location of the output executable

function(includeProton )
	if(WIN32)
		include_directories($ENV{proton_path}/qpid-proton/proton-c/include)
	elseif()
		#nothing to do here, it is assumed qpid-proton has been installed and headers and libraries are in the system
	endif()
endfunction(includeProton)

function(linkProton whatExecutableIsBuilding)
	includeProton()
	if(WIN32)
		target_link_libraries(${whatExecutableIsBuilding} $ENV{proton_path}/qpid-proton/build/proton-c/Debug/qpid-protond.lib)
		file(COPY $ENV{proton_path}/qpid-proton/build/proton-c/Debug/qpid-protond.dll DESTINATION ${CMAKE_CURRENT_BINARY_DIR}/Debug)
	elseif()
		target_link_libraries(${whatExecutableIsBuilding} qpid-proton)
	endif()
endfunction(linkProton)

function(includePaho)
if(WIN32)
	#do something
elseif()
	#include something else
endif()
endfunction(includePaho)

function(linkPaho whatExecutableIsBuilding)
	includePaho()
	if(WIN32)
		target_link_libraries(${whatExecutableIsBuilding} "$ENV{paho_path}/org.eclipse.paho.mqtt.c/Windows Build/paho-mqtt3cs/Debug/paho-mqtt3cs.lib")
		file(COPY "$ENV{paho_path}/org.eclipse.paho.mqtt.c/Windows Build/paho-mqtt3cs/Debug/paho-mqtt3cs.dll" DESTINATION ${CMAKE_CURRENT_BINARY_DIR}/Debug)
		file(COPY "$ENV{openssldir}/bin/libeay32.dll" DESTINATION ${CMAKE_CURRENT_BINARY_DIR}/Debug)
		file(COPY "$ENV{openssldir}/bin/ssleay32.dll" DESTINATION ${CMAKE_CURRENT_BINARY_DIR}/Debug)
	elseif()
		target_link_libraries(${whatExecutableIsBuilding} paho-mqtt3cs)
	endif()
endfunction(linkPaho)

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

function(build_test_artifacts whatIsBuilding)

	#message(STATUS "${whatIsBuilding} is having ARGC=${ARGC} ARGV0=${ARGV0} ARGV1=${ARGV1}")

	if(${ARGC} EQUAL 1)
		set(uses_gb_alloc true)
	else()
		if(${ARGV1} EQUAL 0)
			set(uses_gb_alloc false)
		else()
			set(uses_gb_alloc true)
		endif()
	endif()
	
	#message(STATUS "${whatIsBuilding} is having uses_gb_alloc set to ${uses_gb_alloc}}")
	
	#setting #defines
	if(WIN32)
		add_definitions(-D_CRT_SECURE_NO_WARNINGS)
		add_definitions(-DCPP_UNITTEST )
		if(${uses_gb_alloc})
			#message(STATUS "${whatIsBuilding} is getting -DGB_MEASURE_MEMORY_FOR_THIS -DGB_DEBUG_ALLOC")
			add_definitions(-DGB_MEASURE_MEMORY_FOR_THIS -DGB_DEBUG_ALLOC)
		endif(${uses_gb_alloc})			
	else()
		add_definitions(-DUSE_CTEST)
	endif()
	
	#setting includes
	set(common_include_directories ${MICROMOCK_INC_FOLDER} ${TESTRUNNERSWITCHER_INC_FOLDER} ${CTEST_INC_FOLDER} ${SAL_INC_FOLDER} ${COMMON_INC_FOLDER} ${COMMON_SRC_FOLDER})
	if(WIN32)
		include_directories(${common_include_directories} $ENV{VCInstallDir}UnitTest/include)		
	else()
		include_directories(${common_include_directories})
	endif()
	
	#settings link directories
	if(WIN32)
		message(STATUS "setting link_directories for ${whatIsBuilding} to link_directories($ENV{VCInstallDir}UnitTest/lib)")
		link_directories($ENV{VCInstallDir}UnitTest/lib)
	else(WIN32)
		#nothing
	endif(WIN32)
	
	#setting output type
	if(WIN32)
		add_library(
			${whatIsBuilding} SHARED ${${whatIsBuilding}_cpp_files} ${${whatIsBuilding}_h_files} ${${whatIsBuilding}_c_files}
		)
	else(WIN32)
	endif(WIN32)
	
	target_link_libraries(${whatIsBuilding} micromock ctest)
endfunction(build_test_artifacts)

add_subdirectory(testtools)

add_subdirectory(common)

add_subdirectory(iothub_client)
add_subdirectory(serializer)