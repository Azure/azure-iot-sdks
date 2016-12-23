---
platform: windows 10
device: skm-u mpc
language: csharp
---

Run a simple C sample on SKM-U mPC device running Windows 10 Home
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

This document describes how to connect SKM-U mPC device running Windows 10 Pro with Azure IoT SDK. This multi-step process includes:
-   Configuring Azure IoT Hub
-   Registering your IoT device
-   Build and deploy Azure IoT SDK on device

<a name="Prerequisites"></a>
# Step 1: Prerequisites

You should have the following items ready before beginning the process:

-   [Prepare your development enviornment][setup-devbox-windows]
-   [Setup your IoT hub][lnk-setup-iot-hub]
-   [Provision your device and get its credentials][lnk-manage-iot-hub]
-   SKM-U mPC device.
-   Monitor, HDMI Cable, USB Keyboard/Mouse, and the Internet connection   

<a name="PrepareDevice"></a>
# Step 2: Prepare your Device

-   Connect Ethernet cable to the WAN port on SKM-U mPC device and connect the other end to your switch/hub/router.

-   Connect a USB Keyboard and Mouse to the back USB ports of your SKM-U mPC device. Using an HDMI cable (The maximum supported screen resolution is 1280 x 1024 or 1920 x 1080 (depends on models)), also connect a monitor to the device.

-   Connect power cord & power on.

-   Wait until the operating system is ready.


<a name="Build"></a>
# Step 3: Build SDK and Run the sample


## 3.1 Connect the Device
- Connect the board to your network using an Ethernet cable. This step is required, as the sample depends on internet access.

## 3.2 Build the Samples
-  Start a new instance of Visual Studio 2015. Open the **iothub_csharp_client.sln** solution (/azure-iot-sdks/csharp) from your local copy of the repository.

-  In Visual Studio, from **Solution Explorer**, navigate to the **UWPSample(Universal Windows)** project.
 
-  Locate the following code in the **ConnectionStrings.cs** file:
	       
	
		public const string DeviceConnectionString = "<replace>";	


-  Replace the above placeholder with device connection string you obtained in Step 1 and save the changes.

-  Choose the right architecture (x86 or ARM, depending on your device) and set the debugging method to "Remote Machine":

-  To deploy the binaries on your device, right-click on the **UWPSample** project in the **Solution Explorer**, select **Properties** and navigate to the **Debug** tab: 

-  Type in the name of your device. Make sure the "Use authentication" box is unchecked.

-  Build the solution.


## 3.3 Run and Validate the Samples


###  3.3.1 Send Device Events to IoT Hub
-	In Visual Studio, from **Solution Explorer**, right-click the **UWPSample(Universal Windows)** project, click **Debug −> Start new instance** to build and run the sample.
 
-	See Manage IoT Hub to learn how to observe the messages IoT Hub receives from the application.

### 3.3.2 Receive messages from IoT Hub
•	See [Manage IoT Hub][lnk-manage-iot-hub] to learn how to send cloud-to-device messages to the application.


[setup-devbox-windows]: https://github.com/Azure/azure-iot-sdks/blob/master/c/doc/devbox_setup.md
[lnk-setup-iot-hub]: ../../setup_iothub.md
[lnk-manage-iot-hub]: ../../manage_iot_hub.md
