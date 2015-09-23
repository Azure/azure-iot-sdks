// Copyright (c) Microsoft. All rights reserved.
// Licensed under the MIT license. See LICENSE file in the project root for full license information.

package com.microsoft.azure.iothub;

import java.nio.charset.Charset;
import java.nio.charset.StandardCharsets;
import java.util.ArrayList;
import java.util.Arrays;

/**
 * A message to or from an IoT Hub.
 */
public final class IotHubMessage
{
    /** The default charset used to decode string messages. */
    public static final Charset IOTHUB_MESSAGE_DEFAULT_CHARSET =
            StandardCharsets.UTF_8;

    /** The message body. */
    protected final byte[] body;
    /** Message properties. */
    protected ArrayList<IotHubMessageProperty> properties;

    /**
     * Constructs an instance of {@code IotHubMessage} given the
     * message body as a {@code byte} array.
     *
     * @param body the raw message body.
     *
     * @throws IllegalArgumentException if body is {@code null}.
     */
    public IotHubMessage(byte[] body)
    {
        // Codes_SRS_IOTHUBMESSAGE_11_017: [If the message body is null, the constructor shall throw an IllegalArgumentException.]
        if (body == null)
        {
            throw new IllegalArgumentException(
                    "Message body cannot be 'null'.");
        }

        // Codes_SRS_IOTHUBMESSAGE_11_007: [The constructor shall save the message body.]
        this.body = Arrays.copyOf(body, body.length);
        this.properties = new ArrayList<>();
    }

    /**
     * Sets the message property to the given value.
     *
     * @param name the message property name.
     * @param value the message property value.
     *
     * @return itself, for fluent setting.
     *
     * @throws IllegalArgumentException if name or value is {@code null}, or constitutes
     * an invalid IoT Hub message property. A message property can only contain
     * US-ASCII printable chars, with some exceptions as specified in RFC 2047.
     */
    public IotHubMessage setProperty(String name, String value)
    {
        // Codes_SRS_IOTHUBMESSAGE_11_028: [If name is null, the function shall throw an IllegalArgumentException.]
        if (name == null)
        {
            throw new IllegalArgumentException(
                    "IoT Hub message property name cannot be 'null'.");
        }
        // Codes_SRS_IOTHUBMESSAGE_11_029: [If value is null, the function shall throw an IllegalArgumentException.]
        if (value == null)
        {
            throw new IllegalArgumentException(
                    "IoT Hub message property cannot have value 'null'.");
        }

        // Codes_SRS_IOTHUBMESSAGE_11_030: [If name contains a character not specified in RFC 2047, the function shall throw an IllegalArgumentException.]
        // Codes_SRS_IOTHUBMESSAGE_11_031: [If value name contains a character not specified in RFC 2047, the function shall throw an IllegalArgumentException.]
        IotHubMessageProperty property = new IotHubMessageProperty(name, value);
        // Codes_SRS_IOTHUBMESSAGE_11_026: [The function shall set the message property to the given value.]
        this.properties.add(property);

        // Codes_SRS_IOTHUBMESSAGE_11_027: [The function shall return itself, for fluent setting.]
        return this;
    }

    /**
     * Returns the value associated with the message property.
     *
     * @param name the message property name. Both the HTTPS and AMQPS property
     * names can be used.
     *
     * @return the value associated with the message property.
     *
     * @throws IllegalArgumentException if the message property does not exist
     * for this message.
     */
    public String getProperty(String name)
    {
        for (IotHubMessageProperty property : this.properties)
        {
            // Codes_SRS_IOTHUBMESSAGE_11_032: [The function shall return the value associated with the message property name, where the name can be either the HTTPS or AMQPS property name.]
            if (property.hasSameName(name))
            {
                return property.getValue();
            }
        }

        // Codes_SRS_IOTHUBMESSAGE_11_034: [If no value associated with the property name is found, the function shall throw an IllegalArgumentException.]
        String errMsg = String.format("Could not find a value associated "
                + "with the IoT Hub message property '%s'.", name);
        throw new IllegalArgumentException(errMsg);
    }

    /**
     * Returns a copy of the message properties.
     *
     * @return a copy of the message properties.
     */
    public IotHubMessageProperty[] getProperties()
    {
        int propertiesSize = this.properties.size();
        IotHubMessageProperty[] propertiesCopy =
                new IotHubMessageProperty[propertiesSize];

        // Codes_SRS_IOTHUBMESSAGE_11_033: [The function shall return a copy of the message properties.]
        for (int i = 0; i < propertiesSize; ++i)
        {
            IotHubMessageProperty property = this.properties.get(i);
            IotHubMessageProperty propertyCopy =
                    new IotHubMessageProperty(property.getName(),
                            property.getValue());
            propertiesCopy[i] = propertyCopy;
        }

        return propertiesCopy;
    }

    /**
     * Getter for the message body.
     *
     * @return the message body.
     */
    public byte[] getBody()
    {
        // Codes_SRS_IOTHUBMESSAGE_11_002: [The function shall return the message body.]
        return Arrays.copyOf(this.body, this.body.length);
    }

    /**
     * Getter for the message body as a string. Assumes the message
     * body is a byte array encoded using UTF-8 encoding.
     *
     * @return the message body as a string.
     */
    public String getBodyAsString()
    {
        // Codes_SRS_IOTHUBMESSAGE_11_022: [The function shall return the message body, encoded using charset UTF-8.]
        return new String(this.body, IOTHUB_MESSAGE_DEFAULT_CHARSET);
    }

    protected IotHubMessage()
    {
        this.body = null;
    }
}
