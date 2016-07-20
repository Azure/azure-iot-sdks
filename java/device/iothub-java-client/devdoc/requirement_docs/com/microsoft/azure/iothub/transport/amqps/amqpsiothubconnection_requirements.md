# AmqpsIotHubConnection Requirements
 
## Overview

An AMQPS IotHub connection between a device and an IoTHub. This class contains functionality for sending/receiving a message, and logic to re-establish the connection with the IoTHub in case it gets lost.

## References

## Exposed API

```java
public final class AmqpsIotHubConnection extends BaseHandler
{
	public AmqpsIotHubConnection(DeviceClientConfig config, Boolean useWebSockets);
	public void open() throws IOException;
	public void close();
    public Integer sendMessage(Message message)
    public Boolean sendMessageResult(AmqpsMessage message, IotHubMessageResult result);
	
	public void onConnectionInit(Event event);
	public void onConnectionBound(Event event);
	public void onReactorInit(Event event);
	public void onDelivery(Event event);
	public void onLinkFlow(Event event);
	public void onLinkRemoteClose(Event event);
	public void onLinkRemoteOpen(Event event);
	public void onLinkInit(Event event);
	public void onTransportError(Event event);
}
```


### AmqpsIotHubConnection

```java
public AmqpsIotHubConnection(DeviceClientConfig config, Boolean useWebSockets);
```

**SRS_AMQPSIOTHUBCONNECTION_15_001: [**The constructor shall throw IllegalArgumentException if any of the parameters of the configuration is null or empty.**]**

**SRS_AMQPSIOTHUBCONNECTION_15_002: [**The constructor shall save the configuration into private member variables.**]**

**SRS_AMQPSIOTHUBCONNECTION_15_003: [**The constructor shall initialize the sender and receiver endpoint private member variables using the send/receiveEndpointFormat constants and device id.**]**

**SRS_AMQPSIOTHUBCONNECTION_15_004: [**The constructor shall initialize a new Handshaker (Proton) object to handle communication handshake.**]**

**SRS_AMQPSIOTHUBCONNECTION_15_005: [**The constructor shall initialize a new FlowController (Proton) object to handle communication flow.**]**

**SRS_AMQPSIOTHUBCONNECTION_15_006: [**The constructor shall set its state to CLOSED.**]**


### open

```java
public void open() throws IOException
```

**SRS_AMQPSIOTHUBCONNECTION_15_007: [**If the AMQPS connection is already open, the function shall do nothing.**]**

**SRS_AMQPSIOTHUBCONNECTION_15_008: [**The function shall create a new sasToken valid for the duration specified in config to be used for the communication with IoTHub.**]**

**SRS_AMQPSIOTHUBCONNECTION_15_009: [**The function shall trigger the Reactor (Proton) to begin running.**]**

**SRS_AMQPSIOTHUBCONNECTION_15_010: [**The function shall wait for the reactor to be ready and for enough link credit to become available.**]**

**SRS_AMQPSIOTHUBCONNECTION_15_011: [**If any exception is thrown while attempting to trigger the reactor, the function shall close the connection and throw an IOException.**]**


### close

```java
public synchronized void close()
```

**SRS_AMQPSIOTHUBCONNECTION_15_048 [**If the AMQPS connection is already closed, the function shall do nothing.**]**

**SRS_AMQPSIOTHUBCONNECTION_15_012: [**The function shall set the status of the AMQPS connection to CLOSED.**]**

**SRS_AMQPSIOTHUBCONNECTION_15_013: [**The function shall close the AMQPS sender and receiver links, the AMQP session and the AMQP connection.**]**

**SRS_AMQPSIOTHUBCONNECTION_15_014: [**The function shall stop the Proton reactor.**]**


### sendMessage

```java
public Integer sendMessage(Message message)
```

**SRS_AMQPSIOTHUBCONNECTION_15_015: [**If the state of the connection is CLOSED or there is not enough credit, the function shall return -1.**]**

**SRS_AMQPSIOTHUBCONNECTION_15_016: [**The function shall encode the message and copy the contents to the byte buffer.**]**

**SRS_AMQPSIOTHUBCONNECTION_15_017: [**The function shall set the delivery tag for the sender.**]**

**SRS_AMQPSIOTHUBCONNECTION_15_018: [**The function shall attempt to send the message using the sender link.**]**

**SRS_AMQPSIOTHUBCONNECTION_15_019: [**The function shall advance the sender link.**]**

**SRS_AMQPSIOTHUBCONNECTION_15_020: [**The function shall set the delivery hash to the value returned by the sender link.**]**

**SRS_AMQPSIOTHUBCONNECTION_15_021: [**The function shall return the delivery hash.**]**


### sendMessageResult

```java
public Boolean sendMessageResult(AmqpsMessage message, IotHubMessageResult result)
```

**SRS_AMQPSIOTHUBCONNECTION_15_022: [**If the AMQPS Connection is closed, the function shall return false.**]**

