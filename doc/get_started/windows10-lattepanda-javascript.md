---
platform: windows 10
device: lattepanda
language: javascript
---

Run a simple JavaScript sample on LattePanda device running Windows 10
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

This document describes how to connect [LattePanda](http://www.lattepanda.com) device running Windows10 with Azure IoT SDK. This multi-step process includes:
-   Configuring Azure IoT Hub
-   Registering your IoT device
-   Build and deploy Azure IoT SDK on device

<a name="Prerequisites"></a>
# Step 1: Prerequisites

You should have the following items ready before beginning the process:

-   [Prepare your development environment][setup-devbox-linux]

    **Note:** LattePanda runs a full edition of Windows 10, so you are able to install the development environment on your LattePanda and run this example directly. In this tutorial, we developed directly on a LattePanda.

-   [Setup your IoT hub][lnk-setup-iot-hub]

-   [Provision your device and get its credentials][lnk-manage-iot-hub]

-   [LattePanda](http://www.lattepanda.com) device.

<a name="PrepareDevice"></a>
# Step 2: Prepare your Device

-   Install OpenSSL at **C:\OpenSSL** location and perform following actions:
    -   Add `C:\OpenSSL\` to the **PATH** environment variable
    -   Search for the location of file 'openssl.cnf' inside the OpenSSL directory.
    -   Create a new environment variable **OPENSSL_CONF** and set its value as absolute path of the file `openssl.cnf`.

-   Install [NodeJS](https://nodejs.org/en/). Make sure that version of NodeJS is greater than 4.0.

      Install [GIT](https://git-scm.com/download/win).


<a name="Build"></a>
# Step 3: Build and Run the Sample

<a name="Load"></a>
## 3.1 Load the Azure IoT bits and prerequisites on device

-   Create environment variable `IOTHUB_CONNECTION_STRING` and set its value as your **IoTHub connection string** which you got in [Step 1](#Prerequisites).

-   Open a Node.js command prompt.

-   Check all environment variables are properly set.

            echo %PATH%
            echo %OPENSSL_CONF%
            echo %IOTHUB_CONNECTION_STRING%

-   Download the SDK 

            git clone --recursive https://github.com/Azure/azure-iot-sdks.git


<a name="BuildSamples"></a>
## 3.2 Build the samples

-   To validate the source code run the following commands on Node.js command prompt on Device.

        cd azure-iot-sdks\node
        build\dev-setup.cmd
        build\build.cmd > LogFile.txt

    ***Note:*** *LogFile.txt in above command should be replaced with a file name where build output will be written.*

-   Install npm package to run sample.

      	cd \azure-iot-sdks\node\device\samples

        **For AMQP Protocol:**

            npm install azure-iot-device-amqp

        **For HTTP Protocol:**

            npm install azure-iot-device-http

        **For MQTT Protocol:**

            npm install azure-iot-device-mqtt	

-   Update the sample to set the protocol.

            cd azure-iot-sdks\node\device\samples
            notepad simple_sample_device.js

-   This launches a text editor. Scroll down to the protocol information and find the below code:

            var Protocol = require('azure-iot-device-amqp').Amqp;

        The default protocol used is AMQP. Code for other protocols(HTTP/MQTT) are mentioned just below the above line in the script. 
        Uncomment the line as per the protocol you want to use.

-   Scroll down to the connection information and find following placeholder for IoT connection string:

            var connectionString = "[IoT Device Connection String]";

-   Replace the above placeholder with device connection string. You can get this from DeviceExplorer as explained in [Step 1](#Prerequisites), that you copied into Notepad.

-   Save your changes and close the notepad.

-   Run the following command on Node.js command prompt before leaving the **azure-iot-sdks\node\device\samples** directory

            npm link azure-iot-device

<a name="Run"></a>

## 3.3 Run and Validate the Samples

### 3.3.1 Send Device Events to IoT Hub

-   Run the sample by issuing following command and verify that data has been successfully sent and received.

        node azure-iot-sdks\node\device\samples\simple_sample_device.js

-   See [Manage IoT Hub][lnk-manage-iot-hub] to learn how to observe the messages IoT Hub receives from the application.

### 3.3.2 Receive messages from IoT Hub

-   See [Manage IoT Hub][lnk-manage-iot-hub] to learn how to send cloud-to-device messages to the application.

[setup-devbox-linux]: https://github.com/Azure/azure-iot-sdks/blob/master/doc/get_started/node-devbox-setup.md
[lnk-setup-iot-hub]: https://github.com/Azure/azure-iot-sdks/blob/master/doc/setup_iothub.md
[lnk-manage-iot-hub]: https://github.com/Azure/azure-iot-sdks/blob/master/doc/manage_iot_hub.md
