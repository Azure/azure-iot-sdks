---
platform: debian gnu linux 8
device: openblocks iot vx1
language: c
---

Run a simple C sample on OpenBlocks IoT VX1 device running Debian GNU Linux 8
===
---

# Table of Contents

-   [Introduction](#Introduction)
-   [Step 1: Prerequisites](#Prerequisites)
-   [Step 2: Prepare your Device](#PrepareDevice)
-   [Step 3: Build and Run the Sample](#Build)
-   [Tips](#tips)

<a name="Introduction"></a>
# Introduction

**About this document**

This document describes how to connect OpenBlocks IoT BX1G device running Debian GNU Linux 8 with Azure IoT SDK. This multi-step process includes:
-   Configuring Azure IoT Hub
-   Registering your IoT device
-   Build and deploy Azure IoT SDK on device

<a name="Prerequisites"></a>
# Step 1: Prerequisites

You should have the following items ready before beginning the process:

-   [Prepare your development environment][setup-devbox-linux]
-   [Setup your IoT hub][lnk-setup-iot-hub]
-   [Provision your device and get its credentials][lnk-manage-iot-hub]
-   OpenBlocks IoT VX1 device.

<a name="PrepareDevice"></a>
# Step 2: Prepare your Device

-   The build process requiers 9.6GB memory space at least, so, create a swap area temporary on external USB memory at before the building.
-   Attach the 32GB (or more) USB memory that formated with extfs to USB connecter, then following commands from the command line on your board: 

        mount /dev/sda1 /mnt

        dd if=/dev/zero of=/mnt/tmp_swapfile bs=1M count=16384

        mkswap /mnt/tmp_swapfile

        swapon /mnt/tmp_swapfile

-   If possible, is better to use NFS instead of the USB memory file system is faster.

<a name="Build"></a>
# Step 3: Build and Run the sample

<a name="Load"></a>
## 3.1 Build SDK and sample

-   Open a PuTTY session and connect to the device.

-   Install the prerequisite packages for the Microsoft Azure IoT Device SDK for C by issuing the following commands from the command line on your board:

        apt-get update

        apt-get install -y uuid-dev libcurl4-openssl-dev libssl-dev cmake

-   Download the Microsoft Azure IoT Device SDK for C to the board by issuing the following command on the board::

        git clone --recursive https://github.com/Azure/azure-iot-sdks.git

-   Edit the following file using any text editor of your choice:

    **For AMQP protocol:**

        vi azure-iot-sdks/c/iothub_client/samples/iothub_client_sample_amqp/iothub_client_sample_amqp.c

    **For HTTP protocol:**
 
        vi azure-iot-sdks/c/iothub_client/samples/iothub_client_sample_http/iothub_client_sample_http.c

    **For MQTT protocol:**

        vi azure-iot-sdks/c/iothub_client/samples/iothub_client_sample_mqtt/iothub_client_sample_mqtt.c

-   Find the following place holder for IoT connection string:

        static const char* connectionString = "[device connection string]";

-   Replace the above placeholder with device connection string you obtained in [Step 1](#Prerequisites) and save the changes.

-   Build the SDK using following command.

        ./azure-iot-sdks/c/build_all/linux/build.sh

## 3.2 Send Device Events to IoT Hub:

-   Run the sample by issuing following command:

    **If using AMQP protocol:**

        ~/cmake/iothub_client/samples/iothub_client_sample_amqp/linux/iothub_client_sample_amqp

    **If using HTTP protocol:**

        ~/cmake/iothub_client/samples/iothub_client_sample_http/linux/iothub_client_sample_http

    **If using MQTT protocol:**

        ~/cmake/iothub_client/samples/iothub_client_sample_mqtt/linux/iothub_client_sample_mqtt

-   See [Manage IoT Hub][lnk-manage-iot-hub] to learn how to observe the messages IoT Hub receives from the application.

## 3.3 Receive messages from IoT Hub

-   See [Manage IoT Hub][lnk-manage-iot-hub] to learn how to send cloud-to-device messages to the application.

<a name="tips"></a>
# Tips

- If you just want to build the serializer samples, run the following commands:

  ```
  cd ./c/serializer/build/linux
  make -f makefile.linux all
  ```

[setup-devbox-linux]: https://github.com/Azure/azure-iot-sdks/blob/master/c/doc/devbox_setup.md
[lnk-setup-iot-hub]: ../setup_iothub.md
[lnk-manage-iot-hub]: ../manage_iot_hub.md
