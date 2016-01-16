# FeedbackReceiver Requirements

## Overview

FeedbackReceiver is a specialized receiver whose ReceiveAsync method returns a FeedbackBatch instead of a Message.

## References

([IoT Hub SDK.doc](https://microsoft.sharepoint.com/teams/Azure_IoT/_layouts/15/WopiFrame.aspx?sourcedoc={9A552E4B-EC00-408F-AE9A-D8C2C37E904F}&file=IoT%20Hub%20SDK.docx&action=default))

## Exposed API

```java
public class FeedbackReceiver extends Receiver
{
    public FeedbackReceiver(String hostname, String username, String sasToken, String deviceId);
    public void open();
    public void close();
    public FeedbackBatch receive();
    public FeedbackBatch receive(long timeoutMs);
    public CompletableFuture openAsync();
    public CompletableFuture closeAsync();
    public CompletableFuture receiveAsync();
    public CompletableFuture receiveAsync(long timeoutMs);
}
```
### FeedbackReceiver

```java
public FeedbackReceiver (String hostname, String username, String sasToken, String deviceId );
```
**SRS_SERVICE_SDK_JAVA_FEEDBACKRECEIVER_12_001: [** The constructor shall throw IllegalArgumentException if any the input string is null or empty **]**

**SRS_SERVICE_SDK_JAVA_FEEDBACKRECEIVER_12_002: [** The constructor shall store deviceId **]**

**SRS_SERVICE_SDK_JAVA_FEEDBACKRECEIVER_12_003: [** The constructor shall create a new instance of AmqpReceive object **]**

### open

```java
public void open();
```
**SRS_SERVICE_SDK_JAVA_FEEDBACKRECEIVER_12_004: [** The function shall throw IOException if the member AMQPReceive object has not been initialized **]**

**SRS_SERVICE_SDK_JAVA_FEEDBACKRECEIVER_12_005: [** The function shall call open() on the member AMQPReceive object **]**

### close

```java
public void close();
```
**SRS_SERVICE_SDK_JAVA_FEEDBACKRECEIVER _12_006: [** The function shall throw IOException if the member AMQPReceive object has not been initialized **]**

**SRS_SERVICE_SDK_JAVA_FEEDBACKRECEIVER _12_007: [** The function shall call close() on the member AMQPReceive object **]**

### receive

```java
public FeedbackBatch receive();
```
**SRS_SERVICE_SDK_JAVA_FEEDBACKRECEIVER _12_008: [** The function shall call receive(long timeoutMs) function with the default timeout **]**

### receive

```java
public FeedbackBatch receive(long timeoutMs);
```
**SRS_SERVICE_SDK_JAVA_FEEDBACKRECEIVER _12_009: [** The function shall throw IOException if the member AMQPReceive object has not been initialized **]**

**SRS_SERVICE_SDK_JAVA_FEEDBACKRECEIVER _12_010: [** The function shall call receive() on the member AMQPReceive object and return with the result **]**

### openAsync

```java
public CompletableFuture openAsync();
```
**SRS_SERVICE_SDK_JAVA_FEEDBACKRECEIVER _12_011: [** The function shall create an async wrapper around the open() function call **]**

### closeAsync

```java
public CompletableFuture closeAsync();
```
**SRS_SERVICE_SDK_JAVA_FEEDBACKRECEIVER _12_012: [** The function shall create an async wrapper around the close() function call **]**

### receiveAsync

```java
public CompletableFuture receiveAsync();
```
**SRS_SERVICE_SDK_JAVA_FEEDBACKRECEIVER _12_013: [** The function shall create an async wrapper around the receive() function call using the default timeout **]**

### receiveAsync

```java
public CompletableFuture receiveAsync(long timeoutMs);
```
**SRS_SERVICE_SDK_JAVA_FEEDBACKRECEIVER _12_014: [** The function shall create an async wrapper around the receive(long timeoutMs) function call **]**
