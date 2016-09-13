---
platform: windows 10 iot enterprise ltsb
device: titan-500
language: c
---

Run a simple C sample on Titan-500 device running Windows 10 IoT Enterprise LTSB
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

This document describes how to connect Titan-500 device running Windows 10 IoT Enterprise LTSB with Azure IoT SDK. This multi-step process includes:
-   Configuring Azure IoT Hub
-   Registering your IoT device
-   Build and deploy Azure IoT SDK on device

<a name="Prerequisites"></a>
# Step 1: Prerequisites

You should have the following items ready before beginning the process:

-   Computer with GitHub installed and access to the [azure-iot-sdks GitHub](https://github.com/Azure/azure-iot-sdks) private repository.
-   Install any version of [Visual Studio 2015](https://www.visualstudio.com/downloads/download-visual-studio-vs.aspx). 
    Make sure you have following components selected:
    -   Programming Languages
        -   Visual C++
            -   Common Tools for Visual C++ 2015
            -   Microsoft Foundation Classes for C++
    -   Windows and Web Development
        -   Microsoft SQL server Data Tools
    -   Universal Windows App Development Tools
        -   Tools and Windows 10 SDK
        -   Windows 10 SDK
-   Install [Microsoft Azure SDK for .NET](http://www.microsoft.com/download/details.aspx?id=48178)
-   [Setup your IoT hub][lnk-setup-iot-hub]
-   [Provision your device and get its credentials][lnk-manage-iot-hub]
-   Titan-500 device.   

<a name="PrepareDevice"></a>
# Step 2: Prepare your Device

Install Windows 10 IoT Enterprise LTSB on Titan-500
-   Create a bootable USB Drive.
-   Insert the bootable USB Drive from the previous step into your Titan-500. Turn on your Titan-500 device and press the Delete key.
-   Change the BIOS Boot option filter to UEFI and Legacy.
-   Change the BIOS OS Selection to **Windows 10**.
-   Change the Boot Option Priorities to boot from your USB Drive.
-   Save changes and restart your Titan-500. Follow on screen instructions to install Windows Operating System on your Titan-500.

<a name="Build"></a>
# Step 3: Build SDK and Run the sample

-   Start a new instance of Visual Studio 2015. Open the **iothub_client_sample_http.sln** solution in the  **azureIotSDks\c\iothub_client\samples\iothub_client_sample_http\windows** folder in your local copy of the repository.

-   In Visual Studio, in **Solution Explorer**, navigate to **samples** folder.

-   In the **iothub_client_sample_http** project, open the **iothub_client_sample_http.c** file.

-   Locate the following code in the file:

        static const char* connectionString = "[device connection string]";

-   Replace the above placeholder with device connection string you obtained in [Step 1](#Step-1:-Prerequisites) and save the changes.

-   In Visual Studio, under Solution Explorer, right-click the **iothub_client_sample_http** project, click **Debug −> Start new instance** to build and run the sample. The console displays messages as the application sends device-to-cloud messages to IoT Hub.

-   See [Manage IoT Hub][lnk-manage-iot-hub] to learn how to observe the messages IoT Hub receives from the application and how to send cloud-to-device messages to the application.

[setup-devbox-windows]: https://github.com/Azure/azure-iot-sdks/blob/master/c/doc/devbox_setup.md
[lnk-setup-iot-hub]: ../../setup_iothub.md
[lnk-manage-iot-hub]: ../../manage_iot_hub.md
