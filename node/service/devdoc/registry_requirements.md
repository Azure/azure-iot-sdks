# azure-iothub.Registry Requirements

## Overview
`Registry` represents an IoT hub’s device identity service, known as the Registry.  A consumer can add, remove, update, or read device metadata from the Device Registry.
## Example usage

```javascript
'use strict';
var Registry = require('azure-iothub').Registry;

function print(err) {
  console.log(err.toString());
}

var connectionString = '[Connection string goes here]';
var registry = Registry.fromConnectionString(connectionString);

registry.create({deviceId: 'dev1'}, function (err, dev) {
  if (err) print(err);
  else {
    console.log(dev.deviceId);
    registry.get('dev1', function (err, dev) {
      if (err) print(err);
      else {
        console.log(dev.deviceId);
        registry.delete('dev1', function(err) {
          console.log('dev1 deleted');
        });
      }
    });
  }
});
```

## Constructors/Factory methods

### Registry(config, restApiClient) [constructor]
The `Registry` constructor initializes a new instance of a `Registry` object that is used to conduct operations on the device registry.

**SRS_NODE_IOTHUB_REGISTRY_16_023: [** The `Registry` constructor shall throw a `ReferenceError` if the `config` object is falsy. **]**  
**SRS_NODE_IOTHUB_REGISTRY_05_001: [** The `Registry` constructor shall throw an `ArgumentError` if the `config` object is missing one or more of the following properties:
- `host`: the IoT Hub hostname
- `sharedAccessSignature`: shared access signature with the permissions for the desired operations.
 **]**
**SRS_NODE_IOTHUB_REGISTRY_16_024: [** The `Registry` constructor shall use the `restApiClient` provided as a second argument if it is provided. **]**  
**SRS_NODE_IOTHUB_REGISTRY_16_025: [** The `Registry` constructor shall use `azure-iothub.RestApiClient` if no `restApiClient` argument is provided. **]**  

### fromConnectionString(value) [static]
The `fromConnectionString` static method returns a new instance of the `Registry` object.

**SRS_NODE_IOTHUB_REGISTRY_05_008: [** The `fromConnectionString` method shall throw `ReferenceError` if the value argument is falsy. **]**   
**SRS_NODE_IOTHUB_REGISTRY_05_009: [** The `fromConnectionString` method shall derive and transform the needed parts from the connection string in order to create a `config` object for the constructor (see `SRS_NODE_IOTHUB_REGISTRY_05_001`). **]**   
**SRS_NODE_IOTHUB_REGISTRY_05_010: [** The `fromConnectionString` method shall return a new instance of the `Registry` object. **]**   

### fromSharedAccessSignature(value) [static]
The `fromSharedAccessSignature` static method returns a new instance of the `Registry` object.

**SRS_NODE_IOTHUB_REGISTRY_05_011: [** The `fromSharedAccessSignature` method shall throw `ReferenceError` if the value argument is falsy. **]**   
**SRS_NODE_IOTHUB_REGISTRY_05_012: [** The `fromSharedAccessSignature` method shall derive and transform the needed parts from the shared access signature in order to create a `config` object for the constructor (see `SRS_NODE_IOTHUB_REGISTRY_05_001`). **]**   
**SRS_NODE_IOTHUB_REGISTRY_05_013: [** The `fromSharedAccessSignature` method shall return a new instance of the `Registry` object. **]**    

## CRUD operation for the device registry

### create(deviceInfo, done)
The `create` method creates a device with the given device properties.

**SRS_NODE_IOTHUB_REGISTRY_07_001: [** The `create` method shall throw `ReferenceError` if the `deviceInfo` argument is falsy. **]**   
**SRS_NODE_IOTHUB_REGISTRY_07_001: [** The `create` method shall throw `ArgumentError` if the `deviceInfo` argument does not contain a `deviceId` property. **]**   
**SRS_NODE_IOTHUB_REGISTRY_16_026: [** The `create` method shall construct an HTTP request using information supplied by the caller, as follows:
```
PUT /devices/<deviceInfo.deviceId>?api-version=<version> HTTP/1.1
Authorization: <sharedAccessSignature>
Content-Type: application/json; charset=utf-8
Request-Id: <guid>

<deviceInfo>
```
**]**

