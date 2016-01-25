# Microsoft Azure IoT device SDK for Node.js

Build devices that communicate with Azure IoT Hub.

## Features

Use the device SDK to:
* Send event data to Azure IoT Hub.
* Receive messages from IoT Hub.
* Communicate with the service via AMQP (optionally over WebSockets), MQTT, or HTTP.

## Application development guides
For more information on how to use this library refer to the documents below:
- [Prepare your node.js development environment](doc/devbox_setup.md)
- [Setup IoT Hub](../../doc/setup_iothub.md)
- [Provision devices](../../tools/iothub-explorer/doc/provision_device.md)
- [Run a node.js sample application](doc/run_sample.md)
- [Node API reference](http://azure.github.io/azure-iot-sdks/node/api_reference/azure-iot-device/1.0.0-preview.10/index.html)

## Directory structure

Device SDK subfolders under **node/device**:

### /core

Protocol-independent device SDK package.

### /doc

How-to guides and other information.

### /samples

Sample applications excercising basic features.

### /transport

Protocol-specific SDK packages for: AMQP, AMQP over WebSockets, MQTT, and HTTP.