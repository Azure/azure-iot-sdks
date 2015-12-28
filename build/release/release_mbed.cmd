@REM  Copyright (c) Microsoft. All rights reserved.
@REM  Licensed under the MIT license. See LICENSE file in the project root for full license information.

echo Releasing mbed bits ...

call release_mbed_project.cmd iothub_amqp_transport ..\..\c\iothub_client\build iothub_amqp_transport_filelist.txt
call release_mbed_project.cmd iothub_http_transport ..\..\c\iothub_client\build iothub_http_transport_filelist.txt
call release_mbed_project.cmd iothub_client ..\..\c\iothub_client\build iothub_client_filelist.txt
call release_mbed_project.cmd serializer ..\..\c\serializer\build serializer_filelist.txt
call release_mbed_project.cmd azureiot_common ..\..\c\common\build mbed\common_filelist.txt
call release_mbed_project.cmd iothub_client_sample_amqp ..\..\c\iothub_client\samples\iothub_client_sample_amqp mbed\filelist.txt FRDM-K64F
call release_mbed_project.cmd iothub_client_sample_http ..\..\c\iothub_client\samples\iothub_client_sample_http mbed\filelist.txt FRDM-K64F
call release_mbed_project.cmd remote_monitoring ..\..\c\serializer\samples\remote_monitoring mbed\filelist.txt FRDM-K64F
call release_mbed_project.cmd simplesample_amqp ..\..\c\serializer\samples\simplesample_amqp mbed\filelist.txt FRDM-K64F
call release_mbed_project.cmd simplesample_http ..\..\c\serializer\samples\simplesample_http mbed\filelist.txt FRDM-K64F
call release_mbed_project.cmd temp_sensor_anomaly ..\..\c\serializer\samples\temp_sensor_anomaly mbed\filelist.txt FRDM-K64F