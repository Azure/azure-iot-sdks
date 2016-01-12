# Microsoft Azure IoT device SDK for Node.js

The Microsoft Azure IoT device libraries for Node.js contain code that facilitates building devices and applications that connect to and are managed by Azure IoT Hub.

## Features

The IoT device SDK for Node.js libraries enable you to:
* Send event data to Azure IoT based services.
* Map server commands to device functions.
* Batch messages to improve communication efficiency.
* Support pluggable transport protocols. HTTPS protocol is available now.

## Application development guides
For more information on how to use this library refer to the documents below:
- [Prepare your node.js development environment](doc/devbox_setup.md)
- [Setup IoT Hub](../../doc/setup_iothub.md)
- [Provision devices](../../tools/iothub-explorer/doc/provision_device.md)
- [Using the DeviceExplorer application to test IoT Hub device connectivity](../../tools/DeviceExplorer/doc/how_to_use_device_explorer.md)
- [Run the node.js sample application](doc/run_sample.md)
- [Node API reference](http://azure.github.io/azure-iot-sdks/node/api_reference/azure-iot-device/1.0.0-preview.9/index.html)

## Directory structure of repository

All the Node.js specific resources are located in the **node** folder. This folder contains various script files to help you to prepare your development environment. The **node** folder contains the following subfolders:

### /build

This folder contains various JavaScript build scripts.

### /common

This folder contains common library code used by both the device and service libraries.

### /device/doc

This folder contains application development guideline documents:


### /device/lib

This folder contains the device SDK library code.

### /device/samples

This folder contains various sample applications that illustrate how to use the device features of the Microsoft Azure IoT SDK for Node.js.
