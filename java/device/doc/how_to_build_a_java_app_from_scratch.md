# How to write an Azure IoT client application from scratch with Java on Windows

This document describes how to create a client application that uses the *Microsoft Azure IoT device SDK for Java* to communicate with the Azure IoT Hubs service. It uses a garage door opener as the example device to implement and shows you how to create project to build a client to run on Windows.

# Table of contents
- [Installation](#installation)
- [Create application](#createapp)
- [Sample applications](#runningsamples)
	- [Send event](#send)
	- [Receive messages](#receive)
	- [Change logging granularity](#logging)
- [API documentation](#apidoc)

<a name="installation"/>
# Installation
For installation instructions please see the **Introducing the Microsoft Azure IoT device SDK for Java** guide: (devbox_setup.md)

<a name="createapp"/>
## Create application
These are the SDK related steps to create an application using Azure IoT device SDK for Java

- Import SDK libraries

```
import com.microsoft.azure.iothub.IotHubClient;
import com.microsoft.azure.iothub.IotHubClientProtocol;
import com.microsoft.azure.iothub.IotHubMessage;
import com.microsoft.azure.iothub.IotHubMessageResult;
import com.microsoft.azure.iothub.MessageCallback;
import com.microsoft.azure.iothub.IotHubServiceboundMessage;
import com.microsoft.azure.iothub.IotHubStatusCode;
import com.microsoft.azure.iothub.IotHubEventCallback;
```

- Create client object using a valid connection string and protocol

```
String connString = "<Connection string goes here>";

IotHubClientProtocol protocol = IotHubClientProtocol.HTTPS;
or
IotHubClientProtocol protocol = IotHubClientProtocol.AMQPS;

IotHubClient client = new IotHubClient(connString, protocol);
```	

- Create and set up message callback if you want to handle IoT Hub messages

```
protected static class MessageCallback implements IotHubMessageCallback

MessageCallback callback = new MessageCallback();
Counter counter = new Counter(0);
client.setMessageCallback(callback, counter);
```

- Start / stop client

```
client.open;

client.close;
```

- Send events

```
String msgStr = "<Message string goes here>";
IotHubServiceboundMessage msg = new IotHubServiceboundMessage(msgStr);
String messageCount = "1";

MessageCallback callback = new MessageCallback();
client.sendEventAsync(msg, callback, messageCount);
```

<a name="runningsamples"/>
## Sample applications
After the Azure IoT device SDK is installed, working sample applications can be found at:

```
{IoT device SDK root}/java/device/samples/handle-messages/src/main/java/samples
```

<a name="send"/>
### Send events
After the Azure IoT device SDK is installed, the executable JAR file for sending events
can be found at:

```
{IoT device SDK root}/java/device/samples/send-event/target/send-event-{version}-with-deps.jar
```

Navigate to the directory with the send event  sample. Run the sample using the following command:

```
java -jar ./send-event-{version}-with-deps.jar "{connection string}" "{number of requests to send}" "{https or amqps}"
```


<a name="receive"/>
### Receive messages
The executable JAR file for receiving messages can be found at:

```
{IoT device SDK root}/java/device/samples/handle-messages/target/handle-messages-{version}-with-deps.jar
```

Navigate to the directory with the messages sample. Run the sample using the following command:

```
java -jar ./handle-messages-{version}-with-deps.jar "{connection string}" "{https or amqps}"
```

Note that the double quotes around each argument are required.

<a name="logging"/>
### Change logging granularity
The logging granularity can be specified using a dedicated `config.properties` file.
Include the following line in your `config.properties` file:

```
.level = {LOGGING_LEVEL}
```

An explanation of the different logging levels can be found 
[here](http://docs.oracle.com/javase/7/docs/api/java/util/logging/Level.html).

Then, set the JVM property `java.util.logging.config.file={Path to your config.properties file}`.

To log AMQP frames, set the environment variable PN_TRACE_FRM=1.

<a name="apidoc"/>
## API Documentation

The documentation can be found at:

```
{IoT device SDK root}/java/device/iothub-java-client/doc/iothubclient_java_library/index.html
```
