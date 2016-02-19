# Microsoft Azure IoT device SDK for C

The Microsoft Azure IoT device libraries for C contain code that facilitates building devices and applications that connect to and are managed by Azure IoT Hub services.

The device library consists of a set of reusable components with abstract interfaces that enable pluggability between stock and custom modules.

To meet the wide range of device requirements in the Internet of Things space, the C libraries are provided in source code form to support multiple form factors, operating systems, tools sets, protocols and communications patterns widely in use today.


## Features

 * Sends event data to Azure IoT based services.
 * Maps server commands to device functions.
 * Buffers data when the network connection is down.
 * Batches messages to improve communication efficiency.
 * Supports pluggable transport protocols. HTTPS, AMQP and MQTT are the protocols currently available now.
 * Supports pluggable serialization methods. JSON serialization is available now.


The library code:

* Is written in ANSI C (C99) to maximize code portability and broad platform compatibility.
* Avoids compiler extensions.
* Exposes a platform abstraction layer to isolate OS dependencies (threading and mutual exclusion mechanisms, communications protocol e.g. HTTP). Refer to [porting guide](doc/porting_guide.md) for more information.

Azure IoT device SDK for C can be used with a broad range of OS platforms and devices. For a list of tested configurations [click here](https://azure.microsoft.com/en-us/documentation/articles/iot-hub-tested-configurations/).

## Application development guides
For information on how to use this library refer to the documents below:

- [Prepare your development environment to use the Azure IoT device SDK for C](doc/devbox_setup.md)
- [Setup IoT Hub](../doc/setup_iothub.md)
- [Provision and manage devices](../doc/manage_iot_hub.md)
- [Azure IoT device SDK for C  tutorial](https://azure.microsoft.com/documentation/articles/iot-hub-device-sdk-c-intro/)
- [Setup a Protocol Gateway - (i.e. MQTT)](https://github.com/Azure/azure-iot-protocol-gateway/blob/master/README.md)
- [How to port the C libraries to other OS platforms](doc/porting_guide.md)
- [C API reference](http://azure.github.io/azure-iot-sdks/c/api_reference/index.html)

How to run the sample applications on:

- [BeagleBone Black with Debian OS](../doc/get_started/debian-beaglebone-black-c.md)
- [BeagleBone Black with Snappy OS](../doc/get_started/debian-beaglebone-black-snappy-c.md)
- [BeagleBone Green](../doc/get_started/debian-beaglebone-green-c.md)
- [Desktop Linux](../doc/get_started/linux-desktop-c.md)
- [Freescale K64F](../doc/get_started/mbed-freescale-k64f-c.md)
- [RasberryPi 2 with Raspbian OS](../doc/get_started/raspbian-raspberrypi2-c.md)
- [Desktop Windows](../doc/get_started/windows-desktop-c.md)
- [TI CC3200](../doc/get_started/ti-rtos-ti-cc3200-c.md)
- [openSUSE](../doc/get_started/opensuse-desktop-c.md)
- [Arduino Yun](../doc/get_started/openwrt-arduino-yun-c.md)
- [Toradex modules with Windows Embedded Compact 2013](../doc/get_started/wince2013-toradex-module-c.md)
- [Arduino WiFi 101 Shield](../doc/get_started/arduinoide-arduino-wifi101-c.md)

## SDK directory structure
All C specific resources are located in the **azure-iot-sdks\c** folder. A description of the directories follows:
### /doc
This directory contains application development guides and device setup instructions.

### /build_all

This folder contains platform-specific build scripts for the client libraries and dependent components.

### /common

The C library source code is contained in three directories: common, iothub_client and serializer. The Common folder contains components such as adapters and linked-list implementations used by all the C libraries. It includes the following subfolders:

   * build: has one subfolder for each platform (e.g. Windows, Linux, Mbed). Contains makefiles, batch files, and solutions that are used to generate the library binaries.
   * doc: contains requirements, designs notes, manuals.
   * inc: public include files.
   * src: client libraries source files.
   * tools: tools used for libraries.
   * unittests: unit tests for source code.


### /iothub_client

Contains Azure IoT Hub client components that provide the raw messaging capabilities of the library. Refer to the API documentation and samples for information on how to use it.

   * build: has one subfolder for each platform (e.g. Windows, Linux, Mbed). Contains makefiles, batch files, and solutions that are used to generate the library binaries.
   * doc: contains requirements, designs notes, manuals.
   * inc: public include files.
   * src: client libraries source files.
   * samples: contains the send event and receive message samples (with ports for the supported platforms).
   * unittests: unit tests for source code.

### /serializer

Contains libraries that provide modeling and JSON serialization capabilities on top of the raw messaging library. These libraries facilitate uploading structured data and command and control for use with Azure IoT services. Refer to the API documentation and samples for information on how to use it.

   * build: has one subfolder for each platform (e.g. Windows, Linux, Mbed). Contains makefiles, batch files, and solutions that are used to generate the library binaries.
   * doc: contains requirements, designs notes, manuals.
   * inc: public include files.
   * src: client libraries source files.
   * samples: contains the send event and receive message samples (with ports for the supported platforms).
   * unittests: unit tests for source code.

### /testtools

Contains tools that are currently used in testing the client libraries: Mocking Framework (micromock), Generic Test Runner (CTest), Unit Test Project Template, etc.

### /tools

Miscellaneous tools: compilembed, mbed_build, traceabilitytool (checks spec requirements vs code implementation).
