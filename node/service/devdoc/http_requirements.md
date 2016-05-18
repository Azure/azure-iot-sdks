#Azure-iothub.Http Requirements

##Overview
Http provides HTTP protocol support to the service SDK for communicating with Azure IoT Hub.

##Example Usage
```js
'use strict';
var Http = require('azure-iothub').Http;

function print(err) {
  console.log(err.toString());
}
var config = {
  host: /* ... */,
  hubName: /* ... */,
  sharedAccessSignature: /* ... */
};

var device = {
  deviceId: 'dev1',
};

var http = new Https(config);
http.createDevice('/devices/dev1', device, function (err, dev) {
  if (err) print(err);
  else {
    console.log(dev.deviceId);
    http.deleteDevice('/devices/dev1', function (err) {
      if (err) print(err);
    });
  }
});
```
###Http constructor
**SRS_NODE_IOTHUB_HTTP_05_001: [**The Http constructor shall accept an object with three properties:
host – (string) the fully-qualified DNS hostname of an IoT hub
hubName – (string) the name of the IoT hub, which is the first segment of hostname
sharedAccessSignature – (string) a shared access signature generated from the credentials of a policy with the appropriate registry permissions (read and/or write).**]**
     
###createDevice(path, deviceInfo, done)
**SRS_NODE_IOTHUB_HTTP_05_002: [**The createDevice method shall construct an HTTP request using information supplied by the caller, as follows:
```
PUT [path]?api-version=[version] HTTP/1.1
Authorization: [config.sharedAccessSignature]
iothub-name: [config.hubName]
Content-Type: application/json; charset=utf-8
If-Match: *
Host: [config.host]
[deviceInfo]
```
**]**

###updateDevice(path, deviceInfo, done)
**SRS_NODE_IOTHUB_HTTP_05_003: [**The updateDevice method shall construct an HTTP request using information supplied by the caller, as follows:
```
PUT [path]?api-version=[version] HTTP/1.1
Authorization: [config.sharedAccessSignature]
iothub-name: [config.hubName]
Content-Type: application/json; charset=utf-8
Host: [config.host]
[deviceInfo]
```
**]**
 
##getDevice(path, done)
**SRS_NODE_IOTHUB_HTTP_05_004: [**The getDevice method shall construct an HTTP request using information supplied by the caller, as follows:
```
GET [path]?api-version=[version] HTTP/1.1
Authorization: [config.sharedAccessSignature]
iothub-name: [config.hubName]
Host: [config.host]
```
**]**
 
###listDevices(path, done)
**SRS_NODE_IOTHUB_HTTP_05_005: [**The listDevices method shall construct an HTTP request using information supplied by the caller, as follows:
```
GET [path]?api-version=[version] HTTP/1.1
Authorization: [config.sharedAccessSignature]
iothub-name: [config.hubName]
Host: [config.host]
```
**]**
 
###deleteDevice(path, done)
**SRS_NODE_IOTHUB_HTTP_05_006: [**The deleteDevice method shall construct an HTTP request using information supplied by the caller, as follows:
```
DELETE [path]?api-version=[version] HTTP/1.1
Authorization: [config.sharedAccessSignature]
iothub-name: [config.hubName]
If-Match: *
Host: [config.host]
```
**]**   

###importDevicesFromBlob (path, importRequest, done)
**SRS_NODE_IOTHUB_HTTP_16_005: [**The importDeviceFromBlob method shall construct an HTTP request using information supplied by the caller, as follows:
```
POST [path]?api-version=[version] HTTP/1.1
Authorization: [config.sharedAccessSignature]
Content-Type: application/json; charset=utf-8 
Request-Id: [guid]
[importRequest]
```
**]**
 
##exportDevicesToBlob (path, exportRequest, done)
**SRS_NODE_IOTHUB_HTTP_16_004: [**The exportDeviceToBlob method shall construct an HTTP request using information supplied by the caller, as follows:
```
POST [path]?api-version=[version] HTTP/1.1
Authorization: [config.sharedAccessSignature]
Content-Type: application/json; charset=utf-8 
Request-Id: [guid]
[exportRequest]
```
**]**

###listJobs(done)
**SRS_NODE_IOTHUB_HTTP_16_002: [**The listJobs method shall construct an HTTP request using information supplied by the caller, as follows:
```
GET [path]?api-version=[version] HTTP/1.1
Authorization: [config.sharedAccessSignature] 
Request-Id: [guid]
```
**]** 

###getJob(path, done)
**SRS_NODE_IOTHUB_HTTP_16_003: [**The getJob method shall construct an HTTP request using information supplied by the caller, as follows:
```
GET [path]?api-version=[version] HTTP/1.1
Authorization: [config.sharedAccessSignature] 
Request-Id: [guid]**]** 
###cancelJob(path, done)
**SRS_NODE_IOTHUB_HTTP_16_001: [**The cancelJob method shall construct an HTTP request using information supplied by the caller as follows:
DELETE [path]?api-version=[version] HTTP/1.1
Authorization: [config.sharedAccessSignature]
Request-Id: [guid]
```
**]** 

###queryDevicesByTags(path, tagQuery, maxCount, done)
**SRS_NODE_IOTHUB_HTTP_07_001: [**The `queryDevicesByTags` method shall construct an HTTP request using information supplied by the caller as follows:
```
POST [path]?api-version=[version]&tags=[tagList]&top=[maxCount] HTTP/1.1
Authorization: [config.sharedAccessSignature]
Content-Type: application/json; charset=utf-8 
Host: [host-name]
```
**]**

###queryDevices(path, query, done)
**SRS_NODE_IOTHUB_HTTP_16_006: [** The `queryDevices` method shall construct an HTTP request using information supplied by the caller as follows:
```
POST [path]?api-version=[version] HTTP/1.1
Authorization: [config.sharedAccessSignature]
Content-Type: application/json; charset=utf-8 
Host: [host-name]
[query]
``` 
**]**

###setServiceProperties(path, serviceProperties, done)
**SRS_NODE_IOTHUB_HTTP_16_007: [** the `setServiceProperties` method shall construct an HTTP request using information supplied by the caller as follows:
```
PUT [path]?api-version=[version] HTTP/1.1
Authorization: [config.sharedAccessSignature]
Content-Type: application/json; charset=utf-8 
Host: [host-name]
[serviceProperties]
```
**]**

###getRegistryStatistics(path, done)
**SRS_NODE_IOTHUB_HTTP_16_007: [** The `getRegistryStatistics` method shall construct an HTTP request using information supplied by the caller as follows:
```
GET /statistics/devices?api-version=[version] HTTP/1.1
Authorization: [config.sharedAccessSignature]
Accept: application/json,
Host: [host-name]
[serviceProperties]
```
**]**

###All HTTP requests
**SRS_NODE_IOTHUB_HTTP_05_007: [**If any registry operation method encounters an error before it can send the request, it shall invoke the done callback function and pass the standard JavaScript Error object with a text description of the error (err.message).]  
**SRS_NODE_IOTHUB_HTTP_05_008: [**When any registry operation method receives an HTTP response with a status code ]= 300, it shall invoke the done callback function with the following arguments:
err - the standard JavaScript Error object, with the Node.js
 http.ServerResponse object attached as the property response.**]**

**SRS_NODE_IOTHUB_HTTP_05_009: [**When any registry operation receives an HTTP response with a status code [ 300, it shall invoke the done callback function with the following arguments:
err - null
body – the body of the HTTP response
response - the Node.js http.ServerResponse object returned by the transport**]**
