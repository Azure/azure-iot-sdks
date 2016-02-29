---
platform: windows-7-professional
device: EC700-BT
language: c
---

Run a simple C sample on EC700-BT device running Windows 7 Professional
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

This document describes how to build and run the **simplesample_amqp** application on the EC700-BT device running **Windows 7 Professional** with Azure IoT SDK.
This multi-step process includes:
-   Configuring Azure IoT Hub
-   Registering your IoT device
-   Build and deploy Azure IoT SDK on device

<a name="Step-1-Prerequisites"></a>
# Step 1: Prerequisites

-   Computer with GitHub installed and access to the
    [azure-iot-sdks](https://github.com/Azure/azure-iot-sdks) GitHub
    private repository.
-   EC700-BT.
-   Install any version of [Visual Studio 2015][download-visual-studio].
-   Install [Microsoft Azure SDK][download-azure-sdk].
-   [Setup your IoT hub][lnk-setup-iot-hub]
-   [Provision your device and get its credentials][lnk-manage-iot-hub]

<a name="Step-2-PrepareDevice"></a>
# Step 2: Prepare your Device
##  Install Windows 7 Professional on EC700-BT
-   Create a bootable USB Drive. Please follow this guide on [how to create a bootable drive].
-   Insert the bootable USB Drive from the previous step into your EC700-BT. Turn on your EC700-BT device and press the **Delete** key.
-   Change the BIOS Boot option filter to **UEFI and Legacy**.
-   Change the BIOS OS Selection to "Windows 7".
-   Change the **Boot Option Priorities** to boot from your USB Drive.
-   Save changes and restart your EC700-BT. Follow on screen instructions to install Windows Operating System on your EC700-BT.

<a name="Step-3-Build"></a>
# Step 3: Build and Run the sample

-   Download the [Azure IoT SDK](https://github.com/Azure/azure-iot-sdks) and save them to your local repository.
-   Start a new instance of Visual Studio 2015.
-   Open the **iothub_client_sample_amqp.sln** solution in the `azureIotSDks\c\iothub_client\samples\iothub_client_sample_amqp\windows` folder in your local copy of the repository.
-   In Visual Studio, from Solution Explorer, navigate to the **samples** folder.
-   In the **iothub_client_sample_amqp** project, open the *** iothub_client_sample_amqp.c*** file.
-   Locate the following code in the file:

        static const char* connectionString = "<replace>";
        
-   Replace `<replace>` with the connection string for your device.
-   In visual Studio, under Solution Explorer, right-click the **iothub_client_sample_amqp** project, click ***Debug &minus;&gt; Start new instance*** to build and run the sample. The console displays messages as the application sends device-to-cloud messages to IoT Hub.
-   See [Manage IoT Hub][lnk-manage-iot-hub] to learn how to observe the messages IoT Hub receives from the application and how to send cloud-to-device messages to the application.

[download-visual-studio]: https://www.visualstudio.com/en-us/downloads/download-visual-studio-vs.aspx
[download-azure-sdk]: http://www.microsoft.com/en-us/download/details.aspx?id=48178
[create-bootable-drive]: https://www.microsoft.com/en-us/download/windows-usb-dvd-download-tool

[lnk-setup-iot-hub]: ../setup_iothub.md
[lnk-manage-iot-hub]: ../manage_iot_hub.md
