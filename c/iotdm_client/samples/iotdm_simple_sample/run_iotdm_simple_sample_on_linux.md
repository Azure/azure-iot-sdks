# Running iotdm\_simple\_sample on Linux

This document describes how to prepare your Linux development environment to use with the *Microsoft Azure IoT Hub device management library for C* and run the iotdm\_simple\_sample application on Linux.

## Prerequisites

You should also have the following items ready before beginning the process:

-   [Setup your IoT hub]

-   [Provision your device and get its credentials]

This setup process requires [git], **cmake** version 2.8.11 or higher and **gcc** version 4.9 or higher.

-   You can verify the current version installed in your environment using the `cmake --version` command.

-   You can verify the current version of **gcc** installed in your environment using the `gcc --version` command

## Building and running iotdm\_simple\_sample on Linux

1.  Clone the latest version of this repository to your Linux machine with the recursive parameter

  ```
  git clone --recursive --branch dmpreview https://github.com/Azure/azure-iot-sdks.git
  ```

2.  Type the following commands:

  ```
  cd azure-iot-sdks/c/build_all/linux
  ./setup.sh
  ./build.sh
  ```

3.  Before you run the iotdm\_simple\_sample application you need to supply it with a valid device connection string. You can provide this information when you run the application as a command line parameter in the following manner:

  ```
  cd ~/cmake/iotdm_client/samples/iotdm_simple_sample/
  ./iotdm\_simple\_sample "[device connection string]"
  ```

  Alternatively, you can insert your device connection string between the double quotes in the code:

  ```
  static const char* connectionString = "[device connection string]";
  ```

  This is located in the file **azure-iot-sdks/c/iotdm\_client/samples/iotdm\_simple\_sample/iotdm\_simple\_sample.c**.

  To rebuild an individual component, you can run `make` in **~/cmake/path/to/component** you want to rebuild. To rebuild iotdm\_simple\_sample after changing the connection string simply type:

  ```
  make
  ```

When the iotdm\_simple\_sample runs, the device is ready to receive and process requests from the IoT Hub service. Notice that when the device connects to IoT Hub, the service will automatically start to observe resources on the device. The device libraries will invoke the device callbacks to retrieve the latest values from the device and provide them to the service.

[Setup your IoT hub]: https://github.com/Azure/azure-iot-sdks/blob/master/doc/setup_iothub.md
[Provision your device and get its credentials]: https://github.com/Azure/azure-iot-sdks/blob/master/doc/manage_iot_hub.md
[git]: http://www.git-scm.com/
