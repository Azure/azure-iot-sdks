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

- Clone the latest version of this repository to your local machine with the recursive parameter
```
git clone --recursive https://github.com/Azure/azure-iot-sdks.git
```
Use the **master** branch to ensure you fetch the latest release version.

### Preparing Azure Shared Utility

 1. Cloning the repository will bring in the Azure-C-Shared-Utility dependency and give you the following directory structure: 

	\| ...

	\|\-\-\- azure-iot-sdk

	\|\-\-\-\-\-\- c
  
	\|\-\-\-\-\-\-\-\-\- azure-c-shared-utility
 
 2.  If you have a previous version of the repository you will need to run the following commands:
```
    git submodule init
    git submodule update
```   

### Running the Windows sample applications

You can now run the Windows sample applications included in the repository. All the samples use the NuGet package manager to download any required dependencies such as the **Microsoft.Azure.C.SharedUtility**, **Microsoft.Azure.IoTHubClient**, and **Microsoft.Azure.IoTHubSerializer** libraries.

The following is a list of the Visual Studio solution files for the Windows samples in the repository:

- c\\iothub\_client\\samples\\iothub\_client\_sample\_amqp\\windows\\iothub\_client\_sample\_amqp.sln
- c\\iothub\_client\\samples\\iothub\_client\_sample\_http\\windows\\iothub\_client\_sample\_http.sln

- c\\serializer\\samples\\remote\_monitoring\\windows\\remote\_monitoring.sln

- c\\serializer\\samples\\simplesample\_amqp\\windows\\simplesample\_amqp.sln

- c\\serializer\\samples\\simplesample\_http\\windows\\simplesample\_http.sln

- c\\serializer\\samples\\simplesample\_mqtt\\windows\\simplesample\_mqtt.sln

- c\\serializer\\samples\\temp\_sensor\_anomaly\\windows\\temp\_sensor\_anomaly.sln
 

### Building the libraries in Windows

You do *not* need to build the libraries in order to run the sample applications because the samples install all their dependencies using the NuGet package manager.

The following instructions outline how you can build the libraries in Windows:

1. Create the folders **C:\\Proton** and **C:\\OpenSSL** on your development machine in which to download the dependencies.

2. Create the environment variables **PROTON\_PATH=C:\\Proton** and **OpenSSLDir=C:\\OpenSSL**..

3. Install [cmake](http://www.cmake.org/) (make sure it is installed in your path, type "cmake -version" to verify).

4. Install  [Python ver. 2.7.9](https://www.python.org/downloads/) (make sure it is installed in your path, type "python -V" to verify).

5. Install [Perl ver. 5.20.2 or higher](https://www.perl.org/get.html) (make sure it is installed in your path, type "perl -v" to verify).

6. Ensure that the git.exe application is in your system path.

6. Open a Visual Studio 2015 x86 Native Tools command prompt.

7. Run the script **build\_proton.cmd** in the **c\\build_all\\windows** directory to build the proton libraries.

8. Run the script **build\_client.cmd** in the **c\\build_all\\windows** directory.

> Note: In order to test MQTT End2End you must also deploy an [Azure IoT Protocol Gateway](https://github.com/Azure/azure-iot-protocol-gateway/blob/master/README.md).

<a name="linux"/>
## Set up a Linux development environment

This section shows you how to set up a development environment for the Azure IoT device SDK for C on Ubuntu.

**Note:** this setup process requires **cmake** version 3.x or higher and **gcc** version 4.9 or higher. You can verify the current version installed in your environment using the `cmake --version` command. For information about how to upgrade your version of cmake to 3.x on Ubuntu 14.04, see http://askubuntu.com/questions/610291/how-to-install-cmake-3-2-on-ubuntu-14-04.

this library requires **gcc** version 4.9 or higher. You can verify the current version installed in your environment using the `gcc --version` command. For information about how to upgrade your version of gcc on Ubuntu 14.04, see http://askubuntu.com/questions/466651/how-do-i-use-the-latest-gcc-4-9-on-ubuntu-14-04.

1. Clone the latest version of this repository to your Ubuntu machine with the recursive parameter
```
git clone --recursive https://github.com/Azure/azure-iot-sdks.git
```
Use the **master** branch to ensure you fetch the latest release version.

2. Open a shell and navigate to the folder **c/build_all/linux** in your local copy of the repository.

3. Run the `/setup.sh` script to install the prerequisite packages and the dependent libraries.

4. Run the `./build.sh` script.

This script uses **cmake** to make a folder called "cmake" in your home directory and generates a makefile. The script then builds the solution and runs the tests.

> Note: you will not be able to run the samples until you configure them with a valid IoT Hub device connection string. For more information, see [Run sample on Linux](run_sample_on_desktop_linux.md).

<a name="windowsce"/>
## Set up a Windows Embedded Compact 2013 development environment

- Install [Visual Studio 2013][visual-studio]. You can use the free Community Edition if you meet the licensing requirements.
Be sure to include Visual C++ and NuGet Package Manager.
- Install [Application Builder for Windows Embedded Compact 2013][application-builder]
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

1. Open a Visual Studio 2013 Developer command prompt.

2. Navigate to the **c\\build_all\\windowsce** folder in your local copy of the repository.

3. Run the following command:

```
build.cmd --skip-e2e-tests
```

This script uses cmake to make a folder called "cmake_ce8" in your home directory and generates in that folder a Visual Studio solution called azure_iot_sdks.sln. The script will then proceed to build the solution and run tests.

> Note: you will not be able to run the samples until you configure them with a valid IoT hub device connection string. For more information, see [running a C sample application on Windows Embedded Compact 2013 on a Toradex module](run_sample_on_toradex_wec2013.md).

To view the projects and examine the source code, open the **azure_iot_sdks.sln** solution files in Visual Studio.

You can use one of the sample applications as a template to get started when you are creating your own client applications.

<a name="samplecode"/>
## Sample applications

This repository contains various C sample applications that illustrate how to use the Azure IoT device SDK for C. For more information, see the [Read Me][readme].

[visual-studio]: https://www.visualstudio.com/
[readme]: ../readme.md
[device-explorer]: ../../tools/DeviceExplorer/doc/how_to_use_device_explorer.md
[toradex-CE8-sdk]:http://docs.toradex.com/102578
[application-builder]:http://www.microsoft.com/en-us/download/details.aspx?id=38819
