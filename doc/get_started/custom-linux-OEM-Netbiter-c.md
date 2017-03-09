---
platform: Linux
device: OEM Netbiter
language: c
---

Run a simple C sample on the HMS OEM Netbiter device running Linux
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

This document describes how to connect OEM Netbiter running HMS' embedded Linux distribution with Azure IoT SDK. This multi-step process includes:
-   Configuring Azure IoT Hub
-   Registering your IoT device
-   Cross complie and deploy Azure IoT SDK on device

<a name="Prerequisites"></a>
# Step 1: Prerequisites

You should have the following items ready before beginning the process:

-   [Prepare your development environment][setup-devbox-linux]
-   [Setup your IoT hub][lnk-setup-iot-hub]
-   [Provision your device and get its credentials][lnk-manage-iot-hub]
-   Netbiter device.
-   OEM Netbiter SDK

<a name="PrepareDevice"></a>
# Step 2: Prepare your Device
-   Setup OEM Netbiter SDK Cross-Development Toolchain, this will be used to cross compile Azure IoT SDK examples
-   Run the environment setup script ( source environment-setup-cortexa8hf-vfp-neon-oe-linux-gnueabi), this will define many environmental variables that will be used later
-   create a cmake toolchain file, called toolchain-oemNetbiter.cmake, in the directory /azure-iot-sdks/c/build_all/linux/
-   The following is an example of toolchain-oemNetbiter.cmake

```
INCLUDE(CMakeForceCompiler)

SET(CMAKE_SYSTEM_NAME Linux)     # this one is important
SET(CMAKE_SYSTEM_VERSION 1)     # this one not so much

set( CMAKE_CXX_FLAGS $ENV{CXXFLAGS}  CACHE STRING "" FORCE )
set( CMAKE_C_FLAGS $ENV{CFLAGS} CACHE STRING "" FORCE ) #same flags for C sources
set( CMAKE_LDFLAGS_FLAGS ${CMAKE_CXX_FLAGS} CACHE STRING "" FORCE ) #same flags for C sources
set( CMAKE_LIBRARY_PATH ${OECORE_TARGET_SYSROOT}/usr/lib )
set( CMAKE_FIND_ROOT_PATH $ENV{OECORE_TARGET_SYSROOT} $ENV{OECORE_NATIVE_SYSROOT} )
set( CMAKE_FIND_ROOT_PATH_MODE_PROGRAM NEVER )
set( CMAKE_FIND_ROOT_PATH_MODE_LIBRARY ONLY )
set( CMAKE_FIND_ROOT_PATH_MODE_INCLUDE ONLY )


# search for programs in the build host directories
SET(CMAKE_FIND_ROOT_PATH_MODE_PROGRAM NEVER)

# for libraries and headers in the target directories
SET(CMAKE_FIND_ROOT_PATH_MODE_LIBRARY ONLY)
SET(CMAKE_FIND_ROOT_PATH_MODE_INCLUDE ONLY)
```


<a name="Build"></a>
# Step 3: Build and Run the sample

<a name="Load"></a>
## 3.1 Download Azure IoThub SDK for C and cross compile


-   Download the Microsoft Azure IoT Device SDK for C to the board by issuing the following command on the board::

        git clone https://github.com/Azure/azure-iot-sdk-c.git

-   Edit the following files using any text editor of your choice:

    **For AMQP protocol:**

        azure-iot-sdks/c/iothub_client/samples/iothub_client_sample_amqp/iothub_client_sample_amqp.c

    **For HTTPS protocol:**

        azure-iot-sdks/c/iothub_client/samples/iothub_client_sample_http/iothub_client_sample_http.c

     **For MQTT protocol:**

        azure-iot-sdks/c/iothub_client/samples/iothub_client_sample_http/iothub_client_sample_mqtt.c

-   Find the following place holder for IoT connection string:

        static const char* connectionString = "[device connection string]";

-   Replace the above placeholder with device connection string you obtained in [Step 1](#Prerequisites) and save the changes.

-   For MQTT and AMQP, the CA certificates must be specified in the application. To accomplish this, further modifications to the source files are needed.
    First, copy the certificate array from certs/certs.c,  This array looks like
```
    const char certificates[] =
     /* Baltimore */
    "-----BEGIN CERTIFICATE-----\r\n"
```
    Copy this certificate into the iot_client_sample_<mqtt and amqp>.c file. 
    This example will use the MACRO USED_TRUSTED_CERTS to conditionally compile the certs.

```
    #define USED_TRUSTED_CERTS
    #ifdef USED_TRUSTED_CERTS
    const char certificates[] =
    /* Baltimore */
    "-----BEGIN CERTIFICATE-----\r\n"
    "MIIDdzCCAl+gAwIBAgIEAgAAuTANBgkqhkiG9w0BAQUFADBaMQswCQYDVQQGEwJJ\r\n"
    ....
    #endif
```     
    Additionally, the option to add certificates must be used. This is conditionally compiled by the macro MBED_BUILD_TIMESTAMP.
    This example will use the same function, and it will work if the mbed macro is replaced by the previously used macro

```
    #ifdef USED_TRUSTED_CERTS  // MBED_BUILD_TIMESTAMP
                // For mbed add the certificate information
                if (IoTHubClient_LL_SetOption(iotHubClientHandle, "TrustedCerts", certificates) != IOTHUB_CLIENT_OK)
                {
                    printf("failure to set option \"TrustedCerts\"\r\n");
                }
    #endif // USED_TRUSTED_CERTS // MBED_BUILD_TIMESTAMP
```    

-   Copy the cmake toolchain file to the following directory azure-iot-sdks/c/build_all/linux/   
-   Cross compile the SDK by executing build.sh in the directory azure-iot-sdks/c/build_all/linux/
    specify the toolchain file by adding the --toolchain-file parameter

        ./build.sh --toolchain-file toolchain-oemNetbiter.cmake


-  If the builds are successfull, you will find the executables in the following directories:
    ~/azure-iot-sdks/c/cmake/iotsdk_linux/iothub_client/samples/iothub_client_sample_amqp/iothub_client_sample_amqp
    ~/azure-iot-sdks/c/cmake/iotsdk_linux/iothub_client/samples/iothub_client_sample_http/iothub_client_sample_http
    ~/azure-iot-sdks/c/cmake/iotsdk_linux/iothub_client/samples/iothub_client_sample_mqtt/iothub_client_sample_mqtt

-   Copy the executables to the Netibiter
```
    scp <file name> apps@<ip-address>:./
```

## 3.2 Send Device Events to IoT Hub:

-   Run the sample by connecting to the Netbiter using SHH to run the executables


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
[lnk-setup-iot-hub]: ../../setup_iothub.md
[lnk-manage-iot-hub]: ../../manage_iot_hub.md
