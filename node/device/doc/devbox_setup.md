# Prepare your development environment

This document describes how to prepare your development environment to use the *Microsoft Azure IoT SDK for Node.js*.

- [Setup your development environment](#devenv)
- [Sample applications](#readme)

<a name="devenv"/>
## Setup your development environment

Complete the following steps to set up your development environment:
- Ensure that the 0.12.x version of Node.js is installed. Run `node --version` at the command line to check the version.

  You may choose to develop directly on your target device, in which case you must ensure that the correct version of Node.js is installed on the device. To download Node.js and to learn how to install it, visit [Node.js][node-site]. Make sure you download the correct version (0.12.x) from the [Previous Releases][node-prev] page. For information about using a package manager to install on Linux, see [Installing Node.js via package manager][node-linux].

- When you have installed Node.js, clone the latest version of this repository ([azure-iot-sdks](https://github.com/Azure/azure-iot-sdks)) to your development machine or device. You should always use the **master** branch for the latest version of the libraries and samples.

- If you are using Windows, open the **Node.js command prompt**, then navigate to the **node\\build** folder in your local copy of this repository ([azure-iot-sdks](https://github.com/Azure/azure-iot-sdks)). Then run the `build.cmd` script to verify your installation.

- If you are using Linux, open a shell, then navigate to the **node/build** folder in your local copy of this repository ([azure-iot-sdks](https://github.com/Azure/azure-iot-sdks)). Then run the `build.sh` script to verify your installation.

<a name="samplecode"/>
## Sample applications

This repository contains various Node.js sample applications that illustrate how to use the Microsoft Azure IoT SDK for Node.js. To learn how to run a sample application that sends messages to an IoT hub, see the [Getting started - running a Node.js sample application][getstarted].

To run the sample applications, send device-to-cloud messages to IoT Hub, and receive cloud-to-device messages from IoT Hub, use the [Device Explorer][device-explorer] utility. This utility only runs on Windows.


[node-site]: https://nodejs.org/
[node-prev]: https://nodejs.org/en/download/releases/
[node-linux]: https://github.com/nodejs/node-v0.x-archive/wiki/Installing-Node.js-via-package-manager
[getstarted]: run_sample.md
[device-explorer]: ../../tools/DeviceExplorer/doc/how_to_use_device_explorer.md
