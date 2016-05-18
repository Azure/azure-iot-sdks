# Message Requirements

## Overview

A message to or from an IoT Hub.

## References

## Exposed API

```java
public final class Message
{
    public static final Charset DEFAULT_IOTHUB_MESSAGE_CHARSET = StandardCharsets.UTF_8;
    
    public Message(byte[] body);

    public Message setProperty(String name, String value);
    public String getProperty(String name);
    public MessageProperty[] getProperties();
    public boolean isExpired();

    public byte[] getBytes();
}
```


### Message

```java
public Message(byte[] body);
```

**SRS_MESSAGE_11_024: [**The constructor shall save the message body.**]**

**SRS_MESSAGE_11_025: [**If the message body is null, the constructor shall throw an IllegalArgumentException.**]**


### setProperty

```java
public Message setProperty(String name, String value);
```

**SRS_MESSAGE_11_026: [**The function shall set the message property to the given value.**]**

**SRS_MESSAGE_11_028: [**If name is null, the function shall throw an IllegalArgumentException.**]**

**SRS_MESSAGE_11_029: [**If value is null, the function shall throw an IllegalArgumentException.**]**

**SRS_MESSAGE_11_030: [**If name contains a character not specified in RFC 2047, the function shall throw an IllegalArgumentException.**]**

**SRS_MESSAGE_11_031: [**If value name contains a character not specified in RFC 2047, the function shall throw an IllegalArgumentException.**]**


### getProperty

```java
public String getProperty(String name);
```

**SRS_MESSAGE_11_032: [**The function shall return the value associated with the message property name, where the name can be either the HTTPS or AMQPS property name.**]**

**SRS_MESSAGE_11_034: [**If no value associated with the property name is found, the function shall throw an IllegalArgumentException.**]**


### getProperties

```java
public MessageProperty[] getProperties();
```

**SRS_MESSAGE_11_033: [**The function shall return a copy of the message properties.**]**


### isExpired()

```java
public boolean isExpired();
```

**SRS_MESSAGE_15_035: [**The function shall return false if the expiry time is 0.**]**

**SRS_MESSAGE_15_036: [**The function shall return true if the current time is greater than the expiry time and false otherwise.**]**


### getBytes

```java
public byte[] getBytes();
```

**SRS_MESSAGE_11_002: [**The function shall return the message body.**]**