# IotHubSendTask Requirements

## Overview

Sends batched messages and invokes callbacks on completed requests. Meant to be used with an executor that continuously calls run().

## References

## Exposed API

```java
public final class IotHubSendTask implements Runnable
{
    public IotHubSendTask(IotHubTransport transport);

    public void run();
}
```


### IotHubSendTask

```java
public IotHubSendTask(IotHubTransport transport);
```

**SRS_IOTHUBSENDTASK_11_001: [**The constructor shall save the transport.**]**


### run

```java
public void run();
```

**SRS_IOTHUBSENDTASK_11_002: [**The function shall send all messages on the transport queue.**]**

**SRS_IOTHUBSENDTASK_11_003: [**The function shall invoke all callbacks on the transport's completed queue.**]**

**SRS_IOTHUBSENDTASK_11_005: [**The function shall not crash because of an IOException thrown by the transport.**]**

**SRS_IOTHUBSENDTASK_11_008: [**The function shall not crash because of any error or exception thrown by the transport.**]**