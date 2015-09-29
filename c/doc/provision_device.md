# Connecting your device to an IoT hub

IoT Hub only allows connections from known devices that present proper credentials.  This document describes how to use the iothub-explorer tool to provision a device for use in IoT Hub.  While IoT Hub supports multiple authentication schemes, we will use pre-shared keys in our example.

 > Note: You must have an IoT hub running in Azure in order to provision your device.  If you don't have an IoT hub,
 [set one up now.][setup-iothub]

## Table of Contents

- [Provision the device in your IoT hub](#provision)

<a name="provision"/>
## Provision the device in your IoT hub

The iothub-explorer tool enables you to provision devices in your IoT hub.  It runs on any desktop computer where Node.js is available. To provision a new device:

1. [Get the connection string for your IoT Hub][setup-iothub]
2. Run the following commands in a terminal/shell window from the tools/iothub-explorer folder, using the connection string you got in the previous step and replacing "mydevice" with your desired device name.

```
tools/iothub-explorer> npm install
tools/iothub-explorer> node iothub-explorer.js <iothub-connection-string> create mydevice --show="connection-string"
```

You should see a response like this:
```
Created device mydevice

HostName=<hostname>;DeviceId=<device-id>;SharedAccessKey=<device-key>
```
Copy the device connection string information for later use.

 > Note: Windows users also have the option of [using the Device Explorer application to provision devices][device-explorer].

[setup-iothub]: ../../doc/setup_iothub.md
[device-explorer]: ../../tools/DeviceExplorer/doc/how_to_use_device_explorer.md
