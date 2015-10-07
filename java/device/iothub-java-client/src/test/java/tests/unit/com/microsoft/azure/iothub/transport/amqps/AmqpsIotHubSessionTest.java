// Copyright (c) Microsoft. All rights reserved.
// Licensed under the MIT license. See LICENSE file in the project root for full license information.

package tests.unit.com.microsoft.azure.iothub.transport.amqps;

import static org.junit.Assert.assertThat;
import static org.hamcrest.CoreMatchers.is;

import com.microsoft.azure.iothub.DeviceClientConfig;
import com.microsoft.azure.iothub.Message;
import com.microsoft.azure.iothub.MessageProperty;
import com.microsoft.azure.iothub.IotHubMessageResult;
import com.microsoft.azure.iothub.IotHubStatusCode;
import com.microsoft.azure.iothub.auth.IotHubSasToken;
import com.microsoft.azure.iothub.net.IotHubMessageUri;
import com.microsoft.azure.iothub.transport.amqps.AmqpsIotHubSession;
import com.microsoft.azure.iothub.net.IotHubEventUri;
import com.microsoft.azure.iothub.net.IotHubUri;

import mockit.Mocked;
import mockit.NonStrictExpectations;
import mockit.Verifications;
import mockit.VerificationsInOrder;
import org.apache.qpid.jms.JmsConnection;
import org.apache.qpid.jms.JmsConnectionFactory;
import org.apache.qpid.jms.JmsMessageConsumer;
import org.apache.qpid.jms.JmsMessageProducer;
import org.apache.qpid.jms.JmsSession;
import org.apache.qpid.jms.JmsQueue;
import org.apache.qpid.jms.message.JmsBytesMessage;
import org.apache.qpid.jms.message.JmsMessage;
import org.apache.qpid.jms.provider.ProviderConstants;
import org.junit.Test;

import java.io.IOException;
import java.net.URLEncoder;
import java.nio.charset.Charset;
import java.nio.charset.StandardCharsets;
import java.util.Enumeration;

import javax.jms.BytesMessage;
import javax.jms.JMSException;

/** Unit tests for AmqpsIotHubSession. */
public class AmqpsIotHubSessionTest
{
    protected static final Charset UTF8 = StandardCharsets.UTF_8;

    @Mocked
    protected DeviceClientConfig mockConfig;
    @Mocked
    protected JmsConnectionFactory mockConnFactory;
    @Mocked
    protected JmsConnection mockConn;
    @Mocked
    protected JmsSession mockSess;
    @Mocked
    protected IotHubUri mockIotHubUri;
    @Mocked
    protected IotHubSasToken mockToken;

    // Tests_SRS_AMQPSIOTHUBSESSION_11_022: [The constructor shall save the configuration.]
    // Tests_SRS_AMQPSIOTHUBSESSION_11_001: [The function shall establish an AMQPS session with an IoT Hub with the resource URI 'amqps://[urlEncodedDeviceId]@sas.[urlEncodedIotHubName]:[urlEncodedSasToken]@[urlEncodedIotHubHostname].]
    @Test
    public void openEstablishesSessionUsingCorrectUri()
            throws JMSException, IOException
    {
        final String iotHubHostname = "test.iothub.testhub";
        final String iotHubName = "test.iothub";
        final String deviceId = "('test-deviceid')";
        final String deviceKey = "test-devicekey?&test";
        final String resourceUri = "test-resource-uri";
        new NonStrictExpectations()
        {
            {
                mockConfig.getIotHubHostname();
                result = iotHubHostname;
                mockConfig.getIotHubName();
                result = iotHubName;
                mockConfig.getDeviceId();
                result = deviceId;
                mockConfig.getDeviceKey();
                result = deviceKey;
                IotHubUri.getResourceUri(iotHubHostname, deviceId);
                result = resourceUri;
                new IotHubSasToken(resourceUri, deviceId, deviceKey, anyLong);
                result = mockToken;
            }
        };

        AmqpsIotHubSession session = new AmqpsIotHubSession(mockConfig);
        session.open();

        final String expectedUsername = deviceId + "@sas." + iotHubName;
        final String expectedPassword = mockToken.toString();
        final String expectedHostname = "amqps://" + iotHubHostname;
        new Verifications()
        {
            {
                new JmsConnectionFactory(
                        expectedUsername, expectedPassword, expectedHostname);
            }
        };
    }