**SRS_AMQPSIOTHUBCONNECTION_15_023: [**If the message result is COMPLETE, ABANDON, or REJECT, the function shall acknowledge the last message with acknowledgement type COMPLETE, ABANDON, or REJECT respectively.**]**

**SRS_AMQPSIOTHUBCONNECTION_15_024: [**The function shall return true after the message was acknowledged.**]**


## onConnectionInit

```java
public void onConnectionInit(Event event)
```

**SRS_AMQPSIOTHUBCONNECTION_15_025: [**The event handler shall get the Connection (Proton) object from the event handler and set the host name on the connection.**]**

**SRS_AMQPSIOTHUBCONNECTION_15_026: [**The event handler shall create a Session (Proton) object from the connection.**]**

**SRS_AMQPSIOTHUBCONNECTION_15_027: [**The event handler shall create a Receiver and Sender (Proton) links and set the protocol tag on them to a predefined constant.**]**

**SRS_AMQPSIOTHUBCONNECTION_15_028: [**The Receiver and Sender links shall have the properties set to client version identifier.**]**

**SRS_AMQPSIOTHUBCONNECTION_15_029: [**The event handler shall open the connection, session, sender and receiver objects.**]**


## onConnectionBound

```java
public void onConnectionBound(Event event)
```

**SRS_AMQPSIOTHUBCONNECTION_15_030: [**The event handler shall get the Transport (Proton) object from the event.**]**

**SRS_AMQPSIOTHUBCONNECTION_15_031: [**The event handler shall set the SASL_PLAIN authentication on the transport using the given user name and sas token.**]**

**SRS_AMQPSIOTHUBCONNECTION_15_032: [**The event handler shall set VERIFY_PEER authentication mode on the domain of the Transport.**]**


## onReactorInit

```java
public void onReactorInit(Event event)
```

**SRS_AMQPSIOTHUBCONNECTION_15_033: [**The event handler shall set the current handler to handle the connection events.**]**


## onDelivery

```java
public void onDelivery(Event event)
```

**SRS_AMQPSIOTHUBCONNECTION_15_034: [**If this link is the Receiver link, the event handler shall get the Receiver and Delivery (Proton) objects from the event.**]**

**SRS_AMQPSIOTHUBCONNECTION_15_035: [**The event handler shall read the received buffer.**]**

**SRS_AMQPSIOTHUBCONNECTION_15_036: [**The event handler shall create an AmqpsMessage object from the decoded buffer.**]**

**SRS_AMQPSIOTHUBCONNECTION_15_037: [**The event handler shall set the AmqpsMessage Deliver (Proton) object.**]**

**SRS_AMQPSIOTHUBCONNECTION_15_038: [**If this link is the Sender link and the event type is DELIVERY, the event handler shall get the Delivery (Proton) object from the event.**]**

**SRS_AMQPSIOTHUBCONNECTION_15_039: [**The event handler shall note the remote delivery state and use it and the Delivery (Proton) hash code to inform the AmqpsIotHubConnection of the message receipt.**]**

**SRS_AMQPSIOTHUBCONNECTION_15_049: [**All the listeners shall be notified that a message was received from the server.**]**


## onLinkFlow

```java
public void onLinkFlow(Event event)
```

**SRS_AMQPSIOTHUBCONNECTION_15_040 [**The event handler shall save the remaining link credit.**]**


## onLinkRemoteOpen

```java
public void onLinkRemoteOpen(Event event)
```

**SRS_AMQPSIOTHUBCONNECTION_15_041 [**The connection state shall be considered OPEN when the sender link is open remotely.**]**


## onLinkRemoteClose

```java
public void onLinkRemoteClose(Event event)
```

**SRS_AMQPSIOTHUBCONNECTION_15_042 [**The event handler shall attempt to reconnect to the IoTHub.**]**


## onLinkInit

```java
public void onLinkInit(Event event)
```

**SRS_AMQPSIOTHUBCONNECTION_15_043: [**If the link is the Sender link, the event handler shall create a new Target (Proton) object using the sender endpoint address member variable.**]**

**SRS_AMQPSIOTHUBCONNECTION_15_044: [**If the link is the Sender link, the event handler shall set its target to the created Target (Proton) object.**]**

**SRS_AMQPSIOTHUBCONNECTION_15_045: [**If the link is the Sender link, the event handler shall set the SenderSettleMode to UNSETTLED.**]**

**SRS_AMQPSIOTHUBCONNECTION_15_046: [**If the link is the Receiver link, the event handler shall create a new Source (Proton) object using the receiver endpoint address member variable.**]**

**SRS_AMQPSIOTHUBCONNECTION_15_047: [**If the link is the Receiver link, the event handler shall set its source to the created Source (Proton) object.**]**


## onTransportError

```java
public void onTransportError(Event event)
```

**SRS_AMQPSIOTHUBCONNECTION_15_048 [**The event handler shall attempt to reconnect to IoTHub.**]**
