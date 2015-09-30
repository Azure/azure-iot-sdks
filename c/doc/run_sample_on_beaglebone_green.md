# BeagleBone Green Debian Setup

The following document describes the process of connecting a [BeagleBone Green](http://beagleboard.org/Green) device running Linux to Azure IoT Hub.

## Table of Contents

- [Requirements](#requirements)
- [Before you begin](#beforebegin)
- [Preparing the BeagleBone Green board](#preparing)
- [Build the sample application on the device](#buildrunapp)
- [Run the client simple sample](#buildandrun)
- [Tips](#tips)

<a name="requirements"/>
## Requirements

  - Computer with a Git client installed so that you can access the azure-iot-sdks code on GitHub.
  - SSH client on your desktop computer, such as [PuTTY](http://www.putty.org/), so you can remotely access the command line on the BeagleBone Green.
  - Required hardware:
	  - [BeagleBone Green](http://beagleboard.org/Green)
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
## Preparing the BeagleBone Green board

- To set up your BeagleBone Green device and connect it to your machine, follow the instructions on the [beagleboard.org](http://beagleboard.org/getting-started) site
- Connect your BeagleBone Green to your network using an ethernet cable or by using a WiFi dongle on the device.
- You need to discover the IP address of your BeagleBone Green before your can connect using PuTTY. To do this you can find instructions on this [wiki](http://elinux.org/Beagleboard:Terminal_Shells).
- Once you see that your board is alive, open an SSH terminal program such as [PuTTY](http://www.putty.org/) on your desktop machine.
- Use the IP address from step 4 as the Host name, Port=22, and Connection type=SSH to complete the connection.
- When prompted, log in with username **root** (no password by default).

<a name="buildrunapp"/>
## Build the sample application on the device

- Install the prerequisite packages for the Microsoft Azure IoT Device SDK for C by issuing the following commands from the command line on the board in your PuTTY terminal window:

  ```
  sudo apt-get update
  sudo apt-get install -y curl libcurl4-openssl-dev uuid-dev uuid g++ make cmake git
  ```

  **Note:** You can paste into a PuTTY terminal window in Windows by using mouse right-click.

- Download the Microsoft Azure IoT Device SDK for C by issuing the following command on the board:

  ```
  git clone https://github.com/Azure/azure-iot-sdks.git
  ```

  You will be prompted for your GitHub username and password -- if you have two-factor authentication enabled for your account, you'll need to generate/use a personal access token in place of your password.

- Verify that you now have a copy of the SDK under the directory ~/azure-iot-sdks.


- Edit the file ~/azure-iot-sdks/c/serializer/samples/simplesample_amqp/simplesample_amqp.c and replace the placeholder in the following lines of code with your IoT Hub device connection string (see requirements section for how to set up your IoT Hub instance and provision your device info). You can use the console-based text editor **nano** to edit the file:

  ```
  static const char* connectionString = "[device connection string]";
  ```

- On the board, run the following command to build and install Apache Proton library:

  ```
  ~/azure-iot-sdks/c/build_all/linux/build_proton.sh --install
  ```

- Assuming everything went OK on the build_proton.sh, you can now build the SDK samples using the following command:

  ```
  ~/azure-iot-sdks/c/build_all/linux/build.sh
  ```

<a name="#buildandrun"/>
## Run the sample

- Now run the **simplesample_amqp** sample by issuing the following commands:

  ```
  ~/azure-iot-sdks/c/serializer/samples/simplesample_amqp/linux/simplesample_amqp
  ```

<a name="tips"/>
## Tips

- If you just want to build the serializer samples, run the following commands:

  ```
  cd ~/azure-iot-sdks/c/serializer/build/linux
  make -f makefile.linux all
  ```

- You can use the DeviceExplorer tool on your desktop machine to see the data your device is sending and receiving.

[setup-iothub]: ../../doc/setup_iothub.md
[provision-device]: ./provision_device.md