    // Tests_SRS_AMQPSIOTHUBSESSION_11_001: [The function shall establish an AMQPS session with an IoT Hub with the resource URI 'amqps://[urlEncodedDeviceId]@sas.[urlEncodedIotHubName]:[urlEncodedSasToken]@[urlEncodedIotHubHostname].]
    @Test
    public void openEstablishesConnectionWithSasToken()
            throws JMSException, IOException
    {
        final String iotHubHostname = "test-iothubhostname";
        final String deviceId = "test-device-id";
        final String deviceKey = "test-device-key";
        final String resourceUri = "test-resource-uri";
        final String tokenStr = "test-token-str";
        new NonStrictExpectations()
        {
            {
                mockConfig.getIotHubHostname();
                result = iotHubHostname;
                mockConfig.getDeviceId();
                result = deviceId;
                mockConfig.getDeviceKey();
                result = deviceKey;
                IotHubUri.getResourceUri(iotHubHostname, deviceId);
                result = resourceUri;
                new IotHubSasToken(resourceUri, deviceId, deviceKey, anyLong);
                result = mockToken;
                mockToken.toString();
                result = tokenStr;
            }
        };


        AmqpsIotHubSession session = new AmqpsIotHubSession(mockConfig);
        session.open();

        final String expectedToken = tokenStr;
        new Verifications()
        {
            {
                new JmsConnectionFactory(
                        anyString, expectedToken, anyString);
            }
        };
    }

    // Tests_SRS_AMQPSIOTHUBSESSION_11_003: [If an AMQPS session is unable to be established, the function shall throw an IOException.]
    @Test(expected = IOException.class)
    public void openThrowsIOExceptionIfSessionFails()
            throws JMSException, IOException
    {
        new NonStrictExpectations()
        {
            {
                new JmsConnectionFactory(anyString, anyString, anyString);
                result = mockConnFactory;
                mockConnFactory.createConnection();
                result = mockConn;
                mockConn.start();
                result = new JMSException(anyString);
            }
        };

        AmqpsIotHubSession session = new AmqpsIotHubSession(mockConfig);
        session.open();
    }

    // Tests_SRS_AMQPSIOTHUBSESSION_11_020: [If the AMQPS session is already open, the function shall do nothing.]
    @Test
    public void openDoesNothingIfAlreadyOpened() throws IOException
    {
        AmqpsIotHubSession session = new AmqpsIotHubSession(mockConfig);
        session.open();
        session.open();

        new Verifications()
        {
            {
                new JmsConnectionFactory(anyString, anyString, anyString);
                times = 1;
            }
        };
    }

    // Tests_SRS_AMQPSIOTHUBSESSION_11_023: [The function shall close the AMQPS session.]
    @Test
    public void closeClosesAmqpsSession() throws IOException, JMSException
    {
        AmqpsIotHubSession session = new AmqpsIotHubSession(mockConfig);
        session.open();
        session.close();

        final JmsConnection expectedConnection = mockConn;
        new Verifications()
        {
            {
                expectedConnection.close();
            }
        };
    }

    // Tests_SRS_AMQPSIOTHUBSESSION_11_025: [If the AMQPS session is closed, the function shall do nothing.]
    @Test
    public void closeDoesNothingIfSessionNotYetOpened()
            throws IOException, JMSException
    {
        AmqpsIotHubSession session = new AmqpsIotHubSession(mockConfig);
        session.close();

        final JmsConnection expectedConnection = mockConn;
        new Verifications()
        {
            {
                expectedConnection.close();
                times = 0;
            }
        };
    }

