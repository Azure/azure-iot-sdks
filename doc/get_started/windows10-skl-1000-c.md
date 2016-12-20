---
platform: Windows 10
device: SKL-1000
language: c
---

Run a simple C sample on SKL-1000 device running Windows 10 Home
===
---

# Table of Contents

-   [Introduction](#Introduction)
-   [Step 1: Prerequisites](#Prerequisites)
-   [Step 2: Prepare your Device](#PrepareDevice)
-   [Step 3: Build and Run the Sample](#Build)

<a name="Introduction"/>
# Introduction

**About this document**

This document describes how to connect SKL-1000 device running Windows 10 Home with Azure IoT SDK. This multi-step process includes:
-   Configuring Azure IoT Hub
-   Registering your IoT device
-   Build and deploy Azure IoT SDK on device

<a name="Prerequisites"></a>
# Step 1: Prerequisites

You should have the following items ready before beginning the process:

-   [Prepare your development enviornment][setup-devbox-windows]
-   [Setup your IoT hub][lnk-setup-iot-hub]
-   [Provision your device and get its credentials][lnk-manage-iot-hub]
-   SKM-U mPC device.
-   Monitor, HDMI Cable, USB Keyboard/Mouse, and the Internet connection

<a name="PrepareDevice"></a>
# Step 2: Prepare your Device

-   Connect Ethernet cable to the WAN port on SKL-1000 device and connect the other end to your switch/hub/router.
-   Connect a USB Keyboard and Mouse to the back USB ports of your SKL-1000 device. Using an HDMI cable (The maximum supported screen resolution is 1280 x 1024 or 1920 x 1080 (depends on models)), also connect a monitor to the device.
-   Connect power cord & power on.
-   Wait until the operating system is ready.


<a name="Build"></a>
# Step 3: Build SDK and Run the sample

-   Start a new instance of Visual Studio 2015. Open the **iothub_client_sample_http.sln** solution in the  **azureIotSDks\c\iothub_client\samples\iothub_client_sample_http\windows** folder in your local copy of the repository.

-   In Visual Studio, in **Solution Explorer**, navigate to **samples** folder.

-   In the **iothub_client_sample_http** project, open the **iothub_client_sample_http.c** file.

-   Locate the following code in the file:

        static const char* connectionString = "[device connection string]";

-   Replace the above placeholder with device connection string you obtained in [Step 1](#Step-1:-Prerequisites) and save the changes.

-   In visual Studio, under Solution Explorer, right-click the **iothub_client_sample_amqp** project, click **Debug −> Start new instance** to build and run the sample. The console displays messages as the application sends device-to-cloud messages to IoT Hub.

-   See [Manage IoT Hub][lnk-manage-iot-hub] to learn how to observe the messages IoT Hub receives from the application and how to send cloud-to-device messages to the application.

[setup-devbox-windows]: https://github.com/Azure/azure-iot-sdks/blob/master/c/doc/devbox_setup.md
[lnk-setup-iot-hub]: ../../setup_iothub.md
[lnk-manage-iot-hub]: ../../manage_iot_hub.md
