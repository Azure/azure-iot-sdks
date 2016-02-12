# IotHubRejectUri Requirements

## Overview

A URI for a device to instruct an IoT Hub to mark a message as being rejected.

## References

## Exposed API

```java
public final class IotHubRejectUri
{
    public static final String REJECT_PATH_FORMAT = "/messages/devicebound/%s";
    public static final Map<String, String> REJECT_QUERY_PARAM;

    public IotHubRejectUri(String iotHubHostname, String deviceId, String eTag);

    @Override public String toString();
    public String getHostname();
    public String getPath();
}
```


### IotHubRejectUri

```java
public IotHubRejectUri(String iotHubHostname, String deviceId, String eTag; 
```

**SRS_IOTHUBREJECTURI_11_001: [**The constructor returns a URI with the format `[iotHubHostname]/devices/[deviceId]/messages/devicebound/[eTag]??reject=true&api-version=2016-02-03` (the query parameters can be in any order).**]**


### toString

```java
@Override public String toString();
```

**SRS_IOTHUBREJECTURI_11_002: [**The string representation of the IoT Hub event URI shall be constructed with the format `[iotHubHostname]/devices/[deviceId]/messages/devicebound/[eTag]??reject=true&api-version=2016-02-03` (the query parameters can be in any order).**]**


### getHostname

```java
public String getHostname();
```

**SRS_IOTHUBREJECTURI_11_003: [**The function shall return the hostname given in the constructor.**]**


### getPath

```java
public String getPath();
```

**SRS_IOTHUBREJECTURI_11_004: [**The function shall return a URI with the format `/devices/[deviceId]/messages/devicebound/[eTag]`.**]**