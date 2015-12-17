# AmqpsTransport Requirements
 
## Overview

An AMQPS transport. Contains functionality for adding messages and sending messages to an IoT Hub. Buffers unsent messages until they are received by an IoT Hub. A transport is bound at construction to the following parameters: IoT Hub name, device ID, device key, and message valid seconds. The transport can also poll an IoT Hub for messages and invoke a user-defined message callback if a message and callback is found.

## References

## Exposed API

```java
public final class AmqpsTransport implements IotHubTransport
{
    public AmqpsTransport(DeviceClientConfig config);

    public void open() throws IOException;
    public void close() throws IOException;

    public void addMessage(Message msg, IotHubEventCallback callback, Object callbackContext);
    public void sendMessages() throws IOException;
    public void invokeCallbacks();

    public void handleMessage() throws IOException;

    public boolean isEmpty();
}
```


### AmqpsTransport

```java
public AmqpsTransport(DeviceClientConfig config);
```

**SRS_AMQPSTRANSPORT_11_001: [**The function shall initialize an empty queue for messages waiting to be sent.**]**

**SRS_AMQPSTRANSPORT_11_002: [**The function shall initialize an empty queue for completed messages whose callbacks are waiting to be invoked.**]**


### open

```java
public void open() throws IOException;
```

**SRS_AMQPSTRANSPORT_11_019: [**The function shall open an AMQPS connection with the IoT Hub given in the configuration.**]**

**SRS_AMQPSTRANSPORT_11_020: [**If an AMQPS connection is already open, the function shall do nothing.**]**


### close

```java
public void close() throws IOException;
```

**SRS_AMQPSTRANSPORT_11_021: [**The function shall close an AMQPS connection with the IoT Hub given in the configuration.**]**

**SRS_AMQPSTRANSPORT_11_023: [**If the AMQPS connection is closed, the function shall do nothing.**]**


### addMessage

```java
public void addMessage(Message msg, IotHubEventCallback callback, Object callbackContext);
```

**SRS_AMQPSTRANSPORT_11_003: [**The function shall add a packet containing the message, callback, and callback context to the transport queue.**]**

**SRS_AMQPSTRANSPORT_11_025: [**If the AMQPS session is closed, the function shall throw an IllegalStateException.**]**


### sendMessage

```java
public void sendMessages() throws IOException;
```

**SRS_AMQPSTRANSPORT_11_004: [**The function shall attempt to asynchronously send every message on its waiting list, one at a time.**]**

**SRS_AMQPSTRANSPORT_11_005: [**If no AMQPS session exists with the IoT Hub, the function shall establish one.**]**

**SRS_AMQPSTRANSPORT_11_006: [**For each message being sent, the function shall send the message and add the IoT Hub status code along with the callback and context to the callback list.**]**

**SRS_AMQPSTRANSPORT_11_007: [**If the IoT Hub could not be reached, the message shall be buffered to be sent again next time.**]**

**SRS_AMQPSTRANSPORT_11_015: [**If the IoT Hub could not be reached, the function shall throw an IOException.**]**

**SRS_AMQPSTRANSPORT_11_034: [**If the function throws any exception or error, it shall also close the AMQPS session.**]**

**SRS_AMQPSTRANSPORT_11_026: [**If the transport had previously encountered any exception or error while open, it shall reopen a new AMQPS session with the IoT Hub given in the configuration.**]**

**SRS_AMQPSTRANSPORT_11_028: [**If the AMQPS session is closed, the function shall throw an IllegalStateException.**]**


### invokeCallbacks

```java
public void invokeCallbacks();
```

**SRS_AMQPSTRANSPORT_11_008: [**The function shall invoke all callbacks on its callback queue.**]**

**SRS_AMQPSTRANSPORT_11_030: [**If the AMQPS session is closed, the function shall throw an IllegalStateException.**]**


### handleMessage

```java
public void handleMessage() throws IOException;
```

**SRS_AMQPSTRANSPORT_11_010: [**The function shall attempt to consume a message from the IoT Hub.**]**

**SRS_AMQPSTRANSPORT_11_011: [**If a message is found and a message callback is registered, the function shall invoke the callback on the message.**]**

**SRS_AMQPSTRANSPORT_11_012: [**The function shall return the message result (one of COMPLETE, ABANDON, or REJECT) to the IoT Hub.**]**

**SRS_AMQPSTRANSPORT_11_014: [**If no AMQPS session exists with the IoT Hub, the function shall establish one.**]**

**SRS_AMQPSTRANSPORT_11_017: [**If the IoT Hub could not be reached, the function shall throw an IOException.**]**

**SRS_AMQPSTRANSPORT_11_035: [**If the function throws any exception or error, it shall also close the AMQPS session.**]**

**SRS_AMQPSTRANSPORT_11_031: [**If the transport had previously encountered any exception or error while open, it shall reopen a new AMQPS session with the IoT Hub given in the configuration.**]**

**SRS_AMQPSTRANSPORT_11_033: [**If the AMQPS session is closed, the function shall throw an IllegalStateException.**]**


### isEmpty

```java
public boolean isEmpty();
```

**SRS_AMQPSTRANSPORT_11_013: [**The function shall return true if the waiting list and callback list are all empty, and false otherwise.**]**