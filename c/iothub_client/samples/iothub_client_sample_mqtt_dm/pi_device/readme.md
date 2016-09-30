# Firmware update of a Raspberry Pi 3 using Azure IoT Hub device management
The following instructions explain how to do a firmware update on a Raspberry Pi 3 running Raspbian using Azure IoT Hub.
The sample is a simple C application that will run on a Raspberry Pi, establish connection with Azure IoT Hub and wait for a "firmware update" call from Azure IoT Hub.

## Prerequisites
You should have the following items ready before beginning the process:
-   Computer with Git client installed and access to the [azure-iot-sdks-preview](https://github.com/Azure/azure-iot-sdks-preview) GitHub public repository.
-   SSH client on your desktop computer, such as [PuTTY](http://www.putty.org/), so you can remotely access the command line on the Raspberry Pi.
-   If running Windows:
    -   64-bit version of Windows 10 Anniversary update build 14393 or later
    -   Follow the instructions [here](https://msdn.microsoft.com/en-us/commandline/wsl/install_guide) to install the Windows Subsystem for Linux. 
-   Required hardware:
	-   [Raspberry Pi 3](https://www.raspberrypi.org/products/raspberry-pi-3-model-b/)
	-   8GB or larger MicroSD Card
	-   USB keyboard
	-   USB mouse (optional; you can navigate NOOBS with a keyboard)
	-   USB Mini cable
	-   HDMI cable
	-   TV/ Monitor that supports HDMI
	-   Ethernet cable

## Setup your IoT Hub and provision a device
In order to run this sample, you will need to have a Device Management enabled IoT Hub instance.
Follow the [getting started with device management instructions](https://aka.ms/azureiotdmgetstarted) to create the IoT Hub and a device identity.
Make a copy of the device's connection string.

## Download or compile the device application
In order to simplify the experience with the Raspberry Pi, we have compiled and published the sample application in a package.

If you want to compile the application along with the rest of the SDK directly on the Pi, you can do so following these [instructions](../../../../../doc/get_started/raspbian-3gpi-c.md), but the below  instructions assume that you are downloading the app package.

To download the package:
-   Open a bash prompt
-   Navigate to ~
	```
	cd ~
	```
-   Download and unzip the application package
	```
	wget https://github.com/Azure/azure-iot-sdks-preview/releases/download/2016-09-30/iothub_client_sample_mqtt_dm.zip
	unzip iothub_client_sample_mqtt_dm.zip
	```
-   Edit the application configuration file and input the device connection string you retrieved in previous step
   
   *TODO* add details

## Prepare the firmware images
We will prepare 2 separate firmware images.
-   One will be considered the "old" firmware image based on Raspbian 4.1: [NOOBS_1.8.0](https://downloads.raspberrypi.org/NOOBS/images/NOOBS-2016-02-29/NOOBS_1_8_0.zip)
-   One will be considered the "new" firmware image based on a more recent version of Raspbian: [NOOBS_Latest](https://downloads.raspberrypi.org/NOOBS_latest)

We will need to prepare the 2 images the same way, so you will have to run through the following process for each of the 2 packages.
The process consists in downloading the zip package, unzipping it, patching the image adding the sample application, then zipping the package back again.

-   Open a bash prompt
-   Navigate to ~
	```
	cd ~
	```
-   Download and unzip the image package
	```
	wget https://downloads.raspberrypi.org/NOOBS/images/NOOBS-2016-02-29/NOOBS_1_8_0.zip
	mkdir NOOBS_1_8_0_image
	unzip NOOBS_1_8_0.zip -d NOOBS_1_8_0_image 
	cd NOOBS_1_8_0_image
	```
	or 
    ```
	wget https://downloads.raspberrypi.org/NOOBS_latest
	mkdir NOOBS_latest_image
	unzip NOOBS_latest -d NOOBS_latest_image 
	cd NOOBS_latest_image
	```
-   Change directory to os/Raspbian
	```
	cd os/Raspbian
	```
-   patch the os image with the demo application
    ```
	TODO copy app and config file into usr/sbin
	TODO copy app script in etc/init.d
	ln -s  usr/sbin/iothub_client_sample_mqtt_dm etc/rc5.d/iothub_client_sample_mqtt_dm
	```
-   Reassemble the image with the zip utility:
    ```
	cd ~
	zip -r patched_NOOBS_1_8_0.zip NOOBS_1_8_0_image
	```
	or
	```
	cd ~
	zip -r patched_NOOBS_latest.zip NOOBS_latest_image
	```

-   Place the new firmware package on Azure store and use it with the firmware_update method.


## Prepare your Device and run the application
Install the Raspbian operating system using the patched_NOOBS_1_8_0.zip image created during the previous step on your Raspberry Pi by following the instructions in the [NOOBS setup guide](http://www.raspberrypi.org/help/noobs-setup/).

When booting the first time, NOOBS will prompt the user (need keyboard and display attached to the Pi) for installing the Raspbian image. Once the image is installed and the Pi reboots, the sample application will start automatically and establish communication with Aure IoT Hub, waiting for a Firmware update command.

## Stage the new firmware and send the firmware update command 
In order for testing an actual firmware update, you will need to stage the new firmware image created earlier on some public storage. You can use your favorite online storage service or use an Azure Storage account.
If you decide to use an Azure storage account, the simplest way is to use the [Microsoft Azure Storage Explorer](http://storageexplorer.com/)., follow these instructions to upload the patched_NOOBS_latest.zip package.

Once the package is staged, note the URI for it.

In order to trigger the firmware update from an application, you can follow the instructions in this [document](https://aka.ms/azureiotfirmwareupdate), skipping the part about creating a simulated device and using the public URI for your new firmware image.

