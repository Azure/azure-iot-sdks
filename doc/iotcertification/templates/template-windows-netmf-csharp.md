---
platform: {enter the OS name running on device}
device: {enter your device name here}
language: csharp
---

Run a simple Csharp sample on {enter your device name here} device running {enter the OS name running on device}
===
---

# Table of Contents

-   [Introduction](#Introduction)
-   [Step 1: Prerequisites](#Prerequisites)
-   [Step 2: Prepare your Device](#PrepareDevice)
-   [Step 3: Build and Run the Sample](#Build)

# Instructions for using this template

-   Replace the text in {placeholders} with correct values.
-   Delete the lines {{enclosed}} after following the instructions enclosed between them.
-   It is advisable to use external links, wherever possible.
-   Remove this section from final document.

<a name="Introduction"></a>
# Introduction

**About this document**

This document describes how to connect {enter your device name here} device running {enter the OS name running on device} with Azure IoT SDK. This multi-step process includes:
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
-   {enter your device name here} device.

#### Install Visual Studio 2015 and Tools

-   To create Windows .NET Micro Framework solutions, you will need to install [Visual Studio 2015](https://www.visualstudio.com/en-us/products/vs-2015-product-editions.aspx). You can install any edition of Visual Studio, including the free Community edition.

    After installing Visual Studio, goto Tools option in the menu bar and click on *Extensions and Updates*. Search for 'netmf' and install .NET Micro Framework SDK.

-   {{Please specify if any other software(s) or hardware(s) are required.}}

<a name="PrepareDevice"></a>
# Step 2: Prepare your Device

-   {{Write down the instructions required to setup, configure and connect your device. Please use external links when possible pointing to your own page with device preparation steps.}}

<a name="Build"></a>
# Step 3: Build and Run the Sample

<a name="Step_3_1_Connect"></a>
## 3.1 Connect the Device

-   Connect the board to your network using an Ethernet cable. This step is required, as the sample depends on internet access.

-   Plug the device into your computer using a micro-USB cable.

<a name="Step_3_2_Build"></a>
## 3.2  Build the Samples

-   Start a new instance of Visual Studio 2015. Open the **iothub_csharp_netmf_client.sln** solution (/azure-iot-sdks/csharp) from your local copy of the repository.

-   In Visual Studio, from **Solution Explorer**, navigate to the **NetMFDeviceClientHttpSample_<dotnet microframework version>** project.

-   Locate the following code in the **Program.cs** file:

        public const string DeviceConnectionString = "<replace>";

-   Replace the above placeholder with device connection string you obtained in [Step 1](#Prerequisites) and save the changes.
    
-   To build and deploy the binaries on your device, right-click on the project in the **Solution Explorer**, select **Properties** and navigate to the **.NET Micro Framework** tab.
	
    Select the **Transport** and **Device** which you have connected.

-   Build the solution.

<a name="Step_3_3_Run"></a>
## 3.3 Run and Validate the Samples

### 3.3.1 Send Device Events to IoT Hub

-   In Visual Studio, from **Solution Explorer**, right-click the sample project, click **Debug &minus;&gt; Start new instance** to build and run the sample. 

-   See [Manage IoT Hub][lnk-manage-iot-hub] to learn how to observe the messages IoT Hub receives from the application.

### 3.3.2 Receive messages from IoT Hub

-   See [Manage IoT Hub][lnk-manage-iot-hub] to learn how to send cloud-to-device messages to the application.

[lnk-setup-iot-hub]: ../../setup_iothub.md
[lnk-manage-iot-hub]: ../../manage_iot_hub.md
