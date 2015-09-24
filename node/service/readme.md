# Microsoft Azure IoT service SDK for Node.js

The Microsoft Azure IoT Service libraries for Node.js contain code to manage your IoT Hub service.

## Features

Service libraries
* A service client you can use to send cloud-to-device commands from IoT Hub to your devices.
* A registry manager that you can use to manage the contents of the IoT Hub device identity registry.
* Supports pluggable transport protocols. HTTP protocol is available now, with more coming soon.

## Directory structure of repository

All the Node.js specific resources are located in the **node** folder. This folder contains various script files to help you to [Prepare your development environment](doc/devbox_setup.md).

The **node** folder contains the following subfolders:

### /build

This folder contains various JavaScript build scripts used by **Jakefile.js**.

### /doc

This folder contains application development guideline documents:
- [Prepare your development environment](doc/devbox_setup.md)
- [Setup IoT Hub](../../doc/setup_iot_hub.md)
- [Run the sample application](doc/run_sample.md)
- [How to use Device Explorer for IoT Hub devices](../../tools/DeviceExplorer/doc/how_to_use_device_explorer.md)

### /src

The source folder contains the following folders:

- **adapters**: adapter implementations
- **common**: shared code
- **device**: libraries for implementing IoT client devices
- **service**: libraries for implementing IoT Hub management functionality

### /samples

This folder contains various sample applications that illustrate how to use the device and service features of the Microsoft Azure IoT SDK for Node.js.

## API reference

API documentation can be found here:

```
{IoT device SDK root}/node/service/doc/api_reference/index.html
```
