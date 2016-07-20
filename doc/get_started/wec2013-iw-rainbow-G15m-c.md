---
platform: wec2013
device: iw-rainbow-G15m
language: c
---

Run a simple C sample on iW-RainboW-G15M device running WEC2013
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

This document describes how to connect iW-RainboW-G15M device running WEC2013 with Azure IoT SDK. This multi-step process includes:

-   Configuring Azure IoT Hub
-   Registering your IoT device
-   Build and deploy Azure IoT SDK on device

<a name="Prerequisites"></a>
# Step 1: Prerequisites

You should have the following items ready before beginning the process:

-   Refer [Prepare your development environment][setup-devbox-windows] to set up Windows Embedded Compact 2013 development environment.
-   [Setup your IoT hub][lnk-setup-iot-hub]
-   [Provision your device and get its credentials][lnk-manage-iot-hub]
-   iW-RainboW-G15M device.


<a name="PrepareDevice"></a>
# Step 2: Prepare your Device

-   Follow the instructions in the Device User guide to boot the board with SD card.
-   Connect Ethernet cable to the device.

<a name="Build"></a>
# Step 3: Build SDK and Run the sample

-   Start a new instance of Visual Studio 2015. Open the **azure_iot_sdks.sln** solution in the **cmake** folder in your home directory.

-   In Visual Studio, in **Solution Explorer**, navigate to **simplesample_http** project, open the **simplesample_http.c** file.

-   Locate the following code in the file:

        static const char* connectionString = "[device connection string]";

-   Replace *[device connection string]* with device connection string you obtained in [Step 1](#Prerequisites) and save the changes.

-   See [Manage IoT Hub][lnk-manage-iot-hub] to learn how to observe the messages IoT Hub receives from the application.

-   In **Solution Explorer**, right-click the **simplesample_http** project, click **Build**.
-   This will create an “simplesample_http.exe” application in the below path,

		..\cmake_ce8\serializer\samples\simplesample_http\Release.

-   Copy simplesample.exe application to the SD card.

-   Insert the SD card in your iW-Rainbow-G15M-Q7 board and boot your device.

-   Install certificate in the device by clicking the Certificate option in the command menu.

-   Check that date and time on your device are set correctly. If not set properly, set using the “control panel” -   application in the “command” menu. Using a date outside of the validity period of your certificates can prevent your device from getting connected to the hub. 

-   Run the Simplesample.exe application in the SD card through file explorer in the command menu.

-   See [Manage IoT Hub][lnk-manage-iot-hub] to learn how to send cloud-to-device messages to the application.

[setup-devbox-windows]: https://github.com/Azure/azure-iot-sdks/blob/master/c/doc/devbox_setup.md
[lnk-setup-iot-hub]: ../setup_iothub.md
[lnk-manage-iot-hub]: ../manage_iot_hub.md
