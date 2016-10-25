#iothub-explorer
A CLI tool to manage device identities in your IoT hub registry, send and receive messages and files from your devices, and monitor your IoT hub operations.

**iothub-explorer** also lets you simulate a device connected to your IoT hub.

[![npm version](https://badge.fury.io/js/iothub-explorer.svg)](https://badge.fury.io/js/iothub-explorer)

<a name="install"/>
## Installing iothub-explorer

> Note: This tool requires Node.js version 4.x or higher for all features to work.

To install the latest version of the **iothub-explorer** tool, run the following command in your command-line environment:

```shell
npm install -g iothub-explorer
```

You can use the following command to get additional help about all the **iothub-explorer** commands:

```shell
$ iothub-explorer help

  Usage: iothub-explorer [options] <command> [command-options] [command-args]


  Commands:

    login                           start a session on your IoT hub
    logout                          terminate the current session on your IoT hub
    list                            list the device identities currently in your IoT hub device registry
    create <device-id|device-json>  create a device identity in your IoT hub device registry
    delete <device-id>              delete a device identity from your IoT hub device registry
    get <device-id>                 get a device identity from your IoT hub device registry
    import-devices                  import device identities in bulk: local file -> Azure blob storage -> IoT hub
    export-devices                  export device identities in bulk: IoT hub -> Azure blob storage -> local file
    send <device-id> <message>      send a message to the device (cloud-to-device/C2D)
    monitor-feedback                monitor feedback sent by devices to acknowledge cloud-to-device (C2D) messages
    monitor-events [device-id]      listen to events coming from devices (or one in particular)
    monitor-uploads                 monitor the file upload notifications endpoint
    monitor-ops                     listen to the operations monitoring endpoint of your IoT hub instance
    sas-token <device-id>           generate a SAS Token for the given device
    simulate-device <device-id>     simulate a device with the specified id
    help [cmd]                      display help for [cmd]

  Options:

    -h, --help     output usage information
    -V, --version  output the version number
```

Each **iothub-explorer** command has its own help that can be viewed by passing `help` the command name:

```shell
$ iothub-explorer help create

  Usage: iothub-explorer-create [options] [device-id|device-json]

  Create a device identity in your IoT hub device registry, either using the specified device id or JSON description.

  Options:

    -h, --help                       output usage information
    -a, --auto                       create a device with an auto-generated device id
    -cs, --connection-string         show the connection string for the newly created device
    -d, --display <property-filter>  comma-separated list of device properties that should be displayed
    -l, --login <connection-string>  connection string to use to authenticate with your IoT Hub instance
    -k1, --key1 <key>                specify the primary key for newly created device
    -k2, --key2 <key>                specify the secondary key for newly created device
    -r, --raw                        use this flag to return raw JSON instead of pretty-printed output
    -x, --x509                       generate an x509 certificate to authenticate the device
    -dv, --daysValid                 number of days the x509 certificate should be valid for
    -t1, --thumbprint1 <thumbprint>  specify the primary thumbprint of the x509 certificate
    -t2, --thumbprint2 <thumbprint>  specify the secondary thumbprint of the x509 certificate
```

## Examples
<a name="identityregistry"/>
### Working with the device identity registry

Supply your IoT hub connection string once using the **login** command. This means you do not need to supply the connection string for subsequent commands for the duration of the session (defaults to one hour):

```shell
$ iothub-explorer login "HostName=<my-hub>.azure-devices.net;SharedAccessKeyName=<my-policy>;SharedAccessKey=<my-policy-key>"

Session started, expires Fri Jan 15 2016 17:00:00 GMT-0800 (Pacific Standard Time)
```

> Note: See [Set up IoT Hub](../../doc/setup_iothub.md) for information about how to retrieve your IoT hub connection string.

To retrieve information about an already-registered device from the device identity registry in your IoT hub, including the device connection string, use the following command:

```shell
$ iothub-explorer get known-device --connection-string

-
  deviceId:                   known-device
  ...
-
  connectionString: HostName=<my-hub>.azure-devices.net;DeviceId=known-device;SharedAccessKey=<known-device-key>
```

To register a new device and get its connection string from the device identity registry in your IoT hub, use the following command:

```shell
$ iothub-explorer create new-device --connection-string

Created device new-device

-
  deviceId:                   new-device
  ...
-
  connectionString: HostName=<my-hub>.azure-devices.net;DeviceId=new-device;SharedAccessKey=<new-device-key>
```

To delete a device from the device identity registry in your IoT hub, use the following command:

```shell
$ iothub-explorer delete existing-device
```

<a name="devices"/>
### Working with devices

Use the following command to monitor the device-to-cloud messages from a device:

```shell
$ iothub-explorer monitor-events myFirstDevice --login 'HostName=<my-hub>.azure-devices.net;SharedAccessKeyName=<my-policy>;SharedAccessKey=<my-policy-key>'

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

$ iothub-explorer monitor-feedback myFirstDevice

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
