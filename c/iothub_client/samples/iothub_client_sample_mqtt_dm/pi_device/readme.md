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
	-   USB mouse
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

If you want to compile the application along with the rest of the SDK directly on the Pi, you can do so following these [instructions](../../../../../doc/get_started/raspbian-3gpi-c.md), but the below instructions assume that you are downloading the app package.

## Prepare the firmware images
We will prepare 2 separate firmware images.
-   One will be considered the "old" firmware image based on Raspbian 4.1: [NOOBS_1.8.0](https://downloads.raspberrypi.org/NOOBS/images/NOOBS-2016-02-29/NOOBS_1_8_0.zip)
-   One will be considered the "new" firmware image based on a more recent version of Raspbian: [NOOBS_Latest](https://downloads.raspberrypi.org/NOOBS_latest)

## Prepare and Install an "old" firmware image
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
	
-   Prepare the image to install the included 4.1 firmware
    ```
	Open the recovery.cmdline file in your favorite editor and insert 'silentinstall lang=en keyboard=us' at the beginning of the single line. Without this step, NOOBS will automatically download and install the latest Raspbian image. 	
	```

-   Flash the device by following the instructions in the [NOOBS setup guide](http://www.raspberrypi.org/help/noobs-setup/).

-   Install the sample firmware_update package
	After the "old" image is flashed on the device, open a terminal window and install the demo
	```
	sudo apt-get install -y iothub_client_sample_firmware_update
	insert the device's connection string into file /usr/share/iothub_client_sample/.device_connection_string
	```

-   Start the firmware_update service
	```
	sudo systemctl start iothub_client_sample_firmware_update
	```

-   Save the contents from the /usr/share/iothub_client_sample directory on your build machine for the next step.
	```
	scp -r pi@[pi machine ip]:/usr/share/iothub_client_sample iothub_client_sample
	```

## Prepare the "new" firmware image
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

-   Reassemble the image with the zip utility:
    ```
	cd ~
	zip -r patched_NOOBS_latest.zip NOOBS_latest_image/*
	```

## Stage the new firmware and send the firmware update command 
In order for testing an actual firmware update, you will need to stage the new firmware image created earlier on some public storage. You can use your favorite online storage service or use an Azure Storage account.
If you decide to use an Azure storage account, the simplest way is to use the [Microsoft Azure Storage Explorer](http://storageexplorer.com/)., follow these instructions to upload the patched_NOOBS_latest.zip package.

Once the package is staged, note the URI for it.

In order to trigger the firmware update from an application, you can follow the instructions in this [document](https://aka.ms/azureiotfirmwareupdate), skipping the part about creating a simulated device and using the public URI for your new firmware image.

