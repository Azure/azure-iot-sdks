// Copyright (c) Microsoft. All rights reserved.
// Licensed under the MIT license. See LICENSE file in the project root for full license information.

package tests.unit.com.microsoft.azure.iothub.transport.amqps;

import static org.junit.Assert.assertThat;
import static org.hamcrest.CoreMatchers.is;

import com.microsoft.azure.iothub.DeviceClientConfig;
import com.microsoft.azure.iothub.Message;
import com.microsoft.azure.iothub.IotHubMessageResult;
import com.microsoft.azure.iothub.IotHubStatusCode;
import com.microsoft.azure.iothub.auth.IotHubSasToken;
import com.microsoft.azure.iothub.transport.amqps.AmqpsIotHubSession;
import com.microsoft.azure.iothub.net.IotHubUri;

import com.microsoft.azure.iothub.transport.amqps.AmqpsMessage;
import com.microsoft.azure.iothub.transport.amqps.AmqpsReceiver;
import com.microsoft.azure.iothub.transport.amqps.AmqpsSender;
import mockit.Mocked;
import mockit.NonStrictExpectations;
import mockit.Verifications;
import mockit.VerificationsInOrder;
import org.apache.qpid.proton.amqp.Binary;
import org.apache.qpid.proton.amqp.messaging.Data;
import org.apache.qpid.proton.engine.BaseHandler;
import org.apache.qpid.proton.engine.HandlerException;
import org.junit.Test;

import java.io.IOException;
import java.nio.charset.Charset;
import java.nio.charset.StandardCharsets;

/** Unit tests for AmqpsIotHubSession. */
public class AmqpsIotHubSessionTest
{
    protected static final Charset UTF8 = StandardCharsets.UTF_8;

    @Mocked
    protected DeviceClientConfig mockConfig;
    @Mocked
    protected AmqpsSender mockSender;
    @Mocked
    protected AmqpsReceiver mockReceiver;
    @Mocked
    protected IotHubUri mockIotHubUri;
    @Mocked
    protected IotHubSasToken mockToken;

    // Tests_SRS_AMQPSIOTHUBSESSION_11_022: [The constructor shall save the configuration.]
    // Tests_SRS_AMQPSIOTHUBSESSION_11_001: [The function shall establish an AMQPS session with an IoT Hub using the provided host name, user name, device ID, and sas token.]
    @Test
    public void openEstablishesSessionUsingCorrectConfig() throws IOException {
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
        final String expectedHostname = iotHubHostname;
        final String expectedDeviceId = deviceId;
        new Verifications()
        {
            {
                new AmqpsSender(expectedHostname, expectedUsername, expectedDeviceId, expectedPassword);
                new AmqpsReceiver(expectedHostname, expectedUsername, expectedDeviceId, expectedPassword);
            }
        };
    }

    // Tests_SRS_AMQPSIOTHUBSESSION_11_003: [If an AMQPS session is unable to be established, the function shall throw an IOException.]
    @Test(expected = IOException.class)
    public void openThrowsIOExceptionIfSessionFails()
            throws IOException, InterruptedException {
        new NonStrictExpectations()
        {
            {
                new AmqpsSender(anyString, anyString, anyString, anyString);
                result = mockSender;
                new AmqpsReceiver(anyString, anyString, anyString, anyString);
                result = mockReceiver;
                mockReceiver.open();
                result = new Exception(anyString);
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
                new AmqpsSender(anyString, anyString, anyString, anyString); times = 1;
                new AmqpsReceiver(anyString, anyString, anyString, anyString); times = 1;
            }
        };
    }

    // Tests_SRS_AMQPSIOTHUBSESSION_11_023: [The function shall close the AMQPS session.]
    @Test
    public void closeClosesAmqpsSession() throws IOException
    {
        AmqpsIotHubSession session = new AmqpsIotHubSession(mockConfig);
        session.open();
        session.close();

        final AmqpsSender expectedSender = mockSender;
        final AmqpsReceiver expectedReceiver = mockReceiver;
        new Verifications()
        {
            {
                expectedSender.close();
                expectedReceiver.close();
            }
        };
    }

