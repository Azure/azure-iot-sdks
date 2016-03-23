---
platform: windows-10-iot-mobile-enterprise
device: fz-f1
language: csharp
---

Run a simple Csharp sample on FZ-F1 device running Windows 10 IoT Mobile Enterprise
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

This document describes how to connect FZ-F1 device running Windows 10 IoT Mobile Enterprise with Azure IoT SDK. This multi-step process includes:
-   Configuring Azure IoT Hub
-   Registering your IoT device
-   Build and deploy Azure IoT SDK on device

<a name="Prerequisites"></a>
# Step 1: Prerequisites

You should have the following items ready before beginning the process:

-   [Prepare your development environment][devbox-setup]
-   Computer with Git client installed and access to the
    [azure-iot-sdks](https://github.com/Azure/azure-iot-sdks) GitHub public repository.
-   [Setup your IoT hub][lnk-setup-iot-hub]
-   [Provision your device and get its credentials][lnk-manage-iot-hub]
-   FZ-F1 device.

#### Install Visual Studio 2015 and Tools

-   To create Windows IoT Core solutions, you will need to install [Visual Studio 2015](https://www.visualstudio.com/en-us/products/vs-2015-product-editions.aspx). You can install any edition of Visual Studio, including the free Community edition.

    Make sure to select the **Universal Windows App Development Tools**, the component required for writing apps Windows 10:

<a name="PrepareDevice"></a>
# Step 2: Prepare your Device

-   Power on FZ-F1 device.
-   Connet the device to network to use internet access.
-   Turn on [Developer mode](https://msdn.microsoft.com/library/windows/apps/xaml/dn706236.aspx)

<a name="Build"></a>
# Step 3: Build and Run the Sample

<a name="Step_3_1_Connect"/>
## 3.1 Connect the Device

-   Plug the device into your computer using a micro-USB cable.

<a name="Step_3_2_Build"/>
## 3.2  Build the Samples

-   Start a new instance of Visual Studio 2015. Open the **iothub_csharp_client.sln** solution (/azure-iot-sdks/csharp) from your local copy of the repository.

-   In Visual Studio, from **Solution Explorer**, navigate to the **UWPSample(Universal Windows)** project.

-   Locate the following code in the **ConnectionStrings.cs** file:

        public const string DeviceConnectionString = "<replace>";

-   Replace the above placeholder with device connection string you obtained in [Step 1](#Step-1:-Prerequisites) and save the changes.

-   Choose the ARM architecture and set the debugging method to "Device":

-   Build the solution.

-   See [Manage IoT Hub][lnk-manage-iot-hub] to learn how to observe the messages IoT Hub receives from the application and how to send cloud-to-device messages to the application.

[lnk-setup-iot-hub]: ../setup_iothub.md
[lnk-manage-iot-hub]: ../manage_iot_hub.md
[devbox-setup]: https://github.com/Azure/azure-iot-sdks/blob/master/c/doc/devbox_setup.md
