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

Azure IoT device SDK for C can be used with a broad range of OS platforms and devices. For a list of tested configurations [click here](https://catalog.azureiotsuite.com/).

## Application development guides
For information on how to use this library refer to the documents below:

- [Prepare your development environment to use the Azure IoT device SDK for C](doc/devbox_setup.md)
- [Setup IoT Hub](../doc/setup_iothub.md)
- [Provision and manage devices](../doc/manage_iot_hub.md)
- [Azure IoT device SDK for C  tutorial](https://azure.microsoft.com/documentation/articles/iot-hub-device-sdk-c-intro/)
- [Setup a Protocol Gateway - (i.e. MQTT)](https://github.com/Azure/azure-iot-protocol-gateway/blob/master/README.md)
- [How to port the C libraries to other OS platforms](doc/porting_guide.md)
- [Cross compilation example](doc/SDK_cross_compile_example.md)
- [C API reference](http://azure.github.io/azure-iot-sdks/c/api_reference/index.html)

For guides on how to run the sample applications on supported platforms, check out in this [folder](../doc/get_started/).

## SDK folder structure

All C specific resources are located in the **azure-iot-sdks\c** folder. A description of the key directories follows:

### /c-utility, /uamqp, and /umqtt

These are git submodules that contain code, such as adapters and protocol implementations, shared with other projects. Note that some of them contain nested submodules.

### /blob

This folder contains client components that enable access to Azure blob storage.

### /certs

Contains certificates needed to communicate with Azure IoT Hub.

### /doc

This folder contains application development guides and device setup instructions.

### /build_all

This folder contains platform-specific build scripts for the client libraries and dependent components.

### /iothub_client

Contains Azure IoT Hub client components that provide the raw messaging capabilities of the library. Refer to the API documentation and samples for information on how to use it.

   * build: has one subfolder for each platform (e.g. Windows, Linux, Mbed). Contains makefiles, batch files, and solutions that are used to generate the library binaries.
   * devdoc: contains requirements, designs notes, manuals.
   * inc: public include files.
   * src: client libraries source files.
   * samples: contains the send event and receive message samples (with ports for the supported platforms).
   * tests: unit and end-to-end tests for source code.

### /serializer

Contains libraries that provide modeling and JSON serialization capabilities on top of the raw messaging library. These libraries facilitate uploading structured data and command and control for use with Azure IoT services. Refer to the API documentation and samples for information on how to use it.

   * build: has one subfolder for each platform (e.g. Windows, Linux, Mbed). Contains makefiles, batch files, and solutions that are used to generate the library binaries.
   * devdoc: contains requirements, designs notes, manuals.
   * inc: public include files.
   * src: client libraries source files.
   * samples: contains the send event and receive message samples (with ports for the supported platforms).
   * tests: unit tests and end-to-end tests for source code.

### /iothub_service_client

Contains libraries that enable interactions with the IoT Hub service to perform operations such as sending messages to devices and managing the device identity registry.

### /testtools

Contains tools that are currently used in testing the client libraries: Mocking Framework (micromock), Generic Test Runner (CTest), Unit Test Project Template, etc.

### /tools

Miscellaneous tools: compilembed, mbed_build, traceabilitytool (checks spec requirements vs code implementation).

## Installation and Use
- Clone azure-iot-sdks by:
```
git clone --recursive https://github.com/Azure/azure-iot-sdks.git
```
- Create a folder build under azure-iot-sdks
- Switch to the build folder and run
   cmake ..

Optionally, you may choose to install azure-iot-sdks on your machine:

1. Switch to the *cmake* folder and run
    ```
    cmake -Duse_installed_dependencies=ON ../
    ```
    ```
    cmake --build . --target install
    ```
    
    or install using the follow commands for each platform:

    On Linux:
    ```
    sudo make install
    ```

    On Windows:
    ```
    msbuild /m INSTALL.vcxproj
    ```

2. Use it in your project (if installed)
    ```
    find_package(azure_iot_sdks REQUIRED CONFIG)
    target_link_library(yourlib iothub_client iothub_service_client serializer)
    ```

_This requires that azure-c-shared-utility, azure-uamqp-c, and azure-umqtt-c are installed (through CMake) on your machine._

_If running tests, this requires that umock-c, azure-ctest, and azure-c-testrunnerswitcher are installed (through CMake) on your machine._
