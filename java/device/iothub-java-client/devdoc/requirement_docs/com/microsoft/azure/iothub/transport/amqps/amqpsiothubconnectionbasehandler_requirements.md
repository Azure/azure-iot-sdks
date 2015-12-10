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
	public void onLinkRemoteClose(Event event);
	public void onConnectionBound(Event event);
	public void onConnectionInit(Event event);
	public void onConnectionLocalOpen(Event event);
	public void onConnectionRemoteClose(Event event);
	public void onSessionRemoteOpen(Event event);
	public void onLinkInit(Event event);
	public void onTransportError(Event event);
	public synchronized CompletableFuture<Integer> createBinaryMessage(byte[] content[, Object messageId]);
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

**SRS_AMQPSIOTHUBCONNECTIONBASEHANDLER_14_014: [**If the event type is DELIVERY, the event handler shall close the Sender link.**]**


### onLinkLocalClose

```java
public void onLinkLocalClose(Event event)
```

**SRS_AMQPSIOTHUBCONNECTIONBASEHANDLER_14_015: [**If this link is the Receiver link, the event handler shall close the Session and Connection (Proton) objects.**]**

**SRS_AMQPSIOTHUBCONNECTIONBASEHANDLER_14_016: [**If the link was locally closed before the current message is sent, the sent message CompletableFuture will be completed exceptionally with a new HandlerException.**]**


### onLinkFlow

```java
public void onLinkFlow(Event event)
```

**SRS_AMQPSIOTHUBCONNECTIONBASEHANDLER_14_017: [**The event handler shall get the Sender (Proton) object from the link.**]**

**SRS_AMQPSIOTHUBCONNECTIONBASEHANDLER_14_018: [**The event handler shall encode the message and copy the contents to the byte buffer.**]**

**SRS_AMQPSIOTHUBCONNECTIONBASEHANDLER_14_019: [**The event handler shall set the delivery tag on the Sender (Proton) object.**]**

**SRS_AMQPSIOTHUBCONNECTIONBASEHANDLER_14_020: [**The event handler shall send the encoded bytes.**]**

**SRS_AMQPSIOTHUBCONNECTIONBASEHANDLER_14_021: [**The event handler shall advance the Sender and complete the sent message CompletableFuture using the Delivery (Proton) object hash code.**]**


### onLinkRemoteClose

```java
public void onLinkRemoteClose(Event event)
```

**SRS_AMQPSIOTHUBCONNECTIONBASEHANDLER_14_022: [**If the link is the Sender link and the message was sent successfully over this link, the event handler shall reset the private Sender (Proton) member variable and open the AmqpsIotHubConnection for sending.**]**


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

**SRS_AMQPSIOTHUBCONNECTIONBASEHANDLER_14_029: [**The event handler shall open the Connection, Session, and Receiver objects.**]**


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


### onSessionRemoteOpen

```java
public void onSessionRemoteOpen(Event event)
```

**SRS_AMQPSIOTHUBCONNECTIONBASEHANDLER_14_036: [**The event handler shall inform the AmqpsIotHubConnection that the Reactor (Proton) is ready.**]**


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


### synchronized

```java
public synchronized CompletableFuture<Integer> createBinaryMessage(byte[] content[, Object messageId])
```

**SRS_AMQPSIOTHUBCONNECTIONBASEHANDLER_14_040: [**The function shall create a new Message (Proton) object.**]**

**SRS_AMQPSIOTHUBCONNECTIONBASEHANDLER_14_041: [**The function shall set the ‘to’ property on the Message object using the created event path.**]**

**SRS_AMQPSIOTHUBCONNECTIONBASEHANDLER_14_042: [**The function shall set the ‘messageId’ property on the Message object if the messageId is not null.**]**

**SRS_AMQPSIOTHUBCONNECTIONBASEHANDLER_14_043: [**The function shall create a Binary (Proton) object from the content array.**]**

**SRS_AMQPSIOTHUBCONNECTIONBASEHANDLER_14_044: [**The function shall create a data Section (Proton) object from the Binary.**]**

**SRS_AMQPSIOTHUBCONNECTIONBASEHANDLER_14_045: [**The function shall set the Message body to the created data Section.**]**

**SRS_AMQPSIOTHUBCONNECTIONBASEHANDLER_14_046: [**The function shall open the Sender (Proton) link.**]**

**SRS_AMQPSIOTHUBCONNECTIONBASEHANDLER_14_047: [**The function shall return a new CompletableFuture for the sent message.**]**

**SRS_AMQPSIOTHUBCONNECTIONBASEHANDLER_14_048: [**The function shall lock sending on the AmqpsIotHubConnection.**]**