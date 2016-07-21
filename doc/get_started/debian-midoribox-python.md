---
platform: Debian
device: midori box
language: python
---

Run a simple PYTHON sample on Midori Box device running Debian
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

This document describes how to connect Midori Box device running Debian with Azure IoT SDK. This multi-step process includes:
-   Configuring Azure IoT Hub
-   Registering your IoT device
-   Build and deploy Azure IoT SDK on device

<a name="Prerequisites"></a>
# Step 1: Prerequisites

You should have the following items ready before beginning the process:

-   [Prepare your development environment][setup-devbox-python]
-   [Setup your IoT hub][lnk-setup-iot-hub]
-   [Provision your device and get its credentials][lnk-manage-iot-hub]
-   Midori Box device.

<a name="PrepareDevice"></a>
# Step 2: Prepare your Device
-   Insert the AC adapter into an outlet.
-   Setup your Midori Box according to the [manual](https://midori-cloud.net/user/info/help/category/1/%E3%83%9E%E3%83%8B%E3%83%A5%E3%82%A2%E3%83%AB%E3%83%80%E3%82%A6%E3%83%B3%E3%83%AD%E3%83%BC%E3%83%89.htm)
-   Connect to the device by SSH or USB serial
-   Log in with username pi, and password (Please contact our support, if you want to know password).

<a name="Build"></a>
# Step 3: Build and Run the sample

<a name="Load"></a>
## 3.1 Build SDK and sample

-   Open a PuTTY session and connect to the device.

-   Install the prerequisite packages for the Microsoft Azure IoT Device SDK for Python by issuing the following commands from the command line on your board:


        sudo apt-get update

        sudo apt-get install -y curl libcurl4-openssl-dev build-essential cmake git python2.7-dev libboost-python-devi uuid-dev


-   Change the configuration in the file /etc/dphys-swapfile, in order to change swapfile size.

    -   default:

            CONF_SWAPSIZE=100

    -   change this to:

            CONF_SWAPSIZE=1024

    -   Restart the service

            sudo /etc/init.d/dphys-swapfile stop
            sudo /etc/init.d/dphys-swapfile start

-   Download the Microsoft Azure IoT Device SDK to the board by issuing the following command on the board::

        git clone --recursive https://github.com/Azure/azure-iot-sdks.git

-   Run following commands to build the SDK:

        cd python/build_all/linux
        sudo ./build.sh    

-   After a successful build, the `iothub_client.so` Python extension module is copied to the **python/device/samples** folder.

-   Navigate to samples folder by executing following command:

        cd azure-iot-sdks/python/device/samples/

-   Edit the following file using any text editor of your choice:

    **For AMQP protocol:**

        nano iothub_client_sample_amqp.py

    **For HTTP protocol:**

        nano iothub_client_sample_http.py

    **For MQTT protocol:**

        nano iothub_client_sample_mqtt.py

-   Find the following place holder for device connection string:

        connectionString = "[device connection string]"

-   Replace the above placeholder with device connection string you obtained in [Step 1](#Prerequisites) and save the changes.

## 3.2 Send Device Events to IoT Hub:

-   Run the sample application using the following command:

    **For AMQP protocol:**

        python iothub_client_sample_amqp.py

    **For HTTP protocol:**

        python iothub_client_sample_http.py

    **For MQTT protocol:**

        python iothub_client_sample_mqtt.py

-   See [Manage IoT Hub][lnk-manage-iot-hub] to learn how to observe the messages IoT Hub receives from the application.

## 3.3 Receive messages from IoT Hub

-   See [Manage IoT Hub][lnk-manage-iot-hub] to learn how to send cloud-to-device messages to the application.

[setup-devbox-python]: https://github.com/Azure/azure-iot-sdks/blob/master/doc/get_started/python-devbox-setup.md
[lnk-setup-iot-hub]: ../setup_iothub.md
[lnk-manage-iot-hub]: ../manage_iot_hub.md
