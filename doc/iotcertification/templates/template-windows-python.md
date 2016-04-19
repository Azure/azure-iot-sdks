---
platform: {enter the OS name running on device}
device: {enter your device name here}
language: python
---

Run a simple python sample on {enter your device name here} device running {enter the OS name running on device}
===
---

# Table of Contents

-   [Introduction](#Introduction)
-   [Step 1: Prerequisites](#Prerequisites)
-   [Step 2: Prepare your Device](#PrepareDevice)
-   [Step 3: Setup your development environment](#Environment)
-   [Step 4: Build and Run the Sample](#Build)

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

-   [Prepare your development environment][setup-devbox-python]
-   [Setup your IoT hub][lnk-setup-iot-hub]
-   [Provision your device and get its credentials][lnk-manage-iot-hub]
-   {enter your device name here} device.
-   {{Please specify if any other software(s) or hardware(s) are required.}}

<a name="PrepareDevice"></a>
# Step 2: Prepare your Device
-   {{Write down the instructions required to setup, configure and connect your device. Please use external links when possible pointing to your own page with device preparation steps.}}

<a name="Build"></a>
# Step 3: Build and Run the sample

<a name="Load"></a>
## 3.1 Build SDK and sample

-   Download latest SDK using following command:

		git clone --recursive https://github.com/Azure/azure-iot-sdks.git

- Ensure that the desired Python version is installed (2.7.x, 3.4.x or 3.5.x). Run python --version or python3 --version at the command line to check the version. 

- Open a Visual Studio 2015 x86 Native Tools command prompt and navigate to the folder python/build_all/windows in your local copy of the repository.

- Run the script build.cmd in the python\build_all\windows directory.

- As a result, the **iothub_client.pyd** Python extension module is copied to the python/device/samples folder.

- Navigate to the folder python/device/samples in your local copy of the repository.

- Open the file **iothub_client_sample_amqp.py**, **iothub_client_sample_http.py** or  **iothub_client_sample_mqtt.py** in a text editor.

- Locate the following code in the file:

		connectionString = "[device connection string]"

-   Replace [device connection string] with the connection string for your device. Save the changes.

## 3.2 Send Device Events to IoT Hub:

-   Run the sample application using the following command through Visual Studio 2015 x86 Native Tools command prompt and navigate to the folder python/build_all/windows in your local copy of the repository:
{{***Keep the command set based on your protocol(s) and remove the rest.***}}

    {{**If using AMQP protocol:**}}

      	python iothub_client_sample_amqp.py

    {{**If using HTTPS protocol:**}}

       	python iothub_client_sample_http.py

    {{**If using MQTT protocol:**}}

       	python iothub_client_sample_mqtt.py

-   See [Manage IoT Hub][lnk-manage-iot-hub] to learn how to observe the messages IoT Hub receives from the application.

## 3.3 Receive messages from IoT Hub

-   See [Manage IoT Hub](https://github.com/Azure/azure-iot-sdks/blob/develop/doc/manage_iot_hub.md) to learn how to send cloud-to-device messages to the application.

[setup-devbox-python]: <https://github.com/Azure/azure-iot-sdks/blob/master/doc/get_started/python-devbox-setup.md>
[lnk-setup-iot-hub]: ../../setup_iothub.md
[lnk-manage-iot-hub]: ../../manage_iot_hub.md