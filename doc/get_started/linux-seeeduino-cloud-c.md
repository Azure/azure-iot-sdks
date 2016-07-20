---
platform: ubuntu
device: seeeduino cloud
language: c
---

Run a simple C sample on Seeeduino Cloud device running Ubuntu
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

This document describes how to connect Seeeduino Cloud device running Ubuntu with Azure IoT SDK. This multi-step process includes:
-   Configuring Azure IoT Hub
-   Registering your IoT device
-   Build and deploy Azure IoT SDK on device

<a name="Prerequisites"></a>
# Step 1: Prerequisites

You should have the following items ready before beginning the process:

-   [Prepare your development environment][setup-devbox-linux]
-   [Setup your IoT hub][lnk-setup-iot-hub]
-   [Provision your device and get its credentials][lnk-manage-iot-hub]
-   [Seeeduino Cloud](https://www.seeedstudio.com/item_detail.html?p_id=2123) device.

<a name="PrepareDevice"></a>
# Step 2: Prepare your Device
-   Use PUTTY tool connect Seeeduino Cloud to a usable network, can make reference to this [wiki](http://www.seeedstudio.com/wiki/Seeeduino_Cloud).

<a name="Build"></a>
# Step 3: Build and Run the sample

<a name="Load"></a>
## 3.1 Build SDK and sample

-   Open a PuTTY session and connect to the device.

-   Install the prerequisite packages for the Microsoft Azure IoT Device SDK for C by issuing the following commands from the command line on your board:

    ```
    sudo add-apt-repository ppa:george-edison55/cmake-3.x      # cmake ppa
    sudo add-apt-repository ppa:ubuntu-toolchain-r/test        # gcc ppa
    sudo apt-get update
    sudo apt-get install -y curl libcurl4-openssl-dev uuid-dev uuid git
    sudo apt-get install cmake
    sudo apt-get install gcc-4.9 g++-4.9
    sudo update-alternatives --install /usr/bin/gcc gcc /usr/bin/gcc-4.9 60 --slave /usr/bin/g++ g++ /usr/bin/g++-4.9
    ```

-   Download the Microsoft Azure IoT Device SDK for C to the board by issuing the following command on the board::

        git clone --recursive https://github.com/Azure/azure-iot-sdks.git

-   Edit the following files using any text editor of your choice:

        azure-iot-sdks/c/iothub_client/samples/iothub_client_sample_amqp/iothub_client_sample_amqp.c
        azure-iot-sdks/c/iothub_client/samples/iothub_client_sample_mqtt/iothub_client_sample_mqtt.c

-   Find the following place holder for IoT connection string:

        static const char* connectionString = "[device connection string]";

-   Replace the above placeholder with device connection string you obtained in [Step 1](#Prerequisites) and save the changes.

-   Build the SDK using following command.

    ```
    cd c/build_all/linux
    sudo ./setup.sh
    sudo ./build.sh
    ```

-   Use "scp" command copy files to the path /root of Seeeduino Cloud

-   Install ca-certificates

    ```
    wget http://downloads.openwrt.org/snapshots/trunk/ar71xx/generic/packages/base/ca-certificates_20160104_all.ipk
    opkg install ca-certificates_20160104_all.ipk
    ```

-   Change the files as a executable file

    ```
    chmod +x ./iothub_client_sample_amqp
    chmod +x ./iothub_client_sample_mqtt
    ```

## 3.2 Send Device Events to IoT Hub:

-   Run the sample by issuing following command:

        cd /root

    **For AMQP protocol:**

        ./iothub_client_sample_amqp

    **For MQTT protocol:**

        ./iothub_client_sample_mqtt

-   See [Manage IoT Hub][lnk-manage-iot-hub] to learn how to observe the messages IoT Hub receives from the application.

## 3.3 Receive messages from IoT Hub

-   See [Manage IoT Hub][lnk-manage-iot-hub] to learn how to send cloud-to-device messages to the application.


[setup-devbox-linux]: https://github.com/Azure/azure-iot-sdks/blob/master/c/doc/devbox_setup.md
[lnk-setup-iot-hub]: ../setup_iothub.md
[lnk-manage-iot-hub]: ../manage_iot_hub.md
