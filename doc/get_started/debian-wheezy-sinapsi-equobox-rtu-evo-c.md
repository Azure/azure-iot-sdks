---
platform: debian linux 7.1 "wheezy"
device: sinapsi equobox rtu evo
language: c
---

Run a simple C sample on Sinapsi EQUOBOX RTU EVO device running Debian Linux 7.1 "Wheezy"
===
---

# Table of Contents

-   [Introduction](#Introduction)
-   [Step 1: Prerequisites](#Prerequisites)
-   [Step 2: Prepare your Device](#PrepareDevice)
-   [Step 3: Build and Run the Sample](#Build)

# Introduction

**About this document**

This document describes how to connect Sinapsi EQUOBOX RTU EVO device running Debian Linux 7.1 "Wheezy" with Azure IoT SDK. This multi-step process includes:
-   Configuring Azure IoT Hub
-   Registering your IoT device
-   Build and deploy Azure IoT SDK on device

<a name="Prerequisites"></a>
# Step 1: Prerequisites

You should have the following items ready before beginning the process:

-   [Prepare your development environment][setup-devbox-linux]
-   [Setup your IoT hub][lnk-setup-iot-hub]
-   [Provision your device and get its credentials][lnk-manage-iot-hub]
-   Sinapsi EQUOBOX RTU EVO device.

<a name="PrepareDevice"></a>
# Step 2: Prepare your Device

The device comes with Debian 7.1 "Wheezy" preinstalled.

Access the device on its default IP address with ssh:

    ssh -l cloud 192.168.1.110  # (password: sinapsitech)

The device root partition (/) is mounted in read-only mode by default; in order to perform all the subsequent steps it is needed to perform the following command:

    sudo service root_rw.sh start

this command remounts the root partition in read-write mode.

<a name="Build"></a>
# Step 3: Build and Run the sample

<a name="Load"></a>
## 3.1 Build SDK and sample

-   Open a terminal or PuTTY session and connect to the device (See Step 2).

-   Install the prerequisite packages for the Microsoft Azure IoT Device SDK for C by issuing the following commands from the command line on your board:

        sudo apt-get update

        sudo apt-get install -y curl libcurl4-openssl-dev uuid-dev uuid gcc g++ make cmake git unzip openjdk-7-jre nano

-   Download the Microsoft Azure IoT Device SDK for C to the board by issuing the following command on the board::

        cd /apps

        git clone --recursive https://github.com/Azure/azure-iot-sdks.git

-   Edit the following file using any text editor of your choice:

        nano azure-iot-sdks/c/iothub_client/samples/iothub_client_sample_amqp/iothub_client_sample_amqp.c

-   Find the following place holder for IoT connection string:

        static const char* connectionString = "[device connection string]";

-   Replace the above placeholder with device connection string you obtained in [Step 1](#Prerequisites) and save the changes.

-   Save your changes by pressing Ctrl+O and when nano prompts you to save it as the same file, just press ENTER.

-   Set environment variables.

-   Open **IOT_DEVICE_PARAMS.TXT** to edit.

        nano azure-iot-sdks/tools/iot_hub_e2e_tests_params/iot_device_params.txt

-   Set the values for all the variables listed in the file as explained below.

    -   **IOTHUB_CONNECTION_STRING:** Connection string to your IoT Hub you have received in [Setup your IoT hub](Setup your IoT hub)

    -   **IOTHUB_EVENTHUB_CONNECTION_STRING:** Connection string to your Event Hub. It should be in the form:
    
            Endpoint=[Event Hub-compatible endpoint];SharedAccessKeyName=[IOTHUB_POLICY_NAME];SharedAccessKey=[IOTHUB_POLICY_KEY]
        
    -   **IOTHUB_EVENTHUB_CONSUMER_GROUP:** Set value as **$Default**
    -   **IOTHUB_PARTITION_COUNT:** Partition count from azure portal, as shown in figure below.

        ![](https://github.com/Azure/azure-iot-sdks/raw/master/doc/iotcertification/iot_certification_linux_c/images/azure-portal-partition-count.png)

-   Set environment variables by running following command on your device:

        cd ./azure-iot-sdks/tools/iot_hub_e2e_tests_params/
        chmod +x setiotdeviceparametersfore2etests.sh
        sudo ./setiotdeviceparametersfore2etests.sh

-   Restart the Linux machine. Remember to reissue following command in order to be able to write on root partition.

        sudo service root_rw.sh start 

-   Build the SDK using following commands.

        sudo su
        export LDFLAGS=-lrt
        cd /apps
        ./azure-iot-sdks/c/build_all/linux/build.sh

## 3.2 Send Device Events to IoT Hub:

-   Run the sample by issuing following command:

        /apps/azure-iot-sdks/c/cmake/iotsdk_linux/iothub_client/samples/iothub_client_sample_amqp/iothub_client_sample_amqp

-   See [Manage IoT Hub][lnk-manage-iot-hub] to learn how to observe the messages IoT Hub receives from the application.

## 3.3 Receive messages from IoT Hub

-   See [Manage IoT Hub][lnk-manage-iot-hub] to learn how to send cloud-to-device messages to the application.


[lnk-setup-iot-hub]: https://github.com/Azure/azure-iot-sdks/blob/master/doc/setup_iothub.md
[lnk-manage-iot-hub]: https://github.com/Azure/azure-iot-sdks/blob/master/doc/manage_iot_hub.md