# AmqpSend Requirements

## Overview

Instance of the QPID-Proton-J BaseHandler class overriding the events what are needed to handle high level open, close and send methods. Initialize and use AmqpSendHandler class for low level ampqs operationsAmqpS. 

## References

## Exposed API

```java
public class AmqpSend 
{
    AmqpSend(String hostName, String userName, String sasToken);
    public void onReactorInit(Event event);
    public void open();
    public void close();
    public void send(String deviceId, String content);
}
```

### AmqpSend

```java
AmqpSend(String hostName, String userName, String sasToken);
```
**SRS_SERVICE_SDK_JAVA_AMQPSEND_12_001: [** The constructor shall throw IllegalArgumentException if any of the input parameter is null or empty **]**

**SRS_SERVICE_SDK_JAVA_AMQPSEND_12_002: [** The constructor shall copy all input parameters to private member variables for event processing **]**

### onReactorInit

```java
public void onReactorInit(Event event);
```
**SRS_SERVICE_SDK_JAVA_AMQPSEND_12_003: [** The event handler shall set the member AmqpSendHandler object to handle the given connection events **]**

### open

```java
public void open();
```
**SRS_SERVICE_SDK_JAVA_AMQPSEND_12_004: [** The function shall create an AmqpSendHandler object to handle reactor events **]**

### close

```java
public void close();
```
**SRS_SERVICE_SDK_JAVA_AMQPSEND_12_005: [** The function shall invalidate the member AmqpSendHandler object **]**

### send

```java
public void send(String deviceId, String content);
```
**SRS_SERVICE_SDK_JAVA_AMQPSEND_12_006: [** The function shall create a Proton message with the given content **]**

**SRS_SERVICE_SDK_JAVA_AMQPSEND_12_007: [** The function shall initialize the Proton reactor object **]**

**SRS_SERVICE_SDK_JAVA_AMQPSEND_12_008: [** The function shall start the Proton reactor object **]**

**SRS_SERVICE_SDK_JAVA_AMQPSEND_12_009: [** The function shall throw IOException if the send handler object is not initialized **]**
