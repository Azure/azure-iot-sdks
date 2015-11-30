# Getting started - running a C# sample application on Windows

This "Getting Started" document shows you how to build and run the **Device Client AMQP Sample** application on the Windows platform.

## Prerequisites

Before you get started, you should:

- [Prepare your development environment][setup-devbox-windows].
- [Setup your IoT Hub][setup-iothub]
- Configure your device in IoT Hub. <br/>See the section "Configure IoT Hub connection" in the document [How to use Device Explorer][device-explorer].

## Build and run the sample

1.  Start a new instance of Visual Studio 2015. Open the **iothub_csharp_client.sln** solution in the **csharp\device** folder in your local copy of the repository.

2.  In Visual Studio, in **Solution Explorer**, navigate to the samples folder. In the **DeviceClientAmqpSample** project, open the **Program.cs** file.

3.  Locate the following code in the file:

    ```
    private const string DeviceConnectionString = "<replace>";
    ```

4.  Replace `<replace>` with the connection string for your device. The section "Get device connection string or data" in the document [How to use Device Explorer][device-explorer] explains how to retrieve the connection string for your device. Save the changes.

5. The section "Send events" in the document [How to use Device Explorer][device-explorer] describes how to prepare the **DeviceExplorer** tool to receive device-to-cloud messages from the sample application.

6. In **Solution Explorer**, right-click the **DeviceClientAmqpSample** project, click **Debug**, and then click **Start new instance** to build and run the sample. The console displays messages as the application sends device-to-cloud messages to IoT Hub.

7. Use the **DeviceExplorer** utility to observe the messages IoT Hub receives from the **Device Client AMQP Sample** application.

8. See the section "Send cloud-to-device messages" in the document [How to use Device Explorer][device-explorer] to learn how to use the **DeviceExplorer** utility to send cloud-to-device messages to the **Device Client AMQP Sample** application.


[setup-devbox-windows]: devbox_setup.md
[device-explorer]: ../../../tools/DeviceExplorer/doc/how_to_use_device_explorer.md
[setup-iothub]: ../../doc/setup_iothub.md
