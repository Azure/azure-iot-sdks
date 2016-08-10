# azure-iot-device-mqtt.translateError Requirements

## Overview
`translateError` is a method that translates MQTT errors into Azure IoT Hub errors, effectively abstracting the error that is returned to the SDK user of from the transport layer. 

## Requirements 
**SRS_NODE_DEVICE_MQTT_ERRORS_18_001: [** Any error object returned by `translateError` shall inherit from the generic `Error` Javascript object and have 2 properties:
- `message` shall contain a human-readable error message
- `transportError` shall contain the MQTT error object **]**

**SRS_NODE_DEVICE_MQTT_ERRORS_18_002: [** `translateError` shall return a `NotConnectedError` if the MQTT error message contains the string 'client disconnecting' **]**

**SRS_NODE_DEVICE_MQTT_ERRORS_18_003: [** `translateError` shall return a `FormatError` if the MQTT error message contains the string 'Invalid topic' **]**

**SRS_NODE_DEVICE_MQTT_ERRORS_18_004: [** `translateError` shall return a `NotConnectedError` if the MQTT error message contains the string 'No connection to broker' **]**

**SRS_NODE_DEVICE_MQTT_ERRORS_18_005: [** `translateError` shall return a `NotImplementedError` if the MQTT error message contains the string 'Unacceptable protocol version' **]**

**SRS_NODE_DEVICE_MQTT_ERRORS_18_006: [** `translateError` shall return a `UnauthorizedError` if the MQTT error message contains the string 'Identifier rejected' **]**

**SRS_NODE_DEVICE_MQTT_ERRORS_18_007: [** `translateError` shall return a `ServiceUnavailableError` if the MQTT error message contains the string 'Server unavailable' **]**

**SRS_NODE_DEVICE_MQTT_ERRORS_18_008: [** `translateError` shall return a `UnauthorizedError` if the MQTT error message contains the string 'Bad username or password' **]**

**SRS_NODE_DEVICE_MQTT_ERRORS_18_009: [** `translateError` shall return a `UnauthorizedError` if the MQTT error message contains the string 'Not authorized' **]**

**SRS_NODE_DEVICE_MQTT_ERRORS_18_010: [** `translateError` shall return a `InternalServerError` if the MQTT error message contains the string 'unrecognized packet type' **]**

**SRS_NODE_DEVICE_MQTT_ERRORS_18_011: [** `translateError` shall return an `Error` if none of the other string rules match **]**