### update(deviceInfo, done)
The `update` method updates an existing device identity with the given device properties.

**SRS_NODE_IOTHUB_REGISTRY_16_043: [** The `update` method shall throw `ReferenceError` if the `deviceInfo` argument is falsy. **]**   
**SRS_NODE_IOTHUB_REGISTRY_07_003: [** The `update` method shall throw `ArgumentError` if the first argument does not contain a `deviceId` property. **]**   
**SRS_NODE_IOTHUB_REGISTRY_16_027: [** The `update` method shall construct an HTTP request using information supplied by the caller, as follows:
```
PUT /devices/<deviceInfo.deviceId>?api-version=<version> HTTP/1.1
Authorization: <config.sharedAccessSignature>
Content-Type: application/json; charset=utf-8
Request-Id: <guid>
If-Match: <deviceInfo.eTag>

<deviceInfo>
```
**]**

### get(deviceId, done)
The `get` method requests information about the device with the given ID.

**SRS_NODE_IOTHUB_REGISTRY_05_006: [** The `get` method shall throw `ReferenceError` if the supplied deviceId is falsy. **]**   
**SRS_NODE_IOTHUB_REGISTRY_16_028: [** The `get` method shall construct an HTTP request using information supplied by the caller, as follows:
```
GET /devices/<deviceInfo.deviceId>?api-version=<version> HTTP/1.1
Authorization: <config.sharedAccessSignature>
Request-Id: <guid>
```
**]**

### list(done)
The `list` method requests information about the first 1000 devices registered in an IoT hub’s identity service.

**SRS_NODE_IOTHUB_REGISTRY_16_029: [** The `list` method shall construct an HTTP request using information supplied by the caller, as follows:
```
GET /devices?api-version=<version> HTTP/1.1
Authorization: <config.sharedAccessSignature>
Request-Id: <guid>
```
**]**


### delete(deviceId, done)
The `delete` method removes a device with the given ID.

**SRS_NODE_IOTHUB_REGISTRY_07_007: [** The `delete` method shall throw `ReferenceError` if the supplied deviceId is falsy. **]**   
**SRS_NODE_IOTHUB_REGISTRY_16_030: [** The `delete` method shall construct an HTTP request using information supplied by the caller, as follows:
```
DELETE /devices/<deviceInfo.deviceId>?api-version=<version> HTTP/1.1
Authorization: <config.sharedAccessSignature>
If-Match: *
Request-Id: <guid>
```
**]**

## Bulk Import/Export of devices

### importDevicesFromBlob(inputBlobContainerUri, outputBlobContainerUri, done)
The `importDevicesFromBlob` imports a list of devices from a blob named devices.txt found at the input URI given as a parameter, and output logs from the import job in a blob at found at the output URI given as a parameter.

**SRS_NODE_IOTHUB_REGISTRY_16_001: [** A `ReferenceError` shall be thrown if `inputBlobContainerUri` is falsy **]**    
**SRS_NODE_IOTHUB_REGISTRY_16_002: [** A `ReferenceError` shall be thrown if `outputBlobContainerUri` is falsy **]**    
**SRS_NODE_IOTHUB_REGISTRY_16_031: [** The `importDeviceFromBlob` method shall construct an HTTP request using information supplied by the caller, as follows:
```
POST /jobs/create?api-version=<version> HTTP/1.1
Authorization: <config.sharedAccessSignature>
Content-Type: application/json; charset=utf-8 
Request-Id: <guid>

{
  "type": "import",
  "inputBlobContainerUri": "<input container Uri given as parameter>",
  "outputBlobContainerUri": "<output container Uri given as parameter>"
}
```
**]**

