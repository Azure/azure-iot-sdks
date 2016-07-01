#azure-iot-device
The core components of the Azure IoT device SDK.

[![npm version](https://badge.fury.io/js/azure-iot-device.svg)](https://badge.fury.io/js/azure-iot-device)

## Install

`npm install -g azure-iot-device@latest` to get the latest (pre-release) version.

## Getting Started

This package contains the core components of the Azure IoT device SDK, but is lacking a transport over which to send events and receive messages. Your application must require a transport package in addition to the core package to do something useful.

For example, if you want to send an event from your device to an IoT Hub _using the AMQP protocol_ you must first install the **azure-iot-device-amqp** package:

```
npm install -g azure-iot-device-amqp@latest
```

Then you can use the following code to send a message to IoT Hub:

```js
var connectionString = '[IoT Hub device connection string]';

// use factory function from AMQP-specific package
var clientFromConnectionString = require('azure-iot-device-amqp').clientFromConnectionString;

// AMQP-specific factory function returns Client object from core package
var client = clientFromConnectionString(connectionString);

// use Message object from core package
var Message = require('azure-iot-device').Message;

var connectCallback = function (err) {
  if (err) {
    console.error('Could not connect: ' + err);
  } else {
    console.log('Client connected');
    var msg = new Message('some data from my device');
    client.sendEvent(msg, function (err) {
      if (err) {
        console.log(err.toString());
      } else {
        console.log('Message sent');
      };
    });
  };
};


client.open(connectCallback);
```

See the `azure-iot-device-*` transport-specific packages for more information.