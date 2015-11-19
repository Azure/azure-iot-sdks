# Connecting your device to an IoT hub

IoT Hub only allows connections from known devices that present valid credentials. This document describes how to use the *iothub-explorer* tool to provision a device for use in IoT Hub.  While IoT Hub supports multiple authentication schemes, the iothub-explorer tool generates a pre-shared key.

 > Note: You must have an IoT hub running in Azure before you can provision your device. The document [Setup IoT Hub][setup-iothub] describes how to set up an IoT hub.

## Provision the device in your IoT hub

The iothub-explorer tool enables you to provision devices in your IoT hub. It runs on any computer where Node.js is available. To provision a new device:

1. Get the connection string for your IoT Hub. See [Setup IoT Hub][setup-iothub] for more details.
2. Run the following commands in a terminal/shell window from the tools/iothub-explorer folder in the repository using the IoT Hub connection string from the previous step and replacing "mydevice" with your desired device name.

```
npm install
node iothub-explorer.js <iothub-connection-string> create mydevice --connection-string
```

You should see a response like this:

```
Created device mydevice

-
...
-
  connectionString: HostName=<hostname>;DeviceId=mydevice;SharedAccessKey=<device-key>
```
Copy the device connection string information for later use.

To get help on using the iothub-explorer tool to perform other tasks such as listing devices, deleting devices, and sending commands to devices, enter the following command:

```
node iothub-explorer.js
```

> Note: Windows users also have the option of using the [Device Explorer][device-explorer] application to provision devices.

[setup-iothub]: ../../doc/setup_iothub.md
[device-explorer]: ../../tools/DeviceExplorer/doc/how_to_use_device_explorer.md
