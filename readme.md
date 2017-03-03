This project has adopted the [Microsoft Open Source Code of Conduct](https://opensource.microsoft.com/codeofconduct/). For more information see the [Code of Conduct FAQ](https://opensource.microsoft.com/codeofconduct/faq/) or contact [opencode@microsoft.com](mailto:opencode@microsoft.com) with any additional questions or comments.

# Microsoft Azure IoT SDKs

This repository contains both IoT device SDKs and IoT service SDKs. Device SDKs enable you connect client devices to Azure IoT Hub. Service SDKs enable you to manage your IoT Hub service instance.

Visit http://azure.com/iotdev to learn more about developing applications for Azure IoT.

> **Important Notice**
>
> All SDKs have now been moved in their own repositories (see links below).
> We are going to deprecate the *develop* branch on Jan 31, 2016.
> We are not accepting pull-requests on this repository anymore. If you have a pending pull-request, please move it to the appropriate SDK repository.

## Microsoft Azure IoT SDKs

The Microsoft Azure IoT **Device** SDKs contain code that facilitate building devices and applications that connect to and are managed by Azure IoT Hub services.
The Microsoft Azure IoT **Service** SDKs help you to build applications that interact with your devices and manage device identities in your IoT hub.

Devices and data sources in an IoT solution can range from a simple network-connected sensor to a powerful, standalone computing device. Devices may have limited processing capability, memory, communication bandwidth, and communication protocol support. The IoT device SDKs enable you to implement client applications for a wide variety of devices.

This repository serves as a landing page for all SDKs, each being actually maintained in their own repository, including sample code and documentation:

### Device-only SDKs
- [Azure IoT SDK for C](https://github.com/azure/azure-iot-sdk-c)
- [Azure IoT SDK for Python](https://github.com/azure/azure-iot-sdk-python)

### Device and Service SDKs
- [Azure IoT SDK for Node.js](https://github.com/azure/azure-iot-sdk-node)
- [Azure IoT SDK for Java](https://github.com/azure/azure-iot-sdk-java)
- [Azure IoT SDK for .NET](https://github.com/azure/azure-iot-sdk-csharp)

### OS platforms and hardware compatibility

Azure IoT device SDKs can be used with a broad range of OS platforms and devices. The minimum requirements are for the device platform to support the following:

- Being capable of establishing an IP connection: only IP-capable devices can communicate directly with Azure IoT Hub.
- Support TLS: required to establish a secure communication channel with Azure IoT Hub.
- Support SHA-256: necessary to generate the secure token for authenticating the device with the service.
- Have a Real Time Clock or implement code to connect to an NTP server: necessary for both establishing the TLS connection and generating the secure token for authentication.
- Having at least 64KB of RAM: the memory footprint of the SDK depends on the SDK and protocol used as well as the platform targeted. The smallest footprint is achieved using the C SDK targeting microcontrollers.

You can find an exhaustive list of the OS platforms the various SDKs have been tested against in the [Azure Certified for IoT device catalog](https://catalog.azureiotsuite.com/). Note that you might still be able to use the SDKs on OS and hardware platforms that are not listed on this page: all the SDKs are open sourced and designed to be portable. If you have suggestions, feedback or issues to report, refer to the Contribution and Support sections below.

## Contribution, feedback and issues

If you encounter any bugs, have suggestions for new features or if you would like to become an active contributor to this project please follow the instructions provided in the contribution guideline for the corresponding repo.

## Support

If you are having issues using one of the packages or using the Azure IoT Hub service that go beyond simple bug fixes or help requests that would be dealt within the [issues section](https://github.com/Azure/azure-iot-sdks/issues) of this project, the Microsoft Customer Support team will try and help out on a best effort basis.
To engage Microsoft support, you can create a support ticket directly from the [Azure portal](https://ms.portal.azure.com/#blade/Microsoft_Azure_Support/HelpAndSupportBlade).
Escalated support requests for Azure IoT Hub SDKs development questions will only be available Monday thru Friday during normal coverage hours of 6 a.m. to 6 p.m. PST.
Here is what you can expect Microsoft Support to be able to help with:
* **Client SDKs issues**: If you are trying to compile and run the libraries on a supported platform, the Support team will be able to assist with troubleshooting or questions related to compiler issues and communications to and from the IoT Hub.  They will also try to assist with questions related to porting to an unsupported platform, but will be limited in how much assistance can be provided.  The team will be limited with trouble-shooting the hardware device itself or drivers and or specific properties on that device. 
* **IoT Hub / Connectivity Issues**: Communication from the device client to the Azure IoT Hub service and communication from the Azure IoT Hub service to the client.  Or any other issues specifically related to the Azure IoT Hub.
* **Portal Issues**: Issues related to the portal, that includes access, security, dashboard, devices, Alarms, Usage, Settings and Actions.
* **REST/API Issues**: Using the IoT Hub REST/APIs that are documented in the [documentation]( https://msdn.microsoft.com/library/mt548492.aspx).

## Additional Resources

In addition to the language SDKs, this repository ([azure-iot-sdks](https://github.com/Azure/azure-iot-sdks)) contains the following folders:

### /doc

This folder contains the following documents that are relevant to all the language SDKs:

- [Set up IoT Hub](doc/setup_iothub.md) describes how to configure your Azure IoT Hub service.
- [Manage IoT Hub](doc/manage_iot_hub.md) describes how to provision devices in your Azure IoT Hub service.
- [FAQ](doc/faq.md) contains frequently asked questions about the SDKs and libraries.
- [Azure Certified for IoT device catalog](https://catalog.azureiotsuite.com/).

### /tools

This folder contains links to the repositories of tools you will find useful when you are working with IoT Hub and the device SDKs.
- [iothub-explorer](https://github.com/azure/iothub-explorer): describes how to use the iothub-explorer node.js tool to provision a device for use in IoT Hub, monitor the messages from the device, and send commands to the device.
- [Device Explorer](https://github.com/Azure/azure-iot-sdk-csharp/tree/master/tools/DeviceExplorer): this tool enables you to perform operations such as manage the devices registered to an IoT hub, view device-to-cloud messages sent to an IoT hub, and send cloud-to-device messages from an IoT hub. Note this tool only runs on Windows.
- [iothub-diagnostics](https://github.com/azure/iothub-diagnostics): this tool is provided to help diagnose issues with a device connecting to Azure IoT Hubs.
- [Azure IoT Toolkit for Visual Studio Code](https://github.com/formulahendry/vscode-azure-iot-toolkit): this tool enables you to perform operations such as manage the devices registered to an IoT hub, view device-to-cloud messages sent to an IoT hub, and send device-to-cloud messages as a simulated device. Note this tool is an extension of Visual Studio Code which is cross-platform.
