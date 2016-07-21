---
platform: windows 10 iot enterprise 2015 ltsb
device: hw-5000
language: c
---

Run a simple C sample on HW-5000 device running Windows 10 IoT Enterprise 2015 LTSB
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

This document describes how to connect HW-5000 device running Windows 10 IoT Enterprise 2015 LTSB with Azure IoT SDK. This multi-step process includes:
-   Configuring Azure IoT Hub
-   Registering your IoT device
-   Build and deploy Azure IoT SDK on device

<a name="Prerequisites"></a>
# Step 1: Prerequisites

You should have the following items ready before beginning the process:

-   [Prepare your development environment][setup-devbox-windows]
-   [Setup your IoT hub][lnk-setup-iot-hub]
-   [Provision your device and get its credentials][lnk-manage-iot-hub]
-   HW-5000 device.

<a name="PrepareDevice"></a>
# Step 2: Prepare your Device

-   Nothing specific.

<a name="Build"></a>
# Step 3: Build and Run the Sample

<a name="Step_3_1:_Connect"/>
## 3.1 Connect the Device

-   Turn on your HW-5000 device and connect to internet.

<a name="Step_3_2:_Build"/>
## 3.2  Build the Samples

-   Start a new instance of Visual Studio 2015. Open the **azure_iot_sdks.sln** solution in the **cmake** folder in your home directory.

-   In Visual Studio, in **Solution Explorer**, navigate to **simplesample_amqp** project, open the **simplesample_amqp.c** file.

-   Locate the following code in the file:

        static const char* connectionString = "[device connection string]";

-   Replace the above placeholder with device connection string you obtained in [Step 1](#Prerequisites) and save the changes.

-   See [Manage IoT Hub][lnk-manage-iot-hub] to learn how to observe the messages IoT Hub receives from the application.

-   In **Solution Explorer**, right-click the **simplesample_amqp** project, click **Debug**, and then click **Start new instance** to build and run the sample. The console displays messages as the application sends device-to-cloud messages to IoT Hub.

-   See [Manage IoT Hub][lnk-manage-iot-hub] to learn how to send cloud-to-device messages to the application.

[setup-devbox-windows]: https://github.com/Azure/azure-iot-sdks/blob/master/c/doc/devbox_setup.md
[lnk-setup-iot-hub]: ../setup_iothub.md
[lnk-manage-iot-hub]: ../manage_iot_hub.md