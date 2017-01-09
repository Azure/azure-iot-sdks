---
platform: linux (msc-ldk)
device: q7-imx6plus
language: c
---

Run a simple C sample on Q7-IMX6Plus device running MSC-LDK
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

This document describes how to connect Q7-IMX6Plus device running MSC-LDK with Azure IoT SDK. This multi-step process includes:
-   Configuring Azure IoT Hub
-   Registering your IoT device
-   Build and deploy Azure IoT SDK on device

<a name="Prerequisites"></a>
# Step 1: Prerequisites

You should have the following items ready before beginning the process:

-   [Prepare your development environment][setup-devbox-linux]
-   [Setup your IoT hub][lnk-setup-iot-hub]
-   [Provision your device and get its credentials][lnk-manage-iot-hub]
-   Q7-IMX6Plus device.
-   MSC-LDK (Linux Development Kit)

<a name="PrepareDevice"></a>
# Step 2: Prepare your Device

Follow the instructions in the generic chapter "Getting Started" of the [devices user manual][link_manual] for Baytrail.

<a name="Build"></a>
# Step 3: Build and Run the sample

<a name="Load"></a>
## 3.1 Build SDK and sample

-   Open a PuTTY session and connect to the device.

-   The Microsoft Azure IoT Device SDK for C is already installed in the standard images as the package "azure-iot-sdks-samples".

-   Build the SDK using following command:

        make msc-image-sato
    
    or
    
        make msc-image-lxqt

## 3.2 Send Device Events to IoT Hub:

-   Run the sample by issuing following command:

    **For HTTP:**

        iothub_client_sample_http <device connection string>
        
- Replace the above placeholder "&lt;device connection string&gt;" with device connection string you obtained in [Step 1](#Step-1:-Prerequisites) and save the changes.

-   See [Manage IoT Hub][lnk-manage-iot-hub] to learn how to observe the messages IoT Hub receives from the application.

## 3.3 Receive messages from IoT Hub

-   See [Manage IoT Hub][lnk-manage-iot-hub] to learn how to send cloud-to-device messages to the application.

<a name="tips"></a>
# Tips

- If you just want to build the serializer samples, run the following commands:

        ./build.sh bitbake -c cleanall azure-iot-sdks
        ./build.sh bitbake -c compile azure-iot-sdks
        ./build.sh bitbake -c devshell azure-iot-sdks

  and in the devshell
  
  
[setup-devbox-linux]: https://github.com/Azure/azure-iot-sdk-c/blob/master/doc/devbox_setup.md
[lnk-setup-iot-hub]: ../setup_iothub.md
[lnk-manage-iot-hub]: ../manage_iot_hub.md
[link_manual]: http://www.msc-technologies.eu/fileadmin/documentpool/Support-Center/Qseven/Q7-BT/20-Drivers/80-Linux/MSC_Q7-BT-ldk-bsp-C984.pdf 
