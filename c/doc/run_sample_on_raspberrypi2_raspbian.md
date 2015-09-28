# Raspberry Pi 2 Raspbian Setup

This document describes the process of setting up a [Raspberry Pi 2](https://www.raspberrypi.org/) device to connect to an Azure IoT hub.

## Table of Contents
- [Requirements](#requirements)
- [Before you begin](#beforebegin)
- [Prepare the Raspberry Pi 2](#preparing)
	- [Build the sample application on the device](#buildrunapp)
- [Run the AMQP simple sample](#buildsimplesample)
- [Tips](#tips)


<a name="requirements"/>
## Requirements

- SSH client on your desktop computer, such as [PuTTY](http://www.putty.org/), so you can remotely access the command line on the Raspberry Pi.
- Required hardware:
	- [Raspberry Pi 2](http://www.amazon.com/Raspberry-Pi-Model-Project-Board/dp/B00T2U7R7I/ref=sr_1_1?ie=UTF8&qid=1429516842&sr=8-1&keywords=raspberry+pi)
	- 8GB or larger MicroSD Card
	- USB keyboard
	- USB mouse (optional; you can navigate NOOBS with a keyboard)
	- USB Mini cable
	- HDMI cable
	- TV/ Monitor that supports HDMI
	- Ethernet cable or Wi-Fi dongle


You may wish to consider a Starter Kit such as [CanaKit](http://www.amazon.com/CanaKit-Raspberry-Complete-Original-Preloaded/dp/B008XVAVAW/ref=sr_1_4?ie=UTF8&qid=1429516842&sr=8-4&keywords=raspberry+pi) that
	 includes some of these hardware requirements.

<a name="beforebegin"/>
## Before you begin
Before you begin you will need to create and configure an IoT hub to connect to.
  1. [Set up your IoT hub][setup-iothub].
  1. With your IoT hub configured and running in Azure, follow the instructions in [Connecting your device to an IoT hub][provision-device].
  1. Make note of the Connection String for your device from the previous step.

  > Note: You can skip this step if you just want to build the sample application without running it.

<a name="preparing"/>
## Prepare the Raspberry Pi 2

1. Install the latest Raspbian operating system on your Raspberry Pi 2 by
following the instructions in the [NOOBS setup guide](http://www.raspberrypi.org/help/noobs-setup/).
1. When the installation process is complete, the Raspberry Pi configuration menu
(raspi-config) loads. Here you are able to set the time and date for your region
and enable a Raspberry Pi camera board, or even create users. Under **Advanced
Options**, enable **ssh** so you can access the device remotely with
PuTTY or WinSCP. For more information, see
https://www.raspberrypi.org/documentation/remote-access/ssh/.

1. Connect your Raspberry Pi to your network using an ethernet cable or by using
a WiFi dongle on the device.

1. You need to determine the IP address of your Raspberry Pi in order to connect over the network. For more information, see
https://www.raspberrypi.org/documentation/troubleshooting/hardware/networking/ip-address.md.

1. Once you see that your board is working, open an SSH terminal program such as [PuTTY](http://www.putty.org/) on your desktop machine.

1. Use the IP address from step 4 as the Host name, Port=22, and Connection type=SSH to complete the connection.

1. When prompted, log in with username **pi**, and password **raspberry**.

1. Create a **root** account using the following command `sudo passwd root` and choosing a new password:

  ![][1]

The root account is necessary in order to install some libraries required by the device SDK.


<a name="buildrunapp"/>
## Build the sample application on the device

Run the following commands in the terminal window connected to your Raspberry Pi.

- Install the prerequisite packages for the device SDK:

  ```
  sudo apt-get update
  sudo apt-get install -y curl libcurl4-openssl-dev uuid-dev uuid g++ make cmake git
  ```
  If you get errors running sudo, make sure your root password is set as decribed above.

	> Note: Right-click in a PuTTY terminal window to paste text.

- Navigate to the directory where you want to  install the SDK.  If you're not sure, navigate to your home directory:
  ```
  cd ~
  ```
- Download the Azure IoT device SDK to your Raspberry Pi:

  ```
  git clone https://github.com/Azure/azure-iot-sdks.git
  ```

- Confirm that you now have a copy of the SDK under the directory ./azure-iot-sdks.
Then cd to the directory:
  ```
  cd azure-iot-sdks
  ```

- Edit the file ./c/serializer/samples/simplesample_amqp/simplesample_amqp.c and replace connection string placeholder with the connection string
you obtained in the "Connecting your device to an IoT hub" step above.
(You can use the console-based text editor **nano** to edit the file):

  ```
  static const char* connectionString = "[device connection string]";
  ```
  > Note: You can skip this step if you only want to build the samples without running them.

- Build and install the Apache Proton library:

  ```
    sudo ./c/build_all/linux/build_proton.sh --install /usr
  ```

- Finally, build the sample applications:

  ```
  ./c/build_all/linux/build.sh
  ```

<a name="buildsimplesample"/>
## Run the AMQP simple sample

- Run the **simplesample_amqp** sample:

  ```
  ./c/serializer/samples/simplesample_amqp/linux/simplesample_amqp
  ```

This sample application sends simulated sensor data to your IoT Hub.


<a name="tips"/>
## Tips

- If you just want to build the serializer samples, run the following commands:

  ```
  cd ./c/serializer/build/linux
  make -f makefile.linux all
  ```
- On Windows, you can use the [Device Explorer][device-explorer] to see the data your device is sending and receiving.

[1]: ./media/service-bus-iot-raspberrypi-raspbian-setup/raspbian01.png

[provision-device]: ./provision_device.md
[setup-iothub]: ../../doc/setup_iothub.md
[device-explorer]: ../../tools/DeviceExplorer/doc/how_to_use_device_explorer.md
