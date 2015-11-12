/*
 * Copyright (c) Microsoft. All rights reserved.
 * Licensed under the MIT license. See LICENSE file in the project root for full license information.
 */

package com.microsoft.azure.iothub.transport.amqps;

import org.apache.qpid.proton.amqp.messaging.Accepted;
import org.apache.qpid.proton.amqp.messaging.Rejected;
import org.apache.qpid.proton.amqp.messaging.Released;
import org.apache.qpid.proton.engine.Delivery;
import org.apache.qpid.proton.message.impl.MessageImpl;

/**
 * Extension of the QPID-Proton-J MessageImpl class which implements the Message interface. Adds a Delivery object as a
 * private member variable and adds a new ACK_TYPE enum. Adds the ability to easily acknowledge a single message.
 */
public class AmqpsMessage extends MessageImpl{
    private Delivery _delivery;

    public enum ACK_TYPE{
        COMPLETE,
        ABANDON,
        REJECT
    }

    /**
     * Sends acknowledgement of this message using the provided ACK_TYPE.
     * @param ackType acknowledgement type to send
     */
    public void acknowledge(ACK_TYPE ackType){
        switch(ackType)
        {
            // Codes_SRS_AMQPSMESSAGE_14_001: [If the ACK_TYPE is COMPLETE, the function shall set an Accepted disposition on the private Delivery object.]
            case COMPLETE:
                _delivery.disposition(Accepted.getInstance());
                break;
            // Codes_SRS_AMQPSMESSAGE_14_002: [If the ACK_TYPE is ABANDON, the function shall set a Released disposition on the private Delivery object.]
            case ABANDON:
                _delivery.disposition(Released.getInstance());
                break;
            // Codes_SRS_AMQPSMESSAGE_14_003: [If the ACK_TYPE is REJECT, the function shall set a Rejected disposition on the private Delivery object.]
            case REJECT:
                _delivery.disposition(new Rejected());
                break;
            default:
                //This should never happen
                throw new IllegalStateException("Invalid ack type given. Type "+ackType+" does not exist.");
        }
        // Codes_SRS_AMQPSMESSAGE_14_005: [The function shall settle the delivery after setting the proper disposition.]
        _delivery.settle();
    }

    /**
     * Gets this AmqpsMessage Delivery object
     * @return the Delivery object
     */
    public Delivery getDelivery() {
        return _delivery;
    }

    /**
     * Sets this AmqpsMessage Delivery Object
     * @param _delivery the new Delivery
     */
    public void setDelivery(Delivery _delivery) {
        this._delivery = _delivery;
    }
}
