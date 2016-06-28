---
platform: debian
device: gropius
language: c
---

Run a simple C sample on Gropius device running Debian
===
---

# Table of Contents

-   [Introduction](#Introduction)
-   [Step 1: Prerequisites](#Prerequisites)
-   [Step 2: Prepare your Device](#PrepareDevice)
-   [Step 3: Build and Run the Sample](#Build)
-   [Tips](#tips)

# Introduction

**About this document**

This document describes how to connect Gropius device running Debian with Azure IoT SDK. This multi-step process includes:
-   Configuring Azure IoT Hub
-   Registering your IoT device
-   Build and deploy Azure IoT SDK on device

![](http://support.sintau.it/gropius/gropius.jpg)

### Main features ###
-   CPU Marvell Kirkwood 88F6281 (ARMv5 @ 800MHz)
-   512 MB RAM
-   4Gbit NAND Flash
-   USB mass storage support
-   2xGbE LAN interfaces
-   1xRS232 interface
-   1xRS485 interface
-   1xCANBUS interface
-   WiFi IEEE 802.11 g/n interface support
-   GPRS/UMTS/HSDPA/LTE interface support
-   daughter boards for further interfaces support

### Productor ###
[SinTau Srl](http://www.sintau.it/index.php?lang=en)
![](http://support.sintau.it/logo.png)

# Step 1: Prerequisites

You should have the following items ready before beginning the process:

-   [Prepare your development environment][setup-devbox-linux]
-   [Setup your IoT hub][lnk-setup-iot-hub]
-   [Provision your device and get its credentials][lnk-manage-iot-hub]
-   [Gropius][lnk-gropius-device] device.
-   SSH client on your desktop computer, such as [PuTTY][lnk-putty] or [openssh][lnk-openssh], so you can remotely access the command line on the BeagleBone Black.
-   Ethernet cable and Gropius console cable.


# Step 2: Prepare your Device
-   Follow the instructions on the [Gropius support site][lnk-gropius-support] to setup, configure and connect Gropius to your computer

# Step 3: Build and Run the sample

## 3.1 Build SDK and sample

-   Open a SSH session and connect to the device.

-   Install the prerequisite packages for the Microsoft Azure IoT Device SDK for C by issuing the following commands from the command line on your board:

        sudo apt-get update
        sudo apt-get install -y curl uuid-dev libcurl4-openssl-dev libssl-dev build-essential g++ make cmake git

    ***Note:*** *If any package is not available, use following steps to add source in sources.list and rerun above commands again.*
    
    -   Edit /etc/apt/sources.list
    
    -   Add below line and save the changes.
    
    ```    
		deb http://http.debian.net/debian/ wheezy-backports main contrib non-free
		deb http://ftp.debian.org/debian/ sid main contrib non-free
		```

-   Download the Microsoft Azure IoT Device SDK for C to the board by issuing the following command on the board:

        git clone --recursive https://github.com/Azure/azure-iot-sdks.git

-   Edit the following file using any text editor of your choice:

        azure-iot-sdks/c/build_all/linux/build.sh

-   Replace the variable CORES with the value 1

-   Edit the following file using any text editor of your choice:

    **For AMQP protocol:**

        azure-iot-sdks/c/iothub_client/samples/iothub_client_sample_amqp/iothub_client_sample_amqp.c

    **For HTTPS protocol:**

        azure-iot-sdks/c/iothub_client/samples/iothub_client_sample_http/iothub_client_sample_http.c

    **For MQTT protocol:**

        azure-iot-sdks/c/iothub_client/samples/iothub_client_sample_mqtt/iothub_client_sample_mqtt.c


-   Find the following place holder for IoT connection string:

        static const char* connectionString = "[device connection string]";

-   Replace the above placeholder with device connection string you obtained in [Step 1](#Prerequisites) and save the changes.

-   Build the SDK using following command.

        ./azure-iot-sdks/c/build_all/linux/build.sh --skip-unittests

## 3.2 Send Device Events to IoT Hub:

-   Run the sample by issuing following command:

    **If using AMQP protocol:**

        ~/azure-iot-sdks/c/cmake/iotsdk_linux/iothub_client/samples/iothub_client_sample_amqp/iothub_client_sample_amqp

    **If using HTTPS protocol:**

        ~/azure-iot-sdks/c/cmake/iotsdk_linux/iothub_client/samples/iothub_client_sample_http/iothub_client_sample_http

    **If using MQTT protocol:**

        ~/azure-iot-sdks/c/cmake/iotsdk_linux/iothub_client/samples/iothub_client_sample_mqtt/iothub_client_sample_mqtt

-   See [Manage IoT Hub][lnk-manage-iot-hub] to learn how to observe the messages IoT Hub receives from the application.

## 3.3 Receive messages from IoT Hub

-   See [Manage IoT Hub][lnk-manage-iot-hub] to learn how to send cloud-to-device messages to the application.

# Tips

-   If you just want to build the serializer samples, run the following commands:

  		cd ./c/serializer/build/linux
  		make -f makefile.linux all
 

[setup-devbox-linux]: https://github.com/Azure/azure-iot-sdks/blob/master/c/doc/devbox_setup.md
[lnk-setup-iot-hub]: https://github.com/Azure/azure-iot-sdks/blob/master/doc/setup_iothub.md
[lnk-manage-iot-hub]: https://github.com/Azure/azure-iot-sdks/blob/master/doc/manage_iot_hub.md
[lnk-gropius-device]: http://www.sintau.it/index.php?option=com_virtuemart&view=productdetails&virtuemart_product_id=6&virtuemart_category_id=1&Itemid=342&lang=en
[lnk-putty]: http://www.putty.org/
[lnk-openssh]: http://www.openssh.com/
[lnk-gropius-support]: http://support.sintau.it/gropius
