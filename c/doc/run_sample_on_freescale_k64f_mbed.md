# Freescale FRDM-K64F Setup

The following instructions describe the steps for connecting an [mbed-enabled Freescale FRDM-K64F](https://developer.mbed.org/platforms/FRDM-K64F/) device to Azure IoT Hub.


## Requirements

- SSH client, such as [PuTTY](http://www.putty.org/), so you can access the command line.

- Required hardware: [mbed-enabled Freescale K64F](http://developer.mbed.org/platforms/IBMEthernetKit).

## Before you begin
Before you begin you will need to create and configure an IoT hub to connect to.
  1. [Set up your IoT Hub][setup-iothub].
  1. With your IoT hub configured and running in Azure, follow the instructions in [Connecting your device to an IoT hub][provision-device].
  1. Make note of the Connection String for your device from the previous step.

  > Note: You can skip this step if you just want to build the sample application without running it.

## Connect the device

- Connect the board to your network using an Ethernet cable. This step is required, as the sample depends on internet access.

- Plug the device into your computer using a micro-USB cable. Be sure to attach the cable to the correct USB port on the device, as pictured [here](https://developer.mbed.org/platforms/IBMEthernetKit/), in the "Getting started" section.

- Follow the [instructions on the mbed handbook](https://developer.mbed.org/handbook/SerialPC) to set up the serial connection with your device from your development machine. If you are on Windows, install the Windows serial port drivers located [here](http://developer.mbed.org/handbook/Windows-serial-configuration#1-download-the-mbed-windows-serial-port).

## Create mbed project and import the sample code

- In your web browser, go to the mbed.org [developer site](https://developer.mbed.org/). If you haven't signed up, you will see an option to create a new account (it's free). Otherwise, log in with your account credentials. Then click on **Compiler** in the upper right-hand corner of the page. This should bring you to the Workspace Management interface.

- Make sure the hardware platform you're using appears in the upper right-hand corner of the window, or click the icon in the right-hand corner to select your hardware platform.

- Click **Import** on the main menu. Then click the **Click here** to import from URL link next to the mbed globe logo.

	![][1]

- In the popup window, enter the link for the sample code https://developer.mbed.org/users/AzureIoTClient/code/iothub_client_sample_amqp/ (note that if you want to try the sample using HTTP instead of AMQP, you can import this other sample: https://developer.mbed.org/users/AzureIoTClient/code/iothub_client_sample_http/

	![][2]

- You can see in the mbed compiler that importing this project imported various libraries. Some are provided and maintained by the Azure IoT team ([azureiot_common](https://developer.mbed.org/users/AzureIoTClient/code/azureiot_common/), [iothub_client](https://developer.mbed.org/users/AzureIoTClient/code/iothub_client/), [iothub_amqp_transport](https://developer.mbed.org/users/AzureIoTClient/code/iothub_amqp_transport/), [iothub_http_transport](https://developer.mbed.org/users/AzureIoTClient/code/iothub_http_transport/), [proton-c-mbed](https://developer.mbed.org/users/AzureIoTClient/code/proton-c-mbed/)), while others are third party libraries available in the mbed libraries catalog.

	![][3]

- Open iothub_client_sample_amqp/iothub_client_sample_amqp.c, and replace "[device connection string]" with the device connection string you noted [earlier](#beforebegin):

	![][4]

- Save the changes.

## Build and run the program

- Click **Compile** to build the program. You can safely ignore any warnings, but if the build generates errors, fix them before proceeding.

	![][5]

- If the build is successful, a .bin file with the name of your project is generated. Copy the .bin file to the device. Saving the .bin file to the device causes the current terminal session to the device to reset. When it reconnects, reset the terminal again manually, or start a new terminal. This enables the mbed device to reset and start executing the program.

- Connect to the device using an SSH client application, such as PuTTY. You can determine which serial port your device uses by checking the Windows Device Manager:

	![][6]

- In PuTTY, click the **Serial** connection type. The device most likely connects at 115200, so enter that value in the **Speed** box. Then click **Open**:

	![][7]

The program starts executing. You may have to reset the board (press CTRL+Break or press on the board's reset button) if the program does not start automatically when you connect.

## Monitor device data and send notifications using Device Explorer

- Now  your device is sending telemetry data to Azure IoT Hub and you can monitor this data and send notifications to your device using the [device explorer tool](../../tools/DeviceExplorer/doc/how_to_use_device_explorer.md).


[setup-iothub]: ../../doc/setup_iothub.md
[provision-device]: ./provision_device.md
[1]: ./media/mbed1.png
[2]: ./media/mbed2.png
[3]: ./media/mbed3.png
[4]: ./media/mbed4.png
[5]: ./media/mbed5.png
[6]: ./media/mbed6.png
[7]: ./media/mbed7.png