    // Tests_SRS_AMQPSIOTHUBSESSION_11_025: [If the AMQPS session is closed, the function shall do nothing.]
    @Test
    public void closeDoesNothingIfSessionAlreadyClosed()
            throws IOException, JMSException
    {
        AmqpsIotHubSession session = new AmqpsIotHubSession(mockConfig);
        session.open();
        session.close();
        session.close();

        final JmsConnection expectedConnection = mockConn;
        new Verifications()
        {
            {
                expectedConnection.close();
                times = 1;
            }
        };
    }

    // Tests_SRS_AMQPSIOTHUBSESSION_11_004: [The function shall send an event message to the IoT Hub given in the configuration.]
    @Test
    public void sendEventSendsEventToCorrectIotHub(
            @Mocked final IotHubEventUri mockEventUri,
            @Mocked final Message mockMsg,
            @Mocked final JmsQueue mockDest,
            @Mocked final JmsMessageProducer mockProducer)
            throws JMSException, IOException
    {
        final String iotHubHostname = "test.iothub";
        final String deviceId = "test-deviceid";
        final String eventPath =
                "/devices/" + deviceId + "/messages/events";
        new NonStrictExpectations()
        {
            {
                mockConfig.getIotHubHostname();
                result = iotHubHostname;
                mockConfig.getDeviceId();
                result = deviceId;
                new IotHubEventUri(iotHubHostname, deviceId);
                result = mockEventUri;
                mockEventUri.getPath();
                result = eventPath;
                new JmsQueue(eventPath);
                result = mockDest;
                mockSess.createProducer(mockDest);
                result = mockProducer;
            }
        };

        AmqpsIotHubSession sess = new AmqpsIotHubSession(mockConfig);
        sess.open();
        sess.sendEvent(mockMsg);

        final JmsMessageProducer expectedProducer = mockProducer;
        new Verifications()
        {
            {
                expectedProducer.send((BytesMessage) any);
            }
        };
    }

    // Tests_SRS_AMQPSIOTHUBSESSION_11_005: [The function shall send the message body.]
    @Test
    public void sendEventSendsMessageBody(
            @Mocked final IotHubEventUri mockEventUri,
            @Mocked final Message mockMsg,
            @Mocked final JmsQueue mockDest,
            @Mocked final JmsMessageProducer mockProducer,
            @Mocked final JmsBytesMessage mockBytesMsg)
            throws JMSException, IOException
    {
        final byte[] msgBody = { 0x61, 0x62, 0x63 };
        new NonStrictExpectations()
        {
            {
                new JmsQueue(anyString);
                result = mockDest;
                mockSess.createProducer(mockDest);
                result = mockProducer;
                mockMsg.getBytes();
                result = msgBody;
                mockSess.createBytesMessage();
                result = mockBytesMsg;
            }
        };

        AmqpsIotHubSession sess = new AmqpsIotHubSession(mockConfig);
        sess.open();
        sess.sendEvent(mockMsg);

        final JmsBytesMessage expectedBytesMsg = mockBytesMsg;
        new VerificationsInOrder()
        {
            {
                expectedBytesMsg.writeBytes(msgBody);
                mockProducer.send(expectedBytesMsg);
            }
        };
    }

