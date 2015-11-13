# Ankaa Ubuntu Setup

This document describes the process of setting up a [Ankaa](http://www.e-consystems.com/iMX6-development-board.asp) device to connect to an Azure IoT hub.

## Table of Contents
- [Requirements](#requirements)
- [Before you begin](#beforebegin)
- [Prepare the Ankaa](#preparing)
	- [Build the sample application on the device](#buildrunapp)
- [Run the AMQP simple sample](#buildsimplesample)
- [Tips](#tips)

<a name="requirements"/>
## Requirements

- Terminal emulator on your desktop computer, such as [Tera Term](https://en.osdn.jp/projects/ttssh2/releases/), so you can access the command line on the Ankaa.
- Required hardware:
	- Ankaa board
	- 4 GB or larger MicroSD Card
	- USB Mini cable
	- Ethernet cable or Wi-Fi dongle

<a name="beforebegin"/>
## Before you begin
Before you begin you will need to create and configure an IoT hub to connect to.
  1. [Set up your IoT hub](https://github.com/Azure/azure-iot-sdks/blob/master/doc/setup_iothub.md).
  1. With your IoT hub configured and running in Azure, follow the instructions in [Connecting your device to an IoT hub](https://github.com/Azure/azure-iot-sdks/blob/master/c/doc/provision_device.md)
  1. Make note of the Connection String for your device from the previous step.

  > Note: You can skip this step if you just want to build the sample application without running it.

<a name="preparing"/>
## Prepare the Ankaa

1. Install the Ubuntu operating system on your Ankaa by following the instructions in the "esomimx6_ubuntu_user_manual.pdf".
2. Connect the Ankaa board to your desktop machine using the USB mini cable and open a terminal emulator program such as [Tera Term](https://en.osdn.jp/projects/ttssh2/releases/) on your desktop machine and switch ON the device. You have to create a new Serial port connection from Setup->Serial Port and set the parameters:  
    * Port number where you have connected the device to PC.
    * Baud Rate = 115200
    * Data = 8 bits
    * Parity = noneStop = 1 bit
    * Flow Control = none
3. When prompted, log in with username **root**, and password **root**. The root account is necessary in order to install some libraries required by the device SDK.
4. If you want to connect your Ankaa to your Wired network connect it using an ethernet cable to the device ethernet port. For Wireless connection, Ankaa have it's own WiFi module so there is no need to connect any WiFi dongle.
5. You need to determine the IP address of your Ankaa in order to connect over the network. To connect the network, follow the steps under **Ethernet** or **WiFi** for the corresponding connection type from the document "esomimx6_ubuntu_peripheral_test_user_manual.pdf"

<a name="buildrunapp"/>
## Build the sample application on the device

Run the following commands in the terminal window connected to your Ankaa.

- Install the prerequisite packages for the device SDK:

  ```
  sudo apt-get update
  sudo apt-get install -y curl libcurl4-openssl-dev uuid-dev uuid g++ make cmake git
  ```
  If you get errors running sudo, make sure your root password is set as decribed above.

	> Note: Right-click in a Tera Term terminal window to paste the copied text.

- Navigate to the directory where you want to  install the SDK.  If you're not sure, navigate to your home directory:
  ```
  cd ~
  ```
- Download the Azure IoT device SDK to your Ankaa:

  ```
  git clone https://github.com/Azure/azure-iot-sdks.git
  ```

- Confirm that you now have a copy of the SDK under the directory ./azure-iot-sdks.
Then cd to the directory:
  ```
  cd azure-iot-sdks
  ```

**For AMQP protocol:**

* Edit the file ./c/serializer/samples/simplesample_amqp/simplesample_amqp.c and replace connection string placeholder with the connection string
you obtained in the "Connecting your device to an IoT hub" step above. 
(You can use the console-based text editor **vi** to edit the file):

  ```
  static const char* connectionString = "[device connection string]";
  ```
**For HTTP protocol:**

* Edit the file ./c/serializer/samples/simplesample_http/simplesample_http.c and replace connection string placeholder with the connection string as mentioned above.

  ```
  static const char* connectionString = "[device connection string]";
  ```
  
  > Note: You can skip these steps if you only want to build the samples without running them.

- Build and install the Apache Proton library:

  ```
    sudo ./c/build_all/linux/build_proton.sh --install /usr
  ```

- Finally, build the sample applications:

  ```
  ./c/build_all/linux/build.sh
  ```

<a name="buildsimplesample"/>
## Run a simple sample app

**For AMQP protocol:**
- Run the **simplesample_amqp** sample:

  ```
  ./c/serializer/samples/simplesample_amqp/linux/simplesample_amqp
  ```
**For HTTP protocol:**
- Run the **simplesample_amqp** sample:

  ```
  ./c/serializer/samples/simplesample_http/linux/simplesample_http
  ```
Thse sample applications send simulated sensor data to your IoT Hub.

<a name="tips"/>
## Tips

- If you just want to build the serializer samples, run the following commands:

  ```
  cd ./c/serializer/build/linux
  make -f makefile.linux all
  ```
- On Windows, you can use the [Device Explorer](https://github.com/Azure/azure-iot-sdks/blob/master/tools/DeviceExplorer/doc/how_to_use_device_explorer.md) to see the data your device is sending and receiving. See "Send cloud-to-device messages" in the document for instructions on sending messages with the DeviceExplorer utility.
