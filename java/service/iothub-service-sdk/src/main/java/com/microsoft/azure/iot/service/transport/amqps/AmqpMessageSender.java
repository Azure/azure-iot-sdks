/*
 * Copyright (c) Microsoft. All rights reserved.
 * Licensed under the MIT license. See LICENSE file in the project root for full license information.
 */

package com.microsoft.azure.iot.service.transport.amqps;

import com.microsoft.azure.iot.service.sdk.Message;
import com.microsoft.azure.iot.service.sdk.MessageConverter;
import com.microsoft.azure.iot.service.sdk.Tools;
import org.apache.qpid.jms.JmsConnectionFactory;
import org.apache.qpid.jms.JmsQueue;

import javax.jms.*;
import javax.jms.IllegalStateException;

public class AmqpMessageSender
{
    private final String sendMessagePath = "/messages/devicebound";
    private String hostUri;
    private String userName;
    private String sasToken;
    private Connection connection;
    private Session session;

    public AmqpMessageSender(String hostName, String userName, String sasToken)
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

    public void send(String deviceId, Message message) throws JMSException
    {
        if (session == null)
        {
            throw new IllegalStateException("Session is not initialized. Call open() before call send.");
        }

        if (Tools.isNullOrEmpty(deviceId) || message == null)
        {
            throw new IllegalArgumentException("DeviceId is empty or message is null");
        }

        Destination path = new JmsQueue(sendMessagePath);
        MessageProducer messageProducer = session.createProducer(path);

        BytesMessage jmsMsg = MessageConverter.convertIotHubMessageToJmsMessage(session, deviceId, message);
        messageProducer.send(jmsMsg, DeliveryMode.NON_PERSISTENT, BytesMessage.DEFAULT_PRIORITY, BytesMessage.DEFAULT_TIME_TO_LIVE);
    }
}
