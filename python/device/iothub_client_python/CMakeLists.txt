#Copyright (c) Microsoft. All rights reserved.
#Licensed under the MIT license. See LICENSE file in the project root for full license information.

#this is CMakeLists.txt for iothub_client_python

if(NOT ${use_mqtt})
    message(FATAL_ERROR "iothub_client_python needs mqtt support")
endif()
if(NOT ${use_amqp})
    message(FATAL_ERROR "iothub_client_python needs amqp support")
endif()
if(NOT ${use_http})
    message(FATAL_ERROR "iothub_client_python needs http support")
endif()

compileAsC99()

# get boost
if(WIN32)
    set(Boost_INCLUDE_DIR $ENV{BOOST_ROOT})
    set(Boost_USE_STATIC_LIBS    ON) # only find static libs
endif()
set(Boost_USE_MULTITHREADED      ON)
set(Boost_USE_STATIC_RUNTIME     OFF)
find_package(Boost COMPONENTS python REQUIRED)

if(WIN32)
    IF (NOT Boost_FOUND)
        message("FATAL ERROR: Boost Python library not found")
        message("On windows, get boost from http://www.boost.org/users/history/version_1_60_0.html")
        message("Extract zip to a folder, then perform bootstrap and build libs with b2 command")
        message("The BOOST_ROOT environment variable should point to the root folder")
        message("of the extracted boost package with headers and libraries")
        message(FATAL_ERROR "iothub_client_python needs Boost library")
    endif()
endif()

include_directories(${Boost_INCLUDE_DIRS})
link_directories(${Boost_LIBRARY_DIRS})

if (WIN32)
  # disable autolinking in boost
  add_definitions( -DBOOST_ALL_NO_LIB )
  # force all boost libraries to static link
  add_definitions( -DBOOST_ALL_STATIC_LINK )
  add_definitions( -DBOOST_PYTHON_STATIC_LIB )
endif()


# get python
find_package(PythonLibs 2.7 REQUIRED)
include_directories(${PYTHON_INCLUDE_DIRS})
link_directories(${PYTHON_LIBRARIES})

set(iothub_client_python_c_files
    ./src/iothub_client_python.cpp
)

set(iothub_client_mock_c_files
    ./test/iothub_client_mock.cpp
)

if(WIN32)
    set(iothub_client_python_c_files ${iothub_client_python_c_files} ./windows/dllmain.c ./windows/iothub_client.def)
    set(iothub_client_mock_c_files ${iothub_client_mock_c_files} ./windows/dllmain.c )
endif()

include_directories(${SHARED_UTIL_INC_FOLDER})
include_directories(${IOTHUB_CLIENT_INC_FOLDER})
if(${use_http})
	include_directories(${IOTHUB_CLIENT_HTTP_TRANSPORT_INC_FOLDER})
endif()
if(${use_amqp})
	include_directories(${IOTHUB_CLIENT_AMQP_TRANSPORT_INC_FOLDER} ${UAMQP_INC_FOLDER})
endif()
if(${use_mqtt})
	include_directories(${IOTHUB_CLIENT_MQTT_TRANSPORT_INC_FOLDER} ${MQTT_INC_FOLDER})
endif()
include_directories(.)
include_directories(./src)

add_library( iothub_client_python SHARED
    ${iothub_client_python_c_files} 
)

add_library( iothub_client_mock SHARED
    ${iothub_client_mock_c_files} 
)

# output name of python module is iothub_client
SET_TARGET_PROPERTIES(iothub_client_python PROPERTIES OUTPUT_NAME "iothub_client")
SET_TARGET_PROPERTIES(iothub_client_python PROPERTIES PREFIX "")
SET_TARGET_PROPERTIES(iothub_client_mock PROPERTIES PREFIX "")
IF(WIN32)
    SET_TARGET_PROPERTIES(iothub_client_python PROPERTIES SUFFIX ".pyd")
    SET_TARGET_PROPERTIES(iothub_client_mock PROPERTIES SUFFIX ".pyd")
ENDIF(WIN32)

target_link_libraries(  
    iothub_client_python
    iothub_client_mqtt_transport
    iothub_client_http_transport
    iothub_client_amqp_transport
    iothub_client 
    uamqp
    ${Boost_LIBRARIES}
    ${PYTHON_LIBRARIES}
)

target_link_libraries(  
    iothub_client_mock
    ${Boost_LIBRARIES}
    ${PYTHON_LIBRARIES}
)

linkSharedUtil(iothub_client_python)
linkUAMQP(iothub_client_python)
