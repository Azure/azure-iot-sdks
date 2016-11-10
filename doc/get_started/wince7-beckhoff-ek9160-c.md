---
platform: windows ce 7
device: ek9160
language: c
---

Run a simple C sample on EK9160 device running Windows CE 7
===
---

# Table of Contents

-   [Introduction](#Introduction)
-   [Step 1: Prerequisites](#Prerequisites)
-   [Step 2: Prepare your Device](#PrepareDevice)


<a name="Introduction"></a>
# Introduction

**About this document**

This document describes how to connect EK9160 device running Windows CE 7 with Azure IoT SDK. This multi-step process includes:
-   Configuring Azure IoT Hub
-   Registering your IoT device
-   Build and deploy Azure IoT SDK on device

<a name="Prerequisites"></a>
# Step 1: Prerequisites

You should have the following items ready before beginning the process:

- [Setup your IoT hub][lnk-setup-iot-hub]
- [Provision your device and get its credentials][lnk-manage-iot-hub]
- Attach the EK9160 to your network by using the integrated Ethernet port
- Ensure that the EK9160 retrieved an IP address from a DHCP server

<a name="PrepareDevice"></a>
# Step 2: Prepare your Device

## Configure EK9160 for Azure IoT Hub by using the integrated webserver

1.  Open the device configuration website by starting your web browser and navigating to http://deviceIpAddress/config.
    Replace deviceIpAddress with the IP address of the EK9160.

2.  On the left navigation bar, open the IoT configuration

3.  Add a new IoT service and select "Microsoft Azure IoT Hub" as the IoT service type

4.  Enter all required IoT Hub connection details and device credentials

## Activate I/O signals for data communication

1.  On the left navigation bar, open the EtherCAT configuration

2.  Select the input terminal and the corresponding channel and link it to the previously created IoT service

3.  Set connection parameters like sampling rate and data format that you would like to use

## Documentation

The documentation can be found [here](http://infosys.beckhoff.com).

[lnk-setup-iot-hub]: ../setup_iothub.md
[lnk-manage-iot-hub]: ../manage_iot_hub.md
