# BeagleBone Black Debian Setup

The following document describes the process of connecting a [BeagleBone Black](http://beagleboard.org/black) device running Linux to Azure IoT Hub.

## Table of Contents
	- [Requirements](#requirements)
	- [Preparing the BeagleBone Black board](#preparing)
	- [Build the sample application on the device](#buildrunapp)
	- [Run the asset monitoring client sample](#buildassetmon)
	- [Tips](#tips)

<a name="requirements"/>
## Requirements

  - Computer with a Git client installed so that you can access the azure-iot-sdks code on GitHub.
  - SSH client on your desktop computer, such as [PuTTY](http://www.putty.org/), so you can remotely access the command line on the Raspberry Pi.
  - Required hardware:
	  - [BeagleBone Black](http://beagleboard.org/black)
	  - A USB Mini cable
	  - An Ethernet cable or Wi-Fi dongle
  - [Setup your IoT Hub][setup-iothub]
  - Configure your device in IoT Hub. <br/>See the section "Configure IoT Hub connection" in the document [How to use Device Explorer][device-explorer].

**Note:** The Device Explorer utility only runs on Windows. You can run it on a different machine from your Linux desktop development environment.

<a name="preparing"/>
## Preparing the BeagleBone Black board

- To setup your BeagleBone Black device and connect it to your machine, follow the instructions on the [beagleboard.org](http://beagleboard.org/getting-started) site
- Connect your BeagleBone Black to your network using an ethernet cable or by using a WiFi dongle on the device.
- You need to discover the IP address of your BeagleBone Black before your can connect using PuTTY. To do this you can find instructions on this [wiki](http://elinux.org/Beagleboard:Terminal_Shells). 
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


- Edit the file ~/azure-iot-sdks/c/serializer/samples/simplesample_amqp/simplesample_amqp.c and replace the placeholders in the following lines of code with your IoT Hub name, device ID, and device key values (see requirements section for how to setup your IoT Hub instance and provision your device info). You can use the console-based text editor **nano** to edit the file:

  ```
  static const char* iotHubName = "[IoT Hub name goes here]";
  static const char* iotHubSuffix = "[IoT Hub suffix goes here, e.g., private.azure-devices-int.net]";
  static const char* deviceId = "[Device ID goes here]";
  static const char* deviceKey = "[Device key goes here]";
  ```

- On the board, run the following command to build and install Apache Proton library:

  ```
  ~/azure-iot-sdks/c/build_all/linux/build_proton.sh --install
  ```

- Assuming everything went OK on the build_proton.sh, you can now build the SDK samples using the following command:

  ```
  ~/azure-iot-sdks/c/build_all/linux/build.sh
  ```

  **Note:** If you receive the following error, ignore it: **"crtabstractions_unittests/crtabstractions_unittests.cpp:119:5"**.

<a name="buildassetmon"/>
## Run the asset monitoring client sample

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

[device-explorer]: ../../tools/DeviceExplorer/doc/how_to_use_device_explorer.md
[setup-iothub]: ../../doc/setup_iothub.md