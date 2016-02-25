# HttpsIotHubConnection Requirements

## Overview

An HTTPS connection between a device and an IoT Hub.

## References

## Exposed API

```java
public class HttpsIotHubConnection
{
    public HttpsIotHubConnection(DeviceClientConfig config);

    public IotHubStatusCode sendEvent(HttpsMessage msg) throws IOException;

    public Message receiveMessage() throws IOException;
    public void sendMessageResult(IotHubMessageResult result) throws IOException;
}
```


### HttpsIotHubConnection

```java
public HttpsIotHubConnection(DeviceClientConfig config);
```

**SRS_HTTPSIOTHUBCONNECTION_11_001: [**The constructor shall save the client configuration.**]**


### sendEvent

```Java
public IotHubStatusCode sendEvent(HttpsMessage msg) throws IOException;**
```

**SRS_HTTPSIOTHUBCONNECTION_11_002: [**The function shall send a request to the URL https://[iotHubHostname]/devices/[deviceId]/messages/events?api-version=2016-02-03.**]**

**SRS_HTTPSIOTHUBCONNECTION_11_003: [**The function shall send a POST request.**]**

**SRS_HTTPSIOTHUBCONNECTION_11_004: [**The function shall set the request body to the message body.**]**

**SRS_HTTPSIOTHUBCONNECTION_11_005: [**The function shall write each message property as a request header.**]**

**SRS_HTTPSIOTHUBCONNECTION_11_006: [**The function shall set the request read timeout to be the configuration parameter readTimeoutMillis.**]**

**SRS_HTTPSIOTHUBCONNECTION_11_007: [**The function shall set the header field 'authorization' to be a valid SAS token generated from the configuration parameters.**]**

**SRS_HTTPSIOTHUBCONNECTION_11_008: [**The function shall set the header field 'iothub-to' to be '/devices/[deviceId]/messages/events'.**]**

**SRS_HTTPSIOTHUBCONNECTION_11_009: [**The function shall set the header field 'content-type' to be the message content type.**]**

**SRS_HTTPSIOTHUBCONNECTION_11_010: [**The function shall return the IoT Hub status code included in the response.**]**

**SRS_HTTPSIOTHUBCONNECTION_11_012: [**If the IoT Hub could not be reached, the function shall throw an IOException.**]**


### receiveMessage

```java
public Message receiveMessage() throws IOException;
```

**SRS_HTTPSIOTHUBCONNECTION_11_013: [**The function shall send a request to the URL https://[iotHubHostname]/devices/[deviceId]/messages/devicebound?api-version=2016-02-03.**]**

**SRS_HTTPSIOTHUBCONNECTION_11_014: [**The function shall send a GET request.**]**

**SRS_HTTPSIOTHUBCONNECTION_11_015: [**The function shall set the request read timeout to be the configuration parameter readTimeoutMillis.**]**

**SRS_HTTPSIOTHUBCONNECTION_11_016: [**The function shall set the header field 'authorization' to be a valid SAS token generated from the configuration parameters.**]**

**SRS_HTTPSIOTHUBCONNECTION_11_017: [**The function shall set the header field 'iothub-to' to be '/devices/[deviceId]/messages/devicebound'.**]**

**SRS_HTTPSIOTHUBCONNECTION_11_018: [**The function shall set the header field 'iothub-messagelocktimeout' to be the configuration parameter messageLockTimeoutSecs.**]**

**SRS_HTTPSIOTHUBCONNECTION_11_019: [**If a response with IoT Hub status code OK is received, the function shall return the IoT Hub message included in the response.**]**

**SRS_HTTPSIOTHUBCONNECTION_11_020: [**If a response with IoT Hub status code OK is received, the function shall save the response header field 'etag'.**]**

**SRS_HTTPSIOTHUBCONNECTION_11_021: [**If a response with IoT Hub status code OK is not received, the function shall return null.**]**

**SRS_HTTPSIOTHUBCONNECTION_11_023: [**If the IoT Hub could not be reached, the function shall throw an IOException.**]**


### sendMessageResult

```java
public void sendMessageResult(IotHubMessageResult result) throws IOException;
```

**SRS_HTTPSIOTHUBCONNECTION_11_024: [**If the result is COMPLETE, the function shall send a request to the URL https://[iotHubHostname]/devices/[deviceId]/messages/devicebound/[eTag]?api-version=2016-02-03.**]**

**SRS_HTTPSIOTHUBCONNECTION_11_025: [**If the result is COMPLETE, the function shall send a DELETE request.**]**

**SRS_HTTPSIOTHUBCONNECTION_11_026: [**If the result is COMPLETE, the function shall set the header field 'iothub-to' to be '/devices/[deviceId]/messages/devicebound/[eTag]'.**]**

**SRS_HTTPSIOTHUBCONNECTION_11_027: [**If the result is ABANDON, the function shall send a request to the URL https://[iotHubHostname]/devices/[deviceId]/messages/devicebound/[eTag]/abandon?api-version=2016-02-03.**]**

**SRS_HTTPSIOTHUBCONNECTION_11_028: [**If the result is ABANDON, the function shall send a POST request.**]**

**SRS_HTTPSIOTHUBCONNECTION_11_029: [**If the result is ABANDON, the function shall set the header field 'iothub-to' to be /devices/[deviceId]/messages/devicebound/[eTag]/abandon.**]**

**SRS_HTTPSIOTHUBCONNECTION_11_030: [**If the result is REJECT, the function shall send a request to the URL https://[iotHubHostname]/devices/[deviceId]/messages/devicebound/[eTag]??reject=true&api-version=2016-02-03 (the query parameters can be in any order).**]**

**SRS_HTTPSIOTHUBCONNECTION_11_031: [**If the result is REJECT, the function shall send a DELETE request.**]**

**SRS_HTTPSIOTHUBCONNECTION_11_032: [**If the result is REJECT, the function shall set the header field 'iothub-to' to be '/devices/[deviceId]/messages/devicebound/[eTag]'.**]**

**SRS_HTTPSIOTHUBCONNECTION_11_033: [**The function shall set the request read timeout to be the configuration parameter readTimeoutMillis.**]**

**SRS_HTTPSIOTHUBCONNECTION_11_034: [**The function shall set the header field 'authorization' to be a valid SAS token generated from the configuration parameters.**]**

**SRS_HTTPSIOTHUBCONNECTION_11_035: [**The function shall set the header field 'if-match' to be the e-tag saved when receiveMessage() was previously called.**]**

**SRS_HTTPSIOTHUBCONNECTION_11_037: [**If the IoT Hub could not be reached, the function shall throw an IOException.**]**

**SRS_HTTPSIOTHUBCONNECTION_11_038: [**If the IoT Hub status code in the response is not OK_EMPTY, the function shall throw an IOException.**]**

**SRS_HTTPSIOTHUBCONNECTION_11_039: [**If the function is called before receiveMessage() returns a message, the function shall throw an IllegalStateException.**]**