/*
 * Copyright (c) Microsoft. All rights reserved.
 * Licensed under the MIT license. See LICENSE file in the project root for full license information.
 */

package com.microsoft.azure.iot.service.transport.amqps;

import com.microsoft.azure.iot.service.sdk.FeedbackBatch;
import com.microsoft.azure.iot.service.sdk.Tools;
import org.apache.qpid.jms.JmsConnectionFactory;
import org.apache.qpid.jms.JmsQueue;

import javax.jms.*;
import javax.jms.IllegalStateException;

public class AmqpFeedbackReceiver
{
    private final String receiveFeedbackPath = "/messages/servicebound/feedback";
    private String hostUri;
    private String userName;
    private String sasToken;
    private Connection connection;
    private Session session;

    public AmqpFeedbackReceiver(String hostName, String userName, String sasToken)
    {
        this.hostUri = "amqps://" + hostName;
        this.userName = userName;
        this.sasToken = sasToken;
    }

    public void open() throws JMSException
    {
        ConnectionFactory factory = new JmsConnectionFactory(this.userName, this.sasToken, this.hostUri);

        this.connection = factory.createConnection(userName, sasToken);
        this.connection.setExceptionListener(new AmqpExceptionListener());
        this.connection.start();

        this.session = connection.createSession(false, Session.AUTO_ACKNOWLEDGE);
    }

    public void close() throws JMSException
    {
        if (this.connection != null)
        {
            this.connection.close();
            this.connection = null;
            this.session = null;
        }
    }

    public FeedbackBatch receive(String deviceId, long timeoutMs) throws JMSException
    {
        if (session == null)
        {
            throw new IllegalStateException("Session is not initialized. Call open() before call receive.");
        }

        if (Tools.isNullOrEmpty(deviceId))
        {
            throw new IllegalArgumentException("DeviceId is empty");
        }

        FeedbackBatch returnBatch = null;
        BytesMessage bytesMessage = null;
        Destination path = new JmsQueue(receiveFeedbackPath);
        MessageConsumer messageConsumer = session.createConsumer(path);
        bytesMessage = (BytesMessage) messageConsumer.receive(timeoutMs);

        return AmqpMessage.parseFeedbackBatch(bytesMessage);
    }
}
