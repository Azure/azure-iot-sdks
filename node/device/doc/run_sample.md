# Getting started - running a Node.js sample application

This "Getting Started" document shows you how to build and run the **simple_sample_http.js** Node.js sample application.

## Prerequisites

Before you get started, you should:

- [Prepare your development environment][setup-devbox].
- [Setup your IoT Hub][setup-iothub]
- Configure your device in IoT Hub. <br/>See the section "Configure IoT Hub connection" in the document [How to use Device Explorer][device-explorer].

**Note:** The Device Explorer utility only runs on Windows. You can run it on a different machine from your Node.js development environment.

## Build and run the sample

1.  Open the file **node\\samples\\simple_sample_http.js** in a text editor.

2.  Locate the following code in the file:

    ```
    var connectionString = '[IoT Device Connection String]';
    ```

3.  Replace `[IoT Device Connection String]` with the connection string for your device. The section "Get device connection string or data" in the document [How to use Device Explorer][device-explorer] explains how to retrieve the connection string for your device. Save the changes.

4. The section "Send events" in the document [How to use Device Explorer][device-explorer] describes how to prepare the **DeviceExplorer** tool to receive device-to-cloud messages from the sample application.

5. Open a shell (Linux) or Node.js command prompt (Windows) and navigate to the **node\\samples** folder. Then run the sample application using the following command:

    ```
    node simple_sample_http.js
    ```

6. Use the **DeviceExplorer** utility to observe the messages IoT Hub receives from the **simplesample_amqp** application.

[setup-devbox]: devbox_setup.md
[device-explorer]: ../../tools/DeviceExplorer/doc/how_to_use_device_explorer.md
[setup-iothub]: ../../doc/setup_iothub.md
