---
platform: mbed
device: renesas gr-peach
language: c
---

Run a simple C sample on Renesas GR-PEACH device running Mbed
===

# Table of Contents

-   [Introduction](#Introduction)
-   [Step 1: Prerequisites](#Step-1-Prerequisites)
-   [Step 2: Prepare your Device](#Step-2-PrepareDevice)
-   [Step 3: Build and Run the Sample](#Step-3-Build)
-   [Tips](#tips)

<a name="Introduction"></a>
# Introduction

**About this document**

This document describes the steps for connecting mbed-enabled [Renesas GR-PEACH](https://developer.mbed.org/platforms/Renesas-GR-PEACH/) to Azure IoT Hub. This multi-step process includes:
-   Configuring Azure IoT Hub
-   Registering your IoT device
-   Build and deploy Azure IoT SDK on device

<a name="Step-1-Prerequisites"></a>
# Step 1: Prerequisites

You should have the following items ready before beginning the process:

-   Computer with Git client installed and access to the [azure-iot-sdks](https://github.com/Azure/azure-iot-sdks) GitHub public repository.
-   SSH client on your desktop computer, such as [PuTTY](http://www.putty.org), so you can remotely access the command line on the Renesas GR-PEACH.
-   Required hardware: [mbed-enabled Renesas GR-PEACH](<https://developer.mbed.org/platforms/Renesas-GR-PEACH/>)
-   [Setup your IoT Hub][lnk-setup-iot-hub]
-   [Provision your device and get its credentials][lnk-manage-iot-hub]

<a name="Step-2-PrepareDevice"></a>
# Step 2: Prepare your Device

-   Connect the board to your network using an Ethernet cable. This step is required as the sample depends on internet access.
-   Plug GR-PEACH into your computer using a micro-USB cable. **Be sure to attach the cable to the USB port next to the reset button on the GR-PEACH platform**.
-   Follow the [instructions on the mbed handbook](https://developer.mbed.org/handbook/SerialPC) to set up the serial connection with your device from your development machine.

<a name="Step-3-Build"></a>
# Step 3: Build and Run the sample

## Create mbed project and import the sample code
-   In your web browser, go to the mbed.org [developer site](https://developer.mbed.org/). If you haven't signed up, you will see an option to create a new account (it's free). Otherwise, log in with your account credentials. Then click on **Compiler** in the upper right-hand corner of the page. This should bring you to the Workspace Management interface.

-   Make sure the hardware platform you're using appears in the upper right-hand corner of the window, or click the icon in the right-hand corner to select your hardware platform.

-   Click **Import** on the main menu. Then click the **Click here** to import from URL link next to the mbed globe logo.

    ![][1]

-   In the popup window, enter the link for the sample code 
   	<https://developer.mbed.org/users/AzureIoTClient/code/iothub_client_sample_mqtt/>

    Note that if you want to try the sample using HTTP or AMQP instead of MQTT, you can import the following samples respectively:
    <https://developer.mbed.org/users/AzureIoTClient/code/iothub_client_sample_http/>
    <https://developer.mbed.org/users/AzureIoTClient/code/iothub_client_sample_amqp/>

    ![][2]

-   You can see in the mbed compiler that various libraries were imported in these projects. Some are provided and maintained by the Azure IoT team ([azure_c_shared_utility](https://developer.mbed.org/users/AzureIoTClient/code/azure_c_shared_utility/), [iothub_client](https://developer.mbed.org/users/AzureIoTClient/code/iothub_client/), [iothub_mqtt_transport](https://developer.mbed.org/users/AzureIoTClient/code/iothub_mqtt_transport/) [iothub_amqp_transport](https://developer.mbed.org/users/AzureIoTClient/code/iothub_amqp_transport/), [iothub_http_transport](https://developer.mbed.org/users/AzureIoTClient/code/iothub_http_transport/), [azure-umqtt-c](https://developer.mbed.org/users/AzureIoTClient/code/azure_umqtt_c/), [azure-uamqp-c](https://developer.mbed.org/users/AzureIoTClient/code/azure_uamqp_c/)), while others are third party libraries available in the mbed libraries catalog.

    ![][3]

-   Open iothub_client_sample_mqtt/iothub_client_sample_mqtt.c, and replace **[device connection string]** with the device connection string you noted [earlier](#Step-1-Prerequisites):

    ![][4]

-   Save the changes.

## Build and run the program

Click **Compile** to build the program. You can safely ignore any warning, but if the build generates errors, fix them before proceeding.

![][5]

-   If the build is successful, a .bin file with the name of your project is generated. Copy the .bin file to the device. Saving the .bin file to the device causes the current terminal session to the device to reset. When it reconnects, reset the terminal again manually, or start a new terminal. This enables the mbed device to reset and start executing the program.

-   Connect to device using an SSH terminal program, such as PuTTY. You can determine which serial port your device uses by checking the Windows Device Manager.

    ![][6]

-   In PuTTY, click the **Serial** connection type. The default baud rate is 9600 and so, enter that value in the **Speed** box. Then, click **Open**:

    ![][7]

The program starts executing. You may have to reset the board (press CTRL+Break or press on the board's reset button) if the program does not start automatically when you connect.

<a name="tips"></a>
## Monitor device data and send messages

-   See [Manage Iot Hub][lnk-manage-iot-hub] to learn how to observe the messages Azure IoT Hub receives from the application and how to send cloud-to-device messages to the application.

[1]: media/gr-peach_mbed1.png
[2]: media/gr-peach_mbed2.png
[3]: media/gr-peach_mbed3.png
[4]: media/gr-peach_mbed4.png
[5]: media/gr-peach_mbed5.png
[6]: media/gr-peach_mbed6.png
[7]: media/gr-peach_mbed7.png

[lnk-setup-iot-hub]: ../setup_iothub.md
[lnk-manage-iot-hub]: ../manage_iot_hub.md
