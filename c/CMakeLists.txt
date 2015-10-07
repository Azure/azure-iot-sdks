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
	elseif(UNIX)
		#nothing to do here, it is assumed qpid-proton has been installed and headers and libraries are in the system
	endif(WIN32)
endfunction(includeProton)

function(linkProton whatExecutableIsBuilding)
	includeProton()
	if(WIN32)
		target_link_libraries(${whatExecutableIsBuilding} $ENV{proton_path}/qpid-proton/build/proton-c/Debug/qpid-protond.lib)
		file(COPY $ENV{proton_path}/qpid-proton/build/proton-c/Debug/qpid-protond.dll DESTINATION ${CMAKE_CURRENT_BINARY_DIR}/Debug)
	elseif(UNIX)
		target_link_libraries(${whatExecutableIsBuilding} qpid-proton)
	endif(WIN32)
endfunction(linkProton)

function(includePaho)
if(WIN32)
	#do something
elseif(UNIX)
	#include something else
endif(WIN32)
endfunction(includePaho)

function(linkPaho whatExecutableIsBuilding)
	includePaho()
	if(WIN32)
		target_link_libraries(${whatExecutableIsBuilding} "$ENV{paho_path}/org.eclipse.paho.mqtt.c/Windows Build/paho-mqtt3cs/Debug/paho-mqtt3cs.lib")
		file(COPY "$ENV{paho_path}/org.eclipse.paho.mqtt.c/Windows Build/paho-mqtt3cs/Debug/paho-mqtt3cs.dll" DESTINATION ${CMAKE_CURRENT_BINARY_DIR}/Debug)
		file(COPY "$ENV{openssldir}/bin/libeay32.dll" DESTINATION ${CMAKE_CURRENT_BINARY_DIR}/Debug)
		file(COPY "$ENV{openssldir}/bin/ssleay32.dll" DESTINATION ${CMAKE_CURRENT_BINARY_DIR}/Debug)
	elseif(UNIX)
		target_link_libraries(${whatExecutableIsBuilding} paho-mqtt3cs)
	endif(WIN32)
endfunction(linkPaho)

function(compileAsC99)
  if (CMAKE_VERSION VERSION_LESS "3.1")
    if (CMAKE_C_COMPILER_ID STREQUAL "GNU")
      set (CMAKE_C_FLAGS "--std=c99 ${CMAKE_C_FLAGS}")
    endif ()
  else ()
    set (CMAKE_C_STANDARD 99)
  endif ()
endfunction(compileAsC99)

add_subdirectory(testtools)

add_subdirectory(common)

add_subdirectory(iothub_client)
add_subdirectory(serializer)