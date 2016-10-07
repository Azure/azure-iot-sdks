@REM  Copyright (c) Microsoft. All rights reserved.
@REM  Licensed under the MIT license. See LICENSE file in the project root for full license information.

@setlocal EnableDelayedExpansion

echo Releasing mbed bits ...

call ..\..\c\c-utility\tools\mbed_build_scripts\release_mbed_project.cmd ..\..\..\c\iothub_client\build\iothub_amqp_transport
if not !ERRORLEVEL!==0 exit /b !ERRORLEVEL!
call ..\..\c\c-utility\tools\mbed_build_scripts\release_mbed_project.cmd ..\..\..\c\iothub_client\build\iothub_http_transport
if not !ERRORLEVEL!==0 exit /b !ERRORLEVEL!
call ..\..\c\c-utility\tools\mbed_build_scripts\release_mbed_project.cmd ..\..\..\c\iothub_client\build\iothub_mqtt_transport
if not !ERRORLEVEL!==0 exit /b !ERRORLEVEL!
call ..\..\c\c-utility\tools\mbed_build_scripts\release_mbed_project.cmd ..\..\..\c\iothub_client\build\iothub_client
if not !ERRORLEVEL!==0 exit /b !ERRORLEVEL!
call ..\..\c\c-utility\tools\mbed_build_scripts\release_mbed_project.cmd ..\..\..\c\uamqp\build_all
if not !ERRORLEVEL!==0 exit /b !ERRORLEVEL!
call ..\..\c\c-utility\tools\mbed_build_scripts\release_mbed_project.cmd ..\..\..\c\umqtt\build_all
if not !ERRORLEVEL!==0 exit /b !ERRORLEVEL!
call ..\..\c\c-utility\tools\mbed_build_scripts\release_mbed_project.cmd ..\..\..\c\serializer\build
if not !ERRORLEVEL!==0 exit /b !ERRORLEVEL!
call ..\..\c\c-utility\tools\mbed_build_scripts\release_mbed_project.cmd ..\..\..\c\c-utility\build_all
if not !ERRORLEVEL!==0 exit /b !ERRORLEVEL!
call ..\..\c\c-utility\tools\mbed_build_scripts\release_mbed_project.cmd ..\..\..\c\iothub_client\samples\iothub_client_sample_amqp
if not !ERRORLEVEL!==0 exit /b !ERRORLEVEL!
call ..\..\c\c-utility\tools\mbed_build_scripts\release_mbed_project.cmd ..\..\..\c\iothub_client\samples\iothub_client_sample_http
if not !ERRORLEVEL!==0 exit /b !ERRORLEVEL!
call ..\..\c\c-utility\tools\mbed_build_scripts\release_mbed_project.cmd ..\..\..\c\iothub_client\samples\iothub_client_sample_mqtt
if not !ERRORLEVEL!==0 exit /b !ERRORLEVEL!
call ..\..\c\c-utility\tools\mbed_build_scripts\release_mbed_project.cmd ..\..\..\c\serializer\samples\remote_monitoring
if not !ERRORLEVEL!==0 exit /b !ERRORLEVEL!
call ..\..\c\c-utility\tools\mbed_build_scripts\release_mbed_project.cmd ..\..\..\c\serializer\samples\simplesample_amqp
if not !ERRORLEVEL!==0 exit /b !ERRORLEVEL!
call ..\..\c\c-utility\tools\mbed_build_scripts\release_mbed_project.cmd ..\..\..\c\serializer\samples\simplesample_http
if not !ERRORLEVEL!==0 exit /b !ERRORLEVEL!
call ..\..\c\c-utility\tools\mbed_build_scripts\release_mbed_project.cmd ..\..\..\c\serializer\samples\simplesample_mqtt
if not !ERRORLEVEL!==0 exit /b !ERRORLEVEL!
call ..\..\c\c-utility\tools\mbed_build_scripts\release_mbed_project.cmd ..\..\..\c\serializer\samples\temp_sensor_anomaly
if not !ERRORLEVEL!==0 exit /b !ERRORLEVEL!