# DeviceClientConfig Requirements

## Overview

Configuration settings for an IoT Hub device client. Validates all user-defined settings.

## References

## Exposed API

```java
public final class DeviceClientConfig
{
    private long tokenValidSecs = 3600;
    public static final int DEFAULT_READ_TIMEOUT_MILLIS = 240000;
    public static final int DEFAULT_MESSAGE_LOCK_TIMEOUT_SECS = 180;

    public DeviceClientConfig(String iotHubHostname, String deviceId, String deviceKey, String sharedAccessToken);
    public String getIotHubName();
    public String getDeviceId();
    public String getDeviceKey();
    public String getSharedAccessToken();
    public long getTokenValidSecs();
    public int getReadTimeoutMillis();

    public String getPathToCertificate();
    public void setPathToCert(String pathToCertificate);

    public void setMessageCallback(MessageCallback callback, Object context);

    public MessageCallback getMessageCallback();
    public Object getMessageContext();
    public int getMessageLockTimeoutSecs();
}
```


### DeviceClientConfig

```java
public DeviceClientConfig(String iotHubHostname, String deviceId, String deviceKey, String sharedAccessToken);
```

** SRS_DEVICECLIENTCONFIG_11_001: [**The constructor shall save the IoT Hub hostname, device ID, and device key.**] **

** SRS_DEVICECLIENTCONFIG_11_014: [**If the IoT Hub hostname is not valid URI, the constructor shall throw a URISyntaxException.**] **

** SRS_DEVICECLIENTCONFIG_11_015: [**If the IoT Hub hostname does not contain a '.', the function shall throw an IllegalArgumentException.**] **

** SRS_DEVICECLIENTCONFIG_25_017: [**The constructor shall save sharedAccessToken.**] **

** SRS_DEVICECLIENTCONFIG_11_016: [**The constructor shall set the default certificate which can be later modified by the user.**] **


### getIotHubHostname

```java
public String getIotHubHostname();
```

** SRS_DEVICECLIENTCONFIG_11_002: [**The function shall return the IoT Hub hostname given in the constructor.**] **


### getIotHubName

```java
public String getIotHubName();
```

** SRS_DEVICECLIENTCONFIG_11_002: [**The function shall return the IoT Hub name given in the constructor, where the IoT Hub name is embedded in the IoT Hub hostname as follows: [IoT Hub name].[valid HTML chars]+.**] ** 


### getDeviceId

```java
public String getDeviceId();
```

** SRS_DEVICECLIENTCONFIG_11_003: [**The function shall return the device ID given in the constructor.**] **


### getDeviceKey

```java
public String getDeviceKey();
```

** SRS_DEVICECLIENTCONFIG_11_004: [**The function shall return the device key given in the constructor.**] **


### getSharedAccessToken

```java
public String getSharedAccessToken();
```

** SRS_DEVICECLIENTCONFIG_25_018: [**The function shall return the SharedAccessToken given in the constructor.**] **


### getMessageValidSecs

```java
public long getMessageValidSecs();
```

** SRS_DEVICECLIENTCONFIG_11_005: [**The function shall return the value of tokenValidSecs.**] **

### setTokenValidSecs

```java
public setTokenValidSecs(long expiryTime);
```

** SRS_DEVICECLIENTCONFIG_25_016: [**The function shall set the value of tokenValidSecs.**] **

### getPathToCertificate

```java
public String getPathToCertificate();
```

** SRS_DEVICECLIENTCONFIG_11_014: [**The function shall return the value of the path to the certificate.**] **

### setPathToCert

```java
public void setPathToCert(String pathToCertificate);
```

** SRS_DEVICECLIENTCONFIG_25_015: [**The function shall set the path to the certificate**] **


### setMessageCallback

```java
public void setMessageCallback(MessageCallback  callback, Object context);
```

** SRS_DEVICECLIENTCONFIG_11_006: [**The function shall set the message callback, with its associated context.**] ** 


### getReadTimeoutMillis

```java
public int getReadTimeoutMillis();
```

** SRS_DEVICECLIENTCONFIG_11_012: [**The function shall return 240000ms.**] **


### getMessageCallback

```java
public MessageCallback getMessageCallback();
```

** SRS_DEVICECLIENTCONFIG_11_010: [**The function shall return the current message callback.**] ** 


### getMessageContext

```java
public Object getMessageContext();
```

** SRS_DEVICECLIENTCONFIG_11_011: [**The function shall return the current message context.**] **


### getMessageLockTimeoutSecs

```java
public int getMessageLockTimeoutSecs();
```

** SRS_DEVICECLIENTCONFIG_11_013: [**The function shall return 180s.**] **