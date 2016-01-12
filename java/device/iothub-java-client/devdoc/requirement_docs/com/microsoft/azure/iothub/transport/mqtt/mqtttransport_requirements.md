# MqttTransport Requirements

## Overview

An MQTT transport. Contains functionality for adding messages and sending batched messages to an IoT Hub. Buffers unsent messages until they are received by an IoT Hub. A transport is bound at construction to the following parameters: IoT Hub name, device ID, device key, and message valid seconds.
The transport can also poll an IoT Hub for messages and invoke a user-defined message callback if a message and callback is found.

## References

## Exposed API

```java
public final class MqttTransport implements IotHubTransport
{
    public MqttTransport(DeviceClientConfig config);

    public void open() throws IOException;
    public void close();

    public void addMessage(Message message, IotHubEventCallback callback, Object callbackContext) throws IllegalStateException;

    public void sendMessages() throws IllegalStateException;
    public void invokeCallbacks() throws IllegalStateException;;

    public void handleMessage() throws IllegalStateException;

    public boolean isEmpty();
}
```


### MqttTransport

```java
public MqttTransport(DeviceClientConfig config);
```

**SRS_MQTTTRANSPORT_15_001: [**The constructor shall initialize an empty transport queue for adding messages to be sent as a batch.**]**

**SRS_MQTTTRANSPORT_15_002: [**The constructor shall initialize an empty queue for completed messages whose callbacks are waiting to be invoked.**]**


### open

```java
public void open() throws IOException;
```

**SRS_MQTTTRANSPORT_15_003: [**The function shall establish an MQTT connection with IoT Hub given in the configuration.**]**

**SRS_MQTTTRANSPORT_15_004: [**If the MQTT connection is already open, the function shall do nothing.**]**


### close

```java
public void close();
```

**SRS_MQTTTRANSPORT_15_005: [**The function shall close the MQTT connection with the IoT Hub given in the configuration.**]**

**SRS_MQTTTRANSPORT_15_006: [**If the MQTT connection is closed, the function shall do nothing.**] 


### addMessage

```java
public void addMessage(Message message, IotHubEventCallback callback, Object callbackContext) throws IllegalStateException;**
```

**SRS_MQTTTRANSPORT_15_007: [**The function shall add a packet containing the message, callback, and callback context to the transport queue.**]**

**SRS_MQTTTRANSPORT_15_008: [**If the transport is closed, the function shall throw an IllegalStateException.**]**


### sendMessages

```java
public void sendMessages() throws IllegalStateException;
```

**SRS_MQTTTRANSPORT_15_009: [**The function shall attempt to send every message on its waiting list, one at a time.**]**

**SRS_MQTTTRANSPORT_15_010: [**For each message being sent successfully, the function shall add the IoT Hub status code along with the callback and context to the callback list.**]**

**SRS_MQTTTRANSPORT_15_011: [**If the IoT Hub could not be reached, the message shall be buffered to be sent again next time.**]**

**SRS_MQTTTRANSPORT_15_012: [**If the MQTT connection is closed, the function shall throw an IllegalStateException.**]**


### invokeCallbacks

```java
public void invokeCallbacks() throws IllegalStateException;
```

**SRS_MQTTTRANSPORT_15_013: [**The function shall invoke all callbacks on the callback queue.**]**

**SRS_MQTTTRANSPORT_15_014: [**If the transport is closed, the function shall throw an IllegalStateException.**]**

**SRS_MQTTTRANSPORT_15_015: [**If an exception is thrown during the callback, the function shall drop the callback from the queue.**]**


### handleMessage

```java
public void handleMessage() throws IllegalStateException;
```

**SRS_MQTTTRANSPORT_15_016: [**The function shall attempt to consume a message from the IoT Hub.**]**

**SRS_MQTTTRANSPORT_15_017: [**If a message is found and a message callback is registered, the function shall invoke the callback on the message.**]**

**SRS_MQTTTRANSPORT_15_018: [**If the MQTT connection is closed, the function shall throw an IllegalStateException.**]**


### isEmpty

```java
public boolean isEmpty();
```

**SRS_MQTTTRANSPORT_15_019: [**The function shall return true if the waiting list, in progress list, and callback list are all empty, and false otherwise.**]**