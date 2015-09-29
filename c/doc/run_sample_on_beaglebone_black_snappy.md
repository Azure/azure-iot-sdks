# BeagleBone Black Setup

This file contains the following information:
- How to get a snap running that connects a BeagleBone Black board to Azure IoT Hubs.
- Connecting a [BeagleBone Black](http://beagleboard.org/black) device and building the SDK sample that sends data from the board to Azure IoT Hubs.

## Table of Contents
- [Requirements](#Requirements)
- [Before you begin](#beforebegin)
- [Prerequisites](#Prerequisites)
	- [Install Linux](#Install-Linux)
	- [Install Snappy](#Install-Snappy)
	- [Build QPID Proton](#Build-QPID-Proton)
	- [Build the Azure client](#Build-the-Azure-client)
- [Connect the board](#Connect-the-board)
- [Verify that you can connect to the device](#Verify-that-you-can-connect-to-the-device)
- [Build and run the Device Explorer tool](#Build-and-run-the-Device-Explorer-tool)
- [Load the Azure IoT bits and prerequisites](#Load-the-Azure-IoT-bits-and-prerequisites)
- [Build the Azure client](#Build-the-Azure-client)
- [Run the samples](#Run-the-samples)
- [Tips](#Tips)

<a name="Requirements"/>
## Requirements

- Computer with GitHub installed and access to the [azure-iot-sdks](https://github.com/Azure/azure-iot-sdks) GitHub private repository.
- SSH client, such as [PuTTY](http://www.putty.org/), so you can access the command line.
- Either the Chrome or Firefox web browser, so you can browse to the embedded webserver on your your board.
- The boards come with all the required software; however, to update the board and run the current sample you must use FTP or a USB driver to copy your files. You can also use [WinSCP](http://winscp.net/eng/index.php).
- Required hardware: [Beagleboard-BeagleBone](http://www.amazon.com/Beagleboard-BeagleBone-Starter-Case--Power-Supply--Micro/dp/B00P6TV9V4/ref=sr_1_3?ie=UTF8&qid=1426002583&sr=8-3&keywords=beaglebone).
- A microSD card attached to a machine where Linux is (or can be) installed. To prepare a microSD card your Linux installation must be able to access the microSD card.

<a name="beforebegin"/>
## Before you begin
Before you begin you will need to create and configure an IoT hub to connect to.
  1. [Set up your IoT Hub][setup-iothub].
  1. With your IoT hub configured and running in Azure, follow the instructions in [Connecting your device to an IoT hub][provision-device].
  1. Make note of the Connection String for your device from the previous step.

  > Note: You can skip this step if you just want to build the sample application without running it.

<a name="Prerequisites"/>
## Prerequisites

<a name="Install-Linux"/>
### Install Linux

- Install Ubuntu Linux 14.04 or later from [http://www.ubuntu.com/download/desktop/](http://www.ubuntu.com/download/desktop/).

<a name="Install-Snappy"/>
###  Install Snappy

- Prepare a microSD card with the Snappy Ubuntu Core operating system by following the "Getting started with a BeagleBone Black" instructions on [http://developer.ubuntu.com/en/snappy/start/#try-beaglebone](http://developer.ubuntu.com/en/snappy/start/#try-beaglebone).

  > Note: These instructions assume you have a Linux desktop machine that can write to a microSD card. If you have a Windows desktop machine, it is possible to prepare the microSD card using the [7-zip](http://www.7-zip.org/) utility to extract the image from the .xz archive and the [Win32DiskImager](http://sourceforge.net/projects/win32diskimager/) utility to write the image to the microSD card.

- Set up cross-compile to enable you to build for ARM on your desktop Ubuntu machine by following the instructions at [http://hypernephelist.com/2015/03/09/cross-compilation-made-easy-on-ubuntu-with-sbuild.html](http://hypernephelist.com/2015/03/09/cross-compilation-made-easy-on-ubuntu-with-sbuild.html). From now on the term _host machine_ is used to denote the desktop Ubuntu machine. The term _qemu machine_ denotes the virtual machine environment that is spawned when you execute the command `sudo sbuild-shell vivid-armhf` on the host machine.

  > Note: Do not follow the instructions in the blog post to build the qpid-proton library, use the instructions in this article instead.

- To download the source code, issue the following commands on the host machine:

		/var/lib/schroot/chroots/vivid-armhf/var/lib/sbuild/build$ git clone -b 0.9-IoTClient https://github.com/Azure/qpid-proton.git

	This downloads the qpid-proton source code.

		/var/lib/schroot/chroots/vivid-armhf/var/lib/sbuild/build$ git clone https://github.com/Azure/azure-iot-sdks.git

	This downloads the Azure IoT Hub device SDK that contains the IoT Hub client library source code and sample applications.

	> Note: These folders appear in the /var/lib/sbuild/build folder on the qemu machine.

- Install figlet:

		apt-get install figlet

<a name="Build-QPID-Proton"/>
### Build QPID Proton

- On the qemu machine, navigate to the /var/lib/sbuild folder and create a folder in the qpid-proton source code named "build":

		$ cd /build/qpid-proton
		$ mkdir build

- Switch to the newly created folder:

		$ cd build

- And run

		$ cmake ..
		$ make
		$ make install

	This builds the qpid-proton C libraries that the snap uses later.

<a name="Build-the-Azure-client"/>
### Build the Azure client

- On the qemu machine, modify vendingmachine.c to ignore a character by commenting out the `getchar()` function.

- On the qemu machine, follow the instructions to build, as if it was a real Linux machine:

		$ build.sh

- On the qemu machine, in the folder /build/azure-iot-sdks/serializer/samples/vendingmachine/ubuntu_snappy, execute ./build.sh:

		$ bash ./build.sh

	This compiles one file and adds some files to the folder.

- Return to the real machine.

		/var/lib/sbuild/build/azure-iot-sdks/serializer/samples/vendingmachine/ubuntu_snappy/snap$ snappy build .
		/var/lib/sbuild/build/azure-iot-sdks/serializer/samples/vendingmachine/ubuntu_snappy/snap$ snappy-remote --url=ssh://10.91.96.133 install ./vendingmachine_84_all.snap

	Make sure to substitute the IP address in this code with the IP of the board. You can determine the IP address by typing `ifconfig` on the board command line. The default user name/password for BeagleBone Black running Ubuntu Snappy is: **ubuntu:ubuntu**.

- Once the snap is installed, perform the following post installation steps:
	- edit /var/lib/apparmor/profiles/*vending* and add the line

			/dev/tty1 mrwlkix

		directly below the line containing

			/tmp/snapps/@{APP_PKGNAME}/@{APP_VERSION}/** mrwlkix

	- Add the executable stdbuf to the allowed list of executable programs by adding the following line:

			/{,usr/}bin/stdbuf ixr,

		just below the existing line

			/{,usr/}bin/stat ixr,/{,usr/}bin/stat ixr

	- Save the file and run the following commands:

			sudo apparmor_parser -r /var/lib/apparmor/profiles/click_vendingmachine_vendingmachine_84

			sudo reboot

		The board reboots and the service will be running.


<a name="Connect-the-board"/>
## Connect the board

- Plug the USB cable into your computer, so you can access the board (your PC will install drivers, etc., the first time). You can use the same USB cable to power the board; it's not necessary to use the separate power block.
- Use one of the following two options to install the Windows drivers:
	-  Download the [x64](http://beagleboard.org/static/Drivers/Windows/BONE_D64.exe) or [x86 (32-bit)](http://beagleboard.org/static/Drivers/Windows/BONE_DRV.exe) drivers. On x64, you only need the x64 drivers.
	-  The drivers (Linux and Windows) are already on the board. When you connect it via USB, the board appears as another drive letter called **BeagleBone Getting Started**. You can find the drivers under **[DRIVE LETTER]:\Drivers\Windows\[BONE\_D64.exe, and BONE\_DRV.exe]**.

<a name="Verify-that-you-can-connect-to-the-device"/>
## Verify that you can connect to the device

- Once you see that your board is alive, use Chrome or Firefox to open **http://192.168.7.2**.
- (Recommended)  Go to the [Interactive Guide](http://192.168.7.2/Support/BoneScript/) page and go through the basic samples. For example, turn the LEDs on, turn LEDs off, etc. Verify that your board behaves as expected.
- Open an SSH terminal program (e.g., [PuTTY](http://www.putty.org/)).
- Use Host name=192.168.7.2, Port=22, Connection type=SSH.
- When prompted, log in with username **root**, no password.
- Connect an Ethernet cable to your board. Ping a website, e.g., bing.com, to verify connectivity.
	- If ping doesn't work, reboot your board, log in again using PuTTY, and run the command **ifconfig**. You should see the **eth0**, **lo**, and **usb0** interfaces.

<a name="Build-and-run-the-Device-Explorer-tool"/>
## Build and run the Device Explorer tool

You can use the Device Explorer sample application on your Windows desktop machine to create and register a device ID and symmetric key for your device. The Device Explorer interfaces with Azure IoT Hubs, and has some basic capabilities such as:

- **Device management**: creates device IDs and obtains a list of registered devices on your IoT Hub.
- **Monitors and consumes data** sent by your devices to your IoT Hub.
- **Sends messages** to your devices.

To run this tool, you need connection and configuration information for your IoT Hub and the Event Hub it is associated with.

- Clone or download a copy of the [azure-iot-sdks](https://github.com/Azure/azure-iot-sdks) GitHub repository on your Windows desktop machine.
- In Visual Studio, open the DeviceExplorer.sln solution in the tools\\DeviceExplorer folder in your local copy of the repository, then build and run it.
- Paste the two connection strings for your IoT Hub and Event Hub, and then click **Update**.
- Click the **Management** tab, then create a device ID for your device and register your device with your IoT Hub:
	1. Click **List** to call the device hub and retrieve a list of devices. If this is your first time, then you shouldn't retrieve anything!
	2. Click **Create** to create a device ID and key.
	3. Save this information in Notepad (or just keep the sample running). You will need this when you configure your device.

<a name="Load-the-Azure-IoT-bits-and-prerequisites"/>
## Load the Azure IoT bits and prerequisites

- Open a PuTTY session and connect to the board, as described in the previous section "Verify that you can connect to the device."
- Install the prerequisite packages by issuing the following commands from the command line on the board:

		sudo apt-get update
		sudo apt-get install -y curl libcurl4-openssl-dev uuid-dev uuid g++ make cmake git

- Download the SDK to the board by issuing the following command in PuTTY:

		git clone https://github.com/Azure/azure-iot-sdks.git

	You will be prompted for your GitHub username and password -- if you have two-factor authentication enabled for your account, you'll need to generate and use a personal access token in place of your password.

- Verify that you now have a copy of our source code under the directory ~/azure-iot-sdks.

<a name="Build-the-Azure-client"/>
## Update the Send sample

- Before performing these steps, you'll need the following prerequisites:

	- Device hub name.
	- Device ID (from Device Explorer).
	- Device key (from Device Explorer).

- Now, build the sample:

	- On the board, run the following command:

			nano azure-iot-sdks/c/iothub_client/samples/iothub_client_sample_amqp/iothub_client_sample_amqp.c

	- This launches a console-based text editor. Scroll down to the connection information.

	- Copy your saved credentials, and paste them here (by right-clicking).

	- Save your changes by pressing **Ctrl+O**, and when nano prompts you to save it as the same file, just press **ENTER**.

	- Press **Ctrl+X** to exit nano.

<a name="Build-the-Azure-client"/>
## Build the sample

The Azure IoT Hub SDK depends on Apache Qpid Proton (AMQP) to integrate with the device hub and Event Hubs.

- On the board, run the following command to build/install Apache Proton:

		sudo ~/azure-iot-sdks/c/build_all/linux/build_proton.sh --install /usr

- Assuming everything went OK on the build\_proton.sh, you can now build the SDK code using the following command:

		~/azure-iot-sdks/c/build_all/linux/build.sh

	**Note:** If you receive the following error, ignore it: **"crtabstractions\_unittests/crtabstractions\_unittests.cpp:119:5"**.

<a name="Run-the-samples"/>
## Run the samples

- Run the **iothub\_client\_sample\_amqp** sample by issuing the following command:

		~/azure-iot-sdks/c/iothub_client/samples/iothub_client_sample_amqp/linux/iothub_client_sample_amqp

- Verify that the **Confirmation** messages show an **OK**. If not, then you may have incorrectly pasted the device hub connection information.

<a name="Tips"/>
## Tips

- If you just want to build iothub\_client, run the following commands:

		cd ~/azure-iot-sdks/c/iothub_client/build/linux
		make -f makefile.linux all

[setup-iothub]: ../../doc/setup_iothub.md
[provision-device]: ./provision_device.md
