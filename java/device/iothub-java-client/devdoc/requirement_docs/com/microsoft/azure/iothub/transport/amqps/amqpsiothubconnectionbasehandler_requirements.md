# AmqpsIotHubConnectionBaseHandler Requirements

## Overview

Instance of the QPID-Proton-J BaseHandler class that overrides the events needed to handle receiving and sending operations. Contains and sets connection parameters, maintains the layers of the AMQP protocol and creates and sets SASL authentication for the transport.

## References

## Exposed API

```java
public final class AMqpsIotHubConnectionBaseHandler extends BaseHandler
{
	public AmqpsIotHubConnectionBaseHandler(String hostName, String userName, String sasToken, String deviceID, AmqpsIotHubConnection parentIotHubConnection);
	public void onDelivery(Event event);
	public void onLinkLocalClose(Event event);
	public void onLinkFlow(Event event);
	public void onConnectionBound(Event event);
	public void onConnectionInit(Event event);
	public void onConnectionLocalOpen(Event event);
	public void onConnectionRemoteClose(Event event);
	public void onLinkInit(Event event);
	public void onTransportError(Event event);
    public int getLinkCredit() throws ExecutionException, InterruptedException, TimeoutException;
	public synchronized CompletableFuture<Integer> createBinaryMessage(byte[] content[, Object messageId]);
    public void shutdown();
}
```


### AmqpsIotHubConnectionBaseHandler

```java
public AmqpsIotHubConnectionBaseHandler(String hostName, String userName, String sasToken, String deviceID, AmqpsIotHubConnection parentIotHubConnection);**
```

**SRS_AMQPSIOTHUBCONNECTIONBASEHANDLER_14_001: [**The constructor shall throw a new IllegalArgumentException if any of the input parameters is null or empty.**]**

**SRS_AMQPSIOTHUBCONNECTIONBASEHANDLER_14_002: [**The constructor shall copy all input parameters to private member variables for event processing.**]**

**SRS_AMQPSIOTHUBCONNECTIONBASEHANDLER_14_003: [**The constructor shall concatenate the host name with the port.**]**

**SRS_AMQPSIOTHUBCONNECTIONBASEHANDLER_14_004: [**The constructor shall initialize the sender and receiver endpoint private member variables using the ENDPOINT_FORMAT constants and deviceID.**]**

**SRS_AMQPSIOTHUBCONNECTIONBASEHANDLER_14_005: [**The constructor shall initialize a new Handshaker (Proton) object to handle communication handshake.**]**

**SRS_AMQPSIOTHUBCONNECTIONBASEHANDLER_14_006: [**The constructor shall initialize a new FlowController (Proton) object to handle communication flow.**]**


### onDelivery

```java
public void onDelivery(Event event)
```

**SRS_AMQPSIOTHUBCONNECTIONBASEHANDLER_14_007: [**If this link is the Receiver link, the event handler shall get the Receiver and Delivery (Proton) objects from the event.**]**

**SRS_AMQPSIOTHUBCONNECTIONBASEHANDLER_14_008: [**The event handler shall read the received buffer.**]**

**SRS_AMQPSIOTHUBCONNECTIONBASEHANDLER_14_009: [**The event handler shall create an AmqpsMessage object from the decoded buffer.**]**

**SRS_AMQPSIOTHUBCONNECTIONBASEHANDLER_14_010: [**The event handler shall set the AmqpsMessage Deliver (Proton) object.**]**

**SRS_AMQPSIOTHUBCONNECTIONBASEHANDLER_14_011: [**The event handler shall give the message back to its parent AmqpsIotHubConnection.**]**

**SRS_AMQPSIOTHUBCONNECTIONBASEHANDLER_14_012: [**If this link is not the Receiver link and the event type is DELIVERY, the event handler shall get the Delivery (Proton) object from the event.**]**

**SRS_AMQPSIOTHUBCONNECTIONBASEHANDLER_14_013: [**If the event type is DELIVERY, the event handler shall note the remote delivery state and use it and the Delivery (Proton) hash code to inform the AmqpsIotHubConnection of the message receipt.**]**


### onLinkLocalClose

```java
public void onLinkLocalClose(Event event)
```

**SRS_AMQPSIOTHUBCONNECTIONBASEHANDLER_14_015: [**The event handler shall close the Session and Connection (Proton) objects.**]**

**SRS_AMQPSIOTHUBCONNECTIONBASEHANDLER_14_016: [**If the link was locally closed before the current message is sent, the sent message CompletableFuture will be completed exceptionally with a new HandlerException.**]**


### onLinkFlow

```java
public void onLinkFlow(Event event)
```
**SRS_AMQPSIOTHUBCONNECTIONBASEHANDLER_14_049: [** The event handler shall get the link credit from the event and complete the linkCredit future with the same value. **]**

**SRS_AMQPSIOTHUBCONNECTIONBASEHANDLER_14_050: [** The event handler shall set the link flow flag to allow sending. **]**


### onConnectionBound

```java
public void onConnectionBound(Event event)
```

**SRS_AMQPSIOTHUBCONNECTIONBASEHANDLER_14_023: [**The event handler shall get the Transport (Proton) object from the event.**]**

**SRS_AMQPSIOTHUBCONNECTIONBASEHANDLER_14_024: [**The event handler shall set the SASL_PLAIN authentication on the transport using the given user name and sas token.**]**

**SRS_AMQPSIOTHUBCONNECTIONBASEHANDLER_14_025: [**The event handler shall set ANONYMOUS_PEER authentication mode on the domain of the Transport.**]**


