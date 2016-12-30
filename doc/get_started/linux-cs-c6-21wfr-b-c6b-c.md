---
platform: linux 2.6.38.8 embedded
device: cs-c6-21wfr-b
language: c
---

Run a simple C sample on CS-C6-21WFR-B device running Linux 2.6.38.8 Embedded
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

This document describes how to connect CS-C6-21WFR-B device running Linux 2.6.38.8 Embeded with Azure IoT SDK. This multi-step process includes:
-   Configuring Azure IoT Hub
-   Registering your IoT device
-   Build and deploy Azure IoT SDK on device

<a name="Prerequisites"></a>
# Step 1: Prerequisites

You should have the following items ready before beginning the process:

-   [Prepare your development environment][setup-devbox-linux]
-   [Setup your IoT hub][lnk-setup-iot-hub]
-   [Provision your device and get its credentials][lnk-manage-iot-hub]
-   CS-C6-21WFR-B device.

<a name="PrepareDevice"></a>
# Step 2: Prepare your Device
-   Install Ubuntu on your device.
-   Install cross-compiler of Ambarella A5S on Ubuntu 
-   Install cross-compiled uuid/openssl 1.0.1u/curl 7.43.0 libs and header files on Ubuntu

<a name="Build"></a>
# Step 3: Build and Run the sample

<a name="Load"></a>
## 3.1 Build SDK and sample

-   Open a PuTTY session and connect to the device.

-   Install the prerequisite packages for the Microsoft Azure IoT Device SDK for C by issuing the following commands from the command line on your board:

        sudo apt-get update

        sudo apt-get install -y curl libcurl4-openssl-dev uuid-dev uuid g++ make cmake git unzip openjdk-7-jre

-   Download the Microsoft Azure IoT Device SDK for C to the board by issuing the following command on the board::

        git clone --recursive https://github.com/Azure/azure-iot-sdks.git

-   Edit the following file using any text editor of your choice:

    **azure-iot-sdks/c/build_all/linux/toolchain.cmake :**
    
        INCLUDE(CMakeForceCompiler)
        SET(CMAKE_SYSTEM_NAME Linux)     # this one is important
        SET(CMAKE_SYSTEM_VERSION 1)     # this one not so much
        # this is the location of the amd64 toolchain targeting the Raspberry Pi
        SET(CMAKE_C_COMPILER /opt/arm-2011.09/bin/arm-none-linux-gnueabi-gcc)
        # this is the file system root of the target
        SET(CMAKE_FIND_ROOT_PATH /opt/arm-2011.09/arm-none-linux-gnueabi)
        # search for programs in the build host directories
        SET(CMAKE_FIND_ROOT_PATH_MODE_PROGRAM NEVER)
        # for libraries and headers in the target directories
        SET(CMAKE_FIND_ROOT_PATH_MODE_LIBRARY ONLY)
        SET(CMAKE_FIND_ROOT_PATH_MODE_INCLUDE ONLY)
        include_directories(/opt/arm-2011.09/arm-none-linux-gnueabi/libc/usr/include/)
    
    **For MQTT protocol:**

        azure-iot-sdks/c/iothub_client/samples/iothub_client_sample_amqp/iothub_client_sample_amqp.c

-   Find the following place holder for IoT connection string:

        static const char* connectionString = "[device connection string]";

-   Replace the above placeholder with device connection string you obtained in [Step 1](#Prerequisites) and save the changes.

-   Build the SDK using following command.

        sudo ./azure-iot-sdks/c/build_all/linux/build.sh--toolchain-file toolchain.cmake --skip-inittests -cl --sysroot=/opt/arm-2011.09/arm-none-linux-gnueabi/libc -cl -ldl -cl -lrt --run-e2e-tests | tee LogFile.txt

## 3.2 Send Device Events to IoT Hub:

-   Run the sample by issuing following command:

    **For MQTT protocol:**

        ~/azure-iot-sdks/c/cmake/iotsdk_linux/iothub_client/samples/iothub_client_sample_mqtt/iothub_client_sample_mqtt

-   See [Manage IoT Hub][lnk-manage-iot-hub] to learn how to observe the messages IoT Hub receives from the application.

## 3.3 Receive messages from IoT Hub

-   See [Manage IoT Hub][lnk-manage-iot-hub] to learn how to send cloud-to-device messages to the application.

<a name="tips"></a>
# Tips

-   If you just want to build the serializer samples, run the following commands:

        cd ./c/serializer/build/linux
        make -f makefile.linux all


[setup-devbox-linux]: https://github.com/Azure/azure-iot-sdks/blob/master/c/doc/devbox_setup.md
[lnk-setup-iot-hub]: ../../setup_iothub.md
[lnk-manage-iot-hub]: ../../manage_iot_hub.md