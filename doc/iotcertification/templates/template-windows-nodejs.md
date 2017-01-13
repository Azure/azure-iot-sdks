---
platform: {enter the OS name running on device}
device: {enter your device name here}
language: javascript
---

Run a simple JavaScript sample on {enter your device name here} device running {enter the OS name running on device}
===
---

# Table of Contents

-   [Introduction](#Introduction)
-   [Step 1: Prerequisites](#Prerequisites)
-   [Step 2: Prepare your Device](#PrepareDevice)
-   [Step 3: Build and Run the Sample](#Build)

# Instructions for using this template

-   Replace the text in {placeholders} with correct values.
-   Delete the lines {{enclosed}} after following the instructions enclosed between them.
-   It is advisable to use external links, wherever possible.
-   Remove this section from final document.

<a name="Introduction"></a>
# Introduction

**About this document**

This document describes how to connect {enter your device name here} device running {enter the OS name running on device} with Azure IoT SDK. This multi-step process includes:
-   Configuring Azure IoT Hub
-   Registering your IoT device
-   Build and deploy Azure IoT SDK on device

<a name="Prerequisites"></a>
# Step 1: Prerequisites

You should have the following items ready before beginning the process:


-   [Prepare your development environment][setup-devbox-linux]
-   [Setup your IoT hub][lnk-setup-iot-hub]
-   [Provision your device and get its credentials][lnk-manage-iot-hub]
-   Computer with Git client installed
-   {enter your device name here} device.
-   {{Please specify if any other software(s) or hardware(s) are required.}}

<a name="PrepareDevice"></a>
# Step 2: Prepare your Device

-   Install OpenSSL at **C:\OpenSSL** location and perform following actions:
    -   Add `C:\OpenSSL\` to the **PATH** environment variable
	-   Search for the location of file 'openssl.cnf' inside the OpenSSL directory.
	-   Create a new environment variable **OPENSSL_CONF** and set its value as absolute path of the file `openssl.cnf`.
	
-   {{Write down the instructions required to setup, configure and connect your device. Please use external links when possible pointing to your own page with device preparation steps.}}

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
[lnk-setup-iot-hub]: ../../setup_iothub.md
[lnk-manage-iot-hub]: ../../manage_iot_hub.md
