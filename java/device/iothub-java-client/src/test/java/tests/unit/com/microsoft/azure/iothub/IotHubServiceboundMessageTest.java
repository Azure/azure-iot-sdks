// Copyright (c) Microsoft. All rights reserved.
// Licensed under the MIT license. See LICENSE file in the project root for full license information.

package tests.unit.com.microsoft.azure.iothub;

import static org.junit.Assert.assertThat;
import static org.hamcrest.CoreMatchers.is;

import com.microsoft.azure.iothub.IotHubMessage;
import com.microsoft.azure.iothub.IotHubMessageProperty;
import com.microsoft.azure.iothub.IotHubServiceboundMessage;

import mockit.Mocked;
import mockit.NonStrictExpectations;
import mockit.Verifications;
import org.junit.Test;

import java.nio.charset.Charset;
import java.nio.charset.StandardCharsets;

import javax.naming.SizeLimitExceededException;
import javax.xml.bind.DatatypeConverter;

/** Unit tests for IotHubServiceboundMessage. */
public class IotHubServiceboundMessageTest
{
    protected static Charset UTF8 = StandardCharsets.UTF_8;
    protected static int SERVICEBOUND_MESSAGE_MAX_SIZE_BYTES = 256 * 1024;

    @Mocked
    protected IotHubMessage mockMsg;
    @Mocked
    protected DatatypeConverter mockConverter;

    // Tests_SRS_IOTHUBSERVICEBOUNDMESSAGE_11_001: [The constructor shall Base64-encode the message.]
    // Tests_SRS_IOTHUBSERVICEBOUNDMESSAGE_11_021: [If base64Encoded is set to true, the function shall return the Base64-encoded message, using charset UTF-8.]
    @Test
    public void byteConstructorBase64EncodesBody() throws
            SizeLimitExceededException
    {
        final byte[] rawMsg = { 0x61, 0x62, 0x63 };
        final String base64EncodedMsgStr = "YWJj";
        final byte[] base64EncodedMsg = base64EncodedMsgStr.getBytes(UTF8);
        new NonStrictExpectations()
        {
            {
                DatatypeConverter.printBase64Binary(rawMsg);
                result = base64EncodedMsgStr;
                new IotHubMessage(base64EncodedMsg);
                result = mockMsg;
                mockMsg.getBodyAsString();
                result = base64EncodedMsgStr;
            }
        };

        IotHubServiceboundMessage msg = new IotHubServiceboundMessage(rawMsg);
        String testBody = msg.getBodyAsString();

        final String expectedBody = base64EncodedMsgStr;
        assertThat(testBody, is(expectedBody));
    }

    // Tests_SRS_IOTHUBSERVICEBOUNDMESSAGE_11_007: [If the body length is greater than 256 kb after Base64-encoding it, the constructor shall throw a SizeLimitExceededException.]
    @Test(expected = SizeLimitExceededException.class)
    public void byteConstructorRejectsOverflowingBody()
            throws SizeLimitExceededException
    {
        final byte[] rawMsg = new byte[SERVICEBOUND_MESSAGE_MAX_SIZE_BYTES + 1];
        final String base64EncodedMsgStr = new String(rawMsg, UTF8);
        new NonStrictExpectations()
        {
            {
                DatatypeConverter.printBase64Binary(rawMsg);
                result = base64EncodedMsgStr;
            }
        };

        new IotHubServiceboundMessage(rawMsg);
    }

    // Tests_SRS_IOTHUBSERVICEBOUNDMESSAGE_11_017: [If body is null, the constructor shall throw an IllegalArgumentException.]
    @Test(expected = IllegalArgumentException.class)
    public void byteConstructorRejectsNullMsg()
            throws SizeLimitExceededException
    {
        final byte[] rawMsg = null;

        new IotHubServiceboundMessage(rawMsg);
    }

