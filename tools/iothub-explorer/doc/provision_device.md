# Connecting your device to an IoT hub

IoT Hub only allows connections from known devices that present valid credentials. This document describes how to use the *iothub-explorer* tool to provision a device for use in IoT Hub.  While IoT Hub supports multiple authentication schemes, the iothub-explorer tool generates a pre-shared key.

 > Note: You must have an IoT hub running in Azure before you can provision your device. The document [Setup IoT Hub][setup-iothub] describes how to set up an IoT hub.

## Provision the device in your IoT hub

The iothub-explorer tool enables you to provision devices in your IoT hub. It runs on any computer where Node.js is available. For your convininence the tool is also published to  npm, preventing you from having to clone the repository or download the code. To provision a new device:

1. Get the connection string for your IoT Hub. See [Setup IoT Hub][setup-iothub] for more details.
2. Run the following commands in a terminal/shell window on your machine after making sure [node](http://nodejs.org) is installed. When running the commands, use the IoT Hub connection string from the previous step and replace "mydevice" with your desired device name.

```
npm install -g iothub-explorer
iothub-explorer <iothub-connection-string> create mydevice --connection-string
```

You should see a response like this:

```
Created device mydevice

-
...
-
  connectionString: HostName=<hostname>;DeviceId=mydevice;SharedAccessKey=<device-key>
```

Copy the device connection string information for later use. The samples in this repository use connection strings in the format `HostName=hub-name.azure-devices.net;DeviceId=device-id;SharedAccessKey=key`.

## Provision multiple devices in your IoT hub

The iothub-explorer tool also enables you to provision multiple new devices at once. To provision multiple new devices:

1. Create a text file and place the device id of each device you want to provision on a new line within the text file.
2. Get the connection string for your IoT Hub. See [Setup IoT Hub][setup-iothub] for more details.
3. Run the following commands in a terminal/shell window from the tools/iothub-explorer folder in the repository using the IoT Hub connection string from the previous step and replacing "path-to-file" with the path to the file created in step 1.

```
npm install
node iothub-explorer.js <iothub-connection-string> import <path-to-file> --connection-string
```

For each device created, you should see a response like this:

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
iothub-explorer --help
```

> Note: Windows users also have the option of using the [Device Explorer][device-explorer] application to provision devices.

[setup-iothub]: ../../doc/setup_iothub.md
[device-explorer]: ../../DeviceExplorer/doc/how_to_use_device_explorer.md
