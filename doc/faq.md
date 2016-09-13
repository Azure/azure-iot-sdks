# Microsoft Azure IoT device SDK FAQ

This document contains both general FAQs about the Microsoft Azure IoT device SDK as a whole, and specific FAQs about the C, .NET, Java, and Node.js SDKs in this repository ([azure-iot-sdks](https://github.com/Azure/azure-iot-sdks)).

**Microsoft Azure IoT SDKs**

- [Using Visual Studio 2013](#vs2013)
- [Line-endings in repository zip archive](#lineendings)

**Microsoft Azure IoT device SDK for C FAQs**

- [Installing CMake manually](#cmake)
- [Using the IoT Hub c-client libraries in C++](#cpp)

**Microsoft Azure IoT device SDK for .NET FAQs**

- [UWP support for Microsoft.Azure.Devices.Client](#uwpsupport)
- [NotImplementedException thrown when using UWP](#notimpluwp)
- [IotHubCommunicationException or FileNotFoundException thrown when using HTTP protocol](#httpexception)

**Microsoft Azure IoT device SDK for Java FAQs**

- [Error when using AMQP on Raspberry Pi2](#javapi2error)
- [qpid-jms build fails](#qpidjmsbuildfail)

**Microsoft Azure IoT SDK for Node.js FAQs**

- [Using promises instead of callbacks with the device client](#nodepromisify)
- [Why not use Typescript instead of Javascript?](#whyunotypescript)

<a name="vs2013"/>
## Using Visual Studio 2013

The Visual Studio native C projects included in this repository ([azure-iot-sdks](https://github.com/Azure/azure-iot-sdks)) are Visual Studio 2015 projects. The following steps describe how to use Visual Studio 2013 if you are unable to install Visual Studio 2015 on your development machine.

Note: You can download the free Community edition of Visual Studio 2015 [here](https://www.visualstudio.com/en-us/downloads/download-visual-studio-vs.aspx).

1. Open the native C solution in Visual Studio 2013 (for example, azure_iot_sdks.sln in your home folder).
2. In **Solution Explorer** select all the projects in the solution. The right-click in **Solution Explorer** and click **Properties**.
3. Expand **Configuration Properties**, and then select **General**.
4. Change the **Platform Toolset** to **Visual Studio 2013 (v120)**, then click **OK**.

  ![][1]

<a name="lineendings"/>
## Line-endings in repository zip archive

If you download a zip archive of this repository to a Windows machine, you may encounter errors when you run some of the scripts. This is due to the way GitHub handles line-endings in zip archives. For more information, see http://stackoverflow.com/questions/17347611/downloading-a-zip-from-github-removes-newlines-from-text-files.

<a name="cmake"/>
## Installing CMake manually

If your Linux OS does not include CMake 3.0 and it is not possible to use a package installer, you can install CMake as follows:

```
wget https://cmake.org/files/v3.4/cmake-3.4.0-Linux-i386.sh
chmod u+x cmake-3.4.0-Linux-i386.sh
./cmake-3.4.0-Linux-i386.sh --help
Usage: ./cmake-3.4.0-Linux-i386.sh [options]
Options: [defaults in brackets after descriptions]
  --help            print this message
  --prefix=dir      directory in which to install
  --include-subdir  include the cmake-3.4.0-Linux-i386 subdirectory
  --exclude-subdir  exclude the cmake-3.4.0-Linux-i386 subdirectory
```

Make sure that the directory where you install CMake is on your path by exporting it. Alternatively, if you use the option `--prefix=/usr` when you install CMake it replaces your current installation.

<a name="cpp"/>
## Using the IoT Hub c-client libraries in C++

Using the IoT Hub c-client code from C++ is no different than using it from c. Create your C++ project, then reference the client library as you normally would in c++, or install the package via the appropriate package manager based on your platform.

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

<a name="notimpluwp"/>
## NotImplementedException thrown when using UWP

The UWP version of the .NET device libraries does not support the AMQP protocol. If you see a **NotImplementedException** thrown in a UWP application that uses the Azure IoT device SDK for .NET, then check that you are using the HTTPS protocol and not the AMQP protocol.

For example:

```
var deviceClient = DeviceClient.Create(iotHubUri, new DeviceAuthenticationWithRegistrySymmetricKey("myFirstDevice", deviceKey), TransportType.Http1);
```

<a name="httpexception"/>
## IotHubCommunicationException or FileNotFoundException thrown when using HTTP protocol

The **DeviceClient** class in the Microsoft.Azure.Devices.Client package requires the **System.Net.Http.Formatting** class to communicate with IoT Hub over HTTP.

You see an **IotHubCommunicationException** or **FileNotFoundException** exception when you try to use the HTTP protocol to send a device-to-cloud message:

```
DeviceClient deviceClient = DeviceClient.CreateFromConnectionString(DeviceConnectionString, TransportType.Http1);

...

await deviceClient.SendEventAsync(eventMessage);
```

To prevent these exceptions from happening, you should add **Microsoft.AspNet.WebApi.Client** NuGet package to your project.

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

<a name="qpidjmsbuildfail"/>
## qpid-jms build fails

If you get a build error when you try to [build qpid-jms](get_started/java-devbox-setup.md), then you should set the following  variable in your environment before you run `mvn install`.

Windows: `set _JAVA_OPTIONS=-Xmx512M`

Linux: `export _JAVA_OPTIONS=-Xmx512M`

[1]: media/PlatformToolset.png

<a name="nodepromisify"/>
## Using promises instead of callbacks with the device client
Currently the device client asynchronous functions follow the callback pattern rather than returning promises. If you wish to use the SDK with promises instead of callbacks, it's extremely easy though, using the bluebird library to "promisify" the device client class.

```javascript
var Promise = require('bluebird'); 
var client = Promise.promisifyAll(Client.fromConnectionString(connectionString, Amqp));
```

And there you have it. All the existing functions of the client still exist, and the promise-returning equivalent has been created and has the same name with `Async` appended to it. In other words:
- `client.open(callback)` becomes `client.openAsync().then(...).catch(...)`
- `client.send(message, callback)` becomes `client.sendAsync(message).then(...).catch(...)`
- `client.complete(message, callback)` becomes `client.completeAsync(message).then(...).catch(...)`
- etc.

Events are unchanged, so you still subscribe to those the way you would with the un-promisified client.

<a name="whyunotypescript" />
## Why not use TypeScript instead of JavaScript for the SDK?
At the time when the SDK development was started, pure JavaScript felt like a better choice in order to make contributions as easy as possible for any Node developer, whether or not he or she was aware and proficient with TypesSript. 
We regularly re-evaluate this decision as we move forward and you are most welcome to provide feedback or contribute by opening issues or pull-requests and help us decide what to do in the future.
