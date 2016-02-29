# azure-iot-device.Http Requirements

## Overview
Http provides HTTP protocol support to the device SDK for communicating with Azure IoT Hub.

```js
'use strict';
var Http = require('azure-iot-device-http').Http;
var Message = require('azure-iot-device-http').Message;

function print(err) {
  console.log(err.toString());
}

var config = {
  host: /* ... */,
  deviceId: /* ... */,
  sharedAccessSignature: /* ... */
};

var http = new Http(config);
var message = new Message('my message');
http.sendEvent(message, function (err, msg) {
  if (err) print(err);
  else console.log('sent message: ' + msg.getData());
});

http.getReceiver(function (receiver) {
  receiver.on('message', function(msg) {
    console.log('received: ' + msg.getData());
    receiver.complete(msg);
  });
  receiver.on('errorReceived', function(err) {
    console.log(Error: ' + err.message);
  });
});
```

## Public Interface

### Http constructor

**SRS_NODE_DEVICE_HTTP_05_001: [**The Http constructor shall accept an object with three properties:
- `host` - (string) the fully-qualified DNS hostname of an IoT hub
- `deviceId` - (string) the name of the IoT hub, which is the first segment of hostname
- `sharedAccessSignature` - (string) a shared access signature generated from the credentials of a policy with the "device connect" permissions.**]**

### sendEvent(message, done)

The `sendEvent` method sends an event to an IoT hub on behalf of the device indicated in the constructor argument.

**SRS_NODE_DEVICE_HTTP_05_002: [**The `sendEvent` method shall construct an HTTP request using information supplied by the caller, as follows:
```
POST <config.host>/devices/<config.deviceId>/messages/events?api-version=<version> HTTP/1.1
Authorization: <config.sharedAccessSignature>
iothub-to: /devices/<config.deviceId>/messages/events
User-Agent: <version string>
Host: <config.host>

<message>
```
**]**

### sendEventBatch(messages, done)

The sendEventBatch method sends a list of events to an IoT hub on behalf of the device indicated in the constructor argument.

**SRS_NODE_DEVICE_HTTP_05_003: [**The `sendEventBatch` method shall construct an HTTP request using information supplied by the caller, as follows:
```
POST <config.host>/devices/<config.deviceId>/messages/events?api-version=<version> HTTP/1.1
Authorization: <config.sharedAccessSignature>
iothub-to: /devices/<config.deviceId>/messages/events
User-Agent: <version string>
Content-Type: application/vnd.microsoft.iothub.json
Host: <config.host>

{"body":"<Base64 Message1>","properties":{"<key>":"<value>"}},
{"body":"<Base64 Message1>"}...
```
**]**

### getReceiver(done)

the getReceiver method queries the client for an `HttpReceiver` object that can be used to receive messages and settle them.

**SRS_NODE_DEVICE_HTTP_16_005: [**The `getReceiver` method shall call the `done` method passed as argument with the receiver object as a parameter**]**
**SRS_NODE_DEVICE_HTTP_16_006: [**The `getReceiver` method shall return the same unique instance if called multiple times in a row**]**

### setOptions(options, done)

**SRS_NODE_DEVICE_HTTP_16_004: [**The `setOptions` method shall call the `setOptions` method of the HTTP Receiver with the options parameter.**]**
**SRS_NODE_DEVICE_HTTP_16_005: [**The `setOptions` method shall call the `done` callback when finished.**]**

### abandon(message, done)

**SRS_NODE_DEVICE_HTTP_16_001: [**The ‘`abandon’` method shall call the ‘`abandon’` method of the receiver object and pass it the message and the callback given as parameters.**]**

### complete(message, done)

**SRS_NODE_DEVICE_HTTP_16_002: [**The ‘`complete’` method shall call the ‘`complete’` method of the receiver object and pass it the message and the callback given as parameters.**]**

### reject(message, done)

**SRS_NODE_DEVICE_HTTP_16_003: [**The ‘`reject’` method shall call the ‘`reject’` method of the receiver object and pass it the message and the callback given as parameters.**]**

### updateSharedAccessSignature(sharedAccessSignature, done)

**SRS_NODE_DEVICE_HTTP_16_006: [**The `updateSharedAccessSignature` method shall save the new shared access signature given as a parameter to its configuration.**]**

**SRS_NODE_DEVICE_HTTP_16_007: [**The `updateSharedAccessSignature` method shall call the `done` callback with a null error object and a SharedAccessSignatureUpdated object as a result, indicating that the client does not need to reestablish the transport connection.**]**

**SRS_NODE_DEVICE_HTTP_16_008: [**The `updateSharedAccessSignature` method shall call the `updateSharedAccessSignature` method of the current receiver object if it exists.**]**

### All HTTP requests

**SRS_NODE_DEVICE_HTTP_05_008: [**If any Http method encounters an error before it can send the request, it shall invoke the `done` callback function and pass the standard JavaScript Error object with a text description of the error (err.message).**]**

**SRS_NODE_DEVICE_HTTP_05_009: [**When any Http method receives an HTTP response with a status code >= 300, it shall invoke the `done` callback function with the following arguments:
err - the standard JavaScript Error object, with the Node.js http.ServerResponse object attached as the property response**]**

**SRS_NODE_DEVICE_HTTP_05_010: [**When any Http method receives an HTTP response with a status code < 300, it shall invoke the `done` callback function with the following arguments:
- `err` - null
- `body` - the body of the HTTP response
- `response` - the Node.js http.ServerResponse object returned by the transport**]**