    // Tests_SRS_IOTHUBSERVICEBOUNDMESSAGE_11_008: [The constructor shall save the message as a byte array using charset UTF-8.]
    // Tests_SRS_IOTHUBSERVICEBOUNDMESSAGE_11_022: [If base64Encoded is set to false, the function shall return the raw message, using charset UTF-8.]
    @Test
    public void stringConstructorSavesBody() throws SizeLimitExceededException
    {
        final String msgStr = "test-msg";
        new NonStrictExpectations()
        {
            {
                new IotHubMessage(msgStr.getBytes(UTF8));
                result = mockMsg;
                mockMsg.getBodyAsString();
                result = msgStr;
            }
        };

        IotHubServiceboundMessage msg = new IotHubServiceboundMessage(msgStr);
        String testBody = msg.getBodyAsString();

        final String expectedBody = msgStr;
        assertThat(testBody, is(expectedBody));
    }

    // Tests_SRS_IOTHUBSERVICEBOUNDMESSAGE_11_010: [If the body byte array length is greater than 256 kb, the constructor shall throw a SizeLimitExceededException.]
    @Test(expected = SizeLimitExceededException.class)
    public void stringConstructorRejectsOverflowingBody()
            throws SizeLimitExceededException
    {
        final byte[] rawMsg = new byte[SERVICEBOUND_MESSAGE_MAX_SIZE_BYTES + 1];
        final String rawMsgStr = new String(rawMsg, UTF8);

        new IotHubServiceboundMessage(rawMsgStr);
    }

    // Tests_SRS_IOTHUBSERVICEBOUNDMESSAGE_11_019: [If body is null, the constructor shall throw an IllegalArgumentException.]
    @Test(expected = IllegalArgumentException.class)
    public void stringConstructorRejectsNullMsg()
            throws SizeLimitExceededException
    {
        final String rawMsg = null;

        new IotHubServiceboundMessage(rawMsg);
    }

    // Tests_SRS_IOTHUBSERVICEBOUNDMESSAGE_11_002: [If base64Encoded is set to true, the function shall return the Base64-encoded message.]
    @Test
    public void getBodyReturnsBase64BodyWhenBase64Encoded()
            throws SizeLimitExceededException
    {
        final byte[] rawMsg = { 0x61, 0x62, 0x63 };
        final String base64EncodedMsgStr = "YWJj";
        final byte[] base64EncodedMsg = base64EncodedMsgStr.getBytes(UTF8);
        new NonStrictExpectations()
        {
            {
                DatatypeConverter.printBase64Binary(rawMsg);
                result = base64EncodedMsgStr;
                new IotHubMessage(base64EncodedMsg);
                result = mockMsg;
                mockMsg.getBody();
                result = base64EncodedMsg;
            }
        };

        IotHubServiceboundMessage msg = new IotHubServiceboundMessage(rawMsg);
        byte[] testBody = msg.getBody();

        final byte[] expectedBody = base64EncodedMsg;
        assertThat(testBody, is(expectedBody));
    }

    // Tests_SRS_IOTHUBSERVICEBOUNDMESSAGE_11_003: [If base64Encoded is set to false, the function shall return the raw message.]
    @Test
    public void getBodyReturnsPlainBodyWhenPlain()
            throws SizeLimitExceededException
    {
        final byte[] rawMsg = { 0x61, 0x62, 0x63 };
        final String rawMsgStr = new String(rawMsg, UTF8);
        new NonStrictExpectations()
        {
            {
                new IotHubMessage(rawMsg);
                result = mockMsg;
                mockMsg.getBody();
                result = rawMsg;
            }
        };

        IotHubServiceboundMessage msg =
                new IotHubServiceboundMessage(rawMsgStr);
        byte[] testBody = msg.getBody();

        final byte[] expectedBody = rawMsg;
        assertThat(testBody, is(expectedBody));
    }

