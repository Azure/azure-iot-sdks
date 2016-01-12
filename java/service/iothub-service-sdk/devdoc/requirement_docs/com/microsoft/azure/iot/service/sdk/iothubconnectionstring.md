# IotHubConnectionString Requirements

## Overview

Expose all connections string properties and methods for user, device and connection string serialization.

## References

([IoT Hub SDK.doc](https://microsoft.sharepoint.com/teams/Azure_IoT/_layouts/15/WopiFrame.aspx?sourcedoc={9A552E4B-EC00-408F-AE9A-D8C2C37E904F}&file=IoT%20Hub%20SDK.docx&action=default))

## Exposed API

```java
public class IotHubConnectionString extends IotHubConnectionStringBuilder
{
    public String getUserString();
    public URL getUrlDevice(String deviceId);
    public URL getUrlDeviceList(Integer maxCount);
    public URL getDeviceStatistics();
    public String toString();
}
```

### getUserString

```java
public String getUserString();
```
**SRS_SERVICE_SDK_JAVA_IOTHUBCONNECTIONSTRING_12_001: [** The function shall serialize the object properties to a string using the following format: SharedAccessKeyName@sas.root.IotHubName **]**

### getUrlDevice

```java
public URL getUrlDevice(String deviceId) throws MalformedURLException, IllegalArgumentException
```
**SRS_SERVICE_SDK_JAVA_IOTHUBCONNECTIONSTRING_12_002: [** The function shall throw IllegalArgumentException if the input string is empty or null **]**

**SRS_SERVICE_SDK_JAVA_IOTHUBCONNECTIONSTRING_12_003: [** The function shall create a URL object from the given deviceId using the following format: https:hostname/devices/deviceName?api-version=201X-XX-XX **]**

### getUrlDeviceList

```java
public URL getUrlDeviceList(Integer maxCount) throws MalformedURLException, IllegalArgumentException
```
**SRS_SERVICE_SDK_JAVA_IOTHUBCONNECTIONSTRING_12_004: [** The constructor shall throw IllegalArgumentException if the input integer null or less then 1 **]**

**SRS_SERVICE_SDK_JAVA_IOTHUBCONNECTIONSTRING_12_005: [** The function shall create a URL object from the given integer using the following format: https:hostname/devices/?maxCount=XX&api-version=201X-XX-XX **]**

### getDeviceStatistics

```java
public URL getUrlDeviceStatistics() throws MalformedURLException;
```
**SRS_SERVICE_SDK_JAVA_IOTHUBCONNECTIONSTRING_12_006: [** The function shall create a URL object from the object properties using the following format: https:hostname/statistics/devices?api-version=201X-XX-XX **]**

### getUrlCreateExportImportJob

```java
public URL getUrlCreateExportImportJob() throws MalformedURLException;
```
**SRS_SERVICE_SDK_JAVA_IOTHUBCONNECTIONSTRING_15_008: [** The function shall create a URL object from the object properties using the following format: https:hostname/jobs/create?api-version=201X-XX-XX **]**

### getUrlImportExportJob

```java
public URL getUrlImportExportJob(String jobId) throws MalformedURLException
```
**SRS_SERVICE_SDK_JAVA_IOTHUBCONNECTIONSTRING_15_009: [** The function shall create a URL object from the object properties using the following format: https:hostname/jobs/jobId?api-version=201X-XX-XX **]**

### toString

```java
protected String toString();
```
**SRS_SERVICE_SDK_JAVA_IOTHUBCONNECTIONSTRING_12_007: [** The function shall serialize the object to a string using the following format: HostName=HOSTNAME.b.c.d;SharedAccessKeyName=ACCESSKEYNAME;SharedAccessKey=1234567890abcdefghijklmnopqrstvwxyz=;SharedAccessSignature= **]**
