# HttpsResponse Requirements

## Overview

An HTTPS response. Contains the status code, body, header fields, and error reason (if any).

## References

## Exposed API

```java
public class HttpsResponse
{
    public static int NOT_RECEIVED = -1;

    public HttpsResponse(int status, byte[] body, Map<String, List<String>> headerFields, byte[] errorReason);

    public int getStatus();
    public byte[] getBody();
    public String getHeaderField(String field);
    public Map<String, String> getHeaderFields();
    public byte[] getErrorReason();
}
```


### HttpsResponse

```java
public HttpsResponse(int status, String body, Map<String, List<String>> headerFields, String errorReason);
```

**SRS_HTTPSRESPONSE_11_001: [**The constructor shall store the input arguments so that the getters can return them later.**]**


### getStatus

```java
public int getStatus();
```

**SRS_HTTPSRESPONSE_11_002: [**The function shall return the status code given in the constructor.**]**


### getBody

```java
public byte[] getBody();
```

**SRS_HTTPSRESPONSE_11_003: [**The function shall return a copy of the body given in the constructor.**]**


### getHeaderField

```java
public String getHeaderField(String field);
```

**SRS_HTTPSRESPONSE_11_004: [**The function shall return a comma-separated list of the values associated with the header field name.**]**

**SRS_HTTPSRESPONSE_11_008: [**The function shall match the header field name in a case-insensitive manner.**]**

**SRS_HTTPSRESPONSE_11_006: [**If a value could not be found for the given header field name, the function shall throw an IllegalArgumentException.**]**


### getErrorReason

```java
public byte[] getErrorReason();
```

**SRS_HTTPSRESPONSE_11_007: [**The function shall return the error reason given in the constructor.**]**
