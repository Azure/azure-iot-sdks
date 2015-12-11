# IotHubConnectionStringBuilder Requirements

## Overview

Provide static constructors to create IotHubConnectionString object

## References

([IoT Hub SDK.doc](https://microsoft.sharepoint.com/teams/Azure_IoT/_layouts/15/WopiFrame.aspx?sourcedoc={9A552E4B-EC00-408F-AE9A-D8C2C37E904F}&file=IoT%20Hub%20SDK.docx&action=default))

## Exposed API

```java
public class IotHubConnectionStringBuilder
{
    public IotHubConnectionString createConnectionString(String connectionString);
    public IotHubConnectionString createConnectionString(String hostname, AuthenticationMethod authenticationMethod);
    protected static void parse(String connectionString, IotHubConnectionString iotHubConnectionString);
    protected static void parseIotHubName(String hostName, IotHubConnectionString iotHubConnectionString);
    protected static void validate(IotHubConnectionString iotHubConnectionString);
    protected static void validateFormat(String value, String propertyName, String regex);
    protected static void validateFormatIfSpecified(String value, String propertyName, String regex);
    protected static void setHostName(String value, IotHubConnectionString iotHubConnectionString);
    protected static void setAuthenticationMethod(AuthenticationMethod authenticationMethod, IotHubConnectionString iotHubConnectionString);
}
```

### createConnectionString

```java
public IotHubConnectionString createConnectionString(String connectionString);
```
**SRS_SERVICE_SDK_JAVA_IOTHUBCONNECTIONSTRINGBUILDER_12_001: [** The function shall throw IllegalArgumentException if the input string is empty or null **]**

**SRS_SERVICE_SDK_JAVA_IOTHUBCONNECTIONSTRINGBUILDER_12_002: [** The function shall create a new IotHubConnectionString object deserializing the given string **]**

### createConnectionString

```java
public IotHubConnectionString createConnectionString(String hostname, AuthenticationMethod authenticationMethod);
```
**SRS_SERVICE_SDK_JAVA_IOTHUBCONNECTIONSTRINGBUILDER_12_003: [** The function shall throw IllegalArgumentException if the input string is empty or null **]**

**SRS_SERVICE_SDK_JAVA_IOTHUBCONNECTIONSTRINGBUILDER_12_004: [** The function shall throw IllegalArgumentException if the input authenticationMethod is null **]**

**SRS_SERVICE_SDK_JAVA_IOTHUBCONNECTIONSTRINGBUILDER_12_005: [** The function shall create a new IotHubConnectionString object using the given hostname and auhtenticationMethod **]**

### parse

```java
protected static void parse(String connectionString, IotHubConnectionString iotHubConnectionString);
```
**SRS_SERVICE_SDK_JAVA_IOTHUBCONNECTIONSTRINGBUILDER_12_006: [** The function shall throw IllegalArgumentException if the input string is empty or null **]**

**SRS_SERVICE_SDK_JAVA_IOTHUBCONNECTIONSTRINGBUILDER_12_007: [** The function shall throw IllegalArgumentException if the input target itoHubConnectionString is null **]**

**SRS_SERVICE_SDK_JAVA_IOTHUBCONNECTIONSTRINGBUILDER_12_008: [** The function shall throw exception if tokenizing or parsing failed **]**

**SRS_SERVICE_SDK_JAVA_IOTHUBCONNECTIONSTRINGBUILDER_12_009: [** The function shall tokenize and parse the given connection string and fill up the target IotHubConnectionString object with proper values **]**

**SRS_SERVICE_SDK_JAVA_IOTHUBCONNECTIONSTRINGBUILDER_12_010: [** The function shall create   new ServiceAuthenticationWithSharedAccessPolicyToken and set the authenticationMethod if sharedAccessKey is not defined **]**

**SRS_SERVICE_SDK_JAVA_IOTHUBCONNECTIONSTRINGBUILDER_12_011: [** The function shall create   new ServiceAuthenticationWithSharedAccessPolicyKey and set the authenticationMethod if the sharedAccessSignature is not defined **]**

**SRS_SERVICE_SDK_JAVA_IOTHUBCONNECTIONSTRINGBUILDER_12_012: [** The function shall validate the connection string object **]**

### parseIotHubName

```java
protected static void parseIotHubName(String hostName, IotHubConnectionString iotHubConnectionString);
```
**SRS_SERVICE_SDK_JAVA_IOTHUBCONNECTIONSTRINGBUILDER_12_013: [** The function shall return the substring of the host name until the first “.” character **]**

**SRS_SERVICE_SDK_JAVA_IOTHUBCONNECTIONSTRINGBUILDER_12_014: [** The function shall return empty string if “.” character was not found **]**

### validate

```java
protected static void validate(IotHubConnectionString iotHubConnectionString);
```
**SRS_SERVICE_SDK_JAVA_IOTHUBCONNECTIONSTRINGBUILDER_12_015: [** The function shall throw IllegalArgumentException if the sharedAccessKeyName of the input itoHubConnectionString is null or empty **]**

**SRS_SERVICE_SDK_JAVA_IOTHUBCONNECTIONSTRINGBUILDER_12_016: [** The function shall throw IllegalArgumentException if either of the sharedAccessKey or the sharedAccessSignature of the input itoHubConnectionString is empty **]**

**SRS_SERVICE_SDK_JAVA_IOTHUBCONNECTIONSTRINGBUILDER_12_017: [** The function shall call property validation functions for hostname, sharedAccessKeyName, sharedAccessKey, sharedAccessSignature **]**

### validateFormat

```java
protected static void validateFormat(String value, String propertyName, String regex);
```
**SRS_SERVICE_SDK_JAVA_IOTHUBCONNECTIONSTRINGBUILDER_12_018: [** The function shall validate the property value against the given regex **]**

**SRS_SERVICE_SDK_JAVA_IOTHUBCONNECTIONSTRINGBUILDER_12_019: [** The function shall throw IllegalArgumentException if the value did not match with the pattern **]**

### validateFormatIfSpecified

```java
protected static void validateFormatIfSpecified(String value, String propertyName, String regex);
```
**SRS_SERVICE_SDK_JAVA_IOTHUBCONNECTIONSTRINGBUILDER_12_020: [** The function shall validate the property value against the given regex if the value is not null or empty **]**

### setHostName

```java
protected static void setHostName(String value, IotHubConnectionString iotHubConnectionString);
```
**SRS_SERVICE_SDK_JAVA_IOTHUBCONNECTIONSTRINGBUILDER_12_021: [** The function shall validate the given hostName **]**

**SRS_SERVICE_SDK_JAVA_IOTHUBCONNECTIONSTRINGBUILDER_12_022: [** The function shall parse and set the hostname to the given target iotHubConnectionString object **]**

### setAuthenticationMethod

```java
protected static void setAuthenticationMethod(AuthenticationMethod authenticationMethod, IotHubConnectionString iotHubConnectionString);
```
**SRS_SERVICE_SDK_JAVA_IOTHUBCONNECTIONSTRINGBUILDER_12_023: [** The function shall populate and set the authenticationMethod on the given target iotHubConnectionString object **]**
