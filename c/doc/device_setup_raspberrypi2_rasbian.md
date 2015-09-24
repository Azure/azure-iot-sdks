# Raspberry Pi 2 Raspbian Setup

This document describes the process of setting up a [Raspberry Pi 2](https://www.raspberrypi.org/) device to connect to an Azure IoT hub.

## Table of Contents
- [Requirements](#requirements)
- [Prepare the Raspberry Pi 2 board](#preparing)
- [Provision the device in your IoT hub](#provision)
	- [Run the provisioning tool](#runprovision)
	- [Run the Device Explorer tool (optional; Windows-only)](#buildrundevexp)
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
	- An HDMI cable
	- TV/ Monitor that supports HDMI
	- An Ethernet cable or Wi-Fi dongle

Note: You may wish to consider a Starter Kit such as [CanaKit](http://www.amazon.com/CanaKit-Raspberry-Complete-Original-Preloaded/dp/B008XVAVAW/ref=sr_1_4?ie=UTF8&qid=1429516842&sr=8-4&keywords=raspberry+pi) that includes some of these hardware requirements.

<a name="preparing"/>
## Prepare the Raspberry Pi 2 board

- Install the latest Raspbian operating system on your Raspberry Pi 2 by
following the instructions in the [NOOBS setup guide](http://www.raspberrypi.org/help/noobs-setup/).

- When the installation process is complete, the Raspberry Pi configuration menu
(raspi-config) loads. Here you are able to set the time and date for your region
and enable a Raspberry Pi camera board, or even create users. Under **Advanced
Options**, enable **ssh** so you can access the device remotely with
PuTTY or WinSCP. For more information, see
https://www.raspberrypi.org/documentation/remote-access/ssh/.

- Connect your Raspberry Pi to your network using an ethernet cable or by using
a WiFi dongle on the device.

- You need to discover the IP address of your Raspberry Pi before your can
connect using PuTTY. For more information see
https://www.raspberrypi.org/documentation/troubleshooting/hardware/networking/ip-address.md.

- Once you see that your board is working, open an SSH terminal program such as [PuTTY](http://www.putty.org/) on your desktop machine.

- Use the IP address from step 4 as the Host name, Port=22, and Connection type=SSH to complete the connection.

- When prompted, log in with username **pi**, and password **raspberry**.

- Create a **root** account using the following command `sudo passwd root` and choosing a new password:

  ![][1]

The root account is necessary in order to install some libraries required by the device SDK.

<a name="provision"/>
## Provision the device in your IoT hub

IoT Hub only allows connections from known devices that present proper credentials.  While IoT Hub supports multiple authentication scenarios, we will
 use pre-shared keys in our example. There are three steps to properly set up a device to connect to an IoT Hub with pre-shared keys.
 1. Register the device in your IoT hub
 1. Generate a secret key for that device
 1. Provision the device with the secret key generated in step 2
  
<a name="runprovision"/>
#### Run the device provisioning tool

TODO: ***insert instructions for using the node-based Device Explorer replacement tool***


*** -or- ***

<a name="buildrundevexp"/>
#### Run the Device Explorer tool (Windows-only)
If you are running on a Windows-based PC, you also have the option of using the Device Explorer tool in the tools/DeviceExplorer folder in this repository.
TODO:  ***need link to instructions***

To run this tool, you need connection and configuration information for your IoT Hub and the Event Hub it is associated with. You can get this information from
the Azure Portal by selecting your IoT Hub.  If you have not already set up an IoT hub, TODO: ***follow these instructions*** before continuing.

- Install Device Explorer from TODO: (location)
- Run Device Explorer
- Paste in the two connection strings for your IoT hub and associated event hub, and then click **Update**.

- Click the **Management** tab, then create a device ID for your device and register your device with your IoT hub:
	- Click **List** to retrieve a list of devices. If this is your first time, then you shouldn't retrieve anything!
	- Click **Create** to create a device ID and secret key.
	- Save this information. You will need this when configuring your device.


<a name="buildrunapp"/>
## Build the sample application on the device

Run the following commands in the terminal window connected to your Raspberry Pi.

- Install the prerequisite packages for the device SDK:

  ```
  sudo apt-get update
  sudo apt-get install -y curl libcurl4-openssl-dev uuid-dev uuid g++ make cmake git
  ```
  If you get errors running sudo, make sure your root password is set as decribed above.

  **Note:** Right-click in a PuTTY terminal window to paste text.

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
you obtained in the "Provision the device in your IoT hub" above.
(You can use the console-based text editor **nano** to edit the file):

  ```
  static const char* connectionString = "[device connection string]";
  ```

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
  ./serializer/samples/simplesample_amqp/linux/simplesample_amqp
  ```

This sample application sends simulated sensor data to your IoT Hub. 

(TODO: how to see that the data is getting through without Device Explorer?)

<a name="tips"/>
## Tips

- If you just want to build the serializer samples, run the following commands:

  ```
  cd ./c/serializer/build/linux
  make -f makefile.linux all
  ```

- On Windows, you can use the Device Explorer tool to see the data your device is sending and receiving.

[1]: ./media/service-bus-iot-raspberrypi-raspbian-setup/raspbian01.png
