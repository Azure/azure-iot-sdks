How to Certify IoT devices running Windows .NET Micro Framework with Azure IoT SDK 
===
---


# Table of Contents

-   [Introduction](#Introduction)
-   [Step 1: Sign Up To Azure IoT Hub](#Step_1:_Sign_Up)
-   [Step 2: Register Device](#Step_2:_Register)
-   [Step 3: Build and Validate the Sample using C# Client Libraries](#Step_3:_Build_and_Validate)
    -   [3.1 Connect the Device](#Step_3_1:_Connect)
    -   [3.2 Build the Samples](#Step_3_2:_Build)
    -   [3.3 Run and Validate the Samples](#Step_3_3:_Run)
-   [Step 4: Package and Share](#Step_4:_Package_Share)
    -   [4.1 Package build logs and sample test results](#Step_4_1:_Package)
    -   [4.2 Share package with Engineering Support](#Step_4_2:_Share)
    -   [4.3 Next steps](#Step_4_3:_Next)
-   [Step 5: Troubleshooting](#Step_5:_Troubleshooting)

<a name="Introduction"></a>
# Introduction

**About this document**

This document provides step by step guidance to IoT hardware publishers on how to certify an IoT enabled hardware with Azure IoT SDK. This multi-step process includes:

-   Configuring Azure IoT Hub 
-   Registering your IoT device
-   Build and deploy Azure IoT SDK on device
-   Packaging and sharing the logs  

**Prepare**

Before executing any of the steps below, read through each process, step
by step to ensure end to end understanding.

You should have the following items ready before beginning the process:

-   Computer with GitHub installed and access to the [azure-iot-sdks](https://github.com/Azure/azure-iot-sdks) GitHub public repository.
-   Install Visual Studio 2015 and Tools. You can install any edition of Visual Studio, including the free Community edition.

    After installing Visual Studio, goto **Tools** menu and click **Extensions and Updates**. Search for 'netmf' and install .NET Micro Framework SDK for version running on device.
    
    ![Menubar->tools](images/vs_install_tools_menu.png)
    
    
    ![Install\_required\_tools](images/vs_install_tools.png)
-   Required hardware running Windows .NET Micro Framework to certify


<a name="Step_1:_Sign_Up"></a>
# Step 1: Sign Up To Azure IoT Hub

Follow the instructions [here](https://account.windowsazure.com/signup?offer=ms-azr-0044p) on how to sign up to the Azure IoT Hub service.

As part of the sign up process, you will receive the connection string.

-   **IoT Hub Connection String**: An example of IoT Hub Connection String is as below:

        HostName=[YourIoTHubName];SharedAccessKeyName=[YourAccessKeyName];SharedAccessKey=[YourAccessKey]

<a name="Step_2:_Register"></a>
# Step 2: Register Device

In this section, you will register your device using DeviceExplorer. The DeviceExplorer is a Windows application that interfaces with Azure IoT Hub and can perform the following operations:

-   Device management
    -   Create new devices
    -   List existing devices and expose device properties stored on Device Hub
    -   Provides ability to update device keys
    -   Provides ability to delete a device
-   Monitoring events from your device
-   Sending messages to your device

To run DeviceExplorer tool, use following configuration string as described in [Step1](#Step_1:_Sign_Up):

-   IoT Hub Connection String

**Steps:**

1.  Click [here](https://github.com/Azure/azure-iot-sdks/blob/master/tools/DeviceExplorer/doc/how_to_use_device_explorer.md) to download and install DeviceExplorer.

2.  Add connection information under the **Configuration** tab and click the **Update** button.

3.  Create and register the device with your IoT Hub using instructions as below.

    a.  Click the **Management** tab.    
    
    b.  Your registered devices will be visible in the list. In case your device is not there in the list, click **Refresh** button. If this is your first time, then you shouldn't retrieve anything.
       
    c.  Click **Create** button to create a device ID and key. 
    
    d.  Once created successfully, device will be listed in DeviceExplorer. 
    
    e.  Right click the device and from context menu select "**Copy connection string for selected device**".
    
    f.  Save this information in Notepad. You will need this information in later steps.

***Not running Windows on your PC?*** - Please follow the instructions [here](<https://github.com/Azure/azure-iot-sdks/blob/master/doc/manage_iot_hub.md>) to provision your device and get its credentials.

<a name="Step_3:_Build_and_Validate"></a>
# Step 3: Build and Validate the Sample using C# Client Libraries 

This section walks you through building, deploying and validating the IoT Client SDK on your device running Windows .NET Micro Framework operating system. You will install the necessary prerequisites on your device. Once done, you will build and deploy the IoT Client SDK, and validate the sample tests required for IoT certification with the Azure IoT SDK.

<a name="Step_3_1:_Connect"></a>
## 3.1 Connect the Device

1.  Connect the board to your network using an Ethernet cable. This step is required, as the sample depends on internet access.

2.  Plug the device into your computer.

<a name="Step_3_2:_Build"></a>
## 3.2  Build the Samples

1. Clone [Azure IoT SDK](https://github.com/Azure/azure-iot-sdks.git) repository to your machine.

2.  Start a new instance of Visual Studio 2015. Open the **iothub_csharp_netmf_client.sln** solution (/azure-iot-sdks/csharp) from your local copy of the repository.

3.  In Visual Studio, from **Solution Explorer**, navigate to the **NetMFDeviceClientHttpSample_<.net framework version>** project.

	![Navigation\_terminal](images/navigation.png)


4.  Locate the following code in the **Program.cs** file:

        private const string DeviceConnectionString = "<replace>";

5.  Replace `<replace>` with the connection string for your device and **Save** the changes. You can get the connection string from DeviceExplorer as explained in [Step 2](#Step_2:_Register).
   
6.  To build and deploy the binaries on your device, right-click on the project in **Solution Explorer**, select **Properties** and navigate to the **.NET Micro Framework** tab:

    ![VisualStudio\_Properties\_Debug](images/vs_properties_netmf.png)

    Select the **Transport** and **Device** which you have connected.

7.  Build the solution.


<a name="Step_3_3:_Run"></a>
## 3.3 Run and Validate the Samples
    
In this section you will run the Azure IoT client SDK samples to validate the
communication between your device and Azure IoT Hub. You will send the messages to the Azure IoT Hub service and validate that IoT Hub has successfully receive the data. You will also monitor any messages sent from the Azure IoT Hub to client.

***Note:*** *Take screenshots of all the operations you will perform in this
section. These will be needed in [Step 4](#Step_4_2:_Share).*

### 3.3.1 Send Device Events to IoT Hub

1.  Launch the DeviceExplorer as explained in [Step 2](#Step_2:_Register) and navigate to **Data** tab. Select the device name you created from the drop-down list of device IDs and click **Monitor** button.

    ![DeviceExplorer\_Monitor](images/device_explorer_monitor.png)

2.  DeviceExplorer is now monitoring data sent from the selected device to the IoT Hub.
     

3.  In Visual Studio, from **Solution Explorer**, right-click the project, click **Debug &minus;&gt; Start new instance** to build and run the sample. 

       
4. You should be able to see the events received in the DeviceExplorer's data tab.

    ![DeviceExplorer\_Events\_Received](images/device_explorer_message_received.png)

### 3.3.2 Receive messages from IoT Hub

1.  To verify that you can send messages from the IoT Hub to your device, go to the **Messages to Device** tab in DeviceExplorer.

2.  Select the device you created using Device ID drop down.

3.  Add some text to the Message field, then click Send.

    ![DeviceExplorer\_Notification\_Send](images/device_explorer_notification_send.png)
    
    ![DeviceExplorer\_Notification\_Send](images/device_explorer_notification_send_view.png)

4.  You should be able to see the message received in the Output window.
    
<a name="Step_4:_Package_Share"></a>
# Step 4: Package and Share

<a name="Step_4_1:_Package"></a>
## 4.1 Package build logs and sample test results
  
Package the following artifacts from your device:

1.  Build logs from section 3.2.
2. All the screenshots that are shown above in "**Send Device Events to IoT Hub**" section.
3.  All the screenshots that are shown above in "**Receive messages from IoT Hub**" section.
4.  Send us clear instructions of how to run this sample with your hardware (explicitly highlighting the new steps for customers). Please use the template available [here](<https://github.com/Azure/azure-iot-sdks/blob/master/doc/iotcertification/templates/template-windows-netmf-csharp.md>) to create your device-specific instructions.

    As a guideline on how the instructions should look please refer the examples published on GitHub repository [here](<https://github.com/Azure/azure-iot-sdks/tree/master/doc/get_started>).

<a name="Step_4_2:_Share"></a>
## 4.2 Share package with the Azure IoT Certification Team

Send the package in email to <iotcert@microsoft.com>.

<a name="Step_4_3:_Next"></a>
## 4.3 Next steps

Once you shared the documents with us, we will contact you in the following 48 to 72 business hours with next steps.

<a name="Step_5:_Troubleshooting"></a>
# Step 5: Troubleshooting

Please contact engineering support on <iotcert@microsoft.com> for help with  troubleshooting.
