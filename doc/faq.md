# Microsoft Azure IoT Device SDK FAQ

This document contains both general FAQs about the Microsoft Azure IoT Device SDK as a whole, and specific FAQs about the C, .NET, Java, and Node.js SDKs in this repository.

## Microsoft Azure IoT Device SDK for .NET FAQs

- [UWP support for Microsoft.Azure.Devices.Client](#uwpsupport)

## Microsoft Azure IoT Device SDK for Java FAQs

- [Error when using AMQP on Raspberry Pi2](#javapi2error)

<a name="uwpsupport"/>
## UWP support for Microsoft.Azure.Devices.Client

- [Overview](#overview)
- [Project file and assembly](#project)
- [Device client](#deviceclient)
- [Asynchrony](#asynchrony)
- [Library-specific behaviors](#library)
- [Resources](#resources)

<a name="overview"/>
### Overview: Why UWP?

UWP (Universal Windows Platform) is an evolution of Windows app model introduced in Windows 8. UWP provides a common app platform available on every device that runs Windows 10, including its IoT flavor, the IoT Core. (See https://msdn.microsoft.com/en-us/library/dn894631.aspx). UWP is the official application model supported on IoT Core.

An existing .NET library can be made UWP-compatible by exposing WinRT interfaces and by porting the implementation to .NET Core (See http://blogs.msdn.com/b/dotnet/archive/2014/12/04/introducing-net-core.aspx)

WinRT imposes certain constraints on the public APIs. Most importantly, only WinRT (and not .NET) types can be exposed. This allows other languages (including unmanaged languages like C++/CX and JavaScript) to consume such libraries.

<a name="project"/>
### Project file and assembly

A new project file, Microsoft.Azure.Devices.Client.WinRT.csproj has been created. The project has been added to the main solution. The project produces a WinRT AppX package with metadata in Microsoft.Azure.Devices.Client.winmd.

The existing .NET library, Microsoft.Azure.Devices.Client.dll, has remained unchanged (modulo a small number of breaking changes as described below).

<a name="deviceclient"/>
### DeviceClient

WinRT does not allow exporting abstract classes, therefore **DeviceClient** has become a sealed concrete class. A new private abstract class, **DeviceClientImpl** has been added to maximally reuse the existing implementation.

<a name="asynchrony"/>
### Asynchrony

Only WinRT types can be exported by WinRT assemblies. The signatures of async methods such as **ReceiveAsync** have been changed to return **AsyncTask** or **AsyncTaskOfMessage**, which is aliased to **Windows.Foundation.IAsyncAction** or **Windows.Foundation.IAsyncOperation<Message>** for WinRT, or **System.Threading.Tasks.Task** and **System.Threading.Tasks.Task<Message>** for .NET.

<a name="library"/>
### Library-specific Behaviors

WINDOWS_UWP macro is used to differentiate between .NET and UWP behaviors at compile time. Currently, there are almost 200 occurrences of `#if WINDOWS_UWP` in the code.

<a name="resources"/>
#### Resources

Desktop apps use either text files or XML (.resx) files to create resources. The compiler then embeds them into the assembly. The resources are retrieved using the System.Resources.ResourceManager class.

UWP uses the Windows Store resource model that replaces the hub-and-spoke model common to .NET Framework desktop apps. In UWP apps, .resw files are used to create resources. The format of the file is the same as .resx, but the packaging mechanism is different. At compile time, all the .resw files for an app are packed into a single PRI file by the MakePRI utility and included with the app's deployment package. At run time, the **Windows.ApplicationModel.Resources.ResourceLoader** class and the types in the **Windows.ApplicationModel.Resources.Core** namespace provide access to app resources.

To support resources in Microsoft.Azure.Devices.Client library, the existing Resource.resx file has been copied to Resource.resw. The two files will now need to be kept in sync. Unlike in the .NET version of the library, the UWP version does not contain generated C# files. Instead, a new file, WinRTResources.cs is introduced. Whenever a new string is added to the .resx/.resw file, a corresponding entry must be copied from Resources.Designer.cs to WinRTResources.cs (follow the existing entries as an example)

<a name="javapi2error"/>
## Error when using AMQP on Raspberry Pi2
When building Qpid using Maven on a Raspberry Pi2 you might encounter a known error with the *SureFire* plugin:

```
Error occurred during initialization of VM Could not reserve enough space for object heap

Error: Could not create the Java Virtual Machine.
```
This results in a failure to build Qpid.  This is a known issue with OpenJDK7 (for more information, see http://laurenthinoul.com/how-to-solve-the-maven-vm-initialization-error/). This article recommends updating SureFire by updating your pom file. As an alternative, you can opt to skip the SureFire tests with the Maven build, using the following command:

```
mvn install -Dmaven.test.skip=true
```

For more information, see http://maven.apache.org/surefire/maven-surefire-plugin/examples/skipping-test.html.
