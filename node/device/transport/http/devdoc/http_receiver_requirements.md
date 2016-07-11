# azure-iot-device.HttpReceiver Requirements
 
## Overview
`HttpReceiver` is used to receive and settle messages using the HTTP protocol.

## Example usage
```js
'use strict';
var Http = require('azure-iot-device').Http;

function print(err) {
  console.log(err.toString());
}
var config = {
  host: /* ... */,
  deviceId: /* ... */,
  sharedAccessSignature: /* ... */
};

var http = new Https(config);
http.sendEvent('my message', function (err, msg) {
  if (err) print(err);
  else console.log('sent message: ' + msg.getData());
});

http.getReceiver(function (receiver) {
  else {
    receiver.setOptions({interval: 10; at: null; cron: null; drain: true });
    receiver.on('message', function(msg) { 
      console.log('received: ' + msg.body); 
      receiver.complete(msg);
    });
    receiver.on('errorReceived', function(err) { console.log(Error: ' + err.message); });
  }
});
```

## Public Interface

### receive(done) [Transport-specific]
The `receive` method queries the IoT hub for the next message in the indicated device’s cloud-to-device (c2d) message queue. It is a transport-specific method.

**SRS_NODE_DEVICE_HTTP_05_004: [** The receive method shall construct an HTTP request using information supplied by the caller, as follows:
GET <config.host>/devices/<config.deviceId>/messages/devicebound?api-version=<version> HTTP/1.1
iothub-to: /devices/<config.deviceId>/messages/devicebound 
User-Agent: <version string>
Host: <config.host>
**]**

### setOptions(opts)
**SRS_NODE_DEVICE_HTTP_RECEIVER_16_001: [** The setOptions method shall accept an argument formatted as such:
 {
     interval: (Number),
     at: (Date)
     cron: (string)
     drain: (Boolean)
     manualPolling: (Boolean)
}
**]**

**SRS_NODE_DEVICE_HTTP_RECEIVER_16_008: [**Only one of the `interval`, at, `manualPolling` and `cron` fields should be populated: if more than one is populated, an ArgumentError shall be thrown.**]** 

**SRS_NODE_DEVICE_HTTP_RECEIVER_16_021: [**If `opts.interval` is set, messages should be received repeatedly at that interval**]** 

**SRS_NODE_DEVICE_HTTP_RECEIVER_16_002: [**If `opts.interval` is not a number, an ArgumentError should be thrown.**]** 

**SRS_NODE_DEVICE_HTTP_RECEIVER_16_005: [**If `opts.interval` is a negative number, an ArgumentError should be thrown.**]** 

**SRS_NODE_DEVICE_HTTP_RECEIVER_16_003: [**if `opts.at` is set, messages shall be received at the Date and time specified.**]** 

**SRS_NODE_DEVICE_HTTP_RECEIVER_16_022: [**If `opts.at` is not a Date object, an ArgumentError should be thrown**]** 

**SRS_NODE_DEVICE_HTTP_RECEIVER_16_020: [**If `opts.cron` is set messages shall be received according to the schedule described by the expression.**]** 

**SRS_NODE_DEVICE_HTTP_RECEIVER_16_004: [**if `opts.cron` is set it shall be a string that can be interpreted as a cron expression**]** 

**SRS_NODE_DEVICE_HTTP_RECEIVER_16_007: [**If `opts.cron` is not a parseable cron string, an ArgumentError should be thrown.**]** 

**SRS_NODE_DEVICE_HTTP_RECEIVER_16_017: [**If `opts.drain` is true all messages in the queue should be pulled at once.**]** 

**SRS_NODE_DEVICE_HTTP_RECEIVER_16_018: [**If `opts.drain` is false, only one message shall be received at a time**]** 

**SRS_NODE_DEVICE_HTTP_RECEIVER_16_019: [**If the receiver is already running with a previous configuration, the existing receiver should be restarted with the new configuration**]** 

**SRS_NODE_DEVICE_HTTP_RECEIVER_16_023: [**If `opts.manualPolling` is true, messages shall be received only when receive() is called**]** 

### abandon(message)
**SRS_NODE_DEVICE_HTTP_RECEIVER_16_009: [**`abandon` shall construct an HTTP request using information supplied by the caller, as follows:
```
POST <config.host>/devices/<config.deviceId>/messages/devicebound/<lockToken>/abandon?api-version=<version> HTTP/1.1
If-Match: <lockToken>
Host: <config.host>
```
**]** 

**SRS_NODE_DEVICE_HTTP_RECEIVER_16_025: [**If `message` is falsy, `abandon` should throw a `ReferenceException`**]** 

**SRS_NODE_DEVICE_HTTP_RECEIVER_16_024: [**When successful, `abandon` should call the `done` callback with a null error object and a result object of type `MessageAbandoned`**]** 

### reject(message)
**SRS_NODE_DEVICE_HTTP_RECEIVER_16_010: [**`reject` shall construct an HTTP request using information supplied by the caller, as follows:
```
DELETE <config.host>/devices/<config.deviceId>/messages/devicebound/<lockToken>?api-version=<version>&reject HTTP/1.1
If-Match: <lockToken>
Host: <config.host>
```
**]** 

**SRS_NODE_DEVICE_HTTP_RECEIVER_16_026: [**If `message` is falsy, `reject` should throw a `ReferenceException`**]** 

**SRS_NODE_DEVICE_HTTP_RECEIVER_16_029: [**When successful, `reject` should call the `done` callback with a `null` error object and a result object of type `MessageRejected`**]** 

### complete(message)
**SRS_NODE_DEVICE_HTTP_RECEIVER_16_011: [**complete shall construct an HTTP request using information supplied by the caller, as follows:
```
DELETE <config.host>/devices/<config.deviceId>/messages/devicebound/<lockToken>?api-version=<version> HTTP/1.1
If-Match: <lockToken>
Host: <config.host> 
```
**]**

**SRS_NODE_DEVICE_HTTP_RECEIVER_16_027: [**If `message` is falsy, ` complete ` should throw a `ReferenceException`**]** 

**SRS_NODE_DEVICE_HTTP_RECEIVER_16_028: [**When successful, `complete` should call the `done` callback with a `null` error object and a result object of type `MessageCompleted`**]** 

### updateSharedAccessSignature(sharedAccessSignature)

**SRS_NODE_DEVICE_HTTP_RECEIVER_16_032: [** `updateSharedAccessSignature` shall throw a `ReferenceError` if the `sharedAccessSignature` argument is falsy. **]**

**SRS_NODE_DEVICE_HTTP_RECEIVER_16_033: [** All subsequent HTTP requests shall use the value of the `sharedAccessSignature` argument in their headers. **]**

### all HTTP requests

**SRS_NODE_DEVICE_HTTP_RECEIVER_16_031: [** If using a shared access signature for authentication, the following additional header should be used in the HTTP request: 
```
Authorization: <config.sharedAccessSignature>
``` 
**]**

**SRS_NODE_DEVICE_HTTP_RECEIVER_16_030: [** If using x509 authentication the `Authorization` header shall not be set and the x509 parameters shall instead be passed to the underlying transpoort. **]**
