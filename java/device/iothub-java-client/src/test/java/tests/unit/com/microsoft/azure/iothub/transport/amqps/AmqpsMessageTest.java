/*
 * Copyright (c) Microsoft. All rights reserved.
 * Licensed under the MIT license. See LICENSE file in the project root for full license information.
 */
package tests.unit.com.microsoft.azure.iothub.transport.amqps;

import com.microsoft.azure.iothub.transport.amqps.AmqpsMessage;
import mockit.MockUp;
import mockit.Mocked;
import mockit.NonStrictExpectations;
import mockit.Verifications;
import org.apache.qpid.proton.amqp.messaging.Accepted;
import org.apache.qpid.proton.amqp.messaging.Rejected;
import org.apache.qpid.proton.amqp.messaging.Released;
import org.apache.qpid.proton.engine.Delivery;
import org.junit.Test;

public class AmqpsMessageTest {

    @Mocked
    protected Delivery mockDelivery;

    // Tests_SRS_AMQPSMESSAGE_14_001: [If the ACK_TYPE is COMPLETE, the function shall set an Accepted disposition on the private Delivery object.]
    @Test
    public void acknowledgeSetsAcceptedDispositionForComlete(){
        AmqpsMessage message = new AmqpsMessage();
        message.setDelivery(mockDelivery);
        message.acknowledge(AmqpsMessage.ACK_TYPE.COMPLETE);

        final Delivery expectedDelivery = mockDelivery;

        new Verifications()
        {
            {
                expectedDelivery.disposition(Accepted.getInstance());
            }
        };
    }

    // Tests_SRS_AMQPSMESSAGE_14_002: [If the ACK_TYPE is ABANDON, the function shall set a Released disposition on the private Delivery object.]
    @Test
    public void acknowledgeSetsReleasedDispositionForAbandon(){
        AmqpsMessage message = new AmqpsMessage();
        message.setDelivery(mockDelivery);
        message.acknowledge(AmqpsMessage.ACK_TYPE.ABANDON);

        final Delivery expectedDelivery = mockDelivery;

        new Verifications()
        {
            {
                expectedDelivery.disposition(Released.getInstance());
            }
        };
    }

    // Tests_SRS_AMQPSMESSAGE_14_003: [If the ACK_TYPE is REJECT, the function shall set a Rejected disposition on the private Delivery object.]
    @Test
    public void acknowledgeSetsRejectedDispositionForReject(
            @Mocked final Rejected mockRejected
    ){
        new NonStrictExpectations()
        {
            {
                new Rejected();
                result = mockRejected;
            }
        };
        AmqpsMessage message = new AmqpsMessage();
        message.setDelivery(mockDelivery);
        message.acknowledge(AmqpsMessage.ACK_TYPE.REJECT);

        final Delivery expectedDelivery = mockDelivery;

        new Verifications()
        {
            {
                expectedDelivery.disposition(mockRejected);
            }
        };
    }

    // Tests_SRS_AMQPSMESSAGE_14_005: [The function shall settle the delivery after setting the proper disposition.]
    @Test
    public void acknowledgeSettlesDelivery()
    {
        AmqpsMessage message = new AmqpsMessage();
        message.setDelivery(mockDelivery);
        message.acknowledge(AmqpsMessage.ACK_TYPE.COMPLETE);
        message.acknowledge(AmqpsMessage.ACK_TYPE.ABANDON);
        message.acknowledge(AmqpsMessage.ACK_TYPE.REJECT);

        final Delivery expectedDelivery = mockDelivery;

        new Verifications()
        {
            {
                expectedDelivery.settle(); times = 3;
            }
        };
    }

}
