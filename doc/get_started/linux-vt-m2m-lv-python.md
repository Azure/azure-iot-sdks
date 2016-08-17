---
platform: custom linux
device: vt-m2m-lv
language: python
---

Run a simple PYTHON sample on VT-M2M-LV device running Custom Linux
===
---

# Table of Contents

-   [Introduction](#Introduction)
-   [Step 1: Prerequisites](#Prerequisites)
-   [Step 2: Prepare your Device](#PrepareDevice)
-   [Step 3: Build and Run the Sample](#Build)

# Introduction

**About this document**

This document describes how to connect VT-M2M-LV device running Custom Linux with Azure IoT SDK. This multi-step process includes:
-   Configuring Azure IoT Hub
-   Registering your IoT device
-   Build and deploy Azure IoT SDK on device

# Step 1: Prerequisites

You should have the following items ready before beginning the process:

-   [Prepare your development environment][setup-devbox-python]
-   [Setup your IoT hub][lnk-setup-iot-hub]
-   [Provision your device and get its credentials][lnk-manage-iot-hub]
-   VT-M2M-LV device.

# Step 2: Prepare your Device
   the URL for the device is http://vantrontech.com/hardwares/VT-M2M-LV.htm
   
   The Python module builds on the Azure IoT device SDK for C. Please make sure you can build the C samples as described in [setup-devbox] before you continue in this section.

The Python iothub_client supports python versions 2.7.x, 3.4.x or 3.5.x. Know the appropriate version you would like to build the library with for the following instructions.

 1. Ensure that the desired Python version (2.7.x, 3.4 or 3.5.x) is installed and active. Run `python --version` or `python3 --version` at the command line to check the version.
 2. Open a shell and navigate to the folder **python/build_all/linux** in your local copy of the repository.
 3. Run the `./setup.sh` script to install the prerequisite packages and the dependent libraries.
    * Setup will default to python 2.7
    * To setup dependencies for python 3.4 or 3.5, run `./setup.sh --python-version 3.4` or `./setup.sh --python-version 3.5` respectively
 4. Run the `./build.sh` script.
    * Build will default to python 2.7
    * To build with python 3.4 or 3.5, run `./build.sh --build-python 3.4` or `./build.sh --build-python 3.5` respectively 


# Step 3: Build and Run the sample
## 3.1 Build SDK and sample

-   Open a XShell session and connect to the device.

-   Install the prerequisite packages for the Microsoft Azure IoT Device SDK for Python by issuing the following commands from the command line on your board:

        sudo apt-get update

        sudo apt-get install -y curl libcurl4-openssl-dev build-essential cmake git python2.7-dev libboost-python-dev

-   Download the Microsoft Azure IoT Device SDK to the board by issuing the following command on the board::

        git clone --recursive https://github.com/Azure/azure-iot-sdks.git

-   Run following commands to build the SDK:

        cd python/build_all/linux
	    sudo ./build.sh    

-   After a successful build, the `iothub_client.so` Python extension module is copied to the **python/device/samples** folder.

- Navigate to samples folder by executing following command:

        cd azure-iot-sdks/python/device/samples/

-   Edit the following file using any text editor of your choice:
    
        nano iothub_client_sample_mqtt.py

-   Find the following place holder for device connection string:

        connectionString = "[device connection string]"

-   Replace the above placeholder with device connection string you obtained in [Step 1](#Prerequisites) and save the changes.

## 3.2 Send Device Events to IoT Hub:

-   Run the sample application using the following command:

        python iothub_client_sample_mqtt.py

-   See [Manage IoT Hub][lnk-manage-iot-hub] to learn how to observe the messages IoT Hub receives from the application.

## 3.3 Receive messages from IoT Hub

-   See [Manage IoT Hub][lnk-manage-iot-hub] to learn how to send cloud-to-device messages to the application.

[setup-devbox-python]: https://github.com/Azure/azure-iot-sdks/blob/master/doc/get_started/python-devbox-setup.md
[lnk-setup-iot-hub]: ../../setup_iothub.md
[lnk-manage-iot-hub]: ../../manage_iot_hub.md
