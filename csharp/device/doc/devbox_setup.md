# Prepare your development environment

This document describes how to prepare your development environment to build and use the **Microsoft .NET Device SDK for Azure IoT Devices**

- [Setting up a Windows development environment](#windows)
- [Azure SDK for .NET](#azuresdk)
- [Directly using .NET SDK using NuGet packages](#directly_using_sdk)
- [Locally building .NET SDK](#building_sdk)
- [Application samples](#samplecode)

<a name="windows"/>
## Setting up a Windows development environment

- Install [Visual Studio 2015][visual-studio]. You can use the **Visual Studio Community** Free download if you meet the licensing requirements.

Be sure to include [NuGet Package Manager][NuGet-Package-Manager].

<a name="azuresdk"/>
## Azure SDK for .NET

Install the Azure SDK for .NET 2.7 or later. Use the following links to download the SDK installer for your version of Visual Studio:

- [VS 2015][lnk-sdk-vs2015]
- [VS 2013][lnk-sdk-vs2013]
- [VS 2012][lnk-sdk-vs2012]

<a name="directly_using_sdk"/>
## Directly using .NET SDK using NuGet packages

If you are creating a client application from the scratch, use the [NuGet][NuGet] Package Manager to install the latest version of **Microsoft.Azure.Devices.Client** Device SDK for Azure IoT Devices NuGet package to your project.

- The package is located at [Azure IoT Device SDK NuGet Package] [lnk-NuGet-package]. 

<a name="building_sdk"/>
## Building the .NET SDK locally
In some cases, you may want to build the .NET SDK libraries locally for development and testing purpose. For example, you may want to build the latest code available on the develop branch which follows the Continuous Integration (CI) approach.
To pull latest code on **develop** branch you can use following command

`git clone -b develop --recursive https://github.com/Azure/azure-iot-sdks.git azure-iot-develop`

To clone the **master** use following command or simply download the **.zip** from [Azure IoT SDK][lnk-azure-iot] 
```
git clone https://github.com/Azure/azure-iot-sdks.git
```

<a name="samplecode"/>
## Sample applications

This repository contains various .NET sample applications that illustrate how to use the Microsoft Azure IoT device SDK for .NET. For more information, see the [readme][readme].

## Building for iOS and Android using C#  
For building for iOS and Android using C#, you need to install [Xamarin][lnk-visualstudio-xamarin] for VS 2015

For building iOS sample, open **csharp\device\samples\DeviceClientSampleiOS** project file in VS 2015. You will be prompted to install Xamarin to build native iOS apps in C# if tool is not installed. Install the tool. You can test the sample on iPhone simulator running on your Mac.

For building Android sample open **csharp\device\samples\DeviceClientSampleAndroid** VS project file in VS 2015. You will be prompted to install Xamarin to build native Android apps in C# if tool is not installed. Install the tool. You can test the sample on Android emulator running on you host PC.

To check for any latest Xamarin update for Visual Studio check Tools->Options->Xamarin->Other.

## Running CppUWPSample (Universal Windows) sample application

Visual Studio is having an issue with getting all the depedencies (dll) of the Microsoft.Azure.Devices.Client.WinRT which is referenced in the project to the proper location.  This leads to an exception thrown about not loading file or assembly during run-time.  You will have to manually copy the dlls in the appropriate location in order to run the sample application successfully.  Follow the below steps if needed:
* Build the UWPSample (Universal Windows) project
* Note the location of the bin folder, i.e. ...\azure-iot-sdks\csharp\device\samples\UWPSample\bin\x86\Debug\
* Build the CppUWPSample (Universal Windows) project
* Note the location of the Appx folder, i.e. ...\azure-iot-sdks\csharp\device\Debug\CppUWPSample\Appx
* Copy all dll files from the UWPSample bin folder to the CppUWPSample Appx folder.  When prompt about duplicated file names, choose "Skip these files"

Now you are ready to run the CppUWPSample.



[visual-studio]: https://www.visualstudio.com/
[readme]: ../readme.md
[lnk-sdk-vs2015]: http://go.microsoft.com/fwlink/?LinkId=518003
[lnk-sdk-vs2013]: http://go.microsoft.com/fwlink/?LinkId=323510
[lnk-sdk-vs2012]: http://go.microsoft.com/fwlink/?LinkId=323511
[lnk-visualstudio-xamarin]: https://msdn.microsoft.com/en-us/library/mt299001.aspx
[lnk-NuGet-package]:https://www.nuget.org/packages/Microsoft.Azure.Devices.Client
[lnk-azure-iot]:https://github.com/Azure/azure-iot-sdks
[NuGet-Package-Manager]:https://visualstudiogallery.msdn.microsoft.com/5d345edc-2e2d-4a9c-b73b-d53956dc458d
[NuGet]:https://www.nuget.org/

