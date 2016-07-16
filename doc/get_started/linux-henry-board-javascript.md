---
platform: yocto
device: henry-board
language: javascript
---

Run a simple JavaScript sample on henry-board device running Yocto
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

This document describes how to connect henry-board device running Yocto with Azure IoT SDK. This multi-step process includes:
-   Configuring Azure IoT Hub
-   Registering your IoT device
-   Build and deploy Azure IoT SDK on device

<a name="Prerequisites"></a>
# Step 1: Prerequisites

You should have the following items ready before beginning the process:

-   [Prepare your development environment][setup-devbox-linux]
-   [Setup your IoT hub][lnk-setup-iot-hub]
-   [Provision your device and get its credentials][lnk-manage-iot-hub]
-   henry-board device.
-   Intel Edison breakout-board kit.

<a name="PrepareDevice"></a>
# Step 2: Prepare henry-board

-   Ensure the latest OS image has been installed. To do that, follow these instructions on Intel's official documentation [portal][IntelEdisonGettingStarted].
-   After following the standard Intel Edison setup instructions, you will need to establish a [serial connection][IntelEdisonSerialConnection] to your device from your machine.
-   Remove the intel edison compute module from Intel Edison breakout-board kit, attach to henry board.
-   Once you have established a serial connection (command line) to your Intel Edison board, you can proceed to install Azure IoT SDK using the below instructions.
-   Make sure you have run the "configure_edison ?setup" command to set up your board.
-   Make sure your Intel Edison is online via your local Wi-Fi network (should occur during configure_edison setup).

## Installing Git on henry-board

Git is a widely used distributed version control tool, we will need to install Git on the Intel Edison in order to clone the Azure IoT SDK and build it locally. To do that, we must first add extended packages to Intel Edison which include Git. Intel Edison's build of [Yocto Linux][yocto] uses the "opkg" manager which doesn't, by default, include Git support.

-   Update and upgrade your opkg base-feeds on your command line:

    ```
    $ opkg update
    $ opkg install nodejs
    ```

## Downloading the Azure IoT SDK to your Intel Edison

-   On your Intel Edison, use Git and clone the Azure SDK repository directly to your Edison using the following commands. We recommend using the default folder located in /home/root:

    ```
    $ opkg install git
    $ git clone --recursive https://github.com/Azure/azure-iot-sdks.git
    ```

-   You may be prompted to add an RSA key to your device, respond with "yes".

-   Alternate Deploy Method

    -   If for any reason you are unable to clone the Azure IoT SDK directly to Edison, you can clone the repository to your PC / Mac / Linux desktop and then transfer the files over the network to your Intel  Edison using [FileZilla][filezilla] or SCP.
    -   For FileZilla, run "wpa_cli status" on your Intel Edison to find your IP address, then use "sftp://your.ip.address", use password "root" and your Intel Edison password to establish an SFTP connection via FileZilla. Once you have done that, you can drag and drop files over the network directly.


<a name="Build"></a>
# Step 3: Build and Run the sample

This section walks you through building, deploying and validating the IoT Client SDK on your device running a Linux operating system. You will install necessary prerequisites on your device.  Once done,  you will build and deploy the IoT Client SDK and validate the sample tests required for IoT certification with the Azure IoT SDK.

<a name="Load"/>
## 3.1 Load the Azure IoT bits and prerequisites on device

-   Open a PuTTY session and connect to the device.

-   Choose your commands in next steps based on the OS running on your device.

-   Run following command to check if NodeJS is already installed

       $ node --version

    If version is **0.12.x or greater**, then skip next step of installing prerequisite packages. Else uninstall it by issuing following command from command line on the device.

       $ opkg remove nodejs

-   Install the prerequisite packages by issuing the following commands from the command line on the device. Choose your commands based on the OS running on your device.

       $ opkg install nodejs
     
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
[IntelEdisonGettingStarted]: https://software.intel.com/iot/library/edison-getting-started
[IntelEdisonSerialConnection]: https://software.intel.com/setting-up-serial-terminal-intel-edison-board
[yocto]: http://www.yoctoproject.org/docs/latest/adt-manual/adt-manual.html
[filezilla]: https://filezilla-project.org