    // Tests_SRS_AMQPSIOTHUBSESSION_11_006: [The function shall set the property 'to' to '/devices/[deviceId]/messages/events'.]
    @Test
    public void sendEventSetsToProperty(
            @Mocked final IotHubEventUri mockEventUri,
            @Mocked final Message mockMsg,
            @Mocked final JmsQueue mockDest,
            @Mocked final JmsMessageProducer mockProducer,
            @Mocked final JmsBytesMessage mockBytesMsg)
            throws JMSException, IOException
    {
        final String iotHubHostname = "test.iothub";
        final String deviceId = "test-deviceid";
        final String eventPath =
                "/devices/" + deviceId + "/messages/events";
        new NonStrictExpectations()
        {
            {
                mockConfig.getIotHubHostname();
                result = iotHubHostname;
                mockConfig.getDeviceId();
                result = deviceId;
                new IotHubEventUri(iotHubHostname, deviceId);
                result = mockEventUri;
                mockEventUri.getPath();
                result = eventPath;
                new JmsQueue(anyString);
                result = mockDest;
                mockSess.createProducer(mockDest);
                result = mockProducer;
                mockSess.createBytesMessage();
                result = mockBytesMsg;
            }
        };

        AmqpsIotHubSession sess = new AmqpsIotHubSession(mockConfig);
        sess.open();
        sess.sendEvent(mockMsg);

        final JmsBytesMessage expectedBytesMsg = mockBytesMsg;
        final String expectedTo = eventPath;
        new VerificationsInOrder()
        {
            {
                expectedBytesMsg.setStringProperty("to", expectedTo);
                mockProducer.send(expectedBytesMsg);
            }
        };
    }

    // Tests_SRS_AMQPSIOTHUBSESSION_11_007: [The function shall send all message properties.]
    @Test
    public void sendEventSetsAllMessageProperties(
            @Mocked final IotHubEventUri mockEventUri,
            @Mocked final Message mockMsg,
            @Mocked final JmsQueue mockDest,
            @Mocked final JmsMessageProducer mockProducer,
            @Mocked final JmsBytesMessage mockBytesMsg,
            @Mocked final MessageProperty mockProperty)
            throws JMSException, IOException
    {
        final MessageProperty[] msgProperties = { mockProperty };
        final String propertyName = "test-property-name";
        final String propertyValue = "test-property-value";
        new NonStrictExpectations()
        {
            {
                new JmsQueue(anyString);
                result = mockDest;
                mockSess.createProducer(mockDest);
                result = mockProducer;
                mockMsg.getProperties();
                result = msgProperties;
                mockSess.createBytesMessage();
                result = mockBytesMsg;
                mockProperty.getName();
                result = propertyName;
                mockProperty.getValue();
                result = propertyValue;
            }
        };

        AmqpsIotHubSession sess = new AmqpsIotHubSession(mockConfig);
        sess.open();
        sess.sendEvent(mockMsg);

        final JmsBytesMessage expectedBytesMsg = mockBytesMsg;
        final String expectedPropertyName = propertyName;
        final String expectedPropertyValue = propertyValue;
        new VerificationsInOrder()
        {
            {
                mockBytesMsg.setStringProperty(expectedPropertyName,
                        expectedPropertyValue);
                mockProducer.send(expectedBytesMsg);
            }
        };
    }

    // Tests_SRS_AMQPSIOTHUBSESSION_11_008: [If the message was successfully received by the service, the function shall return status code OK_EMPTY.]
    @Test
    public void sendEventReturnsOkEmptyIfSuccessful(
            @Mocked final IotHubEventUri mockEventUri,
            @Mocked final Message mockMsg,
            @Mocked final JmsQueue mockDest,
            @Mocked final JmsMessageProducer mockProducer,
            @Mocked final JmsBytesMessage mockBytesMsg)
            throws JMSException, IOException
    {
        AmqpsIotHubSession sess = new AmqpsIotHubSession(mockConfig);
        sess.open();
        IotHubStatusCode testStatus = sess.sendEvent(mockMsg);

        IotHubStatusCode expectedStatus = IotHubStatusCode.OK_EMPTY;
        assertThat(testStatus, is(expectedStatus));
    }

