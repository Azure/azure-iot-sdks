---
platform: openwrt (chaos calmer, r48884)
device: iomote sensor box
language: javascript
---

Configure IOMote Sensor Box device running  OpenWRT (Chaos Calmer, r48884)
===
---

# Table of Contents

-   [Introduction](#Introduction)
-   [Step 1: Prerequisites](#Prerequisites)
-   [Step 2: Prepare your Device](#PrepareDevice)
-   [Step 3: Build](#Build)

<a name="Introduction"></a>
# Introduction

**About this document**

This document describes how to configure **IOMote Sensor Box** device running OpenWRT (Chaos Calmer, r48884) with Azure IoT SDK. This multi-step process includes:
-   Configuring Azure IoT Hub
-   Registering your IoT device
-   Configure your device
-   Use your device

**[IOMote Sensor Box](http://www.iomote.com/iomotesensorbox.php)**

A plug-and-play device for remote monitoring and management of industrial equipment with Modbus RTU, Modbus TCP, RS-485 or CAN bus using the Azure IoT platform.

<a name="Prerequisites"></a>
# Step 1: Prerequisites

You should have the following items ready before beginning the process:

-   [Prepare your development environment][setup-devbox-linux]
-   [Setup your IoT hub][lnk-setup-iot-hub]
-   [Provision your device and get its credentials][lnk-manage-iot-hub]
-   [IOMote Sensor Box](http://www.iomote.com/iomotesensorbox.php) device
-   [IOMote Devices Configurator Software](http://www.iomote.com/software/configurator.php) to configure Azure IoT Hub device connection string and Network Parameters

<a name="PrepareDevice"></a>
# Step 2: Prepare your Device

Please download the [IOMote Devices Configurator Software](http://www.iomote.com/software/configurator.php) to configure Azure IoT Hub device connection string and Network Parameters. 

## 2.1 Prepare the device connection string

-   Login to your IoT Hub portal and create a new device, following the instructions at [Provision your device and get its credentials][lnk-manage-iot-hub]. At the end of the process, you will have a unique Device Connection String related to your **IOMote Sensor Box** device. Please, take note of such string.

## 2.2 IOMote Devices Configurator Software

-   Using the provided software you have to configure a brand new device. Click on the *New Device* button and follow the step-by-step instructions

    ![](images/iomote-conf-01.png)

-   Fill the configuration form
    -   Paste your *Device Connection String* on the related input box
    -   *If you are using a Wi-Fi device*, please fill the SSID and Password fields with your network parameters
    -   Choose the DHCP method to use

    ![](images/iomote-conf-02.png)

<a name="Build"></a>
## Step 3: Build

-   Your device is now ready to be used with Azure IoT Hub. It will automatically send timed data to your Azure IoT Hub account.
-   See [Manage IoT Hub][lnk-manage-iot-hub] to learn how to send cloud-to-device messages to the application.

[setup-devbox-linux]: https://github.com/Azure/azure-iot-sdks/blob/master/doc/get_started/node-devbox-setup.md
[lnk-setup-iot-hub]: ../../setup_iothub.md
[lnk-manage-iot-hub]: ../../manage_iot_hub.md
