# RegistryManager Requirements

## Overview

Use the RegistryManager class to manage the identity registry in IoT Hubs.

## References

([IoT Hub SDK.doc](https://microsoft.sharepoint.com/teams/Azure_IoT/_layouts/15/WopiFrame.aspx?sourcedoc={9A552E4B-EC00-408F-AE9A-D8C2C37E904F}&file=IoT%20Hub%20SDK.docx&action=default))

## Exposed API


```java
public class RegistryManager 
{
    public static RegistryManager createFromConnectionString(String connectionString);
    private RegistryManager(IotHubConnectionString iotHubConnectionString);
    public void open();
    public void close();
    public Device addDevice(Device device);
    public CompletableFuture<Device> addDeviceAsync(Device device);
    public Device getDevice(String deviceId);
    public CompletableFuture<Device> getDeviceAsync(String deviceId);
    public ArrayList<Device> getDevices(Integer maxCount);
    public CompletableFuture<ArrayList<Device>> getDevicesAsync(Integer maxCount);
    public Device updateDevice(Device device, Boolean forceUpdate);
    public Device updateDevice(Device device);
    public CompletableFuture<Device> updateDeviceAsync(Device device);
    public CompletableFuture<Device> updateDeviceAsync(Device device, Boolean forceUpdate);
    public void removeDevice(String deviceId);
    public CompletableFuture<Void> removeDeviceAsync(String deviceId);
    public RegistryStatistics getStatistics();
    public CompletableFuture<RegistryStatistics> getStatisticsAsync();
}
```

### createFromConnectionString

```java
public static RegistryManager createFromConnectionString(String connectionString);
```
**SRS_SERVICE_SDK_JAVA_REGISTRYMANAGER_12_001: [** The constructor shall throw IllegalArgumentException if the input string is null or empty **]**

**SRS_SERVICE_SDK_JAVA_REGISTRYMANAGER_12_002: [** The constructor shall create an IotHubConnectionString object from the given connection string **]**

**SRS_SERVICE_SDK_JAVA_REGISTRYMANAGER_12_003: [** The constructor shall create a new RegistryManager, stores the created IotHubConnectionString object and return with it **]**

### addDevice

```java
public Device addDevice(Device device);
```
**SRS_SERVICE_SDK_JAVA_REGISTRYMANAGER_12_004: [** The function shall throw IllegalArgumentException if the input device is null **]**

**SRS_SERVICE_SDK_JAVA_REGISTRYMANAGER_12_005: [** The function shall deserialize the given device object to Json string **]**

**SRS_SERVICE_SDK_JAVA_REGISTRYMANAGER_12_006: [** The function shall get the URL for the device **]**

**SRS_SERVICE_SDK_JAVA_REGISTRYMANAGER_12_007: [** The function shall create a new SAS token for the device **]**

**SRS_SERVICE_SDK_JAVA_REGISTRYMANAGER_12_008: [** The function shall create a new HttpRequest for adding the device to IotHub **]**

**SRS_SERVICE_SDK_JAVA_REGISTRYMANAGER_12_009: [** The function shall send the created request and get the response **]**

**SRS_SERVICE_SDK_JAVA_REGISTRYMANAGER_12_010: [** The function shall verify the response status and throw proper Exception **]**

**SRS_SERVICE_SDK_JAVA_REGISTRYMANAGER_12_011: [** The function shall create a new Device object from the response and return with it **]**

### addDeviceAsync

```java
public CompletableFuture<Device> addDeviceAsync(Device device);
```
**SRS_SERVICE_SDK_JAVA_REGISTRYMANAGER_12_012: [** The function shall throw IllegalArgumentException if the input device is null **]**

**SRS_SERVICE_SDK_JAVA_REGISTRYMANAGER_12_013: [** The function shall create an async wrapper around the addDevice() function call, handle the return value or delegate exception **]**

### getDevice

```java
public Device getDevice(String deviceId);
```
**SRS_SERVICE_SDK_JAVA_REGISTRYMANAGER_12_014: [** The function shall throw IllegalArgumentException if the input string is null or empty **]**

**SRS_SERVICE_SDK_JAVA_REGISTRYMANAGER_12_015: [** The function shall get the URL for the device **]**

**SRS_SERVICE_SDK_JAVA_REGISTRYMANAGER_12_016: [** The function shall create a new SAS token for the device **]**

**SRS_SERVICE_SDK_JAVA_REGISTRYMANAGER_12_017: [** The function shall create a new HttpRequest for getting a device from IotHub **]**

**SRS_SERVICE_SDK_JAVA_REGISTRYMANAGER_12_018: [** The function shall send the created request and get the response **]**

**SRS_SERVICE_SDK_JAVA_REGISTRYMANAGER_12_019: [** The function shall verify the response status and throw proper Exception **]**

**SRS_SERVICE_SDK_JAVA_REGISTRYMANAGER_12_020: [** The function shall create a new Device object from the response and return with it **]**

### getDeviceAsync

```java
public CompletableFuture<Device> getDeviceAsync(String deviceId);
```
**SRS_SERVICE_SDK_JAVA_REGISTRYMANAGER_12_021: [** The function shall throw IllegalArgumentException if the input device is null **]**

**SRS_SERVICE_SDK_JAVA_REGISTRYMANAGER_12_022: [** The function shall create an async wrapper around the getDevice() function call, handle the return value or delegate exception **]**

### getDevices

```java
public ArrayList<Device> getDevices(Integer maxCount);
```
**SRS_SERVICE_SDK_JAVA_REGISTRYMANAGER_12_023: [** The function shall throw IllegalArgumentException if the input count number is less than 1 **]**

**SRS_SERVICE_SDK_JAVA_REGISTRYMANAGER_12_024: [** The function shall get the URL for the device **]**

**SRS_SERVICE_SDK_JAVA_REGISTRYMANAGER_12_025: [** The function shall create a new SAS token for the device **]**

**SRS_SERVICE_SDK_JAVA_REGISTRYMANAGER_12_026: [** The function shall create a new HttpRequest for getting a device list from IotHub **]**

**SRS_SERVICE_SDK_JAVA_REGISTRYMANAGER_12_027: [** The function shall send the created request and get the response **]**

**SRS_SERVICE_SDK_JAVA_REGISTRYMANAGER_12_028: [** The function shall verify the response status and throw proper Exception **]**

**SRS_SERVICE_SDK_JAVA_REGISTRYMANAGER_12_029: [** The function shall create a new ArrayList<Device> object from the response and return with it **]**

### getDevicesAsync

```java
public CompletableFuture<ArrayList<Device>> getDevicesAsync(Integer maxCount);
```
**SRS_SERVICE_SDK_JAVA_REGISTRYMANAGER_12_030: [** The function shall throw IllegalArgumentException if the input count number is less than 1 **]**

**SRS_SERVICE_SDK_JAVA_REGISTRYMANAGER_12_031: [** The function shall create an async wrapper around the getDevices() function call, handle the return value or delegate exception **]**

### updateDevice

```java
public Device updateDevice(Device device);
```
**SRS_SERVICE_SDK_JAVA_REGISTRYMANAGER_12_032: [** The function shall throw IllegalArgumentException if the input device is null **]**

**SRS_SERVICE_SDK_JAVA_REGISTRYMANAGER_12_033: [** The function shall call updateDevice with forceUpdate = false **]**

### updateDevice

```java
public Device updateDevice(Device device, Boolean forceUpdate);
```
**SRS_SERVICE_SDK_JAVA_REGISTRYMANAGER_12_034: [** The function shall throw IllegalArgumentException if the input device is null **]**

**SRS_SERVICE_SDK_JAVA_REGISTRYMANAGER_12_035: [** The function shall set forceUpdate on the device **]**

**SRS_SERVICE_SDK_JAVA_REGISTRYMANAGER_12_036: [** The function shall get the URL for the device **]**

**SRS_SERVICE_SDK_JAVA_REGISTRYMANAGER_12_037: [** The function shall create a new SAS token for the device **]**

**SRS_SERVICE_SDK_JAVA_REGISTRYMANAGER_12_038: [** The function shall create a new HttpRequest for updating the device on IotHub **]**

**SRS_SERVICE_SDK_JAVA_REGISTRYMANAGER_12_039: [** The function shall send the created request and get the response **]**

**SRS_SERVICE_SDK_JAVA_REGISTRYMANAGER_12_040: [** The function shall verify the response status and throw proper Exception **]**

**SRS_SERVICE_SDK_JAVA_REGISTRYMANAGER_12_041: [** The function shall create a new Device object from the response and return with it **]**

### updateDeviceAsync

```java
public CompletableFuture<Device> updateDeviceAsync(Device device);
```
**SRS_SERVICE_SDK_JAVA_REGISTRYMANAGER_12_042: [** The function shall throw IllegalArgumentException if the input device is null **]**

**SRS_SERVICE_SDK_JAVA_REGISTRYMANAGER_12_043: [** The function shall create an async wrapper around the updateDevice() function call, handle the return value or delegate exception **]**

### updateDeviceAsync

```java
public CompletableFuture<Device> updateDeviceAsync(Device device, Boolean forceUpdate);
```
**SRS_SERVICE_SDK_JAVA_REGISTRYMANAGER_12_044: [** The function shall throw IllegalArgumentException if the input device is null **]**

**SRS_SERVICE_SDK_JAVA_REGISTRYMANAGER_12_045: [** The function shall create an async wrapper around the updateDevice(Device, device, Boolean forceUpdate) function call, handle the return value or delegate exception **]**

### removeDevice

```java
public void removeDevice(String deviceId);
```
**SRS_SERVICE_SDK_JAVA_REGISTRYMANAGER_12_046: [** The function shall throw IllegalArgumentException if the input string is null or empty **]**

**SRS_SERVICE_SDK_JAVA_REGISTRYMANAGER_12_047: [** The function shall get the URL for the device **]**

**SRS_SERVICE_SDK_JAVA_REGISTRYMANAGER_12_048: [** The function shall create a new SAS token for the device **]**

**SRS_SERVICE_SDK_JAVA_REGISTRYMANAGER_12_049: [** The function shall create a new HttpRequest for removing the device from IotHub **]**

**SRS_SERVICE_SDK_JAVA_REGISTRYMANAGER_12_050: [** The function shall send the created request and get the response **]**

**SRS_SERVICE_SDK_JAVA_REGISTRYMANAGER_12_051: [** The function shall verify the response status and throw proper Exception **]**

### removeDeviceAsync

```java
public CompletableFuture<Void> removeDeviceAsync(String deviceId);
```
**SRS_SERVICE_SDK_JAVA_REGISTRYMANAGER_12_052: [** The function shall throw IllegalArgumentException if the input string is null or empty **]**

**SRS_SERVICE_SDK_JAVA_REGISTRYMANAGER_12_053: [** The function shall create an async wrapper around the removeDevice() function call, handle the return value or delegate exception **]**

### getStatistics

```java
public RegistryStatistics getStatistics();
```
**SRS_SERVICE_SDK_JAVA_REGISTRYMANAGER_12_054: [** The function shall get the URL for the device **]**

**SRS_SERVICE_SDK_JAVA_REGISTRYMANAGER_12_055: [** The function shall create a new SAS token for the device **]**

**SRS_SERVICE_SDK_JAVA_REGISTRYMANAGER_12_056: [** The function shall create a new HttpRequest for getting statistics from IotHub **]**

**SRS_SERVICE_SDK_JAVA_REGISTRYMANAGER_12_057: [** The function shall send the created request and get the response **]**

**SRS_SERVICE_SDK_JAVA_REGISTRYMANAGER_12_058: [** The function shall verify the response status and throw proper Exception **]**

**SRS_SERVICE_SDK_JAVA_REGISTRYMANAGER_12_059: [** The function shall create a new RegistryStatistics object from the response and return with it **]**

### getStatisticsAsync

```java
public CompletableFuture<RegistryStatistics> getStatisticsAsync();
```
**SRS_SERVICE_SDK_JAVA_REGISTRYMANAGER_12_060: [** The function shall create an async wrapper around the getStatistics() function call, handle the return value or delegate exception **]**
