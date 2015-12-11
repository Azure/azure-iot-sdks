# AmqpsMessage Requirements

## Overview

Extension of the QPID-Proton-J MessageImpl class which implements the Message interface. Adds a Delivery object as a private member variable and adds a new ACK_TYPE enum. Adds the ability to easily acknowledge a single message.

## References

## Exposed API

```java
public class AmqpsMessage extends MessageImpl
{
	public enum ACK_TYPE
	{
		COMPLETE,
		ABANDON,
		REJECT
	}

	public void acknowledge(ACK_TYPE ackType);
	public Delivery getDelivery();
	public void setDelivery(Delivery _delivery);
}
```


### acknowledge

```java
public void acknowledge(ACK_TYPE ackType)
```

**SRS_AMQPSMESSAGE_14_001: [**If the ACK_TYPE is COMPLETE, the function shall set an Accepted disposition on the private Delivery object.**]**

**SRS_AMQPSMESSAGE_14_002: [**If the ACK_TYPE is ABANDON, the function shall set a Released disposition on the private Delivery object.**]**

**SRS_AMQPSMESSAGE_14_003: [**If the ACK_TYPE is REJECT, the function shall set a Rejected disposition on the private Delivery object.**]**

**SRS_AMQPSMESSAGE_14_005: [**The function shall settle the delivery after setting the proper disposition.**]**