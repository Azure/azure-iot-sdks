# SymmetricKey Requirements

## Overview

Store primary and secondary keys and provide function for key length validation. 

## References

## Exposed API

```java
public class SymmetricKey
{
   public void setPrimaryKey(String primaryKey);
   public void setSecondaryKey(String secondayKey);
}
```
### setPrimaryKey

```java
public void setPrimaryKey(String primaryKey);
```
**SRS_SERVICE_SDK_JAVA_SYMMETRICKEY_12_001: [** The function shall throw IllegalArgumentException if the length of the key less than 16 or greater than 64 **]**

**SRS_SERVICE_SDK_JAVA_SYMMETRICKEY_12_002: [** The function shall set the private primaryKey member to the given value if the length validation passed **]**

### setSecondaryKey

```java
public void setSecondaryKey(String secondayKey);
```
**SRS_SERVICE_SDK_JAVA_SYMMETRICKEY_12_003: [** The function shall throw IllegalArgumentException if the length of the key less than 16 or greater than 64 **]**

**SRS_SERVICE_SDK_JAVA_SYMMETRICKEY_12_004: [** The function shall set the private secondaryKey  member to the given value if the length validation passed **]**

