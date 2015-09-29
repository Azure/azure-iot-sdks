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
 * Supports pluggable serialization methods. JSON serialization is currently available now.


The library code:

* Is written in ANSI C (C99) to maximize code portability.
* Avoids compiler extensions.
* Exposes a platform abstraction layer to isolate OS dependencies (threading and mutual exclusion mechanisms, communications protocol e.g. HTTP). Refer to [porting guide](doc/porting_guide.md) for more information.

Azure IoT device SDK for C can be used with a broad range of OS platforms and devices. For a list of tested configurations [click here](../doc/tested_configurations.md).

## SDK directory structure
All C specific resources are located in the **azure-iot-sdks\c** folder. A description of the directories follows:
### /doc

This directory contains application development guides, such as:

- [Prepare your development environment to use the Azure IoT device SDK for C](doc/devbox_setup.md)
- [Setup IoT Hub](../doc/setup_iot_hub.md)
- [Setup a Protocol Gateway - (i.e. MQTT)](https://github.com/Azure/azure-iot-protocol-gateway/blob/master/README.md)
- [How to use the DeviceExplorer application to tests IoT Hub device connectivity](../tools/DeviceExplorer/doc/tools_device_explorer.md)
- [How to Port the C Libraries to other OS platforms](doc/porting_guide.md)

Run the sample application on:

- [BeagleBone Black with Debian OS](doc/run_sample_on_beaglebone_black_debian.md)
- [BeagleBone Black with Snappy OS](doc/run_sample_on_beaglebone_black_snappy.md)
- [BeagleBone Green](doc/run_sample_on_beaglebone_green.md)
- [Desktop Linux](doc/run_sample_on_desktop_linux.md)
- [Freescale K64F](doc/run_sample_on_freescale_k64f_mbed.md)
- [RasberryPi 2 with Raspbian OS](doc/run_sample_on_raspberrypi2_raspbian.md)
- [Desktop Windows](doc/run_sample_on_desktop_windows.md)
- [TI CC3200](doc/run_sample_on_ti_cc3200.md)

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

## API reference documentation

API reference documentation can be found online at http://azure.github.io/azure-iot-sdks/c/api_reference/index.html.

If you clone the repository to your local machine, you can also find a copy of the API reference documents here:

```
{IoT device SDK root}/c/doc/api_reference/html/index.html
```
