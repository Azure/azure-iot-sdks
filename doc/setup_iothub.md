# Setup IoT Hub

[Azure IoT Hub][iothub-landing] is a fully managed service that enables reliable and secure bi-directional communications between millions of IoT devices and an application back end. Azure IoT Hub offers reliable device-to-cloud and cloud-to-device hyper-scale messaging, enables secure communications using per-device security credentials and access control, and includes device libraries for the most popular languages and platforms.

Before you can communicate with IoT Hub from a device you must create an IoT hub instance in your Azure subscription and then provision your device in your IoT hub. You must complete these steps before you try to run any of the sample IoT Hub device client applications in this repository ([azure-iot-sdks](https://github.com/Azure/azure-iot-sdks)).

## Create an IoT hub

You can use the [Azure Preview Portal][azure-portal] to create an IoT hub to use with your devices.

1. Log on to the [Azure Preview Portal][azure-portal].

2. In the jumpbar, click **New**, then click **Internet of Things**, and then click **IoT Hub**.

   ![][1]

3. In the **New IoT Hub** blade, specify the desired configuration for the IoT Hub.

   ![][2]

    * In the **Name** box, enter a name to identify your IoT hub. When the **Name** is validated, a green check mark appears in the **Name** box.
    * Change the **Pricing and scale tier** as desired. This tutorial does not require a specific tier.
    * In the **Resource group** box, create a new resource group, or select and existing one. For more information, see [Using resource groups to manage your Azure resources](resource-group-portal.md).
    * Use **Location** to specify the geographic location in which to host your IoT hub.  


4. Once the new IoT hub options are configured, click **Create**.  It can take a few minutes for the IoT hub to be created.  To check the status, you can monitor the progress on the Startboard. Or, you can monitor your progress from the Notifications section.

    ![][3]


5. After the IoT hub has been created successfully, open the blade of the new IoT hub, take note of the URI, and select the **Key** icon on the top.

    ![][4]

6. Select the Shared access policy called **iothubowner**, then copy and take note of the connection string on the right blade.

    ![][5]

Your IoT hub is now created, and you have the connection string you need to configure the [Device Explorer][use-device-explorer]. This connection string enables client applications to perform management operations on the IoT hub such as adding a new device to the IoT hub. The [Device Explorer][use-device-explorer] utility in this repository is an example of such an IoT hub management application.

## Add devices to IoT Hub

You must details of your device to IoT Hub before that device can communicate with the hub. When you add a device to an IoT hub, the hub generates the connection string that the device must use when it establishes the secure connection to the IoT hub.

To add a device to your IoT hub, you can use the [Device Explorer][use-device-explorer] utility in this repository ([azure-iot-sdks](https://github.com/Azure/azure-iot-sdks)). This utility enables you to copy and paste the device specific connection string into the source code of application you will run on the device.

[iothub-landing]: http://azure.microsoft.com/en-us/documentation/services/iot-hub/
[azure-portal]: https://portal.azure.com
[manage-iothub-portal]: http://azure.microsoft.com/en-us/documentation/articles/iot-hub-manage-portal/
[use-device-explorer]: ../tools/DeviceExplorer/doc/how_to_use_device_explorer.md

[1]: media/create-iot-hub1.png
[2]: media/create-iot-hub2.png
[3]: media/create-iot-hub3.png
[4]: media/create-iot-hub4.png
[5]: media/create-iot-hub5.png
