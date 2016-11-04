# azure-iot-device.deviceMethod.DeviceMethodRequest requirements

# Overview
`DeviceMethodRequest` is a class that captures the data passed in from the service when a device method call is made.

# Public API

## DeviceMethodRequest(requestId, methodName, body) [constructor]
Initializes a new instance of the `DeviceMethodRequest` class.

**SRS_NODE_DEVICE_METHOD_REQUEST_13_001: [** `DeviceMethodRequest` shall throw a `ReferenceError` if `requestId` is falsy or is not a string. **]**

**SRS_NODE_DEVICE_METHOD_REQUEST_13_002: [** `DeviceMethodRequest` shall throw an `Error` if `requestId` is an empty string. **]**

**SRS_NODE_DEVICE_METHOD_REQUEST_13_003: [** `DeviceMethodRequest` shall throw a `ReferenceError` if `methodName` is falsy or is not a string. **]**

**SRS_NODE_DEVICE_METHOD_REQUEST_13_004: [** `DeviceMethodRequest` shall throw an `Error` if `methodName` is an empty string. **]**

