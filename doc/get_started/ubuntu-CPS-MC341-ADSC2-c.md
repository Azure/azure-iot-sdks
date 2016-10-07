---
platform: Ubuntu 10.04LTS
device: CPS-MC341-ADSC2
language: c
---

Run a simple C sample on CPS-MC341-ADSC2 device running Ubuntu 10.04LTS
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

This document describes how to connect CPS-MC341-ADSC2 device running  ubuntu10.04LTS with Azure IoT SDK. This multi-step process includes:
-   Configuring Azure IoT Hub
-   Registering your IoT device
-   Build and deploy Azure IoT SDK on device

<a name="Prerequisites"></a>
# Step 1: Prerequisites

You should have the following items ready before beginning the process:

  -  By connecting a serial cable from host PC to CONPROSYS through the serial port (Diameter 3.5 MINI-JACK), you can log in from the console to the product.
See the setting details below.
     - Baud rate:115200 bps
     - Data bit:8bit
     - Parity:none
     - Stop bit:1bit
     - Hardware flow:none

- We recommend the following serial cable for connecting the host PC to CONPROSYS.Download appropriate driver software to match OS of PC serial monitoring.
    - TTL-232R-3V3 AJ by FTDI (included when purchasing the package version)
    - URL for driver can be found [here](<http://www.ftdichip.com/Drivers/VCP.htm>)
    -  Default log-in and password are
        - Log-in:conprosys
        - Password:contec

-   Computer with Git client installed and access to the
    [azure-iot-sdks](https://github.com/Azure/azure-iot-sdks) GitHub
    public repository.
-   CPS-MC341-ADSC2 device.
-   Download and install [DeviceExplorer](https://github.com/Azure/azure-iot-sdks/releases/download/2015-11-13/SetupDeviceExplorer.msi).
-   [Set up your IoT hub](https://github.com/Azure/azure-iot-sdks/blob/master/doc/setup_iothub.md).
#### Create a device on IoT Hub
-   With your IoT hub configured and running in Azure, follow the instructions in **"Create Device"** section of [DeviceExplorer Usage document](https://github.com/Azure/azure-iot-sdks/blob/master/tools/DeviceExplorer/doc/how_to_use_device_explorer.md).
#### Write down device credentials
-   Make note of the Connection String for your device by following the instructions in **"Get device connection string or configuration data"** section of [DeviceExplorer Usage document](https://github.com/Azure/azure-iot-sdks/blob/master/tools/DeviceExplorer/doc/how_to_use_device_explorer.md).

<a name="PrepareDevice"></a>
# Step 2: Prepare your Device
-   Default network setting is listed below:
      -   LAN A (eth0):  10.1.1.101
      -   LAN B (eth1):  192.168.1.101    (for 2 LAN Type setting only)

- If you would like to change the network setting, edit the file on target at location **etc/network/interfaces**

        Example)  Set "eth0" to "dhcp".
        auto eth0
        iface eth0 inet dhcp

-   On the board, system reboot.

-   Set the time synchronization using the ntpdate command.

-   Install the uuid-dev

        sudo apt-get install uuid-dev

<a name="Build"></a>
# Step 3: Build and Run the sample

<a name="Load"></a>
## 3.1 Build SDK and sample

-   Download the Microsoft Azure IoT Device SDK for C to the board by issuing the following command on the board::

	git clone --recursive https://github.com/Azure/azure-iot-sdks.git

-   Edit the following file using vi editor:

    {{**For AMQP protocol:**}}

        azure-iot-sdks/c/iothub_client/samples/iothub_client_sample_amqp/iothub_client_sample_amqp.c

    {{**For HTTPS protocol:**}}

        azure-iot-sdks/c/iothub_client/samples/iothub_client_sample_http/iothub_client_sample_http.c

-   Find the following place holder for device connection string:

        static const char* connectionString = "[device connection string]";

-   Replace the above placeholder with device connection string you obtained in [Step 1](#Step-1:-Prerequisites) and save the changes.

-   Set the time synchronization using the ntpdate command.

-   Edit the following file using vi editor:

        azure-iot-sdks/c/build_all/linux/build.sh

-   Find the following place holder:

        CORES=$(grep -c ^processor /proc/cpuinfo 2>/dev/null || sysctl -n hw.ncpu)

-   Edit as follows, and then save the changes:

        CORES=1

-   Edit the following file using vi editor:

        azure-iot-sdks/c/c-utility/tests/condition_unittests/CMakeLists.txt

-   Add, and then save the changes:

        Set(CMAKE_CXX_FLAGS "-lrt")

-   Build the SDK samples using the following command:

        sudo -E ./azure-iot-sdks/c/build_all/linux/build.sh 

## 3.2 Send Device Events to IoT Hub:

-   Run the sample by issuing following command:

    {{**If using AMQP protocol:**}}

        azure-iot-sdks/c/cmake/iotsdk_linux/iothub_client/samples/iothub_client_sample_amqp/iothub_client_sample_amqp

    {{**If using HTTPS protocol:**}}

        azure-iot-sdks/c/cmake/iotsdk_linux/iothub_client/samples/iothub_client_sample_http/iothub_client_sample_http

-   On Windows, refer "Monitor device-to-cloud events" in [DeviceExplorer Usage document](https://github.com/Azure/azure-iot-sdks/blob/master/tools/DeviceExplorer/doc/how_to_use_device_explorer.md) to see the data your device is sending.

## 3.3 Receive messages from IoT Hub

-   On Windows, refer "Send cloud-to-device messages" in [DeviceExplorer Usage document](https://github.com/Azure/azure-iot-sdks/blob/master/tools/DeviceExplorer/doc/how_to_use_device_explorer.md) for instructions on sending messages to device.

