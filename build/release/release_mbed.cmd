@REM  Copyright (c) Microsoft. All rights reserved.
@REM  Licensed under the MIT license. See LICENSE file in the project root for full license information.

echo Releasing mbed bits ...

call release_mbed_project.cmd ..\..\..\c\iothub_client\build\iothub_amqp_transport
call release_mbed_project.cmd ..\..\..\c\iothub_client\build\iothub_http_transport
call release_mbed_project.cmd ..\..\..\c\iothub_client\build\iothub_mqtt_transport
call release_mbed_project.cmd ..\..\..\c\iothub_client\build\iothub_client
call release_mbed_project.cmd ..\..\..\c\azure-uamqp-c\build
call release_mbed_project.cmd ..\..\..\c\azure-umqtt-c\build
call release_mbed_project.cmd ..\..\..\c\serializer\build
call release_mbed_project.cmd ..\..\..\c\azure-c-shared-utility\build
call release_mbed_project.cmd ..\..\..\c\iothub_client\samples\iothub_client_sample_amqp
call release_mbed_project.cmd ..\..\..\c\iothub_client\samples\iothub_client_sample_http
call release_mbed_project.cmd ..\..\..\c\iothub_client\samples\iothub_client_sample_mqtt
call release_mbed_project.cmd ..\..\..\c\serializer\samples\remote_monitoring
call release_mbed_project.cmd ..\..\..\c\serializer\samples\simplesample_amqp
call release_mbed_project.cmd ..\..\..\c\serializer\samples\simplesample_http
call release_mbed_project.cmd ..\..\..\c\serializer\samples\simplesample_mqtt
call release_mbed_project.cmd ..\..\..\c\serializer\samples\temp_sensor_anomaly

call release_mbed_project.cmd ..\..\..\c\iothub_client\build\iothub_amqp_transport iothub_amqp_transport_bld
call release_mbed_project.cmd ..\..\..\c\iothub_client\build\iothub_http_transport iothub_http_transport_bld
call release_mbed_project.cmd ..\..\..\c\iothub_client\build\iothub_mqtt_transport iothub_mqtt_transport_bld
call release_mbed_project.cmd ..\..\..\c\iothub_client\build\iothub_client iothub_client_bld
call release_mbed_project.cmd ..\..\..\c\azure-uamqp-c\build azure_uamqp_c_bld
call release_mbed_project.cmd ..\..\..\c\azure-umqtt-c\build azure_umqtt_c_bld
call release_mbed_project.cmd ..\..\..\c\serializer\build serializer_bld
call release_mbed_project.cmd ..\..\..\c\azure-c-shared-utility\build azure_c_shared_utility_bld
call release_mbed_project.cmd ..\..\..\c\iothub_client\samples\iothub_client_sample_amqp iothub_client_sample_amqp_bld
call release_mbed_project.cmd ..\..\..\c\iothub_client\samples\iothub_client_sample_http iothub_client_sample_http_bld
call release_mbed_project.cmd ..\..\..\c\iothub_client\samples\iothub_client_sample_mqtt iothub_client_sample_mqtt_bld
call release_mbed_project.cmd ..\..\..\c\serializer\samples\remote_monitoring remote_monitoring_bld
call release_mbed_project.cmd ..\..\..\c\serializer\samples\simplesample_amqp simplesample_amqp_bld
call release_mbed_project.cmd ..\..\..\c\serializer\samples\simplesample_http simplesample_http_bld
call release_mbed_project.cmd ..\..\..\c\serializer\samples\simplesample_mqtt simplesample_mqtt_bld
call release_mbed_project.cmd ..\..\..\c\serializer\samples\temp_sensor_anomaly temp_sensor_anomaly_bld