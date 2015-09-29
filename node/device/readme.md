# Microsoft Azure IoT device SDK for Node.js

The Microsoft Azure IoT device libraries for Node.js contain code that facilitates building devices and applications that connect to and are managed by Azure IoT Hub.

## Features

The IoT device SDK for Node.js libraries enable you to:
* Send event data to Azure IoT based services.
* Map server commands to device functions.
* Batch messages to improve communication efficiency.
* Support pluggable transport protocols. HTTPS protocol is available now.

## Directory structure of repository

All the Node.js specific resources are located in the **node** folder. This folder contains various script files to help you to [Prepare your development environment](doc/devbox_setup.md).

The **node** folder contains the following subfolders:

### /build

This folder contains various JavaScript build scripts.

### /common

This folder contains common library code used by both the device and service libraries.

### /device/doc

This folder contains application development guideline documents:
- [Prepare your development environment](doc/devbox_setup.md)
- [Setup IoT Hub](../../doc/setup_iot_hub.md)
- [Run the sample application](doc/run_sample.md)
- [How to use Device Explorer for IoT Hub devices](../../tools/DeviceExplorer/doc/how_to_use_device_explorer.md)

### /device/lib

This folder contains the device SDK library code.

### /device/samples

This folder contains various sample applications that illustrate how to use the device features of the Microsoft Azure IoT SDK for Node.js.

## API reference documentation

API reference documentation can be found online at http://azure.github.io/azure-iot-sdks/node/api_reference/index.html.

If you clone the repository to your local machine, you can also find a copy of the API reference documents here:

```
{IoT device SDK root}/node/device/doc/api_reference/index.html
```
