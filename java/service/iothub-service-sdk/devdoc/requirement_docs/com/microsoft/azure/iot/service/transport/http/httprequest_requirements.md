# HttpRequest Requirements

## Overview

An HTTP request.

## References

## Exposed API

```java
public class HttpRequest
{
    public HttpRequest(URL url, HttpMethod method, byte[] body, String headerFields) throws IOException;
    public HttpResponse send() throws IOException;
    public HttpRequest setHeaderField(String field, String value);
    public HttpRequest setReadTimeoutMillis(int timeout);
}
```

### HttpRequest

```java
public HttpRequest(URL url, HttpMethod method, byte[] body, String headerFields) throws IOException;
```
**SRS_SERVICE_SDK_JAVA_HTTPREQUEST_12_001: [** The function shall open a connection with the given URL as the endpoint. **]**

**SRS_SERVICE_SDK_JAVA_HTTPREQUEST_12_002: [** The function shall write the body to the connection. **]**

**SRS_SERVICE_SDK_JAVA_HTTPREQUEST_12_003: [** The function shall use the given HTTP method (i.e. GET) as the request method. **]**

**SRS_SERVICE_SDK_JAVA_HTTPREQUEST_12_004: [** If an IOException occurs in setting up the HTTP connection, the function shall throw an IOException. **]**

### send

```java
public HttpResponse send() throws IOException;
```
**SRS_SERVICE_SDK_JAVA_HTTPREQUEST_12_005: [** The function shall send an HTTP request as formatted in the constructor. **]**

**SRS_SERVICE_SDK_JAVA_HTTPREQUEST_12_006: [** The function shall return the HTTP response received, including the status code, body, header fields, and error reason (if any). **]**

**SRS_SERVICE_SDK_JAVA_HTTPREQUEST_12_007: [** If the client cannot connect to the server, the function shall throw an IOException. **]**

**SRS_SERVICE_SDK_JAVA_HTTPREQUEST_12_008: [** If an I/O exception occurs because of a bad response status code, the function shall attempt to flush or read the error stream so that the underlying HTTP connection can be reused. **]**

### setHeaderField

```java
public HttpRequest setHeaderField(String field, String value);
```
**SRS_SERVICE_SDK_JAVA_HTTPREQUEST_12_009: [** The function shall set the header field with the given name to the given value. **]**

### setReadTimeoutMillis

```java
public HttpRequest setReadTimeoutMillis(int timeout);
```
**SRS_SERVICE_SDK_JAVA_HTTPREQUEST_12_010: [** The function shall set the read timeout for the request to the given value. **]**
