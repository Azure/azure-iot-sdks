# Running iotdm\_simple\_sample on Windows

This document describes how to prepare your Visual Studio development environment to use with the *Microsoft Azure IoT Hub device management library for C* and run the iotdm\_simple\_sample application on Windows devices.

## Prerequisites

You should have the following items ready before beginning the process:

-   [Setup your IoT hub]

-   [Provision your device and get its credentials]

## Building and running iotdm\_simple\_sample on Windows

1.  Install Visual Studio 2015. You can use the free Community Edition if you meet the licensing requirements. Be sure to include Visual C++.

2.  Install [cmake]. Use version 2.8 or above. Verify that **cmake **is properly installed and in your path by typing:

  ```
  cmake -version
  ```

3.  Search for the **VS2015 x86 Native Tools command prompt** app and run it. A command prompt will open. Use this command prompt.

4.  Install [git] making sure `git.exe` can be run from a command line.

5.  Clone the latest version of the IoT Hub SDKs repository to your local machine with the recursive parameter:

  ```
  git clone --recursive --branch dmpreview  https://github.com/Azure/azure-iot-sdks.git
  ```

6.  Type the following commands:

  ```
  mkdir cmake
  cd cmake
  cmake ..\azure-iot-sdks\c
  ```

7.  To build iotdm\_simple\_sample type the following command:

  ```
  msbuild /m "/p:Configuration=Release;Platform=Win32" iotdm_client\samples\iotdm_simple_sample\iotdm_simple_sample.vcxproj
  ```

  Alternatively, you can open the above project in Visual Studio and build the application using the VS IDE.

8.  You need to provide iotdm\_simple\_sample with a valid device connection string to connect to IoT Hub.

  You can pass a device connection string as a command line parameter to the program in the following manner:

  ```
  iotdm_client\samples\iotdm_simple_sample\Release\iotdm_simple_sample.exe "[device connection string]"
  ```

  Or if you are using the Visual Studio IDE go to the Solution Explorer in VS and in the tree view navigate to iotdm\_simple\_sample Source Files iotdm\_simple\_sample.c

  Locate the following code in the file:

  ```
  static const char* connectionString = "[device connection string]";
  ```

  Insert your device connection string between the double quotes.

9.  When the iotdm\_simple\_sample runs, the device is ready to receive and process requests from the IoT Hub service. Notice that when the device connects to IoT Hub, the service will automatically start to observe resources on the device. The device libraries will invoke the device callbacks to retrieve the latest values from the device and provide them to the service.

You can use the **iotdm\_simple\_sample** application as a starting point for creating devices that can be managed by Azure IoT Hub service.

[Setup your IoT hub]: https://github.com/Azure/azure-iot-sdks/blob/master/doc/setup_iothub.md
[Provision your device and get its credentials]: https://github.com/Azure/azure-iot-sdks/blob/master/doc/manage_iot_hub.md
[cmake]: http://www.cmake.org/
[git]: http://www.git-scm.com/
