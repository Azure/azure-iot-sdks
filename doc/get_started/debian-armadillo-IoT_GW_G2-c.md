---
platform: Debian wheezy
device: Armadillo-IoT Gateway G2
language: c
---

Run a simple C sample on Armadillo-IoT Gateway G2 device running Debian wheezy
===
---

# Table of Contents

-   [Introduction](#Introduction)
-   [Step 1: Prerequisites](#Prerequisites)
-   [Step 2: Prepare your Device](#PrepareDevice)
-   [Step 3: Build and Run the Sample](#Build)
-   [Tips](#tips)

<a name="Introduction"></a>
# Introduction

**About this document**

This document describes how to connect Armadillo-IoT Gateway G2 device running Debian wheezy with Azure IoT SDK. This multi-step process includes:
-   Configuring Azure IoT Hub
-   Registering your IoT device
-   Build and deploy Azure IoT SDK on device

<a name="Prerequisites"></a>
# Step 1: Prerequisites

You should have the following items ready before beginning the process:

-   [Prepare your development environment](#Prepare_dev_env)
-   [Setup your IoT hub][lnk-setup-iot-hub]
-   [Provision your device and get its credentials][lnk-manage-iot-hub]
-   Armadillo-IoT G2 device.

<a name="PrepareDevice"></a>
# Step 2: Prepare your Device
-   Follow the instruction described this [page](http://manual.atmark-techno.com/armadillo-iot/armadillo-iotg-std_product_manual_ja-2.6.1/ch14.html) for setup Debian bootable SD memory card. Here is summary:

	-	Boot from NOR Flash
	-	Insert SD card to the slot and make boot partition with following commands:

			su
			fdisk /dev/mmcblk0
			d
			n
			p
			1

			w
			mke2fs -j -L rootfs /dev/mmcblk0p1

	-	Download and extract Debian wheezy boot image on the SD card with following commands:

			mount /dev/mmcblk0p1 /mnt/
			cd /mnt
			curl -O http://armadillo.atmark-techno.com/files/downloads/armadillo-iot/debian/debian-wheezy-armel_aiot_20150826.tar.gz
			tar zxf debian-wheezy-armel_aiot_20150826.tar.gz

	-	Reboot the Armadillo-IoT

			cd
			umount /mnt
			reboot

	-	Then, change the kernel parameter from the bootloader (hermit) and boot Debian from the SD card:

			setenv console=ttymxc1,115200 root=/dev/mmcblk0p1 noinitrd rootwait
			boot

-   After booting Debian from the SD card, install packages which neccessary to running the IoT SDK samples:

	- first, install sudo package

			$ su
			# apt-get update
			# apt-get install sudo
			# vigr
			...add 'atmark' to sudoer group
			# exit
			$ exit
	- Then, install other packages via sudo (1)

			sudo apt-get install ntpdate
			sudo ntpdate ntp.nict.jp
			sudo apt-get install -y uuid curl

	- For cmake 3.8, gcc/g++ 4.9 which newer than Debian wheezy, modify the sources.list

			sudo cp /etc/apt/sources.list /etc/apt/sources.list.wheezy
			sudo sed -i 's/wheezy/jessie/g' /etc/apt/sources.list
			sudo apt-get update
	- Then, install other packages via sudo (2)

			sudo apt-get install -y cmake g++ pkgconf
	- Recover the sources.list

			sudo cp /etc/apt/sources.list.wheezy  /etc/apt/sources.list
			sudo apt-get update

	- Prepare the directory for [the next step](#Load):

			cd
			mkdir work
			cd work
			mkdir IoT_SDK
	- After that, shutdown the device:

			su halt
	- When the device is halted, powering off it and remove the SD card

<a name="Build"></a>
# Step 3: Build and Run the sample

<a name="Load"></a>
## 3.1 Build SDK and sample with ATDE
***In this section, you build the SDK and sample with the ATDE on the host PC, not on the device*** (You can build them on the device, but it may take some time.)

-   Install the prerequisite packages for the Microsoft Azure IoT Device SDK for C by issuing the commands from the command line on the ATDE which given [here](#Prepare_dev_env).
	-	Then, issuing the following commands:

			cd ~/work
			mkdir IoT_SDK
			cd IoT_SDK
-   Download the Microsoft Azure IoT Device SDK for C to the board by issuing the following command on the ATDE:

        git clone --recursive https://github.com/Azure/azure-iot-sdks.git
-	Edit/Create the following file using any text editor of your choice (this file is necessary to cross build the SDK):

		azure-iot-sdks/c/build_all/linux/toolchain-armadillo.cmake

	-	Insert the following lines to the file:

			INCLUDE(CMakeForceCompiler)
			SET(CMAKE_SYSTEM_NAME Linux)
			SET(CMAKE_SYSTEM_VERSION 1)
			SET(CMAKE_C_COMPILER /usr/bin/arm-linux-gnueabi-gcc-4.9)
			SET(CMAKE_CXX_COMPILER /usr/bin/arm-linux-gnueabi-g++-4.9)
-	Edit the following file using any text editor of your choice (without this modification, the e2e-tests test cases will fail):

		azure-iot-sdks/c/testtools/iothub_test/src/iothub_account.c
	-	Make following modification aroud line 526 (at IoTHubAccount_GetEventhubListenName()):

			#if __linux__
			    static char  listenName[64];
			    char* envVarValue = getenv("IOTHUB_EVENTHUB_LISTEN_NAME");
			    if (envVarValue != NULL)
			    {
			        strcpy(listenName, envVarValue);
			        return listenName;
			    }
			    else
			    {
			        LogError("Failure retrieving Listen Name value.\n");
			        return NULL;
			    }
			#else   // not __linux__
			    return IoTHubAccount_GetIoTHubName(acctHandle);
			#endif  // __linux__
	-	The original line is:

			    return IoTHubAccount_GetIoTHubName(acctHandle);

-   Edit the following file using any text editor of your choice:

    **For AMQP protocol:**

        azure-iot-sdks/c/iothub_client/samples/iothub_client_sample_amqp/iothub_client_sample_amqp.c

    **For HTTPS protocol:**

        azure-iot-sdks/c/iothub_client/samples/iothub_client_sample_http/iothub_client_sample_http.c

    **For MQTT protocol:**

        azure-iot-sdks/c/iothub_client/samples/iothub_client_sample_mqtt/iothub_client_sample_mqtt.c

-   Find the following place holder for IoT connection string:

        static const char* connectionString = "[device connection string]";

-   Replace the above placeholder with device connection string you obtained in [Step 1](#Prerequisites) and save the changes.

-   Build the SDK using following command. (**This is cross building**; so after build is complete, all test cases will fail since it cannot execute the ARM binary on the x86 PC**)

        sudo azure-iot-sdks/c/build_all/linux/build.sh -cl -fsigned-char --toolchain-file azure-iot-sdks/c/build_all/linux/toolchain-armadillo.cmake
	-	You should remember the following two options:
	-	(1) -cl -fsigned-char :without this option, one of the test case in unittests will fail
	-	(2) --toolchain-file :for the cross build

### 3.1.1 Copy the built image to the device:

-	At first, you should modify the build.sh to keep the already built binaries (edit the file using any text editor of your choice):

		azure-iot-sdks/c/build_all/linux/build.sh
	- comment out the line 95 and 96:

			#rm -r -f ~/cmake
			#mkdir ~/cmake

-	Then, archive the two directory (azure-iot-sdks/ and cmake/):

		$ tar cvf azure-iot-sdks.tar ./azure-iot-sdks
		$ su
		# cd
		# tar cvf cmake.tar ./cmake
		# exit

-	Mount the SD card which contain the rootfs of the device to ATDE.

-	Copy above two archives to the rootfs on the SD card:

		cp azure-iot-sdks.tar /media/rootfs/home/atmark/work/IoT_SDK/
		sudo cp cmake.tar /media/rootfs/home/atmark/work/IoT_SDK/

-	Unmount the SD card from the ATDE, and attach the device

-	Boot Debian on the device

-	When the Debian booted on the device, extract the archive and run the build.sh to execute SDK's test cases.

		$ cd work/IoT_SDK
		$ tar xvf ./azure-iot-sdks.tar
		$ su
		# cd
		# tar xvf ./cmake.tar
		# cd ~atmark/work/IoT_SDK/
		# ./azure-iot-sdks/c/build_all/linux/build.sh -cl -fsigned-char | tee LogFile.txt

## 3.2 Send Device Events to IoT Hub:

-   Run the sample by issuing following command:

    **If using AMQP protocol:**

        azure-iot-sdks/c/cmake/iotsdk_linux/iothub_client/samples/iothub_client_sample_amqp/iothub_client_sample_amqp

    **If using HTTPS protocol:**

        azure-iot-sdks/c/cmake/iotsdk_linux/iothub_client/samples/iothub_client_sample_http/iothub_client_sample_http

    **If using MQTT protocol:**

        azure-iot-sdks/c/cmake/iotsdk_linux/iothub_client/samples/iothub_client_sample_mqtt/iothub_client_sample_mqtt

-   See [Manage IoT Hub][lnk-manage-iot-hub] to learn how to observe the messages IoT Hub receives from the application.

## 3.3 Receive messages from IoT Hub

-   See [Manage IoT Hub][lnk-manage-iot-hub] to learn how to send cloud-to-device messages to the application.

<a name="tips"></a>
# Tips

- If you just want to build the serializer samples, run the following commands:

  ```
  cd ./c/serializer/build/linux
  make -f makefile.linux all
  ```

[setup-devbox-linux]: https://github.com/Azure/azure-iot-sdks/blob/master/c/doc/devbox_setup.md
[lnk-setup-iot-hub]: ../setup_iothub.md
[lnk-manage-iot-hub]: ../manage_iot_hub.md


<a name="Prepare_dev_env"></a>
## Prepare your development environment

- Download the ATDE v5 (Atmark Techno Development Environment v5) from here: [http://armadillo.atmark-techno.com/atde](http://armadillo.atmark-techno.com/atde)

- Open the .vmx image file in the downloaded archve with VMware.

- When after ATDE is booted, issue following commands in console (for install gcc 4.9 and cmake 3.8):

		sudo apt-get install -y uuid uuid-dev
		sudo cp /etc/apt/sources.list /etc/apt/sources.list.wheezy
		sudo sed -i 's/wheezy/jessie/g' /etc/apt/sources.list
		sudo apt-get update
		sudo apt-get install -y gcc cmake

- Next, issue following commands in console (for install gcc 4.9 cross toolchain):

		mkdir work
		cd work
		mkdir pkgs
		cd pkgs
		wget http://launchpadlibrarian.net/191842424/libisl13_0.14-1_amd64.deb
		sudo dpkg -i libisl13_0.14-1_amd64.deb
		wget http://launchpadlibrarian.net/200853746/binutils-arm-linux-gnueabi_2.25-5ubuntu3cross1.106_amd64.deb
		sudo dpkg -i binutils-arm-linux-gnueabi_2.25-5ubuntu3cross1.106_amd64.deb
		wget http://launchpadlibrarian.net/200858128/gcc-4.9-arm-linux-gnueabi-base_4.9.2-10ubuntu10cross0.6_amd64.deb
		sudo dpkg -i gcc-4.9-arm-linux-gnueabi-base_4.9.2-10ubuntu10cross0.6_amd64.deb
		wget http://launchpadlibrarian.net/202006648/libc6-armel-cross_2.21-0ubuntu4cross1.108_all.deb
		sudo dpkg -i libc6-armel-cross_2.21-0ubuntu4cross1.108_all.deb
		wget http://launchpadlibrarian.net/200858152/libatomic1-armel-cross_4.9.2-10ubuntu10cross0.6_all.deb
		sudo dpkg -i libatomic1-armel-cross_4.9.2-10ubuntu10cross0.6_all.deb
		wget http://launchpadlibrarian.net/200858166/libgomp1-armel-cross_4.9.2-10ubuntu10cross0.6_all.deb
		sudo dpkg -i libgomp1-armel-cross_4.9.2-10ubuntu10cross0.6_all.deb
		wget http://launchpadlibrarian.net/200858150/libasan1-armel-cross_4.9.2-10ubuntu10cross0.6_all.deb
		sudo dpkg -i libasan1-armel-cross_4.9.2-10ubuntu10cross0.6_all.deb
		wget http://launchpadlibrarian.net/200858155/libgcc1-armel-cross_4.9.2-10ubuntu10cross0.6_all.deb
		sudo dpkg -i libgcc1-armel-cross_4.9.2-10ubuntu10cross0.6_all.deb
		wget http://launchpadlibrarian.net/200858197/libubsan0-armel-cross_4.9.2-10ubuntu10cross0.6_all.deb
		sudo dpkg -i libubsan0-armel-cross_4.9.2-10ubuntu10cross0.6_all.deb
		wget http://launchpadlibrarian.net/200858154/libgcc-4.9-dev-armel-cross_4.9.2-10ubuntu10cross0.6_all.deb
		sudo dpkg -i libgcc-4.9-dev-armel-cross_4.9.2-10ubuntu10cross0.6_all.deb
		wget http://launchpadlibrarian.net/200858119/cpp-4.9-arm-linux-gnueabi_4.9.2-10ubuntu10cross0.6_amd64.deb
		sudo dpkg -i cpp-4.9-arm-linux-gnueabi_4.9.2-10ubuntu10cross0.6_amd64.deb
		wget http://launchpadlibrarian.net/200858129/gcc-4.9-arm-linux-gnueabi_4.9.2-10ubuntu10cross0.6_amd64.deb
		sudo dpkg -i gcc-4.9-arm-linux-gnueabi_4.9.2-10ubuntu10cross0.6_amd64.deb
		wget http://launchpadlibrarian.net/200858196/libstdc++6-armel-cross_4.9.2-10ubuntu10cross0.6_all.deb
		sudo dpkg -i libstdc++6-armel-cross_4.9.2-10ubuntu10cross0.6_all.deb
		wget http://launchpadlibrarian.net/202006650/libc6-dev-armel-cross_2.21-0ubuntu4cross1.108_all.deb
		sudo dpkg -i libc6-dev-armel-cross_2.21-0ubuntu4cross1.108_all.deb
		wget http://launchpadlibrarian.net/200858193/libstdc++-4.9-dev-armel-cross_4.9.2-10ubuntu10cross0.6_all.deb
		sudo dpkg -i libstdc++-4.9-dev-armel-cross_4.9.2-10ubuntu10cross0.6_all.deb
		wget http://launchpadlibrarian.net/200858124/g++-4.9-arm-linux-gnueabi_4.9.2-10ubuntu10cross0.6_amd64.deb
		sudo dpkg -i g++-4.9-arm-linux-gnueabi_4.9.2-10ubuntu10cross0.6_amd64.deb

- Next, issue following commands in console (for install libcurl4-openssl-dev cross):

		sudo cp /etc/apt/sources.list.wheezy  /etc/apt/sources.list
		sudo apt-get update
		sudo apt-get download libcurl3:armel
		sudo apt-get download libcurl4-openssl-dev:armel
		sudo dpkg-cross --build --arch armel -M libcurl3_7.26.0-1+wheezy13_armel.deb
		sudo dpkg-cross --build --arch armel -M libcurl4-openssl-dev_7.26.0-1+wheezy13_armel.deb
		sudo dpkg -i libcurl3-armel-cross_7.26.0-1+wheezy13_all.deb
		sudo dpkg -i --force-conflicts --force-overwrite libcurl4-openssl-dev-armel-cross_7.26.0-1+wheezy13_all.deb