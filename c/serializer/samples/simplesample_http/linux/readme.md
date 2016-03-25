#Sample Details
--------------------
This sample does the following:
 
 * Uploads raw data (at the transport level) to Azure IoT Hub via HTTPS
 * Defines a model and uploads structured data to Azure IoT Hub based on that model via HTTPs

 Dependencies
----------------
- Ubuntu 14.04 OS onto the target device.
- Install required libraries and compiler tools.

# Installing required libraries and compiler tools
--------------------------------------------------
 Prerequisites:

* Your Linux device is connected to the Internet so that you can install packages that are needed to build the Sample.
* You have the "apt-get" command-line tool loaded in your Linux development computer.

Installing libraries and compiler tools

From the Linux command prompt, perform the following steps:

1. To make sure that you have the latest packages, type the following command and then press Enter:

	sudo apt-get update

2. To install curl, type the following command and press Enter:
	
	sudo apt-get install curl
3. To install the curl dev library, type the following and then press Enter:

	sudo apt-get install libcurl4-openssl-dev

4. To install uuid-dev, type the following and then press Enter:

	sudo apt-get install uuid-dev
5. To install uuid, type the following and then press Enter:

	sudo apt-get install uuid

6. To install g++, type the following and then press Enter:

	sudo apt-get install g++

7. To install make tool, type the following and then press Enter:

	sudo apt-get install make

8. To make sure that you have the latest versions of packages, type the following and then press Enter:
 
	sudo apt-get upgrade
				

#Building sample
----------------------
1. Copy the contents of the IoT SDK under \Source\Native to a directory of your choosing in the Linux system. 
2. Assuming that the name of this directory is /AzureIoT, navigate to /AzureIoT/Sources/Samples/SimpleSample/Linux directory
3. Update main.c with your account and device specific information
4. Run "make clean"
5. Run "make"
6. Navigate to /AzureIoT/Sources/Samples/SimpleSample/Linux/Debug directory
7. Run ./simplesample_http. The data you specified in main.c will be sent to Event Hub
