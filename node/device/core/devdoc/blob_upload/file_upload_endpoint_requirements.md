# azure-iot-device.FileUploadApi requirements

## Overview
The `FileUploadApi` class provide methods to get an Azure Storage blob SAS URI from the service and to notify the IoT Hub service when the upload is finished.

## Usage

## Public API
### FileUploadApi(deviceId, hostname, transport) [constructor]

**SRS_NODE_FILE_UPLOAD_ENDPOINT_16_002: [** `FileUploadApi` shall throw a `ReferenceError` if `deviceId` is falsy. **]**

**SRS_NODE_FILE_UPLOAD_ENDPOINT_16_003: [** `FileUploadApi` shall throw a `ReferenceError` if `hostname` is falsy. **]**

**SRS_NODE_FILE_UPLOAD_ENDPOINT_16_018: [** `FileUploadApi` shall instantiate the default `azure-iot-http-base.Http` transport if `transport` is not specified, otherwise it shall use the specified transport. **]**

### getBlobSharedAccessSignature(blobName, iotHubSas, done)

**SRS_NODE_FILE_UPLOAD_ENDPOINT_16_004: [** `getBlobSharedAccessSignature` shall throw a `ReferenceError` if `blobName` is falsy. **]**

**SRS_NODE_FILE_UPLOAD_ENDPOINT_16_005: [** `getBlobSharedAccessSignature` shall throw a `ReferenceError` if `auth` is falsy. **]**

**SRS_NODE_FILE_UPLOAD_ENDPOINT_16_006: [** `getBlobSharedAccessSignature` shall create a `GET` HTTP request to a path formatted as the following:
`/devices/<deviceId>/files/<filename>?api-version=<api-version>`
**]**

**SRS_NODE_FILE_UPLOAD_ENDPOINT_16_007: [** The `GET` HTTP request shall have the following headers:
```
Accept: 'application/json'
Host: <hostname>
Authorization: <iotHubSas>
'User-Agent': <sdk name and version>,
```
**]**

**SRS_NODE_FILE_UPLOAD_ENDPOINT_16_008: [** `getBlobSasUri` shall call the `done` callback with an `Error` object if the request fails **]**

**SRS_NODE_FILE_UPLOAD_ENDPOINT_16_009: [** `getBlobSasUri` shall call the `done` callback with a `null` error object and a result object containing a correlation ID and a SAS parameters if the request succeeds, in the following format:
{
  correlationId: '<correlationIdString>',
  hostName: '<hostName>',
  blobName: '<blobName>',
  containerName: '<containerName>',
  sasToken: '<sasUriString>'
}
 **]**

### notifyUploadComplete(correlationId, sharedAccessSignature, uploadResult, done)
**SRS_NODE_FILE_UPLOAD_ENDPOINT_16_010: [** `notifyUploadComplete` shall throw a `ReferenceError` if `correlationId` is falsy. **]**

**SRS_NODE_FILE_UPLOAD_ENDPOINT_16_011: [** `notifyUploadComplete` shall throw a `ReferenceError` if `auth` is falsy. **]**

**SRS_NODE_FILE_UPLOAD_ENDPOINT_16_012: [** `notifyUploadComplete` shall throw a `ReferenceError` if `uploadResult` is falsy. **]**

**SRS_NODE_FILE_UPLOAD_ENDPOINT_16_013: [** `notifyUploadComplete` shall create a `POST` HTTP request to a path formatted as the following:
`/devices/<deviceId>/files/<correlationId>?api-version=<api-version>` **]**

**SRS_NODE_FILE_UPLOAD_ENDPOINT_16_014: [** The `POST` HTTP request shall have the following headers:
```
Host: <hostname>,
Authorization: <sharedAccessSignature>,
'User-Agent': <version>,
'Content-Type': 'application/json; charset=utf-8',
'Content-Length': <content length>,
'iothub-name': <hub name>
``` 
**]**

**SRS_NODE_FILE_UPLOAD_ENDPOINT_16_015: [** The `POST` HTTP request shall have the following body:
```
{
  isSuccess: <true/false>,
  statusCode: <upload status code from the blob upload>,
  statusDescription: <string describing the status code>
}
```
**]**

**SRS_NODE_FILE_UPLOAD_ENDPOINT_16_016: [** `notifyUploadComplete` shall call the `done` callback with an `Error` object if the request fails. **]**

**SRS_NODE_FILE_UPLOAD_ENDPOINT_16_017: [** `notifyUploadComplete` shall call the `done` callback with no parameters if the request succeeds. **]**