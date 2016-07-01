---
platform: linux
device: vho1-x
language: c
---

Run a simple C sample on VHo1-X running Linux Beaglebone 3.2.0
===
---

# Table of Contents

-   [Introduction](#Introduction)
-   [Step 1: Prerequisites](#Prerequisites)
-   [Step 2: Prepare your Device](#PrepareDevice)
-   [Step 3: Setup the development environment](#Setup)
-   [Step 4: Build and Run the Sample](#Build)

<a name="Introduction"></a>
# Introduction

**About this document**

This document describes how to connect **VHo1-X** device running **Linux** with Azure IoT SDK. This multi-step process includes:
-   Configuring Azure IoT Hub
-   Registering your IoT device
-   Build and deploy Azure IoT SDK on device

<a name="Prerequisites"></a>
# Step 1: Prerequisites

You should have the following items ready before beginning the process:
-   Computer with a Git client installed so that you can access the azure-iot-sdks code on GitHub.
-   VHo1-X Device.
-   Ubuntu x86 machine (for cross compiling) 
-   [Setup your IoT hub][lnk-setup-iot-hub]
-   [Provision your device and get its credentials][lnk-manage-iot-hub]

<a name="PrepareDevice"></a>
# Step 2: Prepare your Device
-  Connect the VHo1-X using the twisted pair for SDK download and data transfer.

<a name="Setup"></a>
# Step 3: Setup the development environment

This section shows you how to set up a development environment for the Azure IoT device SDK for C on VHo1-X.

## Install dependencies under /usr/lib

-   Download curl-7.49.1.tar.gz and openssl-1.0.2h.tar.gz for cross compiling.

-  Download the Microsoft Azure IoT Device SDK by issuing following command:

        git clone --recursive https://github.com/Azure/azure-iot-sdks.git

-   Navigate to the folder **c/build_all/linux** in your local copy of the repository.

<a name="Build"></a>
# Step 4: Build and Run the sample

## 4.1 Setting up cmake to cross compile

-   Switch to the SDK directory tree.

    ```
    cd ~/azure-iot-sdks/c/build_all/linux
    ```

-   Using the text editor of your choice, create a new file in this directory and call it toolchain-rpi.cmake and add following lines to it:

    ```
    INCLUDE(CMakeForceCompiler)

    SET(CMAKE_SYSTEM_NAME Linux)     # this one is important
    SET(CMAKE_SYSTEM_VERSION 1)     # this one not so much

    # this is the location of the arm toolchain targeting the beaglebone
    SET(CMAKE_C_COMPILER ~/cross_tools/linux-devkit/bin/arm-arago-linux-gnueabi-gcc)

    # this is the file system root of the target
    SET(CMAKE_FIND_ROOT_PATH ~/cross_tools/linux-devkit/)

    # search for programs in the build host directories
    SET(CMAKE_FIND_ROOT_PATH_MODE_PROGRAM NEVER)

    # for libraries and headers in the target directories
    SET(CMAKE_FIND_ROOT_PATH_MODE_LIBRARY ONLY)
    SET(CMAKE_FIND_ROOT_PATH_MODE_INCLUDE ONLY)
    ```

-   Save the toolchain file. Your cross compilation environment is now complete.

## 4.2 Build the sample

-   Edit the following file using any text editor of your choice:
        azure-iot-sdks/c/iothub_client/samples/iothub_client_sample_mqtt/iothub_client_sample_mqtt.c

-   Find the following place holder for IoT connection string:

        static const char* connectionString = "[device connection string]";

-   Replace the above placeholder with device connection string you obtained in [Step 1](#Prerequisites) and save the changes.

-   Add certificates define.

    ```
    const char certificates[];
    ```

    ```
    if (IoTHubClient_LL_SetOption(iotHubClientHandle, "TrustedCerts", certificates) != IOTHUB_CLIENT_OK)
    {
        printf("failure to set option \"TrustedCerts\"\r\n");
    }
    ```

-   Conditional compile on the above code to pass certificate.

-   Because cross compiling toolchain gcc version is 4.5.3, **/c/serializer/inc/agenttypesystem.h** code must be modified to pass compiling.

    ```
    Del Line33:typedef struct AGENT_DATA_TYPE_TAG AGENT_DATA_TYPE;
    Del Line540:AGENT_DATA_TYPE* value;
    Add Line540:struct AGENT_DATA_TYPE_TAG * value;
    ```

-   Build the SDK using following command.

    ```
    sudo ./azure-iot-sdks/c/build_all/linux/build.sh --toolchain-file toolchain-rpi.cmake  --no-amqp --no-http --skip-unittests  -cl -DMBED_BUILD_TIMESTAMP | tee LogFile.txt
    ```

<a name="deploy"/>
## 4.3 Deploy the sample

-   Transfer following libraries to target board directory /usr/lib.
    ```
    libcrypto.so.1.0.0
    libssl.so.1.0.0
    libcurl.so.4.4.0 
    ```
-   Transfer the sample executable iothub_client_sample_mqtt to target board directory /usr/local/bin.

***Note:****Because sample code is compiled with 'TrustedCerts', there is no need to install certificates.*

-   Before run the sample ,the device must clock calibrating ,use following command to calibrate.

        ntpdate [NtpServer]

<a name="run"/>
## 4.4 Send Device Events to IoT Hub:

        /usr/local/bin/iothub_client_sample_mqtt

-   See [Manage IoT Hub][lnk-manage-iot-hub] to learn how to observe the messages IoT Hub receives from the application.

## 4.5 Receive messages from IoT Hub

-   See [Manage IoT Hub][lnk-manage-iot-hub] to learn how to send cloud-to-device messages to the application.


[setup-devbox-linux]: ../../c/doc/devbox_setup.md
[lnk-setup-iot-hub]: ../setup_iothub.md
[lnk-manage-iot-hub]: ../manage_iot_hub.md