### exportDevicesToBlob(outputBlobContainerUri, excludeKeys, done)
The `exportDevicesToBlob` exports a list of devices in a blob named devices.txt and logs from the export job at the output URI given as a parameter given as a parameter the export will contain security keys if the excludeKeys is false.

**SRS_NODE_IOTHUB_REGISTRY_16_004: [** A `ReferenceError` shall be thrown if outputBlobContainerUri is falsy **]**        
**SRS_NODE_IOTHUB_REGISTRY_16_032: [** The `exportDeviceToBlob` method shall construct an HTTP request using information supplied by the caller, as follows:
```
POST /jobs/create?api-version=<version> HTTP/1.1
Authorization: <config.sharedAccessSignature>
Content-Type: application/json; charset=utf-8 
Request-Id: <guid>

{
  "type": "export",
  "outputBlobContainerUri": "<output container Uri given as parameter>",
  "excludeKeysInExport": "<excludeKeys Boolean given as parameter>"
}
```
**]**

### listJobs(done)
The `listJobs` method will obtain a list of recent bulk import/export jobs (including the active one, if any).

**SRS_NODE_IOTHUB_REGISTRY_16_037: [** The `listJobs` method shall construct an HTTP request using information supplied by the caller, as follows:
```
GET /jobs?api-version=<version> HTTP/1.1
Authorization: <config.sharedAccessSignature> 
Request-Id: <guid>
```
**]**

### getJob(jobId, done)
The `getJob` method will obtain status information of the bulk import/export job identified by the `jobId` parameter.

**SRS_NODE_IOTHUB_REGISTRY_16_006: [** A `ReferenceError` shall be thrown if jobId is falsy **]**        
**SRS_NODE_IOTHUB_REGISTRY_16_038: [** The `getJob` method shall construct an HTTP request using information supplied by the caller, as follows:
```
GET /jobs/<jobId>?api-version=<version> HTTP/1.1
Authorization: <config.sharedAccessSignature> 
Request-Id: <guid>
```
**]**

### cancelJob(jobId, done)
The `cancelJob` method will cancel the bulk import/export job identified by the `jobId` parameter.

**SRS_NODE_IOTHUB_REGISTRY_16_012: [** A `ReferenceError` shall be thrown if the jobId is falsy **]**    
**SRS_NODE_IOTHUB_REGISTRY_16_039: [** The `cancelJob` method shall construct an HTTP request using information supplied by the caller as follows:
```
DELETE /jobs/<jobId>?api-version=<version> HTTP/1.1
Authorization: <config.sharedAccessSignature>
Request-Id: <guid>
```
 **]**

## Device Twin operations

### getTwin(deviceId, done)
The `getTwin` method retrieves the latest Device Twin state in the device registry.

**SRS_NODE_IOTHUB_REGISTRY_16_019: [** The `getTwin` method shall throw a `ReferenceError` if the `deviceId` parameter is falsy. **]**  
**SRS_NODE_IOTHUB_REGISTRY_16_020: [** The `getTwin` method shall throw a `ReferenceError` if the `done` parameter is falsy. **]**  
**SRS_NODE_IOTHUB_REGISTRY_16_036: [** The `getTwin` method shall call the `done` callback with a `twin` object updated with the latest property values stored in the IoT Hub service. **]**  
**SRS_NODE_IOTHUB_REGISTRY_16_049: [** The `getTwin` method shall construct an HTTP request using information supplied by the caller, as follows:
```
GET /twins/<twin.deviceId>?api-version=<version> HTTP/1.1
Authorization: <config.sharedAccessSignature>
Request-Id: <guid>
``` **]**

### updateDeviceTwin(deviceId, patch, etag, done)
The `updateDeviceTwin` method updates the device twin identified with the `deviceId` argument with the properties and tags contained in the `patch` object.

