# Prepare your development environment

This document describes how to prepare your development environment to use the *Microsoft Azure IoT device SDK for C#*.

- [Visual Studio](#vs)
- [Azure SDK for .NET](#azuresdk)
- [NuGet package](#prerequisites)
- [Application samples](#samplecode)

<a name="vs"/>
## Visual Studio

Install [Visual Studio 2015][visual-studio]. You can use any version of Visual Studio 2015, including the Community edition.

<a name="azuresdk"/>
## Azure SDK for .NET

Install the Azure SDK for .NET 2.7 or later. Use the following links to download the SDK installer for your version of Visual Studio:

- [VS 2015][lnk-sdk-vs2015]
- [VS 2013][lnk-sdk-vs2013]
- [VS 2012][lnk-sdk-vs2012]

<a name="prerequisites"/>
## NuGet package

If you are creating a client application from scratch, use the Visual Studio package manager to add the latest version of the **Microsoft.Azure.Devices.Client** NuGet package to your project.

<a name="samplecode"/>
## Sample applications

This repository contains various .NET sample applications that illustrate how to use the Microsoft Azure IoT device SDK for .NET. For more information, see the [readme][readme].

## Building for iOS and Android using C#  
For building for iOS and Android using C#, you need to install [Xamarin][lnk-visualstudio-xamarin] for VS 2015

For building iOS sample, open **csharp\device\samples\DeviceClientSampleiOS** project file in VS 2015. You will be prompted to install Xamarin to build native iOS apps in C# if tool is not installed. Install the tool. You can test the sample on iPhone simulator running on your Mac.

For building Android sample open **csharp\device\samples\DeviceClientSampleAndroid** VS project file in VS 2015. You will be prompted to install Xamarin to build native Android apps in C# if tool is not installed. Install the tool. You can test the sample on Android emulator running on you host PC.

To check for any latest Xamarin update for Visual Studio check Tools->Options->Xamarin->Other.

[visual-studio]: https://www.visualstudio.com/
[readme]: ../readme.md
[lnk-sdk-vs2015]: http://go.microsoft.com/fwlink/?LinkId=518003
[lnk-sdk-vs2013]: http://go.microsoft.com/fwlink/?LinkId=323510
[lnk-sdk-vs2012]: http://go.microsoft.com/fwlink/?LinkId=323511
[lnk-visualstudio-xamarin]: https://msdn.microsoft.com/en-us/library/mt299001.aspx

