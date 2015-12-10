# MqttIotHubConnectionHandler Requirements

## Overview

Implementation of the Eclipse Paho MqttCallback class that overrides the events needed to handle receiving operations.

## References

## Exposed API

```java
public class MqttIotHubConnectionHandler implements MqttCallback
{
	public MqttIotHubConnectionHandler(String hostName, String deviceID, String sasToken);
	public void open();
	public void close();
	public void send(byte[] content[, Object messageId]) throws IOException, HandlerException
}
```


### MqttIotHubConnectionHandler

```java
public MqttIotHubConnectionHandler(String hostName, String deviceID, String sasToken)
```

** SRS_MQTTIOTHUBCONNECTIONHANDLER_15_001: [The constructor shall throw a new IllegalArgumentException if any of the input parameters is null or empty.] **

** SRS_MQTTIOTHUBCONNECTIONHANDLER_15_002: [The Constructor shall copy all input parameters to private member variables.] **

** SRS_MQTTIOTHUBCONNECTIONHANDLER_15_003: [The Constructor shall create the publish/subscribe topics for the specified input parameters to private member variables.] **

** SRS_MQTTIOTHUBCONNECTIONHANDLER_15_004: [The constructor shall initialize a new private queue for received messages.] **

** SRS_MQTTIOTHUBCONNECTIONHANDLER_15_015: [The constructor shall save the connection options into a private variable which shall be used for publishing/receiving events/messages to/from IotHub] **


### open

```java
public void open()
```

** SRS_MQTTIOTHUBCONNECTIONHANDLER_15_005: [The function shall initialize its MqttAsyncClient using the saved host name and device id.] **


### close

```java
public void close()
```

** SRS_MQTTIOTHUBCONNECTIONHANDLER_15_006: [The function shall invalidate its MqttAsyncClient.] **


### send

```java
public void send(byte[] content[, Object messageId]) throws IOException
```

** SRS_MQTTIOTHUBCONNECTIONHANDLER_15_007: [If the MqttAsyncClient has not been initialized, the function shall throw a new IOException.] ** 

** SRS_MQTTIOTHUBCONNECTIONHANDLER_15_010: [The function shall connect to the IoT Hub broker using the connection options saved previously.] **

** SRS_MQTTIOTHUBCONNECTIONHANDLER_15_011: [The function shall publish the content of the binary message to the specific device id topic] **


### consumeMessage

```java
public Message consumeMessage() throws IOException
```

** SRS_MQTTIOTHUBCONNECTIONHANDLER_15_012: [If the MqttIotHubConnectionHandler has not been initialized, the function shall throw a new IOException.] **

** SRS_MQTTIOTHUBCONNECTIONHANDLER_15_013: [The function shall attempt to remove a message from the message queue.] **

** SRS_MQTTIOTHUBCONNECTIONHANDLER_15_014: [The function shall return the message if one was pulled from the queue, otherwise it shall return null.] ** 