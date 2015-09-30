# Microsoft Azure IoT device SDK for .NET

The Microsoft Azure IoT device SDK for .NET facilitates building devices and applications that connect to and are managed by Azure IoT Hub services.

The library is available as a NuGet package for you include in your own development projects. This repository contains documentation and samples to help you get started using this SDK.

## Features

 * Sends event data to Azure IoT based services.
 * Maps server commands to device functions.
 * Batches messages to improve communication efficiency.
 * Supports pluggable transport protocols.

Note the library Microsoft.Azure.Devices.Client supports both HTTPS and AMQP protocols. The Microsoft.Azure.Devices.Client.WinRT library currently supports HTTPS at this time.


## Application development guidelines
For more information on how to use this library refer to the documents below:
- [Preparing your Windows development environment][devbox-setup]
- [Running the C# samples on Windows][run-sample-on-desktop-windows]
- [Running the C# samples on Windows IoT Core][run-sample-on-windows-iot-core]

Other useful documents include:
- [Setup IoT Hub][setup-iothub]
- [How to use Device Explorer][device-explorer]
- [Microsoft Azure IoT device SDK FAQ][faq-doc]
- [Microsoft Azure IoT device SDK for .NET API reference][dotnet-api-ref]


## Folder structure of repository

All the .NET specific resources are located in the **csharp** folder.

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

[setup-iothub]: ../doc/setup_iothub.md
[devbox-setup]: doc/devbox_setup.md
[run-sample-on-desktop-windows]: doc/run_sample_on_desktop_windows.md
[run-sample-on-windows-iot-core]: doc/run_sample_on_windows_iot_core.md
[device-explorer]: ../tools/DeviceExplorer/doc/how_to_use_device_explorer.md
[faq-doc]: ../doc/faq.md
[dotnet-api-ref]: https://msdn.microsoft.com/library/microsoft.azure.devices.aspx
