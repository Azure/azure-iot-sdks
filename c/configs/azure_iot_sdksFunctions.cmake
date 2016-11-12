#Copyright (c) Microsoft. All rights reserved.
#Licensed under the MIT license. See LICENSE file in the project root for full license information.

function(linkUAMQP whatExecutableIsBuilding)
    include_directories(${UAMQP_INC_FOLDER})
    
    if(WIN32)
        #windows needs this define
        add_definitions(-D_CRT_SECURE_NO_WARNINGS)
        add_definitions(-DGB_MEASURE_MEMORY_FOR_THIS -DGB_DEBUG_ALLOC)

        target_link_libraries(${whatExecutableIsBuilding} uamqp aziotsharedutil ws2_32 secur32)

        if(${use_openssl} OR ${use_wsio})
            target_link_libraries(${whatExecutableIsBuilding} $ENV{OpenSSLDir}/lib/ssleay32.lib $ENV{OpenSSLDir}/lib/libeay32.lib)
        
            file(COPY $ENV{OpenSSLDir}/bin/libeay32.dll DESTINATION ${CMAKE_CURRENT_BINARY_DIR}/Debug)
            file(COPY $ENV{OpenSSLDir}/bin/ssleay32.dll DESTINATION ${CMAKE_CURRENT_BINARY_DIR}/Debug)

            file(COPY $ENV{OpenSSLDir}/bin/libeay32.dll DESTINATION ${CMAKE_CURRENT_BINARY_DIR}/Release)
            file(COPY $ENV{OpenSSLDir}/bin/ssleay32.dll DESTINATION ${CMAKE_CURRENT_BINARY_DIR}/Release)
        endif()
    else()
        target_link_libraries(${whatExecutableIsBuilding} uamqp aziotsharedutil ssl crypto)
    endif()
    
    if(${use_wsio})
        target_link_libraries(${whatExecutableIsBuilding} websockets)
    endif()
endfunction(linkUAMQP)

function(includeMqtt)
    include_directories(${MQTT_INC_FOLDER})
endfunction(includeMqtt)

function(linkMqttLibrary whatExecutableIsBuilding)
    includeMqtt()
    target_link_libraries(${whatExecutableIsBuilding} umqtt)
endfunction(linkMqttLibrary)

function(includeHttp)
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

function(linkWebSockets whatExecutableIsBuilding)
    if(${use_wsio})
        target_link_libraries(${whatExecutableIsBuilding} websockets)
    endif()
endfunction(linkWebSockets)

function(linkSharedUtil whatIsBuilding)
    target_link_libraries(${whatIsBuilding} aziotsharedutil)
endfunction(linkSharedUtil)