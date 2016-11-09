#Copyright (c) Microsoft. All rights reserved.
#Licensed under the MIT license. See LICENSE file in the project root for full license information.

cmake_minimum_required(VERSION 2.8.11)
#this is CMakeLists for serializer

option(use_floats "if use_floats is used, serializer will use floats in its typesystem" ON)

if(${use_floats})
else()
	add_definitions(-DNO_FLOATS)
endif()

compileAsC99()

set(serializer_c_files
./src/agenttypesystem.c
./src/codefirst.c
./src/commanddecoder.c
./src/datamarshaller.c
./src/datapublisher.c
./src/dataserializer.c
./src/iotdevice.c
./src/jsondecoder.c
./src/jsonencoder.c
./src/makefile
./src/multitree.c
./src/schema.c
./src/schemalib.c
./src/schemaserializer.c
../parson/parson.c
)

set(serializer_h_files
./inc/agenttypesystem.h
./inc/codefirst.h
./inc/commanddecoder.h
./inc/datamarshaller.h
./inc/datapublisher.h
./inc/dataserializer.h
./inc/iotdevice.h
./inc/jsondecoder.h
./inc/jsonencoder.h
./inc/multitree.h
./inc/schema.h
./inc/schemalib.h
./inc/schemaserializer.h
./inc/serializer.h
./inc/serializer_devicetwin.h
)

if(MSVC)
    set_source_files_properties(../parson/parson.c PROPERTIES COMPILE_FLAGS "/wd4244 /wd4232")
endif()

#these are the include folders
#the following "set" statetement exports across the project a global variable called SHARED_UTIL_INC_FOLDER that expands to whatever needs to included when using COMMON library
set(SERIALIZER_INC_FOLDER ${CMAKE_CURRENT_LIST_DIR}/inc CACHE INTERNAL "this is what needs to be included if using serializer lib" FORCE)

include_directories(../parson)
include_directories(${SERIALIZER_INC_FOLDER} ${SHARED_UTIL_INC_FOLDER})
include_directories(${AZURE_C_SHARED_UTILITY_INCLUDES})

IF(WIN32)
	#windows needs this define
	add_definitions(-D_CRT_SECURE_NO_WARNINGS)
ENDIF(WIN32)

add_library(
serializer ${serializer_c_files} ${serializer_h_files}
)

if (NOT ${skip_samples})
if(WIN32)
	if (NOT ${ARCHITECTURE} STREQUAL "ARM")
		add_subdirectory(samples)
	endif()
else()
	add_subdirectory(samples)
	endif()
endif()

if(NOT IN_OPENWRT)
	# Disable tests and samples for OpenWRT
	if(NOT ${skip_unittests})
		add_subdirectory(tests)
	endif()
endif()

if(${use_installed_dependencies})
    include(GNUInstallDirs)

	#Set CMAKE_INSTALL_LIBDIR if not defined
	if(NOT DEFINED CMAKE_INSTALL_LIBDIR)
        set(CMAKE_INSTALL_LIBDIR "lib")
    endif()

    install(TARGETS serializer EXPORT azure_iot_sdksTargets
        LIBRARY DESTINATION ${CMAKE_INSTALL_LIBDIR}
        ARCHIVE DESTINATION ${CMAKE_INSTALL_LIBDIR}
        RUNTIME DESTINATION ${CMAKE_INSTALL_LIBDIR}/../bin
        INCLUDES DESTINATION ${CMAKE_INSTALL_INCLUDEDIR}/azureiot 
    )
    install(FILES ${serializer_h_files} DESTINATION ${CMAKE_INSTALL_INCLUDEDIR}/azureiot)

else()
    message(WARNING "This package may only be installed when 'use_installed_dependencies' is ON")
endif()