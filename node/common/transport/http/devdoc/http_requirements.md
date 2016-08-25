# azure-iot-common.Http Requirements

## Overview
Http provides generalized HTTP support for higher-level libraries that will be communicating with Azure IoT Hub.
## Example usage

```js
'use strict';
var Http = require('azure-iot-common').Http;
var Message = require('azure-iot-common').Message;

function handleResponse(err, body, response) {
  if (err) console.log(err.toString());
  var msg = http.toMessage(response, body);
  if (msg) console.log(msg);
}

var http = new Http();
var request = http.buildRequest(
  'GET', '/path/to/resource', headers, hostname, handleResponse);
request.write('request body');
request.end();
```

## Public Interface
### buildRequest(method, path, httpHeaders, host, done)
The buildRequest method receives all the information necessary to make an HTTP request to an IoT hub.  This method should only be called by higher-level Azure IoT Hub libraries, and does not validate input.

**SRS_NODE_HTTP_05_001: [** `buildRequest` shall accept the following arguments:
- `method` - an HTTP verb, e.g., 'GET', 'POST', 'DELETE'
- `path` - the path to the resource, not including the hostname
- `httpHeaders` - an object whose properties represent the names and values of HTTP headers to include in the request
- `host` - the fully-qualified DNS hostname of the IoT hub
- `x509Options` - *[optional]* the x509 certificate, key and passphrase that are needed to connect to the service using x509 certificate authentication
- `done` - a callback that will be invoked when a completed response is returned from the server **]**

**SRS_NODE_HTTP_05_002: [** `buildRequest` shall return a Node.js https.ClientRequest object, upon which the caller must invoke the end method in order to actually send the request. **]**

**SRS_NODE_HTTP_05_003: [** If `buildRequest` encounters an error before it can send the request, it shall invoke the `done` callback function and pass the standard JavaScript `Error` object with a text description of the error (err.message). **]**

**SRS_NODE_HTTP_05_005: [** When an HTTP response is received, the callback function indicated by the `done` argument shall be invoked with the following arguments:
- `err` - the standard JavaScript `Error` object if status code >= 300, otherwise null
- `body` - the body of the HTTP response as a string
- `response` - the Node.js `http.ServerResponse` object returned by the transport **]**

**SRS_NODE_HTTP_16_001: [** If `x509Options` is specified, the certificate, key and passphrase in the structure shall be used to authenticate the connection. **]**

### toMessage(response, body)
The `toMessage` function converts an HTTP response to an IoT Hub Message.

**SRS_NODE_HTTP_05_006: [** If the status code of the HTTP response < 300, `toMessage` shall create a new `azure-iot-common.Message` object with data equal to the body of the HTTP response. **]**

**SRS_NODE_HTTP_05_007: [** If the HTTP response has an `iothub-messageid` header, it shall be saved as the messageId property on the created `Message`. **]**

**SRS_NODE_HTTP_05_008: [** If the HTTP response has an `iothub-to` header, it shall be saved as the to property on the created `Message`. **]**

**SRS_NODE_HTTP_05_009: [** If the HTTP response has an `iothub-expiry` header, it shall be saved as the `expiryTimeUtc` property on the created `Message`. **]**

**SRS_NODE_HTTP_05_010: [** If the HTTP response has an `iothub-correlationid` header, it shall be saved as the `correlationId` property on the created `Message`. **]**

**SRS_NODE_HTTP_13_001: [** If the HTTP response has a header with the prefix `iothub-app-` then a new property with the header name and value as the key and value shall be added to the message. **]**

**SRS_NODE_HTTP_05_011: [** If the HTTP response has an `etag` header, it shall be saved as the `lockToken` property on the created `Message`, minus any surrounding quotes. **]**

### parseErrorBody(body)
The `parseErrorBody` function converts the body of an HTTP error response into an object.

**SRS_NODE_DEVICE_HTTP_16_001: [** If the error body can be parsed the `parseErrorBody` function shall return an object with the following properties:
- `code`: the code of the error that the server is returning
- `message`: the human-readable error message **]**

**SRS_NODE_DEVICE_HTTP_16_002: [** If the error body cannot be parsed the `parseErrorBody` function shall return `null`. **]**


