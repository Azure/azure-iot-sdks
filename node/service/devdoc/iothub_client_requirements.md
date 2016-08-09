#azure-iothub.Client Requirements

## Overview
`Client` exposes methods for sending cloud-to-device (c2d) messages, and receiving feedback regarding message delivery.

##Example usage
```js
'use strict';
var Client = require('azure-iothub').Client;
var Message = require('azure-iot-common').Message;

var message = new Message('hello');
message.messageId = 'unique-message-id';
message.ack = 'full';

var client = Client.fromConnectionString('Connection String');
client.open(function (err) {
  if (err) handleErrorAndExit(err);
  client.getFeedbackReceiver(function (err, receiver) {
    if (err) handleErrorAndExit(err);
    receiver.on('errorReceived', function (err) {
      handleErrorAndExit(err);
    });
    receiver.on('message', function (feedback) {
      console.log(feedback.body);
      client.close();
    });
  });
  client.send('my-device', message, function (err) {
    if (err) handleErrorAndExit(err);
  });
});
```

##Public Interface
###Client(transport) constructor
**SRS_NODE_IOTHUB_CLIENT_05_001: [**The `Client` constructor shall throw `ReferenceError` if the transport argument is falsy.**]** 

###fromConnectionString(connStr, Transport) [static]
The `fromConnectionString` static method returns a new instance of the `Client` object using the transport provided as a second argument, or the default (AMQP) transport if the second argument is null.

**SRS_NODE_IOTHUB_CLIENT_05_002: [**The `fromConnectionString` method shall throw `ReferenceError` if the `connStr` argument is falsy.**]**  

**SRS_NODE_IOTHUB_CLIENT_05_003: [**Otherwise, it shall derive and transform the needed parts from the connection string in order to create a new instance of the default transport (azure-iothub.Transport).**]**

**SRS_NODE_IOTHUB_CLIENT_05_004: [**The `fromConnectionString` method shall return a new instance of the Client object, as by a call to new Client(transport).**]**

###fromSharedAccessSignature(sharedAccessSignature, Transport) [static]
The `fromSharedAccessSignature` static method returns a new instance of the `Client` object using the default (AMQP) transport.

**SRS_NODE_IOTHUB_CLIENT_05_005: [**The `fromSharedAccessSignature` method shall throw `ReferenceError` if the sharedAccessSignature argument is falsy.**]**  

**SRS_NODE_IOTHUB_CLIENT_05_006: [**Otherwise, it shall derive and transform the needed parts from the shared access signature in order to create a new instance of the default transport (azure-iothub.Transport).**]**

**SRS_NODE_IOTHUB_CLIENT_05_007: [**The `fromSharedAccessSignature` method shall return a new instance of the `Client` object, as by a call to `new Client(transport)`.**]** 

###open(done)
The open method opens a connection to the IoT Hub service.

**SRS_NODE_IOTHUB_CLIENT_05_008: [**The `open` method shall open a connection to the IoT Hub that was identified when the `Client` object was created (e.g., in Client.fromConnectionString).**]**  

**SRS_NODE_IOTHUB_CLIENT_05_009: [**When the `open` method completes, the callback function (indicated by the `done` argument) shall be invoked with the following arguments:
- `err` - standard JavaScript `Error` object (or subclass)**]** 

**SRS_NODE_IOTHUB_CLIENT_05_010: [**The argument `err` passed to the callback `done` shall be null if the protocol operation was successful.**]**  

**SRS_NODE_IOTHUB_CLIENT_05_011: [**Otherwise the argument `err` shall have an `amqpError` property containing implementation-specific response information for use in logging and troubleshooting.**]**  

**SRS_NODE_IOTHUB_CLIENT_05_012: [**If the connection is already open when `open` is called, it shall have no effect—that is, the `done` callback shall be invoked immediately with a null argument.**]**

**SRS_NODE_IOTHUB_CLIENT_16_002: [** If the transport successfully establishes a connection the `open` method shall subscribe to the `disconnect` event of the transport.**]**

