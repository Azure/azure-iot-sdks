---
platform: yocto linux
device: almach
language: c
---

Run a simple C sample on Almach device running Yocto Linux
===
---

# Table of Contents

-   [Introduction](#Introduction)
-   [Step 1: Prerequisites](#Prerequisites)
-   [Step 2: Prepare your Device](#PrepareAlmach)
-   [Step 3: Build and Run the Sample](#Build)
-   [Tips](#tips)

<a name="Introduction"></a>
# Introduction

**About this document**

This document describes how to connect Almach device running Yocto Linux with Azure IoT SDK. This multi-step process includes:
-   Configuring Azure IoT Hub
-   Registering your IoT device
-   Build and deploy Azure IoT SDK on device

<a name="Prerequisites"></a>
# Step 1: Prerequisites

You should have the following items ready before beginning the process:

-   Computer with Git client installed and access to the
    [azure-iot-sdks](https://github.com/Azure/azure-iot-sdks) GitHub
    public repository.
-   Almach device
-   For wired connection, [USB Ethernet board](http://www.e-consystems.com/DM3730-development-board.asp#accessories)
-   PC to build Yocto Image
-   Terminal Emulator software
-   [Setup your IoT hub][lnk-setup-iot-hub]
-   [Provision your device and get its credentials][lnk-manage-iot-hub]

<a name="PrepareAlmach"></a>
# Step 2: Prepare Almach
-   Prepare the Yocto Root file system for your Almach board by following the instructions in the "esom3730_yocto_user_manual.pdf".
-   Apply the patch file "azure.patch" to yocto source directory in build PC
-   Add the below line to local.conf ( $YOCTO_INSTALL_PATH/build_esom/conf/local.conf )

    ```
    IMAGE_INSTALL_append += "packagegroup-esom-azure"
    ```

-   Run the below command after setting up yocto environment

    ```
    bitbake esom3730-console-image
    ```

-   Once the image is built, flash the boot image and rootfs to the SD card as per the User manual.
-   Connect Almach to your desktop machine using the USB mini cable and open a terminal emulator program such as [Tera Term](https://en.osdn.jp/projects/ttssh2/releases/) on your desktop and switch ON the device. You have to create a new Serial port connection from Setup->Serial Port and set the parameters:  
    * Port number where you have connected the device to PC.
    * Baud Rate = 115200
    * Data = 8 bits
    * Parity = noneStop = 1 bit
    * Flow Control = none
-   When prompted, log in with username **root**.
-   To connect Almach with a Wired network, use the **USB Ethernet Board**. For Wireless connection, Almach have it's own WiFi module so there is no need to connect any WiFi dongle. Follow the steps under **Ethernet** or **WiFi** for the corresponding connection type from the "esom3730_yocto_peripheral_test_user_manual.pdf" document to connect with the network.

<a name="Build"></a>
# Step 3: Build and Run the sample

You can build the azure-iot-sdk and samples directly in Almach or from PC where you can build the samples and then flash the files to the board and use them there. All the prerequisite packages for the Microsoft Azure IoT Device SDK for C have been built using the patch file mentioned in the above step.

<a name="Load"></a>
## 3.1 Build SDK and sample in Almach

-   Download the Microsoft Azure IoT Device SDK for C in PC
-   Copy it to the **root file system path "/home/root"** in Almach boot SD card along with **azure-shared-utility** ( which is to be copied under "azure-iot-sdk/c" folder) and then insert the SD card to device
-   Open a TeraTerm session, connect it with the device and then power ON the device.
-   After logging in to the command prompt terminal, check the presence of the azure-iot-sdk folder in the path ***"$HOME" (/home/root)*** which we have copied from PC.
-   Edit the following file using **vi text editor**:

    **For AMQP protocol:**

        vi azure-iot-sdks/c/iothub_client/samples/iothub_client_sample_amqp/iothub_client_sample_amqp.c

    **For HTTPS protocol:**

        vi azure-iot-sdks/c/iothub_client/samples/iothub_client_sample_http/iothub_client_sample_http.c

-   Find the following place holder for IoT connection string:

        static const char* connectionString = "[device connection string]";

-   Replace the above placeholder with device connection string you obtained in [Step 1](#Step-1:-Prerequisites) and save the changes.

-   You can now build the SDK samples using the following command from ***$HOME***:

        ./azure-iot-sdks/c/build_all/linux/build.sh

## 3.2 Build SDK and sample in PC

-   Download the Microsoft Azure IoT Device SDK for C in the Yocto build PC path

    ```
    {$YOCTO_INSTALL_PATH}/poky-fido-13.0.0/meta-esom3730/recipes-connectivity/azure-iot/files/
    ```

-   Edit the following file using any text editor of your choice:

    **For AMQP protocol:**

        azure-iot-sdks/c/iothub_client/samples/iothub_client_sample_amqp/iothub_client_sample_amqp.c

    **For HTTPS protocol:**

        azure-iot-sdks/c/iothub_client/samples/iothub_client_sample_http/iothub_client_sample_http.c

-   Find the following place holder for IoT connection string:

        static const char* connectionString = "[device connection string]";

-   Replace the above placeholder with device connection string you obtained in [Step 1](#Step-1:-Prerequisites) and save the changes.

-   You can now build the SDK samples using the following command from ***{$YOCTO_INSTALL_PATH}/build_esom***:
    ```
    bitbake azure-iot
    ```
-   Create a folder named "cmake" in ***"$HOME"*** of the Root file system in Almach SD card.
-   After the build process gets completed, copy the generated files from the following folder in build PC, to the "cmake" folder of the above step.

    ```
    {$YOCTO_INSTALL_PATH}/build_esom/tmp/work/cortexa8-vfp-neon-poky-linux-gnueabi/azure-iot/1.0-r0/build/
    ```

    Once the copy process gets completed, insert the card to Almach.
-   Open a TeraTerm session, connect it with the device and then power ON the device
-   After logging in to the command prompt terminal, check the presence of the **cmake** folder in the path ***"$HOME" (/home/root)*** which we have copied from PC.

## 3.3 Send Device Events to IoT Hub:

-   Run the sample by issuing following command:

    **If using AMQP protocol:**

        azure-iot-sdks/c/cmake/iotsdk_linux/iothub_client/samples/iothub_client_sample_amqp/iothub_client_sample_amqp

    **If using HTTPS protocol:**

        azure-iot-sdks/c/cmake/iotsdk_linux/iothub_client/samples/iothub_client_sample_http/iothub_client_sample_http

-   See [Manage IoT Hub][lnk-manage-iot-hub] to learn how to observe the messages IoT Hub receives from the application.

## 3.4 Receive messages from IoT Hub

-   See [Manage IoT Hub][lnk-manage-iot-hub] to learn how to send cloud-to-device messages to the application.

<a name="tips"></a>
# Tips

- If you just want to build the serializer samples, run the following commands:

  ```
  cd ./c/serializer/build/linux
  make -f makefile.linux all
  ```

[lnk-setup-iot-hub]: ../setup_iothub.md
[lnk-manage-iot-hub]: ../manage_iot_hub.md
