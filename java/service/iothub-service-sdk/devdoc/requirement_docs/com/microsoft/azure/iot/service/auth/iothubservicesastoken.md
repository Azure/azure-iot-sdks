# IotHubServiceSasToken Requirements

## Overview

Grants device access to an IoT Hub for the specified amount of time. 

## References

## Exposed API

```java
public class IotHubServiceSasToken
{
    public IotHubServiceSasToken(IotHubConnectionString iotHubConnectionString);
    public void toString();
}
```

### IotHubServiceSasToken

```java
public IotHubServiceSasToken(IotHubConnectionString iotHubConnectionString);
```
**SRS_SERVICE_SDK_JAVA_IOTHUBSERVICESASTOKEN_12_001: [** The constructor shall throw IllegalArgumentException if the input object is null **]**

**SRS_SERVICE_SDK_JAVA_IOTHUBSERVICESASTOKEN_12_002: [** The constructor shall create a target uri from the url encoded host name) **]**

**SRS_SERVICE_SDK_JAVA_IOTHUBSERVICESASTOKEN_12_003: [** The constructor shall create a string to sign by concatenating the target uri and the expiry time string (one year) **]**

**SRS_SERVICE_SDK_JAVA_IOTHUBSERVICESASTOKEN_12_004: [** The constructor shall create a key from the shared access key signing with HmacSHA256 **]**

**SRS_SERVICE_SDK_JAVA_IOTHUBSERVICESASTOKEN_12_005: [** The constructor shall compute the final signature by url encoding the signed key **]**

**SRS_SERVICE_SDK_JAVA_IOTHUBSERVICESASTOKEN_12_006: [** The constructor shall concatenate the target uri, the signature, the expiry time and the key name using the format: "SharedAccessSignature sr=%s&sig=%s&se=%s&skn=%s" **]**

**SRS_SERVICE_SDK_JAVA_IOTHUBSERVICESASTOKEN_12_007: [** The constructor shall throw Exception if building the token failed **]**

### toString

```java
public void toString();
```
**SRS_SERVICE_SDK_JAVA_IOTHUBSERVICESASTOKEN_12_008: [** The function shall return with the generated token **]**
