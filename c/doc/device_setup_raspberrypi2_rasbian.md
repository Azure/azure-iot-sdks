# Raspberry Pi 2 Raspbian Setup

The following document describes the process of connecting to a [Raspberry Pi 2](https://www.raspberrypi.org/) device and building the IoT SDK sample.

## Table of Contents
- [Raspbian OS Setup](#rasbinaos)
	- [Requirements](#requirements)
	- [Preparing the Raspberry Pi 2 board](#preparing)
	- [Provision your device in IoT Hubs](#provision)
	- [Build and run the Device Explorer tool](#buildrundevexp)
	- [Build the sample application on the device](#buildrunapp)
	- [Run the asset monitoring client sample](#buildassetmon)
	- [Tips](#tips)

<a name="rasbinaos"/>
# Raspbian OS Setup

The following procedure describes the process of connecting to a [Raspberry Pi 2](https://www.raspberrypi.org/) device running the Raspbian OS and building the SDK sample.

<a name="requirements"/>
## Requirements

- Computer with a Git client installed so that you can access the azure-iot-suite-sdks code on GitHub.

- SSH client on your desktop computer, such as [PuTTY](http://www.putty.org/), so you can remotely access the command line on the Raspberry Pi.

- Required hardware:
	- [Raspberry Pi 2](http://www.amazon.com/Raspberry-Pi-Model-Project-Board/dp/B00T2U7R7I/ref=sr_1_1?ie=UTF8&qid=1429516842&sr=8-1&keywords=raspberry+pi)
	- 8GB MicroSD Card
	- A USB keyboard
	- A USB mouse
	- A USB Mini cable
	- A 5 Volt - 1 Amp USB power supply
	- An HDMI cable
	- TV/ Monitor that supports HDMI
	- An Ethernet cable or Wi-Fi dongle

Note: You may wish to consider a Starter Kit such as [CanaKit](http://www.amazon.com/CanaKit-Raspberry-Complete-Original-Preloaded/dp/B008XVAVAW/ref=sr_1_4?ie=UTF8&qid=1429516842&sr=8-4&keywords=raspberry+pi) that includes some of these hardware requirements.

<a name="preparing"/>
## Preparing the Raspberry Pi 2 board

- Install the latest Raspbian operating system on your Raspberry Pi 2 by
following the instructions in the [NOOBS setup
guide](http://www.raspberrypi.org/help/noobs-setup/).

- When the install process is complete, the Raspberry Pi configuration menu
(raspi-config) loads. Here you are able to set the time and date for your region
and enable a Raspberry Pi camera board, or even create users. Under **Advanced
Options** make sure to enable **ssh** so you can access the device remotely with
PuTTY or WinSCP. For more information, see
https://www.raspberrypi.org/documentation/remote-access/ssh/.

- Connect your Raspberry Pi to your network using an ethernet cable or by using
a WiFi dongle on the device.

- You need to discover the IP address of your Raspberry Pi before your can
connect using PuTTY. For more information see
https://www.raspberrypi.org/documentation/troubleshooting/hardware/networking/ip-address.md.

- Once you see that your board is alive, open an SSH terminal program such as [PuTTY](http://www.putty.org/) on your desktop machine.

- Use the IP address from step 4 as the Host name, Port=22, and Connection type=SSH to complete the connection.

- When prompted, log in with username **pi**, and password **raspberry**.

- Create a **root** account using the following command `sudo passwd root` and choosing a new password:

  ![][1]

<a name="provision"/>
## Provision your device in an IoT Hub

You need an IoT hub to which will communicate with your device. You can move on to the next step if you already have an IoT hub set up, otherwise you'll need to create one. Step by step instructions to do this can be fond in the **Create an IoT Hub** section of the [Getting started with IoT Hub](https://azure.microsoft.com/en-us/documentation/articles/iot-hub-csharp-csharp-getstarted/) article. This article covers much more than just setting up an IoT hub, but you only need to follow the six steps in the **Create an IoT Hub** section for this sample.  

You have two options for provisioning your Raspberry Pi device in IoT Hubs. Both options enable you to provision a new device and obtain the **Device ID** and **connection string** you need to connect your device to your IoT Hub.
- (Recommended) Use the IoT Device Administration Portal you deployed when you followed the [Asset Monitoring Sample Solution Walkthrough](https://github.com/Azure/azure-iot-solution/blob/master/Docs/iot-asset-monitoring-sample-walkthrough.md).

- Alternatively, use the Device Explorer tool in the tools/DeviceExplorer folder in this repository.

<a name="buildrundevexp"/>
### Build and run the Device Explorer tool

We recommend you use the IoT Device Administration Portal (see [Asset Monitoring Sample Solution Walkthrough](https://github.com/Azure/azure-iot-solution/blob/master/Docs/iot-asset-monitoring-sample-walkthrough.md)) to provision and manage your device, but you can also use the Device Explorer sample application on your Windows desktop machine to create and register a device ID and symmetric key for your device. The Device Explorer interfaces with Azure IoT Hubs, and has some basic capabilities such as:

- **Device management**: creates device IDs and obtains a list of registered devices on your IoT Hub.

- **Monitors and consumes data** sent by your devices to your IoT Hub.

- **Sends messages** to your devices.

To run this tool, you need connection and configuration information for your IoT Hub and the Event Hub it is associated with.

- The Device Explorer sample solution can be found at **\azure-iot-suite-sdks\tools\DeviceExplorer\DeviceExplorer.sln**.

- Open the solution in Visual Studio, then build and run it.

- Paste the **connection string - primary key** for your IoT hub into the **IoT Hub Connection String** field in Device Explorer and then click the **Update** button (you wrote down the **connection string - primary key** in step 6 if you created your IoT hub using the directions in the **Create an IoT Hub** section of the [Getting started with IoT Hub](https://azure.microsoft.com/en-us/documentation/articles/iot-hub-csharp-csharp-getstarted/) article).

- Create a device ID for your device and register it  with your IoT hub:
	- Click the **Management** tab.
	- Click **Create** to create and register a device ID and key with your IoT hub.
	- Save the device's ID, primary key, and **connection string**  in Notepad (or just keep the sample running). You will need this later.
		- Click the **Refresh** button in the **Managment** tab to make sure that Device Manager is showing the information for your newly registered device.
		- Right click on the device and select **Copy data for selected device** OR copy the information by hand.

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
  git clone https://github.com/Azure/azure-iot-suite-sdks.git
  ```

  You will be prompted for your GitHub username and password -- if you have two-factor authentication enabled for your account, you'll need to generate/use a personal access token in place of your password.

- Verify that you now have a copy of the SDK under the directory ~/azure-iot-suite-sdks.


- Edit the file ~/azure-iot-suite-sdks/c/serializer/samples/simplesample_amqp/simplesample_amqp.c and replace the placeholders in the following lines of code with your IoT Hub name, device ID, and device key values (you can find these values in the IoT Device Administration portal). You can use the console-based text editor **nano** to edit the file:

  ```
  static const char* iotHubName = "[IoT Hub name goes here]";
  static const char* iotHubSuffix = "[IoT Hub suffix goes here, e.g., private.azure-devices-int.net]";
  static const char* deviceId = "[Device ID goes here]";
  static const char* deviceKey = "[Device key goes here]";
  ```

- On the board, run the following command to build and install Apache Proton library:

  ```
    sudo ~/azure-iot-suite-sdks/c/build_all/linux/build_proton.sh --install /usr
  ```

- This SDK sample depends on the presences of a few libraries. Run the following command to build them:

  ```
  sudo ~/azure-iot-suite-sdks/c/build_all/linux/build_paho.sh
  ```

- You can now build the SDK code using the following command, assuming everything went OK on build\_proton.sh and build_paho.sh

  ```  
  ~/azure-iot-suite-sdks/c/build_all/linux/build.sh
  ```

  **Note:** If you receive the following error, ignore it: **"crtabstractions_unittests/crtabstractions_unittests.cpp:119:5"**.

<a name="buildassetmon"/>
## Run the asset monitoring client sample

- Now run the **simplesample_amqp** sample by issuing the following commands:

  ```
  ~/azure-iot-suite-sdks/c/serializer/samples/simplesample_amqp/linux/simplesample_amqp
  ```

- This sample application sends simulated sensor data to your IoT Hub. See the [Asset Monitoring Sample Solution Walkthrough](https://github.com/Azure/azure-iot-solution/blob/master/Docs/iot-asset-monitoring-sample-walkthrough.md) for information about how you can verify that the sensor data is reaching your IoT Hub.

<a name="tips"/>
## Tips

- If you just want to build the serializer samples, run the following commands:

  ```
  cd ~/azure-iot-suite-sdks/c/serializer/build/linux
  make -f makefile.linux all
  ```

- You can use the DeviceExplorer tool on your desktop machine to see the data your device is sending and receiving.

[1]: ./media/service-bus-iot-raspberrypi-raspbian-setup/raspbian01.png
