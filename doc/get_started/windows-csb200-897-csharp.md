---
platform: windows 10 enterprise ltsb
device: csb200-897
language: csharp
---

Run a simple Csharp sample on CSB200-897 device running Windows 10 Enterprise LTSB
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

This document describes how to connect CSB200-897 device running Windows 10 Enterprise LTSB with Azure IoT SDK. This multi-step process includes:

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
-   CSB200-897 device.

<a name="PrepareDevice"></a>
# Step 2: Prepare your Device

###Install Windows 10 Enterprise LTSB on CSB200-897

-   Create a bootable USB Drive. Please follow this guide on how to create a bootable drive (<https://www.microsoft.com/download/windows-usb-dvd-download-tool>)
-   Insert the bootable USB Drive from the previous step into your CSB200-897. Turn on your CSB200-897 device and press the **Delete** key.
-   Change the BIOS Boot option filter to **Boot**.
-   Change the **Boot Option Priorities** to boot from your USB Drive.
-   Save changes and restart your device. Follow on screen instructions to install Windows Operating System on your CSB200-897.
-   When Windows 10 installation is completed, remember to [change to developer mode](https://blogs.msdn.microsoft.com/devschool/2015/05/06/windows-10-setting-the-developer-mode/).


<a name="Build"></a>
# Step 3: Build and Run the sample

-   Download the [Azure IoT SDK](https://github.com/Azure/azure-iot-sdks) and the sample programs and save them to your local repository.
-   Start a new instance of Visual Studio 2015.
-   Open the **iothub_csharp_client.sln** solution in the `csharp\device` folder in your local copy of the repository.
-   In Visual Studio, from Solution Explorer, navigate to the **samples** folder.
-   In the **DeviceClientHttpSample** project, open the ***Program.cs*** file.
-   Locate the following code in the file:

        private const string DeviceConnectionString = "<replace>";
        
-   Replace `<replace>` with the connection string for your device.
-   In **Solution Explorer**, right-click the **DeviceClientHttpSample** project, click **Debug**, and then click **Start new instance** to build and run the sample. The console displays messages as the application sends device-to-cloud messages to IoT Hub.
-   Use the **DeviceExplorer** utility to observe the messages IoT Hub receives from the **Device Client HTTP Sample** application.
-   Refer "Monitor device-to-cloud events" in [DeviceExplorer Usage document](https://github.com/Azure/azure-iot-sdks/blob/master/tools/DeviceExplorer/doc/how_to_use_device_explorer.md) to see the data your device is sending.
-   Refer "Send cloud-to-device messages" in [DeviceExplorer Usage document](https://github.com/Azure/azure-iot-sdks/blob/master/tools/DeviceExplorer/doc/how_to_use_device_explorer.md) for instructions on sending messages to device.

[setup-devbox-windows]: https://github.com/Azure/azure-iot-sdks/blob/master/c/doc/devbox_setup.md
[lnk-setup-iot-hub]: ../setup_iothub.md
[lnk-manage-iot-hub]: ../manage_iot_hub.md
