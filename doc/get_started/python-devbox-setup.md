# Prepare your development environment

This document describes how to prepare your development environment to use the *Microsoft Azure IoT SDK for Python*.

- [Setup your development environment](#devenv)
- [Build the samples on Linux](#linux)
- [Build the samples on Windows using nuget packages](#windows)
- [Build the samples on Windows using cmake and boost libraries](#windows-cmake)
- [Sample applications](#samplecode)

<a name="devenv"/>
## Setup your development environment

Complete the following steps to set up your development environment:

- Follow the instructions to setup [Microsoft Azure IoT device SDK for C][setup-devbox].

- Ensure that Python version 2.7.x is installed. Run `python --version` at the command line to check the version. Python 2.7 is typically already installed on Linux. On Windows install the latest 32 bit x86 Python 2.7 client from ([python-2.7] (https://www.python.org/downloads/)).

- When you have installed Python, make sure you cloned the latest version of this repository ([azure-iot-sdks](https://github.com/Azure/azure-iot-sdks)) to your development machine or device. You should always use the **master** branch for the latest version of the libraries and samples.

<a name="linux"/>
## Build the Python iothub_client module on Ubuntu

The Python module builds on the Azure IoT device SDK for C. Please make sure you successfully built the C samples [setup-devbox] before you continue in this section.

1. Ensure that Python version 2.7.x is installed and active. Run `python --version` at the command line to check the version.

2. Open a shell and navigate to the folder **python/build_all/linux** in your local copy of the repository.

3. Run the `./setup.sh` script to install the prerequisite packages and the dependent libraries.

4. Run the `./build.sh` script.

5. After a successful build, the `iothub_client.so` Python extension module is copied to the **python/device/samples** folder. Please follow instructions in [Sample applications](#samplecode) to run the Python samples.

<a name="windows"/>
## Build the Python iothub_client module in Windows using Nuget packages

The following instructions outline how you can build the libraries in Windows:

1. Open a Visual Studio 2015 x86 Native Tools command prompt and navigate to the folder **python/build_all/windows** in your local copy of the repository.

2. Run the script `build.cmd` in the **python\\build_all\\windows** directory.

3. As a result, the `iothub_client.pyd` Python extension module is copied to the **python/device/samples** folder. Please follow instructions in [Sample applications](#samplecode) to run the Python samples.

<a name="windows-cmake"/>

## Build the Python iothub_client module in Windows using cmake and boost libraries

The following instructions outline how you can build the boost::python libraries in Windows. This step is only required once:

1. Open a Visual Studio 2015 x86 Native Tools command prompt.

2. Download the zip version of the boost 1.60 library for windows from [boost-zip]. 

3. Unpack zip to a local folder, e.g. **C:\\boost_1_60_0**.

3. Navigate to the root folder of the boost library, e.g. **C:\\boost_1_60_0**.

4. Run the script `bootstrap.bat` in the boost folder.

5. Run the build command `b2` in the same folder.

6. Set the environment variable BOOST_ROOT to the boost folder using the command `SET BOOST_ROOT=C:\\boost_1_60_0`.

Now the boost::python library is ready for use in cmake. 

The following instructions outline how you can build the Python extension module in Windows:

1. Open a Visual Studio 2015 x86 Native Tools command prompt and navigate to the folder **python/build_all/windows** in your local copy of the repository.

2. Verify that `BOOST_ROOT` points to the local boost installation.

3. Run the script `build_client.cmd` in the **python\\build_all\\windows** directory.

3. After a successful build, the `iothub_client.pyd` Python extension module is copied to the **python/device/samples** folder. Please follow instructions in [Sample applications](#samplecode) to run the Python samples.

<a name="samplecode"/>
## Sample applications

This repository contains various Python sample applications that illustrate how to use the Microsoft Azure IoT SDK for Python. To learn how to run a sample application that sends messages to an IoT hub, see [Getting started - running a Python sample application][getstarted].

[python-2.7]: https://www.python.org/downloads/
[setup-devbox]: https://github.com/Azure/azure-iot-sdks/blob/master/c/doc/devbox_setup.md
[getstarted]: python-run-sample.md
[boost-zip]: http://www.boost.org/users/history/version_1_60_0.html
