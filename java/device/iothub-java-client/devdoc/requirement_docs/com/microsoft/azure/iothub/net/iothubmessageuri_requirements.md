# IotHubMessageUri Requirements
 
## Overview

A URI for a device to poll for messages from an IoT Hub.

## References

## Exposed API

```java
public final class IotHubMessageUri
{
    public static final String MESSAGE_PATH = "/messages/devicebound";

    public IotHubMessageUri(String iotHubHostname, String deviceId);

    @Override public String toString();
    public String getHostname();
    public String getPath();
}
```


### IotHubMessageUri

```java
public IotHubMessageUri(String iotHubHostname, String deviceId); 
```

**SRS_IOTHUBMESSAGEURI_11_001: [**The constructor returns a URI with the format `[iotHubHostname]/devices/[deviceId]/messages/devicebound?api-version=2016-02-03`.**]**


### toString

```java
@Override public String toString();
```

**SRS_IOTHUBMESSAGEURI_11_002: [**The string representation of the IoT Hub event URI shall be constructed with the format `[iotHubHostname]/devices/[deviceId]/messages/devicebound?api-version=2016-02-03`.**]**


### getHostname

```java
public String getHostname();
```

**SRS_IOTHUBMESSAGEURI_11_003: [**The function shall return the hostname given in the constructor.**]**


### getPath

```java
public String getPath();
```

**SRS_IOTHUBMESSAGEURI_11_004: [**The function shall return a URI with the format `/devices/[deviceId]/messages/devicebound`.**]**