    // Tests_SRS_AMQPSIOTHUBSESSION_11_009: [If the message was not successfully received by the service, the function shall return status code ERROR.]
    @Test
    public void sendEventReturnsErrorIfMessageNotReceived(
            @Mocked final IotHubEventUri mockEventUri,
            @Mocked final Message mockMsg,
            @Mocked final JmsQueue mockDest,
            @Mocked final JmsMessageProducer mockProducer,
            @Mocked final JmsBytesMessage mockBytesMsg)
            throws JMSException, IOException
    {
        new NonStrictExpectations()
        {
            {
                new JmsQueue(anyString);
                result = mockDest;
                mockSess.createProducer(mockDest);
                result = mockProducer;
                mockProducer.send((JmsBytesMessage) any);
                result = new JMSException(anyString);
            }
        };

        AmqpsIotHubSession sess = new AmqpsIotHubSession(mockConfig);
        sess.open();
        IotHubStatusCode testStatus = sess.sendEvent(mockMsg);

        IotHubStatusCode expectedStatus = IotHubStatusCode.ERROR;
        assertThat(testStatus, is(expectedStatus));
    }

    // Tests_SRS_AMQPSIOTHUBSESSION_11_010: [If a message producer could not be successfully registered, the function shall throw an IOException.]
    @Test(expected = IOException.class)
    public void sendEventThrowsIOExceptionIfProducerCouldNotBeRegistered(
            @Mocked final IotHubEventUri mockEventUri,
            @Mocked final Message mockMsg,
            @Mocked final JmsQueue mockDest,
            @Mocked final JmsMessageProducer mockProducer)
            throws JMSException, IOException

    {
        new NonStrictExpectations()
        {
            {
                new JmsQueue(anyString);
                result = mockDest;
                mockSess.createProducer(mockDest);
                result = new JMSException(anyString);
            }
        };

        AmqpsIotHubSession sess = new AmqpsIotHubSession(mockConfig);
        sess.open();
        sess.sendEvent(mockMsg);
    }

    // Tests_SRS_AMQPSIOTHUBSESSION_11_027: [If the AMQPS session is closed, the function shall throw an IllegalStateException.]
    @Test(expected = IllegalStateException.class)
    public void sendEventFailsIfSessionNotYetOpened(
            @Mocked final Message mockMsg) throws IOException
    {
        AmqpsIotHubSession sess = new AmqpsIotHubSession(mockConfig);
        sess.sendEvent(mockMsg);
    }

    // Tests_SRS_AMQPSIOTHUBSESSION_11_027: [If the AMQPS session is closed, the function shall throw an IllegalStateException.]
    @Test(expected = IllegalStateException.class)
    public void sendEventFailsIfSessionAlreadyClosed(
            @Mocked final Message mockMsg) throws IOException
    {
        AmqpsIotHubSession sess = new AmqpsIotHubSession(mockConfig);
        sess.open();
        sess.close();
        sess.sendEvent(mockMsg);
    }

    // Tests_SRS_AMQPSIOTHUBSESSION_11_011: [The function shall attempt to consume a message from the IoT Hub given in the configuration.]
    @Test
    public void receiveMessageSendsToCorrectIotHub(
            @Mocked final IotHubMessageUri mockMessageUri,
            @Mocked final Message mockMsg,
            @Mocked final JmsBytesMessage mockAmqpsMsg,
            @Mocked final JmsQueue mockDest,
            @Mocked final JmsMessageConsumer mockConsumer)
            throws JMSException, IOException
    {
        final String iotHubHostname = "test.iothub";
        final String deviceId = "test-deviceid";
        final String messagePath =
                "/devices/" + deviceId + "/messages/devicebound";
        new NonStrictExpectations()
        {
            {
                mockConfig.getIotHubHostname();
                result = iotHubHostname;
                mockConfig.getDeviceId();
                result = deviceId;
                new IotHubMessageUri(iotHubHostname, deviceId);
                result = mockMessageUri;
                mockMessageUri.getPath();
                result = messagePath;
                new JmsQueue(messagePath);
                result = mockDest;
                mockSess.createConsumer(mockDest);
                result = mockConsumer;
                mockConsumer.receive(anyLong);
                result = mockAmqpsMsg;
            }
        };

        AmqpsIotHubSession sess = new AmqpsIotHubSession(mockConfig);
        sess.open();
        sess.receiveMessage();

        final JmsMessageConsumer expectedConsumer = mockConsumer;
        new Verifications()
        {
            {
                expectedConsumer.receive(anyLong);
            }
        };
    }

