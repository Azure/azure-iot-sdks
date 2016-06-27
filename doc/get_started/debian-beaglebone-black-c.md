---
platform: debian
device: beaglebone-black
language: c
---

Run a simple C sample on BeagleBone Black device running Debian
===
---

# Table of Contents

-   [Introduction](#Introduction)
-   [Step 1: Prerequisites](#Step-1-Prerequisites)
-   [Step 2: Prepare your Device](#Step-2-PrepareDevice)
-   [Step 3: Build and Run the Sample](#Step-3-Build)
-   [Tips](#tips)

<a name="Introduction"></a>
# Introduction

**About this document**

This document describes how to connect BeagleBone Black device running Debian with Azure IoT SDK. This multi-step process includes:
-   Configuring Azure IoT Hub
-   Registering your IoT device
-   Build and deploy Azure IoT SDK on device

<a name="Step-1-Prerequisites"></a>
# Step 1: Prerequisites

You should have the following items ready before beginning the process:

-   Computer with Git client installed and access to the
    [azure-iot-sdks](https://github.com/Azure/azure-iot-sdks) GitHub
    public repository.
-   BeagleBone Black device.
-   SSH client on your desktop computer, such as [PuTTY](http://www.putty.org/), so you can remotely access the command line on the BeagleBone Black.
-   USB Mini cable.
-   Ethernet cable or Wi-Fi dongle.
-   [Setup your IoT hub][lnk-setup-iot-hub]
-   [Provision your device and get its credentials][lnk-manage-iot-hub]

<a name="Step-2-PrepareDevice"></a>
# Step 2: Prepare your Device
-   Follow the instructions on the [beagleboard.org](http://beagleboard.org/getting-started) site to set up your BeagleBone Black device and connect it to your computer.
-   Connect your BeagleBone Black to your network using an ethernet cable or by using a WiFi dongle on the device.
-   Follow the instructions on this [wiki](http://elinux.org/Beagleboard:Terminal_Shells) to obtain your device's IP address.
-   Open an SSH terminal program, such as [PuTTY](http://www.putty.org/), on your desktop machine and connect to your device using the IP address from the previous step.
-   Connection settings:
    -   Port = 22
    -   Connection Type = SSH
-   When prompted, log in with username **root** (no password by default).

<a name="Step-3-Build"></a>
# Step 3: Build and Run the sample

<a name="Step-3-1-Load"></a>
## 3.1 Build SDK and sample

-   Open a PuTTY session and connect to the device.

-   Install the prerequisite packages for the Microsoft Azure IoT Device SDK for C by issuing the following commands from the command line on your board:

        sudo apt-get update

        sudo apt-get install -y curl libcurl4-openssl-dev build-essential cmake git

    ***Note:*** *This setup process requires cmake version 2.8.12 or higher.* 
    
    *You can verify the current version installed in your environment using the  following command:*

        cmake --version

    *This library also requires gcc version 4.9 or higher. You can verify the current version installed in your environment using the following command:*
    
        gcc --version 

-   Download the Microsoft Azure IoT Device SDK for C to the board by issuing the following command on the board::

        git clone --recursive https://github.com/Azure/azure-iot-sdks.git

-   Edit the following file using any text editor of your choice:

        azure-iot-sdks/c/serializer/samples/simplesample_amqp/simplesample_amqp.c

-   Find the following place holder for IoT connection string:

        static const char* connectionString = "[device connection string]";

-   Replace the above placeholder with device connection string you obtained in [Step 1](#Step-1-Prerequisites) and save the changes.

-   Build the SDK samples using the following command:

        sudo ./azure-iot-sdks/c/build_all/linux/build.sh

## 3.2 Send Device Events to IoT Hub

-   Run the sample by issuing following command:

        azure-iot-sdks/c/cmake/iotsdk_linux/serializer/samples/simplesample_amqp/simplesample_amqp

-   See [Manage IoT Hub][lnk-manage-iot-hub] to learn how to observe the messages IoT Hub receives from the application.

## 3.3 Receive messages from IoT Hub

-   See [Manage IoT Hub][lnk-manage-iot-hub] to learn how to send cloud-to-device messages to the application.


<a name="tips"></a>
# Tips

-   If you just want to build the serializer samples, run the following commands:

        cd azure-iot-sdks/c/cmake/iotsdk_linux/serializer/samples	
        make -f Makefile all


[lnk-setup-iot-hub]: ../setup_iothub.md
[lnk-manage-iot-hub]: ../manage_iot_hub.md