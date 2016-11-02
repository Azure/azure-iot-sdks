# DeviceMethod requirements

## Overview
`DeviceMethod` exposes methods for invoking device methods, and receiving the result of those.

##Example usage
```js
'use strict';
var DeviceMethod = require('azure-iothub').DeviceMethod;
var RestApiClient = require('azure-iothub').RestApiClient;

var restClient = new RestApiClient({host: 'host', sharedAccessSignature: 'sharedAccessSignature'});
var params = {
  methodName: 'getDeviceLogs',
  payload: {
    maxSizeInKb: 256
  },
  timeoutInSeconds: 180
};

var rebootMethod = new DeviceMethod(params, restClient);
rebootMethod.invokeOn('deviceId', function(err, result, response) {
  if(err) {
    console.error('Could not invoke method: ' + err.message);
  } else {
    console.log('Method completed with status: ' + result.status);
    console.log(JSON.stringify(result.payload, null, 2));
  }
});
```

##Public Interface

### DeviceMethod(params, restApiClient) [Constructor]
The `DeviceMethod` constructor initializes a new instance of a `DeviceMethod` object.

**SRS_NODE_IOTHUB_DEVICE_METHOD_16_004: [** The `DeviceMethod` constructor shall throw a `ReferenceError` if `params.methodName` is `null`, `undefined` or an empty string. **]**  

**SRS_NODE_IOTHUB_DEVICE_METHOD_16_005: [** The `DeviceMethod` constructor shall throw a `TypeError` if `params.methodName` is not a `string`. **]**  

**SRS_NODE_IOTHUB_DEVICE_METHOD_16_013: [** The `DeviceMethod` constructor shall set the `DeviceMethod.params` property to the `params` argument value. **]**

**SRS_NODE_IOTHUB_DEVICE_METHOD_16_006: [** The `DeviceMethod` constructor shall set the `DeviceMethod.params.timeoutInSeconds` property value to the `params.timeoutInSeconds` argument value or to the default (`30`) if the `timeoutInSeconds` value is falsy. **]**  

**SRS_NODE_IOTHUB_DEVICE_METHOD_16_015: [** The `DeviceMethod` constructor shall set the `DeviceMethod.params.payload` property value to the `params.payload` argument value or to the default (`null`) if the `payload` argument is `null` or `undefined`. **]**  

### invokeOn(deviceId, done)
The `invokeOn` uses the IoT Hub service to run a method on a device and returns the result. The payload will be sent to the device and act as method arguments. The `timeoutInSeconds` value
is expressed in seconds and indicates how much time a method should wait for a result before timing out, once the connection between the service and the device has been established.

**SRS_NODE_IOTHUB_DEVICE_METHOD_16_008: [** The `invokeOn` method shall throw a `ReferenceError` if `deviceId` is `null`, `undefined` or an empty string. **]**  

**SRS_NODE_IOTHUB_DEVICE_METHOD_16_011: [** The `invokeOn` method shall construct an HTTP request using information supplied by the caller, as follows:
```
POST /twins/<deviceId>/methods?api-version=<version> HTTP/1.1
Authorization: <config.sharedAccessSignature> 
Content-Type: application/json; charset=utf-8
Request-Id: <guid>
{
  "methodName": <DeviceMethod.params.name>,
  "timeoutInSeconds": <DeviceMethod.params.timeoutInSeconds>,
  "payload": <DeviceMethod.params.payload>
}
```
**]**  

**SRS_NODE_IOTHUB_DEVICE_METHOD_16_009: [** The `invokeOn` method shall invoke the `done` callback with a standard javascript `Error` object if the method execution failed. **]**  

**SRS_NODE_IOTHUB_DEVICE_METHOD_16_010: [** The `invokeOn` method shall invoke the `done` callback with a `null` first argument, a result second argument and a transport-specific response third argument if the method execution succeeded. **]**