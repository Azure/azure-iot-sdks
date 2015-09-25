# Microsoft Azure IoT service SDK for Node.js

The Microsoft Azure IoT Service libraries for Node.js contain code to manage your IoT Hub service.

## Features

Service libraries
* A service client you can use to send cloud-to-device commands from IoT Hub to your devices.
* A registry manager that you can use to manage the contents of the IoT Hub device identity registry.
* Supports pluggable transport protocols. HTTP protocol is available now, with more coming soon.

## Directory structure of repository

All the Node.js specific resources are located in the **node** folder.

The **node** folder contains the following subfolders:

### /build

This folder contains various JavaScript build scripts.

### /common

This folder contains common library code used by both the device and service libraries.

### /device/lib

This folder contains the device SDK library code.

### /device/samples

This folder contains various sample applications that illustrate how to use the service features of the Microsoft Azure IoT SDK for Node.js.

## API reference

API documentation can be found here:

```
{IoT device SDK root}/node/service/doc/api_reference/index.html
```
