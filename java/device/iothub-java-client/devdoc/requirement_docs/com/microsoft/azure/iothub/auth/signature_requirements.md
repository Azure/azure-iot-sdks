# Signature Requirements

## Overview
A signature that is used in the SAS token to authenticate the client.

## References

## Exposed API


```java
public final class Signature
{
    public Signature(String resourceUri, long expiryTime, String deviceKey);
    public String toString();
}
```


### Signature

```java
public Signature(String resourceUri, long expiryTime, String deviceKey);
```

**SRS_SIGNATURE_11_001: [**The signature shall be computed from a composition of functions as such: `encodeSignatureWebSafe(encodeSignatureUtf8(encodeSignatureBase64(encryptSignatureHmacSha256(buildRawSignature(resourceUri, expiryTime)))))`.**]**
 
**SRS_SIGNATURE_11_002: [**The device key shall be decoded using Base64 before the signature computation begins, excluding buildRawSignature().**]**

**SRS_SIGNATURE_11_003: [**The signature string shall be encoded using charset UTF-8.**]**


### toString

```java
public String toString();
```

**SRS_SIGNATURE_11_005: [**The function shall return the string representation of the signature.**]**