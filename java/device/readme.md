# Microsoft Azure IoT device SDK for Java

The Microsoft Azure IoT device SDK for Java facilitates building devices and applications that connect and are managed by Azure IoT Suite services.

The device library consists of a set of reusable components with abstract interfaces that enable pluggability between stock and custom modules.

## Features

 * Sends event data to Azure IoT based services.
 * Receives messages from Azure IoT Hub.
 * Maps server commands to device functions.
 * Buffers data when network connection is down.
 * Batches messages to improve communication efficiency.
 * Supports pluggable transport protocols. HTTPS and AMQP protocols are available now.

## Application development guides
For more information on how to use this library refer to the documents below:
- [Setup your development environment][devbox-setup]
- [Setup IoT Hub](../../doc/setup_iothub.md)
- [Provision devices](../../c/doc/provision_device.md)
- [Using the DeviceExplorer application to tests IoT Hub device connectivity](../../tools/DeviceExplorer/doc/how_to_use_device_explorer.md)
- [Run the Java sample][run-java-sample]
- [How to build an IoTHub Java application from scratch on Windows][how-to-build-a-java-app-from-scratch]
- [Java API reference](http://azure.github.io/azure-iot-sdks/java/api_reference/index.html)


## Folder structure of repository

All of the Java specific resources are located in the **java** folder.

### /doc

This folder contains setup and getting started documents for Java.

### /iothub-java-client

This folder contains the client library for Java.

### /samples

This folder contains various Java samples that illustrate how to use the client library.

[devbox-setup]: doc/devbox_setup.md
[run-java-sample]: doc/run_sample_on_java.md
[how-to-build-a-java-app-from-scratch]: doc/how_to_build_a_java_app_from_scratch.md
