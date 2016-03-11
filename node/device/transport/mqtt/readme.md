#azure-iot-device-mqtt
Communicate with Azure IoT Hub from any device over MQTT.

[![npm version](https://badge.fury.io/js/azure-iot-device-mqtt.svg)](https://badge.fury.io/js/azure-iot-device-mqtt)

## Install

`npm install -g azure-iot-device-mqtt@latest` to get the latest (pre-release) version.

## Getting Started

Create a device client:

```js
var clientFromConnectionString = require('azure-iot-device-mqtt').clientFromConnectionString;
var Message = require('azure-iot-device').Message;

var connectionString = '[IoT Hub device connection string]';

var client = clientFromConnectionString(connectionString);
```

Create a callback that sends a message and receives messages. When it receives a message it sends an acknowledgement receipt to the server:

```js
var connectCallback = function (err) {
  if (err) {
    console.error('Could not connect: ' + err);
  } else {
    console.log('Client connected');
    var message = new Message('some data from my device');
    client.sendEvent(message, function (err) {
      if (err) console.log(err.toString());
    });

    client.on('message', function (msg) { 
      console.log(msg); 
      client.complete(msg, function () {
        console.log('completed');
      });
    }); 
  }
};
```

Open the connection and invoke the callback:

```js
client.open(connectCallback);
```