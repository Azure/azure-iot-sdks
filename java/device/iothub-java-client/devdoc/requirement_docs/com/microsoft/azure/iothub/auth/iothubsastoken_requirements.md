# IotHubSasToken Requirements
 
## Overview

Grants device access to an IoT Hub for the specified amount of time.

## References

## Exposed API


```java
public final class IotHubSasToken
{
  public static final String TOKEN_FORMAT = "SharedAccessSignature sig=%s&se=%s&skn=%s&sr=%s";
  public IotHubSasToken(String resourceUri, String deviceId, String deviceKey, long expiryTime;
  @Override public String toString();
}
```


### IotHubSasToken

```java
public IotHubSasToken(String resourceUri, String deviceId, String deviceKey, long expiryTime);
```
    
**SRS_IOTHUBSASTOKEN_11_001: [**The SAS token shall have the format `SharedAccessSignature sig=<signature >&se=<expiryTime>&skn=&sr=<resourceURI>`. The params can be in any order.**]**
Note: the ‘skn’ attribute must be an empty string for connecting to IoT Hub.

**SRS_IOTHUBSASTOKEN_11_002: [**The expiry time shall be the given expiry time, where it is a UNIX timestamp and indicates the time after which the token becomes invalid.**]**

**SRS_IOTHUBSASTOKEN_11_003: [**The key name shall be the device ID.**]**

**SRS_IOTHUBSASTOKEN_11_004: [**The resource URI shall be the given resource URI.**]**

**SRS_IOTHUBSASTOKEN_11_005: [**The signature shall be correctly computed and set.**]**


### toString

```java
@Override public String toString();
```
    
**SRS_IOTHUBSASTOKEN_11_006: [**The function shall return the string representation of the SAS token.**]**

**SRS_IOTHUBSASTOKEN_08_012: [**The skn key value shall be empty if connecting to IoT Hub, or the device Id if connecting to the Event Hub directly**]**