# Firmware update of a Raspberry Pi 3 using Azure IoT Hub device management
The following instructions explain how to do a firmware update on a Raspberry Pi 3 running Raspbian using Azure IoT Hub.
The sample is a simple C application that will run on a Raspberry Pi, establish connection with Azure IoT Hub and wait for a "firmware update" call from Azure IoT Hub.
Before jumping in, it is recommended to read through the following documentation in order to understand the way device management is done using Azure IoT Hub:
-   [Overview of Azure IoT Hub Device Management](https://azure.microsoft.com/en-us/documentation/articles/iot-hub-device-management-overview/)
-   [Device Management tutorials](https://azure.microsoft.com/en-us/documentation/articles/iot-hub-device-management-get-started/)

## Prerequisites
You should have the following items ready before beginning the process:
-   Computer with Git client installed and access to the [azure-iot-sdks](https://github.com/Azure/azure-iot-sdks) GitHub public repository.
-   SSH client on your desktop computer, such as [PuTTY](http://www.putty.org/), so you can remotely access the command line on the Raspberry Pi.
-   If running MacOS or Linux, you are set.
-   If running Windows 10:
    -   64-bit version of Windows 10 Anniversary update build 14393 or later
    -   Follow the instructions [here](https://msdn.microsoft.com/en-us/commandline/wsl/install_guide) to install the Windows Subsystem for Linux. 
-   If running previous versions of Windows:
	-   [CygWin for Windows](https://www.cygwin.com/install.html), in order to run the few UNIX commands the following tutorial uses.
-   Required hardware:
	-   [Raspberry Pi 3](https://www.raspberrypi.org/products/raspberry-pi-3-model-b/) or [Raspberry Pi 2](https://www.raspberrypi.org/products/raspberry-pi-2-model-b/)
	-   8GB or larger MicroSD Card
	-   USB keyboard
	-   USB mouse (optinal)
	-   USB Mini cable
	-   HDMI cable
	-   TV/ Monitor that supports HDMI
	-   Ethernet cable (optional if you have a Raspbery Pi 3 and are connected through WiFi)

## Setup your IoT Hub and provision a device
In order to run this sample, you will need to have a Device Management enabled IoT Hub instance.
Follow the [getting started with device management instructions](https://azure.microsoft.com/en-us/documentation/articles/iot-hub-device-management-get-started#create-a-device-management-enabled-iot-hub) to create the IoT Hub and a device identity.
Make a copy of the device's connection string.

## The device application sample
In order to simplify the experience with the Raspberry Pi, we have created a simple application that acts as an agent.
The application needs to be installed on the current running image on the Raspberry Pi.
Once installed and started, the application establishes connection with Azure IoT Hub, reports on the current firmware version using the [Device Twins](https://azure.microsoft.com/en-us/documentation/articles/iot-hub-node-node-twin-getstarted/) feature (you can learn more on how to [use device twins to configure devices here](https://azure.microsoft.com/en-us/documentation/articles/iot-hub-node-node-twin-how-to-configure/)), and awaits for a "Firmware Update" command from the Cloud (through the [Device Method](https://azure.microsoft.com/en-us/documentation/articles/iot-hub-c2d-methods/) feature).
The instructions below show how to trigger the firmware update after preparing a new firmware image.
Once triggered, the application will download the new firmware image package, install it then reboot the device.
The new firmware image contains the application already, which is setup to run as a service at boottime, reporting on the new firmware image version through Device Twins.

The code for the sample application is available in the [azure-io-sdks](https://github.com/azure/azure-io-sdks) repository, but to simplify the exercise, we have compiled and published the sample application in a package.
If you want to compile the application along with the rest of the SDK directly on the Pi, you can do so following these [instructions](../../../../../doc/get_started/raspbian-3gpi-c.md), but the below instructions assume that you are downloading the app package.

## Run the sample
To run the sample, you will need to prepare an existing image running on a Raspberry Pi to run the sample application, and prepare a new firmware image that will be deployed to your device.
The instructions show how to update the firmware version to the latest Raspbian image contained in [NOOBS](https://downloads.raspberrypi.org/NOOBS_latest), this means you need to have an older version of the OS running on your Pi. If you already have an older version of the OS running on your device, you can skip the next step and go directly to the [installation of the application](#Install_the_sample_application).

### Install "older" Raspbian image on your Raspberry Pi
If you already have a version or Raspbian running on your Pi (which is NOT the latest image from NOOBS), you can skip this step and go directly to the [installation of the application](#Install_the_sample_application)
If you need to install an older version of Raspbian, the below instructions show how to install Raspbian 4.1 (using [NOOBS_1.8.0](https://downloads.raspberrypi.org/NOOBS/images/NOOBS-2016-02-29/NOOBS_1_8_0.zip)).

-   Open a bash prompt
-   Navigate to ~
	```
	cd ~
	```
	
-   Download and unzip the image package
	```
	wget https://downloads.raspberrypi.org/NOOBS/images/NOOBS-2016-02-29/NOOBS_v1_8_0.zip
	mkdir NOOBS_1_8_0_image
	unzip NOOBS_1_8_0.zip -d NOOBS_1_8_0_image 
	cd NOOBS_1_8_0_image
	```
	
-   Prepare the image to install the included 4.1 firmware automatically
    ```
	Open the recovery.cmdline file in your favorite editor and insert 'silentinstall lang=en keyboard=us' at the beginning of the single line. Without this step, NOOBS will automatically download and install the latest Raspbian image. 	
	```

-   Flash the device by following the instructions in the [NOOBS setup guide](http://www.raspberrypi.org/help/noobs-setup/).

### Install the sample application
Assuming you have an older version of Raspbian running on your Raspberry Pi, follow the below instructions to install the sample application:
-   Install the sample firmware_update package using SSH and typing the following commands:
	
	add the following lines to the /etc/apt/source.list files:
	```
	deb http://ppa.launchpad.net/aziotsdklinux/ppa-azureiot/ubuntu vivid main
	deb-src http://ppa.launchpad.net/aziotsdklinux/ppa-azureiot/ubuntu vivid main
	deb http://ppa.launchpad.net/aziotsdklinux/azureiot-firmware-update-sample/ubuntu vivid main 
	deb-src http://ppa.launchpad.net/aziotsdklinux/azureiot-firmware-update-sample/ubuntu vivid main
	```

	add the keys to the device:
	```
	sudo apt-key adv --keyserver keyserver.ubuntu.com --recv-keys FDA6A393E4C2257F
	```

	```
	sudo apt-get update
	sudo apt-get install -y iothub_client_sample_firmware_update
	```

-	Insert the device's connection string into file /usr/share/iothub_client_sample/.device_connection_string
-   Start the firmware_update service
	```
	sudo systemctl start iothub_client_sample_firmware_update
	```

-   When running, the application will update the reported state of its Device Twin to Azure IoT Hub. You can verify that youre device is correctly connected by checking the reported properties of the device. You can learn more [here](https://azure.microsoft.com/en-us/documentation/articles/iot-hub-node-node-twin-getstarted/) on how to work with Device Twins.

### Save the application and its settings for next step
For preparing the new firmware image, we will need to get a copy of the sample application as deployed and configured in the "old" firmware.

-   Save the contents from the /usr/share/iothub_client_sample directory on your build machine for the next step (still from SSH).
	```
	scp -r pi@[pi machine ip]:/usr/share/iothub_client_sample iothub_client_sample
	```

### Prepare the "new" firmware image
The following steps instruct how to prepare the new firmware image package, working offline on your build machine. As a starting point we are using the latest NOOBS image for Raspberry Pi. 
-   On your build machine run the following commands
    ```
	wget https://downloads.raspberrypi.org/NOOBS_latest
	mkdir NOOBS_latest_image
	unzip NOOBS_latest -d NOOBS_latest_image 
	cd NOOBS_latest_image
	```
	
-   Prepare the os image for sideloading the demo app
    ```
	cd os/Raspbian
	xz --decompress root.tar.xz
    ```

-   prepare a temporary staging location
    ```
	cd ~
	mkdir temp_root
	cd temp_root
	mkdir -p usr/share/iothub_client_sample
	mkdir -p lib/systemd/system
	mkdir -p etc/systemd/system/multi-user.target.wants
    ```

-   stage the demo app by copying the files which you had saved from the last step of "Prepare and Install an old firmware image" into the ~/temp_root/usr/share/iothub_client_sample directory.

-   set up the support files for the demo service app
    ```
	cd ~/temp_root/lib/systemd/system
	ln -sf ../../../usr/share/iothub_client_sample/iothub_client_sample_firmware_update.service iothub_client_sample_firmware_update.service
	cd ~/temp_root/etc/systemd/system/multi-user.target.wants
	ln -sf ../../../../usr/share/iothub_client_sample/iothub_client_sample_firmware_update.service iothub_client_sample_firmware_update.service
    ```

-   append the demo app to the root.tar
	```
	cd ~/temp_root
	tar -rf ~/NOOBS_latest_image/os/Raspbian/root.tar ./*
	cd ~/NOOBS_latest_image/os/Raspbian
	make a note of the new root.tar file size and update the "uncompressed_tarball_size" property in the partitions.json file.
	``` 

-   compress the root.tar image
	``` 
	cd ~/NOOBS_latest_image/os/Raspbian
	xz -9 -e --verbose root.tar
    ```

-   Prepare the image to install the included firmware automatically
    ```
	Open the recovery.cmdline file in your favorite editor and insert 'silentinstall lang=en keyboard=us' at the beginning of the single line. Without this step, NOOBS will automatically download and install the latest Raspbian image. 	
	```

-   Reassemble the image with the zip utility:
    ```
	cd ~
	zip -r patched_NOOBS_latest.zip NOOBS_latest_image/*
	```

### Stage the new firmware and send the firmware update command 
In order for testing an actual firmware update, you will need to stage the new firmware image created earlier on some public storage. You can use your favorite online storage service or use an Azure Storage account.
If you decide to use an Azure storage account, the simplest way is to use the [Microsoft Azure Storage Explorer](http://storageexplorer.com/)., follow these instructions to upload the patched_NOOBS_latest.zip package.

Once the package is staged, note the URI for it.

In order to trigger the firmware update from an application, you can follow the instructions in this [document](https://aka.ms/azureiotfirmwareupdate), skipping the part about creating a simulated device and using the public URI for your new firmware image.

