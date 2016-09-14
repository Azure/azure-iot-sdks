# Microsoft Azure IoT device SDK for Node.js

Build devices that communicate with Azure IoT Hub.

## Features

Use the device SDK to:
* Send event data to Azure IoT Hub.
* Receive messages from IoT Hub.
* Communicate with the service via AMQP (optionally over WebSockets), MQTT, or HTTP.

## Application development guides
For more information on how to use this library refer to the documents below:
- [Prepare your node.js development environment](../../doc/get_started/node-devbox-setup.md)
- [Setup IoT Hub](../../doc/setup_iothub.md)
- [Provision devices](../../doc/manage_iot_hub.md)
- [Run a node.js sample application](../../doc/get_started/node-run-sample.md)
- [Node API reference](http://azure.github.io/azure-iot-sdks/node/api_reference/azure-iot-device/1.0.14/index.html)
- [Debugging with Visual Studio Code](../../doc/get_started/node-debug-vscode.md)

## Directory structure

Device SDK subfolders under **node/device**:

### /core

Protocol-independent device SDK package.

### /doc

Node device specific documentation.

### /node-red

Node-RED module for Azure IoT Hub.

### /samples

Sample applications exercising basic features.

### /transport

Protocol-specific SDK packages for: AMQP, AMQP over WebSockets, MQTT, and HTTP.
