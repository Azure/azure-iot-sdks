---
platform: debian
device: beaglebone-green
language: c
---

Run a simple C sample on BeagleBone Green device running Debian
===
---

# Table of Contents

-   [Introduction](#Introduction)
-   [Step 1: Prerequisites](#Prerequisites)
-   [Step 2: Prepare your Device](#Step-2-PrepareDevice)
-   [Step 3: Build and Run the Sample](#Step-3-Build)
-   [Tips](#tips)

<a name="Introduction"></a>
# Introduction

**About this document**

This document describes how to connect BeagleBone Green device running Debian with Azure IoT SDK. This multi-step process includes:
-   Configuring Azure IoT Hub
-   Registering your IoT device
-   Build and deploy Azure IoT SDK on device

<a name="Prerequisites"></a>
# Step 1: Prerequisites

You should have the following items ready before beginning the process:

-   Computer with Git client installed and access to the
    [azure-iot-sdks-c](https://github.com/Azure/azure-iot-sdk-c.git) GitHub
    public repository.
-   [BeagleBone Green](http://beagleboard.org/Green).
-   USB Mini cable.
-   Ethernet cable or Wi-Fi dongle
-   [Setup your IoT hub][lnk-setup-iot-hub]
-   [Provision your device and get its credentials][lnk-manage-iot-hub]

<a name="Step-2-PrepareDevice"></a>
# Step 2: Prepare your Device

-   To set up your BeagleBone Green device and connect it to your machine, follow the instructions on the [beagleboard.org](http://beagleboard.org/getting-started).
-   Connect your BeagleBone Green to your network using an ethernet cable or by using a WiFi dongle on the device.
-   You need to discover the IP address of your BeagleBone Green before your can connect using PuTTY. To do this you can find instructions on [wiki](http://elinux.org/Beagleboard:Terminal_Shells).
-   Once you see that your board is alive, open an SSH terminal program such as [PuTTY](http://www.putty.org/) on your desktop machine.
-   Use the IP address discovered above as the Host name, Port=22, and Connection type=SSH to complete the connection.
-   When prompted, log in with username **root** (no password by default).

<a name="Step-3-Build"></a>
# Step 3: Build and Run the sample

<a name="Step-3-1-Load"></a>
## 3.1 Build SDK and sample

-   Open a PuTTY session and connect to the device.

-   Install the prerequisite packages for the Microsoft Azure IoT Device SDK for C by issuing the following commands from the command line on your board:

        sudo apt-get update

        sudo apt-get install -y curl libcurl4-openssl-dev build-essential cmake git

-   Change the configuration in the file /etc/dphys-swapfile, in order to change swapfile size.

	***Note:*** *It is recommended to allocate at least 1GB for the swap partition.*
    -   default:

            CONF_SWAPSIZE=100

    -   change this to:

            CONF_SWAPSIZE=1024

    -   Restart the service

            sudo /etc/init.d/dphys-swapfile stop
            sudo /etc/init.d/dphys-swapfile start
        
    ***Note:*** *This setup process requires cmake version 2.8.12 or higher.* 
    
    *You can verify the current version installed in your environment using the  following command:*

        cmake --version

    *This library also requires gcc version 4.9 or higher. You can verify the current version installed in your environment using the following command:*
    
        gcc --version 

-   Download the Microsoft Azure IoT Device SDK for C to the board by issuing the following command on the board::

        git clone --recursive https://github.com/Azure/azure-iot-sdk-c.git

	***Note:*** *It is recommended to get an USB disk and clone the SDK into it and do the compilation there.*

-   Edit the following file using any text editor of your choice:

        azure-iot-sdk-c/serializer/samples/simplesample_amqp/simplesample_amqp.c

-   Find the following place holder for IoT connection string:

        static const char* connectionString = "[device connection string]";

-   Replace the above placeholder with device connection string you obtained in [Step 1](#Prerequisites) and save the changes.

-   Build the SDK samples using the following command:

        sudo ./azure-iot-sdk-c/build_all/linux/build.sh

## 3.2 Send/Receive messages to/from IoT Hub:

-   Run the sample by issuing following command:

        azure-iot-sdk-c/cmake/iotsdk_linux/serializer/samples/simplesample_amqp/simplesample_amqp

-   See [Manage IoT Hub][lnk-manage-iot-hub] to learn how to observe the messages IoT Hub receives from the application and how to send cloud-to-device messages to the application.


<a name="tips"></a>
# Tips

-   If you just want to build the serializer samples, run the following commands:

     	cd azure-iot-sdk-c/cmake/iotsdk_linux/serializer/samples
        make -f Makefile all

[lnk-setup-iot-hub]: ../setup_iothub.md
[lnk-manage-iot-hub]: ../manage_iot_hub.md
