---
platform: raspbian
device: bitjoule magic box
language: c
---

Run a simple C sample on Bitjoule Magic Box device running Raspbian GNU/Linux
===
---

# Table of Contents

-   [Introduction](#Introduction)
-   [Step 1: Prerequisites](#Prerequisites)
-   [Step 2: Prepare your Device](#PrepareDevice)
-   [Step 3: Build and Run the Sample](#Build)


<a name="Introduction"></a>
# Introduction

**About this document**

This document describes how to connect Bitjoule Magic Box device running Raspbian GNU/Linux with Azure IoT SDK. This multi-step process includes:
-   Configuring Azure IoT Hub
-   Registering your IoT device
-   Build and deploy Azure IoT SDK on device

<a name="Prerequisites"></a>
# Step 1: Prerequisites

You should have the following items ready before beginning the process:

-   [Setup your IoT hub][lnk-setup-iot-hub]
-   [Provision your device and get its credentials][lnk-manage-iot-hub]
-   Bitjoule Magic Box device.
-   One x64 Linux machine, installed with Ubuntu-Server 14.04, amd64-based architecture

<a name="PrepareDevice"></a>
# Step 2: Prepare your Device
In the actual transactions, Bitjoule Magic Box should have already gathered the following data points coming from your vegetable factory being monitored by Bitjoule Magic Box. As part of Bitjoule IoT Cloud Services, these data will be stored in Microsoft Azure to be viewed remotely and|or processed for analysis in the cloud.

    -   Date/Time
    -   Its own IP address
    -   Humidity
    -   Temperature
    -   CO2 density PPM
    -   Wind speed m/s
    -   Light intensity PPFD
    -   Duty cycle % to regulate fans
    
    [Optional]
    -   EC
    -   PH
    -   Water flow, etc

However, in the set-up purpose of the sample Azure IoT SDK run, they are not necessary, and therefore there is no preparation necessary except for connecting the Magic box to the internet in order to access Azure IoT Hub.

<a name="Build"></a>
# Step 3: Build and Run the sample

<a name="Load"></a>
## 3.1 Build SDK and sample

-   On Ubuntu machine, install the prerequisite packages for the Microsoft Azure IoT Device SDK for C by issuing the following commands from the command line:

        sudo apt-get update
        sudo apt-get install -y curl uuid-dev libcurl4-openssl-dev build-essential cmake git

-   Check the cmake version and make sure it is 2.8.12 or higher.
		
        cmake --version

-   Check the gcc version and make sure it is 4.9 or higher.

        gcc --version

    *For information about how to upgrade your version of gcc on Ubuntu 14.04, see <http://askubuntu.com/questions/466651/how-do-i-use-the-latest-gcc-4-9-on-ubuntu-14-04>.*

-   Download the Microsoft Azure IoT Device SDK for C on Ubuntu PC by issuing following commands:

    ```
    cd ~
    mkdir Source
    cd Source
    git clone --recursive https://github.com/Azure/azure-iot-sdks.git
    ```

-   Obtain RaspberryPi development package.
    ```
    cd ~
    mkdir RPiTools
    cd RPiTools
    git clone https://github.com/raspberrypi/tools.git
    ```

-   Sync your Ubuntu machine from Bitjoule Magic Box.

    ***Note:*** *Please enable ssh by running `raspi-config` so that ssh and scp work from a remote machine.*

        cd ~/RPiTools/tools/arm-bcm2708/gcc-linaro-arm-linux-gnueabihf-raspbian-x64/arm-linux-gnueabihf
        rsync -rl --safe-links pi@<Bitjoule Magic Box>:/{lib,usr} 
	 
    *Please specify the IP address of the Bitjoule Magic Box, which is displayed in its LCD screen.*

    *Else you can log in the console and check `ifconfig wlan0`.*

-   Create the required file for cross-compiling SDK at following location:

        cd ~/Source/azure-iot-sdks/c/build_all/linux

-   Use any text editor and create the file named as **toolchain-rip.cmake**, and copy and paste the following content:

    ```
    INCLUDE(CMakeForceCompiler)

    SET(CMAKE_SYSTEM_NAME Linux)     # this one is important

    SET(CMAKE_SYSTEM_VERSION 1)     # this one not so much

    # this is the location of the amd64 toolchain targeting the Raspberry Pi
    SET(CMAKE_C_COMPILER $ENV{RPI_ROOT}/../bin/arm-linux-gnueabihf-gcc)

    # this is the file system root of the target
    SET(CMAKE_FIND_ROOT_PATH $ENV{RPI_ROOT})

    # search for programs in the build host directories
    SET(CMAKE_FIND_ROOT_PATH_MODE_PROGRAM NEVER)

    # for libraries and headers in the target directories
    SET(CMAKE_FIND_ROOT_PATH_MODE_LIBRARY ONLY)

    SET(CMAKE_FIND_ROOT_PATH_MODE_INCLUDE ONLY)
    ```

-   Edit the following file using any text editor of your choice:

        ~/Sources/azure-iot-sdks/c/iothub_client/samples/iothub_client_sample_http/iothub_client_sample_http.c 

-   Find the following place holder for IoT connection string:

        static const char* connectionString = "[device connection string]";

-   Replace the above placeholder with device connection string you obtained in [Step 1](#Prerequisites) and save the changes.

-   Edit following file to set environment variables:

        ~/Sources/azure-iot-sdks/tools/iot_hub_e2e_tests_params/iot_device_params.txt

-   Set environment variables by running following command on your device:

        cd ~/Sources/azure-iot-sdks/tools/iot_hub_e2e_tests_params/
        chmod +x setiotdeviceparametersfore2etests.sh
        sudo ./setiotdeviceparametersfore2etests.sh

-   Restart the Linux machine.

-   Build the SDK using following command.

        cd ~/Source/azure-iot-sdks/c/build_all/linux 
        ./build.sh --toolchain-file toolchain-rpi.cmake --skip-unittests --no-amqp --no-mqtt -cl -D__STDC_NO_ATOMICS__ -cl --sysroot=$RPI_ROOT | tee ~/Sources/LogFile.txt       

-   Copy the samples to Bitjoule Magic Box.

        cd ~/Source/azure-iot-sdks/c/cmake/iotsdk_linux/iothub_client/samples/iothub_client_sample_http/
        scp iothub_client_sample_http pi@<Bitjoule Magic Box>/home/pi/

    *Please specify the IP address of the Bitjoule Magic Box, which is displayed in its LCD screen, if LCD screen is not equipped, log in the console and check `ifconfig wlan0`*

## 3.2 Send Device Events to IoT Hub:

-   Now the following instructions are applied to Bitjoule Magic Box:

        cd ~

-   Run the sample by issuing following command:

        ./iothub_client_sample_http

-   See [Manage IoT Hub][lnk-manage-iot-hub] to learn how to observe the messages IoT Hub receives from the application.

## 3.3 Receive messages from IoT Hub

-   See [Manage IoT Hub][lnk-manage-iot-hub] to learn how to send cloud-to-device messages to the application.


[setup-devbox-linux]: https://github.com/Azure/azure-iot-sdks/blob/master/c/doc/devbox_setup.md
[lnk-setup-iot-hub]: ../setup_iothub.md
[lnk-manage-iot-hub]: ../manage_iot_hub.md
