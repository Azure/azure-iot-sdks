---
platform: ubuntu
device: dell edge gateway 5000 series
language: c
---

Run a simple C sample on Dell Edge Gateway 5000 Series device running Ubuntu
===
---

# Table of Contents

-   [Introduction](#Introduction)
-   [Step 1: Prerequisites](#Prerequisites)
-   [Step 2: Prepare your Device](#PrepareDevice)
-   [Step 3: Build and Run the Sample](#Build)
-   [Tips](#tips)

<a name="Introduction"></a>
# Introduction

**About this document**

This document describes how to connect **Dell Edge Gateway 5000 Series** device running **Ubuntu** with Azure IoT SDK. This multi-step process includes:
-   Configuring Azure IoT Hub
-   Registering your IoT device
-   Build and deploy Azure IoT SDK on device

<a name="Prerequisites"></a>
# Step 1: Prerequisites

You should have the following items ready before beginning the process:

-   [Prepare your development environment][setup-devbox-linux]
-   [Setup your IoT hub][lnk-setup-iot-hub]
-   [Provision your device and get its credentials][lnk-manage-iot-hub]

<a name="PrepareDevice"></a>
# Step 2: Prepare your Device
-   Follow the instructions from [device website](http://www.dell.com/us/business/p/dell-edge-gateway-5000/pd?oc=xctoi5000us).

<a name="Build"></a>
# Step 3: Build and Run the sample

<a name="Load"></a>
## 3.1 Build SDK and sample

-   Open a PuTTY session and connect to the device.

-   Install the prerequisite packages for the Microsoft Azure IoT Device SDK for C by issuing the following commands from the command line on your board:

        sudo apt-get update

        sudo apt-get install -y curl libcurl4-openssl-dev uuid-dev uuid g++ make cmake git unzip openjdk-7-jre
    
    ***Note:*** *This setup process requires cmake version 3.0 or higher.* 
    
    *You can verify the current version installed in your environment using the  following command:*

        cmake --version

    *For information about how to upgrade your version of cmake to 3.2 on Ubuntu 14.04, see <http://askubuntu.com/questions/610291/how-to-install-cmake-3-2-on-ubuntu-14-04>.*
    
    *This library also requires gcc version 4.9 or higher. You can verify the current version installed in your environment using the following command:*
    
        gcc --version 

    *For information about how to upgrade your version of gcc on Ubuntu 14.04, see <http://askubuntu.com/questions/466651/how-do-i-use-the-latest-gcc-4-9-on-ubuntu-14-04>.*

-  Download the Microsoft Azure IoT Device SDK for C to the board by issuing the following command on the board::

        git clone --recursive https://github.com/Azure/azure-iot-sdks.git

-  Verify that you now have a copy of the source code under the directory ~/azure-iot-sdks.

-   Edit the following file using any text editor of your choice:
  
    **For AMQP protocol:**

        azure-iot-sdks/c/iothub_client/samples/iothub_client_sample_amqp/iothub_client_sample_amqp.c

    **For HTTP protocol:**

        azure-iot-sdks/c/iothub_client/samples/iothub_client_sample_http/iothub_client_sample_http.c
	
    **For MQTT protocol:**
		
        azure-iot-sdks/c/iothub_client/samples/iothub_client_sample_mqtt/iothub_client_sample_mqtt.c

-   Find the following place holder for IoT connection string:

        static const char* connectionString = "[device connection string]";

-   Replace the above placeholder with device connection string you obtained in [Step 1](#Step-1:-Prerequisites) and save the changes.

-   Build the SDK using following command.
	
        sudo ./azure-iot-sdks/c/build_all/linux/build.sh | tee LogFile.txt

## 3.2 Send Device Events to IoT Hub:

-   Run the sample by issuing following command:

    **For AMQP protocol:** Run sample iothub_client_sample_amqp

        ~/cmake/iothub_client/samples/iothub_client_sample_amqp/iothub_client_sample_amqp

    **For HTTP protocol:** Run sample iothub_client_sample_http

        ~/cmake/iothub_client/samples/iothub_client_sample_http/iothub_client_sample_http

 	  **For MQTT protocol:** Run sample iothub_client_sample_mqtt

        ~/cmake/iothub_client/samples/iothub_client_sample_mqtt/iothub_client_sample_mqtt

-   On Windows, refer "Send cloud-to-device messages" in [DeviceExplorer Usage document][lnk-device-explorer] for instructions on sending messages to device.

-   If you are running other OS, please use the JavaScript tool [iot-hub explorer tool][lnk-iothub-explorer]

## 3.3 Receive messages from IoT Hub

-   On Windows, refer "Send cloud-to-device messages" in [DeviceExplorer Usage document][lnk-device-explorer] for instructions on sending messages to device.

-   If you are running other OS, please use the JavaScript tool [iot-hub explorer tool][lnk-iothub-explorer]

<a name="tips"></a>
# Tips

-   If you just want to build the serializer samples, run the following commands:
 
        cd ./c/serializer/build/linux
        make -f makefile.linux all

[lnk-setup-iot-hub]: ../setup_iothub.md
[lnk-manage-iot-hub]: ../manage_iot_hub.md
[lnk-device-explorer]: ../../tools/DeviceExplorer/doc/how_to_use_device_explorer.md
[lnk-iothub-explorer]: ../../tools/iothub-explorer/readme.md