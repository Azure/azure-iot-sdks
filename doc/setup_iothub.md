# Setup IoT Hub

[Azure IoT Hub][iothub-landing] is a fully managed service that enables reliable and secure bi-directional communications between millions of IoT devices and an application back end. Azure IoT Hub offers reliable device-to-cloud and cloud-to-device hyper-scale messaging, enables secure communications using per-device security credentials and access control, and includes device libraries for the most popular languages and platforms.

Before you can communicate with IoT Hub from a device you must create an IoT hub instance in your Azure subscription and then provision your device in your IoT hub. You must complete these steps before you try to run any of the sample IoT Hub device client applications in this repository ([azure-iot-sdks](https://github.com/Azure/azure-iot-sdks)).

## Create an IoT hub

You can use the [Azure Portal][azure-portal] to create an IoT hub to use with your devices. For more information about creating and managing an IoT hub in the Azure portal, see [Manage IoT Hub through the Azure Portal][manage-iothub-portal].

You can obtain a connection string for your IoT hub from the Azure Portal. This connection string enables client applications to perform management operations on the IoT hub such as adding a new device to the IoT hub. The [Device Explorer][use-device-explorer] utility in this repository ([azure-iot-sdks](https://github.com/Azure/azure-iot-sdks)) is an example of such an IoT hub management application.

## Add devices to IoT Hub

You must details of your device to IoT Hub before that device can communicate with the hub. When you add a device to an IoT hub, the hub generates the connection string that the device must use when it establishes the secure connection to the IoT hub.

To add a device to your IoT hub, you can use the [Device Explorer][use-device-explorer] utility in this repository ([azure-iot-sdks](https://github.com/Azure/azure-iot-sdks)). This utility enables you to copy and paste the device specific connection string into the source code of application you will run on the device.


[iothub-landing]: http://azure.microsoft.com/en-us/documentation/services/iot-hub/
[azure-portal]: https://portal.azure.com
[manage-iothub-portal]: http://azure.microsoft.com/en-us/documentation/articles/iot-hub-manage-portal/
[use-device-explorer]: ../tools/DeviceExplorer/doc/how_to_use_device_explorer.md
