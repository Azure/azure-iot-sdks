# Manage IoT Hub

Before a device can communicate with IoT Hub, you must add details of that device to the IoT Hub device identity registry. When you add a device to your IoT Hub device identity registry, the hub generates the connection string that the device must use when it establishes its secure connection to your hub. You can also use the device identity registry to disable a device and prevent it from connecting to your hub.

To add devices to your IoT hub and manage those devices, you can use either of:

- The cross-platform, command-line [iothub-explorer](#iothub-explorer) tool
- The Windows-only, graphical [Device Explorer](#device-explorer) tool

Use either of these tools to generate a device-specific connection string that you can copy and paste in the source code of the application running on your device. Both tools are available in this [repository][lnk-this-repo].
 
> Note: While IoT Hub supports multiple authentication schemes for devices, both these tools generate a pre-shared key to use for authentication.

> Note: You must have an IoT hub running in Azure before you can provision your device. The document [Set up IoT Hub][setup-iothub] describes how to set up an IoT hub.

You can also use both of these tools to monitor the messages that your device sends to an IoT hub and send commands to you your devices from IoT Hub.

<a name="iothub-explorer"/>
## Use the iothub-explorer tool to provision a device

The iothub-explorer tool is a cross-platform, command-line tool (written in Node.js) for managing your devices in IoT hub. You will need to pre-install Node.js for your platform including npm package manager from [nodejs.org](https://nodejs.org)

To install this tool in your environment, run the following command in a terminal/shell window on your machine:

```
npm install -g iothub-explorer
```

See [Install iothub-explorer][lnk-install-iothub-explorer] for more information.

To provision a new device:

1. Get the connection string for your IoT hub. See [Set up IoT Hub][setup-iothub] for more details.

2.  Run the following command to register your device with your IoT hub. When you run the command, replace `<iothub-connection-string>` with the IoT Hub connection string from the previous step and replace `<device-name>` with a name for your device such as **mydevice**.

```
iothub-explorer <iothub-connection-string> create <device-name> --connection-string
```

You should see a response like this:

```
Created device mydevice

-
...
-
  connectionString: HostName=<hostname>;DeviceId=mydevice;SharedAccessKey=<device-key>
```

Copy the device connection string information for later use. The samples in this repository use connection strings in the format `HostName=<iothub-name>.azure-devices.net;DeviceId=<device-name>;SharedAccessKey=<device-key>`.

To get help on using the iothub-explorer tool to perform other tasks such as listing devices, deleting devices, and sending commands to devices, enter the following command:

```
iothub-explorer --help
```

For further information about using the iothub-explorer tool to perform tasks such as disabling a device, monitoring a device, and sending commands to a device see:

- [Working with the device identity registry][lnk-iothub-explorer-identity]
- [Working with devices][lnk-iothub-explorer-devices]

<a name="device-explorer"/>
## Use the Device Explorer tool to provision a device

The Device Explorer tool is a Windows-only graphical tool for managing your devices in IoT Hub. 

The easiest way to install the Device Explorer tool in your environment is to download the pre-built version by clicking [Azure IoT SDKs releases][lnk-releasepage]. Scroll down to the **Downloads** section to locate the download link for the **SetupDeviceExplorer.msi** installer. Download and run the installer.

To run the Device Explorer tool, double-click the **DeviceExplorer.exe** file in Windows Explorer.
The default installation folder for this application is **C:\Program Files (x86)\Microsoft\DeviceExplorer**.

> Tip: Pin the **DeviceExplorer.exe** application to your taskbar in Windows for easier access.

Before you register a new device in the IoT Hub device identity registry, you must connect Device Explorer to your IoT hub:

1. Get the connection string for your IoT hub. See [Set up IoT Hub][setup-iothub] for more details.

2. On the **Configuration** tab, paste the IoT Hub connection-string for your IoT hub into **IoT Hub connection string** and click **Update**:

    ![][img-getstarted1]

To register a new device with IoT Hub:

1.  Click the **Management** tab to manage the devices connected to the IoT hub.

    ![][img-getstarted2]

2.  On the **Management** tab, click **Create** to register a new device with your IoT hub. The **Create Device** dialog appears. In the **Device ID** field, type a unique name for your device such as **mydevice**, or select **Auto Generate ID** to generate a unique ID. Then click **Create**.

    ![][img-getstarted3]

3.  The **Device Created** window appears, indicating that your device has been successfully registered with this IoT Hub.

    ![][img-getstarted4]

4. Right-click on a selected device to retrieve the connection string for your device:

    ![][img-connstr]
  
5. Select **Copy connection string** to copy the device connection string to the clipboard. You can now paste this connection-string into the source code of the device application you are working with. The samples in this repository use connection strings in the format `HostName=<iothub-name>.azure-devices.net;DeviceId=<device-name>;SharedAccessKey=<device-key>`.

For further information about using the Device Explorer tool to perform tasks such as disabling a device, monitoring a device, and sending commands to a device see [Using the Device Explorer tool][lnk-device-explorer-docs].


[img-getstarted1]: media/device_explorer/iotgetstart1.png
[img-getstarted2]: media/device_explorer/iotgetstart2.png
[img-getstarted3]: media/device_explorer/iotgetstart3.png
[img-getstarted4]: media/device_explorer/iotgetstart4.png
[img-connstr]: media/device_explorer/connstr.png

[lnk-this-repo]: https://github.com/Azure/azure-iot-sdks
[setup-iothub]: setup_iothub.md
[lnk-install-iothub-explorer]: ../tools/iothub-explorer/readme.md#install
[lnk-iothub-explorer-identity]: ../tools/iothub-explorer/readme.md#identityregistry
[lnk-iothub-explorer-devices]: ../tools/iothub-explorer/readme.md#devices
[lnk-releasepage]: https://github.com/Azure/azure-iot-sdks/releases
[lnk-device-explorer-docs]: ../tools/DeviceExplorer/readme.md
