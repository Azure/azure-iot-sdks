---
platform: windows
device: desktop
language: c
---

Run a simple C sample on Windows
===
---

# Table of Contents

-   [Introduction](#Introduction)
-   [Step 1: Prerequisites](#Step-1-Prerequisites)
-   [Step 2: Build and Run the Sample](#Step-2-Build)

<a name="Introduction"></a>
# Introduction

**About this document**

This document describes how to build and run sample applications on the Windows platform. This multi-step process includes:
-   Configuring Azure IoT Hub
-   Registering your IoT device
-   Build and deploy Azure IoT SDK on device

<a name="Step-1-Prerequisites"></a>
# Step 1: Prerequisites

You should have the following items ready before beginning the process:

-   [Prepare your development environment][devbox-setup]
-   Computer with Git client installed and access to the
    [azure-iot-sdks](https://github.com/Azure/azure-iot-sdks) GitHub public repository.
-   [Setup your IoT hub][lnk-setup-iot-hub]
-   [Provision your device and get its credentials][lnk-manage-iot-hub]

<a name="Step-2-Build"></a>
# Step 2: Build and Run the sample

1.  Start a new instance of Visual Studio 2015. Open the **azure_iot_sdks.sln** solution in the **cmake** folder in your home directory.

2.  In Visual Studio, in **Solution Explorer**, navigate to project based on your choice of protocol:

    **For AMQP protocol:**
    
    Navigate to **simplesample_amqp** project and open the **simplesample_amqp.c** file.

    **For HTTP protocol:**
    
    Navigate to **simplesample_http** project and open the **simplesample_http.c** file.

    **For MQTT protocol:**
    
    Navigate to **simplesample_mqtt** project and open the **simplesample_mqtt.c** file.

    ![cfile\_edit](media/navigate_simplesampleamqp.png)

3.  Locate the following code in the file:

      ```
      static const char* connectionString = "[device connection string]";
      ```

4.  Replace "[device connection string]" with the device connection string you noted [earlier](#Step-1-Prerequisites) and save the changes:

       ```
       static const char* connectionString = "HostName=..."
       ```
       
5.  In **Solution Explorer**, right-click the project updated, click **Debug**, and then click **Start new instance** to build and run the sample. 

    ![project\_debug](media/project_amqp_debug.png)
    
6.  The console displays messages as the application sends device-to-cloud messages to IoT Hub.

7.   See [Manage IoT Hub][lnk-manage-iot-hub] to learn how to observe the messages IoT Hub receives from the application and how to send cloud-to-device messages to the application.

[lnk-setup-iot-hub]: ../setup_iothub.md
[lnk-manage-iot-hub]: ../manage_iot_hub.md
[devbox-setup]: https://github.com/Azure/azure-iot-sdks/blob/master/c/doc/devbox_setup.md
