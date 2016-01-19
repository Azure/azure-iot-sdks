#iothub-explorer
A sample CLI to manage devices using the Azure IoT Hub service SDK.

[![npm version](https://badge.fury.io/js/iothub-explorer.svg)](https://badge.fury.io/js/iothub-explorer)

## Install

`npm install -g iothub-explorer@latest` to get the latest (pre-release) version.

## Getting Started

Supply your IoT Hub connection string once via `login`:

```shell
$ iothub-explorer login "HostName=<my-hub>.azure-devices.net;SharedAccessKeyName=<my-policy>;SharedAccessKey=<my-policy-key>"

Session started, expires Fri Jan 15 2016 17:00:00 GMT-0800 (Pacific Standard Time)
```

Get information about an already-registered device, including the device connection string:

```shell
$ iothub-explorer get known-device --connection-string

-
  deviceId:                   known-device
  ...
-
  connectionString: HostName=<my-hub>.azure-devices.net;DeviceId=known-device;SharedAccessKey=<known-device-key>
```

Register a new device, and get its connection string:

```shell
$ iothub-explorer create new-device --connection-string

Created device new-device

-
  deviceId:                   new-device
  ...
-
  connectionString: HostName=<my-hub>.azure-devices.net;DeviceId=new-device;SharedAccessKey=<new-device-key>
```

To find out more about all supported commands:

```shell
$ iothub-explorer help
```