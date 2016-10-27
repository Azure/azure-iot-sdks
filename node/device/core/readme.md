#azure-iot-device
The core components of the Azure IoT device SDK.

[![npm version](https://badge.fury.io/js/azure-iot-device.svg)](https://badge.fury.io/js/azure-iot-device)

## Prerequisites
You need to install **Node.js** JavaScript runtime environment for Azure IoT JavaScript client SDK to run on your platform. For downloading Node.js refer to [Node.js][nodejs_lnk]. To check if your platform (OS) is **supported** check if install package is available at [Node.js][nodejs_dwld_lnk].
[npm][npm_lnk] is a package manager command line tool that will be included when Node.js is installed. This is the tool that you will use to install Azure IoT node.js client side SDK.


## Install

`npm install -g azure-iot-device@latest` to get the latest (pre-release) version.

## Getting Started

This package contains the core components of the Azure IoT device SDK, but is lacking a transport over which to send events and receive messages. Your application must require a transport package in addition to the core package to do something useful.

For example, if you want to send an event from your device to an IoT Hub _using the AMQP protocol_ you must first install the **azure-iot-device-amqp** package:

```
npm install -g azure-iot-device-amqp@latest
```

Then you can use the code below to send a message to IoT Hub.

Note that for this sample to work, you will need to [setup your IoT hub][lnk-setup-iot-hub] and [provision your device and get its credentials][lnk-manage-iot-hub]. In the code, replace '[IoT Hub device connection string]' with the device credentials created in the IoT Hub.

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

[nodejs_lnk]: https://nodejs.org/
[nodejs_dwld_lnk]: https://nodejs.org/en/download/
[npm_lnk]:https://docs.npmjs.com/getting-started/what-is-npm
[lnk-setup-iot-hub]: ../../../doc/setup_iothub.md
[lnk-manage-iot-hub]: ../../../doc/manage_iot_hub.md