    // Tests_SRS_AMQPSIOTHUBSESSION_11_012: [If a message is found, the function shall include the message body in the returned message.]
    @Test
    public void receiveMessageReturnsMessageBody(
            @Mocked final IotHubMessageUri mockMessageUri,
            @Mocked final Message mockMsg,
            @Mocked final JmsBytesMessage mockAmqpsMsg,
            @Mocked final JmsQueue mockDest,
            @Mocked final JmsMessageConsumer mockConsumer)
            throws JMSException, IOException
    {
        final int amqpsMsgLength = 10;
        new NonStrictExpectations()
        {
            {
                mockConsumer.receive(anyLong);
                result = mockAmqpsMsg;
                mockAmqpsMsg.getBodyLength();
                result = amqpsMsgLength;
            }
        };

        AmqpsIotHubSession sess = new AmqpsIotHubSession(mockConfig);
        sess.open();
        sess.receiveMessage();

        new VerificationsInOrder()
        {
            {
                mockAmqpsMsg.getBodyLength();
                mockAmqpsMsg.readBytes((byte[]) any);
                new Message((byte[]) any);
            }
        };
    }

    // Tests_SRS_AMQPSIOTHUBSESSION_11_013: [If a message is found, the function shall include all valid application properties in the returned message.]
    @Test
    public void receiveMessageReturnsMessageAppProperties(
            @Mocked final IotHubMessageUri mockMessageUri,
            @Mocked final Message mockMsg,
            @Mocked final JmsBytesMessage mockAmqpsMsg,
            @Mocked final Enumeration mockPropertyNames,
            @Mocked final MessageProperty mockProperty,
            @Mocked final JmsQueue mockDest,
            @Mocked final JmsMessageConsumer mockConsumer)
            throws JMSException, IOException
    {
        final String propertyName0 = "test-property-0";
        final String propertyValue0 = "test-property-value-0";
        final String propertyName1 = "test-property-1";
        final String propertyValue1 = "test-property-value-1";
        final String propertyName2 = "test-property-2";
        final String propertyValue2 = "test-property-value-2";
        new NonStrictExpectations()
        {
            {
                mockConsumer.receive(anyLong);
                result = mockAmqpsMsg;
                mockAmqpsMsg.getPropertyNames();
                result = mockPropertyNames;
                mockPropertyNames.hasMoreElements();
                returns(true, true, true, false);
                mockPropertyNames.nextElement();
                returns(propertyName0, propertyName1, propertyName2);
                mockAmqpsMsg.getStringProperty(propertyName0);
                result = propertyValue0;
                mockAmqpsMsg.getStringProperty(propertyName1);
                result = propertyValue1;
                mockAmqpsMsg.getStringProperty(propertyName2);
                result = propertyValue2;
                MessageProperty
                        .isValidAppProperty(propertyName0, propertyValue0);
                result = true;
                MessageProperty
                        .isValidAppProperty(propertyName1, propertyValue1);
                result = false;
                MessageProperty
                        .isValidAppProperty(propertyName2, propertyValue2);
                result = true;
            }
        };

        AmqpsIotHubSession sess = new AmqpsIotHubSession(mockConfig);
        sess.open();
        sess.receiveMessage();

        final Message expectedMessage = mockMsg;
        new VerificationsInOrder()
        {
            {
                expectedMessage.setProperty(propertyName0, propertyValue0);
                expectedMessage.setProperty(propertyName2, propertyValue2);
            }
        };
    }

