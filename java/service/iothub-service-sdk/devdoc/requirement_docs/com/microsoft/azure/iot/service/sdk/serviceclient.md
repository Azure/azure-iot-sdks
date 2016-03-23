# ServiceClient Requirements

## Overview

Use the ServiceClient class to send C2D messages and receive feedback.

## References

([IoT Hub SDK.doc](https://microsoft.sharepoint.com/teams/Azure_IoT/_layouts/15/WopiFrame.aspx?sourcedoc={9A552E4B-EC00-408F-AE9A-D8C2C37E904F}&file=IoT%20Hub%20SDK.docx&action=default))

## Exposed API

```java
public class ServiceClient
{
   public static ServiceClient createFromConnectionString(String connectionString, IotHubServiceClientProtocol protocol);
   protected ServiceClient(IotHubConnectionString connectionString);
   public void open();
   public void close();
   public void send(String deviceId, String message);
   public CompletableFuture openAsync();
   public CompletableFuture closeAsync();
   public CompletableFuture sendAsync(String deviceId, String message);
   public void getFeedbackReceiver(String deviceId);
}
```
### createFromConnectionString

```java
public static ServiceClient createFromConnectionString (String connectionString, IotHubServiceClientProtocol protocol);
```
**SRS_SERVICE_SDK_JAVA_SERVICECLIENT_12_001: [** The constructor shall throw IllegalArgumentException if the input string is empty or null **]**

**SRS_SERVICE_SDK_JAVA_SERVICECLIENT_12_002: [** The constructor shall create IotHubConnectionString object using the IotHubConnectionStringBuilder **]**

**SRS_SERVICE_SDK_JAVA_SERVICECLIENT_12_003: [** The constructor shall create a new instance of ServiceClient using the created IotHubConnectionString object and the given protocol return with it **]**

### ServiceClient

```java
public static ServiceClient(IotHubConnectionString connectionString);
```
**SRS_SERVICE_SDK_JAVA_SERVICECLIENT_12_004: [** The constructor shall throw IllegalArgumentException if the input object is null **]**

**SRS_SERVICE_SDK_JAVA_SERVICECLIENT_12_005: [** The constructor shall create a SAS token object using the IotHubConnectionString **]**

**SRS_SERVICE_SDK_JAVA_SERVICECLIENT_12_006: [** The constructor shall store connection string, hostname, username and sasToken **]**

**SRS_SERVICE_SDK_JAVA_SERVICECLIENT_12_007: [** The constructor shall create a new instance of AmqpSend object **]**

### open

```java
public void open();
```
**SRS_SERVICE_SDK_JAVA_SERVICECLIENT_12_008: [** The function shall throw IOException if the member AMQP sender object has not been initialized **]**

**SRS_SERVICE_SDK_JAVA_SERVICECLIENT_12_009: [** The function shall call open() on the member AMQP sender object **]**

### close

```java
public void close();
```
**SRS_SERVICE_SDK_JAVA_SERVICECLIENT_12_010: [** The function shall throw IOException if the member AMQP sender object has not been initialized **]**

**SRS_SERVICE_SDK_JAVA_SERVICECLIENT_12_011: [** The function shall call close() on the member AMQP sender object **]**

### send

```java
public void send(String deviceId, String message);
```
**SRS_SERVICE_SDK_JAVA_SERVICECLIENT_12_012: [** The function shall throw IOException if the member AMQP sender object has not been initialized **]**

**SRS_SERVICE_SDK_JAVA_SERVICECLIENT_12_013: [** The function shall call send() on the member AMQP sender object with the given parameters **]**


### openAsync

```java
public CompletableFuture openAsync();
```
**SRS_SERVICE_SDK_JAVA_SERVICECLIENT_12_014: [** The function shall create an async wrapper around the open() function call, handle the return value or delegate exception **]**

### closeAsync

```java
public CompletableFuture closeAsync();
```
**SRS_SERVICE_SDK_JAVA_SERVICECLIENT_12_015: [** The function shall create an async wrapper around the close() function call, handle the return value or delegate exception **]**

### sendAsync

```java
public CompletableFuture sendAsync(String deviceId, String message);
```
**SRS_SERVICE_SDK_JAVA_SERVICECLIENT_12_016: [** The function shall create an async wrapper around the send() function call, handle the return value or delegate exception **]**

### getFeedbackReceiver

```java
public void getFeedbackReceiver(String deviceId);
```
**SRS_SERVICE_SDK_JAVA_SERVICECLIENT_12_017: [** The function shall create a FeedbackReceiver object and returns with it **]**
