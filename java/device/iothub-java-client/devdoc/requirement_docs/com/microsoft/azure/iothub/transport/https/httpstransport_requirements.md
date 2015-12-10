# HttpsTransport Requirements

## Overview

An HTTPS transport. Contains functionality for adding messages and sending batched messages to an IoT Hub. Buffers unsent messages until they are received by an IoT Hub. A transport is bound at construction to the following parameters: IoT Hub name, device ID, device key, and message valid seconds.
The transport can also poll an IoT Hub for messages and invoke a user-defined message callback if a message and callback is found.

## References

## Exposed API

```java
public final class HttpsTransport implements IotHubTransport
{
    public HttpsTransport(DeviceClientConfig config);

    public void open() throws IOException;
    public void close() throws IOException;

    public void addMessage(Message message, IotHubEventCallback callback, Object callbackContext);

    public void sendMessages() throws IOException;
    public void invokeCallbacks();

    public void handleMessage() throws IOException;

    public boolean isEmpty();
}
```


### HttpsTransport

```java
public HttpsTransport(DeviceClientConfig config);
```

**SRS_HTTPSTRANSPORT_11_001: [**The constructor shall initialize an empty transport queue for adding messages to be sent as a batch.**]**

**SRS_HTTPSTRANSPORT_11_002: [**The constructor shall initialize an empty queue for adding callbacks waiting to be.**]**


### open

```java
public void open() throws IOException;
```

**SRS_HTTPSTRANSPORT_11_021: [**The function shall establish an HTTPS connection with the IoT Hub given in the configuration.**]**

**SRS_HTTPSTRANSPORT_11_022: [**If the transport is already open, the function shall do nothing.**]**


### close

```java
public void close() throws IOException;
```

**SRS_HTTPSTRANSPORT_11_035: [**The function shall mark the transport as being closed.**]**


### addMessage

```java
public void addMessage(Message message, IotHubEventCallback callback, Object callbackContext);
```

**SRS_HTTPSTRANSPORT_11_003: [**The function shall add a packet containing the message, callback, and callback context to the transport queue.**]**

**SRS_HTTPSTRANSPORT_11_027: [**If the transport is closed, the function shall throw an IllegalStateException.**]**


### sendMessage

```java
public void sendMessages() throws IOException;
```

**SRS_HTTPSTRANSPORT_11_004: [**If no previous send request had failed while in progress, the function shall batch as many messages as possible such that the batched message body is of size at most 256 kb.**]**

**SRS_HTTPSTRANSPORT_11_012: [**If a previous send request had failed while in progress, the function shall resend the request.**]**

**SRS_HTTPSTRANSPORT_11_013: [**If no messages fit using the batch format, the function shall send a single message without the batch format.**]**

**SRS_HTTPSTRANSPORT_11_014: [**If the send request fails while in progress, the function shall throw an IOException.**]**

**SRS_HTTPSTRANSPORT_11_008: [**The request shall be sent to the IoT Hub given in the configuration from the constructor.**]**

**SRS_HTTPSTRANSPORT_11_005: [**The function shall configure a valid HTTPS request and send it to the IoT Hub.**]**

**SRS_HTTPSTRANSPORT_11_006: [**The function shall add a packet containing the callbacks, contexts, and response for all sent messages to the callback queue.**]**

**SRS_HTTPSTRANSPORT_11_029: [**If the transport is closed, the function shall throw an IllegalStateException.**]**


### invokeCallbacks

```java
public void invokeCallbacks();
```

**SRS_HTTPSTRANSPORT_11_007: [**The function shall invoke all callbacks on the callback queue.**]**

**SRS_HTTPSTRANSPORT_11_016: [**If an exception is thrown during the callback, the function shall drop the callback from the queue.**]**

**SRS_HTTPSTRANSPORT_11_031: [**If the transport is closed, the function shall throw an IllegalStateException.**]**


### handleMessage

```java
public void handleMessage() throws IOException;
```

**SRS_HTTPSTRANSPORT_11_009: [**The function shall poll the IoT Hub for messages.**]**

**SRS_HTTPSTRANSPORT_11_010: [**If a message is found and a message callback is registered, the function shall invoke the callback on the message.**]**

**SRS_HTTPSTRANSPORT_11_011: [**The function shall return the message result (one of COMPLETE, ABANDON, or REJECT) to the IoT Hub.**]**

**SRS_HTTPSTRANSPORT_11_019: [**If the IoT Hub could not be reached, the function shall throw an IOException.**]**

**SRS_HTTPSTRANSPORT_11_020: [**If the response from sending the IoT Hub message result does not have status code OK_EMPTY, the function shall throw an IOException.**]**

**SRS_HTTPSTRANSPORT_11_033: [**If the transport is closed, the function shall throw an IllegalStateException.**]**


### isEmpty

```java
public boolean isEmpty();
```

**SRS_HTTPSTRANSPORT_11_015: [**The function shall return true if the waiting list, in progress list, and callback list are all empty, and false otherwise.**]**