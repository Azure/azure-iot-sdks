# Prepare your development environment

This document describes how to prepare your development environment to use the *Microsoft Azure IoT device SDK preview for C*.
It describes preparing a development environment in Windows using Visual Studio and in Linux.

## Setting up a Windows development environment

This section shows you how to set up a development environment for the Azure IoT device SDK for C on Windows.

1. Install [Visual Studio 2015][visual-studio]. You can use the free Community Edition if you meet the licensing requirements.
Be sure to include Visual C++ and NuGet Package Manager.
1. Install [git](http://www.git-scm.com) making sure git.exe can be run from a command line.
1. Clone the latest version of this repository to your local machine with the recursive parameter
   ```
   git clone --recursive https://github.com/Azure/azure-iot-dt-sdks-preview.git
   ```
   Use the **master** branch to ensure you fetch the latest release version and use the **develop** branch to fetch the latest code.
1. Install [cmake 3.6.1](http://www.cmake.org/) (make sure it is installed in your path, type "cmake -version" to verify).
1. Open a Developer Command Prompt for VS2015.
1. Run the script **build_client.cmd** in the **c\\build_all\\windows** directory.

The build process will take some time (5+ minutes), but once built, you will have the client libraries all built and ready for use in the sample applications.

## Set up a Linux development environment

This section shows you how to set up a development environment for the Azure IoT device SDK for C on Ubuntu.

**Note:** This setup process requires **cmake** version 3.x or higher and **gcc** version 4.9 or higher. You can verify the current version installed in your environment using the `cmake --version` command. For information about how to upgrade your version of cmake to 3.x on Ubuntu 14.04, see http://askubuntu.com/questions/610291/how-to-install-cmake-3-2-on-ubuntu-14-04.
This library requires **gcc** version 4.9. You can verify the current version installed in your environment using the `gcc --version` command. For information about how to upgrade your version of gcc on Ubuntu 14.04, see http://askubuntu.com/questions/466651/how-do-i-use-the-latest-gcc-4-9-on-ubuntu-14-04.

**New note:** Older **gcc** version 4.4.7 support has been added and tested on 14.04.1-Ubuntu SMP. It is tested with ```--skip-unittests --use-websockets``` build option.

1. Clone the latest version of this repository to your Ubuntu machine with the recursive parameter
   ```
   git clone --recursive https://github.com/Azure/azure-iot-sdks-preview.git
   ```
   Use the **master** branch to ensure you fetch the latest release version and use the **develop** branch to fetch the latest code.

2. Open a shell and navigate to the folder **c/build_all/linux** in your local copy of the repository.

3. Run the `./setup.sh` script to install the prerequisite packages and the dependent libraries.

4. Run the `./build.sh` script.

This script uses **cmake** to make a folder called "cmake" in your home directory and generates a makefile. The script then builds the solution and runs the tests.

> Note: Every time you run `build.sh`, it deletes and then recreates the "cmake" folder in your home directory.


[visual-studio]: https://www.visualstudio.com/
