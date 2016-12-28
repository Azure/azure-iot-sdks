---
platform: windows 10
device:  dash t3
language: csharp
---

Run a simple C-Sharp sample on  DASH T3 device running Windows 10 IoT Enterprise
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

This document describes how to connect  DASH T3 device running Windows 10 IoT Enterprise with Azure IoT SDK. This multi-step process includes:
-   Configuring Azure IoT Hub
-   Registering your IoT device
-   Build and deploy Azure IoT SDK on device

<a name="Prerequisites"></a>
# Step 1: Prerequisites

You should have the following items ready before beginning the process:

-   [Prepare your development enviornment][setup-devbox-windows]
-   [Setup your IoT hub][lnk-setup-iot-hub]
-   [Provision your device and get its credentials][lnk-manage-iot-hub]
-   DASH T3 device.
-  	#####Install Visual Studio 2015 and Tools
 	To create Windows IoT Core solutions, you will need to install Visual Studio 2015. You can install any edition of Visual Studio, including the free Community edition.


<a name="PrepareDevice"></a>
# Step 2: Prepare your Device

-  Follow the instructions [here](http://www.pioneersolution.com/tablets/t3/)


<a name="Build"></a>
# Step 3: Build SDK and Run the sample


## 3.1 Connect the Device
-	Connect the device to your network using a WiFi connection. This step is required, as the sample depends on internet access.

## 3.2 Build and Run the Samples
-	Download the [Azure IoT SDK](https://github.com/Azure/azure-iot-sdks) and the sample programs and save them to your local repository.

-	Start a new instance of Visual Studio 2015.

-	Open the **iothub_csharp_client.sln** solution in the csharp\device folder in your local copy of the repository.

-	In Visual Studio, from Solution Explorer, navigate to the **samples** folder.

-	In the **DeviceClientHttpSample** project, open the **Program.cs** file.

-	Locate the following code in the file:
	
		private const string DeviceConnectionString = "<replace>";

-	Replace <replace> with the connection string for your device.

-	In **Solution Explorer**, right-click the **DeviceClientHttpSample** project, click **Debug**, and then click **Start new instance** to build and run the sample. The console displays messages as the application sends device-to-cloud messages to IoT Hub.

-	See [Manage IoT Hub](https://github.com/Azure/azure-iot-sdks/blob/d5435b940253fc90cfd38ac3ab33b63300a4942c/doc/manage_iot_hub.md) to learn how to observe the messages IoT Hub receives from the application and how to send cloud-to-device messages to the application.
