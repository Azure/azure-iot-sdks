# azure-iot-device.Client Requirements

## Overview
azure-iot-device.Client provides a means for devices to send events to and receive messages from an Azure IoT Hub.  The client handles communication with IoT Hub through a transport supplied by the caller (e.g., azure-iot-device-http.Http).

## Example usage
```js
'use strict';
var Client = require('azure-iot-device-http').Client;
var Http = require('azure-iot-device-http').Http;

function print(err, res) {
  if (err) console.log(err.toString());
  if (res) console.log(res.statusCode + ' ' + res.statusMessage);
}

var config = {
  host: '<hostname>',
  deviceId: '<device-id>',
  sharedAccessSignature: '<shared-access-signature>'
};

var client = new Client(new Http(config));

client.sendEvent(new Message('hello world'), print);
```

## Public Interface

### Factory methods
#### fromConnectionString
**SRS_NODE_DEVICE_CLIENT_05_003: [** The `fromConnectionString` method shall throw ReferenceError if the connStr argument is falsy. **]**

**SRS_NODE_DEVICE_CLIENT_05_005: [** `fromConnectionString` shall derive and transform the needed parts from the connection string in order to create a new instance of Transport. **]**

**SRS_NODE_DEVICE_CLIENT_05_006: [** The `fromConnectionString` method shall return a new instance of the `Client` object, as by a call to `new Client(new Transport(...))`. **]**

#### fromSharedAccessSignature

**SRS_NODE_DEVICE_CLIENT_16_029: [** The `fromSharedAccessSignature` method shall throw a `ReferenceError` if the sharedAccessSignature argument is falsy. **]**

**SRS_NODE_DEVICE_CLIENT_16_030: [** The `fromSharedAccessSignature` method shall return a new instance of the `Client` object **]**

### Constructors
#### Client(transport, connectionString) constructor

**SRS_NODE_DEVICE_CLIENT_05_001: [** The `Client` constructor shall accept a transport object **]**  , e.g. `azure-iot-device-http.Http`.

**SRS_NODE_DEVICE_CLIENT_16_026: [** The `Client` constructor shall accept a connection string as an optional second argument **]**

**SRS_NODE_DEVICE_CLIENT_16_027: [** If a connection string argument is provided and is using SharedAccessKey authentication, the `Client` shall automatically generate and renew SAS tokens. **]**

### Public Methods

#### open(done)