**SRS_NODE_IOTHUB_REGISTRY_16_044: [** The `updateDeviceTwin` method shall throw a `ReferenceError` if the `deviceId` argument is `undefined`, `null` or an empty string. **]**  
**SRS_NODE_IOTHUB_REGISTRY_16_045: [** The `updateDeviceTwin` method shall throw a `ReferenceError` if the `patch` argument is falsy. **]**  
**SRS_NODE_IOTHUB_REGISTRY_16_046: [** The `updateDeviceTwin` method shall throw a `ReferenceError` if the `etag` argument is falsy. **]**  
**SRS_NODE_IOTHUB_REGISTRY_16_050: [** The `updateDeviceTwin` method shall call the `done` callback with a `twin` object updated with the latest property values stored in the IoT Hub service. **]**  
**SRS_NODE_IOTHUB_REGISTRY_16_048: [** The `updateDeviceTwin` method shall construct an HTTP request using information supplied by the caller, as follows:
```
PATCH /twins/<deviceId>?api-version=<version> HTTP/1.1
Authorization: <config.sharedAccessSignature>
Content-Type: application/json; charset=utf-8
Request-Id: <guid>
If-Match: <etag>

<patch>
```
**]**

### createQuery(sqlQuery, pageSize)
The `createQuery` method initializes a new instance of a `DeviceQuery` object with the `sqlQuery` and `pageSize` arguments.

**SRS_NODE_IOTHUB_REGISTRY_16_051: [** The `createQuery` method shall throw a `ReferenceError` if the `sqlQuery` argument is falsy. **]**

**SRS_NODE_IOTHUB_REGISTRY_16_052: [** The `createQuery` method shall throw a `TypeError` if the `sqlQuery` argument is not a string. **]**

**SRS_NODE_IOTHUB_REGISTRY_16_053: [** The `createQuery` method shall throw a `TypeError` if the `pageSize` argument is not `null`, `undefined` or a number. **]**

**SRS_NODE_IOTHUB_REGISTRY_16_054: [** The `createQuery` method shall return a new `Query` instance initialized with the `sqlQuery` and the `pageSize` argument if specified. **]**

### _executeQueryFunc(sqlQuery, pageSize)
The `_executeQueryFunc` method runs a SQL query against the device databases and calls the `done` callback with the results.

**SRS_NODE_IOTHUB_REGISTRY_16_057: [** The `_executeQueryFunc` method shall construct an HTTP request as follows:
```
POST /devices/query?api-version=<version> HTTP/1.1
Authorization: <config.sharedAccessSignature>
Content-Type: application/json; charset=utf-8
x-ms-continuation: continuationToken
x-ms-max-item-count: pageSize
Request-Id: <guid>

{
  query: <sqlQuery>
}
```
**]**

### All HTTP requests
All HTTP requests to the registry API should implement the following requirements:

**SRS_NODE_IOTHUB_REGISTRY_16_040: [** All requests shall contain a `User-Agent` header that uniquely identifies the SDK and SDK version used. **]**
**SRS_NODE_IOTHUB_REGISTRY_16_041: [** All requests shall contain a `Request-Id` header that uniquely identifies the request and allows tracing of requests/responses in the logs. **]**  
**SRS_NODE_IOTHUB_REGISTRY_16_042: [** All requests shall contain a `Authorization` header that contains a valid shared access key. **]**
**SRS_NODE_IOTHUB_REGISTRY_16_033: [** If any registry operation method encounters an error before it can send the request, it shall invoke the `done` callback function and pass the standard JavaScript `Error` object with a text description of the error (err.message). **]**   
**SRS_NODE_IOTHUB_REGISTRY_16_035: [** When any registry operation method receives an HTTP response with a status code >= 300, it shall invoke the `done` callback function with an error translated using the requirements detailed in `registry_http_errors_requirements.md` **]**   
**SRS_NODE_IOTHUB_REGISTRY_16_034: [** When any registry operation receives an HTTP response with a status code < 300, it shall invoke the `done` callback function with the following arguments:
- `err`: `null`
- `result`: A javascript object parsed from the body of the HTTP response
- `response`: the Node.js `http.ServerResponse` object returned by the transport
 **]**