---
platform: windows 10 iot enterprise ltsb
device: etile22-fw
language: csharp
---

Run a simple Csharp sample on ETILE22-FW device running Windows 10 IoT Enterprise LTSB
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

This document describes how to connect ETILE22-FW device running Window 10 IoT Enterprise LTSB with Azure IoT SDK. This multi-step process includes:
-   Configuring Azure IoT Hub
-   Registering your IoT device
-   Build and deploy Azure IoT SDK on device

<a name="Prerequisites"></a>
# Step 1: Prerequisites

You should have the following items ready before beginning the process:

-   Computer with Git client installed and access to the
    [azure-iot-sdks](https://github.com/Azure/azure-iot-sdks) GitHub public repository.
-   [Setup your IoT hub][lnk-setup-iot-hub]
-   [Provision your device and get its credentials][lnk-manage-iot-hub]
-   AOPEN ETILE22-FW device.

#### Install Visual Studio 2015 and Tools

-   To create Windows IoT Core solutions, you will need to install [Visual Studio 2015](https://www.visualstudio.com/en-us/products/vs-2015-product-editions.aspx). You can install any edition of Visual Studio, including the free Community edition.

    Make sure to select the **Universal Windows App Development Tools**, the component required for writing apps Windows 10:

<a name="PrepareDevice"></a>
# Step 2: Prepare your Device

-   Connect the power adapter with AOPEN ETILE22-FW.
-   Press the power button by “Power on\off Steel tool” .
-   Wait until the operating system is ready.
-   You may download [user manual](http://global.aopen.com/download.aspx)

<a name="Build"></a>
# Step 3: Build and Run the Sample

<a name="Step_3_1:_Connect"></a>
## 3.1 Connect the Device

-   Connect the board to your network using an Ethernet cable. This step is required, as the sample depends on internet access.

-   Plug the device into your computer using a micro-USB cable.

<a name="Step_3_2:_Build"></a>
## 3.2  Build the Samples

-   Download the Azure IoT SDK and the sample programs and save them to your local repository.

-   Start a new instance of Visual Studio 2015. Open the **iothub_csharp_client.sln** solution (/azure-iot-sdks/csharp) from your local copy of the repository.

-   In Visual Studio, from **Solution Explorer**, navigate to the **UWPSample(Universal Windows)** project.

-   Locate the following code in the **ConnectionStrings.cs** file:

        public const string DeviceConnectionString = "<replace>";

-   Replace the above placeholder with device connection string you obtained in [Step 1](#Step-1:-Prerequisites) and save the changes.

-   Choose the right architecture (x86 or ARM, depending on your device) and set the debugging method to "Remote Machine":
    
-   To deploy the binaries on your device, right-click on the UWPSample project in the **Solution Explorer**, select **Properties** and navigate to the **Debug** tab:

    Type in the name of your device. Make sure the "Use authentication" box is unchecked.

-   Build the solution.

<a name="Step_3_3:_Run"/>
## 3.3 Run and Validate the Samples

### 3.3.1 Send Device Events to IoT Hub

-   In Visual Studio, from **Solution Explorer**, right-click the **UWPSample(Universal Windows)** project, click **Debug &minus;&gt; Start new instance** to build and run the sample. 

-   See [Manage IoT Hub][lnk-manage-iot-hub] to learn how to observe the messages IoT Hub receives from the application.

### 3.3.2 Receive messages from IoT Hub

-   See [Manage IoT Hub][lnk-manage-iot-hub] to learn how to send cloud-to-device messages to the application.

[lnk-setup-iot-hub]: ../../setup_iothub.md
[lnk-manage-iot-hub]: ../../manage_iot_hub.md
