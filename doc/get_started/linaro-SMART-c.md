---
platform: Linaro
device: SMART
language: c
---

Run a simple C sample on SMART device running Linaro
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

This document describes how to connect SMART device running Linaro with Azure IoT SDK. This multi-step process includes:

-   Configuring Azure IoT Hub
-   Registering your IoT device
-   Build and deploy Azure IoT SDK on device

<a name="Prerequisites"></a>
# Step 1: Prerequisites

You should have the following items ready before beginning the process:


-   Micro USB to USB Cable.
-   SMART device(It have own wifi device to connect the Internet).
-   Take "Ubuntu 14.04 x86_64" as linaro develope system.
-   [Setup your IoT hub][lnk-setup-iot-hub]
-   [Provision your device and get its credentials][lnk-manage-iot-hub]

<a name="PrepareDevice"></a>
# Step 2: Prepare your Device
**On ubuntu 14.04 linux system,we can use micro USB cable to connect SMART device,use andriod-tools-adb to communicate datas with SMART device and run adb shell command. SO:**

-   install android device tools:

            sudo apt-get install -y android-tools-adb
- Connect SMART device with micro USB cable. 
- Open ubuntu linux bash shell and run:

            adb root
            adb shell
and then you can connect to SMART device.


<a name="Build"></a>
# Step 3: Build and Run the sample

<a name="Load"></a>
## 3.1 Build SDK and sample

-   Install the prerequisite packages for the Microsoft Azure IoT Device SDK for C by issuing the following commands from the command line on your board and in Ubuntu Linux:
 -   Install development environment:

          sudo apt-get update
          sudo apt-get install -y uuid-dev build-essential cmake git
 
          ***Note:*** *This setup process requires cmake version 2.8.12 or higher.* 
    
         *You can verify the current version installed in your environment using the  following command:*

              cmake --version

         *This library also requires gcc version 4.9 or higher. You can verify the current version installed in your environment using the following command:*
    
              gcc --version 
 -   Install SMART device toolchain (package from qualcomm official website).
 -   Install qrSDK and get rootfs (package from qualcomm official website). **set env param $QTI_SYSTEM_ROOT as  rootfs dirictory path**

-    Get [curl-7.42.0](https://curl.haxx.se/download/curl-7.42.0.tar.bz2) package and build libcurl with SMART toolchain:
  
           ./configure --host=arm-linux-gnueabi 
            make
            cp include/curl/ ${QTI_SYSTEM_ROOT}/usr/include/ -rf
            cp lib/.lib/libcurl.so* ${QTI_SYSTEM_ROOT}/usr/lib -r
  


-   Download the Microsoft Azure IoT Device SDK for C to the board by issuing the following command on the board::

        git clone --recursive https://github.com/Azure/azure-iot-sdks.git

-   Edit the following file using any text editor of your choice:
   

    For AMQP protocol:

        azure-iot-sdks/c/iothub_client/samples/iothub_client_sample_amqp/iothub_client_sample_amqp.c

    For HTTPS protocol:

        azure-iot-sdks/c/iothub_client/samples/iothub_client_sample_http/iothub_client_sample_http.c
        
    For MQTT protocol

        azure-iot-sdks/c/iothub_client/samples/iothub_client_sample_mqtt/iothub_client_sample_mqtt.c

-   Find the following place holder for IoT connection string:

        static const char* connectionString = "[device connection string]";

-   Replace the above placeholder with device connection string you obtained in [Step 1](#Prerequisites) and save the changes.
-   Create gcc_qti.cmake in azure-iot-sdks directory; gcc_qti.cmake file  show below:
    
        #gcc_qti.cmake
        INCLUDE(CMakeForceCompiler)
        
        SET(CMAKE_SYSTEM_NAME Linux)     # this one is important
        SET(CMAKE_SYSTEM_VERSION 1)     # this one not so much
        
        # this is the location of the amd64 toolchain targeting the Raspberry Pi
        SET(CMAKE_C_COMPILER $ENV{QTI_TOOLCHAIN_ROOT}/bin/arm-linux-gnueabihf-gcc)
        SET(CMAKE_CXX_COMPILER $ENV{QTI_TOOLCHAIN_ROOT}/bin/arm-linux-gnueabihf-g++)
        
        # this is the file system root of the target
        SET(CMAKE_FIND_ROOT_PATH $ENV{QTI_SYSTEM_ROOT})
        
        # search for programs in the build host directories
        SET(CMAKE_FIND_ROOT_PATH_MODE_PROGRAM NEVER)
        
        # for libraries and headers in the target directories
        SET(CMAKE_FIND_ROOT_PATH_MODE_LIBRARY ONLY)
        SET(CMAKE_FIND_ROOT_PATH_MODE_INCLUDE ONLY)

-   Build the SDK using following command.

        cd  ./azure-iot-sdks/
        sudo ./c/build_all/linux/build.sh --toolchain-file ./gcc_qti.cmake --skip-e2e-tests --skip-unittests  -cl -D__STDC_NO_ATOMICS__ -cl --sysroot=$QTI_SYSTEM_ROOT
-   Copy exe-data and dll to SMART device
    
        adb push ~/azure-iot-sdks/c/cmake/iotsdk_linux/iothub_client/samples/iothub_client_sample_http/iothub_client_sample_http /home/iottest/
        adb push ~/azure-iot-sdks/c/cmake/iotsdk_linux/iothub_client/samples/iothub_client_sample_mqtt/iothub_client_sample_mqtt /home/iottest/
        adb push ~/azure-iot-sdks/c/cmake/iotsdk_linux/iothub_client/samples/iothub_client_sample_amqp/iothub_client_sample_amqp /home/iottest/
        adb push ${QTI_SYSTEM_ROOT}/usr/lib/libcurl* /usr/lib/ 

## 3.2 Send Device Events to IoT Hub:

-   Enter into  adb shell
    
        adb shell
    
-   Run the sample by issuing following command:

    **Test AMQP protocol:**

        /home/iottest/iothub_client_sample_amqp
    **Test HTTPS protocol:**

        /home/iottest/iothub_client_sample_http

    **Test MQTT protocol:**

        /home/iottest/iothub_client_sample_mqtt

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

