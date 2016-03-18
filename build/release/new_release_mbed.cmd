@REM  Copyright (c) Microsoft. All rights reserved.
@REM  Licensed under the MIT license. See LICENSE file in the project root for full license information.

echo Releasing mbed bits ...

call new_release_mbed_project.cmd iothub_amqp_transport     ..\..\..\c\iothub_client\build\iothub_amqp_transport
call new_release_mbed_project.cmd iothub_http_transport     ..\..\..\c\iothub_client\build\iothub_http_transport
call new_release_mbed_project.cmd iothub_mqtt_transport     ..\..\..\c\iothub_client\build\iothub_mqtt_transport
call new_release_mbed_project.cmd iothub_client             ..\..\..\c\iothub_client\build\iothub_client
@REM call release_mbed_project.cmd azure_uamqp ..\..\c\azure-uamqp-c\build azure_uamqp_filelist.txt
@REM call release_mbed_project.cmd azure_umqtt ..\..\c\azure-umqtt-c build\mbed\shared-umqtt_filelist.txt
call new_release_mbed_project.cmd serializer                ..\..\..\c\serializer\build
@REM call release_mbed_project.cmd azureiot_common ..\..\c\azure-c-shared-utility\c build\mbed\shared-util_filelist.txt
call new_release_mbed_project.cmd iothub_client_sample_amqp ..\..\..\c\iothub_client\samples\iothub_client_sample_amqp
call new_release_mbed_project.cmd iothub_client_sample_http ..\..\..\c\iothub_client\samples\iothub_client_sample_http
call new_release_mbed_project.cmd iothub_client_sample_mqtt ..\..\..\c\iothub_client\samples\iothub_client_sample_mqtt
call new_release_mbed_project.cmd remote_monitoring         ..\..\..\c\serializer\samples\remote_monitoring
call new_release_mbed_project.cmd simplesample_amqp         ..\..\..\c\serializer\samples\simplesample_amqp
call new_release_mbed_project.cmd simplesample_http         ..\..\..\c\serializer\samples\simplesample_http
call new_release_mbed_project.cmd simplesample_mqtt         ..\..\..\c\serializer\samples\simplesample_mqtt
call new_release_mbed_project.cmd temp_sensor_anomaly       ..\..\..\c\serializer\samples\temp_sensor_anomaly