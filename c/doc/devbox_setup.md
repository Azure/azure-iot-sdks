# Prepare your development environment

This document describes how to prepare your development environment to use the *Microsoft Azure IoT device SDK for C*. It describes preparing a development environment in Windows using Visual Studio and in Linux.

- [Setup a Windows development environment](#windows)
- [Setup a Linux development environment](#linux)
- [Sample applications](#samplecode)

<a name="windows"/>
## Setup a Windows development environment

This section shows you how to use Visual Studio to build an IoT Hubs client application to run on Windows.

- Install [Visual Studio 2015][visual-studio]. You should ensure that your Visual Studio installation includes Visual C++ 2015 and NuGet Package Manager.

- Install [git](www.git-scm.com) making sure git.exe can be run from a command line.

- Install [nuget](www.nuget.org). nuget.exe must be in your PATH so it is executable from any location.

- Clone the latest version of this repository to your local machine. Use the **master** branch to ensure you fetch the latest release version.

## Preparing QPID Proton libraries in Windows

To use the AMQP samples provided your machine must have the QPID Proton-C library.  There are two ways to use the obtain the library, use a Nuget package or build the library from git.

### Using QPID Proton Nuget package in Windows

1. Within Visual Studio 2015 right click on the IoTHub_client solution and select "Manage NuGet Package for Solutions"

2. Enter Apache.QPID.Proton.AzureIot in the Search box (Ctrl+E) to find the Proton NuGet Package.

3. Select the Apache.QPID.Proton.AzureIot package and on the right pane click the Install button.

4. Enter Ok at the dialog and Accept the license terms to continue.

### Rebuilding the QPID Proton library in Windows

1. Create a folder on your development machine in which to download the proton libraries. This example uses the location **C:\Proton**.

2. Create an environment variable **PROTON_PATH=C:\Proton** that points to the folder.

3. Install [cmake](http://www.cmake.org/) (make sure it is installed in your path, type "cmake -version" to verify).

4. Install  [Python ver. 2.7.9](https://www.python.org/downloads/) (make sure it is installed in your path, type "python -V" to verify).

5. Open a Visual Studio 2015 x86 Native Tools command prompt.

6. Run the script **build_proton.cmd** in the **\build_all\windows** directory to build the proton libraries. You need to complete this step before continuing to the next section.

### Building the Apache MQTT library in Windows

1. Create the environment variables **OpenSSLDir=c:\OpenSSL** and **PAHO_PATH=c:\Paho**.

2. Install [Perl ver. 5.20.2 or higher](https://www.perl.org/get.html) (make sure it is installed in your path, type "perl -v" to verify).

3. Open a Command Prompt.

4. To build the MQTT library, run the script **build_paho.cmd** in the **\build_all\windows** directory.

5. In order to test MQTT End2End you have to make sure to deploy a [Protocol Gateway](https://github.com/Azure/azure-iot-protocol-gateway/blob/master/README.md).

### Verify your environment

You can build the Windows samples to verify that your environment is setup correctly.

1. Open a Visual Studio developer command prompt.

2. Navigate to the **c\\build_all\\windows** folder in your local copy of the repository.

3. Run the `build.cmd` script.

This script builds the **iothub_client** and **serializer** libraries and their associated samples. It also runs the test suite.

**Note:** You will not be able to run the samples until you configure them with a valid IoT Hub device connection string. For more information, see [Run sample on Windows](run_sample_on_Windows.md) or [Run sample on desktop Linux](run_sample_on_desktop_linux.md).

To view the projects and examine the source code, open the **c\\iothubclient\\build\\windows\\iothub_client.sln** or **c\\serializer\\build\\windows\\serializer.sln** solution files.

You should use one of the sample applications as a template to get started when you are creating your own client applications.

**Note:** some of the projects in these solutions use the **Apache.QPID.Proton.AzureIot** NuGet package to reference the QPID Proton libraries, some use the QPID Proton libraries referenced by the **PROTON_PATH** environment variable.

<a name="linux"/>
## Setup a Linux development environment

This section shows you how to setup a development environment for the *Microsoft Azure IoT device SDK for C* on a clean installation of Ubuntu Linux.

1. Clone this repository to the Ubuntu machine you are using.

2. Open a shell and navigate to the folder **c/build_all/linux** in your local copy of the repository.

3. Execute the `setup.sh` script to install the prerequisite packages and the dependant libraries.

4. Run the `build.sh` script.

This script builds the **iothub_client** and **serializer** libraries and their associated samples.

**Note:** you will not be able to run the samples until you configure them with a valid IoT Hub device connection string. For more information, see [Run sample on Linux](run_sample_on_desktop_linux.md).

<a name="samplecode"/>
## Sample applications

This repository contains various C sample applications that illustrate how to use the Microsoft Azure IoT device SDK for C. For more information, see the [readme][readme].

To run the sample applications and send device-to-cloud messages to IoT Hub, and receive cloud-to-device messages from IoT Hub, use the [device explorer tool](../../tools/DeviceExplorer/doc/how_to_use_device_explorer.md) utility. This utility only runs on Windows.

[visual-studio]: https://www.visualstudio.com/
[readme]: ../readme.md
[device-explorer]: ../../tools/DeviceExplorer/doc/how_to_use_device_explorer.md
