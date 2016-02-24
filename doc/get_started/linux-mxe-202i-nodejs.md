---
platform: linux
device: mxe-202i
language: nodejs
---

Run a simple C sample on MXE-202i device running Linux
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

This document describes how to connect MXE-202i device running Linux with Azure IoT SDK. This multi-step process includes:
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
-   Follow the instructions from [device website](http://www.adlinktech.com/PD/web/PD_detail.php?pid=1589).

<a name="Build"></a>
# Step 3: Build and Run the sample

<a name="Load"/>
## 3.1 Load the Azure IoT bits and prerequisites on device

-   Open a PuTTY session and connect to the device.

-   Choose your commands in next steps based on the OS running on your device.

-   Run following command to check if NodeJS is already installed

        node --version

    If version is **0.12.x or greater**, then skip next step of installing prerequisite packages. Else uninstall it by issuing following command from command line on the device.

        sudo apt-get remove nodejs

-   Install the prerequisite packages by issuing the following commands from the command line on the device. Choose your commands based on the OS running on your device.
    
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

-   Edit the following file using any text editor of your choice:

        cd ~/azure-iot-sdks/node/device/samples

    **For simple_sample_http.js:**

        simple_sample_http.js

    **For send_batch_http.js:**

        send_batch_http.js
   
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

-   On Windows, refer "Send cloud-to-device messages" in [DeviceExplorer Usage document][lnk-device-explorer] for instructions on sending messages to device.

-   If you are running other OS, please use the JavaScript tool [iot-hub explorer tool][lnk-iothub-explorer]

-   Run the sample to register a device by issuing following command. Verify that you receive information for new device created in the messages.

        node ~/azure-iot-sdks/node/service/samples/registry_sample.js

**Note:** The registry_sample.js sample will create and delete a device. In order to see it in the DeviceExplorer tool you will need to refresh your devices before the sample finishes running.

### 3.3.2 Receive messages from IoT Hub

-   On Windows, refer "Send cloud-to-device messages" in [DeviceExplorer Usage document][lnk-device-explorer] for instructions on sending messages to device.

-   If you are running other OS, please use the JavaScript tool [iot-hub explorer tool][lnk-iothub-explorer]

[lnk-setup-iot-hub]: ../setup_iothub.md
[lnk-manage-iot-hub]: ../manage_iot_hub.md
[lnk-device-explorer]: ../../tools/DeviceExplorer/doc/how_to_use_device_explorer.md
[lnk-iothub-explorer]: ../../tools/iothub-explorer/readme.md
