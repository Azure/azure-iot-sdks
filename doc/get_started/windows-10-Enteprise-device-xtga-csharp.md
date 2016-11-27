---
platform: Windows 10 Enteprise
device: device-xtga（DS-96128/96256N-I24）
language: csharp
---

Run a simple Csharp sample on device-xtga（DS-96128/96256N-I24） device running Windows 10 Enteprise
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

This document describes how to connect device-xtga（DS-96128/96256N-I24） device running Windows 10 Enteprise with Azure IoT SDK. This multi-step process includes:
-   Configuring Azure IoT Hub
-   Registering your IoT device
-   Build and deploy Azure IoT SDK on device

<a name="Prerequisites"></a>
# Step 1: Prerequisites

You should have the following items ready before beginning the process:

-   Computer with GitHub installed and access to the azure-iot-sdks GitHub private repository
-   Install any version of Visual Studio 2015.
-   Install Microsoft Azure SDK.
-   [Setup your IoT hub][lnk-setup-iot-hub]
-   [Provision your device and get its credentials][lnk-manage-iot-hub]
-   [device-xtga（DS-96128/96256N-I24）][device-link] device.

<a name="PrepareDevice"></a>
# Step 2: Prepare your Device
-   Connect the power adapter, USB Keyborad/Mouse with Avalue [device-xtga（DS-96128/96256N-I24）][device-link].
-   Press the power button on the back of the device.
-   Wait until the operating system is ready.

<a name="Build"></a>
# Step 3: Build and Run the sample
- 	Download the Azure IoT SDK and the sample programs and save them to your local repository.
- 	Start a new instance of Visual Studio 2015.
- 	Open the iothub_csharp_client.sln solution in the csharp\device folder in your local copy of the repository.
- 	In Visual Studio, from Solution Explorer, navigate to the samples folder.
- 	In the DeviceClientAmqpSample project, open the Program.cs file.
-   Locate the following code in the file:

        private const string DeviceConnectionString = "<replace>";
        
-   Replace `<replace>` with the connection string for your device.
- 	In Solution Explorer, right-click the DeviceClientAmqpSample project, click Debug, and then click Start new instanceto build and run the sample. The console displays messages as the application sends device-to-cloud messages to IoT Hub.
- 	Use the DeviceExplorer utility to observe the messages IoT Hub receives from the Device Client AMQP Sampleapplication.
-   Refer "Monitor device-to-cloud events" in [DeviceExplorer Usage document](https://github.com/Azure/azure-iot-sdks/blob/master/tools/DeviceExplorer/doc/how_to_use_device_explorer.md) to see the data your device is sending.
-   Refer "Send cloud-to-device messages" in [DeviceExplorer Usage document](https://github.com/Azure/azure-iot-sdks/blob/master/tools/DeviceExplorer/doc/how_to_use_device_explorer.md) for instructions on sending messages to device.
 

[setup-devbox-windows]: https://github.com/Azure/azure-iot-sdks/blob/master/c/doc/devbox_setup.md
[lnk-setup-iot-hub]: https://github.com/Azure/azure-iot-sdks/blob/master/doc/setup_iothub.md
[lnk-manage-iot-hub]: https://github.com/Azure/azure-iot-sdks/blob/master/doc/manage_iot_hub.md
[device-link]:http://www.hikvision.com/cn/prgs_642_i13158.html
