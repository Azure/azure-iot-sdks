// Copyright (c) Microsoft. All rights reserved.
// Licensed under the MIT license. See LICENSE file in the project root for full license information.

package com.microsoft.azure.iothub;

import javax.xml.bind.DatatypeConverter;
import javax.naming.SizeLimitExceededException;

/**
 * A message to an IoT Hub. A message body has a maximum size of 255 kb.
 */
public final class IotHubServiceboundMessage
{
    /** The maximum size, in bytes, of an IoT Hub service-bound message body. */
    public static final int SERVICEBOUND_MESSAGE_MAX_SIZE_BYTES = 255 * 1024;

    /** The underlying IoT Hub message. */
    protected final IotHubMessage msg;
    /** Whether the message body has been Base64-encoded. */
    protected final boolean base64Encoded;

    /**
     * Constructor. The raw message body will be Base64-encoded because some
     * bytes are unsafe to use with the IoT Hub HTTPS batch message format.
     *
     * @param body the raw message body.
     *
     * @throws IllegalArgumentException if body is null.
     * @throws SizeLimitExceededException if body has a size of more than 255
     * kb.
     */
    public IotHubServiceboundMessage(byte[] body)
            throws SizeLimitExceededException
    {
        // Codes_SRS_IOTHUBSERVICEBOUNDMESSAGE_11_017: [If body is null, the constructor shall throw an IllegalArgumentException.]
        if (body == null)
        {
            throw new IllegalArgumentException(
                    "Message body cannot be 'null'.");
        }
        // Codes_SRS_IOTHUBSERVICEBOUNDMESSAGE_11_001: [The constructor shall Base64-encode the message.]
        byte[] base64EncodedBody = DatatypeConverter.printBase64Binary(body)
                .getBytes(IotHubMessage.IOTHUB_MESSAGE_DEFAULT_CHARSET);
        // Codes_SRS_IOTHUBSERVICEBOUNDMESSAGE_11_007: [If the body length is greater than 255 kb after Base64-encoding it, the constructor shall throw a SizeLimitExceededException.]
        if (base64EncodedBody.length > SERVICEBOUND_MESSAGE_MAX_SIZE_BYTES)
        {
            String errMsg = String.format("Service-bound message body "
                            + "cannot exceed %d bytes after being "
                            + "Base64-encoded.\n",
                    SERVICEBOUND_MESSAGE_MAX_SIZE_BYTES);
            throw new SizeLimitExceededException(errMsg);
        }

        // Codes_SRS_IOTHUBSERVICEBOUNDMESSAGE_11_006: [The constructor shall save the content-type as 'application/octet-stream'.]
        this.msg = new IotHubMessage(base64EncodedBody);
        this.base64Encoded = true;
    }

    /**
     * Constructor. The message body will be encoded using charset UTF-8. The
     * message content-type will be text/plain.
     *
     * @param body the message body.
     *
     * @throws IllegalArgumentException if body is null.
     * @throws SizeLimitExceededException if body has a size of more than 255
     * bytes.
     */
    public IotHubServiceboundMessage(String body)
            throws SizeLimitExceededException
    {
        // Codes_SRS_IOTHUBSERVICEBOUNDMESSAGE_11_019: [If body is null, the constructor shall throw an IllegalArgumentException.]
        if (body == null)
        {
            throw new IllegalArgumentException(
                    "Message body cannot be 'null'.");
        }
        // Codes_SRS_IOTHUBSERVICEBOUNDMESSAGE_11_008: [The constructor shall save the message as a byte array using charset UTF-8.]
        byte[] bodyBytes =
                body.getBytes(IotHubMessage.IOTHUB_MESSAGE_DEFAULT_CHARSET);
        // Codes_SRS_IOTHUBSERVICEBOUNDMESSAGE_11_010: [If the body byte array length is greater than 255 kb, the constructor shall throw a SizeLimitExceededException.]
        if (bodyBytes.length > SERVICEBOUND_MESSAGE_MAX_SIZE_BYTES)
        {
            String errMsg = String.format("Service-bound message body "
                            + "cannot exceed %d bytes.\n",
                    SERVICEBOUND_MESSAGE_MAX_SIZE_BYTES);
            throw new SizeLimitExceededException(errMsg);
        }

        // Codes_SRS_IOTHUBSERVICEBOUNDMESSAGE_11_009: [The constructor shall save the content-type as 'text/plain'.]
        this.msg = new IotHubMessage(bodyBytes);
        this.base64Encoded = false;
    }