    // Tests_SRS_AMQPSIOTHUBSESSION_11_014: [If the IoT Hub could not be reached, the function shall throw an IOException.]
    @Test(expected = IOException.class)
    public void receiveMessageThrowsIOExceptionIfReceiveFails(
            @Mocked final IotHubMessageUri mockMessageUri,
            @Mocked final Message mockMsg,
            @Mocked final JmsQueue mockDest,
            @Mocked final JmsMessageConsumer mockConsumer)
            throws JMSException, IOException
    {
        new NonStrictExpectations()
        {
            {
                mockConsumer.receive(anyLong);
                result = new JMSException(anyString, anyString);
            }
        };

        AmqpsIotHubSession sess = new AmqpsIotHubSession(mockConfig);
        sess.open();
        sess.receiveMessage();
    }

    // Tests_SRS_AMQPSIOTHUBSESSION_11_029: [If the AMQPS session is closed, the function shall throw an IllegalStateException.]
    @Test(expected = IllegalStateException.class)
    public void receiveMessageFailsIfSessionNotYetOpened()
            throws IOException
    {
        AmqpsIotHubSession sess = new AmqpsIotHubSession(mockConfig);
        sess.receiveMessage();
    }

    // Tests_SRS_AMQPSIOTHUBSESSION_11_029: [If the AMQPS session is closed, the function shall throw an IllegalStateException.]
    @Test(expected = IllegalStateException.class)
    public void receiveMessageFailsIfSessionAlreadyClosed()
            throws IOException
    {
        AmqpsIotHubSession sess = new AmqpsIotHubSession(mockConfig);
        sess.open();
        sess.close();
        sess.receiveMessage();
    }

    // Tests_SRS_AMQPSIOTHUBSESSION_11_015: [If the message result is COMPLETE, the function shall acknowledge the last message with acknowledgement type CONSUMED.]
    @Test
    public void sendMessageResultSendsConsumedAckForComplete(
            @Mocked final IotHubMessageUri mockMessageUri,
            @Mocked final Message mockMsg,
            @Mocked final JmsBytesMessage mockAmqpsMsg,
            @Mocked final JmsQueue mockDest,
            @Mocked final JmsMessageConsumer mockConsumer)
            throws JMSException, IOException
    {
        new NonStrictExpectations()
        {
            {
                mockConsumer.receive(anyLong);
                result = mockAmqpsMsg;
            }
        };

        AmqpsIotHubSession sess = new AmqpsIotHubSession(mockConfig);
        sess.open();
        sess.receiveMessage();
        sess.sendMessageResult(IotHubMessageResult.COMPLETE);

        final JmsBytesMessage expectedAmqpsMsg = mockAmqpsMsg;
        new Verifications()
        {
            {
                expectedAmqpsMsg.acknowledge(
                        ProviderConstants.ACK_TYPE.CONSUMED);
            }
        };
    }

    // Tests_SRS_AMQPSIOTHUBSESSION_11_016: [If the message result is ABANDON, the function shall acknowledge the last message with acknowledgement type RELEASED.]
    @Test
    public void sendMessageResultSendsReleasedAckForAbandon(
            @Mocked final IotHubMessageUri mockMessageUri,
            @Mocked final Message mockMsg,
            @Mocked final JmsBytesMessage mockAmqpsMsg,
            @Mocked final JmsQueue mockDest,
            @Mocked final JmsMessageConsumer mockConsumer)
            throws JMSException, IOException
    {
        new NonStrictExpectations()
        {
            {
                mockConsumer.receive(anyLong);
                result = mockAmqpsMsg;
            }
        };

        AmqpsIotHubSession sess = new AmqpsIotHubSession(mockConfig);
        sess.open();
        sess.receiveMessage();
        sess.sendMessageResult(IotHubMessageResult.ABANDON);

        final JmsBytesMessage expectedAmqpsMsg = mockAmqpsMsg;
        new Verifications()
        {
            {
                expectedAmqpsMsg.acknowledge(
                        ProviderConstants.ACK_TYPE.RELEASED);
            }
        };
    }

