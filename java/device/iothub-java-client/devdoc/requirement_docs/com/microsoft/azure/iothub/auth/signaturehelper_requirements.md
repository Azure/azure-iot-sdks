# SignatureHelper Requirements

## Overview

Builds the authorization signature as a composition of functions.

## References

## Exposed API

```java
public final class SignatureHelper
{
    public static final String RAW_SIGNATURE_FORMAT = "%s\n%s";
    public static final Charset SIGNATURE_CHARSET = StandardCharsets.UTF_8;

    public static byte[] buildRawSignature(String resourceUri, long expiryTime);
    public static byte[] decodeDeviceKeyBase64(String deviceKey);
    public static byte[] encryptSignatureHmacSha256(byte[] sig, byte[] deviceKey);
    public static byte[] encodeSignatureBase64(byte[] sig);
    public static String encodeSignatureUtf8(byte[] sig);
    public static String encodeSignatureWebSafe(byte[] sig);
}
```


### buildRawSignature

```java
public static byte[] buildRawSignature(String resourceUri, long expiryTime);
```

**SRS_SIGNATUREHELPER_11_001: [**The function shall initialize the message being encoded as `"<scope>\n<expiryTime>"`.**]**

**SRS_SIGNATUREHELPER_11_002: [**The function shall decode the message using the charset UTF-8.**]**


### decodeDeviceKeyBase64

```java
public static byte[] decodeDeviceKeyBase64(String deviceKey);
```

**SRS_SIGNATUREHELPER_11_003: [**The function shall decode the device key using Base64.**]**


### encryptSignatureHmacSha256

```java
public static byte[] encryptSignatureHmacSha256(byte[] sig, byte[] deviceKey);
```

**SRS_SIGNATUREHELPER_11_004: [**The function shall encrypt the signature using the HMAC-SHA256 algorithm.**]**
 
**SRS_SIGNATUREHELPER_11_005: [**The function shall use the device key as the secret for the algorithm.**]**


### encodeSignatureBase64

```java
public static byte[] encodeSignatureBase64(byte[] sig);
```

**SRS_SIGNATUREHELPER_11_006: [**The function shall encode the signature using Base64.**]**


### encodeSignatureUtf8

```java
public static String encodeSignatureUtf8(byte[] sig);
```

**SRS_SIGNATUREHELPER_11_010: [**The function shall encode the signature using charset UTF-8.**]**


### encodeSignatureWebSafe

```java
public static String encodeSignatureWebSafe(String sig);
```

**SRS_SIGNATUREHELPER_11_007: [**The function shall replace web-unsafe characters in the signature with a '%' followed by two hexadecimal digits, where the hexadecimal digits are determined by the UTF-8 charset.**]**

**SRS_SIGNATUREHELPER_11_008: [**The function shall replace spaces with '+' signs.**]**