    // Tests_SRS_AMQPSIOTHUBSESSION_11_025: [If the AMQPS session is closed, the function shall do nothing.]
    @Test
    public void closeDoesNothingIfSessionNotYetOpened()
            throws IOException
    {
        AmqpsIotHubSession session = new AmqpsIotHubSession(mockConfig);
        session.close();

        final AmqpsSender expectedSender = mockSender;
        final AmqpsReceiver expectedReceiver = mockReceiver;
        new Verifications()
        {
            {
                expectedSender.close(); times = 0;
                expectedReceiver.close(); times = 0;
            }
        };
    }

    // Tests_SRS_AMQPSIOTHUBSESSION_11_025: [If the AMQPS session is closed, the function shall do nothing.]
    @Test
    public void closeDoesNothingIfSessionAlreadyClosed()
            throws IOException
    {
        AmqpsIotHubSession session = new AmqpsIotHubSession(mockConfig);
        session.open();
        session.close();
        session.close();

        final AmqpsSender expectedSender = mockSender;
        final AmqpsReceiver expectedReceiver = mockReceiver;
        new Verifications()
        {
            {
                expectedSender.close(); times = 1;
                expectedReceiver.close(); times = 1;
            }
        };
    }

    // Tests_SRS_AMQPSIOTHUBSESSION_11_004: [The function shall send an event message to the IoT Hub given in the configuration.]
    // Tests_SRS_AMQPSIOTHUBSESSION_11_005: [The function shall send the message body.]
    @Test
    public void sendEventSendsEventToCorrectIotHub(
            @Mocked final Message mockMsg)
            throws IOException
    {
        final byte[] msgBody = { 0x61, 0x62, 0x63 };
        new NonStrictExpectations()
        {
            {
                mockMsg.getBytes();
                result = msgBody;
            }
        };

        AmqpsIotHubSession sess = new AmqpsIotHubSession(mockConfig);
        sess.open();
        sess.sendEvent(mockMsg);

        final AmqpsSender expectedSender = mockSender;
        new Verifications()
        {
            {
                expectedSender.send(msgBody, any);
            }
        };
    }

//    // Tests_SRS_AMQPSIOTHUBSESSION_11_007: [The function shall send all message properties.]
//    @Test
//    public void sendEventSetsAllMessageProperties(
//            @Mocked final IotHubEventUri mockEventUri,
//            @Mocked final Message mockMsg,
//            @Mocked final JmsQueue mockDest,
//            @Mocked final JmsMessageProducer mockProducer,
//            @Mocked final JmsBytesMessage mockBytesMsg,
//            @Mocked final MessageProperty mockProperty)
//            throws JMSException, IOException
//    {
//        final MessageProperty[] msgProperties = { mockProperty };
//        final String propertyName = "test-property-name";
//        final String propertyValue = "test-property-value";
//        new NonStrictExpectations()
//        {
//            {
//                new JmsQueue(anyString);
//                result = mockDest;
//                mockSess.createProducer(mockDest);
//                result = mockProducer;
//                mockMsg.getProperties();
//                result = msgProperties;
//                mockSess.createBytesMessage();
//                result = mockBytesMsg;
//                mockProperty.getName();
//                result = propertyName;
//                mockProperty.getValue();
//                result = propertyValue;
//            }
//        };
//
//        AmqpsIotHubSession sess = new AmqpsIotHubSession(mockConfig);
//        sess.open();
//        sess.sendEvent(mockMsg);
//
//        final JmsBytesMessage expectedBytesMsg = mockBytesMsg;
//        final String expectedPropertyName = propertyName;
//        final String expectedPropertyValue = propertyValue;
//        new VerificationsInOrder()
//        {
//            {
//                mockBytesMsg.setStringProperty(expectedPropertyName,
//                        expectedPropertyValue);
//                mockProducer.send(expectedBytesMsg);
//            }
//        };
//    }

