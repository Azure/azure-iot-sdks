# Prepare your development environment

This document describes how to prepare your development environment to use the *Microsoft Azure IoT device SDK for C*. It describes preparing a development environment in Windows using Visual Studio and in Linux.

- [Setting up a Windows development environment](#windows)
- [Setting up a Linux development environment](#linux)
- [Sample applications](#samplecode)

<a name="windows"/>
## Setting up a Windows development environment

- Install [Visual Studio 2015][visual-studio]. You can use the free Community Edition if you meet the licensing requirements.
Be sure to include Visual C++ and NuGet Package Manager.
- Install [git](http://www.git-scm.com) making sure git.exe can be run from a command line.

- Clone the latest version of this repository to your local machine. Use the **master** branch to ensure you fetch the latest release version.

### Preparing Qpid Proton libraries in Windows

The AMQP samples require the Qpid Proton-C library.  There are two alternative ways to obtain the library-- via NuGet (recommended) or by downloading the source from GitHub and building it yourself.

#### Option 1: Getting Qpid Proton via NuGet

 1. In Visual Studio 2015 right click on your solution and select "Manage NuGet Package for Solutions".

 2. Click the "Include prerelease" check box.

 3. Enter Apache.Qpid.Proton.AzureIot in the Search box (Ctrl+E) to find the Proton NuGet Package.

 4. Select the Apache.Qpid.Proton.AzureIot package and on the right pane click the Install button.

 5. Enter OK at the dialog and Accept the license terms to continue.

#### Option 2: Building the Qpid Proton library

If you prefer to build the Qpid libraries yourself, follow these steps:

1. Create a folder on your development machine in which to download the proton libraries. This example uses the location **C:\Proton**.

2. Create an environment variable **PROTON_PATH=C:\Proton** that points to the folder.

3. Install [cmake](http://www.cmake.org/) (make sure it is installed in your path, type "cmake -version" to verify).

4. Install  [Python ver. 2.7.9](https://www.python.org/downloads/) (make sure it is installed in your path, type "python -V" to verify).

5. Ensure that the git.exe application is in your system path.

6. Open a Visual Studio 2015 x86 Native Tools command prompt.

7. Run the script **build_proton.cmd** in the **\build_all\windows** directory to build the proton libraries. You need to complete this step before continuing to the next section.

### Building the Apache MQTT library in Windows

To build the MQTT-based samples you must install the Paho open source library.

1. Create the environment variables **OpenSSLDir=c:\OpenSSL** and **PAHO_PATH=c:\Paho**.

2. Install [Perl ver. 5.20.2 or higher](https://www.perl.org/get.html) (make sure it is installed in your path, type "perl -v" to verify).

3. Open a Visual Studio 2015 x86 Native Tools command prompt.

4. Run the script **build_paho.cmd** in the **c\build_all\windows** directory.

5. In order to test MQTT End2End you must also deploy an [Azure IoT Protocol Gateway](https://github.com/Azure/azure-iot-protocol-gateway/blob/master/README.md).

### Verify your environment

> Note: If you want to complete this verification task, the **PROTON_PATH** environment variable must be set, even if you are installing the Qpid Proton libraries using NuGet.

You can build the Windows samples to verify that your environment is set up correctly.

1. Open a Visual Studio 2015 x86 Native Tools command prompt.

2. Navigate to the **c\\build_all\\windows** folder in your local copy of the repository.

3. Run the following command:

  ```
  build.cmd --skip-e2e-tests
  ```

This script uses cmake to make a folder called "cmake" in your home directory and generates in that folder a Visual Studio solution called azure_iot_sdks.sln. The script will then proceed to build the solution and run tests.

  > Note: you will not be able to run the samples until you configure them with a valid IoT hub device connection string. For more information, see [Run sample on Windows](run_sample_on_desktop_windows.md).

To view the projects and examine the source code, open the **azure_iot_sdks.sln** solution files in Visual Studio.

You can use one of the sample applications as a template to get started when you are creating your own client applications.

  > Note: the projects in this solution always use the Qpid Proton libraries referenced by the **PROTON_PATH** environment variable.

<a name="linux"/>
## Set up a Linux development environment

This section shows you how to set up a development environment for the Azure IoT device SDK for C on Ubuntu.

**Note:** this setup process requires **cmake** version 3.0 or higher. You can verify the current version installed in your environment using the `cmake --version` command. For information about how to upgrade your version of cmake to 3.2 on Ubuntu 14.04, see http://askubuntu.com/questions/610291/how-to-install-cmake-3-2-on-ubuntu-14-04.

1. Clone this repository ([azure-iot-sdks](https://github.com/Azure/azure-iot-sdks)) to the Ubuntu machine you are using.

2. Open a shell and navigate to the folder **c/build_all/linux** in your local copy of the repository.

3. Run the `/setup.sh` script to install the prerequisite packages and the dependent libraries.

4. Run the `./build.sh` script.

This script uses **cmake** to make a folder called "cmake" in your home directory and generates a makefile. The script then builds the solution and runs the tests.

 > Note: you will not be able to run the samples until you configure them with a valid IoT Hub device connection string. For more information, see [Run sample on Linux](run_sample_on_desktop_linux.md).

<a name="samplecode"/>
## Sample applications

This repository contains various C sample applications that illustrate how to use the Azure IoT device SDK for C. For more information, see the [Read Me][readme].


[visual-studio]: https://www.visualstudio.com/
[readme]: ../readme.md
[device-explorer]: ../../tools/DeviceExplorer/doc/how_to_use_device_explorer.md
