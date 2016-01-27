Run a simple Node.js sample on device
===
---

# Table of Contents

-   [Introduction](#Introduction)
-   [Step 1: Prerequisites](#Prerequisites)
-   [Step 2: Prepare your Device](#PrepareDevice)
-   [Step 3: Build and Run the Sample](#Build)

<a name="Introduction"></a>
# Introduction

**About this document**

This document describes how to build and run the **simple_sample_http.js** Node.js sample application. This multi-step process includes:
-   Configuring Azure IoT Hub
-   Registering your IoT device
-   Build and deploy Azure IoT SDK on device

<a name="Prerequisites"></a>
# Step 1: Prerequisites

You should have the following items ready before beginning the process:

-   [Prepare your development environment][setup-devbox].
You should have the following items ready before beginning the process:
-   Computer with Git client installed and access to the
    [azure-iot-sdks](https://github.com/Azure/azure-iot-sdks) GitHub public repository.
-   [Prepare your development environment](https://github.com/Azure/azure-iot-sdks/blob/master/csharp/device/doc/devbox_setup.md).
-   Download and install [DeviceExplorer](https://github.com/Azure/azure-iot-sdks/releases/download/2015-11-13/SetupDeviceExplorer.msi).
-   [Set up your IoT hub](https://github.com/Azure/azure-iot-sdks/blob/master/doc/setup_iothub.md).

#### Create a device on IoT Hub
-   With your IoT hub configured and running in Azure, follow the instructions in **"Create Device"** section of [DeviceExplorer Usage document](https://github.com/Azure/azure-iot-sdks/blob/master/tools/DeviceExplorer/doc/how_to_use_device_explorer.md).

#### Write down device credentials
-   Make note of the Connection String for your device by following the instructions in **"Get device connection string or configuration data"** section of [DeviceExplorer Usage document](https://github.com/Azure/azure-iot-sdks/blob/master/tools/DeviceExplorer/doc/how_to_use_device_explorer.md).

<a name="PrepareDevice"></a>
# Step 2: Prepare your Device

-   Make sure desktop is ready as per instructions given on [Prepare your development environment][setup-devbox].

<a name="Build"></a>
# Step 3: Build and Run the sample

- Get the following sample files from https://github.com/Azure/azure-iot-sdks/node/device/samples
    - **package.json**
    - **simple_sample_http.js**

- Place the files in the folder of your choice on the target machine/device

- Open the file **simple_sample_http.js** in a text editor.

- Locate the following code in the file:

    ```
    var connectionString = '[IoT Device Connection String]';
    ```

- Replace `[IoT Device Connection String]` with the connection string for your device. Save the changes.

- From the shell or command prompt you used earlier to run the **iothub-explorer** utility, use the following command to receive device-to-cloud messages from the sample application (replace <device-id> with the ID you assigned your device earlier):

    ```
    node iothub-explorer.js "<iothub-connection-string>" monitor-events <device-id>
    ```

- Open a new shell or Node.js command prompt and navigate to the folder where you placed the sample files. Run the sample application using the following commands:

    ```
    npm install
    node .
    ```

- The sample application will send messages to your IoT hub, and the **iothub-explorer** utility will display the messages as your IoT hub receives them.

[setup-devbox]: node-devbox-setup.md
