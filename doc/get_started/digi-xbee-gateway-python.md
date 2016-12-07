---
platform: Digi Embedded Linux
device: Digi XBee Gateway
language: python
---

Run a simple python sample on Digi XBee Gateway device running Digi Embedded Linux
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

This document describes how to connect a Digi XBee device running Digi Embedded Linux with Azure IoT SDK. This multi-step process includes:
-   Configuring Azure IoT Hub
-   Registering your IoT device
-   Build and deploy Azure IoT SDK on device

<a name="Step-1-Prerequisites"></a>
# Step 1: Prerequisites

You should have the following items ready before beginning the process:

-   A Linux machine with Git client installed and access to the
    [azure-iot-sdk-python](https://github.com/Azure/azure-iot-sdk-python) GitHub public repository.
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

<a name="Step-3-1-Install-Tools"></a>
## 3.1 Install tools needed for cross-compile

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

- Install g++ for ARM Linux:

```
sudo apt-get install g++-arm-linux-gnueabi
```

<a name="Step-3-2-Cross-Compile-OpenSSL"></a>
## 3.1 Cross-compile OpenSSL

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

<a name="Step-3-3-Cross-Python"></a>
## 3.1 Cross-compile Python

-   Download Python 2.7.1 and extract it in a folder (~/Python-2.7.1)

```
cd ~
wget https://www.python.org/ftp/python/2.7.1/Python-2.7.1.tgz
tar -xzf Python-2.7.1.tgz
```

-   Configure Python to be cross-compiled:

```
cd ~/Python-2.7.1
./configure
make python Parser/pgen
mv python hostpython
mv Parser/pgen Parser/hostpgen
make distclean
```

-   Apply needed patch for Python cross compile

```
sudo apt install looptools
wget http://www.cnx-software.com/patch/python-2.7.1-cross-compile.patch
patch -p1 < python-2.7.1-cross-compile.patch
```

-   Cross-compile:

```
CC=arm-linux-gnueabi-gcc CXX=arm-linux-gnueabi-g++ AR=arm-linux-gnueabi-ar RANLIB=arm-linux-gnueabi-ranlib ./configure --host=arm-linux-gnueabi --target=arm-linux-gnueabi --prefix=/~/python_install
make HOSTPYTHON=./hostpython HOSTPGEN=./Parser/hostpgen BLDSHARED="arm-linux-gnueabi-gcc -shared" CROSS_COMPILE=arm-linux-gnueabi- CROSS_COMPILE_TARGET=yes
```

<a name="Step-3-4-Cross-Compile-Boost-Python"></a>
## 3.1 Cross-compile Boost Python

-   Download Boost (1.62.0) and extract it in a folder (~/boost_1.62.0)

```
cd ~
wget https://sourceforge.net/projects/boost/files/boost/1.62.0/boost_1_62_0.tar.gz
tar -xzf boost_1_62_0.tar.gz
```

-   Bootstrap boost by running the following in the folder where boost was extracted:

```
        cd ~/boost_1_62_0
        ./bootstrap.sh
```

-   Edit Modify the configuration file (project-config.jam) to use the ARM toolchain by replacing the line with "using gcc" by:

```
using gcc : : /usr/bin/arm-linux-gnueabi-g++ ;  
``` 

Also make sure the following portion is in the project-config.jam:

```
# Python configuration
import python ;
if ! [ python.configured ]
{
    using python : 2.7 : ../python_install : ../Python-2.7.1 ../Python-2.7.1/Include : ../Python-2.7.1/lib ;
}
``` 

-   Build by:

```
sudo ./b2 install --prefix=/usr/boost_local
``` 

<a name="Step-3-5-Cross-Compile-IoTHub-SDK-For-Python"></a>
## 3.1 Cross-compile the Azure IoTHub SDK for Python

-   Download the Microsoft Azure IoT Device SDK for Python:

```
cd ~
git clone --recursive https://github.com/Azure/azure-iot-sdk-python.git
```

-   Create a toolchain file for cross-compiling the SDK. This file instructs cmake to use the arm toolchain to build the SDK.

Create the toolchain file named ~/toolchain-arm.cmake using you favorite editor (i.e. nano): 
Paste in the toolchain file:

```
INCLUDE(CMakeForceCompiler)

SET(CMAKE_SYSTEM_NAME Linux)    # this one is important
SET(CMAKE_SYSTEM_VERSION 1)     # this one not so much

# this is the location of the arm toolchain
SET(CMAKE_C_COMPILER /usr/bin/arm-linux-gnueabi-gcc)
SET(CMAKE_CXX_COMPILER /usr/bin/arm-linux-gnueabi-g++)

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
cd ~/azure-iot-sdk-python
mkdir cmake
cd cmake
cmake ../c -DCMAKE_TOOLCHAIN_FILE=~/toolchain-arm.cmake -Dskip_unittests:bool=ON -Duse_http:bool=OFF -Duse_default_uuid:bool=ON -Dbuild_python:STRING="2.7" -DCMAKE_C_FLAGS="-I/usr/openssl_arm_install/include -L/usr/openssl_arm_install/lib" -DCMAKE_CXX_FLAGS="-I/usr/openssl_arm_install/include -I/usr/boost_local/include -I/home/{username}/Python-2.7.1 -I/home/{username}/Python-2.7.1/Include -L/usr/openssl_arm_install/lib -L/usr/boost_local/lib -L/home/{username}/Python-2.7.1 -Wno-deprecated-declarations" -DPYTHON_INCLUDE_DIR=/home/{username}/Python-2.7.1/Include -DBOOST_ROOT=/usr/boost_local -DPYTHON_LIBRARY=/home/{username}/Python-2.7.1/libpython2.7.a
cd python
make
```

Note that {username} is the user that you are being logged under, as the Pyhton cross compiled bits are in the home directory of that user.

<a name="Step-3-6-Copy-Bits-To-The-Device"></a>
## 3.1 Copy the sample bits to the device

-   Copy the Python binding shared library by using:

```
scp ~/azure-iot-sdk-python/cmake/python/src/iothub_client.so {username}@{IP address}:~/iothub_client.so
```

-   Copy the sample bits to your device by using:

```
scp ~/azure-iot-sdk-python/device/samples/iothub_client_sample.py {username}@{IP address}:~/iothub_client_sample.py
scp ~/azure-iot-sdk-python/device/samples/iothub_client_args.py {username}@{IP address}:~/iothub_client_args.py
scp ~/azure-iot-sdk-python/device/samples/iothub_client_class.py {username}@{IP address}:~/iothub_client_class.py
```

-    Copy the C++ standard library runtime libstdc++.so.6 and libstdc++.so.6.0.17 to the device. The shared library has to be the one built for arm.

-    Copy the libboost Python shared library to the device:

```
scp /usr/boost_local/lib/libboost_python.so {username}@{IP address}:~/libboost_python.so
scp /usr/boost_local/lib/libboost_python.so.1.62.0 {username}@{IP address}:~/libboost_python.so.1.62.0
```

## 3.3 Send Device Events to IoT Hub

-   Run the sample on your device by running this in your SSH session that you previously established with your Digi device:  

```
python iothub_client_sample.py -p amqp -c "<<<Enter your connection string here>>>"
```

-   See [Manage IoT Hub][lnk-manage-iot-hub] to learn how to observe the messages IoT Hub receives from the application.

## 3.4 Receive messages from IoT Hub

-   See [Manage IoT Hub][lnk-manage-iot-hub] to learn how to send cloud-to-device messages to the application.


[lnk-setup-iot-hub]: ../setup_iothub.md
[lnk-manage-iot-hub]: ../manage_iot_hub.md
