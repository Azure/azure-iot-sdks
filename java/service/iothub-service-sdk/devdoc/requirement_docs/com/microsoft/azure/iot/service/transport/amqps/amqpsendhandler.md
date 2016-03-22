# AMQPSENDHANDLER Requirements

## Overview

Instance of the QPID-Proton-J BaseHandler class overriding the events what are needed to handle the send operation. Contains and sets connection parameters (path, port, endpoint). Maintains the layers of AMQP protocol (Link, Session, Connection, Transport). Creates and sets SASL authentication for transport. 

## References

## Exposed API

```java
public class AmqpSendHandler 
{
    AmqpSendHandler(String hostName, String userName, String sasToken);
    public void createBinaryMessage(String deviceId, String content);
    public void onConnectionBound(Event event);
    public void onConnectionInit(Event event);
    public void onLinkInit(Event event);
    public void onLinkFlow(Event event);
    public void onTransportError(Event event);
}
```

### AmqpSendHandler

```java
AmqpSendHandler(String hostName, String userName, String sasToken);
```
**SRS_SERVICE_SDK_JAVA_AMQPSENDHANDLER_12_001: [** The constructor shall throw IllegalArgumentException if any of the input parameter is null or empty **]**

**SRS_SERVICE_SDK_JAVA_AMQPSENDHANDLER_12_002: [** The constructor shall copy all input parameters to private member variables for event processing **]**

**SRS_SERVICE_SDK_JAVA_AMQPSENDHANDLER_12_003: [** The constructor shall concatenate the host name with the port **]**

**SRS_SERVICE_SDK_JAVA_AMQPSENDHANDLER_12_004: [** The constructor shall initialize a new Handshaker (Proton) object to handle communication handshake **]**

### createBinaryMessage

```java
public void createBinaryMessage(String deviceId, String content);
```
**SRS_SERVICE_SDK_JAVA_AMQPSENDHANDLER_12_005: [** The function shall create a new Message (Proton) object **]**

**SRS_SERVICE_SDK_JAVA_AMQPSENDHANDLER_12_006: [** The function shall set the standard properties on the Proton Message object **]**

**SRS_SERVICE_SDK_JAVA_AMQPSENDHANDLER_15_023: [** The function shall set the application properties on the Proton Message object **]**

**SRS_SERVICE_SDK_JAVA_AMQPSENDHANDLER_12_007: [** The function shall create a Binary (Proton) object from the content string **]**

**SRS_SERVICE_SDK_JAVA_AMQPSENDHANDLER_12_008: [** The function shall create a data Section (Proton) object from the Binary **]**

**SRS_SERVICE_SDK_JAVA_AMQPSENDHANDLER_12_009: [** The function shall set the Message body to the created data section **]**

### onConnectionBound

```java
public void onConnectionBound(Event event);
```
**SRS_SERVICE_SDK_JAVA_AMQPSENDHANDLER_12_010: [** The event handler shall set the SASL PLAIN authentication on the Transport using the given user name and sas token **]**

**SRS_SERVICE_SDK_JAVA_AMQPSENDHANDLER_12_011: [** The event handler shall set ANONYMUS_PEER authentication mode on the domain of the Transport **]**

**SRS_SERVICE_SDK_JAVA_AMQPSENDHANDLER_12_023: [** The event handler shall not initialize WebSocket if the protocol is AMQP **]**

**SRS_SERVICE_SDK_JAVA_AMQPSENDHANDLER_12_024: [** The event handler shall initialize WebSocket if the protocol is AMQP_WS **]**

### onConnectionInit

```java
public void onConnectionInit(Event event);
```
**SRS_SERVICE_SDK_JAVA_AMQPSENDHANDLER_12_012: [** The event handler shall set the host name on the connection **]**

**SRS_SERVICE_SDK_JAVA_AMQPSENDHANDLER_12_013: [** The event handler shall create a Session (Proton) object from the connection **]**

**SRS_SERVICE_SDK_JAVA_AMQPSENDHANDLER_12_014: [** The event handler shall create a Sender (Proton) object and set the protocol tag on it to a predefined constant **]**

**SRS_SERVICE_SDK_JAVA_AMQPSENDHANDLER_12_015: [** The event handler shall open the Connection, the Session and the Sender object **]**

**SRS_SERVICE_SDK_JAVA_AMQPSENDHANDLER_15_023: [** The Sender object shall have the properties set to service client version identifier.**]**

### onLinkInit

```java
public void onLinkInit(Event event);
```
**SRS_SERVICE_SDK_JAVA_AMQPSENDHANDLER_12_016: [** The event handler shall create a new Target (Proton) object using the given endpoint address **]**

**SRS_SERVICE_SDK_JAVA_AMQPSENDHANDLER_12_017: [** The event handler shall get the Link (Proton) object and set its target to the created Target (Proton) object **]**

### onLinkFlow

```java
public void onLinkFlow(Event event);
```
**SRS_SERVICE_SDK_JAVA_AMQPSENDHANDLER_12_018: [** The event handler shall get the Sender (Proton) object from the link **]**

**SRS_SERVICE_SDK_JAVA_AMQPSENDHANDLER_12_019: [** The event handler shall encode the message and copy to the byte buffer **]**

**SRS_SERVICE_SDK_JAVA_AMQPSENDHANDLER_12_020: [** The event handler shall set the delivery tag on the Sender (Proton) object **]**

**SRS_SERVICE_SDK_JAVA_AMQPSENDHANDLER_12_021: [** The event handler shall send the encoded bytes **]**

**SRS_SERVICE_SDK_JAVA_AMQPSENDHANDLER_12_022: [** The event handler shall close the Sender, Session and Connection **]**
