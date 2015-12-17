# RegistryStatistics Requirements

## Overview

Provide static function to parse Json string to RegistryStatistics object. 

## References

## Exposed API

```java
public class RegistryStatistics
{
    public static RegistryStatistics parse(String jsonString);
}
```

### RegistryStatistics

```java
public static RegistryStatistics parse(String jsonString);
```
**SRS_SERVICE_SDK_JAVA_REGISTRYSTATISTICS_12_001: [** The function shall return an empty RegistryStatistics object if the input is empty or null **]**

**SRS_SERVICE_SDK_JAVA_REGISTRYSTATISTICS_12_002: [** The function shall throw an Exception if the parsing failed **]**

**SRS_SERVICE_SDK_JAVA_REGISTRYSTATISTICS_12_003: [** The function shall parse all the Json records to the RegistryStatistics object and return with it **]**
