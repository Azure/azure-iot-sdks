---
platform: windows 10
device: sbc-a80-enuc
language: c
---

Run a simple C sample on SBC-A80-eNUC device running Windows 10
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

This document describes how to connect **Seco SBC-A80-eNUC** device running **Windows 10** with Azure IoT SDK. This multi-step process includes:
-   Configuring Azure IoT Hub
-   Registering your IoT device
-   Build and deploy Azure IoT SDK on device

<a name="Prerequisites"></a>
# Step 1: Prerequisites

You should have the following items ready before beginning the process:

-   [Prepare your development environment][setup-devbox-windows]
-   [Setup your IoT hub][lnk-setup-iot-hub]
-   [Provision your device and get its credentials][lnk-manage-iot-hub]
-   Seco SBC-A80-eNUC device.
-   Visual Studio 2015 (Any edition of Visual Studio is suitable for the test, including the free Community edition.)

<a name="PrepareDevice"></a>
# Step 2: Prepare your Device

-   Install Windows 10 on your device, [device website](http://www.seco.com/prods/it/category/single-board-computer/sbc-a80-enuc.html)

<a name="Build"></a>
# Step 3: Build SDK and Run the sample

-   Download the [Azure IoT SDK](https://github.com/Azure/azure-iot-sdks) and the sample programs and save them to your local repository.

-   Start a new instance of Visual Studio 2015.

-   Edit the following file using any text editor of your choice:
  
    **For AMQP protocol:**

        azure-iot-sdks/c/iothub_client/samples/iothub_client_sample_amqp/iothub_client_sample_amqp.c
  
    **For HTTP protocol:**

        azure-iot-sdks/c/iothub_client/samples/iothub_client_sample_http/iothub_client_sample_http.c
	
    **For MQTT protocol:**
		
        azure-iot-sdks/c/iothub_client/samples/iothub_client_sample_mqtt/iothub_client_sample_mqtt.c

-   Find the following place holder for IoT connection string:

        static const char* connectionString = "[device connection string]";

-   Replace the above placeholder with device connection string you obtained in [Step 1](#Prerequisites) and save the changes.

-   See [Manage IoT Hub][lnk-manage-iot-hub] to learn how to observe the messages IoT Hub receives from the application.

-   Open the solution corresponding to the test to perform

    **For AMQP protocol:**

        /c/iothub_client/samples/iothub_client_sample_amqp/windows/iothub_client_sample_amqp.sln
  
    **For HTTP protocol:**

        /c/iothub_client/samples/iothub_client_sample_http/windows/iothub_client_sample_http.sln
	
    **For MQTT protocol:**
		
        /c/iothub_client/samples/iothub_client_sample_mqtt/windows/iothub_client_sample_mqtt.sln

-   See [Manage IoT Hub][lnk-manage-iot-hub] to learn how to send cloud-to-device messages to the application.

[setup-devbox-windows]: https://github.com/Azure/azure-iot-sdks/blob/master/c/doc/devbox_setup.md
[lnk-setup-iot-hub]: ../setup_iothub.md
[lnk-manage-iot-hub]: ../manage_iot_hub.md
