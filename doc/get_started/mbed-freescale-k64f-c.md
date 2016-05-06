---
platform: mbed
device: freescale-frdm-k64f
language: c
---

Run a simple C sample on Freescale FRDM-K64F device running mbed
===
---

# Table of Contents

-   [Introduction](#Introduction)
-   [Step 1: Prerequisites](#Step-1-Prerequisites)
-   [Step 2: Prepare your Device](#Step-2-PrepareDevice)
-   [Step 3: Build and Run the Sample](#Step-3-Build)
-   [Tips](#tips)

<a name="Introduction"></a>
# Introduction

**About this document**

This document describes the steps for connecting an [mbed-enabled Freescale FRDM-K64F](https://developer.mbed.org/platforms/FRDM-K64F/) device to Azure IoT Hub. This multi-step process includes:
-   Configuring Azure IoT Hub
-   Registering your IoT device
-   Build and deploy Azure IoT SDK on device

<a name="Step-1-Prerequisites"></a>
# Step 1: Prerequisites

You should have the following items ready before beginning the process:
-   Computer with Git client installed and access to the
    [azure-iot-sdks](https://github.com/Azure/azure-iot-sdks) GitHub public repository.
-   SSH client on your desktop computer, such as [PuTTY](http://www.putty.org/), so you can remotely access the command line on the Freescale FRDM-K64F.
-   Required hardware: [mbed-enabled Freescale K64F](http://developer.mbed.org/platforms/IBMEthernetKit).
-   [Setup your IoT hub][lnk-setup-iot-hub]
-   [Provision your device and get its credentials][lnk-manage-iot-hub]

<a name="Step-2-PrepareDevice"></a>
# Step 2: Prepare your Device

-   Connect the board to your network using an Ethernet cable. This step is required, as the sample depends on internet access.

-   Plug the device into your computer using a micro-USB cable. **Be sure to attach the cable to the USB port next to the reset button on the FRDM-K64F device**.

-   Follow the [instructions on the mbed handbook](https://developer.mbed.org/handbook/SerialPC) to set up the serial connection with your device from your development machine. If you are on Windows, install the Windows serial port drivers located [here](http://developer.mbed.org/handbook/Windows-serial-configuration#1-download-the-mbed-windows-serial-port).

<a name="Step-3-Build"></a>
# Step 3: Build and Run the sample

## Create mbed project and import the sample code

-   In your web browser, go to the mbed.org [developer site](https://developer.mbed.org/). If you haven't signed up, you will see an option to create a new account (it's free). Otherwise, log in with your account credentials. Then click on **Compiler** in the upper right-hand corner of the page. This should bring you to the Workspace Management interface.

-   Make sure the hardware platform you're using appears in the upper right-hand corner of the window, or click the icon in the right-hand corner to select your hardware platform.

-   Click **Import** on the main menu. Then click the **Click here** to import from URL link next to the mbed globe logo.

	![][1]

-   In the popup window, enter the link for the sample code https://developer.mbed.org/users/AzureIoTClient/code/iothub_client_sample_amqp/ (note that if you want to try the sample using HTTP instead of AMQP, you can import this other sample: https://developer.mbed.org/users/AzureIoTClient/code/iothub_client_sample_http/

	![][2]

-   You can see in the mbed compiler that importing this project imported various libraries. Some are provided and maintained by the Azure IoT team ([azureiot_common](https://developer.mbed.org/users/AzureIoTClient/code/azureiot_common/), [iothub_client](https://developer.mbed.org/users/AzureIoTClient/code/iothub_client/), [iothub_amqp_transport](https://developer.mbed.org/users/AzureIoTClient/code/iothub_amqp_transport/), [iothub_http_transport](https://developer.mbed.org/users/AzureIoTClient/code/iothub_http_transport/), [azure-uamqp-c](https://developer.mbed.org/users/AzureIoTClient/code/azure_uamqp_c/)), while others are third party libraries available in the mbed libraries catalog.

	![][3]

-   Open iothub_client_sample_amqp/iothub_client_sample_amqp.c, and replace "[device connection string]" with the device connection string you noted [earlier](#Step-1-Prerequisites):

	![][4]

-   Save the changes.

## Build and run the program

-   Click **Compile** to build the program. You can safely ignore any warnings, but if the build generates errors, fix them before proceeding.

	![][5]

-   If the build is successful, a .bin file with the name of your project is generated. Copy the .bin file to the device. Saving the .bin file to the device causes the current terminal session to the device to reset. When it reconnects, reset the terminal again manually, or start a new terminal. This enables the mbed device to reset and start executing the program.

-   Connect to the device using an SSH terminal program, such as PuTTY. You can determine which serial port your device uses by checking the Windows Device Manager:

	![][6]

-   In PuTTY, click the **Serial** connection type. The device most likely connects at 115200, so enter that value in the **Speed** box. Then click **Open**: **NOTE** Try 9600 if unable to communicate with the board.

	![][7]

The program starts executing. You may have to reset the board (press CTRL+Break or press on the board's reset button) if the program does not start automatically when you connect.

## Monitor device data and send messages

-   See [Manage IoT Hub][lnk-manage-iot-hub] to learn how to observe the messages IoT Hub receives from the application and how to send cloud-to-device messages to the application.


[1]: ./media/mbed1.png
[2]: ./media/mbed2.png
[3]: ./media/mbed3.png
[4]: ./media/mbed4.png
[5]: ./media/mbed5.png
[6]: ./media/mbed6.png
[7]: ./media/mbed7.png

[lnk-setup-iot-hub]: ../setup_iothub.md
[lnk-manage-iot-hub]: ../manage_iot_hub.md
