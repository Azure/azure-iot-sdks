---
platform: ubuntu
device: G2021ES
language: c
---

Run a simple C sample on G2021ES device running Linux Kernel 3.14 Ubuntu distribution
===
---

# Table of Contents

-   [Requirements]
-   [Step 1: Before you begin]
-   [Step 2: Prepare G2021ES]
-   [Step 3: Build and Run the HTTPS Sample]
-   [Tips]

<a name="Requirements"></a>
-   SSH client on your desktop computer, such as PuTTY, so you can remotely access the command line on the G2021ES device.
-   WinSCP tool on your desktop to copy data from your desktop to G2021ES device.

**About this document**
This document describes how to connect G2021ES device running Linux Kernel 3.14 Ubuntu distribution with Azure IoT SDK. This multi-step process includes:
-   Configuring Azure IoT Hub
-   Registering your IoT device
-   Build and deploy Azure IoT SDK on device

<a name="Before you begin></a>
You should have the following items ready before beginning the process:

-   Computer with Git client installed and access to the azure-iot-sdks(https://github.com/Azure/azure-iot-sdks) GitHub public repository.
-   G2021ES device. 
-   Install the cross tool chain poky-eglibc-i686-g2021es-console-basic-cortexa8t2hf-vfp-neon-toolchain-1.6.3.sh on your desktop, this is required to build and run applications on G2021ES device. 
-   [Prepare your development environment][setup-devbox-linux]
-   [Setup your IoT hub][lnk-setup-iot-hub]
-   [Provision your device and get its credentials][lnk-manage-iot-hub]
-   Make note of the Connection String for your device by following the instructions in **"Get device connection string or configuration data"** section of [DeviceExplorer Usage document](https://github.com/Azure/azure-iot-sdks/blob/master/tools/DeviceExplorer/doc/how_to_use_device_explorer.md).
Note: You can skip this step if you just want to build the sample application without running it.

<a name="Prepare G2021ES Device"></a>
-   Connect your G2021ES Device to your network using an Ethernet cable or access the device by using 3g.
-   You need to determine the IP address of your G2021ES Device in order to connect over the network.
-   Once you see that your Device is working, open an SSH terminal program such as PuTTY on your desktop machine.
-   Use the IP address as the Host name, Port=22, and Connection type=SSH to complete the connection.
-   When prompted, log in with user name as admin, and password as admin.

<a name="Build"></a>
# Step 3: Build and Run the HTTPS sample

<a name="Load"></a>
## 3.1 Build SDK and sample

-   Open a PuTTY session and connect to the device.

-   Install the prerequisite packages like Curl library for the Microsoft Azure IoT Device SDK for C from the command line on your board:
    
-   Download the Microsoft Azure IoT Device SDK for C to the desktop by issuing the following command::

        git clone https://github.com/Azure/azure-iot-sdks.git
		
-   Build the sample application with the tool chain poky-eglibc-i686-g2021es-console-basic-cortexa8t2hf-vfp-neon-toolchain-1.6.3.sh (contact systems@pacificcontrols.net for tool chain) on your desktop by using the makefile.

-   Keep the binary file generated from above step in the G2021ES device folder /opt/Azure through the help of WinSCP.


## 3.2 Run the sample by using following steps:

-   Default IP ip of G2021ES gateway is 172.16.0.16X (where x is last digit of the device serial number).

-   In the device configuration and administration page go to the Services tab and select the Cloud Connectors option.

-   In the Cloud Connector option select Azure Cloud Connector option and enable the same.

-   Fill the Host name, Device ID, Shared Access Key from device connection string you obtained in [Step 1](#Step-1:-Before you begin) and click submit button.

-   Now your sample application starts running.

## 3.3 Send Device Events to IoT Hub:   

-   See [Manage IoT Hub][lnk-manage-iot-hub] to learn how to observe the messages IoT Hub receives from the application.

## 3.4 Receive messages from IoT Hub

-   See [Manage IoT Hub][lnk-manage-iot-hub] to learn how to send cloud-to-device messages to the application.

<a name="tips"></a>
# Tips

-   By executing the top command in the G2021ES device command line, you can verify whether your sample application is running or not.
-	The “G2021ES” is a purpose built, cost effective, multi-technology gateway hosted with edge analytics capability, rich set of wired and wireless communication channels and protocols and on board input/output. The product design, form factors suitable for industrial extended vibrations and temperature ranges. The product is designed for building and factory automation sectors, OEM asset management in automotive, industrial and mining sectors.
-	The gateway G2021ES - ensures interoperability with fieldbus-based industrial networks, allowing data communication using Modbus RTU/TCP and Profibus* protocols, SAE automotive interoperability with industry adopted SAE CAN J1939, J1708, J1587 protocols and BACNet in building automation spectrum.
-	To build end-to-end connectivity from the edge to the cloud, G2021ES gateway support a wide variety of field communication protocols which are wired and wireless in nature and of course the cloud communication link via cellular, Wi-Fi and 10/100 Bate-T WAN interface. As the devices or device networks to exchanging data over local radio frequency networks, the gateway designed with various RF technologies including ZigBee and LoRa used to maximize the availability of the communication link for field senor connectivity.Pacific Controls G2021ES gateways provides an embedded device framework which has a layered south bound protocol stack (Modbus TCP/IP and RTU, BACnet IP and MSTP, CAN J1939, J1708 and many more), control engine for edge analytics.
-	For addtion information and support, please get in touch with systems@pacificcontrols.net


[setup-devbox-linux]: https://github.com/Azure/azure-iot-sdks/blob/master/c/doc/devbox_setup.md
[lnk-setup-iot-hub]: ../setup_iothub.md
[lnk-manage-iot-hub]: ../manage_iot_hub.md
