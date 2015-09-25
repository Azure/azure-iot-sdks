# Microsoft Azure IoT SDKs

This repository contains both IoT device SDKs and IoT service SDKs. Device SDKs enable you connect client devices to Azure IoT Hub. Service SDKs enable you to manage your IoT Hub service instance.

Visit http://azure.com/iotdev to learn more about developing applications for Azure IoT.


## Microsoft Azure IoT device SDKs

The Microsoft Azure IoT device SDKs contain code that facilitate building devices and applications that connect to and are managed by Azure IoT Hub services.

Devices and data sources in an IoT solution can range from a simple network-connected sensor to a powerful, standalone computing device. Devices may have limited processing capability, memory, communication bandwidth, and communication protocol support. The IoT device SDKs enable you to implement client applications for a wide variety of devices.

This repository contains the following IoT device SDKs:

- [Azure IoT device SDK for C](c/readme.md) (written in ANSI C (C99) for portability and broad platform compatibility).
- [Azure IoT device SDK for .NET](csharp/readme.md)
- [Azure IoT device SDK for Java](java/device/readme.md)
- [Azure IoT device SDK for Node.js](node/device/readme.md)

Each language SDK includes sample code and documentation in addition to the library code.

## OS platforms and hardware compatibility

Azure IoT device SDKs can be used with a broad range of OS platforms and devices. See [OS Platforms and hardware compatibility](doc/tested_configurations.md).

## Microsoft Azure IoT service SDKs

The IoT Service SDKs for Java and node.js are coming soon.

**Note:** To manage an IoT hub using a .NET application, use the **Microsoft.Azure.Devices** NuGet package.

## Additional resources

In addition to the language SDKs, this repository ([azure-iot-sdks](https://github.com/Azure/azure-iot-sdks)) contains the following folders:

### /build

This folder contains various build scripts to build the libraries.

### /doc

This folder contains the following documents that are relevant to all the language SDKs:

- [Set up IoT Hub](doc/setup_iothub.md) describes how to configure your Azure IoT Hub service.
- [FAQ](doc/faq.md) contains frequently asked questions about the SDKs and libraries.
- [OS Platforms and hardware compatibility](doc\tested_configurations.md) describes the SDK compatibility with different OS platforms as well as specific device configurations.

### /tools

This folder contains tools you will find useful when you are working with IoT Hub and the device SDKs.

- [Device Explorer](tools/DeviceExplorer/doc/how_to_use_device_explorer.md): this tool enables you to perform operations such as manage the devices registered to an IoT hub, view device-to-cloud messages sent to an IoT hub, and send cloud-to-device messages from an IoT hub. Note that Device Explorer only runs on Windows.
