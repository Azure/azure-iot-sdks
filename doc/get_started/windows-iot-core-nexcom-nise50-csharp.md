---
platform: windows-iot-core
device: nexcom-nise50
language: csharp
---

Run a simple Csharp sample on NEXCOM NISE50 device running Windows 10 IoT Core
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

This document describes how to connect NISE50 device running Windows 10 IoT Core with Azure IoT SDK. This multi-step process includes:
-   Configuring Azure IoT Hub
-   Registering your IoT device
-   Build and deploy Azure IoT SDK on device

<a name="Prerequisites"></a>
# Step 1: Prerequisites

You should have the following items ready before beginning the process:

-   Computer with Git client installed and access to the
    [azure-iot-sdks](https://github.com/Azure/azure-iot-sdks) GitHub public repository.
-   NISE50 device.
-   [Azure SDK for .NET](http://go.microsoft.com/fwlink/p/?linkid=618715&clcid=0x409)
-   Download and install [DeviceExplorer](https://github.com/Azure/azure-iot-sdks/releases/download/2015-11-13/SetupDeviceExplorer.msi).
-   [Set up your IoT hub](https://github.com/Azure/azure-iot-sdks/blob/master/doc/setup_iothub.md).

#### Create a device on IoT Hub
-   With your IoT hub configured and running in Azure, follow the instructions in **"Create Device"** section of [DeviceExplorer Usage document](https://github.com/Azure/azure-iot-sdks/blob/master/tools/DeviceExplorer/doc/how_to_use_device_explorer.md).

#### Write down device credentials
-   Make note of the Connection String for your device by following the instructions in **"Get device connection string or configuration data"** section of [DeviceExplorer Usage document](https://github.com/Azure/azure-iot-sdks/blob/master/tools/DeviceExplorer/doc/how_to_use_device_explorer.md).

#### Install Visual Studio 2015 and Tools

-   To create Windows IoT Core solutions, you will need to install [Visual Studio 2015](https://www.visualstudio.com/en-us/products/vs-2015-product-editions.aspx). You can install any edition of Visual Studio, including the free Community edition.

    Make sure to select the **Universal Windows App Development Tools**, the component required for writing apps Windows 10:

<a name="PrepareDevice"></a>
# Step 2: Prepare your Device

First of all, you need to set up your device.
1.   Make Bootable USB with Windows 10 with following software
     -   Windows 10 32 bit
     -   WiNToBootic :
     -   Flash.ffu [here](ftp://nexcomq:nexcomq@ftp.nexcom.com.tw/Flash.ffu)
2.   Use WintoBootic to make bootable USB
    ![](./media/WintoBootic.png)
3.   When you finish this bootable Device, copy Flash.FFU to this USB
4.   Install Win 10 Core on NIS50
     -   Disable "Security Boot"
     -   Boot your computer with this USB(UEFI)
         ![](./media/Boot_NISE50.png) 
     -   Select Windows 10 Setup (32ibit)
         ![](./media/Win10_32Bit.png)
5.  When system display "Install" button on screen, Press Shift+F10 to open terminal and using following command
     -   diskpart -> list disk(Confirm your device ID which you to deploy)
         ![](./media/ConfirmDeviceId.png)
     -   Enter exit (Leave diskpart)
     -   Enter following command to start install Win 10 Core     
                   
             dism.exe /Apply-Image /ImageFile:flash.ffu /ApplyDrive:\\.\PhysicalDrive0 /SkipPlatformCheck

<a name="Build"></a>
# Step 3: Build and Run the Sample

This section walks you through building, deploying and validating the IoT Client SDK on your device running Windows 10 IoT Core operating system. 

<a name="Step_3_1:_Connect"/>
## 3.1 Connect the Device

-   Connect the board to your network using an Ethernet cable. This step is required, as the sample depends on internet access.

-   Plug the device into your computer using a micro-USB cable.

<a name="Step_3_2:_Build"/>
## 3.2  Build the Samples

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

-   On Windows, refer "Monitor device-to-cloud events" in [DeviceExplorer Usage document](https://github.com/Azure/azure-iot-sdks/blob/master/tools/DeviceExplorer/doc/how_to_use_device_explorer.md) to see the data your device is sending.

-   If you are running other OS, please use the JavaScript tool [iot-hub explorer tool](https://github.com/Azure/azure-iot-sdks/tree/master/tools/iothub-explorer/doc).
     
-   In Visual Studio, from **Solution Explorer**, right-click the **UWPSample(Universal Windows)** project, click **Debug &minus;&gt; Start new instance** to build and run the sample. 
       
-   You should be able to see the events received in the DeviceExplorer's data tab.

### 3.3.2 Receive messages from IoT Hub

-   On Windows, refer "Send cloud-to-device messages" in [DeviceExplorer Usage document](https://github.com/Azure/azure-iot-sdks/blob/master/tools/DeviceExplorer/doc/how_to_use_device_explorer.md) for instructions on sending messages to device.

-   If you are running other OS, please use the JavaScript tool [iot-hub explorer tool](https://github.com/Azure/azure-iot-sdks/tree/master/tools/iothub-explorer/doc).

-   You should be able to see the message received in the device console window after following DeviceExplorer usage document.