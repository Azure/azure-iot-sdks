---
platform: debian
device: arietta g25
language: c
---

# Run a simple C sample on Arietta G25 device running Debian

# Table of Contents

-   [Introduction](#Introduction)
-   [Step 1: Prerequisites](#Prerequisites)
-   [Step 2: Prepare your Device](#PrepareDevice)
-   [Step 3: Build and Run the Sample](#Build)

<a name="Introduction"></a>
# Introduction

**About this document**

This document describes the process of connecting a [Arietta G25 (Low cost Linux embedded module)](http://www.acmesystems.it/arietta) device running Linux with Azure IoT SDK. This multi-step process includes:
-   Configuring Azure IoT Hub
-   Registering your IoT device
-   Build and deploy Azure IoT SDK on device

    ![](http://www.acmesystems.it/products/ARIETTA-G25.jpg)

###Main features###
-   CPU Atmel AT91SAM9G25 SoC (ARM9 @ 400Mhz)
-   DDR2 Ram 128 or 256 MByte
-   Power supply: 5VDC via microUSB or 3.3VDC using the PCB pads
-   Line levels: TTL 3.3V (NOT 5V tolerant)
-   Operative temperature range: 
	-   ARIETTA-G25 (128MByte RAM): -20 +70 °C
	-   ARIETTA-G25-256 (256MByte RAM): -0 +70 °C
-   Size: 53x25mm
-   PCB layers: 8
-   RoHS and REACH compliant. Low EMI
-   Availability: > 5 years
-   Designed and Made in Italy

###Productor###
[ACME SYSTEMS](http://www.acmesystems.it/)
   ![](http://www.acmesystems.it/images/logo_acme_h.jpg)

<a name="Prerequisites"></a>
# Step 1: Prerequisites

Before executing any of the steps below, read through each process, step by step to ensure complete understanding.

You should have the following items ready, before beginning the process:

-   A computer with GitHub installed and access to the
    [azure-iot-sdks](https://github.com/Azure/azure-iot-sdks) GitHub
    private repository.
-   SSH client, such as [PuTTY](http://www.putty.org/), to access the
    command line.
-   An Internet connection to update or download software. We recommend that you connect your PC to the internet either via ethernet cable or  wifi adapter. Arietta can use your PC without a WiFi adapter as a gateway in order to have access to the Debian packages repositories.
-   Required hardware:
	  - [Arietta G25](http://www.acmesystems.it/arietta)
	  - [IEE802.11 b/g/n Wi-Fi module](http://www.acmesystems.it/arietta_wifi) 
	  - A micro SD Card of 4,8,16 or 32 GByte class 4 or higher preinstalled with Debian Linux
	  - A micro USB data cable
	  - A 5 Volt DC power supply. You need a power supply only when you are using the Arietta in stand alone mode o with a WiFi or Ethernet adapter. Be careful that whatever power supply you use, it must output at least 5V; insufficient power will cause your Arietta to behave in strange ways. The power can be provided via the micro usb connector or via the J4 pads.
-   Download and install [DeviceExplorer](https://github.com/Azure/azure-iot-sdks/releases/download/2015-11-13/SetupDeviceExplorer.msi).
-   [Set up your IoT hub](https://github.com/Azure/azure-iot-sdks/blob/master/doc/setup_iothub.md).
#### Create a device on IoT Hub
-   With your IoT hub configured and running in Azure, follow the instructions in **"Create Device"** section of [DeviceExplorer Usage document](https://github.com/Azure/azure-iot-sdks/blob/master/tools/DeviceExplorer/doc/how_to_use_device_explorer.md).
#### Write down device credentials
-   Make note of the Connection String for your device by following the instructions in **"Get device connection string or configuration data"** section of [DeviceExplorer Usage document](https://github.com/Azure/azure-iot-sdks/blob/master/tools/DeviceExplorer/doc/how_to_use_device_explorer.md).

<a name="PrepareDevice"></a>
# Step 2: Prepare your Device

### Create a bootable microSD from Tarballs

A Linux PC is required.

-   Download the first microSD partition contents from [boot.tar.bz2](http://terzo.acmesystems.it/download/microsd/Arietta-13nov2015/boot.tar.bz2) (3.4MB)
-   Download the second microSD partition contents from: [rootfs.tar.bz2](http://terzo.acmesystems.it/download/microsd/Arietta-13nov2015/rootfs.tar.bz2) (182.4MB)
-   Format a microSD with gparted ([read more](http://www.acmesystems.it/microsd_format)) and mount it
-   Download the binaries
-   Un-tar the binaries to the microSD with the following commands:

	    $ tar -xvjpSf boot.tar.bz2 -C /media/$USER/boot
	    $ sudo tar -xvjpSf rootfs.tar.bz2 -C /media/$USER/rootfs

-   Unmount the microsd and use it to boot on your Arietta

#### Packages installed ####

-   gcc (C compiler)
-   g++ (C++ compiler)
-   Python 
-   Apache2 
-   PHP5
-   iw wpasupplicant
-   Git
-   Bonjour
-   Codiad (In browser Ascii editor)
-   Shell-in-a-box (In browser ssh client)
-   Minicom (Serial terminal)
-   Ntp

#### Login data ####
-   Login: **root** Password: **acmesystems**

#### Getting started with the following image ####
-   Insert the microSD on your PC (Windows, Linux pr Mac)
-   Edit with an ASCII editor the file **/boot/wpa_supplicant.conf** and insert your WiFi Access Point Name and Password
-   Boot Arietta with the microSD
-   Open a session from a PC wired on the same LAN with Bonjour installed.
-   web: [http://arietta.local](http://arietta.local)
-   ssh: root@arietta.local

#### Install the prerequisite package ####

-   Open a PuTTY session and connect to the device.

-   Upgrade sources.list

	    nano /etc/apt/sources.list

-   Add the following rows

	    # Debian Stable (Jessie)
	    deb http://ftp.it.debian.org/debian/ jessie main
	    deb-src http://ftp.it.debian.org/debian/ jessie main

	    # Aggiornamenti della sicurezza
	    deb http://security.debian.org/ jessie/updates main
	    deb-src http://security.debian.org/ jessie/updates main

-   Save your changes by pressing Ctrl+O and when nano prompts you to save it as the same file, just press ENTER. Press Ctrl+X to exit nano.

-   Execute the command

        apt-get update
        apt-get upgrade
        apt.get install apt-utils
        apt-get install build-essential
        apt-get install -y curl uuid-dev uuid g++ make cmake git unzip openjdk-7-jre
        apt-get install -y libssl-dev libcurl4-openssl-dev sasl2-bin openjdk-7-jdk

-   Synchronize date and time

        apt-get install ntpdate
        ntpdate ntp1.inrim.it

-   Download the SDK to the board by issuing the following command in PuTTY:

        git clone --recursive https://github.com/Azure/azure-iot-sdks.git

-   Verify that you now have a copy of the source code under the directory ~/azure-iot-sdks.

<a name="Build"></a>
# Step 3: Build and Run the sample

- There are two different samples available, one supporting AMQP protocol and the other supporting HTTP protocol. You can use one of these protocols to validate a sample on your device. Based on your choice of protocol, run the following command on device.

	For AMQP protocol:

		nano azure-iot-sdks/c/iothub_client/samples/iothub_client_sample_amqp/iothub_client_sample_amqp.c

	For HTTP protocol:

		nano azure-iot-sdks/c/iothub_client/samples/iothub_client_sample_http/iothub_client_sample_http.c

	This launches a console-based text editor. 
	Scroll down to the connection information.
	Find the following place holder for the IoT connection string:

		static const char* connectionString = "[device connection string]";

	Replace connection string with string which you noted in [Step 1](#Prerequisites).Save your changes by pressing Ctrl+O and when nano prompts you to save it as the same file, just press ENTER. Press Ctrl+X to exit nano.


- Azure IoT Hub SDK depends on Apache Qpid Proton AMQP/HTTP to integrate with the IoT Hub. Run the following command to build/install Apache Proton.

	Open the file:

		nano ./azure-iot-sdks/c/build_all/linux/build_proton.sh

	Find the following rows

		make
		make install

	Replace with

		free && sync && echo 3 > /proc/sys/vm/drop_caches && free
		make -j 5
		free && sync && echo 3 > /proc/sys/vm/drop_caches && free
		make -j 5 install

	Save your changes by pressing Ctrl+O and when nano prompts you to save it as the same file, just press ENTER. Press Ctrl+X to exit nano.


	Execute the following command

		free && sync && echo 3 > /proc/sys/vm/drop_caches && free
		./azure-iot-sdks/c/build_all/linux/build_proton.sh --install /usr
		chmod +x ./azure-iot-sdks/c/build_all/linux/build_paho.sh
		free && sync && echo 3 > /proc/sys/vm/drop_caches && free
		./azure-iot-sdks/c/build_all/linux/build_paho.sh

	Assuming everything went OK with the build_proton.sh and build_paho.sh, proceed to set the environment variables.

####Set the Environment Variables####

- Open IOT_DEVICE_PARAMS.TXT to edit.

		nano azure-iot-sdks/tools/iot_hub_e2e_tests_params/iot_device_params.txt

	Set the values for all the variables listed in the file as explained below.
	
		IOTHUB_CONNECTION_STRING: Connection string to your IoT Hub
		IOTHUB_DEVICE_ID: Id of the device you have registered
		IOTHUB_DEVICE_KEY: Primary key of the registered device 
		IOTHUB_EVENTHUB_CONNECTION_STRING: Connection string to your Event Hub
		IOTHUB_EVENTHUB_CONSUMER_GROUP: Set value as $Default
		IOTHUB_EVENTHUB_LISTEN_NAME: Name of your Event Hub
		IOTHUB_SHARED_ACCESS_SIGNATURE: this value can be generated from Device Explorer
		Go to Configuration tab −> Click Generate SAS button
		IOTHUB_PARTITION_COUNT: Set value as 16

	Save your changes by pressing Ctrl+O and when nano prompts you to save it as the same file, just press ENTER. Press Ctrl+X to exit nano.

	Running following command on your device:
			
		cd ./azure-iot-sdks/tools/iot_hub_e2e_tests_params/
		chmod +x setiotdeviceparametersfore2etests.sh
		free && sync && echo 3 > /proc/sys/vm/drop_caches && free
		./setiotdeviceparametersfore2etests.sh

	**Restart the Linux machine.**

- Build the SDK using following command.

		free && sync && echo 3 > /proc/sys/vm/drop_caches && free
		./azure-iot-sdks/c/build_all/linux/build.sh | tee LogFile.txt

	Note:*LogFile.txt in above command should be replaced with a file name where build output will be written, build.sh creates a folder called "cmake" in your home folder. 
	Inside "cmake" are all the results of the compilation of the complete software.*

<a name="#buildandrun"></a>
##Run the sample##

In this section you will run the Azure IoT client SDK samples of communication between your device and Azure IoT Hub. You will send messages to the Azure IoT Hub service and validate that IoT Hub has successfully received data. You will also monitor any messages sent from the Azure IoT Hub to client.

### Send Device Events to IOT Hub:

1.  Launch the DeviceExplorer as explained in [Step 1](#Prerequisites) and
    navigate to **Data** tab. Select the device name you created from the drop-down list of device IDs and click **Monitor** button.

	![](https://github.com/Azure/azure-iot-sdks/raw/master/doc/iotcertification/iot_certification_linux_c/images/3_3_1_01.png)
	
	DeviceExplorer is now monitoring data sent from the selected device to the IoT Hub.

3.  Run the sample by issuing following command.

    **If using AMQP protocol:** Run sample *iothub_client_sample_amqp*

        ~/cmake/iothub_client/samples/iothub_client_sample_amqp/iothub_client_sample_amqp

    **If using HTTP protocol:** Run sample *iothub_client_sample_http*

        ~/cmake/iothub_client/samples/iothub_client_sample_http/iothub_client_sample_http

4.  Verify that the confirmation messages show an OK. If not, then you may have incorrectly copied the device hub connection information.

    **If using AMQP protocol:**
    ![SampleAMQP\_result\_terminal](https://github.com/Azure/azure-iot-sdks/raw/master/doc/iotcertification/iot_certification_linux_c/images/3_3_1_02.png)

    **If using HTTP protocol:**
    ![SampleHTTP\_result\_terminal](https://github.com/Azure/azure-iot-sdks/raw/master/doc/iotcertification/iot_certification_linux_c/images/3_3_1_03.png)

5.  DeviceExplorer should show that IoT Hub has successfully received data sent by sample test.

    **If using AMQP protocol:**
    
    ![SampleAMQP\_result\_DeviceExplorer](https://github.com/Azure/azure-iot-sdks/raw/master/doc/iotcertification/iot_certification_linux_c/images/3_3_1_04.png)

    **If using HTTP protocol:**
    
    ![SampleHTTP\_result\_DeviceExplorer](https://github.com/Azure/azure-iot-sdks/raw/master/doc/iotcertification/iot_certification_linux_c/images/3_3_1_05.png)

### 3.3.2 Receive messages from IoT Hub

1.  To verify that you can send messages from the IoT Hub to your device, go to the **Message To Device** tab in DeviceExplorer.

2.  Select the device you created using the Device ID drop down.

3.  Add some text to the Message field, then click Send.

    ![MessageSend\_DeviceExplorer](https://github.com/Azure/azure-iot-sdks/raw/master/doc/iotcertification/iot_certification_linux_c/images/3_3_1_06.png)

4.  You should be able to see the command received in the console window for the client sample.

    **If using AMQP protocol:**
    ![MessageSend\_terminal](https://github.com/Azure/azure-iot-sdks/raw/master/doc/iotcertification/iot_certification_linux_c/images/3_3_1_07.png)

    **If using HTTP protocol:**
    ![MessageSend\_terminal](https://github.com/Azure/azure-iot-sdks/raw/master/doc/iotcertification/iot_certification_linux_c/images/3_3_1_08.png)
















