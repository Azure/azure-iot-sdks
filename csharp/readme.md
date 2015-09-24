# Microsoft Azure IoT device SDK for .NET

The Microsoft Azure IoT device SDK for .NET facilitates building devices and applications that connect to and are managed by Azure IoT Hub services.

The library is available as a NuGet package for you include in your own development projects. This repository contains documentation and samples to help you get started using the SDK.

## Features

 * Sends event data to Azure IoT based services.
 * Maps server commands to device functions.
 * Buffers data when network connection is down.
 * Batches messages to improve communication efficiency.
 * Supports pluggable transport protocols. Both HTTPS and AMQP protocols are available now for .NET. For the UWP, only HTTPS is available at this time.


## Folder structure of repository

All the .NET specific resources are located in the **csharp** folder.

### /build

This folder contains build scripts for the .NET client libraries and samples.

### /doc

This folder contains setup and getting started documents for .NET.

- [Preparing your Windows development environment][devbox-setup]
- [Getting started - running a C# sample application on Windows][getstarted-windows]

Other useful documents include:

- [Setup IoT Hub][setup-iothub]
- [How to use Device Explorer][device-explorer]
- [Microsoft Azure IoT device SDK FAQ][faq-doc]
- [Microsoft Azure IoT device SDK for .NET API reference][dotnet-api-ref]

### /Microsoft.Azure.Devices.Client and /Microsoft.Azure.Devices.Client.WinRT

These folders contain the .NET client library source code.

The Microsoft.Azure.Devices.Client.WinRT project is for building the UWP (Universal Windows Platform) version of the client library. For more information about the UWP version refer to the [FAQ][faq-doc].

These projects are useful if you want to modify or extend the .NET libraries.

### /NuGet

This folder contains scripts for building the NuGet package that contains the library components.

### /samples

This folder contains various .NET samples that illustrate how to use the client library.

### /iothub_csharp_client.sln

This Visual Studio solution contains the client library and sample projects.

[setup-iothub]: ../doc/setup_iothub.md
[devbox-setup]: doc/devbox_setup.md
[getstarted-windows]: doc/run_sample_on_Windows.md
[device-explorer]: ../tools/DeviceExplorer/doc/how_to_use_device_explorer.md
[faq-doc]: ../doc/faq.md
[dotnet-api-ref]: TBD