**SRS_NODE_DEVICE_CLIENT_12_001: [** The `open` function shall call the transport's `connect` function, if it exists. **]**

**SRS_NODE_DEVICE_CLIENT_16_045: [** If the transport successfully establishes a connection the `open` method shall subscribe to the `disconnect` event of the transport. **]**

**SRS_NODE_DEVICE_CLIENT_16_020: [** The `open` function should start listening for C2D messages if there are listeners on the `message` event **]**

#### close(done)
**SRS_NODE_DEVICE_CLIENT_16_001: [** The `close` function shall call the transport's `disconnect` function if it exists. **]**

**SRS_NODE_DEVICE_CLIENT_16_046: [** The `close` method shall remove the listener that has been attached to the transport `disconnect` event. **]**

#### sendEvent(message, done)
The `sendEvent` method sends an event message to the IoT Hub as the device indicated in the constructor argument.

**SRS_NODE_DEVICE_CLIENT_05_002: [** The `sendEvent` method shall send the event (indicated by the message argument) via the transport associated with the Client instance. **]**

**SRS_NODE_DEVICE_CLIENT_05_003: [** When the `sendEvent` method completes, the callback function (indicated by the done argument) shall be invoked with the same arguments as the underlying transport method's callback. **]**

#### sendEventBatch(messages, done)
The `sendEventBatch` method sends a list of event messages to the IoT Hub as the device indicated in the constructor argument.

**SRS_NODE_DEVICE_CLIENT_07_004: [** The `sendEventBatch` method shall send the list of events (indicated by the messages argument) via the transport associated with the Client instance. **]**

**SRS_NODE_DEVICE_CLIENT_07_005: [** When the `sendEventBatch` method completes the callback function shall be invoked with the same arguments as the underlying transport method's callback. **]**

#### setTransportOptions(options, done)
**`setTransportOptions` is deprecated and will be removed at the next major release.**

**SRS_NODE_DEVICE_CLIENT_16_024: [** The `setTransportOptions` method shall throw a `ReferenceError` if the options object is falsy **]**

**SRS_NODE_DEVICE_CLIENT_16_025: [** The `setTransportOptions` method shall throw a `NotImplementedError` if the transport doesn't implement a 'setOption' method. **]**

**SRS_NODE_DEVICE_CLIENT_16_021: [** The `setTransportOptions` method shall call the `setOptions` method on the transport object. **]**

**SRS_NODE_DEVICE_CLIENT_16_022: [** The `done` callback shall be invoked with a `null` error object and a `TransportConfigured` object once the transport has been configured. **]**

**SRS_NODE_DEVICE_CLIENT_16_023: [** The `done` callback shall be invoked with a standard javascript `Error` object and no result object if the transport could not be configured as requested. **]**

#### setOptions(options, done)
`setOptions` is used to configure the client.

**SRS_NODE_DEVICE_CLIENT_16_042: [** The `setOptions` method shall throw a `ReferenceError` if the options object is falsy. **]**

**SRS_NODE_DEVICE_CLIENT_16_043: [** The `done` callback shall be invoked with no parameters when it has successfully finished setting the client and/or transport options. **]**

**SRS_NODE_DEVICE_CLIENT_16_044: [** The `done` callback shall be invoked with a standard javascript `Error` object and no result object if the client could not be configured as requested. **]**

#### complete(message, done)

**SRS_NODE_DEVICE_CLIENT_16_016: [** The `complete` method shall throw a `ReferenceError` if the `message` parameter is falsy. **]**

**SRS_NODE_DEVICE_CLIENT_16_007: [** The `complete` method shall call the `complete` method of the transport with the message as an argument **]**

**SRS_NODE_DEVICE_CLIENT_16_008: [** The `done` callback shall be called with a `null` error object and a `MessageCompleted` result once the transport has completed the message. **]**

**SRS_NODE_DEVICE_CLIENT_16_009: [** The `done` callback shall be called with a standard javascript `Error` object and no result object if the transport could not complete the message. **]**

#### reject(message, done)

**SRS_NODE_DEVICE_CLIENT_16_018: [** The `reject` method shall throw a ReferenceError if the `message` parameter is falsy. **]**

**SRS_NODE_DEVICE_CLIENT_16_010: [** The `reject` method shall call the `reject` method of the transport with the message as an argument **]**

**SRS_NODE_DEVICE_CLIENT_16_011: [** The `done` callback shall be called with a `null` error object and a `MessageRejected` result once the transport has completed the message. **]**

**SRS_NODE_DEVICE_CLIENT_16_012: [** The `done` callback shall be called with a standard javascript `Error` object and no result object if the transport could not reject the message. **]**

#### abandon(message, done)

**SRS_NODE_DEVICE_CLIENT_16_017: [** The `abandon` method shall throw a ReferenceError if the `message` parameter is falsy. **]**

**SRS_NODE_DEVICE_CLIENT_16_013: [** The `abandon` method shall call the `abandon` method of the transport with the message as an argument **]**

**SRS_NODE_DEVICE_CLIENT_16_014: [** The `done` callback shall be called with a `null` error object and a `MessageAbandoned` result once the transport has completed the message. **]**

**SRS_NODE_DEVICE_CLIENT_16_015: [** The `done` callback shall be called with a standard javascript `Error` object and no result object if the transport could not abandon the message. **]**

#### updateSharedAccessSignature(sharedAccessSignature, done)

**SRS_NODE_DEVICE_CLIENT_16_031: [** The `updateSharedAccessSignature` method shall throw a `ReferenceError` if the sharedAccessSignature parameter is falsy. **]**

**SRS_NODE_DEVICE_CLIENT_16_032: [** The `updateSharedAccessSignature` method shall call the `updateSharedAccessSignature` method of the transport currently in use with the sharedAccessSignature parameter. **]**

**SRS_NODE_DEVICE_CLIENT_16_033: [** The `updateSharedAccessSignature` method shall reconnect the transport to the IoTHub service if it was connected before before the method is called. **]**

**SRS_NODE_DEVICE_CLIENT_16_034: [** The `updateSharedAccessSignature` method shall not reconnect the transport if the transport was disconnected to begin with. **]**

**SRS_NODE_DEVICE_CLIENT_16_035: [** The `updateSharedAccessSignature` method shall call the `done` callback with an error object if an error happened while renewing the token. **]**

**SRS_NODE_DEVICE_CLIENT_16_036: [** The `updateSharedAccessSignature` method shall call the `done` callback with a `null` error object and a result of type SharedAccessSignatureUpdated if the token was updated successfully. **]**

#### uploadToBlob(blobName, stream, done)

**SRS_NODE_DEVICE_CLIENT_16_037: [** The `uploadToBlob` method shall throw a `ReferenceError` if `blobName` is falsy. **]**

**SRS_NODE_DEVICE_CLIENT_16_038: [** The `uploadToBlob` method shall throw a `ReferenceError` if `stream` is falsy. **]**

**SRS_NODE_DEVICE_CLIENT_16_039: [** The `uploadToBlob` method shall throw a `ReferenceError` if `streamLength` is falsy. **]**

**SRS_NODE_DEVICE_CLIENT_16_040: [** The `uploadToBlob` method shall call the `done` callback with an `Error` object if the upload fails. **]**

**SRS_NODE_DEVICE_CLIENT_16_041: [** The `uploadToBlob` method shall call the `done` callback no parameters if the upload succeeds. **]**

### Events
#### message

**SRS_NODE_DEVICE_CLIENT_16_002: [** The `message` event shall be emitted when a cloud-to-device message is received from the IoT Hub service. **]**

**SRS_NODE_DEVICE_CLIENT_16_003: [** The `message` event parameter shall be a `message` object. **]**

**SRS_NODE_DEVICE_CLIENT_16_004: [** The client shall start listening for messages from the service whenever there is a listener subscribed to the `message` event. **]**

**SRS_NODE_DEVICE_CLIENT_16_005: [** The client shall stop listening for messages from the service whenever the last listener unsubscribes from the `message` event. **]**

#### error

**SRS_NODE_DEVICE_CLIENT_16_006: [** The `error` event shall be emitted when an error occurred within the client code. **]**

#### disconnect

**SRS_NODE_DEVICE_CLIENT_16_019: [** The `disconnect` event shall be emitted when the client is disconnected from the server. **]**

