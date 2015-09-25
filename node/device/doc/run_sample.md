# Getting started - running a Node.js sample application

This "Getting Started" document shows how to run the **simple_sample_http.js** Node.js sample application.

## Prerequisites

Before you get started, you should:

- [Setup your IoT Hub][setup-iothub]
- Configure your device in IoT Hub. <br/>See the section "Configure IoT Hub connection" in the document [How to use Device Explorer][device-explorer].

**Note:** The Device Explorer utility only runs on Windows. You can run it on a different machine from your Node.js development environment.

If you want to run the node sample on your machine (PC, Mac), jump directly to the ["Run the sample on your PC (or Mac)"](#runonPC)

## Run the sample on a device

- Follow the instructions from the device manufacturer to install node if it is not pre-isntalled.

- Get the following sample files from https://github.com/Azure/azure-iot-sdks/tree/develop/node/device/samples
    - **package.json**
    - **simple_sample_http.js**
    

- Open the file **simple_sample_http.js** in a text editor.

- Locate the following code in the file:

    ```
    var connectionString = '[IoT Device Connection String]';
    ```

- Replace `[IoT Device Connection String]` with the connection string for your device. The section "Get device connection string or data" in the document [How to use Device Explorer][device-explorer] explains how to retrieve the connection string for your device. Save the changes.

- The section "Send events" in the document [How to use Device Explorer][device-explorer] describes how to prepare the **DeviceExplorer** tool to receive device-to-cloud messages from the sample application.

- Copy these files to your device following your device's manufacturer instructions.

- Follow the instructions for your device to open a command shell (whether remote with an SSH client or directly on the device)
- In the shell, navigate to the folder you copied the files to. Then run the sample application using the following commands:

    ```
    npm install
    node .
    ```

-  Use the **DeviceExplorer** utility to observe the messages IoT Hub receives from the **simplesample_amqp** application.

<a name="runonPC"/>
## Run the sample on your PC (or Mac)

- To install node.js on your machine, follow instructions on http://nodejs.org.

- Get the following sample files from https://github.com/Azure/azure-iot-sdks/node/device/samples
    - **package.json**
    - **simple_sample_http.js**

- Create a new folder in the location of your choice on your machine and place the 2 files in this new folder.    

- Open the file **simple_sample_http.js** in a text editor.

- Locate the following code in the file:

    ```
    var connectionString = '[IoT Device Connection String]';
    ```

- Replace `[IoT Device Connection String]` with the connection string for your device. The section "Get device connection string or data" in the document [How to use Device Explorer][device-explorer] explains how to retrieve the connection string for your device. Save the changes.

- The section "Send events" in the document [How to use Device Explorer][device-explorer] describes how to prepare the **DeviceExplorer** tool to receive device-to-cloud messages from the sample application.

- Open a shell (Linux) or Node.js command prompt (Windows) and navigate to the folder. Then run the sample application using the following commands:

    ```
    npm install
    node .
    ```

-  Use the **DeviceExplorer** utility to observe the messages IoT Hub receives from the **simplesample_amqp** application.

[setup-devbox]: devbox_setup.md
[device-explorer]: ../../tools/DeviceExplorer/doc/how_to_use_device_explorer.md
[setup-iothub]: ../../doc/setup_iothub.md
