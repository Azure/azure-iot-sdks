# IotHubUri Requirements

## Overview

A URI for a device to connect to an IoT Hub.

## References

## Exposed API

```java
public final class IotHubUri
{
    public static final String PATH_FORMAT = "/devices/%s%s";
    public static final String API_VERSION = "api-version=2016-02-03";
    public static final Charset IOTHUB_URL_ENCODING_CHARSET = StandardCharsets.UTF_8;

    public IotHubUri(String iotHubHostname, String deviceId, String iotHubMethodPath, Map<String, String> queryParams);
    public IotHubUri(String iotHubHostname, String deviceId, String iotHubMethodPath);

    @Override public String toString();
    public String getHostname();
    public String getPath();

    public static String getResourceUri(String iotHubHostname, String deviceId);
}
```


### IotHubUri

```java
public IotHubUri(String iotHubHostname, String deviceId, String iotHubMethodPath, Map<String, String> queryParams);
```

**SRS_IOTHUBURI_11_008: [**If queryParams is not empty, the constructor shall return a URI pointing to the address `[iotHubHostname]/devices/[deviceId]/[IoT Hub method path]?api-version=2016-02-03&[queryFragment]`.**]**

**SRS_IOTHUBURI_11_009: [**If the queryParams is empty, the constructor shall return a URI pointing to the address `[iotHubHostname]/devices/[deviceId]/[IoT Hub method path]?api-version=2016-02-03`.**]**

**SRS_IOTHUBURI_11_011: [**The constructor shall URL-encode the device ID.**]**

**SRS_IOTHUBURI_11_012: [**The constructor shall URL-encode the IoT Hub method path.**]**

**SRS_IOTHUBURI_11_013: [**The constructor shall URL-encode each query param.**]**


### IotHubUri

```java
public IotHubUri(String iotHubHostname, String deviceId, String iotHubMethodPath);
```

**SRS_IOTHUBURI_11_007: [**The constructor shall return a URI pointing to the address `[iotHubHostname] /devices/[deviceId]/[IoT Hub method path]?api-version=2016-02-03`.**]**

**SRS_IOTHUBURI_11_015: [**The constructor shall URL-encode the device ID.**]**

**SRS_IOTHUBURI_11_016: [**The constructor shall URL-encode the IoT Hub method path.**]**


### toString

```java
@Override public String toString();
```

**SRS_IOTHUBURI_11_001: [**The string representation of the IoT Hub URI shall be constructed with the format `[iotHubHostname]/devices/[deviceId]/[IoT Hub method path]?api-version=2016-02-03(&[queryFragment])`.**]**


### getHostname

```java
public String getHostname();
```

**SRS_IOTHUBURI_11_005: [**The function shall return the IoT hub hostname given in the constructor.**]**


### getPath

```java
public String getPath();
```

**SRS_IOTHUBURI_11_006: [**The function shall return a URI with the format `/devices/[deviceId]/[IoT Hub method path]`.**]**


### getResourceUri

```java
public static String getResourceUri(String iotHubName, String deviceId);
```

**SRS_IOTHUBURI_11_002: [**The function shall return a URI with the format `[iotHubHostname]/devices/[deviceId]`.**]**

**SRS_IOTHUBURI_11_019: [**The constructor shall URL-encode the device ID.**]**