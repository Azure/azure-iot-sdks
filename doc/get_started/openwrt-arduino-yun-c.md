---
platform: openwrt
device: arduino-yun
language: c
---

Run a simple C sample on Arduino Yun device running OpenWRT Yun
===
---

# Table of Contents

-   [Introduction](#Introduction)
-   [Step 1: Prerequisites](#Step-1-Prerequisites)
-   [Step 2: Build and Run the Sample](#Step-2-Build)

<a name="Introduction"></a>
# Introduction

**About this document**

This document describes how to connect Arduino Yun device running OpenWRT Yun with Azure IoT SDK. This multi-step process includes:
-   Configuring Azure IoT Hub
-   Registering your IoT device
-   Build and deploy Azure IoT SDK on device

<a name="Step-1-Prerequisites"></a>
# Step 1: Prerequisites

You should have the following items ready before beginning the process:

-   Computer with Git client installed and access to the
    [azure-iot-sdks](https://github.com/Azure/azure-iot-sdks) GitHub
    public repository.
-   Arduino Yun device to certify.
-   Debian x86 OS.
-   Download and install [DeviceExplorer](https://github.com/Azure/azure-iot-sdks/releases/download/2015-11-13/SetupDeviceExplorer.msi).
-   [Set up your IoT hub](https://github.com/Azure/azure-iot-sdks/blob/master/doc/setup_iothub.md).
#### Create a device on IoT Hub
-   With your IoT hub configured and running in Azure, follow the instructions in **"Create Device"** section of [DeviceExplorer Usage document](https://github.com/Azure/azure-iot-sdks/blob/master/tools/DeviceExplorer/doc/how_to_use_device_explorer.md).
#### Write down device credentials
-   Make note of the Connection String for your device by following the instructions in **"Get device connection string or configuration data"** section of [DeviceExplorer Usage document](https://github.com/Azure/azure-iot-sdks/blob/master/tools/DeviceExplorer/doc/how_to_use_device_explorer.md).

<a name="Step-2-Build"></a>
# Step 2: Build and Run the sample

<a name="Step-2-1-Load"></a>
## 2.1 Build SDK and sample

### Build Azure IoT SDK

-   Open a PuTTY session and connect to the device.

-   Install the prerequisite packages for the Microsoft Azure IoT Device SDK for C by issuing the following commands from the command line on your board:

        sudo apt-get update

        apt-get install curl libcurl4-openssl-dev uuid-dev uuid g++ make cmake git unzip openjdk-7-jre libssl-dev libncurses-dev subversion gawk

-   Download the Microsoft Azure IoT Device SDK for C to the board by issuing the following command on the board::

        git clone --recursive https://github.com/Azure/azure-iot-sdks.git

-  Open a shell and navigate to the folder **c/build_all/arduino** in your local copy of the repository.

-  Run the `setup.sh` script to install the OpenWRT SDK and prerequisites. By default, the SDK will be installed at **~/openwrt/sdk**

-  (Optional) Enter 'Y' to build the Azure IoT SDK.

    This script builds the **iothub_client** and **serializer** libraries and their associated samples.

    ***Note:*** *You will not be able to run the samples until you configure them with a valid IoT Hub device connection string. For more information, see [Run sample on Linux](run_sample_on_desktop_linux.md).*

### Build the Sample

-   Open the file **c/serializer/samples/simplesample_http/simplesample_http.c** in a text editor.

-   Find the following place holder for IoT device connection string:

        static const char* connectionString = "[device connection string]";

-   Replace the above placeholder with device connection string you obtained in [Step 1](#Step-1-Prerequisites) and save the changes.

-   Run the `build.sh` script in the **c/build_all/arduino** directory.   

### Deploy the sample

-   Open a shell and navigate to the installed OpenWRT SDK folder.

    By default, it is **~/openwrt/sdk**.

-   Transfer the package.

    OpenWRT Yun Image:

        scp bin/ar71xx/packages/azure-iot-sdks_1-1_ar71xx.ipk root@arduino.local:/tmp

    LininoIO Yun Image:

        scp bin/ar71xx/packages/azure-iot-sdks_1-1_ar71xx.ipk root@linino.local:/tmp

## 2.2 Send Device Events to IoT Hub:

***Note:*** *The following instructions assumes the device is flashed with OpenWRT Yun.*

*If you are using LininoIO, substitute `arduino` with `linino` and use the default password `doghunter`.*

-   Open shell and enter `ssh root@arduino.local`

    Enter the device password. By default it is, `arduino` for OpenWRT Yun Image or `doghunter` for LininoIO.

-   Install the package

        opkg install /tmp/azure-iot-sdks_1-1_ar71xx.ipk

-   Run the sample by issuing following command:

         cd /usr/share/azure-iot-sdk/samples
         ./simplesample_http

-   On Windows, refer "Monitor device-to-cloud events" in [DeviceExplorer Usage document](https://github.com/Azure/azure-iot-sdks/blob/master/tools/DeviceExplorer/doc/how_to_use_device_explorer.md) to see the data your device is sending.

-   If you are running other OS, please use the JavaScript tool [iot-hub explorer tool](https://github.com/Azure/azure-iot-sdks/tree/master/tools/iothub-explorer/doc)

## 2.3 Receive messages from IoT Hub

-   On Windows, refer "Send cloud-to-device messages" in [DeviceExplorer Usage document](https://github.com/Azure/azure-iot-sdks/blob/master/tools/DeviceExplorer/doc/how_to_use_device_explorer.md) for instructions on sending messages to device.

-   If you are running other OS, please use the JavaScript tool [iot-hub explorer tool](https://github.com/Azure/azure-iot-sdks/tree/master/tools/iothub-explorer/doc)
