# IotHubEventUri Requirements
 
## Overview

A URI for a device to send events to an IoT Hub.

## References

## Exposed API

```java
public final class IotHubEventUri extends IotHubUri
{
    public static final String EVENT_PATH = "/messages/events";

    public IotHubEventUri(String iotHubHostname, String deviceId);

    @Override public String toString();
    public String getHostname();
    public String getPath();
}
```


### IotHubEventUri

```java
public IotHubEventUri(String iotHubHostname, String deviceId);
```

**SRS_IOTHUBEVENTURI_11_001: [**The constructor returns a URI with the format `[iotHubHostname]/devices/[deviceId]/messages/events?api-version=2016-02-03`.**]**


### toString

```java
@Override public String toString();
```

**SRS_IOTHUBEVENTURI_11_002: [**The string representation of the IoT Hub event URI shall be constructed with the format `[iotHubHostname]/devices/[deviceId]/messages/events?api-version=2016-02-03 `.**]**


### getHostname

```java
public String getHostname();
```

**SRS_IOTHUBEVENTURI_11_003: [**The function shall return the hostname given in the constructor.**]**


### getPath

```java
public String getPath();
```

**SRS_IOTHUBEVENTURI_11_004: [**The function shall return a URI with the format `/devices/[deviceId]/messages/events`.**]**