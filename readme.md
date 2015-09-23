# Microsoft Azure IoT SDKs

This repository contains both IoT Device SDKs and IoT Service SDKs. Device SDKs enable you to implement IoT client applications to run on your devices. Service SDKs enable you to manage your IoT Hub service instance.




## Microsoft Azure IoT Device SDKs

The Microsoft Azure IoT device SDKs contain code that facilitate building devices and applications that connect to and are managed by Azure IoT Hub services.

Devices and data sources in an IoT solution can range from a simple network-connected sensor to a powerful, standalone computing device. Devices may have limited processing capability, memory, communication bandwidth, and communication protocol support. The IoT Device SDKs enable you to implement client applications for a wide variety of devices.

This repository contains the following IoT Device SDKs:

- [Microsoft Azure IoT Device SDK for C](c/readme.md) written in ANSI C (C99) for portability and broad platform compatibility.
- [Microsoft Azure IoT Device SDK for .NET](csharp/readme.md) for creating client applications that target the .NET runtime.
- [Microsoft Azure IoT Device SDK for Java](java/device/readme.md) for portability across the broad range of platforms that support the Java programming language.
- [Microsoft Azure IoT Device SDK for Node.js](node/device/readme.md) for portability across the broad range of platforms that support Node.js.

Each language SDK includes sample code and documentation in addition to the library code.

## Microsoft Azure IoT Service SDKs

The IoT Service SDKs include the following features:
- A service client you can use to send cloud-to-device commands from IoT Hub to your devices.
- A registry manager that you can use to manage the contents of the IoT Hub device identity registry. The device identity registry stores information about each device that is permitted to connect to the IoT hub. This device information includes the unique device identifier, the device's access key, and enabled state of the device.

This repository contains the following IoT Service SDKs:

- [Microsoft Azure IoT Service SDK for Java](java/service/readme.md)
- [Microsoft Azure IoT Service SDK for Node.js](node/service/readme.md)

**Note:** To manage an IoT hub using a .NET application, use the **Microsoft.Azure.Devices** NuGet package.

## OS platforms and hardware compatibility

Azure IoT SDKs can be used with a broad range of OS platforms and devices. For a list of tested configurations [click here](doc/tested_configurations.md)

## Additional resources

In addition to the language SDKs, this repository contains the following folders:

### /build

This folder contains various build scripts to build the libraries.

### /doc

This folder contains the following documents that are relevant to all the language SDKs:

- [Setup IoT Hub](doc/setup_iothub.md) describes how to configure your Azure IoT Hub service.
- [FAQ](doc/faq.md) contains frequently asked questions about the SDKs and libraries.

### /tools

This folder contains tools you will find useful when you are working with IoT Hub and the device SDKs.

- [Device Explorer](tools/DeviceExplorer/doc/how_to_use_device_explorer.md): this tool enables you to perform operations such as manage the devices registered to an IoT hub, view device-to-cloud messages sent to an IoT hub, and send cloud-to-device messages from an IoT hub.
