# azure-iot-device.deviceMethod.DeviceMethodResponse requirements

# Overview
`DeviceMethodResponse` provides functionality that allows a device method implementation to construct and send a response back to the service for a device method call. An instance of this class is passed as the second parameter to the callback registered via the `azure-iot-device.Client.onDeviceMethod` method.

# Public API

## DeviceMethodResponse(requestId, transport) [constructor]
Initializes a new instance of the `DeviceMethodResponse` class.

**SRS_NODE_DEVICE_METHOD_RESPONSE_13_001: [** `DeviceMethodResponse` shall throw a `ReferenceError` if `requestId` is falsy or is not a string. **]**

**SRS_NODE_DEVICE_METHOD_RESPONSE_13_002: [** `DeviceMethodResponse` shall throw an `Error` if `requestId` is an empty string. **]**

**SRS_NODE_DEVICE_METHOD_RESPONSE_13_006: [** `DeviceMethodResponse` shall throw a `ReferenceError` if `transport` is falsy. **]**

## write(data)
Appends the data that's supplied to the output buffer to be sent as the payload for the method's response.

**SRS_NODE_DEVICE_METHOD_RESPONSE_13_003: [** `DeviceMethodResponse.write` shall throw a `ReferenceError` if `data` is falsy. **]**

**SRS_NODE_DEVICE_METHOD_RESPONSE_13_004: [** The `DeviceMethodResponse.write` method shall accumulate the `data` passed to it as internal state. **]**

**SRS_NODE_DEVICE_METHOD_RESPONSE_13_005: [** `DeviceMethodResponse.write` shall throw an `Error` object if it is invoked after `DeviceMethodResponse.end` has been called **]**.

## end(status, done)
Sends the device method's response back to the service via the underlying transport object using the `status` parameter as the status of the method call.

**SRS_NODE_DEVICE_METHOD_RESPONSE_13_007: [** `DeviceMethodResponse.end` shall throw a `ReferenceError` if `status` is undefined or not a number. **]**

**SRS_NODE_DEVICE_METHOD_RESPONSE_13_008: [** `DeviceMethodResponse.end` shall notify the service and supply the response for the request along with the `status` by calling `sendMethodResponse` on the underlying transport object. **]**

**SRS_NODE_DEVICE_METHOD_RESPONSE_13_009: [** `DeviceMethodResponse.end` shall throw an `Error` object if it is called more than once for the same request. **]**

**SRS_NODE_DEVICE_METHOD_RESPONSE_13_010: [** `DeviceMethodResponse.end` shall invoke the callback specified by `done` if it is not falsy. **]**

**SRS_NODE_DEVICE_METHOD_RESPONSE_13_011: [** `DeviceMethodResponse.end` shall pass the status of sending the response to the service to `done`. **]**
