---
platform: windows-8.1-embedded-industry-enterprise
device: el20
language: csharp
---

Run a simple sample on EL20 device running Windows 8.1 Embedded Industry Enterprise
===
---

# Table of Contents

-   [Introduction](#Introduction)
-   [Step 1: Prerequisites](#Step-1:-Prerequisites)
-   [Step 2: Prepare your Device](#Step-2:-PrepareDevice)
-   [Step 3: Build and Run the Sample](#Step-3:-Build)

<a name="Introduction"></a>
# Introduction

**About this document**

This document provides step-by-step guidance on how to connect an EL20 device running **Windows 8.1 Embedded Industry Enterprise** with Azure IoT SDK. This multi-step process includes:
-   Configuring Azure IoT Hub
-   Registering your IoT device
-   Build and deploy Azure IoT SDK on device

<a name="Step-1:-Prerequisites"></a>
# Step 1: Prerequisites

-    Computer with GitHub installed and access to the
    [azure-iot-sdks](https://github.com/Azure/azure-iot-sdks) GitHub
    private repository.
-   EL20 device.
-   Install any version of [Visual Studio 2015](https://www.visualstudio.com/downloads/download-visual-studio-vs.aspx).
-   Install [Microsoft Azure SDK](http://www.microsoft.com/download/details.aspx?id=48178).
-   [Setup your IoT hub][lnk-setup-iot-hub]
-   [Provision your device and get its credentials][lnk-manage-iot-hub]

<a name="Step-2:-PrepareDevice"></a>
# Step 2: Prepare your Device
##  Install Windows 8.1 Embedded Industry Enterprise on EL20
-   Create a bootable USB Drive. Please follow this guide on how to create a bootable drive (<https://www.microsoft.com/en-us/download/windows-usb-dvd-download-tool>).
-   Insert the bootable USB Drive from the previous step into your EL20. Turn on your EL20 device and press the **Delete** key.
-   Change the BIOS Boot option filter to **UEFI and Legacy**.
-   Change the **Boot Option Priorities** to boot from your USB Drive.
-   Save changes and restart your EL20. Follow on screen instructions to install Windows Operating System on your EL20.

<a name="Step-3:-Build"></a>
# Step 3: Build and Run the sample

-   Download the [Azure IoT SDK](https://github.com/Azure/azure-iot-sdks) and save them to your local repository.
-   Start a new instance of Visual Studio 2015.
-   Open the **iothub_csharp_client.sln** solution in the `csharp\device` folder in your local copy of the repository.
-   In Visual Studio, from Solution Explorer, navigate to the **samples** folder.
-   In the **DeviceClientAmqpSample** project, open the ***Program.cs*** file.
-   Locate the following code in the file:

        private const string DeviceConnectionString = "<replace>";
        
-   Replace `<replace>` with the connection string for your device.
-   In visual Studio, under Solution Explorer, right-click the **DeviceClientAmqpSample** project, click ***Debug &minus;&gt; Start new instance*** to build and run the sample. The console displays messages as the application sends device-to-cloud messages to IoT Hub.
-   See [Manage IoT Hub][lnk-manage-iot-hub] to learn how to observe the messages IoT Hub receives from the application and how to send cloud-to-device messages to the application.

[lnk-setup-iot-hub]: ../setup_iothub.md
[lnk-manage-iot-hub]: ../manage_iot_hub.md
