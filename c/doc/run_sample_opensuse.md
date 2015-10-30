# openSUSE Setup

The following document describes the process of connecting an openSUSE system to Azure IoT Hub.

## Table of Contents

- [Requirements](#requirements)
- [Before you begin](#beforebegin)
- [Setup the development environment](#setup)
- [Build and run the sample](#buildrunapp)

<a name="requirements"/>
## Requirements

  - Computer with a Git client installed so that you can access the azure-iot-sdks code on GitHub.

<a name="beforebegin"/>
## Before you begin

Before you begin you will need to create and configure an IoT hub to connect to.
  1. [Set up your IoT Hub][setup-iothub].
  1. With your IoT hub configured and running in Azure, follow the instructions in [Connecting your device to an IoT hub][provision-device].
  1. Make note of the Connection String for your device from the previous step.

  > Note: You can skip this step if you just want to build the sample application without running it.

<a name="setup"/>
## Setup the development environment

This section shows you how to set up a development environment for the Azure IoT device SDK for C on openSUSE.

1. Clone this repository ([azure-iot-sdks](https://github.com/Azure/azure-iot-sdks)) to the machine you are using.
2. Open a shell and navigate to the folder **c/build_all/linux** in your local copy of the repository.

3. Run the `setup_opensuse.sh` script to install the prerequisite packages and the dependent libraries.

4. Run the `build.sh` script.

This script builds the **iothub_client** and **serializer** libraries and their associated samples in a folder called "cmake" inside you home folder.

 > Note: you will not be able to run the samples until you configure them with a valid IoT Hub device connection string. For more information, see [Run sample on Linux](run_sample_on_desktop_linux.md).

 <a name="buildrunapp"/>
## Build and run the sample

1. Open the file **c/serializer/samples/simplesample_amqp/simplesample_amqp.c** in a text editor.

2. Locate the following code in the file:
    ```
   static const char* connectionString = "[device connection string]";
    ```
3. Replace "[device connection string]" with the device connection string you noted [earlier](#beforebegin). Save the changes.

4. The section "Send events" in the document [How to use Device Explorer](../../tools/DeviceExplorer/doc/how_to_use_device_explorer.md) describes how to prepare the **DeviceExplorer** tool to receive device-to-cloud messages from the sample application.

5. Save your changes and build the samples. To build your sample you can run the the build.sh script in the **c/build_all/linux** directory.

6. Run the **~/cmake/serializer/samples/simplesample_amqp/simplesample_amqp** sample application.

7. Use the **DeviceExplorer** utility to observe the messages IoT Hub receives from the **simplesample_amqp** application.

8. See "Send cloud-to-device messages" in the document [How to use Device Explorer for IoT Hub devices][device-explorer] for instructions on sending messages with the **DeviceExplorer** utility.

[setup-devbox-linux]: devbox_setup.md
[device-explorer]: ../../tools/DeviceExplorer/doc/how_to_use_device_explorer.md
[setup-iothub]: ../../doc/setup_iothub.md
[provision-device]: ../../tools/iothub-explorer/doc/provision_device.md
