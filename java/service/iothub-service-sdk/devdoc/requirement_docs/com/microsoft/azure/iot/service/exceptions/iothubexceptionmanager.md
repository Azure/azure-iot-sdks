# IotHubExceptionManager Requirements

## Overview

Provide static function to verify results and throw appropriate exception. 

## References

## Exposed API

```java
public class IotHubExceptionManager
{
    public static void HttpResponseVerification(HttpResponse httpResponse);
}
```

### HttpResponseVerification

```java
public static void HttpResponseVerification(HttpResponse httpResponse);
```
**SRS_SERVICE_SDK_JAVA_IOTHUBEXCEPTIONMANAGER_12_001: [** The function shall throw IotHubBadFormatException if the Http response status equal 400 **]**

**SRS_SERVICE_SDK_JAVA_IOTHUBEXCEPTIONMANAGER_12_002: [** The function shall throw IotHubUnathorizedException if the Http response status equal 401 **]**

**SRS_SERVICE_SDK_JAVA_IOTHUBEXCEPTIONMANAGER_12_003: [** The function shall throw IotHubTooManyDevicesException if the Http response status equal 403 **]**

**SRS_SERVICE_SDK_JAVA_IOTHUBEXCEPTIONMANAGER_12_004: [** The function shall throw IotHubNotFoundException if the Http response status equal 404 **]**

**SRS_SERVICE_SDK_JAVA_IOTHUBEXCEPTIONMANAGER_12_005: [** The function shall throw IotHubPreconditionFailedException if the Http response status equal 412 **]**

**SRS_SERVICE_SDK_JAVA_IOTHUBEXCEPTIONMANAGER_12_006: [** The function shall throw IotHubTooManyRequestsException if the Http response status equal 429 **]**

**SRS_SERVICE_SDK_JAVA_IOTHUBEXCEPTIONMANAGER_12_007: [** The function shall throw IotHubInternalServerErrorException if the Http response status equal 500 **]**

**SRS_SERVICE_SDK_JAVA_IOTHUBEXCEPTIONMANAGER_12_008: [** The function shall throw IotHubServerBusyException if the Http response status equal 503 **]**

**SRS_SERVICE_SDK_JAVA_IOTHUBEXCEPTIONMANAGER_12_009: [** The function shall throw IotHubException if the Http response status none of them above and greater than 300 copying the error Http reason to the exception **]**

**SRS_SERVICE_SDK_JAVA_IOTHUBEXCEPTIONMANAGER_12_010: [** The function shall return without exception if the response status equal or less than 300 **]**
