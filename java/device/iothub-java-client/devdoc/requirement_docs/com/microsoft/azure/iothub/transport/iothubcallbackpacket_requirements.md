# IotHubCallbackPacket Requirements

## Overview

A packet containing the data needed for an IoT Hub transport to invoke a callback on a completed request.

## References

## Exposed API
```java
public final class IotHubCallbackPacket
{
    public IotHubCallbackPacket(IotHubStatusCode status, IotHubEventCallback callback, Object callbackContext);

    public IotHubStatusCode getStatus();
    public IotHubEventCallback getCallback();
    public Object getContext();
}
```

### IotHubCallbackPacket

```java
public IotHubCallbackPacket(IotHubStatusCode status, IotHubEventCallback callback, Object callbackContext);
```

**SRS_IOTHUBCALLBACKPACKET_11_001: [**The constructor shall save the status, callback, and callback context.**]**


### getStatus

```java
public IotHubStatusCode getStatus();
```

**SRS_IOTHUBCALLBACKPACKET_11_002: [**The function shall return the status given in the constructor.**]**


### getCallback

```java
public IotHubEventCallback getCallback();
```

**SRS_IOTHUBCALLBACKPACKET_11_003: [**The function shall return the callback given in the constructor.**]**


### getContext

```java
public Object getContext();
```

**SRS_IOTHUBCALLBACKPACKET_11_004: [**The function shall return the callback context given in the constructor.**]**
