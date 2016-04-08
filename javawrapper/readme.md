# Microsoft Azure IoT device SDK for java

The Microsoft Azure IoT device SDK for java facilitates building devices and applications that connect to and are managed by Azure IoT Hub services.
The javamodule is built as a wrapper around the native C SDK.

## Features

 * Sends event data to Azure IoT based services.
 * Maps server commands to device functions.
 * Batches messages to improve communication efficiency.
 * Supports pluggable transport protocols.

## Application development guidelines
For more information on how to use this library refer to the documents below:
- [Preparing your development environment][devbox-setup]
- [Running the java samples on Windows][run-sample-on-windows]
- [Running the java samples on Linux][run-sample-on-linux]

Other useful documents include:
- [Setup IoT Hub][setup-iothub]
- [How to use Device Explorer][device-explorer]

## Folder structure of repository

All the java wrapper device specific resources are located in the **javawrapper** folder.

### /build

This folder contains build scripts for the java wrapper client libraries on windows and linux.

### /device/iothub_client_javawrapper

This folder contain the java source code for the java wrapper module library.

This project is useful if you want to modify or extend the java wrapper module.

This folder contains scripts for building the library used by the wrapper.

### /samples

This folder contains various java samples that illustrate how to use the client library.

## API reference

API reference documentation can be found online at https://msdn.microsoft.com/library/microsoft.azure.devices.aspx.

