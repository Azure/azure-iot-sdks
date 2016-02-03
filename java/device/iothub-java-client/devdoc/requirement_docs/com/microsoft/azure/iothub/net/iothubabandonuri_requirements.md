# IotHubAbandonUri Requirements
 
## Overview

A URI for a device to instruct an IoT Hub to mark a message as being abandoned.

## References

## Exposed API

```java
public final class IotHubAbandonUri
{
    public static final String ABANDON_PATH_FORMAT = "/messages/devicebound/%s/abandon";

    public IotHubAbandonUri(String iotHubHostname, String deviceId, String eTag);
    
    @Override public String toString();
    public String getHostname();
    public String getPath();
}
```


### IotHubAbandonUri

```java
public IotHubAbandonUri(String iotHubHostname, String deviceId, String eTag); 
```

**SRS_IOTHUBABANDONURI_11_001: [**The constructor returns a URI with the format `[iotHubHostname]/devices/[deviceId]/messages/devicebound/[eTag]/abandon?api-version=2016-02-03`.**]**


### toString

```java
@Override public String toString();
```

**SRS_IOTHUBABANDONURI_11_002: [**The string representation of the IoT Hub event URI shall be constructed with the format `[iotHubHostname]/devices/[deviceId]/messages/devicebound/[eTag]/abandon?api-version=2016-02-03`.**]**


### getHostname

```java
public String getHostname();
```

**SRS_IOTHUBABANDONURI_11_003: [**The function shall return the hostname given in the constructor.**]**


### getPath

```java
public String getPath();
```

**SRS_IOTHUBABANDONURI_11_004: [**The function shall return a URI with the format `/devices/[deviceId]/messages/devicebound/[eTag]/abandon`.**]**