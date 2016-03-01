# AmqpsIotHubConnection Requirements
 
## Overview

An AMQPS IotHub Session for a device and an IotHub.

## References

## Exposed API

```java
public final class AmqpsIotHubConnection extends BaseHandler
{
	public AmqpsIotHubConnection(DeviceClientConfig config);
	public void open() throws IOException, InterruptedException, ExecutionException;
	public synchronized void close();
	public Message consumeMessage() throws IOException;
	public void sendMessageRestult(IotHubMessageResult result) throws IOException;
	public CompletableFuture<Boolean> scheduleSend(byte[] content[, Object messageId);
	public void onReactorInit(Event event);
}
```


### AmqpsIotHubConnection

```java
public AmqpsIotHubConnection(DeviceClientConfig config);
```

**SRS_AMQPSIOTHUBCONNECTION_14_001: [**The constructor shall save the configuration.**]**

**SRS_AMQPSIOTHUBCONNECTION_14_002: [**The constructor shall throw a new IllegalArgumentException if any of the parameters of the configuration is null or empty.**]**

**SRS_AMQPSIOTHUBCONNECTION_14_003: [**The constructor shall create a new SAS token and copy all input parameters to private member variables.**]**

**SRS_AMQPSIOTHUBCONNECTION_14_004: [**The constructor shall set it’s state to CLOSED.**]**

**SRS_AMQPSIOTHUBCONNECTION_14_005: [**The constructor shall initialize a new private queue for received messages.**]**

**SRS_AMQPSIOTHUBCONNECTION_14_006: [**The constructor shall initialize a new private map for messages that are in progress.**]**

**SRS_AMQPSIOTHUBCONNECTION_14_007: [**The constructor shall initialize new private Futures for the status of the Connection and Reactor.**]**


### open

```java
public void open() throws IOException, InterruptedException, ExecutionException
```

**SRS_AMQPSIOTHUBCONNECTION_14_008: [**The function shall initialize it’s AmqpsIotHubConnectionBaseHandler using the saved host name, user name, device ID and sas token.**]**

**SRS_AMQPSIOTHUBCONNECTION_14_009: [**The function shall open the Amqps connection and trigger the Reactor (Proton) to begin running.**]**

**SRS_AMQPSIOTHUBCONNECTION_14_010: [**Once the Reactor (Proton) is ready, the function shall set its state to OPEN.**]**

**SRS_AMQPSIOTHUBCONNECTION_14_011: [**If the AMQPS connection is already open, the function shall do nothing.**]**

**SRS_AMQPSIOTHUBCONNECTION_14_012: [**If the AmqpsIotHubConnectionBaseHandler becomes invalidated before the Reactor (Proton) starts, the function shall throw an IOException.**]**

**SRS_AMQPSIOTHUBCONNECTION_14_031: [** The function shall get the link credit from its AmqpsIotHubConnectionBaseHandler and set the private maxQueueSize member variable. **]**

**SRS_AMQPSIOTHUBCONNECTION_14_032: [** The function shall successfully complete it’s CompletableFuture status member variable. **]**

### close

```java
public synchronized void close()
```

**SRS_AMQPSIOTHUBCONNECTION_14_013: [**The function shall invalidate the private Reactor (Proton) member variable.**]**

**SRS_AMQPSIOTHUBCONNECTION_14_014: [**The function shall free the AmqpsIotHubConnectionBaseHandler.**]**

**SRS_AMQPSIOTHUBCONNECTION_14_015: [**The function shall close the AMQPS connection.**]**

**SRS_AMQPSIOTHUBCONNECTION_14_016: [**If the AMQPS connection is already closed, the function shall do nothing.**]**

**SRS_AMQPSIOTHUBCONNECTION_14_033: [**The function shall close the AmqpsIotHubConnectionBaseHandler.**]**

**SRS_AMQPSIOTHUBCONNECTION_14_034: [**The function shall exceptionally complete all remaining messages that are currently in progress and clear the queue.**]**

### consumeMessage

```java
public Message consumeMessage() throws IOException
```

**SRS_AMQPSIOTHUBCONNECTION_14_019: [**The function shall attempt to remove a message from the queue.**]**

**SRS_AMQPSIOTHUBCONNECTION_14_020: [**The function shall return the message if one was pulled from the queue, otherwise it shall return null.**]**


### sendMessageResult

```java
public void sendMessageResult(IotHubMessageResult result) throws IOException
```

**SRS_AMQPSIOTHUBCONNECTION_14_021: [**If the message result is COMPLETE, ABANDON, or REJECT, the function shall acknowledge the last message with acknowledgement type COMPLETE, ABANDON, or REJECT respectively.**]**

**SRS_AMQPSIOTHUBCONNECTION_14_022: [**If sendMessageResult(result) is called before a message is received, the function shall throw an IllegalStateException.**]**

**SRS_AMQPSIOTHUBCONNECTION_14_023: [**If the acknowledgement fails, the function shall throw an IOException.**]**

**SRS_AMQPSIOTHUBCONNECTION_14_024: [**If the AMQPS Connection is closed, the function shall throw an IllegalStateException.**]**


### scheduleSend

```java
public CompletableFuture<Boolean> scheduleSend(byte[] content[, Object messageId) throws IOException
```

**SRS_AMQPSIOTHUBCONNECTION_14_025: [**If the AmqpsIotHubConnectionBaseHandler has not been initialized, the function shall throw a new IOException and exceptionally complete it’s CompletableFuture status member variable with the same exception.**]**

**SRS_AMQPSIOTHUBCONNECTION_14_026: [**The function shall create a new CompletableFuture for the message acknowledgement.**]**

**SRS_AMQPSIOTHUBCONNECTION_14_027: [**The function shall create a new Tuple containing the CompletableFuture, message content, and message ID.**]**

**SRS_AMQPSIOTHUBCONNECTION_14_028: [**The function shall acquire a lock and attempt to send the message.**]**

**SRS_AMQPSIOTHUBCONNECTION_14_029: [**If the AMQPS Connection is closed, the function shall throw an IllegalStateException.**]**


## onReactorInit

```java
public void onReactorInit(Event event)
```

**SRS_AMQPSIOTHUBCONNECTION_14_030: [**The event handler shall set the member AmqpsIotHubConnectionBaseHandler object to handle the connection events.**]**