    /**
     * Sets the message property to the given value.
     *
     * @param name the message property name.
     * @param value the message property value.
     *
     * @return itself, for fluent setting.
     *
     * @throws IllegalArgumentException if name or value is {@code null}, or name is an
     * invalid IoT Hub message property name.
     */
    public IotHubServiceboundMessage setProperty(String name, String value)
    {
        // Codes_SRS_IOTHUBSERVICEBOUNDMESSAGE_11_023: [The function shall set the message property to the given value.]
        // Codes_SRS_IOTHUBSERVICEBOUNDMESSAGE_11_024: [The function shall return itself, for fluent setting.]
        // Codes_SRS_IOTHUBSERVICEBOUNDMESSAGE_11_025: [If name is null, the function shall throw an IllegalArgumentException.]
        // Codes_SRS_IOTHUBSERVICEBOUNDMESSAGE_11_026: [If value is null, the function shall throw an IllegalArgumentException.]
        // Codes_SRS_IOTHUBSERVICEBOUNDMESSAGE_11_027: [If name contains a character not specified in RFC 2047, the function shall throw an IllegalArgumentException.]
        // Codes_SRS_IOTHUBSERVICEBOUNDMESSAGE_11_028: [If value name contains a character not specified in RFC 2047, the function shall throw an IllegalArgumentException.]
        this.msg.setProperty(name, value);

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
        // Codes_SRS_IOTHUBSERVICEBOUNDMESSAGE_11_029: [The function shall return the value associated with the message property name, where the name can be either the HTTPS or AMQPS property name.]
        // Codes_SRS_IOTHUBSERVICEBOUNDMESSAGE_11_030: [If no value associated with the property name is found, the function shall throw an IllegalArgumentException.]
        return this.msg.getProperty(name);
    }

    /**
     * Returns a copy of the message properties.
     *
     * @return a copy of the message properties.
     */
    public IotHubMessageProperty[] getProperties()
    {
        // Codes_SRS_IOTHUBSERVICEBOUNDMESSAGE_11_031: [The function shall return a copy of the message properties.]
        return this.msg.getProperties();
    }

    /**
     * Getter for the message body.
     *
     * @return the message body.
     */
    public byte[] getBody()
    {
        // Codes_SRS_IOTHUBSERVICEBOUNDMESSAGE_11_002: [If base64Encoded is set to true, the function shall return the Base64-encoded message.]
        // Codes_SRS_IOTHUBSERVICEBOUNDMESSAGE_11_003: [If base64Encoded is set to false, the function shall return the raw message.]
        return this.msg.getBody();
    }

    /**
     * Getter for the message body as a string. Uses UTF-8 to decode the message
     * body as a string.
     *
     * @return the message body as a string.
     */
    public String getBodyAsString()
    {
        // Codes_SRS_IOTHUBSERVICEBOUNDMESSAGE_11_021: [If base64Encoded is set to true, the function shall return the Base64-encoded message, using charset UTF-8.]
        // Codes_SRS_IOTHUBSERVICEBOUNDMESSAGE_11_022: [If base64Encoded is set to false, the function shall return the raw message, using charset UTF-8.]
        return this.msg.getBodyAsString();
    }

    /**
     * Getter for whether the raw message has been Base64-encoded.
     *
     * @return whether the raw message has been Base64-encoded.
     */
    public boolean isBase64Encoded()
    {
        // Codes_SRS_IOTHUBSERVICEBOUNDMESSAGE_11_005: [The function shall return whether getBody() will return a Base64-encoded message.]
        return this.base64Encoded;
    }

    protected IotHubServiceboundMessage()
    {
        this.msg = null;
        this.base64Encoded = false;
    }
}
