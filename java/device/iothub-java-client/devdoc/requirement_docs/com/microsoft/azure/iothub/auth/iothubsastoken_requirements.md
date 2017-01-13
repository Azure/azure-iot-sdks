# IotHubSasToken Requirements
 
## Overview

Grants device access to an IoT Hub for the specified amount of time.

## References

## Exposed API


```java
public final class IotHubSasToken
{
  public static final String TOKEN_FORMAT = "SharedAccessSignature sig=%s&se=%s&sr=%s";
  public IotHubSasToken(DeviceClientConfig config, long expiryTime);
  @Override public String toString();
}
```


### IotHubSasToken

```java
public IotHubSasToken(DeviceClientConfig config, long expiryTime);
```
    
**SRS_IOTHUBSASTOKEN_11_001: [**The SAS token shall have the format `SharedAccessSignature sig=<signature >&se=<expiryTime>&sr=<resourceURI>`. The params can be in any order.**]**
Note: the ‘skn’ attribute must be an empty string for connecting to IoT Hub.

**SRS_IOTHUBSASTOKEN_11_002: [**The constructor shall save all input parameters to member variables.**]**

**SRS_IOTHUBSASTOKEN_25_005: [**If device key is provided then the signature shall be correctly computed and set.**]**

**SRS_IOTHUBSASTOKEN_25_007: [**If device key is not provided in config then the SASToken from config shall be used.**]**

**SRS_IOTHUBSASTOKEN_25_008: [**The required format for the SAS Token shall be verified and IllegalArgumentException is thrown if unmatched.**]**


### toString

```java
@Override public String toString();
```
    
**SRS_IOTHUBSASTOKEN_11_006: [**The function shall return the string representation of the SAS token.**]**

**SRS_IOTHUBSASTOKEN_25_009: [**If SAS Token was provided by config it should be returned as string **]**

**SRS_IOTHUBSASTOKEN_25_010: [**If SAS Token was not provided by config it should be built and returned as string **]**