### onConnectionInit

```java
public void onConnectionInit(Event event)
```

**SRS_AMQPSIOTHUBCONNECTIONBASEHANDLER_14_026: [**The event handler shall get the Connection (Proton) object from the event handler and set the host name on the connection.**]**

**SRS_AMQPSIOTHUBCONNECTIONBASEHANDLER_14_027: [**The event handler shall create a Session (Proton) object from the connection.**]**

**SRS_AMQPSIOTHUBCONNECTIONBASEHANDLER_14_028: [**The event handler shall create a Receiver and Sender (Proton) object and set the protocol tag on them to a predefined constant.**]**

**SRS_AMQPSIOTHUBCONNECTIONBASEHANDLER_15_054: [**The Receiver and Sender objects shall have the properties set to device client version identifier.**]**

**SRS_AMQPSIOTHUBCONNECTIONBASEHANDLER_14_029: [**The event handler shall open the Connection, Session, Sender, and Receiver objects.**]**


### onConnectionLocalOpen

```java
public void onConnectionLocalOpen(Event event)
```

**SRS_AMQPSIOTHUBCONNECTIONBASEHANDLER_14_037: [**If the Connection and Session (Proton) are not opened before a constant number of seconds, this handler will inform the AmqpsIotHubConnection that the Connection was not opened in time.**]**


### onConnectionRemoteClose

```java
public void onConnectionRemoteClose(Event event)
```

**SRS_AMQPSIOTHUBCONNECTIONBASEHANDLER_14_035: [**If the Connection was remotely closed abnormally, the event handler shall complete the sent message CompletableFuture with a new HandlerException.**]**


### onLinkRemoteOpen

```java
public void onLinkRemoteOpen(Event event)
```

**SRS_AMQPSIOTHUBCONNECTIONBASEHANDLER_14_036: [**The event handler shall inform the AmqpsIotHubConnection that the Reactor (Proton) is ready if the event link is the Sender link.**]**


### onLinkInit

```java
public void onLinkInit(Event event)
```

**SRS_AMQPSIOTHUBCONNECTIONBASEHANDLER_14_030: [**If the link is the Receiver link, the event handler shall create a new Source (Proton) object using the receiver endpoint address member variable.**]**

**SRS_AMQPSIOTHUBCONNECTIONBASEHANDLER_14_031: [**If the link is the Sender link, the event handler shall create a new Target (Proton) object using the sender endpoint address member variable.**]**

**SRS_AMQPSIOTHUBCONNECTIONBASEHANDLER_14_032: [**If the link is the Receiver link, the event handler shall get the Link (Proton) object and set its source to the created Source (Proton) object.**]**

**SRS_AMQPSIOTHUBCONNECTIONBASEHANDLER_14_033: [**If the link is the Sender link, the event handler shall get the Link (Proton) object and set its target.**]**

**SRS_AMQPSIOTHUBCONNECTIONBASEHANDLER_14_034: [**If the link is the Sender link, the event handler shall set the SenderSettleMode to UNSETTLED.**]**


### onTransportError

```java
public void onTransportError(Event event)
```

**SRS_AMQPSIOTHUBCONNECTIONBASEHANDLER_14_038: [**The event handler shall log the error and description if there is a description accompanying the error.**]**

**SRS_AMQPSIOTHUBCONNECTIONBASEHANDLER_14_039: [**The event handler shall cause the AmqpsIotHubConnection to fail.**]**


### getLinkCredit

```java
public int getLinkCredit() throws ExecutionException, InterruptedException, TimeoutException
```

**SRS_AMQPSIOTHUBCONNECTIONBASEHANDLER_14_051: [**The method shall get the link credit from the CompletableFuture link credit object. **]**


### createBinaryMessage

```java
public synchronized CompletableFuture<Integer> createBinaryMessage(byte[] content[, Object messageId])
```

**SRS_AMQPSIOTHUBCONNECTIONBASEHANDLER_14_040: [**The function shall create a new Message (Proton) object.**]**

**SRS_AMQPSIOTHUBCONNECTIONBASEHANDLER_14_041: [**The function shall set the ‘to’ property on the Message object using the created event path.**]**

**SRS_AMQPSIOTHUBCONNECTIONBASEHANDLER_14_042: [**The function shall set the ‘messageId’ property on the Message object if the messageId is not null.**]**

**SRS_AMQPSIOTHUBCONNECTIONBASEHANDLER_14_043: [**The function shall create a Binary (Proton) object from the content array.**]**

**SRS_AMQPSIOTHUBCONNECTIONBASEHANDLER_14_044: [**The function shall create a data Section (Proton) object from the Binary.**]**

**SRS_AMQPSIOTHUBCONNECTIONBASEHANDLER_14_045: [**The function shall set the Message body to the created data Section.**]**

**SRS_AMQPSIOTHUBCONNECTIONBASEHANDLER_14_047: [**The function shall return a new CompletableFuture for the sent message.**]**

**SRS_AMQPSIOTHUBCONNECTIONBASEHANDLER_14_054: [**The function shall asynchronously send the message.**]**

### shutdown

```java
public void shutdown()
```

**SRS_AMQPSIOTHUBCONNECTIONBASEHANDLER_14_052: [**The function shall set the linkFlow field to false.**]**

**SRS_AMQPSIOTHUBCONNECTIONBASEHANDLER_14_053: [**The function shall close the Sender, Receiver, Session, and Connection.**]**