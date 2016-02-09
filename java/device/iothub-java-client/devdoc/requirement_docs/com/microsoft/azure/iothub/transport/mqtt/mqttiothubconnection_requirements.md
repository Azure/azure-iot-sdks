# MqttIotHubConnection Requirements

## Overview

An MQTT connection between a device and an IoT Hub. The connection implements the Eclipse Paho MqttCallback interface and overrides the connectionLost and messageArrived events.

## References

## Exposed API

```java
public final class MqttIotHubConnection implements MqttCallback
{
    public MqttIotHubConnection(DeviceClientConfig config);

    public void open() throws IOException;
    public void close();
    public IotHubStatusCode sendEvent(Message msg) throws IllegalStateException;
    public Message receiveMessage() throws IllegalStateException;

    public void connectionLost(Throwable throwable);
    public void messageArrived(String topic, MqttMessage mqttMessage);
}
```


### MqttIotHubConnection

```java
public MqttIotHubConnection(DeviceClientConfig config)
```

**SRS_MQTTIOTHUBCONNECTION_15_001: [**The constructor shall save the configuration.**]**

**SRS_MQTTIOTHUBCONNECTION_15_002: [**The constructor shall create the publish and subscribe topic for the specified device id.**]**

**SRS_MQTTIOTHUBCONNECTION_15_003: [**The constructor shall throw a new IllegalArgumentException if any of the parameters of the configuration is null or empty.**]**


### open

```java
public void open() throws IOException, IllegalArgumentException;
```

**SRS_MQTTIOTHUBCONNECTION_15_004: [**The function shall establish an MQTT connection with an IoT Hub using the provided host name, user name, device ID, and sas token.**]**

**SRS_MQTTIOTHUBCONNECTION_15_005: [**If an MQTT connection is unable to be established for any reason, the function shall throw an IOException.**]**

**SRS_MQTTIOTHUBCONNECTION_15_006: [**If the MQTT connection is already open, the function shall do nothing.**]**


### close

```java
public void close();
```

**SRS_MQTTIOTHUBCONNECTION_15_006: [**The function shall close the MQTT connection.**]**

**SRS_MQTTIOTHUBCONNECTION_15_007: [**If the MQTT connection is closed, the function shall do nothing.**]**


### sendEvent

```java
public IotHubStatusCode sendEvent(Message msg) throws IllegalStateException
```

**SRS_MQTTIOTHUBCONNECTION_15_008: [**The function shall send an event message to the IoT Hub given in the configuration.**]**

**SRS_MQTTIOTHUBCONNECTION_15_009: [**The function shall send the message payload.**]**

**SRS_MQTTIOTHUBCONNECTION_15_010: [**If the message is null, the function shall return status code BAD_FORMAT.**]**

**SRS_MQTTIOTHUBCONNECTION_15_011: [**If the message was successfully received by the service, the function shall return status code OK_EMPTY.**]**

**SRS_MQTTIOTHUBCONNECTION_15_012: [**If the message was not successfully received by the service, the function shall return status code ERROR.**]**

**SRS_MQTTIOTHUBCONNECTION_15_013: [**If the MQTT connection is closed, the function shall throw an IllegalStateException.**]**


### receiveMessage

```java
public Message receiveMessage() throws IllegalStateException;
```

**SRS_MQTTIOTHUBCONNECTION_15_014: [**The function shall attempt to consume a message from the received messages queue.**]**

**SRS_MQTTIOTHUBCONNECTION_15_015: [**If the MQTT connection is closed, the function shall throw an IllegalStateException.**]**


### connectionLost

```java
public void connectionLost(Throwable throwable);
```

**SRS_MQTTIOTHUBCONNECTION_15_016: [**The function shall attempt to reconnect to the IoTHub in a loop with exponential backoff until it succeeds**]**

**SRS_MQTTIOTHUBCONNECTION_15_018: [**The maximum wait interval until a reconnect is attempted shall be 60 seconds.**]**


### messageArrived

```java
public void messageArrived(String topic, MqttMessage mqttMessage);
```

**SRS_MQTTIOTHUBCONNECTION_15_019: [**A new message is created using the payload from the received MqttMessage**]**

**SRS_MQTTIOTHUBCONNECTION_15_020: [**The newly created message is added to the received messages queue.**]**