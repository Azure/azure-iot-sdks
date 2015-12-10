# DeviceClientConfig Requirements

## Overview

Configuration settings for an IoT Hub device client. Validates all user-defined settings.

## References

## Exposed API

```java
public final class DeviceClientConfig
{
    public static final long TOKEN_VALID_SECS = 600;
    public static final int DEFAULT_READ_TIMEOUT_MILLIS = 240000;
    public static final int DEFAULT_MESSAGE_LOCK_TIMEOUT_SECS = 180;
    public AzureHubType targetHubType;

    public DeviceClientConfig(String iotHubHostname, String deviceId, String deviceKey);
    public DeviceClientConfig(String iotHubHostname, String gatewayHostName, String deviceId, String deviceKey);
    public String getIotHubName();
    public String getDeviceId();
    public String getDeviceKey();
    public long getTokenValidSecs();
    public int getReadTimeoutMillis();

    public void setMessageCallback(MessageCallback callback, Object context);

    public MessageCallback getMessageCallback();
    public Object getMessageContext();
    public int getMessageLockTimeoutSecs();
}
```


### DeviceClientConfig

```java
public DeviceClientConfig(String iotHubHostname, String deviceId, String deviceKey);
```

** DeviceClientConfigDEVICECLIENTCONFIG_11_001: [The constructor shall save the IoT Hub hostname, Protocol Gateway hostname, device ID, and device key.] **

** DeviceClientConfigDEVICECLIENTCONFIG_11_014: [If the IoT Hub hostname and Protocol Gateway hostname are not valid URIs, the constructor shall throw a URISyntaxException.] **

** DeviceClientConfigDEVICECLIENTCONFIG_11_015: [If the IoT Hub hostname does not contain a '.', the function shall throw an IllegalArgumentException.] **


### targetHubType

```java
public AzureHubType targetHubType;
```

** DeviceClientConfigDEVICECLIENTCONFIG_08_012: [Configuration shall expose an option to define if client will connect to an IoT Hub or an Event Hub directly] **


### getIotHubHostname

```java
public String getIotHubHostname();
```

** DeviceClientConfigDEVICECLIENTCONFIG_11_002: [The function shall return the IoT Hub hostname given in the constructor.] **


### getIotHubName

```java
public String getIotHubName();
```

** DeviceClientConfigDEVICECLIENTCONFIG_11_002: [The function shall return the IoT Hub name given in the constructor, where the IoT Hub name is embedded in the IoT Hub hostname as follows: [IoT Hub name].[valid HTML chars]+.] ** 


### getDeviceId

```java
public String getDeviceId();
```

** DeviceClientConfigDEVICECLIENTCONFIG_11_003: [The function shall return the device ID given in the constructor.] **


### getDeviceKey

```java
public String getDeviceKey();
```

** DeviceClientConfigDEVICECLIENTCONFIG_11_004: [The function shall return the device key given in the constructor.] **


### getMessageValidSecs

```java
public long getMessageValidSecs();
```

** DeviceClientConfigDEVICECLIENTCONFIG_11_005: [The function shall return 600s.] **


### setMessageCallback

```java
public void setMessageCallback(MessageCallback  callback, Object context);
```

** DeviceClientConfigDEVICECLIENTCONFIG_11_006: [The function shall set the message callback, with its associated context.] ** 


### getReadTimeoutMillis

```java
public int getReadTimeoutMillis();
```

** DeviceClientConfigDEVICECLIENTCONFIG_11_012: [The function shall return 240000ms.] **


### getMessageCallback

```java
public MessageCallback getMessageCallback();
```

** DeviceClientConfigDEVICECLIENTCONFIG_11_010: [The function shall return the current message callback.] ** 


### getMessageContext

```java
public Object getMessageContext();
```

** DeviceClientConfigDEVICECLIENTCONFIG_11_011: [The function shall return the current message context.] **


### getMessageLockTimeoutSecs

```java
public int getMessageLockTimeoutSecs();
```

** DeviceClientConfigDEVICECLIENTCONFIG_11_013: [The function shall return 180s.] **