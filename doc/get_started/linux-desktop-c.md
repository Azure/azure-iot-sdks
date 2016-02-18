---
platform: linux
device: desktop
language: c
---

Run a simple C sample on desktop Linux
===
---

# Table of Contents

-   [Introduction](#Introduction)
-   [Step 1: Prerequisites](#Step-1-Prerequisites)
-   [Step 2: Build and Run the Sample](#Step-2-Build)

<a name="Introduction"></a>
# Introduction

**About this document**

This document describes how to build and run the **simplesample_amqp** application on a desktop Linux platform. This multi-step process includes:
-   Configuring Azure IoT Hub
-   Registering your IoT device
-   Build and deploy Azure IoT SDK on device

<a name="Step-1-Prerequisites"></a>
# Step 1: Prerequisites

You should have the following items ready before beginning the process:

-   [Prepare your development environment][setup-devbox-linux]
-   [Setup your IoT hub][lnk-setup-iot-hub]
-   [Provision your device and get its credentials][lnk-manage-iot-hub]

<a name="Step-2-Build"></a>
# Step 2: Build and Run the sample

1.   Open the file **c/serializer/samples/simplesample_amqp/simplesample_amqp.c** in a text editor.

2.   Locate the following code in the file:
      ```
      static const char* connectionString = "[device connection string]";
      ```
3.   Replace "[device connection string]" with the device connection string you noted [earlier](#Step-1-Prerequisites). Save the changes.

5.   Save your changes and build the samples. To build your sample you can run the the build.sh script in the **c/build_all/linux** directory.

6.   Run the **~/cmake/serializer/samples/simplesample_amqp/simplesample_amqp** sample application.

7.   See [Manage IoT Hub][lnk-manage-iot-hub] to learn how to observe the messages IoT Hub receives from the **simplesample_amqp** application and how to send cloud-to-device messages to the **simplesample_amqp** application.

[setup-devbox-linux]: https://github.com/Azure/azure-iot-sdks/blob/master/c/doc/devbox_setup.md

[lnk-setup-iot-hub]: ../setup_iothub.md
[lnk-manage-iot-hub]: ../manage_iot_hub.md