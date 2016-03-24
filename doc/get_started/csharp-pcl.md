---
platform: windows, Android, iOS
device: desktop, phone, tablet
language: csharp
---

Create a simple a simple Csharp application targeting Android/iOS/UWP under Windows
===
---

# Table of Contents

-   [Introduction](#Introduction)
-   [Step 1: Prerequisites](#Prerequisites)
-   [Step 2: Create a new application](#CreateApplication)
-   [Step 3: Send data to IotHub](#SendData)
-   [Step 4: Receive data from IotHub](#ReceiveData)

<a name="Introduction"></a>
# Introduction

**About this document**

This document describes how to build and run a simple application targeting Android/iOS/Windows Phone using the IoT SDK PCL library on the Windows platform.

<a name="Prerequisites"></a>
# Step 1: Prerequisites

You should have the following items ready before beginning the process:

-   Windows Desktop
-   Visual Studio 2015 with NuGet package manager
-	Xamarin for Windows (https://msdn.microsoft.com/en-us/library/mt613162.aspx)
-   [Setup your IoT hub][lnk-setup-iot-hub]
-   [Provision your device and get its credentials][lnk-manage-iot-hub]

<a name="CreateApplication"></a>
# Step 2: Create a new application

-   Create a solution using the following guide: https://msdn.microsoft.com/en-us/library/mt488769.aspx
-	Right click on the solution in Solution Explorer
-	Click on "Manage NuGet Packages for solution"
-	Install the "Microsoft.Azure.Devices.Client.PCL" nuget in all the projects of the solution
	![](./media/pcl_nuget.png)
-	You're ready to send/receive data to/from Microsoft Azure IotHub!

<a name="SendData"></a>
# Step 3: Send data to IotHub

-   The following code can be copy/pasted anywhere an action is to be performed, in order to send data to IotHub (for instance, in a method triggered by a button click/touch)

            string DeviceConnectionString = "<replace>"; //replace `<replace>` with the connection string for your device.
            DeviceClient deviceClient = DeviceClient.CreateFromConnectionString(DeviceConnectionString, TransportType.Http1);
            string dataBuffer = string.Format("Message from PCL: {0}_{1}", this.count, Guid.NewGuid().ToString()); //dataBuffer is the payload of the message to be sent to IotHub
            var eventMessage = new Message(Encoding.UTF8.GetBytes(dataBuffer));
            deviceClient.SendEventAsync(eventMessage);
			
-   See [Manage IoT Hub][lnk-manage-iot-hub] to learn how to observe the messages IoT Hub receives from the application and how to send cloud-to-device messages to the application.

<a name="ReceiveData"></a>
# Step 4: Receive data from IotHub

-   The following code can be copy/pasted anywhere an action is to be performed, in order to receive data from IotHub (for instance, in a background thread waking every once
in a while to check for new messages):

            var asyncTask = Task.Run(() => this.deviceClient.ReceiveAsync());
            var receivedMessage = asyncTask.Result;
            if (receivedMessage != null)
            {
                Console.WriteLine(Encoding.Default.GetString(receivedMessage.GetBytes()) + "\r\n");
                Console.WriteLine("Message received, now completing it...");
                Task.Run(() => { deviceClient.CompleteAsync(receivedMessage); });
                Console.WriteLine("Message completed.");
            }

[lnk-setup-iot-hub]: ../setup_iothub.md
[lnk-manage-iot-hub]: ../manage_iot_hub.md