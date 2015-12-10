# HttpsSingleMessage Requirements

## Overview
A single HTTPS message.

## References

## Exposed API

```java
public class HttpsSingleMessage implements HttpsMessage
{
    public static final String HTTPS_SINGLE_MESSAGE_CONTENT_TYPE = "binary/octet-stream";

    public static HttpsSingleMessage parseHttpsMessage(Message message);
    public static HttpsSingleMessage parseHttpsMessage(HttpsResponse response);

    public Message toMessage();

    public byte[] getBody();
    public String getBodyAsString();
    public String getContentType();
    public boolean isBase64Encoded();
    public MessageProperty[] getProperties();
}
```


### parseHttpsMessage

```java
public static HttpsSingleMessage parseHttpsMessage(Message msg);
```

**SRS_HTTPSSINGLEMESSAGE_11_001: [**The parsed HttpsSingleMessage shall have a copy of the original message body as its body.**]**

**SRS_HTTPSSINGLEMESSAGE_11_002: [**The parsed HttpsSingleMessage shall have the same base64Encoded value as that of the original message.**]**

**SRS_HTTPSSINGLEMESSAGE_11_003: [**The parsed HttpsSingleMessage shall add the prefix 'iothub-app-' to each of the message properties.**]**


### parseHttpsMessage

```java
public static HttpsSingleMessage parseHttpsMessage(HttpsResponse response);
```

**SRS_HTTPSSINGLEMESSAGE_11_004: [**The parsed HttpsSingleMessage shall have a copy of the original response body as its body.**]**

**SRS_HTTPSSINGLEMESSAGE_11_005: [**The parsed HttpsSingleMessage shall not be Base64-encoded.**]**

**SRS_HTTPSSINGLEMESSAGE_11_006: [**The parsed HttpsSingleMessage shall include all valid HTTPS application-defined properties in the response header as message properties.**]**


### toMessage

```java
public Message toMessage();
```

**SRS_HTTPSSINGLEMESSAGE_11_007: [**The function shall return an IoT Hub message with a copy of the message body as its body.**]**

**SRS_HTTPSSINGLEMESSAGE_11_008: [**The function shall return an IoT Hub message with application-defined properties that have the prefix 'iothub-app' removed.**]**


### getBody

```java
public byte[] getBody();
```

**SRS_HTTPSSINGLEMESSAGE_11_009: [**The function shall return a copy of the message body.**]**


### getBodyAsString

```java
public String getBodyAsString();
```

**SRS_HTTPSSINGLEMESSAGE_11_010: [**The function shall return the message body as a string encoded using charset UTF-8.**]**


### getContentType

```java
public String getContentType();
```

**SRS_HTTPSSINGLEMESSAGE_11_011: [**The function shall return the message content-type as 'binary/octet-stream'.**]**


### isBase64Encoded

```java
public boolean isBase64Encoded();
```

**SRS_HTTPSSINGLEMESSAGE_11_012: [**The function shall return whether the message is Base64-encoded.**]**


### getProperties

```java
public MessageProperty[] getProperties();
```

**SRS_HTTPSSINGLEMESSAGE_11_013: [**The function shall return a copy of the message properties.**]**