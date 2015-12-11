# Device Requirements

## Overview

The Device class is the main DTO for the RegistryManager class. It implements static constructors and stores device parameters.

## References

([IoT Hub SDK.doc](https://microsoft.sharepoint.com/teams/Azure_IoT/_layouts/15/WopiFrame.aspx?sourcedoc={9A552E4B-EC00-408F-AE9A-D8C2C37E904F}&file=IoT%20Hub%20SDK.docx&action=default))

## Exposed API

```java
public class Device
{
    public static Device createFromId(String deviceId);
    public static Device createFromJson(String jsonString);
    protected Device(String deviceId);
    protected Device(JsonObject jsonObject);
    protected String serializeToJson();
}
```
**SRS_SERVICE_SDK_JAVA_DEVICE_12_001: [** The Device class has the following properties: Id, Etag, Authentication.SymmetricKey, State, StateReason, StateUpdatedTime, ConnectionState, ConnectionStateUpdatedTime, LastActivityTime **]**

### createFromId

```java
public static Device createFromId(String deviceId);
```
**SRS_SERVICE_SDK_JAVA_DEVICE_12_002: [** The constructor shall throw IllegalArgumentException if the input string is empty or null **]**

**SRS_SERVICE_SDK_JAVA_DEVICE_12_003: [** The constructor shall create a new instance of Device using the given id and return with it **]**

### createFromJson

```java
public static Device createFromJson(String jsonString);
```
**SRS_SERVICE_SDK_JAVA_DEVICE_12_004: [** The constructor shall throw IllegalArgumentException if the input string is empty or null **]**

**SRS_SERVICE_SDK_JAVA_DEVICE_12_005: [** The constructor shall create a JsonObject by parsing the given jsonString **]**

**SRS_SERVICE_SDK_JAVA_DEVICE_12_006: [** The constructor shall create a new instance of Device using the created JsonObject and return with it **]**

### Device

```java
protected Device(String deviceId);
```
**SRS_SERVICE_SDK_JAVA_DEVICE_12_007: [** The constructor shall throw IllegalArgumentException if the input string is empty or null **]**

**SRS_SERVICE_SDK_JAVA_DEVICE_12_008: [** The constructor shall create a new SymmetricKey instance using AES encryption and store it into a member variable **]**

**SRS_SERVICE_SDK_JAVA_DEVICE_12_009: [** The constructor shall initialize all properties to default value **]**

### Device

```java
protected Device(JsonObject jsonObject);
```
**SRS_SERVICE_SDK_JAVA_DEVICE_12_010: [** The constructor shall throw IllegalArgumentException if the input object is null **]**

**SRS_SERVICE_SDK_JAVA_DEVICE_12_011: [** The constructor shall initialize all properties from the given Json object **]**

**SRS_SERVICE_SDK_JAVA_DEVICE_12_012: [** The constructor shall throw IllegalArgumentException if the device id is empty or null **]**

**SRS_SERVICE_SDK_JAVA_DEVICE_12_013: [** The constructor shall throw Exception if the device authentication key is empty or null **]**

### serializeToJson

```java
protected String serializeToJson();
```
**SRS_SERVICE_SDK_JAVA_DEVICE_12_014: [** The function shall return with a proper Json string containing all properties **]**
