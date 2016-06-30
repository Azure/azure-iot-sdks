# Amqp Requirements (common)

## Overview
Amqp provides common transport functionalities for applications and devices that want to communicate with an Azure IoT Hub using the AMQP protocol.

## Example usage

```js
'use strict';
var Amqp = require('azure-iot-common').Amqp;

function print(err, res) {
  if (err) console.log(err.toString());
  if (res) console.log(res.statusCode + ' ' + res.statusMessage);
}

var deviceConfig = {
  host: 'hostname',
  keyName: 'deviceId',
  key: 'deviceKey'
};
var saslUri = 'amqps://user@sas.iothub:key@iothub.azure-devices.net';
var autoSettle = true;
var amqp = new Amqp (autoSettle, 'amqp version string');

var sendEndpoint = '/messages/devicebound';
var receiveEndpoint = '/messages/serviceBound/feedback;
var to = '/devices/deviceId/messages/devicebound';
amqp.connect(saslUri,null,, function (err) {
  // something in response to the error
});

amqp.send (new Message('hello world'), sendEndpoint, to, print);
amqp.getReceiver(receiveEndpoint, function (err, receiver) {
	// do something with the receiver (subscribe to events, etc)
});
```

## Public Interface

### Amqp constructor

**SRS_NODE_COMMON_AMQP_16_001: [**The Amqp constructor shall accept two parameters:
A Boolean indicating whether the client should automatically settle messages:
- True if the messages should be settled automatically
- False if the caller intends to manually settle messages
A string containing the version of the SDK used for telemetry purposes**]**

### connect(done)
Establishes a connection using the AMQP protocol with the IoT Hub instance.

**SRS_NODE_COMMON_AMQP_06_001: [** The connect method shall accept 3 parameters:
A uri that will be used for the connection.
A possibly undefined sslOptions structure.
A done callback.
 **]**

**SRS_NODE_COMMON_AMQP_06_002: [** The connect method shall throw a ReferenceError if the uri parameter has not been supplied.**]**

**SRS_NODE_COMMON_AMQP_16_002: [**The connect method shall establish a connection with the IoT hub instance and if given as argument call the `done` callback with a null error object in the case of success and a `results.Connected` object.**]**

**SRS_NODE_COMMON_AMQP_16_003: [**If given as an argument, the connect method shall call the `done` callback with a standard `Error` object if the connection fails.**]**


### disconnect(done)
Disconnects the application or device from the IoT Hub instance.

**SRS_NODE_COMMON_AMQP_16_004: [**The disconnect method shall call the `done` callback when the application/service has been successfully disconnected from the service**]**

**SRS_NODE_COMMON_AMQP_16_005: [**The disconnect method shall call the `done` callback and pass the error as a parameter if the disconnection is unsuccessful**]**

### send (message, endpoint, to, done)
Builds and sends an AMQP message with the body set to the message parameter to the IoT Hub service, using the endpoint and destination passed as arguments.

**SRS_NODE_COMMON_AMQP_16_006: [**The send method shall construct an AMQP message using information supplied by the caller, as follows:
- The `to` field of the message should be set to the `to` argument.
- The `body` of the message should be built using the message argument.**]**

**SRS_NODE_COMMON_AMQP_16_007: [**If send encounters an error before it can send the request, it shall invoke the `done` callback function and pass the standard JavaScript Error object with a text description of the error (err.message).**]**

### getReceiver(endpoint, done)
Configures an AmqpReceiver object to use the endpoint passed as a parameter and calls the `done` callback with the receiver instance as an argument.

**SRS_NODE_COMMON_AMQP_16_009: [**If a receiver for this endpoint has already been created, the getReceiver method should call the `done` method with the existing instance as an argument.**]**

**SRS_NODE_COMMON_AMQP_16_010: [**If a receiver for this endpoint doesn�t exist, the getReceiver method should create a new AmqpReceiver object and then call the `done` method with the object that was just created as an argument.**]**

