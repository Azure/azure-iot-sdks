# BeagleBone Black Debian Setup

This document describes the process of setting up a [BeagleBone Black](http://beagleboard.org/black) device to connect to an Azure IoT hub.

## Table of Contents

- [Requirements](#requirements)
- [Before you begin](#beforebegin)
- [Preparing the BeagleBone Black board](#preparing)
- [Build the sample application on the device](#buildrunapp)
- [Run the client simple sample](#buildandrun)
- [Tips](#tips)

<a name="requirements"/>
## Requirements

  - Computer with a Git client installed so that you can access the azure-iot-sdks code on GitHub.
  - SSH client on your desktop computer, such as [PuTTY](http://www.putty.org/), so you can remotely access the command line on the BeagleBone Black.
  - Required hardware:
	  - [BeagleBone Black](http://beagleboard.org/black)
	  - USB Mini cable
	  - Ethernet cable or Wi-Fi dongle

<a name="beforebegin"/>
## Before you begin
Before you begin you will need to create and configure an IoT hub to connect to.
  1. [Set up your IoT Hub][setup-iothub].
  1. With your IoT hub configured and running in Azure, follow the instructions in [Connecting your device to an IoT hub][provision-device].
  1. Make note of the Connection String for your device from the previous step.

  > Note: You can skip this step if you just want to build the sample application without running it.

<a name="preparing"/>
## Preparing the BeagleBone Black

- Follow the instructions on the [beagleboard.org](http://beagleboard.org/getting-started) site to set up your BeagleBone Black device and connect it to your computer.
- Connect your BeagleBone Black to your network using an ethernet cable or by using a WiFi dongle on the device.
- Follow the instructions on this [wiki](http://elinux.org/Beagleboard:Terminal_Shells) to obtain your device's IP address.
- Open an SSH terminal program such as [PuTTY](http://www.putty.org/) on your desktop machine and connect to your
device using the IP address from the previous step.  Use Port=22 and Connection Type=SSH.
- When prompted, log in with username **root** (no password by default).

<a name="buildrunapp"/>
## Build the sample application on the device

Run the following commands in the terminal window connected to your BeagleBone Black.

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
- Download the Azure IoT device SDK to your BeagleBone Black:

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
  ./serializer/samples/simplesample_amqp/linux/simplesample_amqp
  ```

This sample application sends simulated sensor data to your IoT Hub.


<a name="tips"/>
## Tips

- If you just want to build the serializer samples, run the following commands:

  ```
  cd ./c/serializer/build/linux
  make -f makefile.linux all
  ```

- On Windows, you can use the Device Explorer tool to see the data your device is sending and receiving.

[setup-iothub]: ../../doc/setup_iothub.md
[provision-device]: ./provision_device.md
