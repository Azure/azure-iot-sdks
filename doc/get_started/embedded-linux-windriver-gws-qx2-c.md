---
platform: embedded linux
device: ecs qws-qx2
language: c
---

Run a simple C sample on ECS QWS-QX2 device running Embedded LINUX distribution built with Wind River
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

This document describes how to connect ECS QWS-QX2 device running customised embedded LINUX distribution built with Wind River with Azure IoT SDK. This multi-step process includes:
-   Configuring Azure IoT Hub
-   Registering your IoT device
-   Build and deploy Azure IoT SDK on device

<a name="Prerequisites"></a>
# Step 1: Prerequisites

You should have the following items ready before beginning the process:

-   [Prepare your development environment][setup-devbox-linux]
-   [Setup your IoT hub][lnk-setup-iot-hub]
-   [Provision your device and get its credentials][lnk-manage-iot-hub]
-   ECS QWS-QX2 device.
-   LINUX VM-Instance.
-   Cross compiler settings from Wind River.

<a name="PrepareDevice"></a>
# Step 2: Prepare your Device
-   Connect the QWS-QX2 board to your desktop machine using the USB cable and open a terminal emulator program such as Tera Term on your desktop machine and switch ON the device. You have to create a new Serial port connection from Setup->Serial Port and set the parameters:
    -   Port number where you have connected the device to PC.
    -   Baud Rate = 115200
    -   Data = 8 bits
    -   Parity = noneStop = 1 bit
    -   Flow Control = none
    -   When prompted, log in with username root, and password root. The root account is necessary in order to install some libraries required by the device SDK.


<a name="Build"></a>
# Step 3: Build and Run the sample

<a name="Load"></a>
## 3.1 Build SDK and sample

-   Download the Microsoft Azure IoT Device SDK for C to your LINUX development VM by issuing the following command:

        git clone --recursive https://github.com/Azure/azure-iot-sdks.git

-   Set up the cross compiler setting.

-   Edit the following file using any text editor of your choice:

        azure-iot-sdks/c/iothub_client/samples/iothub_client_sample_http/iothub_client_sample_http.c

-   Find the following place holder for IoT connection string:

        static const char* connectionString = "[device connection string]";

-   Replace the above placeholder with device connection string you obtained in [Step 1](#Prerequisites) and save the changes.

-   Build the SDK using following command.

        ./azure-iot-sdks/c/build_all/linux/build.sh --skip-unittests --no-amqp --no-mqtt

## 3.2 Send Device Events to IoT Hub:

-   Run the sample by issuing following command:

        ~/azure-iot-sdks/c/cmake/iotsdk_linux/iothub_client/samples/iothub_client_sample_http/iothub_client_sample_http

-   See [Manage IoT Hub][lnk-manage-iot-hub] to learn how to observe the messages IoT Hub receives from the application.

## 3.3 Receive messages from IoT Hub

-   See [Manage IoT Hub][lnk-manage-iot-hub] to learn how to send cloud-to-device messages to the application.

[setup-devbox-linux]: https://github.com/Azure/azure-iot-sdks/blob/master/c/doc/devbox_setup.md
[lnk-setup-iot-hub]: ../../setup_iothub.md
[lnk-manage-iot-hub]: ../../manage_iot_hub.md
