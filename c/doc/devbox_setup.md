# Prepare your development environment

This document describes how to prepare your development environment to use the *Microsoft Azure IoT device SDK for C*. It describes preparing a development environment in Windows using Visual Studio and in Linux.

- [Setting up a Windows development environment](#windows)
- [Setting up a Linux development environment](#linux)
- [Setting up a Windows Embedded Compact 2013 development environment](#windowsce)
- [Sample applications](#samplecode)

<a name="windows"/>
## Setting up a Windows development environment

- Install [Visual Studio 2015][visual-studio]. You can use the free Community Edition if you meet the licensing requirements.
Be sure to include Visual C++ and NuGet Package Manager.
- Install [git](http://www.git-scm.com) making sure git.exe can be run from a command line.

- Clone the latest available version of this repository (**master branch**) to your local machine with the **recursive** parameter
```
git clone --recursive https://github.com/Azure/azure-iot-sdks.git
```
-recursive parameter will include sub-projects [azure-shared-c-utility],[azure-uamqp-c], [azure-umqtt-c] that **azure-iot-sdks** project has dependence on.

C SDK is located under **c** folder azure-iot-sdks\c

For list of available **releases** check: [List of releases of azure-iot-sdks]

### Building the sample applications only
For quickly testing the sample application of your choice open the corresponding [solution(.sln) file] in VS 2015
For example, to build **MQTT sample**, go to c\iothub_client\samples\iothub_client_sample_mqtt\windows and open **iothub_client_sample_mqtt.sln** file in VS 2015.

Build and run the application after replacing `connectionString = "[device connection string]"` with the actual valid device connection string.

**packages.config** file will have list of libraries (packages) that sample has dependence on and these will be pulled in by [NuGet] package manager while building the sample application.

### Building the C libraries locally along with sample applications
In some cases, you may want to build the **C libraries** locally for development and testing purpose.
For example, you may want to build the latest code on the **develop** branch which has not yet merged into the master branch from which the releases are taken.

To pull latest code on **develop** branch you can use following command

`git clone -b develop --recursive https://github.com/Azure/azure-iot-sdks.git azure-iot-develop`

The following instructions outline how you can build the C SDK (along with samples):

1. Install [CMake] tool.Make sure it is installed in your path by typing `cmake -version` to verify. [CMake] tool will be used to create VS 2015 solution files which will then be subsequently used by [MSBuild] to build libraries and samples.

2. Ensure that the git.exe application is in your system path.

3. Open a Developer Command Prompt for VS2015.

4. Go to scripts folder located at **c\\build_all\\windows**. Run the build script **build_client.cmd** or **build.cmd** which will invoke cmake to create solution files which will then be used by msbuild to build the projects

After successful running of the script, you should see cmake folder created under c folder.
You should see **c\\cmake\\iotsdk_win32** folder for buiding for Win32 (default configuration). For x64, you should see c\cmake\iotsdk_x64.

**azure_iot_sdks.sln** created under cmake folder will contain all sub-projects that you can directly open in VS 2015 IDE to build libraries or samples in IDE.

You can check various configuration options the script provides by entering `build –options`
For example, if you want to build for x64 and skip unit tests, you can run following command

`build.cmd --platform x64 --skip-unittests` 


> Note: To enable support to AMQP over WebSockets, 
  1. Install [OpenSSL 1.0.1 (x86)](https://github.com/openssl/openssl) (tip: build the dll prior to running the following steps); 
  2. Additionally to instructions on **step 1 above**, create the environment variable **OPENSSL_ROOT_DIR=C:\\OpenSSL** and  **OpenSSLDir=C:\\OpenSSL**
  3. Run the **build\_client.cmd** script with the option ```--use-websockets```.

<a name="linux"/>
## Set up a Linux development environment

This section shows you how to set up a development environment for the Azure IoT device SDK for C on Ubuntu.

**Note:** This setup process requires **cmake** version 3.x or higher and **gcc** version 4.9 or higher. You can verify the current version installed in your environment using the `cmake --version` command. For information about how to upgrade your version of cmake to 3.x on Ubuntu 14.04, see http://askubuntu.com/questions/610291/how-to-install-cmake-3-2-on-ubuntu-14-04.
This library requires **gcc** version 4.9. You can verify the current version installed in your environment using the `gcc --version` command. For information about how to upgrade your version of gcc on Ubuntu 14.04, see http://askubuntu.com/questions/466651/how-do-i-use-the-latest-gcc-4-9-on-ubuntu-14-04.

**New note:** Older **gcc** version 4.4.7 support has been added and tested on 14.04.1-Ubuntu SMP. It is tested with ```--skip-unittests --use-websockets``` build option.

1. Clone the latest version of this repository to your Ubuntu machine with the recursive parameter
```
git clone --recursive https://github.com/Azure/azure-iot-sdks.git
```
Use the **master** branch to ensure you fetch the latest release version.

2. Open a shell and navigate to the folder **c/build_all/linux** in your local copy of the repository.

3. Run the `./setup.sh` script to install the prerequisite packages and the dependent libraries.

4. Run the `./build.sh` script.

This script uses **cmake** to make a folder called "cmake" in your home directory and generates a makefile. The script then builds the solution and runs the tests.

> Note: Every time you run `build.sh`, it deletes and then recreates the "cmake" folder in your home directory.

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
build.cmd
```

This script uses cmake to make a folder called "cmake_ce8" in your home directory and generates in that folder a Visual Studio solution called azure_iot_sdks.sln. The script will then proceed to build the HTTP sample.

> Note: you will not be able to run the samples until you configure them with a valid IoT hub device connection string. For more information, see [running a C sample application on Windows Embedded Compact 2013 on a Toradex module](../../doc/get_started/wince2013-toradex-module-c.md).

To view the projects and examine the source code, open the **azure_iot_sdks.sln** solution files in Visual Studio.

You can use one of the sample applications as a template to get started when you are creating your own client applications.

<a name="samplecode"/>
## Sample applications

This repository contains various C sample applications that illustrate how to use the Azure IoT device SDK for C. For more information, see the [Read Me][readme].

[visual-studio]: https://www.visualstudio.com/
[readme]: ../readme.md
[device-explorer]: ../../tools/DeviceExplorer/readme.md
[toradex-CE8-sdk]:http://docs.toradex.com/102578
[application-builder]:http://www.microsoft.com/download/details.aspx?id=38819
[azure-shared-c-utility]:https://github.com/Azure/azure-c-shared-utility
[azure-uamqp-c]:https://github.com/Azure/azure-uamqp-c
[azure-umqtt-c]:https://github.com/Azure/azure-umqtt-c
[List of releases of azure-iot-sdks]:https://github.com/Azure/azure-iot-sdks/releases
[solution(.sln) file]:https://msdn.microsoft.com/en-us/library/bb165951.aspx
[NuGet]:https://www.nuget.org/
[CMake]:https://cmake.org/
[MSBuild]:https://msdn.microsoft.com/en-us/library/0k6kkbsd.aspx
