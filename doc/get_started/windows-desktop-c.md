---
platform: windows
device: desktop
language: c
---

Run a simple C sample on Windows
===
---

# Table of Contents

-   [Introduction](#Introduction)
-   [Step 1: Prerequisites](#Step-1-Prerequisites)
-   [Step 2: Build and Run the Sample](#Step-2-Build)

<a name="Introduction"></a>
# Introduction

**About this document**

This document describes how to build and run the **simplesample_amqp** application on the Windows platform. This multi-step process includes:
-   Configuring Azure IoT Hub
-   Registering your IoT device
-   Build and deploy Azure IoT SDK on device

<a name="Step-1-Prerequisites"></a>
# Step 1: Prerequisites

You should have the following items ready before beginning the process:

-   [Prepare your development environment][devbox-setup]
-   Computer with Git client installed and access to the
    [azure-iot-sdks](https://github.com/Azure/azure-iot-sdks) GitHub public repository.
-   Download and install [DeviceExplorer](https://github.com/Azure/azure-iot-sdks/releases/download/2015-11-13/SetupDeviceExplorer.msi).
-   [Set up your IoT hub](https://github.com/Azure/azure-iot-sdks/blob/master/doc/setup_iothub.md).

### Create a device on IoT Hub
-   With your IoT hub configured and running in Azure, follow the instructions in **"Create Device"** section of [DeviceExplorer Usage document](https://github.com/Azure/azure-iot-sdks/blob/master/tools/DeviceExplorer/doc/how_to_use_device_explorer.md).

### Write down device credentials
-   Make note of the Connection String for your device by following the instructions in **"Get device connection string or configuration data"** section of [DeviceExplorer Usage document](https://github.com/Azure/azure-iot-sdks/blob/master/tools/DeviceExplorer/doc/how_to_use_device_explorer.md).

  > Note: You can skip this step if you just want to build the sample application without running it.

<a name="Step-2-Build"></a>
# Step 2: Build and Run the sample

1.   Start a new instance of Visual Studio 2015. Open the **azure_iot_sdks.sln** solution in the **cmake** folder in your home directory.

2.   In Visual Studio, in **Solution Explorer**, navigate to **simplesample_amqp** project, open the **simplesample_amqp.c** file.

3.   Locate the following code in the file:

      ```
      static const char* connectionString = "[device connection string]";
      ```

4.   Replace "[device connection string]" with the device connection string you noted [earlier](#Step-1-Prerequisites) and save the changes:

       ```
       static const char* connectionString = "HostName=..."
       ```
       
5.   The section "Send events" in the document [How to use Device Explorer][device-explorer] describes how to prepare the **DeviceExplorer** tool to receive device-to-cloud messages from the sample application.

6.   In **Solution Explorer**, right-click the **simplesample_amqp** project, click **Debug**, and then click **Start new instance** to build and run the sample. The console displays messages as the application sends device-to-cloud messages to IoT Hub.

7.   Use the **DeviceExplorer** utility to observe the messages IoT Hub receives from the **simplesample_amqp** application.

8.   See "Monitor device-to-cloud events" in the document [How to use Device Explorer][device-explorer] to learn how to use the **DeviceExplorer** utility to send cloud-to-device messages to the **simplesample_amqp** application.


[devbox-setup]: https://github.com/Azure/azure-iot-sdks/blob/master/c/doc/devbox_setup.md
[device-explorer]: https://github.com/Azure/azure-iot-sdks/blob/master/tools/DeviceExplorer/doc/how_to_use_device_explorer.md
