---
platform: Yocto
device: PasSy Gateway
language: Javascript
---

Run a simple JavaScript sample on PasSy Gateway device running Yocto
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

This document describes how to connect PasSy Gateway device running Yocto with Azure IoT SDK. This multi-step process includes:
-   Configuring Azure IoT Hub
-   Registering your IoT device
-   Build and deploy Azure IoT SDK on device

<a name="Prerequisites"></a>
# Step 1: Prerequisites

You should have the following items ready before beginning the process:

-   [Prepare your development environment][setup-devbox-linux]
-   [Setup your IoT hub][lnk-setup-iot-hub]
-   [Provision your device and get its credentials][lnk-manage-iot-hub]
-   PasSy Gateway device.

<a name="PrepareDevice"></a>
# Step 2: Prepare your Device
    function IoTHubClient(protocol, connectionString){this
        var IoTProtocol;
        self = this;
        switch (protocol.toLowerCase()) {
            case 'amqp':
                logger.debug('IoTProtocol Amqp');
                IoTProtocol = require('azure-iot-device-amqp').Amqp;
                break;
            case 'http':
                logger.debug('IoTProtocol HTTP');
                IoTProtocol = require('azure-iot-device-http').Http;
                break;
            case 'mqtt':
                logger.debug('IoTProtocol Mqtt');
                IoTProtocol = require('azure-iot-device-mqtt').Mqtt;
                break;                
            default:
                logger.debug('IoTProtocol non definito');
                break;
        }
        var client = Client.fromConnectionString(connectionString, IoTProtocol);
        this.getClient = function(){ return client; }
        client.open(buildConnectCallback(client));
    }


<a name="Build"></a>
# Step 3: Build and Run the sample

<a name="Load"/>
## 3.1 Load the Azure IoT bits and prerequisites on device

-   Open a PuTTY session and connect to the device.

-   Run following command to check if NodeJS is already installed

        node --version

    If version is **0.12.x or greater**, then skip next step of installing prerequisite packages. Else uninstall it by issuing following command from command line on the device.{{***Keep the command set based on your OS and remove the rest.***}}

    {{**Yocto**}}

        sudo apt-get remove nodejs


-   Install the prerequisite packages by issuing the following commands from the command line on the device. Choose your commands based on the OS running on your device.{{***Keep the command set based on your OS and remove the rest.***}}

    {{**Yocto**}}

        curl -sL https://deb.nodesource.com/setup_4.x | sudo bash -

        sudo apt-get install -y nodejs

     
    **Note:** To test successful installation of Node JS, try to fetch its version information by running following command:

        node --version

-   Download the SDK to the board by issuing the following command in
    PuTTY:

        git clone https://github.com/Azure/azure-iot-sdks.git

-   Verify that you now have a copy of the source code under the directory ~/azure-iot-sdks.

<a name="BuildSamples"/>
## 3.2 Build the samples

-   To validate the source code run the following commands on the device.

        cd ~/azure-iot-sdks/node
        build/dev-setup.sh
        build/build.sh | tee LogFile.txt

    ***Note:*** *LogFile.txt in above command should be replaced with a file name where build output will be written.*

-   Edit the following file using any text editor of your choice:

    **For simple_sample_http.js:**

        cd ~/azure-iot-sdks/node/device/samples
        nano simple_sample_http.js

    **For send_batch_http.js:**

        cd ~/azure-iot-sdks/node/device/samples
        nano send_batch_http.js
        
    **For registry_sample.js:**

        cd ~/azure-iot-sdks/node/service/samples
        nano registry_sample.js

-   This launches a console-based text editor. Scroll down to the
    connection information.

-   Find the following place holder for IoT connection string:

        var connectionString = "[IoT Device Connection String]";

-   Replace the above placeholder with device connection string you obtained in [Step 1](#Step-1:-Prerequisites) and save the changes.

-   Run the following command before leaving the **~/azure-iot-sdks/node/device/samples** directory

        npm link azure-iot-device

<a name="Run"/>
## 3.3 Run and Validate the Samples

### 3.3.1 Send Device Events to IOT Hub:

-   Run the sample by issuing following command and verify that data has been successfully sent and received.

        node ~/azure-iot-sdks/node/device/samples/simple_sample_http.js

-   Run the sample by issuing following command and verify that data has been successfully sent and received.

        node ~/azure-iot-sdks/node/device/samples/send_batch_http.js

-   See [Manage IoT Hub][lnk-manage-iot-hub] to learn how to observe the messages IoT Hub receives from the application.

-   Run the sample to register a device by issuing following command. Verify that you receive information for new device created in the messages.

        node ~/azure-iot-sdks/node/service/samples/registry_sample.js

**Note:** The registry_sample.js sample will create and delete a device. In order to see it in the DeviceExplorer tool you will need to refresh your devices before the sample finishes running.

### 3.3.2 Receive messages from IoT Hub

-   See [Manage IoT Hub][lnk-manage-iot-hub] to learn how to send cloud-to-device messages to the application.

[setup-devbox-linux]: https://github.com/Azure/azure-iot-sdks/blob/master/doc/get_started/node-devbox-setup.md
[lnk-setup-iot-hub]: ../setup_iothub.md
[lnk-manage-iot-hub]: ../manage_iot_hub.md
