# MessageProperty Requirements

## Overview

An IoT Hub message property.

## References

Exposed API

```java
public final class MessageProperty
{
    public MessageProperty(String name, String value);

    public String getName();
    public String getValue();

    public boolean hasName(String name);

    public static boolean isValidAppProperty(String name, String value);
} 
```


### MessageProperty

```java
public MessageProperty(String name, String value);
```

**SRS_MESSAGEPROPERTY_11_001: [**The constructor shall save the property name and value.**]**

**SRS_MESSAGEPROPERTY_11_002: [**If the name contains a character that is not in US-ASCII printable characters or is one of: ()<>@,;:\"/[]?={} (space) (horizontal tab), the function shall throw an IllegalArgumentException.**]**

**SRS_MESSAGEPROPERTY_11_003: [**If the value contains a character that is not in US-ASCII printable characters or is one of: ()<>@,;:\"/[]?={} (space) (horizontal tab), the function shall throw an IllegalArgumentException.**]**

**SRS_MESSAGEPROPERTY_11_008: [**If the name is a reserved property name, the function shall throw an IllegalArgumentException.**]**



### getName

```java
public String getName();
```

**SRS_MESSAGEPROPERTY_11_004: [**The function shall return the property name.**]**


### getValue

```java
public String getValue();
```

**SRS_MESSAGEPROPERTY_11_005: [**The function shall return the property value.**]**


### hasName

```java
public boolean hasName(String name);
```

**SRS_MESSAGEPROPERTY_11_006: [**The function shall return true if and only if the property has the given name, where the names are compared in a case-insensitive manner.**]**


### isValidAppProperty

```java
public static boolean isValidAppProperty(String name, String  value);
```

**SRS_MESSAGEPROPERTY_11_007: [**The function shall return true if and only if the name and value only use characters in: US-ASCII printable characters, excluding ()<>@,;:\"/[]?={} (space) (horizontal tab), and the name is not a reserved property name.**]**