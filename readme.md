# Microsoft Azure IoT SDKs

This repository contains both IoT device SDKs and IoT service SDKs. Device SDKs enable you connect client devices to Azure IoT Hub. Service SDKs enable you to manage your IoT Hub service instance.

Visit http://azure.com/iotdev to learn more about developing applications for Azure IoT.


## Microsoft Azure IoT device SDKs

The Microsoft Azure IoT device SDKs contain code that facilitate building devices and applications that connect to and are managed by Azure IoT Hub services.

Devices and data sources in an IoT solution can range from a simple network-connected sensor to a powerful, standalone computing device. Devices may have limited processing capability, memory, communication bandwidth, and communication protocol support. The IoT device SDKs enable you to implement client applications for a wide variety of devices.

This repository contains the following IoT device SDKs:

- [Azure IoT device SDK for .NET](csharp/device/readme.md)
- [Azure IoT device SDK for C](c/readme.md)
- [Azure IoT device SDK for Node.js](node/device/core/readme.md)
- [Azure IoT device SDK for Java](java/device/readme.md)

Each language SDK includes sample code and documentation in addition to the library code.

### OS platforms and hardware compatibility

Azure IoT device SDKs can be used with a broad range of OS platforms and devices. See [OS Platforms and hardware compatibility](https://azure.microsoft.com/en-us/documentation/articles/iot-hub-tested-configurations/).

## Microsoft Azure IoT service SDKs

The Azure IoT Service SDKs help you to build applications that interact with your devices and manage device identities in your IoT hub.

- [Azure IoT service SDK for .Net](csharp/service/README.md)
- [Azure IoT service SDK for Node.js](node/service/README.md)
- [Azure IoT service SDK for Java](java/service/readme.md)

## Contribution, feedback and issues

If you would like to become an active contributor to this project please follow the instructions provided in the [contribution guideline](contribute.md).
If you encounter any bugs or have suggestions for new features, please file an issue in the [Issues](https://github.com/Azure/azure-iot-sdks/issues) section of the project.

## Support

If you are having issues using one of the packages or using the Azure IoT Hub service that go beyond simple bug fixes or help requests that would be dealt within the issues section of this project, the Microsoft Customer Support team will try and help out on a best effort basis.
To engage Microsoft support, you can create a support ticket directly from the [Azure portal](https://ms.portal.azure.com/#blade/Microsoft_Azure_Support/HelpAndSupportBlade).
Escalated support requests for Azure IoT Hub SDKs development questions will only be available Monday thru Friday during normal coverage hours of 6 a.m. to 6 p.m. PST.
Here is what you can expect the Microsoft Support to be able to help with:
* **Client SDKs issues**: If you are trying to compile and run the libraries on a supported platform, the Support team will be able to assist with troubleshooting or questions related to compiler issues and communications to and from the IoT Hub.  They will also try to assist with questions related to porting to an unsupported platform, but will be limited in how much assistance can be provided.  The team will be limited with trouble-shooting the hardware device itself or drivers and or specific properties on that device. 
* **IoT Hub / Connectivity Issues**: Communication from the device client to the Azure IoT Hub service and communication from the Azure IoT Hub service to the client.  Or any other issues specifically related to the Azure IoT Hub.
* **Portal Issues**: Issues related to the portal, that includes access, security, dashboard, devices, Alarms, Usage, Settings and Actions.
* **REST/API Issues**: Using the IoT Hub REST/APIs that are documented in the [documentation]( https://msdn.microsoft.com/library/mt548492.aspx).

## Additional resources

In addition to the language SDKs, this repository ([azure-iot-sdks](https://github.com/Azure/azure-iot-sdks)) contains the following folders:

### /build

This folder contains various build scripts to build the libraries.

### /doc

This folder contains the following documents that are relevant to all the language SDKs:

- [Set up IoT Hub](doc/setup_iothub.md) describes how to configure your Azure IoT Hub service.
- [Manage IoT Hub](doc/manage_iot_hub.md) describes how to provision devices in your Azure IoT Hub service.
- [FAQ](doc/faq.md) contains frequently asked questions about the SDKs and libraries.
- [OS Platforms and hardware compatibility](https://azure.microsoft.com/en-us/documentation/articles/iot-hub-tested-configurations/) describes the SDK compatibility with different OS platforms as well as specific device configurations.

### /tools

This folder contains tools you will find useful when you are working with IoT Hub and the device SDKs.
- [iothub-explorer](tools/iothub-explorer/readme.md): describes how to use the iothub-explorer node.js tool to provision a device for use in IoT Hub, monitor the messages from the device, and send commands to the device.
- [Device Explorer](tools/DeviceExplorer/doc/how_to_use_device_explorer.md): this tool enables you to perform operations such as manage the devices registered to an IoT hub, view device-to-cloud messages sent to an IoT hub, and send cloud-to-device messages from an IoT hub. Note this tool only runs on Windows.
