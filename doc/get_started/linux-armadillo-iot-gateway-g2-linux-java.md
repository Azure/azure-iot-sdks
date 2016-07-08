---
platform: Debian wheezy
device: Armadillo-IoT Gateway G2
language: java
---

Run a simple JAVA sample on Armadillo-IoT Gateway G2 device running Debian wheezy
===
---

# Table of Contents

-   [Introduction](#Introduction)
-   [Step 1: Prerequisites](#Prerequisites)
-   [Step 2: Prepare your Device](#PrepareDevice)
-   [Step 3: Build and Run the Sample](#Build)

<a name="Introduction"/></a>
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
-   Armadillo-IoT Gateway G2 device.


<a name="Prepare_dev_env"></a>
## Prepare your development environment

- Download the ATDE v5 (Atmark Techno Development Environment v5) from here: [http://armadillo.atmark-techno.com/atde](http://armadillo.atmark-techno.com/atde)

- Open the .vmx image file in the downloaded archve with VMware.

- When after ATDE is booted, issue following commands in console (for install OpenJDK8 and maven):

		sudo apt-get install -y uuid uuid-dev
		sudo cp /etc/apt/sources.list /etc/apt/sources.list.wheezy
		sudo sed -i 's/wheezy/jessie/g' /etc/apt/sources.list
		sudo sh -c "echo 'deb http://ftp.jp.debian.org/debian jessie-backports main contrib non-free' >> /etc/apt/sources.list"
		sudo apt-get update
		sudo apt-get install -y openjdk-8-jdk maven

- Run the following command to set **JAVA_HOME** environment variable.

        export JAVA_HOME=/usr/lib/jvm/java-8-openjdk-amd64

- Make sure that the JAVA_HOME environment variable includes the full path to the JDK.

        update-alternatives --config java


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
			# usermod -G sudo atmark
			# exit
			$ exit

	- Then, install other packages via sudo (1)

			sudo apt-get install ntpdate
			sudo ntpdate ntp.nict.jp
			sudo apt-get install -y uuid curl

	- For OpenJDK8 and maven, modify the sources.list

			sudo cp /etc/apt/sources.list /etc/apt/sources.list.wheezy
			sudo sed -i 's/wheezy/jessie/g' /etc/apt/sources.list
			sudo sh -c "echo 'deb http://ftp.debian.org/debian jessie-backports main' >> /etc/apt/sources.list"
			sudo sed -i '/atmark/d' /etc/apt/sources.list
			sudo apt-get update

	- Then, install packages via sudo (2)

			sudo apt-get install -y openjdk-8-jdk maven

	- Run the following command to set **JAVA_HOME** environment variable.

			export JAVA_HOME=/usr/lib/jvm/java-8-openjdk-armel

	- Make sure that the JAVA_HOME environment variable includes the full path to the JDK.

			update-alternatives --config java

	- Recover the sources.list

			sudo cp /etc/apt/sources.list.wheezy  /etc/apt/sources.list
			sudo apt-get update

	- Prepare the directory for [the next step](#Step_3_1):

			cd
			mkdir -p work/IoT_SDK_Java

	- After that, shutdown the device:

			su 
			sync; halt

	- When the device is halted, powering off it and remove the SD card


<a name="Build"></a>
# Step 3: Build SDK and Run the sample

<a name="Step_3_1"/></a>
## 3.1 Build SDK and sample with ATDE
***In this section, you build the SDK and sample with the ATDE on the host PC, not on the device*** (You can build them on the device, but it may need too long time.)

-   Install the prerequisite packages for the Microsoft Azure IoT Device SDK for Java by issuing the commands from the command line on the ATDE which given [here](#Prepare_dev_env).
	-	Then, issuing the following commands:

			mkdir -p ~/work/IoT_SDK_Java
			cd ~/work/IoT_SDK_Java

-   Download the Microsoft Azure IoT Device SDK for Java to the board by issuing the following command on the ATDE:

        git clone https://github.com/Azure/azure-iot-sdks.git

-   Build the SDK using following command. 

        cd azure-iot-sdks/java/device
        mvn install -DskipTests=true | tee JavaSDK_Build_Logs.txt

### 3.1.1 Copy the built image to the device:
-	At first, archive the two directory (azure-iot-sdks/ and ~/.m2/repository/):

		cd ~/work/IoT_SDK_Java
		tar cvf azure-iot-sdks.tar ./azure-iot-sdks
		tar cvf repository.tar -C ~/.m2 ./repository

-	Mount the SD card which contain the rootfs of the device to ATDE.

-	Copy above two archives to the rootfs on the SD card:

		cp azure-iot-sdks.tar /media/rootfs/home/atmark/work/IoT_SDK_Java/
		cp repository.tar /media/rootfs/home/atmark/work/IoT_SDK_Java/

-	Unmount the SD card from the ATDE, and attach the device

-	Boot Debian on the device

-	When the Debian booted on the device, extract the archive and run the maven to execute SDK's test cases.

		cd ~/work/IoT_SDK_Java
		tar xvf ./azure-iot-sdks.tar
		mkdir ~/.m2
		tar xvf repository.tar -C ~/.m2
		cd azure-iot-sdks/java/device
		mvn install | tee JavaSDK_Build_Logs.txt

<a name="Step_3_2"/></a>
## 3.2 Run and Validate the Samples

<a name="Step_3_2_1"/></a>
### 3.2.1 Send Device Events to IoT Hub:

-   Navigate to the folder containing the executable JAR file for send event sample.

        cd azure-iot-sdks/java/device/samples/send-event/target

-   Run the sample by issuing following command.

    **If using AMQPS protocol:**

        java -jar ./send-event-{version}-with-deps.jar "{connection string}" "{number of requests to send}" "amqps"
    
    **If using HTTPS protocol:**

        java -jar ./send-event-{version}-with-deps.jar "{connection string}" "{number of requests to send}" "https"

    **If using MQTT protocol:**

        java -jar ./send-event-{version}-with-deps.jar "{connection string}" "{number of requests to send}" "mqtt"
          
    Replace the following in above command:
    
    -   `{version}`: Version of binaries you have build
    -   `{connection string}`: Your device connection string
    -   `{number of requests to send}`: Number of messages you want to send to IoT Hub

-   See [Manage IoT Hub][lnk-manage-iot-hub] to learn how to observe the messages IoT Hub receives from the application.

<a name="Step_3_2_2"/></a>
### 3.2.2 Receive messages from IoT Hub

-   Navigate to the folder containing the executable JAR file for the receive message sample.

        cd azure-iot-sdks/java/device/samples/handle-messages/target
     
-   Run the sample by issuing following command.

    **If using AMQPS protocol:**
   
        java -jar ./handle-messages-{version}-with-deps.jar "{connection string}" "amqps"
    
    **If using HTTPS protocol:**
   
        java -jar ./handle-messages-{version}-with-deps.jar "{connection string}" "https"

    **If using MQTT protocol:**
   
        java -jar ./handle-messages-{version}-with-deps.jar "{connection string}" "mqtt"
        
    Replace the following in above command:
    
    -   `{version}`: Version of binaries you have build
    -   `{connection string}`: Your device connection string
    -   `{number of requests to send}`: Number of messages you want to send to IoT Hub

-   See [Manage IoT Hub][lnk-manage-iot-hub] to learn how to send cloud-to-device messages to the application.


[setup-devbox-linux]: https://github.com/Azure/azure-iot-sdks/blob/master/doc/get_started/java-devbox-setup.md
[lnk-setup-iot-hub]: ../setup_iothub.md
[lnk-manage-iot-hub]: ../manage_iot_hub.md
