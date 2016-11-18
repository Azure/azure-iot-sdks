---
platform: Digi Embedded Linux
device: Digi XBee Gateway
language: c
---

Run a simple C sample on Digi XBee Gateway device running Digi Embedded Linux
===
---

# Table of Contents

-   [Introduction](#Introduction)
-   [Step 1: Prerequisites](#Step-1-Prerequisites)
-   [Step 2: Prepare your Device](#Step-2-PrepareDevice)
-   [Step 3: Build and Run the Sample](#Step-3-Build)

<a name="Introduction"></a>
# Introduction

**About this document**

This document describes how to connect a Digi XBee Gateway device running Digi Embedded Linux with Azure IoT SDK. This multi-step process includes:
-   Configuring Azure IoT Hub
-   Registering your IoT device
-   Build and deploy Azure IoT SDK on device

<a name="Step-1-Prerequisites"></a>
# Step 1: Prerequisites

You should have the following items ready before beginning the process:

-   A Linux machine with Git client installed and access to the
    [azure-iot-sdks](https://github.com/Azure/azure-iot-sdks) GitHub
    public repository.
-   Digi XBee Gateway device.
-   [Setup your IoT hub][lnk-setup-iot-hub]
-   [Provision your device and get its credentials][lnk-manage-iot-hub]

<a name="Step-2-PrepareDevice"></a>
# Step 2: Prepare your Device
-   Follow the instructions received with your [Digi XBee Gateway](https://www.digi.com/products/xbee-rf-solutions/gateways/xbee-gateway).
-   Connect your Digi XBee Gateway and obtain its IP address.
-   ssh into the Digi XBee Gateway by using (replace username with the username setup for your gateway and make sure that the correct IP address is used):

```
ssh {username}@{IP address}
```

-   When prompted type your password for your Digi XBee Gateway device.

<a name="Step-3-Build"></a>
# Step 3: Build and Run the sample

<a name="Step-3-1-Load"></a>
## 3.1 Build SDK and sample

On your Linux machine:

-   Install the prerequisite packages for cross compiling the Microsoft Azure IoT Device SDK for C by issuing the following commands from the command line on your board:

```
        sudo apt-get update

        sudo apt-get install -y curl libcurl4-openssl-dev build-essential cmake git
```

    ***Note:*** *This setup process requires cmake version 2.8.12 or higher.* 
    
    *You can verify the current version installed in your environment using the  following command:*

```
        cmake --version
```

-   Install the ARM GCC toolchain:

```
sudo apt-get install gcc-arm-linux-gnueabi
```

-   Download OpenSSL 1.0.1m. This is needed by the SDK to establish secure a connection to the IoTHub.

```
cd ~
git clone -b OpenSSL_1_0_1m https://github.com/openssl/openssl.git
```

-   Cross compile OpenSSL 1.0.1m:

Go to the location where openssl was cloned.

```
cd ~/openssl 
```

Configure OpenSSL for cross compile (openssldir specifies where the cross compiled version will be installed):
```
./Configure --openssldir=/usr/openssl_arm_install/ shared os/compiler:/usr/bin/arm-linux-gnueabi-gcc
``` 

Build:
```
make
```

Install the cross compiled OpenSSL:
```
sudo make install
```

-   Download the Microsoft Azure IoT Device SDK for C to the board by issuing the following command on the board:

```
cd ~
git clone --recursive https://github.com/Azure/azure-iot-sdks.git
```

-   Edit the following file using any text editor of your choice:

        azure-iot-sdks/c/serializer/samples/simplesample_amqp/simplesample_amqp.c

-   Find the following place holder for IoT connection string:

        static const char* connectionString = "[device connection string]";

-   Replace the above placeholder with device connection string you obtained in [Step 1](#Step-1-Prerequisites) and save the changes.

-   Create a toolchain file for cross-compiling the SDK. This file instructs cmake to use the arm toolchain to build the SDK.

Create the toolchain file named ~/toolchain-arm.cmake using you favorite editor (i.e. nano): 
Paste in the toolchain file:

```
INCLUDE(CMakeForceCompiler)

SET(CMAKE_SYSTEM_NAME Linux)    # this one is important
SET(CMAKE_SYSTEM_VERSION 1)     # this one not so much

# this is the location of the arm toolchain
SET(CMAKE_C_COMPILER /usr/bin/arm-linux-gnueabi-gcc)

# this is the file system root of the target
SET(CMAKE_FIND_ROOT_PATH /usr/arm-linux-gnueabi)

# search for programs in the build host directories
SET(CMAKE_FIND_ROOT_PATH_MODE_PROGRAM NEVER)

# for libraries and headers in the target directories
SET(CMAKE_FIND_ROOT_PATH_MODE_LIBRARY ONLY)
SET(CMAKE_FIND_ROOT_PATH_MODE_INCLUDE ONLY)
``` 

-   Build the SDK samples using the following command:

```
cd ~/azure-iot-sdks/c
mkdir cmake
cd cmake
cmake .. -DCMAKE_TOOLCHAIN_FILE=~/toolchain-arm.cmake -Dskip_unittests:bool=ON -Duse_http:bool=OFF -Duse_default_uuid:bool=ON -DCMAKE_C_FLAGS="-I/usr/openssl_arm_install/include -L/usr/openssl_arm_install/lib"
cd iothub_client/samples/iothub_client_sample_amqp
make
```

-   Copy the sample to your device by using:

```
scp ./iothub_client_sample_amqp {username}@{IP address}:~/iothub_client_sample_amqp
```

## 3.2 Send Device Events to IoT Hub

-   Run the sample on your device by running this in your SSH session that you previously established with your Digi device:  

```
~/iothub_client_sample_amqp
```

-   See [Manage IoT Hub][lnk-manage-iot-hub] to learn how to observe the messages IoT Hub receives from the application.

## 3.3 Receive messages from IoT Hub

-   See [Manage IoT Hub][lnk-manage-iot-hub] to learn how to send cloud-to-device messages to the application.


[lnk-setup-iot-hub]: ../setup_iothub.md
[lnk-manage-iot-hub]: ../manage_iot_hub.md