    // Tests_SRS_IOTHUBSERVICEBOUNDMESSAGE_11_005: [The function shall return whether getBody() will return a Base64-encoded message.]
    @Test
    public void isBase64EncodedReturnsIsBase64Encoded()
            throws SizeLimitExceededException
    {
        final byte[] rawMsg = { 0x61, 0x62, 0x63 };
        final String base64EncodedMsgStr = "YWJj";
        final byte[] base64EncodedMsg = base64EncodedMsgStr.getBytes(UTF8);
        new NonStrictExpectations()
        {
            {
                DatatypeConverter.printBase64Binary(rawMsg);
                result = base64EncodedMsgStr;
                new IotHubMessage(base64EncodedMsg);
                result = mockMsg;
            }
        };

        IotHubServiceboundMessage msg = new IotHubServiceboundMessage(rawMsg);
        boolean testIsBase64Encoded = msg.isBase64Encoded();

        final boolean expectedIsBase64Encoded = true;
        assertThat(testIsBase64Encoded, is(expectedIsBase64Encoded));
    }

    // Tests_SRS_IOTHUBSERVICEBOUNDMESSAGE_11_005: [The function shall return whether getBody() will return a Base64-encoded message.]
    @Test
    public void isNotBase64EncodedReturnsIsNotBase64Encoded()
            throws SizeLimitExceededException
    {
        final String msgStr = "test-msg";

        IotHubServiceboundMessage msg = new IotHubServiceboundMessage(msgStr);
        boolean testIsBase64Encoded = msg.isBase64Encoded();

        final boolean expectedIsBase64Encoded = false;
        assertThat(testIsBase64Encoded, is(expectedIsBase64Encoded));
    }

    // Tests_SRS_IOTHUBSERVICEBOUNDMESSAGE_11_023: [The function shall set the message property to the given value.]
    // Tests_SRS_IOTHUBSERVICEBOUNDMESSAGE_11_029: [The function shall return the value associated with the message property name, where the name can be either the HTTPS or AMQPS property name.]
    @Test
    public void setPropertyAndGetPropertyMatch()
            throws SizeLimitExceededException
    {
        final String msgStr = "test-msg";
        final String name = "test-property-name";
        final String value = "test-property-value";
        new NonStrictExpectations()
        {
            {
                new IotHubMessage((byte[]) any);
                result = mockMsg;
            }
        };

        IotHubServiceboundMessage msg = new IotHubServiceboundMessage(msgStr);
        msg.setProperty(name, value);

        new Verifications()
        {
            {
                mockMsg.setProperty(name, value);
            }
        };
    }

    // Tests_SRS_IOTHUBSERVICEBOUNDMESSAGE_11_024: [The function shall return itself, for fluent setting.]
    @Test
    public void setPropertyReturnsSelf()
            throws SizeLimitExceededException
    {
        final String msgStr = "test-msg";
        final String name = "test-property-name";
        final String value = "test-property-value";
        new NonStrictExpectations()
        {
            {
                new IotHubMessage((byte[]) any);
                result = mockMsg;
            }
        };

        IotHubServiceboundMessage msg = new IotHubServiceboundMessage(msgStr);
        IotHubServiceboundMessage testMsg = msg.setProperty(name, value);

        final IotHubServiceboundMessage expectedMsg = msg;
        assertThat(testMsg, is(expectedMsg));
    }

    // Tests_SRS_IOTHUBSERVICEBOUNDMESSAGE_11_025: [If name is null, the function shall throw an IllegalArgumentException.]
    @Test(expected = IllegalArgumentException.class)
    public void setPropertyRejectsNullName()
            throws SizeLimitExceededException
    {
        final String msgStr = "test-msg";
        final String value = "test-property-value";
        new NonStrictExpectations()
        {
            {
                new IotHubMessage((byte[]) any);
                result = mockMsg;
                mockMsg.setProperty(null, value);
                result = new IllegalArgumentException();
            }
        };

        IotHubServiceboundMessage msg = new IotHubServiceboundMessage(msgStr);
        msg.setProperty(null, value);
    }

