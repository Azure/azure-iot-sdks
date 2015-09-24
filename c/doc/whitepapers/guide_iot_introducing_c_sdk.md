# Introducing the Microsoft Azure IoT device SDK for C

The Azure IoT platform provides different ways for you to connect devices to the cloud. Microsoft has developed a set of SDKs to make the process of connecting those devices easier for you. This article focuses on an introduction to one of the more important SDKs that you’ll work with, the **Azure IoT device SDK for C**.

The **Azure IoT device SDK for C** is a set of a libraries designed to make it easier for devices to send and receive data from the Azure cloud. While different Azure services can be used for this purpose, the **Azure IoT device SDK for C** is designed specifically to enable communication with the **Azure IoT Hub** service.  As the name of the SDK implies, these libraries target the C programming language.

Rather than using the SDK you could write your own software to communicate directly with **IoT Hubs** using one of the communications protocols that it supports. However, taking on an effort like this involves a lot of "plumbing" code that is probably not relevant to your solution; this is why you should use the **Azure IoT device SDK for C**. The libraries that the SDK includes implement the common functions that you need when you communicate with **IoT Hubs**. These libraries abstract the technical details of communicating with **IoT Hubs** using a simple set of APIs that enable you to send and receive data.

The **Azure IoT device SDK for C** includes support for a variety of hardware and software platforms. A complete list can be found in the [readme](https://github.com/Azure/azure-iot-suite-sdks/blob/master/readme.md) in the  GitHub repository. At the time of this writing, the supported platforms include Windows desktop and [Windows IoT](http://ms-iot.github.io/content/en-US/GetStarted.htm), various flavors of Linux, and mbed. The supported programming language is C and the libraries themselves are written to the C99 standard to ensure portability. The list is just the start. The number of supported operating systems will grow in the future.

This article focuses more on the Windows platform. But check back here again, as future articles will address other platforms.

> The **Azure IoT device SDK for C** is constantly being updated. This article was written for the following tagged release in the repository:

> **v0.13.0**

> Be aware that if you’re using a newer release, the APIs described in this article may differ from those in your copy of the code.

# Table of Contents
- [SDK architecture](#sdk_architecture)
- [Before running the samples](#beforerun)
	- [Build the SDK](#beforerun)
	- [Obtaining credentials](#obtaining)
- [IoTHub Client](#iothubclient)
	- [Initializing the library](#initlib)
	- [Sending data](#senddata)
	- [Receiving messages](#receivemessage)
	- [Exchanging data with the cloud](#exchangedata)
	- [Uninitializing the library](#uninitlib)
	- [Checkpoint](#checkpoint)
- [IoTHub Schema Client](#schemaclient)
	- [Initializing the library](#initlib2)
	- [Defining the model](#defmodel)
	- [Sending data](#senddata2)
	- [Receiving messages](#receivemessage)
	- [Exchanging data with the cloud](#exhangedata2)
	- [Uninitializing the library](#uninitlib2)
	- [Checkpoint](#checkpoint2)
	- [Summary](#summary)


<a name="sdk_architecture"/>
# SDK architecture

You can find the **Azure IoT device SDK for C** in the GitHub repository <https://github.com/Azure/azure-iot-suite-sdks>.

You should always use the code in the **master** branch of this repository:
![](media/introducing_iot_c_sdk/image5.png)

This repository contains SDKs that support different languages and platforms.  But this article is about the **Azure IoT device SDK for C** which can be found in the **c** folder:
![](media/introducing_iot_c_sdk/image16.png)

The core implementation is in the **common**, **iothub\_client**, and **iothub\_schema\_client** folders in the repository. The **common** folder contains shared code used throughout the libraries and other tools (typically you don’t use the code in the **common** folder directly). However, the **iothub\_client** and **iothub\_schema\_client** folders contain implementations of the two distinct layers of the application stack that you will use in your code.

-   **IoTHub\_Client** -- The **iothub\_client** folder contains the implementation of the lowest API layer in the library. This layer contains APIs for sending data to **IoT Hubs** as well as receiving messages from it. If you use this layer you are responsible for implementing message serialization, but other details of communicating with **IoT Hubs** are handled for you.

-   **IoTHub\_Schema\_Client** -- The **iothub\_schema\_client** folder contains the implementation of a layer that sits on top of **iothub\_client**. The purpose of the **iothub\_schema\_client** layer is the same as the **iothub\_client** layer (to send data and receive messages), but the **iothub\_schema\_client** layer adds modeling capabilities. If you use the **iothub\_schema\_client**, you define a model that specifies the data you want to send to IoT Hubs and the messages you expect to receive from IoT Hubs. To send data, you populate the model with your data, and then send the portions of the model containing your data to IoT Hubs. Populating the model and sending the data is simply a matter of setting members of a C _struct_ and then calling simple APIs to send the data to IoT Hubs. Details such as data serialization are handled for you.

All of this is easier to understand by looking at some example code. The following sections walk you through a couple of the sample applications that are included in the SDK. This should give you a good feel for the various capabilities of the different architectural layers of the SDK as well as an introduction to how the APIs work.

<a name="beforerun"/>
## Before running the samples

Before you can run the samples in the **Azure IoT device SDK for C** repository you must complete two tasks: build the SDK and obtain device credentials.

### Build the SDK

First, you’ll need to get a copy of the **Azure IoT device SDK for C** from GitHub and then build the source. You should fetch a copy of the source from the **master** branch of the GitHub repository: <https://github.com/Azure/azure-iot-suite-sdks>

When you have obtained a copy of the source, you can build the SDK. There are build instructions for each of the supported platforms in the [doc](https://github.com/Azure/azure-iot-suite-sdks/tree/master/doc) folder of the repository. As an example, you can find the setup instructions for Windows here: <https://github.com/Azure/azure-iot-suite-sdks/blob/master/c/doc/windows_setup.md>

> Be sure to follow the procedure **To install and build Proton on Windows** in the installation guide.

The **To install and build Proton on Windows** procedure includes instructions for installing the _Proton Qpid_ libraries that are used when communicating with IoT Hubs using the AMQP protocol. You must follow this procedure regardless of whether you plan to use either the AMQP or the HTTP protocol, because the Proton libraries are required to compile the SDK. Here are a few tips to help you complete this procedure:

-   When you create your **PROTON\_PATH** environment variable, make it a system environment variable as shown here:

    ![](media/introducing_iot_c_sdk/image7.png)

-   When you install the **CMake** utility, choose the option to add **CMake** to the system PATH for **all users** (adding to **the current user** works as well):

    ![](media/introducing_iot_c_sdk/image8.png)

-   Be sure to install the correct version of Python. Typically in Windows, you should install the x86 version:

    ![](media/introducing_iot_c_sdk/image9.png)

    You can use the default options in the installer.

-   Before you open the **Developer Command Prompt for VS2013**, install the _Git command prompt tools_. To install, complete the following steps:

    1.  Launch **Visual Studio 2013**.

    2.  Open **Team Explorer**.

    3.  Click the link to **Install 3rd-party Git command prompt tools**:
    ![](media/introducing_iot_c_sdk/image10.png)  
    This launches the Web Platform Installer. Follow the instructions to install the tools.

    4.  Add the Git tools **bin** directory to the system **PATH** environment variable. On Windows, this looks like the following:  
    ![](media/introducing_iot_c_sdk/image11.png)

When you run the **build\_proton.cmd** script from the command prompt, the script fetches the Proton source from its GitHub repository and then builds the Proton libraries.

When you have completed the previous steps, you can build the Windows samples included in the repository by opening the Visual Studio solution file and selecting **Build Solution** from the **Build** menu. This article walks you through a couple of these Visual Studio solutions.

<a name="obtaining"/>
### Obtaining credentials

The **Azure IoT device SDK for C** includes libraries that enable you to communicate with **IoT Hubs**. For a device to be able to access an **IoT Hub** endpoint, you must register the device in the **IoT Hub** _device registry_. When you register your device you should make a note of the device credentials, which you will need when you build the samples. In all, you need the following information for each device in order to build and run the samples:

- Device ID
- IoT Hub name
- IoT Hub suffix name (the domain name of the IoT Hub)
- Device key

When you have built the SDK and obtained your device credentials, you are ready to start building and running the Visual Studio solutions. The following sections walk you through two of the sample solutions in the **Azure IoT device SDK for C** SDK that demonstrate the capabilities of the framework.

<a name="iothubclient"/>
## IoTHub\_Client

Within the **iothub\_client** folder in the **azure-iot-suite-sdks** repository there is a **samples** folder that contains a sample called **iothub\_client\_sample\_amqp**.

The Windows version of the **iothub\_client\_sample\_ampq** application includes the following Visual Studio solution:

![](media/introducing_iot_c_sdk/image12.png)

The **iothub\_client\_sample\_amqp** project contains the sample itself. The solution also includes the **common** and **iothub\_client** projects from the **Azure IoT device SDK for C** repository. You always need the **common** project when you are working with the SDK, and because the sample uses the **iothub\_client** API layer, you must also include that project in your solution.

You can find the implementation for the sample application in the **iothub\_client\_sample\_amqp.c** source file:

![](media/introducing_iot_c_sdk/image13.png)

The following sections use this sample application to walk you through what’s required to use the **iothub\_client** library.

<a name="initlib"/>
### Initializing the library

To start working with the libraries you must first create an IoT Hub client handle with code such as the following:

```
IOTHUB_CLIENT_CONFIG iotHubClientConfig;
IOTHUB_CLIENT_LL_HANDLE iotHubClientHandle;

/* Setup IoTHub client configuration */
iotHubClientConfig.iotHubName = iotHubName;
iotHubClientConfig.iotHubSuffix = iotHubSuffix;
iotHubClientConfig.deviceId = deviceId;
iotHubClientConfig.deviceKey = deviceKey;
iotHubClientConfig.protocol = AMQP_Protocol;
iotHubClientHandle = IoTHubClient_LL_Create(&iotHubClientConfig);
```

Notice how the code populates members of the **IOTHUB\_CLIENT\_CONFIG** structure with the credentials for a registered device: device id, device key, IoT Hub Suffix, and IoT Hubs name. This is also where you choose the protocol to use to communicate with IoT Hubs; this example uses AMQP.

When you have a pointer to a valid **IOTHUB\_CLIENT\_LL\_HANDLE**, you can start calling the APIs to send data to an IoT Hub and receive messages from it.

<a name="senddata"/>
### Sending data

So far you've seen how to initialize the library. Next, you'll see how to start sending data to IoT Hubs. This requires you to complete the following steps:

First, create a message:

```
EVENT_INSTANCE message;
message.messageHandle = IoTHubMessage_Create();
```

Next, specify the data you want to send to the IoT Hub:

```
char msgText[1024];
sprintf_s(msgText, sizeof(msgText), "Test Message");
IoTHubMessage_SetData(message.messageHandle,
(const unsigned char*)msgText, strlen(msgText));
```

Finally, send the message:

```
IoTHubClient_LL_SendEventAsync(iotHubClientHandle, message.messageHandle,
ReceiveConfirmationCallback, &message)
```

Every time you send a message, you specify a reference to a callback function that’s invoked when the data is sent:

```
static void ReceiveConfirmationCallback(IOTHUB_CLIENT_CONFIRMATION_RESULT result, void* userContextCallback)
{
  EVENT_INSTANCE* eventInstance =  (EVENT_INSTANCE*)userContextCallback;
  /* Some device specific action code goes here... */
  IoTHubMessage_Destroy(eventInstance->messageHandle);
}
```

Notice the call to the **IoTHubMessage\_Destroy** function when you’re done with the message. You must do this to free the resources allocated when you created the message.

<a name="receivemessage"/>
### Receiving messages

Receiving a message is an asynchronous operation. First, you register a callback to be invoked when the device receives a message. The following code sample shows how to do this:

```
int receiveContext = 0;
IoTHubClient_LL_SetMessageCallback(iotHubClientHandle, ReceiveMessageCallback, &receiveContext);
```

The last parameter is a `void` pointer to whatever you want. In this case, it’s a pointer to an integer but it could be a pointer to a more complex data structure. This enables the callback function to operate on shared state.

When the device receives a message, the registered callback function is invoked:

```
static IOTHUBMESSAGE_DISPOSITION_RESULT ReceiveMessageCallback(IOTHUB_MESSAGE_HANDLE message, void* userContextCallback)
{
  int* counter = (int*)userContextCallback;
  const char* buffer;
  size_t size;
  IoTHubMessage_GetData(message, (const unsigned char**)&buffer, &size);
  (void)printf("Received Message [%d] with Data: <<<%.*s>>> & Size=%d\r\n", *counter, (int)size, buffer, (int)size);
  /* Some device specific action code goes here... */
  (*counter)++;
  return IOTHUBMESSAGE_ACCEPTED;
}
```

Notice how you use the **IoTHubMessage\_GetData** function to retrieve the message, which in this example is a string.

<a name="exchangedata"/>
### Exchanging data with the cloud

In the previous sections you’ve seen how to send data with the **IoTHubClient\_LL\_SendEventAsync** API and how to receive messages by registering a callback function. However, none of the code you've seen so far exchanges data with **IoT Hubs**. For example, when you call the **IoTHubClient\_LL\_SendEventAsync** function, you are putting a message into an in-memory buffer. Nothing goes out on the network until you make the following call:

```
IoTHubClient_LL_DoWork(iotHubClientHandle);
```

The **IoTHubClient\_LL\_DoWork** call does two things: it sends buffered data to the IoT Hub and it requests pending messages for the local device from the IoT Hub. In the latter case, the registered callback function is called for any messages the device receives.

Unless you call the **IoTHubClient\_LL\_DoWork** function, nothing touches the network; buffered messages aren’t sent to the IoT Hub and messages aren’t retrieved from it.

> Note: a single call to the **IoTHubClient\_LL\_DoWork** function does not guarantee that all buffered data is sent to IoT Hubs. Typically, you wrap this function call in a loop, as shown here:

```
IOTHUB_CLIENT_STATUS status;
while ((IoTHubClient_LL_GetSendStatus(iotHubClientHandle, &status) == IOTHUB_CLIENT_OK) && (status == IOTHUB_CLIENT_SEND_STATUS_BUSY))
{
  IoTHubClient_LL_DoWork(iotHubClientHandle);
  ThreadAPI_Sleep(1);
}
```

When the **IoTHubClient\_LL\_GetSendStatus** function no longer returns the **IOTHUB\_CLIENT\_SEND\_STATUS\_BUSY** status, then it has sent all the buffered data.

> Note: even if the **IoTHubClient\_LL\_GetSendStatus** function indicates that you have sent all the buffered data, it’s still possible that messages are waiting to be retrieved (this can happen if there are a lot of messages queued up for the device). If you want to be sure that you have received all the pending messages, a common pattern is to add code to the message callback that tracks the time the last message was received. Then it’s a simple matter of writing logic to wait for a specified timeout period (such as 10 seconds) to check for messages. If you’re calling **IoTHubClient\_LL\_DoWork** during that time period and no messages have been received, then the device must have received any queued messages.

<a name="uninitlib"/>
### Uninitializing the library

When you’re done sending data and receiving messages then you can uninitialize the IoT library. You can do this with the following API call:

```
IoTHubClient_LL_Destroy(iotHubClientHandle);
```

This frees up the resources previously allocated by the **IoTHubClient\_LL\_Create** function call.

<a name="checkpoint"/>
### Checkpoint

As the preceding code samples indicate, it’s easy to send and receive data with the **iothub\_client** library. The library takes care of the details of communicating with IoT Hubs including the protocol to use (which from the perspective of the developer is a simple configuration option). The library also provides you with services, such as buffering, that you can leverage without writing extra code.

The **iothub\_client** library also provides you with precise control over how to serialize the data your device sends to IoT Hubs. In some cases this is an advantage, but in other cases this is an implementation detail with which you don’t want to be concerned. If that's the case, you might consider using the  **iothub\_schema\_client** library, which the next section covers.

<a name="schemaclient"/>
##  IoTHub\_Schema\_Client

The **IoTHub\_Schema\_Client** is a layer that sits on top of the **IoTHub\_Client** layer. It uses the **IoTHub\_Client** layer for the underlying communication with IoT Hubs, but it adds modeling capabilities that remove the burden of dealing with message serialization from the developer. How this layer works is best seen in an example.

Within the **iothub\_schema\_client** folder in the **azure-iot-suite-sdks** repository is a **samples** folder that contains a sample called **simplesample\_http**. The Windows version of this sample includes the following Visual Studio solution:

![](media/introducing_iot_c_sdk/image14.png)

The sample itself is the **SimpleSample\_HTTP** project. As you've seen previously, the solution includes the **common** and **iothub\_client** projects from the **Azure IoT device SDK for C** repository. However, this sample solution includes an extra project called **iothub\_schema\_client**: this is the higher level API that adds modeling support on top of the **iothub\_client** layer.

You can find the implementation of the sample application in the **simplesample\_http.c** code file:

![](media/introducing_iot_c_sdk/image15.png)

The following sections walk you through the key parts of this sample.

<a name="initlib2"/>
### Initializing the library

To start working with the libraries, you must call the initialization APIs:

```
serializer_init(NULL);
IOTHUB_CLIENT_CONFIG iotHubClientConfig;

/* Setup IoTHub client configuration */
iotHubClientConfig.iotHubName = iotHubName;
iotHubClientConfig.iotHubSuffix = iotHubSuffix;
iotHubClientConfig.deviceId = deviceId;
iotHubClientConfig.deviceKey = deviceKey;
iotHubClientConfig.protocol = HTTP_Protocol;
IOTHUB_CLIENT_HANDLE iotHubClientHandle = IoTHubClient_LL_Create(&iotHubClientConfig);
ContosoThermostat505* Thermostat = CREATE_DEVICE(iotHubClientHandle, TRANSPORT_LL, iotHubClientConfig.deviceId, MyThermostat, ContosoThermostat505);
```

The call to the **iothub\_schema\_client\_init** function is a one-time call used to initialize the underlying library. Then you call the **IoTHubClient\_LL\_Create** function, which is the same API you saw in the **iothub\_client** sample, to set your device ID and credentials. Notice that this sample uses HTTP as the transport rather than AMQP.

Finally, call the **CREATE\_DEVICE** function. The last two parameters are of the most interest: **MyThermostat** is the namespace of your model and **ContosoThermostat505** is the name of your model. You can use the pointer returned by the **CREATE\_DEVICE** function to start sending data and receiving messages. First, you must understand what a model is.

<a name="defmodel"/>
### Defining the model

A model in the **iothub\_schema\_client** library defines the data that your device can send to IoT Hubs and the messages, called _actions_ in the model, that it can receive. You define a model using a set of C macros as in the **simplesample\_http** sample application:

```
BEGIN_IOT_DECLARATIONS(MyThermostat);

DECLARE_IOT_MODEL(ContosoThermostat505,
  WITH_DATA(int, Temperature),
  WITH_DATA(int, Humidity),
  WITH_DATA(bool, LowTemperatureAlarm),
  WITH_ACTION(TurnFanOn),
  WITH_ACTION(TurnFanOff),
  WITH_ACTION(SetTemperature, int, DesiredTemperature)
);

END_IOT_DECLARATIONS(MyThermostat);
```

The **BEGIN\_IOT\_DECLARATIONS** and **END\_IOT\_DECLARATIONS** macros both take the namespace of the model as an argument. It’s expected that anything between these macros is the definition of your models and the data structures that the models use.

In this example, there is a single model called **ContosoThermostat505**. This model defines three pieces of data that your device can send to IoT Hubs: **Thermostat**, **Humidity**, and **LowTemperatureAlarm**. It also defines three actions (messages) that your device can receive: **TurnFanOn**, **TurnFanOff**, and **SetTemperature**. Each piece of data (from now on called "data events" or simply "events") has a type, and each action has a name and optionally a set of parameters.

The data events and actions defined in the model define an API surface that you can use to send data to IoT Hubs in the cloud and receive messages from IoT Hubs. This is best seen in an example.

<a name="senddata2"/>
### Sending data

The model defines the data you can send to IoT Hubs. In this sample, that means one of the three data events. If you want to send a **Temperature** event to an IoT Hub, the code looks like this:

```
Thermostat->Temperature = 67;
SEND_ASYNC(sendCallback, (void*)1, Thermostat->Temperature);
```

Remember that **Thermostat** is a reference to an instance of your model. This model is realized as a `struct` with members that match each of the data events defined in the model. To set a temperature, you set the **Temperature** member of the `struct` referenced by **Thermostat**. Once you’ve set the value, you send it by calling the **SEND\_ASYNC** function.

The last parameter to the **SEND\_ASYNC** function is the data event you want to send to the IoT Hub. The first parameter is a reference to a callback function that’s called when the data is successfully sent. The second parameter is a reference to whatever contextual data you want. This data is passed along to the callback function when it's invoked. Here’s an example of a callback function:

```
void sendCallback(IOTHUB_CLIENT_CONFIRMATION_RESULT result,
void* userContextCallback)
{
  int messageTrackingId = (intptr_t)userContextCallback;
  (void)printf("Message Id: %d Received.\r\n", messageTrackingId);
  (void)printf("Result Call Back Called! Result is: %s \r\n",   ENUM_TO_STRING(IOTHUB_CLIENT_CONFIRMATION_RESULT, result));
}
```

Similar to the behavior in the **iothub\_client** layer, when you call the **SEND\_ASYNC** funtion you are not actually sending data to an IoT Hub, you are  placing it in a buffer. The data is sent to the IoT Hub only when you call the **IoTHubClient\_LL\_DoWork** function.

<a name="receivenotfication"/>
### Receiving messages

Receiving a message is an implicit operation. When you define an action in your model, you’re required to implement a corresponding function that’s called when your device receives a message that corresponds to the action. For example, if your model defines this action:

```
WITH_ACTION(SetTemperature, int, DesiredTemperature)
```

You must define a function with this signature:

```
void SetTemperature(ContosoThermostat505* device, int DesiredTemperature)
{
  (void)device;
  (void)printf("Setting home temperature to %d degrees.\\r\\n", DesiredTemperature);
}
```

Notice that the name of the function matches the name of the action in the model and that the parameters of the function match the parameters specified for the action.

When the device receives a message that matches this signature, the corresponding function is called. As before, the library only receives messages when you call the **IoTHubClient\_LL\_DoWork** function. That's all you have to do to receive a message; the corresponding function is invoked automatically.

<a name="exhangedata2"/>
### Exchanging data with the cloud

This part works in the same way as with the **iothub\_client** library. The device does not send any data or receive any messages until you call the **IoTHubClient\_LL\_DoWork** function. The sample application executes this loop forever:

```
while (1)
{
  IoTHubClient_LL_DoWork(iotHubClientHandle);
  ThreadAPI_Sleep(1);
}
```

You can also write logic to terminate this loop after all buffered data is sent by using the **IoTHubClient\_LL\_GetSendStatus** function as described in the previous section.

<a name="uninitlib2"/>
### Uninitializing the library

When you’re done sending data and receiving messages, you can uninitialize the IoT library. Do this with the following API calls:

```
    DESTROY_DEVICE(Thermostat);
  }
  IoTHubClient_LL_Destroy(iotHubClientHandle);
}
serializer_deinit();
```

Each of these three functions aligns with the three initialization functions described previously. Calling these APIs ensures that you free previously allocated resources.

<a name="checkpoint2"/>
### Checkpoint

Functionally, the **iothub\_schema\_client** library is the same as the **iothub\_client** libraries, but it adds modeling support. The benefit is that the API surface is simpler: sending data is just a matter of setting members of a `struct` and then calling an API; receiving messages is a matter of defining a function matching the signature of an action in your model. When you’re sending data you don’t have to deal with serialization and when you're receiving messages you don’t have to deal with parsing messages.

<a name="summary"/>
## Summary

This article covers the basics of using the libraries in the **Azure IoT device SDK for C** repository. It should provide you with enough information to understand what’s included in the SDK, its architecture, and how to get started working with the Windows samples. Future articles will discuss using the SDK on other platforms as well as describe additional capabilities of the libraries.
