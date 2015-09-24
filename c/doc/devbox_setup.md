# Prepare your development environment

This document describes how to prepare your development environment to use the Microsoft Azure IoT device SDK for C. It describes preparing a development environment in Windows using Visual Studio and in Linux.

- [Set up a Windows development environment](#windows)
- [Set up a Linux development environment](#linux)
- [Sample applications](#samplecode)

<a name="windows"/>
## Setting up a Windows development environment

- Install [Visual Studio 2015][visual-studio]. You can use the free Community edition if you meet the requirements.
Be sure to include Visual C++ and NuGet Package Manager.

- Clone the latest version of this repository to your local machine. Use the **master** branch to ensure you fetch the latest released version.

### Preparing QPID Proton libraries in Windows

The AMQP samples require the QPID Proton-C library.  There are two ways to obtain the library-- via Nuget or by downloading the source from GitHub and building it yourself.

#### Getting QPID Proton via Nuget

 1. In Visual Studio 2015 right click on the iothub_client solution and select "Manage NuGet Package for Solutions"

 2. Enter Apache.QPID.Proton.AzureIot in the Search box (Ctrl+E) to find the Proton NuGet Package.

 3. Select the Apache.QPID.Proton.AzureIot package and on the right pane click the Install button.

 4. Enter OK at the dialog and Accept the license terms to continue.

#### Building the QPID Proton library

1. Create a folder on your development machine in which to download the proton libraries. This example uses the location **C:\Proton**.

2. Create an environment variable **PROTON_PATH=C:\Proton** that points to the folder.

3. Install [cmake](http://www.cmake.org/) (make sure it is installed in your path).

4. Install  [Python ver. 2.7.9](https://www.python.org/downloads/) (make sure it is installed in your path).

5. Open a Visual Studio 2015 x86 Native Tools command prompt.

6. Run the script **build_proton.cmd** in the **\build_all\windows** directory.


### Building the Apache MQTT library in Windows
To build the MQTT-based samples you must install the Paho open source library.

1. Create the environment variables **OpenSSLDir=c:\OpenSSL** and **PAHO_PATH=c:\Paho**.

2. Ensure that [Perl ver. 5.20.2 or higher](https://www.perl.org/get.html) is installed.

3. Open a Visual Studio 2015 x86 Native Tools command prompt.

4. Run the script **build_paho.cmd** in the **\build_all\windows** directory.

5. In order to test MQTT End2End you must also deploy an [Azure IoT Protocol Gateway](https://github.com/Azure/azure-iot-protocol-gateway/blob/master/README.md).

### Verify your environment

You can build the Windows samples to verify that your environment is setup correctly.

1. Open a Visual Studio 2015 x86 Native Tools command prompt.

2. Navigate to the **c\\build_all\\windows** folder in your local copy of the repository.

3. Run the `build.cmd` script.

This script builds the **iothub_client** and **serializer** libraries and their associated samples.

**Note:** you will not be able to run the samples until you configure them with a valid IoT hub device connection string. For more information, see [Run sample on Windows](run_sample_on_Windows.md).

To view the projects and examine the source code, open the **c\\iothubclient\\build\\windows\\iothub_client.sln** or **c\\serializer\\build\\windows\\serializer.sln** solution files.

You can use one of the sample applications as a template to get started when you are creating your own client applications.

**Note:** some of the projects in these solutions use the **Apache.QPID.Proton.AzureIot** NuGet package to reference the QPID Proton libraries, some use the QPID Proton libraries referenced by the **PROTON_PATH** environment variable.

<a name="linux"/>
## Set up a Linux development environment

This section shows you how to set up a development environment for the Azure IoT device SDK for C on Ubuntu.

1. Clone this repository to the Ubuntu machine you are using.

2. Open a shell and navigate to the folder **c/build_all/linux** in your local copy of the repository.

3. Execute the `setup.sh` script to install the prerequisite packages and the dependant libraries.

4. Run the `build.sh` script.

This script builds the **iothub_client** and **serializer** libraries and their associated samples.

**Note:** you will not be able to run the samples until you configure them with a valid IoT Hub device connection string. For more information, see [Run sample on Linux](run_sample_on_desktop_linux.md).

<a name="samplecode"/>
## Sample applications

This repository contains various C sample applications that illustrate how to use the Azure IoT device SDK for C. For more information, see the [ReadMe][readme].


[visual-studio]: https://www.visualstudio.com/
[readme]: ../readme.md
[device-explorer]: ../../tools/DeviceExplorer/doc/how_to_use_device_explorer.md
