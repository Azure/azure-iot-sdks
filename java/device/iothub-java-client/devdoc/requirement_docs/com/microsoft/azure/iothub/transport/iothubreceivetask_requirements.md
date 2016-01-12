# IotHubReceiveTask Requirements

## Overview

Polls an IoT Hub for messages and invokes a callback if one is found. Meant to be used with an executor that continuously calls run().

## References

## Exposed API

```java
public final class IotHubReceiveTask implements Runnable
{
    public IotHubReceiveTask(IotHubTransport transport);

    public void run();
}
```


### IotHubReceiveTask

```java
public IotHubReceiveTask(IotHubTransport transport);
```

**SRS_IOTHUBRECEIVETASK_11_001: [**The constructor shall save the transport.**]**


### run

```java
public void run();
```

**SRS_IOTHUBRECEIVETASK_11_002: [**The function shall poll an IoT Hub for messages, invoke the message callback if one exists, and return one of COMPLETE, ABANDON, or REJECT to the IoT Hub.**]**

**SRS_IOTHUBRECEIVETASK_11_004: [**The function shall not crash because of an IOException thrown by the transport.**]**

**SRS_IOTHUBRECEIVETASK_11_005: [**The function shall not crash because of any error or exception thrown by the transport.**]**