###send(devceId, message, done)
The `send` method sends a cloud-to-device message to the service, intended for delivery to the given device.

**SRS_NODE_IOTHUB_CLIENT_05_013: [**The `send` method shall throw `ReferenceError` if the deviceId or message arguments are falsy.**]**

**SRS_NODE_IOTHUB_CLIENT_05_014: [**The `send` method shall convert the message object to type azure-iot-common.Message if necessary.**]**

**SRS_NODE_IOTHUB_CLIENT_05_016: [**When the `send` method completes, the callback function (indicated by the done - argument) shall be invoked with the following arguments:
- `err` - standard JavaScript Error object (or subclass)
- `response` - an implementation-specific response object returned by the underlying protocol, useful for logging and troubleshooting**]** 

**SRS_NODE_IOTHUB_CLIENT_05_017: [**The argument `err` passed to the callback `done` shall be null if the protocol operation was successful.**]**  

**SRS_NODE_IOTHUB_CLIENT_05_018: [**Otherwise the argument `err` shall have an `amqpError` property containing implementation-specific response information for use in logging and troubleshooting.**]**  

**SRS_NODE_IOTHUB_CLIENT_05_019: [**If the `deviceId` has not been registered with the IoT Hub, `send` shall call the `done` callback with a `DeviceNotFoundError`.**]**  

**SRS_NODE_IOTHUB_CLIENT_05_020: [**If the queue which receives messages on behalf of the device is full, `send` shall call the `done` callback with a `DeviceMaximumQueueDepthExceededError`.**]** 

###getFeedbackReceiver(done)
The `getFeedbackReceiver` method is used to obtain an `AmqpReceiver` object which emits events when new feedback messages are received by the client.

**SRS_NODE_IOTHUB_CLIENT_05_027: [**When the `getFeedbackReceiver` method completes, the callback function (indicated by the `done` argument) shall be invoked with the following arguments:
- `err` - standard JavaScript `Error` object (or subclass): `null` if the operation was successful
- `receiver` - an `AmqpReceiver` instance: `undefined` if the operation failed **]**

###getFileNotificationReceiver(done)
The `getFileNotificationReceiver` method is used to obtain an `AmqpReceiver` object which emits events when new file notifications are received by the client.

**SRS_NODE_IOTHUB_CLIENT_16_001: [** When the `getFileNotificationReceiver` method completes, the callback function (indicated by the `done` argument) shall be invoked with the following arguments:
- `err` - standard JavaScript `Error` object (or subclass): `null` if the operation was successful
- `receiver` - an `AmqpReceiver` instance: `undefined` if the operation failed **]**

###close(done)
The `close` method closes the connection opened by open.

**SRS_NODE_IOTHUB_CLIENT_05_021: [**The `close` method shall close the connection.**]**  

**SRS_NODE_IOTHUB_CLIENT_05_022: [**When the `close` method completes, the callback function (indicated by the done argument) shall be invoked with the following arguments:
- `err` - standard JavaScript `Error` object (or subclass)**]**  

**SRS_NODE_IOTHUB_CLIENT_05_023: [**The argument `err` passed to the callback `done` shall be `null` if the protocol operation was successful.**]** 

**SRS_NODE_IOTHUB_CLIENT_05_024: [**Otherwise the argument `err` shall have a transport property containing implementation-specific response information for use in logging and troubleshooting.**]**  

**SRS_NODE_IOTHUB_CLIENT_05_025: [**If the connection is not open when close is called, it shall have no effect— that is, the `done` callback shall be invoked immediately with `null` arguments.**]**

**SRS_NODE_IOTHUB_CLIENT_16_003: [** The `close` method shall remove the listener that has been attached to the transport `disconnect` event. **]**

### Events
#### disconnect
**SRS_NODE_IOTHUB_CLIENT_16_004: [** The `disconnect` event shall be emitted when the client is disconnected from the server. **]**