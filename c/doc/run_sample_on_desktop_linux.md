# Getting started - running a C sample application on desktop Linux

This "Getting Started" document shows you how to build and run the **simplesample_amqp** application on a desktop Linux platform.

## Requirements

These instructions assume that you have done the following:

- [Prepare your development environment][devbox-setup]
- [Setup your IoT Hub][setup-iothub]
- Configure your device in IoT Hub. <br/>See the section "Configure IoT Hub connection" in the document [How to use Device Explorer][device-explorer].

**Note:** The Device Explorer utility only runs on Windows. You can run it on a different machine from your Linux desktop development environment.

## Build and run the sample

1. Open the file **c/serializer/samples/simplesample_amqp/simplesample_amqp.c** in a text editor.

2. Locate the following code in the file:
    ```
   static const char* connectionString = "[device connection string]";
    ```
3. Replace "[device connection string]" with your connection string (to learn how to obtain this device connection string using the device explorer, read [this](../../tools/DeviceExplorer/doc/how_to_use_device_explorer.md)).

4. The section "Send events" in the document [How to use Device Explorer](../../tools/DeviceExplorer/doc/how_to_use_device_explorer.md) describes how to prepare the **DeviceExplorer** tool to receive device-to-cloud messages from the sample application.

5. Save your changes and build the samples.  To build your sample you can run the the build.sh script in the **c/build_all/linux** directory. To skip running the unit tests, use:

  ```
  ./build.sh --skip-unit-tests
  ```

6. Run the **c/serializer/samples/simplesample_amqp/linux/simplesample_amqp** sample application.

7. Use the **DeviceExplorer** utility to observe the messages IoT Hub receives from the **simplesample_amqp** application.

8. See "Send cloud-to-device messages" in the document [How to use Device Explorer for IoT Hub devices][device-explorer] for instructions on sending messages with the **DeviceExplorer** utility.

[setup-devbox-linux]: devbox_setup.md
[device-explorer]: ../../tools/DeviceExplorer/doc/how_to_use_device_explorer.md
[setup-iothub]: ../../doc/setup_iothub.md
