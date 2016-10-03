# Prepare your development environment

This document describes how to prepare your development environment to use the **Microsoft Azure IoT device SDK for C**. It describes preparing a development environment in Windows using Visual Studio and in Linux.

- [Setting up a Windows development environment](#windows)
- [Setting up a Linux development environment](#linux)
- [Setting up a Windows Embedded Compact 2013 development environment](#windowsce)
- [Sample applications](#samplecode)

<a name="windows"/>
## Setting up a Windows development environment

- Install [Visual Studio 2015][visual-studio]. You can use the **Visual Studio Community** Free download if you meet the licensing requirements.

Be sure to include Visual C++ and NuGet Package Manager.
- Install [git](http://www.git-scm.com) making sure git.exe can be run from a command line.

- Clone the **latest** available version of this repository (**master** branch) to your local machine with the **recursive** parameter to include submodules.
```
git clone --recursive https://github.com/Azure/azure-iot-sdks.git
```
**recursive** parameter will include other projects (**submodules**) [azure-shared-c-utility], [azure-uamqp-c], [azure-umqtt-c] that **azure-iot-sdks** project has dependence on.

C SDK source is located under **c** folder **azure-iot-sdks\\c**

[Releases of azure-iot-sdks] has list of available **releases**.

### Building the sample applications only
For **quickly** testing the sample application of your choice open the corresponding [solution (.sln) file] in VS 2015 IDE.
For example, to build **MQTT sample**, go to c\iothub_client\samples\iothub_client_sample_mqtt\windows and open **iothub_client_sample_mqtt.sln** file in VS 2015.

Build and run the application after replacing `connectionString = "[device connection string]"` with the actual valid device connection string.

**packages.config** file will have list of libraries (packages) that sample has dependence on and these will be pulled in by [NuGet] package manager while building the sample application.

### Building the C SDK libraries locally along with sample applications
In some cases, you may want to build the **C SDK libraries** locally for development and testing purpose.
For example, you may want to build the latest code available on the **develop** branch which follows Continuous Integration (CI) approach.

To pull latest code on **develop** branch you can use following command

`git clone -b develop --recursive https://github.com/Azure/azure-iot-sdks.git azure-iot-develop`

The following instructions outline how you can build the C SDK (along with samples):

1. Install [CMake] tool.Make sure it is installed in your path by typing `cmake -version` to verify. [CMake] tool will be used to create VS 2015 solution files which will then be subsequently used by [MSBuild] to build libraries and samples.

2. Open a Developer Command Prompt for VS2015.

4. Go to scripts folder located at **c\\build_all\\windows**. Run the build script **build_client.cmd** or **build.cmd** which will invoke cmake to create solution files which will then be used by msbuild to build the projects

After successful running of the script, you should see cmake folder created under c folder.
For Win32 (default configuration) you should see **c\\cmake\\iotsdk_win32**. For x64, you should see c\cmake\iotsdk_x64.

**azure_iot_sdks.sln** created under cmake folder will contain all sub-projects that you can directly open in VS 2015 IDE to build libraries or samples in IDE.

You can check various configuration options the script provides by entering `build –options`
For example, if you want to build for x64 and skip unit tests, you can run following command

`build --platform x64 --skip-unittests` 

### Building sample that uses WebSocket on Windows 
**iothub_client_sample_amqp_websockets** (AMQP over WebSocket) has dependence on [OpenSSL] libraries **ssleay32** and **libeay32**. So you need to build and install these libraries and DLL's first before you enable building of the sample that uses WebSockets. 

Below are steps to build and install OpenSSL libraries and corresponding DLL's. These steps were tested with **openssl-1.0.2j.tar.gz**

1. Download and extract the available tar.gz file from [OpenSSL Downloads] section locally on your Windows machine that has VS 2015 installed. 

2. Go over [OpenSSL Installation] and [Compilation_and_Installation] and for **additional** details and references on supported configurations, pre-requisites and build steps that you may want to look at.

3. For x86 configuration, open VS 2015 x86 **Native** Tools Command Prompt and enter below commands to set build configuration

 `perl Configure VC-WIN32 no-asm`

 `ms\do_ms`

4. For x64 configuration, open VS 2015 x64 **Native** Tools Command Prompt and enter below commands to set build configuration

 `perl Configure VC-WIN64A no-asm`

 `ms\do_win64a`

5. Build and then install the libraries at your install location by using following commands

 `nmake -f ms\ntdll.mak` : This command will build OpenSSL libraries

 `nmake -f ms\nt.mak install` : This command will copy OpenSSL libraries in OpenSSL install location

After completing the above steps make sure OpenSSL libraries and DLL's are in your OpenSSL install location. Now follow below steps to build library and sample using WebSocket

1. Open VS 2015 Developer Command Prompt and go to **\\c\\build_all\\windows**

2. Set OpenSSLDir and OPENSSL_ROOT_DIR **environment variables** to OpenSSL install location. For example, if your OpenSSL install location is **C:\usr\\local\\ssl**, you will set following

- set OpenSSLDir=C:\usr\local\ssl
- set OPENSSL_ROOT_DIR=C:\usr\local\ssl

Now enter following command to build sample using websocket

`build --use-websockets`  **OR** `build --skip-unittests --use-websockets` (in case you want to skip unit tests)

This will build C SDK libraries along with **iothub_client_sample_amqp_websockets** sample.

<a name="linux"/>
## Set up a Linux development environment

This section shows you how to set up a development environment for the Azure IoT device SDK for C on [Ubuntu]. [CMake] will create makefiles and [make] tool will use these makefiles to compile the C SDK source code using [gcc] compiler.

**Note:** This setup process requires **cmake** version **3.x** or higher and **gcc** version **4.9** or higher. 

For [CMake], verify the current version installed in your environment using the `cmake --version` command. For information about how to upgrade your version of cmake to 3.x on Ubuntu 14.04, see http://askubuntu.com/questions/610291/how-to-install-cmake-3-2-on-ubuntu-14-04.

For [gcc], verify the current version installed in your environment using the `gcc --version` command. For information about how to upgrade your version of gcc on Ubuntu 14.04, see http://askubuntu.com/questions/466651/how-do-i-use-the-latest-gcc-4-9-on-ubuntu-14-04.

Older **gcc** version **4.4.7** support has also been added and tested on 14.04.1-Ubuntu SMP. It is tested with `./build.sh --skip-unittests --use-websockets` build option.

Clone the latest version of this repository to your Ubuntu machine with the recursive parameter to fetch latest code from **master** branch

`git clone --recursive https://github.com/Azure/azure-iot-sdks.git`

> In case, you want to pull latest code on **develop** branch you can use following command
`git clone -b develop --recursive https://github.com/Azure/azure-iot-sdks.git azure-iot-develop`

1. Open a shell and navigate to the folder **c/build_all/linux** in your local copy of the repository.

2. Run the `./setup.sh` script to install the prerequisite packages and the dependent libraries.

3. Run the `./build.sh` script.

This script uses **cmake** to make a folder called "cmake/iotsdk_linux" under c folder and generates necessary makefiles.The script then builds the solution and runs the unit tests.

Below are some of the build **options** you can use

`./build.sh --options` : List available options

`./build.sh --skip-unittests`: Skip unit tests in case you want to just quickly build samples

 `./build.sh --use-websockets`:  Enables the support for AMQP over WebSockets
 
 `./build.sh --use-websockets --skip-unittests` : Skip unit tests and include AMQP over **WebSockets** sample
 
 `./build.sh -cl -g`: Build **debug** build

> Tip: After running `./build.sh` if you want to just build a **particular sample** or library after making changes, you can directly go the corresponding cmake directory and run the [make] command.
For example, to build **mqtt** sample after making changes in it you can directly go to c/cmake/iotsdk_linux/iothub_client/samples/iothub_client_sample_mqtt directory and run `make` command from that directory.


> Note: Every time you run `build.sh`, it deletes and then recreates the "cmake" folder under c folder.

> Note: You will not be able to run the samples until you configure them with a valid IoT Hub device connection string. For more information, see [Run sample on Linux](../../doc/get_started/linux-desktop-c.md).


<a name="windowsce"/>
## Set up a Windows Embedded Compact 2013 development environment

- Install [Visual Studio 2015][visual-studio]. You can use the free Community Edition if you meet the licensing requirements.
Be sure to include Visual C++ and NuGet Package Manager.
- Install [Application Builder for Windows Embedded Compact 2013][application-builder] for Visual Studio 2015
- Install [Toradex Windows Embedded Compact 2013 SDK][toradex-CE8-sdk] or your own SDK.
- Install [git](http://www.git-scm.com) making sure git.exe can be run from a command line.

- Clone the latest version of this repository to your local machine. Use the **master** branch to ensure you fetch the latest release version.

If you installed a different SDK please check azure-iot-sdks\\c\\build_all\\windowsce\\build.cmd and replace:
```
set SDKNAME=TORADEX_CE800
set PROCESSOR=arm
```
with a reference to the name of the SDK and the processor architecture (arm/x86) you plan to use.

### Verify your environment

You can build the Windows samples to verify that your environment is set up correctly.

1. Open a Visual Studio 2015 Developer command prompt.

2. Navigate to the **c\\build_all\\windowsce** folder in your local copy of the repository.

3. Run the following command:

```
build 
```

This script uses cmake to make a folder called "cmake_ce8" in your home directory and generates in that folder a Visual Studio solution called azure_iot_sdks.sln. The script will then proceed to build the **HTTP** sample.

> Note: you will not be able to run the samples until you configure them with a valid IoT hub device connection string. For more information, see [running a C sample application on Windows Embedded Compact 2013 on a Toradex module](../../doc/get_started/wince2013-toradex-module-c.md).

To view the projects and examine the source code, open the **azure_iot_sdks.sln** solution files in Visual Studio.

You can use one of the sample applications as a template to get started when you are creating your own client applications.

<a name="samplecode"/>
## Sample applications

This repository contains various C sample applications that illustrate how to use the Azure IoT device SDK for C. For more information, see the [Read Me][readme].

[visual-studio]: https://www.visualstudio.com/downloads/
[readme]: ../readme.md
[device-explorer]: ../../tools/DeviceExplorer/readme.md
[toradex-CE8-sdk]:http://docs.toradex.com/102578
[application-builder]:http://www.microsoft.com/download/details.aspx?id=38819
[azure-shared-c-utility]:https://github.com/Azure/azure-c-shared-utility
[azure-uamqp-c]:https://github.com/Azure/azure-uamqp-c
[azure-umqtt-c]:https://github.com/Azure/azure-umqtt-c
[Releases of azure-iot-sdks]:https://github.com/Azure/azure-iot-sdks/releases
[solution(.sln) file]:https://msdn.microsoft.com/en-us/library/bb165951.aspx
[NuGet]:https://www.nuget.org/
[CMake]:https://cmake.org/
[MSBuild]:https://msdn.microsoft.com/en-us/library/0k6kkbsd.aspx
[OpenSSL]:https://www.openssl.org/
[OpenSSL Downloads]: https://www.openssl.org/source/
[OpenSSL Installation]:https://github.com/openssl/openssl/blob/master/INSTALL
[Compilation_and_Installation]:https://wiki.openssl.org/index.php/Compilation_and_Installation#Windows
[Ubuntu]:http://www.ubuntu.com/desktop
[gcc]:https://gcc.gnu.org/
[make]:https://www.gnu.org/software/make/
