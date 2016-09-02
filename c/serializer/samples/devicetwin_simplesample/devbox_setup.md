# Prepare your development environment

This document describes how to prepare your development environment to use the *Microsoft Azure IoT device SDK for C*. It describes preparing a development environment in Windows using Visual Studio and in Linux.

## Setting up a Windows development environment

1. Install [Visual Studio 2015][visual-studio]. You can use the free Community Edition if you meet the licensing requirements.
Be sure to include Visual C++ and NuGet Package Manager.
1. Install [git](http://www.git-scm.com) making sure git.exe can be run from a command line.
1. Clone the latest version of this repository to your local machine with the recursive parameter
   ```
   git clone -b develop --recursive https://github.com/Azure/azure-iot-dt-sdks.git
   ```
   Use the **develop** branch to ensure you fetch the latest version.
1. Install [cmake 3.6.1](http://www.cmake.org/) (make sure it is installed in your path, type "cmake -version" to verify).


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

### Building the libraries in Windows

The following instructions outline how you can build the libraries in Windows:

1. Create the folder **C:\\OpenSSL** on your development machine in which to download OpenSSL.
1. Open a Developer Command Prompt for VS2015.
1. Run the script **build_client.cmd** in the **c\\build_all\\windows** directory.

The build process will take some time (5+ minutes), but once built, you will have the client libraries all built and ready for use in the sample applications.

[visual-studio]: https://www.visualstudio.com/
[device-explorer]: ../../tools/DeviceExplorer/readme.md
[toradex-CE8-sdk]:http://docs.toradex.com/102578
[application-builder]:http://www.microsoft.com/download/details.aspx?id=38819