    // Tests_SRS_AMQPSIOTHUBSESSION_11_017: [If the message result is REJECT, the function shall acknowledge the last message with acknowledgement type POISONED.]
    @Test
    public void sendMessageResultSendsPoisonedAckForReject(
            @Mocked final IotHubMessageUri mockMessageUri,
            @Mocked final Message mockMsg,
            @Mocked final JmsBytesMessage mockAmqpsMsg,
            @Mocked final JmsQueue mockDest,
            @Mocked final JmsMessageConsumer mockConsumer)
            throws JMSException, IOException
    {
        new NonStrictExpectations()
        {
            {
                mockConsumer.receive(anyLong);
                result = mockAmqpsMsg;
            }
        };

        AmqpsIotHubSession sess = new AmqpsIotHubSession(mockConfig);
        sess.open();
        sess.receiveMessage();
        sess.sendMessageResult(IotHubMessageResult.REJECT);

        final JmsBytesMessage expectedAmqpsMsg = mockAmqpsMsg;
        new Verifications()
        {
            {
                expectedAmqpsMsg.acknowledge(
                        ProviderConstants.ACK_TYPE.POISONED);
            }
        };
    }

    // Tests_SRS_AMQPSIOTHUBSESSION_11_018: [If sendMessageResult(result) is called before a message is received, the function shall throw an IllegalStateException.]
    @Test(expected = IllegalStateException.class)
    public void sendMessageResultFailsIfNoPreviousMessageReceived(
            @Mocked final IotHubMessageUri mockMessageUri,
            @Mocked final Message mockMsg,
            @Mocked final JmsQueue mockDest,
            @Mocked final JmsMessageConsumer mockConsumer)
            throws JMSException, IOException
    {
        new NonStrictExpectations()
        {
            {
                mockConsumer.receive(anyLong);
                result = null;
            }
        };

        AmqpsIotHubSession sess = new AmqpsIotHubSession(mockConfig);
        sess.open();
        sess.receiveMessage();
        sess.sendMessageResult(IotHubMessageResult.REJECT);
    }

    // Tests_SRS_AMQPSIOTHUBSESSION_11_019: [If the acknowledgement fails, the function shall throw an IOException.]
    @Test(expected = IOException.class)
    public void sendMessageResultThrowsIOExceptionIfAckFails(
            @Mocked final IotHubMessageUri mockMessageUri,
            @Mocked final Message mockMsg,
            @Mocked final JmsBytesMessage mockAmqpsMsg,
            @Mocked final JmsQueue mockDest,
            @Mocked final JmsMessageConsumer mockConsumer)
            throws JMSException, IOException
    {
        new NonStrictExpectations()
        {
            {
                mockConsumer.receive(anyLong);
                result = mockAmqpsMsg;
                mockAmqpsMsg.acknowledge((ProviderConstants.ACK_TYPE) any);
                result = new JMSException(anyString, anyString);
            }
        };

        AmqpsIotHubSession sess = new AmqpsIotHubSession(mockConfig);
        sess.open();
        sess.receiveMessage();
        sess.sendMessageResult(IotHubMessageResult.REJECT);
    }

    // Tests_SRS_AMQPSIOTHUBSESSION_11_031: [If the AMQPS session is closed, the function shall throw an IllegalStateException.]
    @Test(expected = IllegalStateException.class)
    public void sendMessageResultFailsIfSessionNotYetOpened()
            throws IOException
    {
        AmqpsIotHubSession sess = new AmqpsIotHubSession(mockConfig);
        sess.sendMessageResult(IotHubMessageResult.COMPLETE);
    }

    // Tests_SRS_AMQPSIOTHUBSESSION_11_031: [If the AMQPS session is closed, the function shall throw an IllegalStateException.]
    @Test(expected = IllegalStateException.class)
    public void sendMessageResultFailsIfSessionAlreadyClosed()
            throws IOException
    {
        AmqpsIotHubSession sess = new AmqpsIotHubSession(mockConfig);
        sess.open();
        sess.close();
        sess.sendMessageResult(IotHubMessageResult.COMPLETE);
    }
}
