---
platform: debian, fedora, Linux, opensuse, raspbian, Ubuntu, windows, yocto 
device: any
language: javascript
---

Run a simple Node.js service sample to update the device registry
===
---

# Table of Contents

-   [Introduction](#Introduction)
-   [Step 1: Prerequisites](#Prerequisites)
-   [Step 2: Build and Run the Sample](#Build)

<a name="Introduction"></a>
# Introduction

**About this document**

This document describes how to build and run the **registry_sample.js** Node.js sample application that reads and updates the device identity registry in IoT Hub. This multi-step process includes:
-   Configure Azure IoT Hub
-   Build and run the sample

<a name="Prerequisites"></a>
# Step 1: Prerequisites

You should have the following items ready before beginning the process:
- Ensure that Node.js version 0.12.x or later is installed. Run `node --version` at the command line to check the version. For information about using a package manager to install Node.js on Linux, see [Installing Node.js via package manager][node-linux].
- [Setup your IoT hub][lnk-setup-iot-hub] and retrieve your IoT Hub connection string.

<a name="Build"></a>
# Step 2: Build and run the sample

- Get the following sample files from https://github.com/Azure/azure-iot-sdks/tree/master/node/service/samples
    - **package.json**
    - **registry_sample.js**

- Place the files in a folder of your choice on your development machine.

- Open the file **registry_sample.js** in a text editor.

- Locate the following code in the file:

    ```
    var connectionString = '[IoT Connection String]';
    ```

- Replace `[IoT Connection String]` with the connection string for your IoT Hub. You retrieved this connection string when you set up your IoT Hub during the prerequisites step.

- Open a new shell or Node.js command prompt and navigate to the folder where you placed the sample files. Run the sample application using the following commands:

    ```
    npm install
    node registry_sample.js
    ```

- The sample application updates your device identity registry.

# Debugging the samples
[Visual Studio Code](https://code.visualstudio.com/) provides an excellent environment to write and debug Node.js code:
- [Debugging with Visual Studio Code](../../doc/get_started/node-debug-vscode.md)

[lnk-setup-iot-hub]: ../setup_iothub.md
[node-linux]: https://github.com/nodejs/node-v0.x-archive/wiki/Installing-Node.js-via-package-manager