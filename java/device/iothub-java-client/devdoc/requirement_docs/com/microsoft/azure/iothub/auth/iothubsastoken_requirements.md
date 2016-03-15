# IotHubSasToken Requirements
 
## Overview

Grants device access to an IoT Hub for the specified amount of time.

## References

## Exposed API


```java
public final class IotHubSasToken
{
  public static final String TOKEN_FORMAT = "SharedAccessSignature sig=%s&se=%s&sr=%s";
  public IotHubSasToken(String resourceUri, String deviceId, String deviceKey, long expiryTime;
  @Override public String toString();
}
```


### IotHubSasToken

```java
public IotHubSasToken(String scope, String key, long expiryTime);
```
    
**SRS_IOTHUBSASTOKEN_11_001: [**The SAS token shall have the format `SharedAccessSignature sig=<signature >&se=<expiryTime>&sr=<resourceURI>`. The params can be in any order.**]**
Note: the ‘skn’ attribute must be an empty string for connecting to IoT Hub.

**SRS_IOTHUBSASTOKEN_11_002: [**The constructor shall save all input parameters to member variables.**]**

**SRS_IOTHUBSASTOKEN_11_005: [**The signature shall be correctly computed and set.**]**

### toString

```java
@Override public String toString();
```
    
**SRS_IOTHUBSASTOKEN_11_006: [**The function shall return the string representation of the SAS token.**]**