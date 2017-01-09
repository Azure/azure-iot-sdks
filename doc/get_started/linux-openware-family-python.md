---
platform: Ubuntu/Debian Linux
device: openware family
language: python
---

Run a simple PYTHON sample on OpenWare device (OpenWare Gateway/OpenWare Router/OpenWare Sensor Device) running Linux
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

This document describes how to connect an OpenWare device running Linux with the Azure IoT Platform. This multi-step process includes:
-   Configuring Azure IoT Hub
-   Registering your IoT device
-   Build and deploy Azure IoT SDK on device

<a name="Prerequisites"></a>
# Step 1: Prerequisites

You should have the following items ready before beginning the process:

-   OpenWare Gateway device with power supply
-   Computer with internet access and the following software installed
    -   Git client installed [git](https://git-scm.com)
    -   Python 2.7 or 3.4 or greater [Python](https://python.org)
    -   SSH client, such as [PuTTY](http://www.putty.org/)
-   Ethernet cable
-   TCP/IP network with router
-   Login credentials for OpenWare Gatway device
-   [Setup your IoT hub][lnk-setup-iot-hub]
-   [Provision your device and get its credentials][lnk-manage-iot-hub]

<a name="PrepareDevice"></a>
# Step 2: Prepare your Device
-   Follow the instructions provided with the OpenWare device packaging
-   Connect your OpenWare Gateway to your network using an ethernet cable
-   Run the provided OpenWare device finder in a shell/command window (python pmdGatewayFinder.py)
    -   If the device will be used over WiFi run pmdGatewayWifiSetup.py to configure device on WiFi network.
        -   Provide network login information when prompted and note IP address
        -   Disconnect the ethernet cable from the OpenWare device
-   Open an SSH terminal program, such as [PuTTY](http://www.putty.org/), on your desktop machine and connect to your device using the IP address from the previous step.
-   Connection settings:
    -   Port = 22
    -   Connection Type = SSH
-   When prompted, log in with username and password provided with your OpenWare account

<a name="Build"></a>
# Step 3: Build and Run the sample

<a name="Load"></a>
## 3.1 Build SDK and sample

-   Open a PuTTY session and connect to the device.

-   Install the prerequisite packages for the Microsoft Azure IoT Device SDK for Python by issuing the following commands from the command line on your board:

        sudo apt-get update

        sudo apt-get install -y curl libcurl4-openssl-dev build-essential cmake git python2.7-dev libboost-python-dev

-   Download the Microsoft Azure IoT Device SDK to the OpenWare device by issuing the following command on the OpenWare device:

        git clone --recursive https://github.com/Azure/azure-iot-sdks.git

-   Run following commands to build the SDK:

        cd python/build_all/linux
        sudo ./build.sh

-   After a successful build, the `iothub_client.so` Python extension module is copied to the **python/device/samples** folder.

- Navigate to samples folder by executing following command:

        cd azure-iot-sdks/python/device/samples/

-   Edit the following file using any text editor of your choice:

        nano iothub_client_sample_mqtt.py

-   Find the following place holder for device connection string:

        connectionString = "[device connection string]"

-   Replace the above placeholder with device connection string you obtained in [Step 1](#Prerequisites) and save the changes.

## 3.2 Send Device Events to IoT Hub:

-   Run the sample application using the following command:

        python iothub_client_sample_mqtt.py

-   See [Manage IoT Hub][lnk-manage-iot-hub] to learn how to observe the messages IoT Hub receives from the application.

## 3.3 Receive messages from IoT Hub

-   See [Manage IoT Hub][lnk-manage-iot-hub] to learn how to send cloud-to-device messages to the application.

[lnk-setup-iot-hub]: ../setup_iothub.md
[lnk-manage-iot-hub]: ../manage_iot_hub.md
