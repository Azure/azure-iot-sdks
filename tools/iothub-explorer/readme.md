#iothub-explorer
A sample CLI tool to manage IoT devices using the Azure IoT Hub service SDK. The tool enables you to manage the devices in the identity registry, monitor device-to-cloud messages, and send cloud-to-device commands. You can also use the tool to schedule jobs on the device (e.g., firmware update).

[![npm version](https://badge.fury.io/js/iothub-explorer.svg)](https://badge.fury.io/js/iothub-explorer)

<a name="install"/>
## Installing iothub-explorer

> Note: This tool requires Node.js version 4.x or higher for all features to work.

To install the *dmpreview* version of the *iothub-explorer* tool, run the following command in your command-line environment:

```
$ npm install -g iothub-explorer@dmpreview
```

You can use the following command to get additional help about all the iothub-explorer commands and their parameters:

```shell
$ iothub-explorer help
```

<a name="identityregistry"/>
## Working with the device identity registry

Supply your IoT Hub connection string once using the **login** command. This means you do not need to supply the connection string for subsequent commands for the duration of the session (defaults to one hour):

```shell
$ iothub-explorer login "HostName=<my-hub>.azure-devices.net;SharedAccessKeyName=<my-policy>;SharedAccessKey=<my-policy-key>"

Session started, expires Fri Jan 15 2016 17:00:00 GMT-0800 (Pacific Standard Time)
```

> Note: See [Set up IoT Hub](../../doc/setup_iothub.md) for information about how to retrieve your IoT Hub connection string.

To retrieve information about an already-registered device from the device identity registry in your IoT Hub, including the device connection string, use the following command:

```shell
$ iothub-explorer get known-device --connection-string

-
  deviceId:                   known-device
  ...
-
  connectionString: HostName=<my-hub>.azure-devices.net;DeviceId=known-device;SharedAccessKey=<known-device-key>
```

To register a new device and get its connection string from the device identity registry in your IoT Hub, use the following command:

```shell
$ iothub-explorer create new-device --connection-string

Created device new-device

-
  deviceId:                   new-device
  ...
-
  connectionString: HostName=<my-hub>.azure-devices.net;DeviceId=new-device;SharedAccessKey=<new-device-key>
```

To delete a device from the device identity registry in your IoT Hub, use the following command:

```shell
$ iothub-explorer delete existing-device
```

<a name="devices"/>
## Working with devices

Use the following command to monitor the device-to-cloud messages from a device:

```shell
$ iothub-explorer 'HostName=<my-hub>.azure-devices.net;SharedAccessKeyName=<my-policy>;SharedAccessKey=<my-policy-key>' monitor-events myFirstDevice              

Monitoring events from device myFirstDevice
Listening on endpoint iothub-ehub-<my-endpoint>/ConsumerGroups/$Default/Partitions/0 start time: 1453821103646
Listening on endpoint iothub-ehub-<my-endpoint>/ConsumerGroups/$Default/Partitions/1 start time: 1453821103646
Event received: 
{ deviceId: 'myFirstDevice', windSpeed: 10.92403794825077 }

Event received: 
{ deviceId: 'myFirstDevice', windSpeed: 10.671534826979041 }

Event received: 
{ deviceId: 'myFirstDevice', windSpeed: 13.557703581638634 }

Event received: 
{ deviceId: 'myFirstDevice', windSpeed: 11.123057782649994 }
```

> Note: The **monitor-events** command requires you to provide the IoT Hub connection string even if you have created a session using the **login** command.

Use the following commands to send a cloud-to-device command and then wait for the device to respond with an acknowledgment:

```shell
$ iothub-explorer send myFirstDevice MyMessage --ack=full

Message sent (id: 13bc1852-eeb4-4611-b0bd-329d80c83f6f)

$ iothub-explorer receive myFirstDevice

Waiting for feedback... (Ctrl-C to quit)

Feedback message
iothub-enqueuedtime: Tue Jan 26 2016 15:51:50 GMT+0000 (GMT Standard Time)
body:
  -
    originalMessageId:  13bc1852-eeb4-4611-b0bd-329d80c83f6f
    description:        Success
    deviceGenerationId: 635875186443339004
    deviceId:           myFirstDevice
    enqueuedTimeUtc:    2016-01-26T15:51:03.4197888Z
```
<a name="devicemanagement"/>
## Working with Device Management

The Azure IoT Hub device management client library enables you to manage your IoT devices with Azure IoT Hub. “Manage” includes actions such as rebooting, factory resetting, and updating firmware. Today, we provide a platform-independent C library, but we will add support for other languages soon.

For greater detail, please reference the following blog:

[Introducing the Azure IoT Hub device management (DM) client library](https://azure.microsoft.com/en-us/documentation/articles/iot-hub-device-management-library/)

###Commands:
The device management commands offered by `iothub-explorer` are as follows:

- `iothub-explorer [<connection-string>] get-job <job-id>`
  - Displays information about the given job.

- `[<connection-string>] read <device-ids> <device-properties> [--async]`
  - Reads and displays properties, given as a comma-delimted list of names, from one or more devices (aka "deep read").

- `[<connection-string>] write <device-ids> <device-properties> [--async]`
  - Writes properties, given as a JSON object, to one or more devices (aka "deep write").

- `[<connection-string>] firmware-update <device-ids> <firmware-uri> [--async] [--timeout=<num-minutes>]`
  - Issues a command to one or more devices to update their firmware to the specified image.
  - The job will fail if it does not complete within the given timeout period; default timeout is one hour.

- `[<connection-string>] factory-reset <device-ids> [--async]`
  - Issues a command to one or more devices to reset their firmware to the factory default image.

- `[<connection-string>] {green}reboot{/green} <device-ids> [--async]`
  - Issues a command to one or more devices to reboot.

> NOTE:

> - `<device-ids>` are comma-delimited.

> - Use the `--async` option to display the job ID and return immediately (the job ID can be given to `get-job`).
