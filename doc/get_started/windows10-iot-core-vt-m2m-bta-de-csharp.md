---
platform: windows 10 iot core
device: vt-m2m-bta_de
language: csharp
---
Run a simple Csharp sample on VT-M2M-BTA_DE device running Windows 10 IoT Core
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

This document describes how to connect VT-M2M-BTA_DE device running Windows 10 IoT Core with Azure IoT SDK. This multi-step process includes:
-   Configuring Azure IoT Hub
-   Registering your IoT device
-   Build and deploy Azure IoT SDK on device

<a name="Prerequisites"></a>
# Step 1: Prerequisites

You should have the following items ready before beginning the process:

-   [Setup your IoT hub][lnk-setup-iot-hub]
-   [Provision your device and get its credentials][lnk-manage-iot-hub]
-   VT-M2M-BTA_DE device.

#### Install Visual Studio 2015 and Tools

-   To create Windows IoT Core solutions, you will need to install [Visual Studio 2015](https://www.visualstudio.com/en-us/products/vs-2015-product-editions.aspx). You can install any edition of Visual Studio, including the free Community edition.

    Make sure to select the **Universal Windows App Development Tools**, the component required for writing apps in Windows 10.

<a name="PrepareDevice"></a>
# Step 2: Prepare your Device

#### Install IoT Core

    To install IoT core, please click [here](http://ms-iot.github.io/content/zh-CN/win10/SetupMBM.htm) for details.

#### Run VT-M2M-BTA_DE on Win10 IoT
-   Insert our IoT core disk into VT-M2M-BTA_DE. Turn on device.
-   When boot, press ESC to get to the BIOS settings. Enter Boot Manager, Select our disk to boot.
-   Once the device has booted, the DefaultApp will launch.

<a name="Build"></a>
# Step 3: Build and Run the Sample

<a name="Step_3_1:_Connect"></a>
## 3.1 Connect the Device

-   Connect the board to your network using an Ethernet cable or Wifi adapter. This step is required, as the sample depends on internet access.

<a name="Step_3_2:_Build"></a>
## 3.2  Build the Samples

-   Start a new instance of Visual Studio 2015. Open the **iothub_csharp_client.sln** solution (/azure-iot-sdks/csharp) from your local copy of the repository.

-   In Visual Studio, from **Solution Explorer**, navigate to the **UWPSample(Universal Windows)** project.

-   Locate the following code in the **ConnectionStrings.cs** file:
        public const string DeviceConnectionString = "<replace>";

-   Replace the above placeholder with device connection string you obtained in [Step 1](#Prerequisites) and save the changes.

-   Choose the right architecture (ARM) and set the debugging method to "Remote Machine":

-   To deploy the binaries on your device, right-click on the UWPSample project in the **Solution Explorer**, select **Properties** and navigate to the **Debug** tab:
    Type in the IP Address of your device. Make sure the "Use authentication" box is unchecked.

-   Build the solution.


<a name="Step_3_3:_Run"></a>
## 3.3 Run and Validate the Samples

### 3.3.1 Send Device Events to IoT Hub
-   In Visual Studio, from **Solution Explorer**, right-click the **UWPSample(Universal Windows)** project, click **Debug &minus;&gt; Start new instance** to build and run the sample. 
-   See [Manage IoT Hub][lnk-manage-iot-hub] to learn how to observe the messages IoT Hub receives from the application.

### 3.3.2 Receive messages from IoT Hub
-   See [Manage IoT Hub][lnk-manage-iot-hub] to learn how to send cloud-to-device messages to the application.

[lnk-setup-iot-hub]: ../setup_iothub.md
[lnk-manage-iot-hub]: ../manage_iot_hub.md
