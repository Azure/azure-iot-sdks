/*
 * Copyright (c) Microsoft. All rights reserved.
 * Licensed under the MIT license. See LICENSE file in the project root for full license information.
 */

package com.microsoft.azure.iot.service.sdk;

import org.apache.qpid.jms.JmsQueue;

import javax.jms.BytesMessage;
import javax.jms.JMSException;
import javax.jms.Session;

/**
 *
 * @author zolvarga
 */
public class MessageConverter
{
    public static Message convertJmsMessageToIotHubMessage(BytesMessage jmsMessage) throws JMSException 
    {
        Message iotHubMessage = null;
        if (jmsMessage != null)
        {
             iotHubMessage = new Message();
             iotHubMessage.setMessageId(jmsMessage.getJMSMessageID());
             iotHubMessage.setCorrelationId(jmsMessage.getJMSCorrelationID());
        }
        return iotHubMessage;
    }
    
    public static BytesMessage convertIotHubMessageToJmsMessage(Session session, String deviceId, Message iotHubMessage) throws JMSException
    {
        BytesMessage jmsMessage = null;
        if (iotHubMessage != null)
        {
            iotHubMessage.setTo(deviceId);
            jmsMessage = session.createBytesMessage();
            jmsMessage.setJMSDestination(new JmsQueue(iotHubMessage.getTo()));
            jmsMessage.writeBytes(iotHubMessage.getBytes());
        }
        return jmsMessage;
    }
}
