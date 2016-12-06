# Microsoft Azure IoT device SDK for .NET

The Microsoft Azure IoT device SDK for .NET facilitates building devices and applications that connect to and are managed by Azure IoT Hub services.

The library is available as a NuGet package for you include in your own development projects. This repository contains documentation and samples to help you get started using this SDK.

## Features

 * Sends event data to Azure IoT based services.
 * Maps server commands to device functions.
 * Batches messages to improve communication efficiency.
 * Supports pluggable transport protocols.

> Note: Currently, **Microsoft.Azure.Devices.Client.WinRT** doesn't support  **MQTT** protocol.

For example,calling `DeviceClient deviceClient = DeviceClient.CreateFromConnectionString(DeviceConnectionString, TransportType.Mqtt);` will result in "Mqtt protocol is not supported" exception.

> Note : Currently, **Microsoft.Azure.Devices.Client.PCL** only supports **HTTPS** protocol.

For example, calling `DeviceClient deviceClient = DeviceClient.CreateFromConnectionString(DeviceConnectionString, TransportType.Amqp);` will result in "Amqp protocol is not supported" exception.


## Application development guidelines
For more information on how to use this library refer to the documents below:
- [Preparing your Windows development environment][devbox-setup]
- [Running the C# samples on Windows][run-sample-on-desktop-windows]
- [Running the C# samples on Windows IoT Core][run-sample-on-windows-iot-core]

### Using client SDK in [Azure Functions] [https://azure.microsoft.com/en-us/services/functions/]
Azure Function doesn't current support [bindingredirect][https://msdn.microsoft.com/en-us/library/eftw1fys(v=vs.110).aspx] element
Example
<bindingRedirect oldVersion="0.0.0.0-2.2.0.0" newVersion="2.2.0.0" />
So workaround would be directly install older version of validation.dll

Use following in your project.json. This will install latest version of the available Azure IoT SDK assemblies but will install older version of validation.dll
**Example**

```
{
"frameworks": {
    "net46":{
        "dependencies": {
            "Microsoft.Azure.Devices.Client": "1.1.1",
             "Microsoft.Azure.Devices.Shared": "1.0.2",
             "Validation": "2.0.6.15003"
              }
             }
        }
}
```
This is covered in more detail under #978 [GitHub issue #978][https://github.com/Azure/azure-iot-sdks/issues/978]




Other useful documents include:
- [Setup IoT Hub][setup-iothub]
- [How to use Device Explorer][device-explorer]
- [Microsoft Azure IoT device SDK FAQ][faq-doc]
- [Microsoft Azure IoT device SDK for .NET API reference][dotnet-api-ref]


## Folder structure of repository

All the .NET device specific resources are located in the **csharp** folder.

### /build

This folder contains build scripts for the .NET client libraries and samples.

### /doc

This folder contains setup and getting started documents for .NET.

### /Microsoft.Azure.Devices.Client  /Microsoft.Azure.Devices.Client.WinRT

These folders contain the .NET client library source code.

The Microsoft.Azure.Devices.Client.WinRT project is for building the UWP (Universal Windows Platform) version of the client library. For more information about the UWP version of this library refer to the [FAQ][faq-doc].

These projects are useful if you want to modify or extend the .NET libraries.

### /NuGet

This folder contains scripts for building the NuGet package that contains the library components.

### /samples

This folder contains various .NET samples that illustrate how to use the client library.

### /iothub_csharp_client.sln

This Visual Studio solution contains the client library and sample projects.

## API reference

API reference documentation can be found online at https://msdn.microsoft.com/library/microsoft.azure.devices.aspx.

[setup-iothub]: ../../doc/setup_iothub.md
[devbox-setup]: doc/devbox_setup.md
[run-sample-on-desktop-windows]: ../../doc/get_started/windows-desktop-csharp.md
[run-sample-on-windows-iot-core]: ../../doc/get_started/windows10-iotcore-csharp.md
[device-explorer]: ../../tools/DeviceExplorer/readme.md
[faq-doc]: ../../doc/faq.md
[dotnet-api-ref]: https://msdn.microsoft.com/library/microsoft.azure.devices.aspx
