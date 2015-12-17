# Tools Requirements

## Overview

Collection of static helper functions. 

## References

## Exposed API

```java
public class Tools
{
   public static Boolean isNullOrEmpty(String value);
   public static Boolean isNullOrWhiteSpace(String value);
   public static String getValueStringByKey(String value);
   public static String getValueFromJsonObject(JsonObject jsonObject,String key);
   public static String getValueFromJsonString(JsonString jsonString);
   public static long getNumberValueFromJsonObject(JsonObject jsonObject, String key);
   public static void appendJsonAttribute(StringBuilder strBuilder, String name, String value, Boolean isQuoted, Boolean isLast);
}
```

### isNullOrEmpty

```java
public static Boolean isNullOrEmpty(String value);
```
**SRS_SERVICE_SDK_JAVA_TOOLS_12_001: [** The function shall return true if the input is null **]**

**SRS_SERVICE_SDK_JAVA_TOOLS_12_002: [** The function shall return true if the input string length is zero **]**

**SRS_SERVICE_SDK_JAVA_TOOLS_12_003: [** The function shall return false if the input string length is greater than zero **]**

### isNullOrWhiteSpace

```java
public static Boolean isNullOrWhiteSpace(String value);
```
**SRS_SERVICE_SDK_JAVA_TOOLS_12_004: [** The function shall return true if the input is null **]**

**SRS_SERVICE_SDK_JAVA_TOOLS_12_005: [** The function shall call the isNullOrEmpty function and return with itï¿½s return value **]**

### getValueStringByKey

```java
public static String getValueStringByKey(String value);
```
**SRS_SERVICE_SDK_JAVA_TOOLS_12_006: [** The function shall return empty string if any of the input is null **]**

**SRS_SERVICE_SDK_JAVA_TOOLS_12_007: [** The function shall get the value of the given key from the map **]**

**SRS_SERVICE_SDK_JAVA_TOOLS_12_008: [** The function shall return with trimmed string if the value of the key is not null **]**

**SRS_SERVICE_SDK_JAVA_TOOLS_12_009: [** The function shall return with empty string if the value of the key is null **]**

### getValueFromJsonObject

```java
public static String getValueFromJsonObject(JsonObject jsonObject,String key);
```
**SRS_SERVICE_SDK_JAVA_TOOLS_12_010: [** The function shall return empty string if any of the input is null **]**

**SRS_SERVICE_SDK_JAVA_TOOLS_12_011: [** The function shall get the JsonValue of the key **]**

**SRS_SERVICE_SDK_JAVA_TOOLS_12_012: [** The function shall get the JsonString from the JsonValue if the JsonValue is not null **]**

**SRS_SERVICE_SDK_JAVA_TOOLS_12_013: [** The function shall return the string value from the JsonString calling the getValueFromJsonString function **]**

**SRS_SERVICE_SDK_JAVA_TOOLS_12_014: [** The function shall return empty string if the JsonValue is null **]**

### getValueFromJsonString

```java
public static String getValueFromJsonString(JsonString jsonString);
```
**SRS_SERVICE_SDK_JAVA_TOOLS_12_015: [** The function shall return empty string if the input is null **]**

**SRS_SERVICE_SDK_JAVA_TOOLS_12_016: [** The function shall get the string value from JsonString **]**

**SRS_SERVICE_SDK_JAVA_TOOLS_12_017: [** The function shall trim the leading and trailing parenthesis from the string and return with it **]**

### getNumberValueFromJsonObject

```java
public static long getNumberValueFromJsonObject(JsonObject jsonObject, String key);
```
**SRS_SERVICE_SDK_JAVA_TOOLS_12_018: [** The function shall return zero if any of the input is null **]**

**SRS_SERVICE_SDK_JAVA_TOOLS_12_019: [** The function shall get the JsonValue of the key and return zero if it is null **]**

**SRS_SERVICE_SDK_JAVA_TOOLS_12_020: [** The function shall get the JsonNumber from the JsonValue and return zero if it is null **]**

**SRS_SERVICE_SDK_JAVA_TOOLS_12_021: [** The function shall return the long value from the JsonNumber if the JsonNumber is not null **]**

### appendJsonAttribute

```java
public static void appendJsonAttribute(StringBuilder strBuilder, String name, String value, Boolean isQuoted, Boolean isLast);
```

**SRS_SERVICE_SDK_JAVA_TOOLS_12_022: [** The function shall do nothing if the input StringBuilder is null **]**

**SRS_SERVICE_SDK_JAVA_TOOLS_12_023: [** The function shall append the input StringBuilder string using the following format: “name”:”value” if isQuoted is false **]**

**SRS_SERVICE_SDK_JAVA_TOOLS_12_024: [** The function shall append the input StringBuilder string using the following format: “name”:””value”” if isQuoted is true **]**

**SRS_SERVICE_SDK_JAVA_TOOLS_12_024: [** The function shall append the input StringBuilder string with trailing “,” character isLast is false **]**




