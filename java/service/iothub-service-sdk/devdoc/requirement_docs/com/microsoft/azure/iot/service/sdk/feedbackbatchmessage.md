# FeedbackBatchMessage Requirements

## Overview

Provide static function to parse Json string to FeedbackBatch object. 

## References

## Exposed API

```java
public class FeedbackBatchMessage
{
    public static FeedbackBatch parse(String jsonString);
}
```

### FeedbackBatch

```java
public static FeedbackBatch parse(String jsonString);
```
**SRS_SERVICE_SDK_JAVA_FEEDBACKBATCHMESSAGE_12_001: [** The function shall return an empty FeedbackBatch object if the input is empty or null **]**

**SRS_SERVICE_SDK_JAVA_FEEDBACKBATCHMESSAGE_12_002: [** The function shall return an empty FeedbackBatch object if the content of the Data input is empty **]**

**SRS_SERVICE_SDK_JAVA_FEEDBACKBATCHMESSAGE_12_003: [** The function shall remove data batch brackets if they exist **]**

**SRS_SERVICE_SDK_JAVA_FEEDBACKBATCHMESSAGE_12_004: [** The function shall throw a JsonParsingException if the parsing failed **]**

**SRS_SERVICE_SDK_JAVA_FEEDBACKBATCHMESSAGE_12_005: [** The function shall parse all the Json records to the FeedbackBatch **]**

**SRS_SERVICE_SDK_JAVA_FEEDBACKBATCHMESSAGE_12_006: [** The function shall copy the last record UTC time for batch UTC time **]**