    // Tests_SRS_IOTHUBSERVICEBOUNDMESSAGE_11_026: [If value is null, the function shall throw an IllegalArgumentException.]
    @Test(expected = IllegalArgumentException.class)
    public void setPropertyRejectsNullValue()
            throws SizeLimitExceededException
    {
        final String msgStr = "test-msg";
        final String name = "test-property-name";
        new NonStrictExpectations()
        {
            {
                new IotHubMessage((byte[]) any);
                result = mockMsg;
                mockMsg.setProperty(name, null);
                result = new IllegalArgumentException();
            }
        };

        IotHubServiceboundMessage msg = new IotHubServiceboundMessage(msgStr);
        msg.setProperty(name, null);
    }

    // Tests_SRS_IOTHUBSERVICEBOUNDMESSAGE_11_027: [If name contains a character not specified in RFC 2047, the function shall throw an IllegalArgumentException.]
    @Test(expected = IllegalArgumentException.class)
    public void setPropertyRejectsInvalidName()
            throws SizeLimitExceededException
    {
        final String msgStr = "test-msg";
        final String invalidName = "[test-property-name]";
        final String value = "test-property-value";
        new NonStrictExpectations()
        {
            {
                new IotHubMessage((byte[]) any);
                result = mockMsg;
                mockMsg.setProperty(invalidName, value);
                result = new IllegalArgumentException();
            }
        };

        IotHubServiceboundMessage msg = new IotHubServiceboundMessage(msgStr);
        msg.setProperty(invalidName, value);
    }

    // Tests_SRS_IOTHUBSERVICEBOUNDMESSAGE_11_028: [If value name contains a character not specified in RFC 2047, the function shall throw an IllegalArgumentException.]
    @Test(expected = IllegalArgumentException.class)
    public void setPropertyRejectsInvalidValue()
            throws SizeLimitExceededException
    {
        final String msgStr = "test-msg";
        final String name = "test-property-name";
        final String invalidValue = "test-property-value:";
        new NonStrictExpectations()
        {
            {
                new IotHubMessage((byte[]) any);
                result = mockMsg;
                mockMsg.setProperty(name, invalidValue);
                result = new IllegalArgumentException();
            }
        };

        IotHubServiceboundMessage msg = new IotHubServiceboundMessage(msgStr);
        msg.setProperty(name, invalidValue);
    }

    // Tests_SRS_IOTHUBSERVICEBOUNDMESSAGE_11_030: [If no value associated with the property name is found, the function shall throw an IllegalArgumentException.]
    @Test(expected = IllegalArgumentException.class)
    public void getPropertyRejectsNonexistentName()
            throws SizeLimitExceededException
    {
        final String msgStr = "test-msg";
        final String name = "test-property-name";
        new NonStrictExpectations()
        {
            {
                new IotHubMessage((byte[]) any);
                result = mockMsg;
                mockMsg.getProperty(name);
                result = new IllegalArgumentException();
            }
        };

        IotHubServiceboundMessage msg = new IotHubServiceboundMessage(msgStr);
        msg.getProperty(name);
    }

    // Tests_SRS_IOTHUBSERVICEBOUNDMESSAGE_11_031: [The function shall return a copy of the message properties.]
    @Test
    public void getPropertiesReturnsCopy(
            @Mocked final IotHubMessageProperty mockProperty)
            throws SizeLimitExceededException
    {
        final String msgStr = "test-msg";
        final IotHubMessageProperty[] properties = { mockProperty };
        new NonStrictExpectations()
        {
            {
                new IotHubMessage((byte[]) any);
                result = mockMsg;
                mockMsg.getProperties(); result = properties;
            }
        };

        IotHubServiceboundMessage msg = new IotHubServiceboundMessage(msgStr);
        IotHubMessageProperty[] testProperties = msg.getProperties();

        IotHubMessageProperty[] expectedProperties = properties;
        assertThat(testProperties, is(expectedProperties));
    }
}
