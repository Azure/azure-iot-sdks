# Getting started - running a C# sample application on Toradex modules with Linux

This "Getting Started" document shows you how to setup Mono and run the **Device Client HTTPS Sample** application on a Toradex module with Linux.

## Prerequisites

Before you get started, you should:

- [Setup your IoT Hub][setup-iothub]
- Configure your device in IoT Hub. <br/>See the section "Configure IoT Hub connection" in the document [How to use Device Explorer][device-explorer].
- [Build DeviceClientHttpsSample C# sample application][run_sample_on_desktop_windows]
- Ensure the module is flashed with [Toradex V2.5 Linux image or newer][toradex_image_update].

## Configure Mono on Toradex module with Linux

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

## Run C# Sample Application

1.  Copy the the contents of the compiled **Device Client HTTPS Sample** application's **bin** directory to the module.

2.  Execute the application with Mono:

    ```
    mono DeviceClientHttpsSample.exe
    ```

3. Use the **DeviceExplorer** utility to observe the messages IoT Hub receives from the **Device Client HTTPS Sample** application.


[device-explorer]: ../../tools/DeviceExplorer/doc/how_to_use_device_explorer.md
[setup-iothub]: ../../doc/setup_iothub.md
[run_sample_on_desktop_windows]: run_sample_on_desktop_windows.md
[toradex_image_update]: http://developer.toradex.com/knowledge-base/how-to-setup-environment-for-embedded-linux-application-development#Linux_Image_Update