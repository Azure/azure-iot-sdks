# Prepare your development environment

This document describes how to prepare your development environment to use the *Microsoft Azure IoT SDK for Node.js*.

- [Setup your development environment](#devenv)
- [Sample applications](#readme)

<a name="devenv"/>
## Setup your development environment

Complete the following steps to set up your development environment:
- Ensure that Node.js version 0.12.x or later is installed. Run `node --version` at the command line to check the version. For information about using a package manager to install Node.js on Linux, see [Installing Node.js via package manager][node-linux].

- When you have installed Node.js, clone the latest version of this repository ([azure-iot-sdks](https://github.com/Azure/azure-iot-sdks)) to your development machine or device. You should always use the **master** branch for the latest version of the libraries and samples.

- If you are using Windows, open the **Node.js command prompt** and navigate to the **node** folder in your local copy of this repository ([azure-iot-sdks](https://github.com/Azure/azure-iot-sdks)). Run the `build\dev-setup.cmd` script to prepare your development environment. Then run the `build\build.cmd` script to verify your installation.

- If you are using Linux, open a shell and navigate to the **node** folder in your local copy of this repository ([azure-iot-sdks](https://github.com/Azure/azure-iot-sdks)). Run the `build/dev-setup.sh` script to prepare your development environment. Then run the `build/build.sh` script to verify your installation.

<a name="samplecode"/>
## Sample applications

This repository contains various Node.js sample applications that illustrate how to use the Microsoft Azure IoT SDK for Node.js. To learn how to run a sample application that sends messages to an IoT hub, see [Getting started - running a Node.js sample application][getstarted].

[node-linux]: https://github.com/nodejs/node-v0.x-archive/wiki/Installing-Node.js-via-package-manager
[getstarted]: run_sample.md
