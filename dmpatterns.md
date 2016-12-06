This project has adopted the [Microsoft Open Source Code of Conduct](https://opensource.microsoft.com/codeofconduct/). For more information see the [Code of Conduct FAQ](https://opensource.microsoft.com/codeofconduct/faq/) or contact [opencode@microsoft.com](mailto:opencode@microsoft.com) with any additional questions or comments.

# The device management patterns samples

The device management pattern samples support the following device management tutorials, which provide both the device and service side code ready to execute. 

- [Get started with device management](https://azure.microsoft.com/en-us/documentation/articles/iot-hub-device-management-get-started/)
- [How to do a firmware update](https://azure.microsoft.com/en-us/documentation/articles/iot-hub-firmware-update/)

To learn more about Azure IoT Hub device management, see [Overview of Azure IoT Hub device management](https://azure.microsoft.com/en-us/documentation/articles/iot-hub-device-management-overview/).

## Installing the samples

1. From the root directory of the repository, run the following commands:
    ```
    cd node/device/samples
    npm install
    cd ../../service/samples
    npm install
    ```

## Running the samples

From the root directory of the repository, run through the following steps to see the device and service interacting to enable the device management patterns:

### Reboot device management pattern:

1. Start the device side first, as it will register the C2D method listener for reboot:
    ```
    cd node/device/samples
    node dmpatterns_reboot_device.js <IotHub device connection string>
    ```

2. In a new terminal window, start the service side to initate the reboot:
    ```
    cd node/service/samples
    node dmpatterns_reboot_service.js <IotHub connection string> <targetDeviceId>
    ```

### Firmware Update device management pattern:

1. Start the device side first, as it will register the C2D method listener for firmware update:
    ```
    cd node/device/samples
    node dmpatterns_fwupdate_device.js <IotHub device connection string>
    ```

2. In a new terminal window, start the service side to initate the firmware update:
    ```
    cd node/service/samples
    node dmpatterns_fwupdate_service.js <IotHub connection string> <targetDeviceId>
    ```
