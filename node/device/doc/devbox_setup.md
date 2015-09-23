# Prepare your development environment

This document describes how to prepare your development environment to use the *Microsoft Azure IoT SDK for Node.js*.

- [Setup your development environment](#devenv)
- [Sample applications](#readme)

<a name="devenv"/>
## Setup your development environment

Complete the following steps to setup your development environment:
- Install Node.js version 0.12.x on your development machine. You may choose to develop directly on your target device, in which case you must ensure that the correct version of Node.js is installed on the device. To download Node.js and to learn how to install it, visit [Node.js][node-site]. Make sure you download the correct version (0.12.x) from the [Previous Releases][node-prev] page. For information about using a package manager to install on Linux, see [Installing Node.js via package manager][node-linux].

- When you have installed Node.js, clone the latest version of this repository to your development machine or device. You should always use the **master** branch for the latest version of the libraries and samples.

- If you are using Windows, open the **Node.js command prompt**, then navigate to the **node** folder in your local copy of this repository. Then run the `build.cmd` script to verify your installation.

- If you are using Linux, open a shell, then navigate to the **node** folder in your local copy of this repository. Then run the `build.sh` script to verify your installation.

<a name="samplecode"/>
## Sample applications

This repository contains various Node.js sample applications that illustrate how to use the Microsoft Azure IoT SDK for Node.js. For more information, see the [readme][readme].

To run the sample applications, send device-to-cloud messages to IoT Hub, and receive cloud-to-device messages from IoT Hub, use the [Device Explorer][device-explorer] utility. This utility only runs on Windows.


[node-site]: https://nodejs.org/
[node-prev]: https://nodejs.org/en/download/releases/
[node-linux]: https://github.com/nodejs/node-v0.x-archive/wiki/Installing-Node.js-via-package-manager
[readme]: ../readme.md
[device-explorer]: ../../tools/DeviceExplorer/doc/how_to_use_device_explorer.md
