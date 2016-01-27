---
platform: linux
device: toradex-module
language: csharp
---

Run a simple Csharp sample on Toradex modules running Linux
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

This document describes how to how to setup Mono and run the **Device Client HTTPS Sample** application on a Toradex module with Linux. This multi-step process includes:
-   Configuring Azure IoT Hub
-   Registering your IoT device
-   Build and deploy Azure IoT SDK on device

<a name="Prerequisites"></a>
# Step 1: Prerequisites

You should have the following items ready before beginning the process:

-   Download and install [DeviceExplorer](https://github.com/Azure/azure-iot-sdks/releases/download/2015-11-13/SetupDeviceExplorer.msi).
-   [Set up your IoT hub](https://github.com/Azure/azure-iot-sdks/blob/master/doc/setup_iothub.md).
-   [Build DeviceClientHttpsSample C# sample application][run_sample_on_desktop_windows]
-   Ensure the module is flashed with [Toradex V2.5 Linux image or newer][toradex_image_update].

<a name="PrepareDevice"></a>
# Step 2: Prepare your Device

1.  Insert the module into a compatible carrier board.  Power on the system and connect to the internet.

2.  Use the command line to install Mono and necessary dependencies:

    ```
    opkg update
    opkg install mono mono-dev ca-certificates
    ```

3.  Import trusted root certificates:

    ```
    mono /usr/lib/mono/4.5/mozroots.exe --import --sync
    ```

<a name="Build"></a>
# Step 3: Build and Run the sample

1.  Copy the the contents of the compiled **Device Client HTTPS Sample** application's **bin** directory to the module.

2.  Execute the application with Mono:

    ```
    mono DeviceClientHttpsSample.exe
    ```

3. Use the **DeviceExplorer** utility to observe the messages IoT Hub receives from the **Device Client HTTPS Sample** application.


[run_sample_on_desktop_windows]: https://github.com/Azure/azure-iot-sdks/blob/master/csharp/device/doc/run_sample_on_desktop_windows.md
[toradex_image_update]: http://developer.toradex.com/knowledge-base/how-to-setup-environment-for-embedded-linux-application-development#Linux_Image_Update