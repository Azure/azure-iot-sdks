# MqttIotHubConnection Requirements

## Overview

An MQTT connection between a device and an IoT Hub.

## References

## Exposed API

```java
public final class MqttIotHubConnection
{
    public MqttIotHubConnection(DeviceClientConfig config);

    public void open() throws IOException;
    public void close() throws IOException;
    public IotHubStatusCode sendEvent(Message msg) throws IOException;
    public Message receiveMessage() throws IOException;
    public void sendMessageResult(IotHubMessageResult result) throws IOException;
}
```


### MqttIotHubConnection

```java
public MqttIotHubConnection(DeviceClientConfig config);
```

**SRS_MQTTIOTHUBCONNECTION_15_001: [**The constructor shall save the configuration.**]**

**SRS_MQTTIOTHUBCONNECTION_15_018: [**The constructor shall throw a new IllegalArgumentException if any of the parameters of the configuration is null or empty.**]**


### open

```java
public void open() throws IOException;
```

**SRS_MQTTIOTHUBCONNECTION_15_002: [**The function shall establish an MQTT connection with an IoT Hub using the provided host name, user name, device ID, and sas token.**]**

**SRS_MQTTIOTHUBCONNECTION_15_003: [**If an MQTT connection is unable to be established for any reason, the function shall throw an IOException.**]**

**SRS_MQTTIOTHUBCONNECTION_15_004: [**If the MQTT connection is already open, the function shall do nothing.**]**


### close

```java
public void close() throws IOException;
```

**SRS_MQTTIOTHUBCONNECTION_15_005: [**The function shall close the MQTT connection.**]**

**SRS_MQTTIOTHUBCONNECTION_15_006: [**If the MQTT connection is closed, the function shall do nothing.**]**


### sendEvent

```java
public IotHubStatusCode sendEvent(Message msg) throws IOException;
```

**SRS_MQTTIOTHUBCONNECTION_15_007: [**The function shall send an event message to the IoT Hub given in the configuration.**]**

**SRS_MQTTIOTHUBCONNECTION_15_008: [**The function shall send the message body.**]**

**SRS_MQTTIOTHUBCONNECTION_15_019: [**If the message is null, the function shall return status code BAD_FORMAT.**]**

**SRS_MQTTIOTHUBCONNECTION_15_010: [**If the message was successfully received by the service, the function shall return status code OK_EMPTY.**]**

**SRS_MQTTIOTHUBCONNECTION_15_011: [**If the message was not successfully received by the service, the function shall return status code ERROR.**]**

**SRS_MQTTIOTHUBCONNECTION_15_012: [**If the MQTT connection is closed, the function shall throw an IllegalStateException.**]**


### receiveMessage

```java
public Message receiveMessage() throws IOException;
```

**SRS_MQTTIOTHUBCONNECTION_15_013: [**The function shall attempt to consume a message from the IoT Hub given in the configuration.**]**

**SRS_MQTTIOTHUBCONNECTION_15_014: [**If a message is found, the function shall include the message body in the returned message.**]**

**SRS_MQTTIOTHUBCONNECTION_15_015: [**If a message is found, the function shall include all valid application properties in the returned message.**]**

**SRS_MQTTIOTHUBCONNECTION_15_016: [**If the IoT Hub could not be reached, the function shall throw an IOException.**]**

**SRS_MQTTIOTHUBCONNECTION_15_017: [**If the MQTT connection is closed, the function shall throw an IllegalStateException.**]**