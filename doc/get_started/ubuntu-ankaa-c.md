---
platform: ubuntu
device: ankaa
language: c
---

Run a simple C sample on Ankaa device running Ubuntu
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

This document describes how to connect Ankaa device running Ubuntu with Azure IoT SDK. This multi-step process includes:
-   Configuring Azure IoT Hub
-   Registering your IoT device
-   Build and deploy Azure IoT SDK on device

<a name="Step-1-Prerequisites"></a>
# Step 1: Prerequisites

You should have the following items ready before beginning the process:
-   Computer with Git client installed and access to the
    [azure-iot-sdks](https://github.com/Azure/azure-iot-sdks) GitHub public repository.
-   Ankaa device.
-   [Setup your IoT hub][lnk-setup-iot-hub]
-   [Provision your device and get its credentials][lnk-manage-iot-hub]

<a name="Step-2-PrepareDevice"></a>
# Step 2: Prepare your Device

-   Install the Ubuntu operating system on your Ankaa device.
-   Connect the Ankaa board to your desktop machine using the USB mini cable and open a terminal emulator program such as [Tera Term](https://en.osdn.jp/projects/ttssh2/releases/) on your desktop machine and switch ON the device. You have to create a new Serial port connection from Setup->Serial Port and set the parameters:
    -   Port number where you have connected the device to PC.
    -   Baud Rate = 115200
    -   Data = 8 bits
    -   Parity = noneStop = 1 bit
    -   Flow Control = none
-   When prompted, log in with username **root**, and password **root**. The root account is necessary in order to install some libraries required by the device SDK.
-   If you want to connect your Ankaa to your Wired network connect it using an ethernet cable to the device ethernet port. For Wireless connection, Ankaa have it's own WiFi module so there is no need to connect any WiFi dongle.
-   You need to determine the IP address of your Ankaa in order to connect over the network. To connect the network, follow the steps under **Ethernet** or **WiFi** for the corresponding connection type from the document "esomimx6_ubuntu_peripheral_test_user_manual.pdf"

<a name="Step-3-Build"></a>
# Step 3: Build and Run the sample

<a name="Step-3-1-Load"></a>
## 3.1 Build SDK and sample

Run the following commands in the terminal window connected to your Ankaa device.

-   Install the prerequisite packages for the Microsoft Azure IoT Device SDK for C by issuing the following commands from the command line on your board:

        sudo apt-get update
        sudo apt-get install -y curl libcurl4-openssl-dev build-essential cmake git

    If you get errors running sudo, make sure your root password is set as described above.

    ***Note:*** *Right-click in a Tera Term terminal window to paste the copied text.*
    
    ***Note:*** *This setup process requires cmake version 2.8.12 or higher.* 
    
    *You can verify the current version installed in your environment using the  following command:*

        cmake --version

    *This library also requires gcc version 4.9 or higher. You can verify the current version installed in your environment using the following command:*
    
        gcc --version 

    *For information about how to upgrade your version of gcc on Ubuntu 14.04, see <http://askubuntu.com/questions/466651/how-do-i-use-the-latest-gcc-4-9-on-ubuntu-14-04>.*

-   Download the Microsoft Azure IoT Device SDK for C to the board by issuing the following command on the board::

        git clone --recursive https://github.com/Azure/azure-iot-sdks.git

-   Edit the following file using any text editor of your choice:

    **For AMQP protocol:**

        azure-iot-sdks/c/iothub_client/samples/iothub_client_sample_amqp/iothub_client_sample_amqp.c

    **For HTTP protocol:**

        azure-iot-sdks/c/iothub_client/samples/iothub_client_sample_http/iothub_client_sample_http.c

-   Find the following place holder for IoT connection string:

        static const char* connectionString = "[device connection string]";

-   Replace the above placeholder with device connection string you obtained in [Step 1](#Step-1-Prerequisites) and save the changes.

-   Build the SDK samples using the following command:

        sudo ./azure-iot-sdks/c/build_all/linux/build.sh

## 3.2 Send Device Events to IoT Hub:

-   Run the sample by issuing following command:

    **For AMQP protocol:**

        azure-iot-sdks/c/cmake/iotsdk_linux/iothub_client/samples/iothub_client_sample_amqp/iothub_client_sample_amqp

    **For HTTP protocol:**

        azure-iot-sdks/c/cmake/iotsdk_linux/iothub_client/samples/iothub_client_sample_http/iothub_client_sample_http

-   See [Manage IoT Hub][lnk-manage-iot-hub] to learn how to observe the messages IoT Hub receives from the application.

## 3.3 Receive messages from IoT Hub

-   See [Manage IoT Hub][lnk-manage-iot-hub] to learn how to send cloud-to-device messages to the application.

<a name="tips"></a>
# Tips

- If you just want to build iothub\_client, go to the cmake output directory and navigate to the iothub_client/samples folder then run the following command:

		make -f Makefile all

[lnk-setup-iot-hub]: ../setup_iothub.md
[lnk-manage-iot-hub]: ../manage_iot_hub.md
