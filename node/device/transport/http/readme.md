#azure-iot-device-http
Communicate with Azure IoT Hub from any device over HTTP 1.1.

[![npm version](https://badge.fury.io/js/azure-iot-device-http.svg)](https://badge.fury.io/js/azure-iot-device-http)

## Install

`npm install -g azure-iot-device-http@latest` to get the latest (pre-release) version.

## Getting Started

Create a device client:

```js
var clientFromConnectionString = require('azure-iot-device-http').clientFromConnectionString;
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