// Copyright (c) Microsoft. All rights reserved.
// Licensed under the MIT license. See LICENSE file in the project root for full license information.

package tests.unit.com.microsoft.azure.iothub;

import static org.hamcrest.CoreMatchers.is;
import static org.hamcrest.CoreMatchers.not;
import static org.junit.Assert.assertThat;

import com.microsoft.azure.iothub.Message;
import com.microsoft.azure.iothub.MessageProperty;
import mockit.Mocked;
import mockit.NonStrictExpectations;
import mockit.Verifications;
import org.junit.Test;

import java.nio.charset.Charset;
import java.nio.charset.StandardCharsets;

/** Unit tests for Message. */
public class MessageTest
{
    protected static Charset UTF8 = StandardCharsets.UTF_8;

    // Tests_SRS_MESSAGE_11_024: [The constructor shall save the message body.]
    // Tests_SRS_MESSAGE_11_002: [The function shall return the message body.]
    @Test
    public void constructorSavesBody()
    {
        final byte[] body = { 1, 2, 3 };

        Message msg = new Message(body);
        byte[] testBody = msg.getBytes();

        byte[] expectedBody = body;
        assertThat(testBody, is(expectedBody));
    }

    // Tests_SRS_MESSAGE_11_025: [If the message body is null, the constructor shall throw an IllegalArgumentException.]
    @Test(expected = IllegalArgumentException.class)
    public void constructorRejectsNullBody()
    {
        final byte[] body = null;

        new Message(body);
    }

    // Tests_SRS_MESSAGE_11_022: [The function shall return the message body, encoded using charset UTF-8.]
    @Test
    public void getBodyAsStringReturnsUtf8Body()
    {
        final byte[] body = { 0x61, 0x62, 0x63 };

        Message msg = new Message(body);
        String testBody = new String(msg.getBytes(), Message.DEFAULT_IOTHUB_MESSAGE_CHARSET);

        String expectedBody = new String(body, UTF8);
        assertThat(testBody, is(expectedBody));
    }

    // Tests_SRS_MESSAGE_11_026: [The function shall set the message property to the given value.]
    // Tests_SRS_MESSAGE_11_032: [The function shall return the value associated with the message property name, where the name can be either the HTTPS or AMQPS property name.]
    @Test
    public void setPropertyAndGetPropertyMatch(
            @Mocked final MessageProperty mockProperty)
    {
        final byte[] body = { 0x61, 0x62, 0x63 };
        final String name = "test-name";
        final String value1 = "test-value1";
        final String value2 = "test-value2";
        new NonStrictExpectations()
        {
            {
                new MessageProperty(name, value1);
                result = mockProperty;
                mockProperty.hasSameName(name);
                result = true;
                mockProperty.getValue();
                result = value1;
            }
        };

        Message msg = new Message(body);
        msg.setProperty(name, value1);

        String testValue = msg.getProperty(name);
        String expectedValue = value1;

        assertThat(testValue, is(expectedValue));

        new NonStrictExpectations()
        {
            {
                new MessageProperty(name, value2);
                result = mockProperty;
                mockProperty.hasSameName(name);
                result = true;
                mockProperty.getValue();
                result = value2;
            }
        };

        msg.setProperty(name, value2);
        testValue = msg.getProperty(name);
        expectedValue = value2;

        assertThat(testValue, is(expectedValue));
    }

    // Tests_SRS_MESSAGE_11_028: [If name is null, the function shall throw an IllegalArgumentException.]
    @Test(expected = IllegalArgumentException.class)
    public void setPropertyRejectsNullName()
    {
        final byte[] body = { 0x61, 0x62, 0x63 };
        final String value = "test-value";

        Message msg = new Message(body);
        msg.setProperty(null, value);
    }

    // Tests_SRS_MESSAGE_11_029: [If value is null, the function shall throw an IllegalArgumentException.]
    @Test(expected = IllegalArgumentException.class)
    public void setPropertyRejectsNullValue()
    {
        final byte[] body = { 0x61, 0x62, 0x63 };
        final String name = "test-name";

        Message msg = new Message(body);
        msg.setProperty(name, null);
    }

    // Tests_SRS_MESSAGE_11_030: [If name contains a character not specified in RFC 2047, the function shall throw an IllegalArgumentException.]
    @Test(expected = IllegalArgumentException.class)
    public void setPropertyRejectsIllegalName(
            @Mocked final MessageProperty mockProperty)
    {
        final byte[] body = { 0x61, 0x62, 0x63 };
        final String invalidName = "  ";
        final String value = "test-value";
        new NonStrictExpectations()
        {
            {
                new MessageProperty(invalidName, value);
                result = new IllegalArgumentException();
            }
        };

        Message msg = new Message(body);
        msg.setProperty(invalidName, value);
    }

    // Tests_SRS_MESSAGE_11_031: [If value name contains a character not specified in RFC 2047, the function shall throw an IllegalArgumentException.]
    @Test(expected = IllegalArgumentException.class)
    public void setPropertyRejectsIllegalValue(
            @Mocked final MessageProperty mockProperty)
    {
        final byte[] body = { 0x61, 0x62, 0x63 };
        final String name = "test-name";
        final String invalidValue = "test-value@";
        new NonStrictExpectations()
        {
            {
                new MessageProperty(name, invalidValue);
                result = new IllegalArgumentException();
            }
        };

        Message msg = new Message(body);
        msg.setProperty(name, invalidValue);
    }

    // Tests_SRS_MESSAGE_11_034: [If no value associated with the property name is found, the function shall throw an IllegalArgumentException.]
    @Test(expected = IllegalArgumentException.class)
    public void getPropertyRejectsNonexistentProperty(
            @Mocked final MessageProperty mockProperty)
    {
        final byte[] body = { 0x61, 0x62, 0x63 };
        final String name = "test-name";

        Message msg = new Message(body);
        msg.getProperty(name);
    }

    // Tests_SRS_MESSAGE_11_033: [The function shall return a copy of the message properties.]
    @Test
    public void getPropertiesReturnsCopyOfProperties(
            @Mocked final MessageProperty mockProperty)
    {
        final byte[] body = { 0x61, 0x62, 0x63 };
        final String name = "test-name";
        final String value = "test-value";
        final String httpsName = "test-https-name";
        new NonStrictExpectations()
        {
            {
                new MessageProperty(name, value);
                result = mockProperty;
                mockProperty.hasSameName(name);
                result = true;
                mockProperty.getValue();
                result = value;
                mockProperty.getName();
                result = httpsName;
            }
        };

        Message msg = new Message(body);
        msg.setProperty(name, value);
        MessageProperty[] testProperties = msg.getProperties();

        int expectedNumProperties = 1;
        assertThat(testProperties.length, is(expectedNumProperties));
        assertThat(testProperties[0], is(not(mockProperty)));
    }
}
