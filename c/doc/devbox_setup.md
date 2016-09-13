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

	&nbsp;&nbsp;&nbsp;&nbsp;&nbsp;&nbsp;\\\-\-\-\-\-\- c
  
	&nbsp;&nbsp;&nbsp;&nbsp;&nbsp;&nbsp;&nbsp;&nbsp;&nbsp;&nbsp;&nbsp;&nbsp;&nbsp;&nbsp;&nbsp;&nbsp;\|\-\-\-\-\-\- azure-c-shared-utility
	
	&nbsp;&nbsp;&nbsp;&nbsp;&nbsp;&nbsp;&nbsp;&nbsp;&nbsp;&nbsp;&nbsp;&nbsp;&nbsp;&nbsp;&nbsp;&nbsp;\|\-\-\-\-\-\- azure-uamqp-c
	
	&nbsp;&nbsp;&nbsp;&nbsp;&nbsp;&nbsp;&nbsp;&nbsp;&nbsp;&nbsp;&nbsp;&nbsp;&nbsp;&nbsp;&nbsp;&nbsp;\|&nbsp;&nbsp;&nbsp;&nbsp;&nbsp;&nbsp;&nbsp;&nbsp;\|\-\-\-\-\-\- azure-c-shared-utility
	
	&nbsp;&nbsp;&nbsp;&nbsp;&nbsp;&nbsp;&nbsp;&nbsp;&nbsp;&nbsp;&nbsp;&nbsp;&nbsp;&nbsp;&nbsp;&nbsp;\|&nbsp;&nbsp;&nbsp;&nbsp;&nbsp;&nbsp;&nbsp;&nbsp;\\\-\-\-\-\-\- libwebsockets
	
	&nbsp;&nbsp;&nbsp;&nbsp;&nbsp;&nbsp;&nbsp;&nbsp;&nbsp;&nbsp;&nbsp;&nbsp;&nbsp;&nbsp;&nbsp;&nbsp;\\\-\-\-\-\-\- azure-umqtt-c
	
	&nbsp;&nbsp;&nbsp;&nbsp;&nbsp;&nbsp;&nbsp;&nbsp;&nbsp;&nbsp;&nbsp;&nbsp;&nbsp;&nbsp;&nbsp;&nbsp;&nbsp;&nbsp;&nbsp;&nbsp;&nbsp;&nbsp;&nbsp;&nbsp;&nbsp;&nbsp;\\\-\-\-\-\-\- azure-c-shared-utility
 
 2.  If you have a previous version of the repository you will need to run the following commands (under each submodule):
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

1. Create the folder **C:\\OpenSSL** on your development machine in which to download OpenSSL.

2. Install [cmake](http://www.cmake.org/) (make sure it is installed in your path, type "cmake -version" to verify).

3. Ensure that the git.exe application is in your system path.

4. Open a Developer Command Prompt for VS2015.

5. Run the script **build_client.cmd** in the **c\\build_all\\windows** directory.


> Note: To enable support to AMQP over WebSockets, 
  1. Install [OpenSSL 1.0.1 (x86)](https://github.com/openssl/openssl) (tip: build the dll prior to running the following steps); 
  2. Additionally to instructions on **step 1 above**, create the environment variable **OPENSSL_ROOT_DIR=C:\\OpenSSL** and  **OpenSSLDir=C:\\OpenSSL**
  3. Run the **build\_client.cmd** script with the option ```--use-websockets```.

<a name="linux"/>
## Set up a Linux development environment

This section shows you how to set up a development environment for the Azure IoT device SDK for C on Ubuntu.

**Note:** This setup process requires **cmake** version 3.x or higher and **gcc** version 4.9 or higher. You can verify the current version installed in your environment using the `cmake --version` command. For information about how to upgrade your version of cmake to 3.x on Ubuntu 14.04, see http://askubuntu.com/questions/610291/how-to-install-cmake-3-2-on-ubuntu-14-04.
This library requires **gcc** version 4.9. You can verify the current version installed in your environment using the `gcc --version` command. For information about how to upgrade your version of gcc on Ubuntu 14.04, see http://askubuntu.com/questions/466651/how-do-i-use-the-latest-gcc-4-9-on-ubuntu-14-04.

**New note:** Older **gcc** version 4.4.7 support has been added and tested on 14.04.1-Ubuntu SMP. It is tested with ```--skip-unittests``` build option.

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
