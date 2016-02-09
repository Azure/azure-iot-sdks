#azure-iot-device-amqp
Communicate with Azure IoT Hub from any device over AMQP.

[![npm version](https://badge.fury.io/js/azure-iot-device-amqp.svg)](https://badge.fury.io/js/azure-iot-device-amqp)

## Install

`npm install -g azure-iot-device-amqp@latest` to get the latest (pre-release) version.

## Getting Started

Create a device client:

```js
var clientFromConnectionString = require('azure-iot-device-amqp').clientFromConnectionString;
var Message = require('azure-iot-device').Message;

var connectionString = '[IoT Hub device connection string]';

var client = clientFromConnectionString(connectionString);
```

Send an event:

```js
var msg = new Message('some data from my device');
client.sendEvent(message, function (err) {
  if (err) console.log(err.toString());
});
```

Receive a message, then acknowledge receipt to the server:

```js
client.getReceiver(function (err, rcv) {
  rcv.on('message', function (msg) {
    console.log(msg);
    rcv.complete(msg, function () {
      console.log('completed');
    });
  });
  rcv.on('errorReceived', function (err) {
    console.warn(err);
  });
});
```