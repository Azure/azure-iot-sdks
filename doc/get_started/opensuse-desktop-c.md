---
platform: opensuse
device: desktop
language: c
---

Run a simple C sample on device running openSUSE
===
---

# Table of Contents

-   [Introduction](#Introduction)
-   [Step 1: Prerequisites](#Step-1-Prerequisites)
-   [Step 2: Prepare your Device](#Step-2-PrepareDevice)
-   [Step 3: Build and Run the Sample](#Step-3-Build)

<a name="Introduction"></a>
# Introduction

**About this document**

This document describes how to connect devices running openSUSE with Azure IoT SDK. This multi-step process includes:
-   Configuring Azure IoT Hub
-   Registering your IoT device
-   Build and deploy Azure IoT SDK on device

<a name="Step-1-Prerequisites"></a>
# Step 1: Prerequisites

You should have the following items ready before beginning the process:

-   Computer with Git client installed and access to the
    [azure-iot-sdks](https://github.com/Azure/azure-iot-sdks) GitHub public repository.
-   Download and install [DeviceExplorer](https://github.com/Azure/azure-iot-sdks/releases/download/2015-11-13/SetupDeviceExplorer.msi).
-   [Set up your IoT hub](https://github.com/Azure/azure-iot-sdks/blob/master/doc/setup_iothub.md).

### Create a device on IoT Hub
-   With your IoT hub configured and running in Azure, follow the instructions in **"Create Device"** section of [DeviceExplorer Usage document](https://github.com/Azure/azure-iot-sdks/blob/master/tools/DeviceExplorer/doc/how_to_use_device_explorer.md).

### Write down device credentials
-   Make note of the Connection String for your device by following the instructions in **"Get device connection string or configuration data"** section of [DeviceExplorer Usage document](https://github.com/Azure/azure-iot-sdks/blob/master/tools/DeviceExplorer/doc/how_to_use_device_explorer.md).


<a name="Step-2-PrepareDevice"></a>
# Step 2: Prepare your Device

-   Open a PuTTY session and connect to the device.

  > Note: You can skip this step if you just want to build the sample application without running it.

<a name="Step-3-Build"></a>
# Step 3: Build and Run the sample

<a name="setup"/>
## Setup the development environment

This section shows you how to set up a development environment for the Azure IoT device SDK for C on openSUSE.

1. Clone this repository ([azure-iot-sdks](https://github.com/Azure/azure-iot-sdks)) to the machine you are using.
2. Open a shell and navigate to the folder **c/build_all/linux** in your local copy of the repository.

3. Run the `setup_opensuse.sh` script to install the prerequisite packages and the dependent libraries.

4. Run the `build.sh` script.

This script builds the **iothub_client** and **serializer** libraries and their associated samples in a folder called "cmake" inside you home folder.

 > Note: you will not be able to run the samples until you configure them with a valid IoT Hub device connection string. For more information, see [Run sample on Linux](https://github.com/Azure/azure-iot-sdks/blob/master/c/doc/run_sample_on_desktop_linux.md).

 <a name="buildrunapp"/>
## Run the sample

1. Open the file **c/serializer/samples/simplesample_amqp/simplesample_amqp.c** in a text editor.

2. Locate the following code in the file:
    ```
   static const char* connectionString = "[device connection string]";
    ```
3. Replace "[device connection string]" with the device connection string you noted [earlier](#Step-1-Prerequisites). Save the changes.

4. The section "Send events" in the document [How to use Device Explorer](https://github.com/Azure/azure-iot-sdks/blob/master/tools/DeviceExplorer/doc/how_to_use_device_explorer.md) describes how to prepare the **DeviceExplorer** tool to receive device-to-cloud messages from the sample application.

5. Save your changes and build the samples. To build your sample you can run the build.sh script in the **c/build_all/linux** directory.

6. Run the **~/cmake/serializer/samples/simplesample_amqp/simplesample_amqp** sample application.

7. Use the **DeviceExplorer** utility to observe the messages IoT Hub receives from the **simplesample_amqp** application.

8. See "Send cloud-to-device messages" in the document [How to use Device Explorer for IoT Hub devices](https://github.com/Azure/azure-iot-sdks/blob/master/tools/DeviceExplorer/doc/how_to_use_device_explorer.md) for instructions on sending messages with the **DeviceExplorer** utility.