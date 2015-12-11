# HttpsBatchMessage Requirements

## Overview

Builds a batched IoT Hub request body as a JSON array.

## References

## Exposed API

```java
public class HttpsBatchMessage implements HttpsMessage
{
    public static String BATCH_CONTENT_TYPE = "application/vnd.microsoft.iothub.json";
    public static Charset BATCH_CHARSET = StandardCharsets.UTF_8;

    public HttpsBatchMessage();
    public void addMessage(HttpsSingleMessage msg);
    public byte[] getBody();
    public String getContentType();
    public MessageProperty[] getProperties();
    public int numMessages();
}
```


### HttpsBatchMessage

```java
public HttpsBatchMessage();
```

**SRS_HTTPSBATCHMESSAGE_11_001: [**The constructor shall initialize the batch message with the body as an empty JSON array.**]**


### addMessage

```java
public void addMessage(HttpsSingleMessage msg);
```

**SRS_HTTPSBATCHMESSAGE_11_002: [**The function shall add the message as a JSON object appended to the current JSON array.**]**

**SRS_HTTPSBATCHMESSAGE_11_003: [**The JSON object shall have the field "body" set to the raw message.**]**

**SRS_HTTPSBATCHMESSAGE_11_004: [**The JSON object shall have the field "base64Encoded" set to whether the raw message was Base64-encoded.**]**

**SRS_HTTPSBATCHMESSAGE_11_005: [**The JSON object shall have the field "properties" set to a JSON object which has a key-value pair for each message property, where the key is the HTTPS property name and the value is the property value.**]**

**SRS_HTTPSBATCHMESSAGE_11_009: [**If the function throws a SizeLimitExceededException, the batched message shall remain as if the message was never added.**]**


### byte

```java
public byte[] getBody();
```

**SRS_HTTPSBATCHMESSAGE_11_006: [**The function shall return the current batch message body as a byte array.**]**

**SRS_HTTPSBATCHMESSAGE_11_007: [**The batch message body shall be encoded using UTF-8.**]**


### getContentType

```java
public String getContentType();
```

**SRS_HTTPSBATCHMESSAGE_11_011: [**The function shall return 'application/vnd.microsoft.iothub.json'.**]**


### getProperties

```java
public MessageProperty[] getProperties();
```

**SRS_HTTPSBATCHMESSAGE_11_012: [**The function shall return an empty array.**]**


### numMessages

```java
public int numMessages();
```

**SRS_HTTPSBATCHMESSAGE_11_010: [**The function shall return the number of messages currently in the batch.**]**