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

Send an event:

```js
var msg = new Message('some data from my device');
client.sendEvent(message, function (err) {
  if (err) console.log(err.toString());
});
```

Receive a message from IoT Hub:

```js
client.receive(function (err, msg) {
  if (err) console.error(err);
  else console.log(msg);
});
```