    // Tests_SRS_AMQPSIOTHUBSESSION_11_008: [If the message was successfully received by the service, the function shall return status code OK_EMPTY.]
    @Test
    public void sendEventReturnsOkEmptyIfSuccessful(
            @Mocked final Message mockMsg)
            throws IOException
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
            @Mocked final Message mockMsg)
            throws IOException
    {
        final byte[] msgBody = { 0x61, 0x62, 0x63 };
        new NonStrictExpectations()
        {
            {
                mockMsg.getBytes();
                result = msgBody;
                mockSender.send(msgBody, any);
                result = new HandlerException(new BaseHandler(), new Throwable());
            }
        };

        AmqpsIotHubSession sess = new AmqpsIotHubSession(mockConfig);
        sess.open();
        IotHubStatusCode testStatus = sess.sendEvent(mockMsg);

        IotHubStatusCode expectedStatus = IotHubStatusCode.ERROR;
        assertThat(testStatus, is(expectedStatus));
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
    public void receiveMessageSendsToCorrectIotHub()
            throws IOException
    {
        new NonStrictExpectations()
        {
            {
                mockReceiver.consumeMessage();
                result = null;
            }
        };
        AmqpsIotHubSession sess = new AmqpsIotHubSession(mockConfig);
        sess.open();
        sess.receiveMessage();

        final AmqpsReceiver expectedReceiver = mockReceiver;
        new Verifications()
        {
            {
                expectedReceiver.consumeMessage();
            }
        };
    }

    // Tests_SRS_AMQPSIOTHUBSESSION_11_012: [If a message is found, the function shall include the message body in the returned message.]
    @Test
    public void receiveMessageReturnsMessageBody(
            @Mocked final Message mockMsg,
            @Mocked final AmqpsMessage mockAmqpsMsg)
            throws IOException
    {
        final byte[] msgBody = { 0x61, 0x62, 0x63 };
        new NonStrictExpectations()
        {
            {
                mockReceiver.consumeMessage();
                result = mockAmqpsMsg;
                mockAmqpsMsg.getBody();
                result = new Data(new Binary(msgBody));
            }
        };

        AmqpsIotHubSession sess = new AmqpsIotHubSession(mockConfig);
        sess.open();
        sess.receiveMessage();

        new VerificationsInOrder()
        {
            {
                new Message(msgBody);
            }
        };
    }

//    // Tests_SRS_AMQPSIOTHUBSESSION_11_013: [If a message is found, the function shall include all valid application properties in the returned message.]
//    @Test
//    public void receiveMessageReturnsMessageAppProperties(
//            @Mocked final IotHubMessageUri mockMessageUri,
//            @Mocked final Message mockMsg,
//            @Mocked final JmsBytesMessage mockAmqpsMsg,
//            @Mocked final Enumeration mockPropertyNames,
//            @Mocked final MessageProperty mockProperty,
//            @Mocked final JmsQueue mockDest,
//            @Mocked final JmsMessageConsumer mockConsumer)
//            throws JMSException, IOException
//    {
//        final String propertyName0 = "test-property-0";
//        final String propertyValue0 = "test-property-value-0";
//        final String propertyName1 = "test-property-1";
//        final String propertyValue1 = "test-property-value-1";
//        final String propertyName2 = "test-property-2";
//        final String propertyValue2 = "test-property-value-2";
//        new NonStrictExpectations()
//        {
//            {
//                mockConsumer.receive(anyLong);
//                result = mockAmqpsMsg;
//                mockAmqpsMsg.getPropertyNames();
//                result = mockPropertyNames;
//                mockPropertyNames.hasMoreElements();
//                returns(true, true, true, false);
//                mockPropertyNames.nextElement();
//                returns(propertyName0, propertyName1, propertyName2);
//                mockAmqpsMsg.getStringProperty(propertyName0);
//                result = propertyValue0;
//                mockAmqpsMsg.getStringProperty(propertyName1);
//                result = propertyValue1;
//                mockAmqpsMsg.getStringProperty(propertyName2);
//                result = propertyValue2;
//                MessageProperty
//                        .isValidAppProperty(propertyName0, propertyValue0);
//                result = true;
//                MessageProperty
//                        .isValidAppProperty(propertyName1, propertyValue1);
//                result = false;
//                MessageProperty
//                        .isValidAppProperty(propertyName2, propertyValue2);
//                result = true;
//            }
//        };
//
//        AmqpsIotHubSession sess = new AmqpsIotHubSession(mockConfig);
//        sess.open();
//        sess.receiveMessage();
//
//        final Message expectedMessage = mockMsg;
//        new VerificationsInOrder()
//        {
//            {
//                expectedMessage.setProperty(propertyName0, propertyValue0);
//                expectedMessage.setProperty(propertyName2, propertyValue2);
//            }
//        };
//    }

    // Tests_SRS_AMQPSIOTHUBSESSION_11_014: [If the IoT Hub could not be reached, the function shall throw an IOException.]
    @Test(expected = IOException.class)
    public void receiveMessageThrowsIOExceptionIfReceiveFails()
            throws IOException
    {
        new NonStrictExpectations()
        {
            {
                mockReceiver.consumeMessage();
                result = new IOException(anyString);
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
            @Mocked final AmqpsMessage mockAmqpsMsg)
            throws IOException
    {
        final byte[] msgBody = { 0x61, 0x62, 0x63 };
        new NonStrictExpectations()
        {
            {
                mockAmqpsMsg.getBody();
                result = new Data(new Binary(msgBody));
                mockReceiver.consumeMessage();
                result = mockAmqpsMsg;
            }
        };

        AmqpsIotHubSession sess = new AmqpsIotHubSession(mockConfig);
        sess.open();
        sess.receiveMessage();
        sess.sendMessageResult(IotHubMessageResult.COMPLETE);

        final AmqpsMessage expectedAmqpsMsg = mockAmqpsMsg;
        new Verifications()
        {
            {
                expectedAmqpsMsg.acknowledge(
                        AmqpsMessage.ACK_TYPE.COMPLETE);
            }
        };
    }

    // Tests_SRS_AMQPSIOTHUBSESSION_11_016: [If the message result is ABANDON, the function shall acknowledge the last message with acknowledgement type RELEASED.]
    @Test
    public void sendMessageResultSendsConsumedAckForAbandon(
            @Mocked final AmqpsMessage mockAmqpsMsg)
            throws IOException
    {
        final byte[] msgBody = { 0x61, 0x62, 0x63 };
        new NonStrictExpectations()
        {
            {
                mockAmqpsMsg.getBody();
                result = new Data(new Binary(msgBody));
                mockReceiver.consumeMessage();
                result = mockAmqpsMsg;
            }
        };

        AmqpsIotHubSession sess = new AmqpsIotHubSession(mockConfig);
        sess.open();
        sess.receiveMessage();
        sess.sendMessageResult(IotHubMessageResult.ABANDON);

        final AmqpsMessage expectedAmqpsMsg = mockAmqpsMsg;
        new Verifications()
        {
            {
                expectedAmqpsMsg.acknowledge(
                        AmqpsMessage.ACK_TYPE.ABANDON);
            }
        };
    }

    // Tests_SRS_AMQPSIOTHUBSESSION_11_017: [If the message result is REJECT, the function shall acknowledge the last message with acknowledgement type POISONED.]
    @Test
    public void sendMessageResultSendsConsumedAckForReject(
            @Mocked final AmqpsMessage mockAmqpsMsg)
            throws IOException
    {
        final byte[] msgBody = { 0x61, 0x62, 0x63 };
        new NonStrictExpectations()
        {
            {
                mockAmqpsMsg.getBody();
                result = new Data(new Binary(msgBody));
                mockReceiver.consumeMessage();
                result = mockAmqpsMsg;
            }
        };

        AmqpsIotHubSession sess = new AmqpsIotHubSession(mockConfig);
        sess.open();
        sess.receiveMessage();
        sess.sendMessageResult(IotHubMessageResult.REJECT);

        final AmqpsMessage expectedAmqpsMsg = mockAmqpsMsg;
        new Verifications()
        {
            {
                expectedAmqpsMsg.acknowledge(
                        AmqpsMessage.ACK_TYPE.REJECT);
            }
        };
    }

    // Tests_SRS_AMQPSIOTHUBSESSION_11_018: [If sendMessageResult(result) is called before a message is received, the function shall throw an IllegalStateException.]
    @Test(expected = IllegalStateException.class)
    public void sendMessageResultFailsIfNoPreviousMessageReceived()
            throws IOException
    {
        new NonStrictExpectations()
        {
            {
                mockReceiver.consumeMessage();
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
            @Mocked final AmqpsMessage mockAmqpsMsg)
            throws IOException
    {
        final byte[] msgBody = { 0x61, 0x62, 0x63 };
        new NonStrictExpectations()
        {
            {
                mockAmqpsMsg.getBody();
                result = new Data(new Binary(msgBody));
                mockReceiver.consumeMessage();
                result = mockAmqpsMsg;
                mockAmqpsMsg.acknowledge((AmqpsMessage.ACK_TYPE) any);
                result = new Exception(anyString);
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
