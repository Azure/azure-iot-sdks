# Prepare your development environment

This document describes how to prepare your development environment to build and use the **Microsoft .NET Device SDK for Azure IoT Devices**

1.  [Setting up a Windows development environment](#windows)
2.  [Azure SDK for .NET](#azuresdk)
3.  [Directly using IoT Device Client SDK using NuGet packages](#directly_using_sdk)
4.  [Locally building IoT Device Client SDK](#building_sdk)
5.  [Application samples](#samplecode)

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
## Directly using Azure IoT Device Client SDK using NuGet packages
Go to VS 2015 Solution Explorer and right click on the solution or project and click Manage NuGet Packages.There are 2 different **NuGet** packages to choose from

### 1.) Microsoft.Azure.Devices.Client
For bulding classic desktop .NET application, use NuGet Package Manager to install latest version of **Microsoft.Azure.Devices.Client** Device SDK for Azure IoT Devices NuGet package to your project.

- Microsoft.Azure.Devices.Client.dll is a .NET assembly for building classic .NET desktop application.
- Microsoft.Azure.Devices.Client.winmd (Windows Runtime Metadata) is an assembly for building UWP application.

The package is located at [Azure IoT Device Client SDK NuGet Package] [lnk-NuGet-package]


### 2.) Microsoft.Azure.Devices.Client.PCL
[PCL] [PCL] stands for Portable Class Library. This version of .NET library helps you to build cross-platform apps.
For example,for bulding iOS and Android application in C# in VS 2015, use NuGet Package Manager to install latest version of **Microsoft.Azure.Devices.Client.PCL** Device SDK for Azure IoT Devices NuGet package to your project.

- The package is located at [Azure IoT Device Client PCL SDK NuGet Package] [lnk-NuGet-package_pcl]


### Building UWP Apps 
Microsoft.Azure.Devices.Client.winmd is WinRT version which will installed via NuGet Package manager versus Microsoft.Azure.Devices.Client.dll

#### For Building C# and Visual Basic UWP apps
For building UWP, follow the same steps as you would follow if building classic .NET desktop application.

Visual Basic .NET call would be something like this
``
Dim deviceClient As Microsoft.Azure.Devices.Client.DeviceClient = Microsoft.Azure.Devices.Client.DeviceClient.CreateFromConnectionString("{My device connection string}", Microsoft.Azure.Devices.Client.TransportType.Amqp)
Dim myMessage = "Hello!" + DateTime.Now.ToString()
Dim message = New Microsoft.Azure.Devices.Client.Message(Encoding.ASCII.GetBytes(myMessage))
Dim Async = deviceClient.SendEventAsync(message)
``



#### For Building C++ and JavaScript UWP apps
For Visual C++ and JavaScript UWP, there may get "Error	Failed to add reference to 'Microsoft.Azure.Amqp.Uwp" error while installing NuGet Package. 

Use the below workaround to fix the error.Since the assemblies fail to install, you need to manually copy them for C++ or JavaScript UWP application.

- Workaround for building C++ and JavaScript UWP apps

Create a temporary blank UWP project in C#
Right click on Project and click Manage NuGet Packages and install Microsoft.Azure.Devices.Client NuGet package
Build the project
Go the corresponding bin project. For example, for Debug build you will go to bin\x64\Debug and copy following 4 files
1.) Microsoft.Azure.Devices.Client.winmd
2.) Microsoft.Azure.Amqp.Uwp.dll
3.) PCLCrypto.dll
4.) Validation.dll

There are the files you need to copy in the bin\x64\Debug\AppX folder of your AppX package.
To add the reference, you reference WinRT assembly  
\bin\x64\Debug\Microsoft.Azure.Devices.Client.winmd

For JavaScript call would something like this.
`
var azureClient = Microsoft.Azure.Devices.Client;
var deviceClient = azureClient.DeviceClient.createFromConnectionString(deviceConnectionString, azureClient.TransportType.amqp);
deviceClient.sendEventAsync(eventMessage);

`

- 2 Microsoft.Azure.Devices.Client.PCL
This is Portable Class library. You may want to use this for Xamarin C# cross-compatible project on Xamarin and iOS.
It currently only supports HTTPS protocol.



<a name="building_sdk"/>
## Building the Azure IoT Device Client .NET/PCL SDK locally
In some cases, you may want to build the .NET SDK libraries **locally** for development and testing purpose. For example, you may want to build the **latest** code available on the develop branch which follows the Continuous Integration (CI) approach and step through the client library code while debugging.

To pull latest code on **develop** branch you can use following command

`git clone -b develop --recursive https://github.com/Azure/azure-iot-sdks.git azure-iot-develop`

To clone the **master** use following command or simply download the **.zip** from [Azure IoT SDK][lnk-azure-iot] 

`git clone https://github.com/Azure/azure-iot-sdks.git`

The following instructions outline how you can build the C# SDK (along with samples). You can either build using command line or you can build using VS 2015 IDE

### Option 1:  Build using command line
1. Open a Developer Command Prompt for VS2015.
2. Go to scripts folder located at **csharp\\device\\build**. Run the build script `build` which will build the SDK and the samples using default option.

Use 'build --options' to list various build options.
For example to build for x64 as debug build, you can use

`build --platform x64 --config Debug`
You will be prompted to download and run nuget.exe for downloading packages that Azure IoT SDK has dependency on.
Once build completes, it will create

1. Microsoft.Azure.Devices.Client.dll (Client SDK Assembly)
2. Microsoft.Azure.Devices.Client.PCL.dll (PCL library)
3. Microsoft.Azure.Devices.Client.winmd (WinRT version that you will need to UWP application)
4. Sample executables 

### Option 2: Build using VS 2015 IDE
1. Open iothub_csharp_deviceclient.sln file in VS 2015 IDE.
2. Select the configuration that you want and press Build->Build Solution command.
Just like command line option it will create Device Client SDK along with the samples.

<a name="samplecode"/>
## Sample applications

This repository contains various .NET sample applications that illustrate how to use the Microsoft Azure IoT device SDK for .NET. For more information, see the [readme][readme].

## Building for iOS and Android using C#  
For building for iOS and Android using C#, you need to install [Xamarin][lnk-visualstudio-xamarin] for VS 2015

For building iOS sample, open **csharp\device\samples\DeviceClientSampleiOS** project file in VS 2015. You will be prompted to install Xamarin to build native iOS apps in C# if tool is not installed. Install the tool. You can test the sample on iPhone simulator running on your Mac.

For building Android sample open **csharp\device\samples\DeviceClientSampleAndroid** VS project file in VS 2015. You will be prompted to install Xamarin to build native Android apps in C# if tool is not installed. Install the tool. You can test the sample on Android emulator running on you host PC.

To check for any latest Xamarin update for Visual Studio check Tools->Options->Xamarin->Other.

## Running CppUWPSample (Universal Windows) sample application

Visual Studio is having an issue with getting all the dependencies (dll) of the Microsoft.Azure.Devices.Client.WinRT which is referenced in the project to the proper location.  This leads to an exception thrown about not loading file or assembly during run-time.  You will have to manually copy the dlls in the appropriate location in order to run the sample application successfully.  Follow the below steps if needed:
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
[lnk-NuGet-package_pcl]:https://www.nuget.org/packages/Microsoft.Azure.Devices.Client.PCL
[lnk-azure-iot]:https://github.com/Azure/azure-iot-sdks
[NuGet-Package-Manager]:https://visualstudiogallery.msdn.microsoft.com/5d345edc-2e2d-4a9c-b73b-d53956dc458d
[NuGet]:https://www.nuget.org/
[PCL]:https://msdn.microsoft.com/en-us/library/gg597391(v=vs.110).aspx

