> **Important Notice**
>
> **All SDKs have now been moved in their own repositories (see links below).
> We are not accepting pull-requests on this repository anymore.**

# Microsoft Azure IoT SDKs

This repository serves as a landing page for all Azure IoT SDKs, each being maintained in their own repository, including sample code and documentation.  Visit [this page](https://docs.microsoft.com/en-us/azure/iot-hub/iot-hub-devguide-sdks) to learn more about Azure IoT SDKs.

Visit http://azure.com/iotdev to learn more about developing applications for Azure IoT.

## Microsoft Azure IoT SDKs

Microsoft Azure IoT SDKs for Azure IoT Hub and Azure IoT Hub Device Provisioning Service:
- [Azure IoT SDK for Embedded C](https://github.com/Azure/azure-sdk-for-c/tree/master/sdk/docs/iot) is an alternative for **constrained devices** which enables the BYO (bring your own) network approach: IoT developers have the freedom of choice to bring MQTT client, TLS and Socket of their choice to create a device solution.
- [Azure IoT middleware for Azure RTOS](https://github.com/azure-rtos/netxduo/tree/master/addons/azure_iot) builds on top of the embedded SDK and tightly couples with the Azure RTOS family of networking and OS products. This gives you very performant and small applications for real-time, constrained devices.
- [Azure IoT middleware for FreeRTOS](https://github.com/Azure/azure-iot-middleware-freertos) builds on top of the embedded SDK and takes care of the MQTT stack while integrating with FreeRTOS. This maintains the focus on constrained devices and gives users a distilled Azure IoT feature set while allowing for flexibility with their networking stack.
- [Azure IoT SDK for C](https://github.com/Azure/azure-iot-sdk-c) written in ANSI C (C99) for portability and broad platform compatibility. There are two device client libraries for C, the low-level iothub_ll_client and the iothub_client (threaded).
- [Azure IoT SDK for Python](https://github.com/Azure/azure-iot-sdk-python)
- [Azure IoT SDK for Node.js](https://github.com/Azure/azure-iot-sdk-node)
- [Azure IoT SDK for Java](https://github.com/Azure/azure-iot-sdk-java)
- [Azure IoT SDK for .NET](https://github.com/Azure/azure-iot-sdk-csharp)

### OS platforms and hardware compatibility

See individual repositories for details.

You can find an exhaustive list of the OS platforms the various SDKs have been tested against in the [Azure Certified for IoT device catalog](https://catalog.azureiotsuite.com/). Note that you might still be able to use the SDKs on OS and hardware platforms that are not listed on this page: all the SDKs are open sourced and designed to be portable. If you have suggestions, feedback or issues to report, refer to the Contribution and Support sections below.

## Contribution, feedback and issues

If you encounter any bugs, have suggestions for new features or if you would like to become an active contributor to this project please follow the instructions provided in the contribution guideline for the corresponding repo.

## Support

If you are having issues using one of the packages or using the Azure IoT Hub service that go beyond simple bug fixes or help requests that would be dealt within the issues section of the individual SDKs repositories, the Microsoft Customer Support team will try and help out on a best effort basis.
To engage Microsoft support, you can create a support ticket directly from the [Azure portal](https://ms.portal.azure.com/#blade/Microsoft_Azure_Support/HelpAndSupportBlade).
Escalated support requests for Azure IoT Hub SDKs development questions will only be available Monday thru Friday during normal coverage hours of 6 a.m. to 6 p.m. PST.
Here is what you can expect Microsoft Support to be able to help with:
* **Client SDKs issues**: If you are trying to compile and run the libraries on a supported platform, the Support team will be able to assist with troubleshooting or questions related to compiler issues and communications to and from the IoT Hub.  They will also try to assist with questions related to porting to an unsupported platform, but will be limited in how much assistance can be provided.  The team will be limited with trouble-shooting the hardware device itself or drivers and or specific properties on that device. 
* **IoT Hub / Connectivity Issues**: Communication from the device client to the Azure IoT Hub service and communication from the Azure IoT Hub service to the client.  Or any other issues specifically related to the Azure IoT Hub.
* **Portal Issues**: Issues related to the portal, that includes access, security, dashboard, devices, Alarms, Usage, Settings and Actions.
* **REST/API Issues**: Using the IoT Hub REST/APIs that are documented in the [documentation]( https://msdn.microsoft.com/library/mt548492.aspx).

This project has adopted the [Microsoft Open Source Code of Conduct](https://opensource.microsoft.com/codeofconduct/). For more information see the [Code of Conduct FAQ](https://opensource.microsoft.com/codeofconduct/faq/) or contact [opencode@microsoft.com](mailto:opencode@microsoft.com) with any additional questions or comments.

# Long Term Support

The project offers a Long Term Support (LTS) version to allow users that do not need the latest features to be shielded from unwanted changes.

A new LTS version will be created every 6 months. The lifetime of an LTS branch is currently planned for one year. LTS branches receive all bug fixes that fall in one of these categories:

- security bugfixes
- critical bugfixes (crashes, memory leaks, etc.)

Please see each language repo for details.
