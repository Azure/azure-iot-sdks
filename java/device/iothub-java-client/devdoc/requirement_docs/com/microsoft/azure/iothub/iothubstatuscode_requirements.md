# IotHubStatusCode Requirements

## Overview

An IoT Hub status code. Included in a message from an IoT Hub to a device.

## References

## Exposed API

```java
public enum IotHubStatusCode
{
    OK, OK_EMPTY, BAD_FORMAT, UNAUTHORIZED, TOO_MANY_DEVICES,
    HUB_OR_DEVICE_ID_NOT_FOUND, PRECONDITION_FAILED, THROTTLED,
    INTERNAL_SERVER_ERROR, SERVER_BUSY, ERROR;

    public static IotHubStatusCode getIotHubStatusCode(int httpsStatus);
}
```


### getIotHubStatusCode

```java
public static IotHubStatusCode getIotHubStatusCode(int httpsStatus);
```

**SRS_IOTHUBSTATUSCODE_11_001: [**The function shall convert the given HTTPS status code to the corresponding IoT Hub status code.**]**

**SRS_IOTHUBSTATUSCODE_11_002: [**If the given HTTPS status code does not map to an IoT Hub status code, the function return status code ERROR.**]**