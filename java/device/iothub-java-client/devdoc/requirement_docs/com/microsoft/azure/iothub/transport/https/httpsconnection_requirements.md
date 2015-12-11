# HttpsConnection Requirements

## Overview

A wrapper for the Java SE class HttpsURLConnection. Used to avoid compatibility issues when testing with the mocking framework JMockit, as well as to avoid some undocumented side effects when using HttpsURLConnection.
The underlying HttpsURLConnection is transparently managed by Java. To reuse connections, for each time connect() is called, the input streams (input stream or error stream, if input stream is not accessible) must be completely read. Otherwise, the data remains in the stream and the connection will not be reusable.

## References

## Exposed API

```java
public class HttpsConnection
{
    public HttpsConnection(URL url, HttpsMethod method) throws IOException;

    public void connect() throws IOException;

    public void setRequestMethod(HttpsMethod method);
    public void setRequestHeader(String field, String value);
    public void setReadTimeoutMillis(int timeout);

    public void writeOutput(byte[] body);
    public byte[] readInput() throws IOException;
    public byte[] readError() throws IOException;

    public int getResponseStatus() throws IOException;
    public Map<String, List<String>> getResponseHeaders() throws IOException;
}
```


### HttpsConnection

```java
public HttpsConnection(URL url, HttpsMethod method) throws IOException;
```

**SRS_HTTPSCONNECTION_11_001: [**The constructor shall open a connection to the given URL.**]**

**SRS_HTTPSCONNECTION_11_002: [**The constructor shall throw an IOException if the connection was unable to be opened.**]**

**SRS_HTTPSCONNECTION_11_021: [**The constructor shall set the HTTPS method to the given method.**]**

**SRS_HTTPSCONNECTION_11_022: [If the URI given does not use the HTTPS protocol, the constructor shall throw an IllegalArgumentException.**]**


### connect

```java
public void connect() throws IOException;
```

**SRS_HTTPSCONNECTION_11_003: [**The function shall send a request to the URL given in the constructor.**]**

**SRS_HTTPSCONNECTION_11_004: [**The function shall stream the request body, if present, through the connection.**]**

**SRS_HTTPSCONNECTION_11_005: [**The function shall throw an IOException if the connection could not be established, or the server responded with a bad status code.**]**


### setRequestMethod

```java
public void setRequestMethod(HttpsMethod method);
```

**SRS_HTTPSCONNECTION_11_006: [**The function shall set the request method.**]**

**SRS_HTTPSCONNECTION_11_007: [**The function shall throw an IllegalArgumentException if the request currently has a non-empty body and the new method is not a POST or a PUT.**]**


### setRequestHeader

```java
public void setRequestHeader(String field, String value);
```

**SRS_HTTPSCONNECTION_11_008: [**The function shall set the given request header field.**]**


### setReadTimeoutMillis

```java
public void setReadTimeoutMillis(int timeout);
```

**SRS_HTTPSCONNECTION_11_023: [**The function shall set the read timeout to the given value.**]**


### writeOutput

```java
public void writeOutput(byte[] body);
```

**SRS_HTTPSCONNECTION_11_009: [**The function shall save the body to be sent with the request.**]**

**SRS_HTTPSCONNECTION_11_010: [**The function shall throw an IllegalArgumentException if the request does not currently use method POST or PUT and the body is non-empty.**]**


### readInput

```java
public byte[] readInput() throws IOException;
```

**SRS_HTTPSCONNECTION_11_011: [**The function shall read from the input stream (response stream) and return the response.**]**

**SRS_HTTPSCONNECTION_11_012: [**The function shall throw an IOException if the input stream could not be accessed.**]**

**SRS_HTTPSCONNECTION_11_019: [**The function shall close the input stream after it has been completely read.**]**


### readError

```java
public byte[] readError() throws IOException;
```

**SRS_HTTPSCONNECTION_11_013: [**The function shall read from the error stream and return the response.**]**

**SRS_HTTPSCONNECTION_11_014: [**The function shall throw an IOException if the error stream could not be accessed.**]**

**SRS_HTTPSCONNECTION_11_020: [**The function shall close the error stream after it has been completely read.**]**


### getResponseStatus

```java
public int getResponseStatus() throws IOException;
```

**SRS_HTTPSCONNECTION_11_015: [**The function shall return the response status code.**]**

**SRS_HTTPSCONNECTION_11_016: [**The function shall throw an IOException if no response was received.**]**


### getResponseHeaders

```java
public Map<String, List<String>> getResponseHeaders() throws IOException;
```

**SRS_HTTPSCONNECTION_11_017: [**The function shall return a mapping of header field names to the values associated with the header field name.**]**

**SRS_HTTPSCONNECTION_11_018: [**The function shall throw an IOException if no response was received.**]**