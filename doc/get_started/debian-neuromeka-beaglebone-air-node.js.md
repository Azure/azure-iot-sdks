---
platform: Debian
device: Neuromeka BeagleBone Air
language: Node.js
---
Run Node.js provided from Neuromeka BeagleBone Air device that runs Debian
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

This document describes how to connect Neuromeka BeagleBone Air (referred "BBAir") that runs Debian with Azure IoT SDK. This multi-steps process includes:
-   Configure Azure IoT Hub
-   Register your IoT device
-   Build and deploy Azure IoT SDK on device

<a name="Prerequisites"></a>

# Step 1: Prerequisites

You should have the following prerequisities ready to begin process:

-   Computer with Git client installed and access to the
    [azure-iot-sdks](https://github.com/Azure/azure-iot-sdks) GitHub
    public repository.
-   BeagleBone Air. (BBAir)
-   SSH client on your desktop computer, such as [PuTTY](http://www.putty.org/). (You should access remotely the command line on the BBAir.)
-   USB Mini cable.
-   Ethernet cable or Setting BBAir Wi-Fi. (Folow the instruction on this [wiki](http://wiki.neuromeka.net/index.php?title=WiFi_setting))
-   [Setup your IoT hub](https://github.com/Azure/azure-iot-sdks/blob/master/doc/setup_iothub.md)
-   [Provision your device and get its credentials](https://github.com/Azure/azure-iot-sdks/blob/develop/tools/DeviceExplorer/doc/how_to_use_device_explorer.md)

<a name="PrepareDevice"></a>
# Step 2: Prepare your Device
-   Follow the instructions on linked [wiki](http://wiki.neuromeka.net/index.php?title=Category:Getting_started_with_BB-Air) site to set up your BBAir device and connect it to your computer.
-   Connect your BBAir to your network using an ethernet cable or by using a Wi-Fi on the device.
-   Follow the instructions on linked [wiki](http://wiki.neuromeka.net/index.php?title=Category:Network_settings) site to obtain your device's IP address.
-   Open an SSH terminal program, such as [PuTTY](http://www.putty.org/), on your desktop and connect it to your device getting the IP address from the previous step.
-   Connection settings:
    -   Port = 22
    -   Connection Type = SSH
-   When prompted, log in with username **root** (password is not required on default setting).


<a name="Build"></a>
# Step 3: Build and Run the sample

<a name="Step-3-1-Load"></a>
## 3.1 Build SDK and sample

-   Open a PuTTY session and connect it to the device.

-   Run following commands to check if NodeJS has been installed

        node --version
If version is 0.12.x or greater, then skip next step of installing prerequisite packages. Otherwise, uninstall it from command line on the device.
        
        curl -sL https://deb.nodesource.com/setup_0.12 | bash -
        apt-get install -y nodejs
        
-   Download the SDK to the board by the following command in PuTTY:

        git clone --recursive https://github.com/Azure/azure-iot-sdks.git

## 3.2 Build the sample
-   To validate the source code, Run the following commands on the device.

        cd ~/azure-iot-sdks/node
        build/dev-setup.sh
        build/build.sh | tee LogFile.txt

-   Edit the below file using any text editor program:

        cd ~/azure-iot-sdks/node/device/samples

    **For AMQP Protocol:**

        npm install azure-iot-device-amqp

-   To update the sample, run the following command on device.

        cd ~/azure-iot-sdks/node/device/samples
        nano simple_sample_device.js

-   It will launch console-based text editor. Scroll down to the
    protocol information.
    
-   Find the below code:

        var Protocol = require('azure-iot-device-amqp').Amqp;   

-   Find the following place holder in the code for IoT connection string:

        var connectionString = "[IoT Device Connection String]";

-   Replace the above placeholder to device connection string.

-   Run the following command before leaving the **~/azure-iot-sdks/node/device/samples** directory

        npm link azure-iot-device
## 3.3 Run the sample

### 3.3.1 Send Device Events to IoT Hub

-   Run the sample by issuing following command:

        node ~/azure-iot-sdks/node/device/samples/simple_sample_device.js

-   See [Manage IoT Hub](https://github.com/Azure/azure-iot-sdks/blob/master/doc/manage_iot_hub.md) to learn how to read messages IoT Hub receives from the application.

### 3.3.2 Receive messages from IoT Hub

-   See [Manage IoT Hub](https://github.com/Azure/azure-iot-sdks/blob/master/doc/manage_iot_hub.md) to learn how to send cloud-to-device messages to the application.


[lnk-setup-iot-hub]: ../setup_iothub.md
[lnk-manage-iot-hub]: ../manage_iot_hub.md
