---
platform: Windows Embedded Standard 7
device: Industrial PC PS5000 Series
language: csharp
---

Run a simple Csharp sample on Industrial PC PS5000 Series device running Windows Embedded Standard 7
===
---

# Table of Contents

-   [Introduction](#Introduction)
-   [Step 1: Prerequisites](#Prerequisites)
-   [Step 2: Prepare your Device](#PrepareDevice)
-   [Step 3: Build and Run the Sample](#Build)

<a name="Introduction"></a>
# Introduction

**About this document**

This document describes how to connect Industrial PC PS5000 Series device running Windows Embedded Standard 7 with Azure IoT SDK. This multi-step process includes:
-   Configuring Azure IoT Hub
-   Registering your IoT device
-   Build and deploy Azure IoT SDK on device

<a name="Prerequisites"></a>
# Step 1: Prerequisites

You should have the following items ready before beginning the process:

-   [Prepare your development environment][setup-devbox-windows]
-   [Setup your IoT hub][lnk-setup-iot-hub]
-   [Provision your device and get its credentials][lnk-manage-iot-hub]
-   Industrial PC PS5000 Series device.

<a name="PrepareDevice"></a>
# Step 2: Prepare your Device

-   Connect the power adapter, USB Keyborad/Mouse with PS5000.
-   Power on your PS5000 and connect to the Internet.
-   Setup Windows.
-   Update Windows and .Net Framework the latest.

<a name="Build"></a>
# Step 3: Build and Run the sample

-   Download the [Azure IoT SDK](https://github.com/Azure/azure-iot-sdks) and the sample programs and save them to your local repository.
-   Start a new instance of Visual Studio 2015.
-   Open the **iothub_csharp_client.sln** solution in the `csharp\device` folder in your local copy of the repository.
-   In Visual Studio, from Solution Explorer, navigate to the **samples** folder.
  -   For AMQP protocol:
   Navigate to **DeviceClientAmqpSample** project and open the **Program.cs** file.
  -   For HTTP protocol:
   Navigate to **DeviceClientHttpSample** project and open the **Program.cs** file.
  -   For MQTT protocol:
   Navigate to **DeviceClientMqttSample** project and open the **Program.cs** file.
-   Locate the following code in the file:

        private const string DeviceConnectionString = "<replace>";
        
-   Replace `<replace>` with the connection string for your device.
-   In **Solution Explorer**,
  -   For AMQP protocol:
   Right-click the **DeviceClientAmqpSample** project, click **Debug**, and then click **Start new instance** to build and run the sample. The console displays messages as the application sends device-to-cloud messages to IoT Hub.
  -   For HTTP protocol:
   Right-click the **DeviceClientHttpSample** project, click **Debug**, and then click **Start new instance** to build and run the sample. The console displays messages as the application sends device-to-cloud messages to IoT Hub.
  -   For MQTT protocol:
   Right-click the **DeviceClientMqttSample** project, click **Debug**, and then click **Start new instance** to build and run the sample. The console displays messages as the application sends device-to-cloud messages to IoT Hub.
-   Use the **DeviceExplorer** utility to observe the messages IoT Hub receives from the **Device Client AMQP/HTTP/MQTT Sample** application.
-   Refer "Monitor device-to-cloud events" in [DeviceExplorer Usage document](https://github.com/Azure/azure-iot-sdks-preview/blob/master/tools/DeviceExplorer/doc/how_to_use_device_explorer.md) to see the data your device is sending.
-   Refer "Send cloud-to-device messages" in [DeviceExplorer Usage document](https://github.com/Azure/azure-iot-sdks-preview/blob/master/tools/DeviceExplorer/doc/how_to_use_device_explorer.md) for instructions on sending messages to device.

[setup-devbox-windows]: https://github.com/Azure/azure-iot-sdks/blob/master/c/doc/devbox_setup.md
[lnk-setup-iot-hub]: https://github.com/Azure/azure-iot-sdks/blob/master/doc/setup_iothub.md
[lnk-manage-iot-hub]: https://github.com/Azure/azure-iot-sdks/blob/master/doc/manage_iot_hub.md
