# Getting started - running a C sample application on Windows

This "Getting Started" document shows you how to build and run the **simplesample_amqp** application on the Windows platform.

## Requirements

These instructions assume that you have done the following:

- [Prepare your development environment][devbox-setup]
- [Setup your IoT Hub][setup-iothub]
- Configure your device in IoT Hub. <br/>See the section "Configure IoT Hub connection" in the document [How to use Device Explorer][device-explorer].

## Build and run the sample

1. Start a new instance of Visual Studio 2015. Open the **serializer.sln** solution in the **c\\serializer\\build\\windows** folder in your local copy of the repository.

2. In Visual Studio, in **Solution Explorer**, navigate to the samples folder. In the **simplesample_amqp** project, open the **simplesample_amqp.c** file.

3. Locate the following code in the file:
    ```
   static const char* connectionString = "[device connection string]";
    ```
4. Replace "[device connection string]" with the connection string for your device. The section "Get device connection string or data" in the document [How to use Device Explorer][device-explorer] explains how to retrieve the connection string for your device. Save the changes.

5. The section "Send events" in the document [How to use Device Explorer](../../tools/DeviceExplorer/doc/how_to_use_device_explorer.md) describes how to prepare the **DeviceExplorer** tool to receive device-to-cloud messages from the sample application.

6. In **Solution Explorer**, right-click the **simplesample_amqp** project, click **Debug**, and then click **Start new instance** to build and run the sample. The console displays messages as the application sends device-to-cloud messages to IoT Hub.

7. Use the **DeviceExplorer** utility to observe the messages IoT Hub receives from the **simplesample_amqp** application.

8. See "Monitor device-to-cloud events" in the document [How to use Device Explorer][device-explorer] to learn how to use the **DeviceExplorer** utility to send cloud-to-device notification messages to the **simplesample_amqp** application.


[devbox-setup]: devbox_setup.md
[device-explorer]: ../../tools/DeviceExplorer/doc/how_to_use_device_explorer.md
[setup-iothub]: ../../doc/setup_iothub.md
