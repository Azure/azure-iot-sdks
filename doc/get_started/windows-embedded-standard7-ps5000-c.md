---
platform: windows embedded standard 7
device: pc5000
language: c
---

Run a simple C sample on Industrial PC PS5000 Series device running Windows Embedded Standard 7
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

This document describes how to connect Industrial PC PS5000 Series device running Windows Embedded Standard 7 with Azure IoT SDK. This multi-step process includes:
-   Configuring Azure IoT Hub
-   Registering your IoT device
-   Build and deploy Azure IoT SDK on device

<a name="Prerequisites"></a>
# Step 1: Prerequisites

You should have the following items ready before beginning the process:

-   [Prepare your development environment][setup-devbox-windows]
-   [Setup your IoT hub][lnk-setup-iot-hub]
-   [Provision your device and get its credentials][lnk-manage-iot-hub]
-   [Industrial PC PS5000 Series device](https://www.proface.com/en/product/ipc/ps5000/top)

<a name="PrepareDevice"></a>
# Step 2: Prepare your Device

-   Connect the power adapter, USB Keyborad/Mouse with PS5000.
-   Power on your PS5000 and connect to the Internet.
-   Setup Windows.
-   Update Windows and download The Visual C++ Redistributable for Visual Studio 2015.

<a name="Build"></a>
# Step 3: Build SDK and Run the sample

-   Download the [Azure IoT SDK](https://github.com/Azure/azure-iot-sdk-c) and the sample programs and save them to your local repository.
-   Start a new instance of Visual Studio 2015. 
  -   For AMQP protocol:
     -  In Visual Studio, Open the **iothub_client_sample_amqp.sln** solution in the **iothub_client\sample\iothub_client_sample_amqp\windows** folder in your home directory.
     -  Navigate to **iothub_client_sample_amqp** project and open the **iothub_client_sample_amqp.c** file.
  -   For HTTP protocol:
     -  In Visual Studio, Open the **iothub_client_sample_http.sln** solution in the **iothub_client\sample\iothub_client_sample_http\windows** folder in your home directory.
     -  Navigate to **iothub_client_sample_http** project and open the **iothub_client_sample_http.c** file.
  -   For MQTT protocol:
     -  In Visual Studio, Open the **iothub_client_sample_mqtt.sln** solution in the **iothub_client\sample\iothub_client_sample_mqtt\windows** folder in your home directory.
     -  Navigate to **iothub_client_sample_mqtt** project and open the **iothub_client_sample_mqtt.c** file.

-   Locate the following code in the file:

        static const char* connectionString = "[device connection string]";

-   Replace the above placeholder with device connection string you obtained in [Step 1](#Step-1:-Prerequisites) and save the changes.

-   See [Manage IoT Hub][lnk-manage-iot-hub] to learn how to observe the messages IoT Hub receives from the application.

-   In **Solution Explorer**, right-click the **iothub_client_sample_amqp** project, click **Debug**, and then click **Start new instance** to build and run the sample. The console displays messages as the application sends device-to-cloud messages to IoT Hub.
  -  You can buid and run AMQP/HTTP/MQTT sample projects in the same way.

-   Use the **DeviceExplorer** utility to observe the messages IoT Hub receives from the **Device Client AMQP/HTTP/MQTT Sample** application.

-   Refer "Monitor device-to-cloud events" in [DeviceExplorer Usage document](https://github.com/Azure/azure-iot-sdks-preview/blob/master/tools/DeviceExplorer/doc/how_to_use_device_explorer.md) to see the data your device is sending.
-   Refer "Send cloud-to-device messages" in [DeviceExplorer Usage document](https://github.com/Azure/azure-iot-sdks-preview/blob/master/tools/DeviceExplorer/doc/how_to_use_device_explorer.md) for instructions on sending messages to device.

-   See [Manage IoT Hub][lnk-manage-iot-hub] to learn how to send cloud-to-device messages to the application.

[setup-devbox-windows]: https://github.com/Azure/azure-iot-sdks/blob/master/c/doc/devbox_setup.md
[lnk-setup-iot-hub]: https://github.com/Azure/azure-iot-sdks/blob/master/doc/setup_iothub.md
[lnk-manage-iot-hub]: https://github.com/Azure/azure-iot-sdks/blob/master/doc/manage_iot_hub.md
