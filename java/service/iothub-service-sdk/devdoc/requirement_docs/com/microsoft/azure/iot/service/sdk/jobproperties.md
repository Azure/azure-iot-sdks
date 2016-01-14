# JobProperties Requirements

## Overview

The JobProperties class stores job properties and it is used by the bulk import/export operations.

## References

## Exposed API

```java
public class JobProperties
```

**SRS_SERVICE_SDK_JAVA_JOB_PROPERTIES_15_001: [** The JobProperties class has the following properties: JobId,
StartTimeUtc, EndTimeUtc, JobType, JobStatus, Progress, InputBlobContainerUri, OutputBlobContainerUri,
ExcludeKeysInExport, FailureReason **]**