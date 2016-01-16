// Copyright (c) Microsoft. All rights reserved.
// Licensed under the MIT license. See LICENSE file in the project root for full license information.

package com.microsoft.azure.iothub.transport.https;

import com.microsoft.azure.iothub.Message;
import com.microsoft.azure.iothub.MessageProperty;

import java.util.ArrayList;
import java.util.Arrays;
import java.util.Map;

/** A single HTTPS message. */
public final class HttpsSingleMessage implements HttpsMessage
{
    public static final String HTTPS_SINGLE_MESSAGE_CONTENT_TYPE =
            "binary/octet-stream";

    protected byte[] body;
    protected boolean base64Encoded;
    protected MessageProperty[] properties;

    /**
     * Returns the HTTPS message represented by the service-bound message.
     *
     * @param message the service-bound message to be mapped to its HTTPS message
     * equivalent.
     *
     * @return the HTTPS message represented by the service-bound message.
     */
    public static HttpsSingleMessage parseHttpsMessage(Message message) {
        HttpsSingleMessage httpsMsg = new HttpsSingleMessage();

        // Codes_SRS_HTTPSSINGLEMESSAGE_11_001: [The parsed HttpsSingleMessage shall have a copy of the original message body as its body.]
        byte[] msgBody = message.getBytes();
        httpsMsg.body = Arrays.copyOf(msgBody, msgBody.length);

        // Codes_SRS_HTTPSSINGLEMESSAGE_11_003: [The parsed HttpsSingleMessage shall add the prefix 'iothub-app-' to each of the message properties.]
        MessageProperty[] msgProperties = message.getProperties();
        httpsMsg.properties = new MessageProperty[msgProperties.length];
        for (int i = 0; i < msgProperties.length; ++i)
        {
            MessageProperty property = msgProperties[i];

            httpsMsg.properties[i] = new MessageProperty(
                    HTTPS_APP_PROPERTY_PREFIX + property.getName(),
                    property.getValue());
        }

        return httpsMsg;
    }


    /**
     * Returns the HTTPS message represented by the HTTPS response.
     *
     * @param response the HTTPS response.
     *
     * @return the HTTPS message represented by the HTTPS response.
     */
    public static HttpsSingleMessage parseHttpsMessage(HttpsResponse response) {
        HttpsSingleMessage msg = new HttpsSingleMessage();
        // Codes_SRS_HTTPSSINGLEMESSAGE_11_004: [The parsed HttpsSingleMessage shall have a copy of the original response body as its body.]
        byte[] responseBody = response.getBody();
        msg.body = Arrays.copyOf(responseBody, responseBody.length);

        // Codes_SRS_HTTPSSINGLEMESSAGE_11_006: [The parsed HttpsSingleMessage shall include all valid HTTPS application-defined properties in the response header as message properties.]
        ArrayList<MessageProperty> properties = new ArrayList<>();
        Map<String, String> headerFields = response.getHeaderFields();
        for (Map.Entry<String, String> field : headerFields.entrySet())
        {
            String propertyName = field.getKey();
            String propertyValue = field.getValue();
            if (isValidHttpsAppProperty(propertyName, propertyValue))
            {
                properties.add(new MessageProperty(propertyName,
                        propertyValue));
            }
        }
        msg.properties = new MessageProperty[properties.size()];
        msg.properties = properties.toArray(msg.properties);

        return msg;
    }

    /**
     * Returns the Iot Hub message represented by the HTTPS message.
     *
     * @return the IoT Hub message represented by the HTTPS message.
     */
    public Message toMessage()
    {
        // Codes_SRS_HTTPSSINGLEMESSAGE_11_007: [The function shall return an IoT Hub message with a copy of the message body as its body.]
        Message msg = new Message(this.getBody());
        // Codes_SRS_HTTPSSINGLEMESSAGE_11_008: [The function shall return an IoT Hub message with application-defined properties that have the prefix 'iothub-app' removed.]
        for (MessageProperty property : this.properties)
        {
            String propertyName =
                    httpsAppPropertyToAppProperty(property.getName());
            msg.setProperty(propertyName, property.getValue());
        }

        return msg;
    }

    /**
     * Returns a copy of the message body.
     *
     * @return a copy of the message body.
     */
    public byte[] getBody()
    {
        // Codes_SRS_HTTPSSINGLEMESSAGE_11_009: [The function shall return a copy of the message body.]
        return Arrays.copyOf(this.body, this.body.length);
    }

    /**
     * Returns the message body as a string. The body is encoded using charset
     * UTF-8.
     *
     * @return the message body as a string.
     */
    public String getBodyAsString() {
        // Codes_SRS_HTTPSSINGLEMESSAGE_11_010: [The function shall return the message body as a string encoded using charset UTF-8.]
        return new String(this.body, Message.DEFAULT_IOTHUB_MESSAGE_CHARSET);
    }

    /**
     * Returns the message content-type as 'binary/octet-stream'.
     *
     * @return the message content-type as 'binary/octet-stream'.
     */
    public String getContentType()
    {
        // Codes_SRS_HTTPSSINGLEMESSAGE_11_011: [The function shall return the message content-type as 'binary/octet-stream'.]
        return HTTPS_SINGLE_MESSAGE_CONTENT_TYPE;
    }

    /**
     * Returns whether the message is Base64-encoded.
     *
     * @return whether the message is Base64-encoded.
     */
    public boolean isBase64Encoded()
    {
        // Codes_SRS_HTTPSSINGLEMESSAGE_11_012: [The function shall return whether the message is Base64-encoded.]
        return this.base64Encoded;
    }

    /**
     * Returns a copy of the message properties.
     *
     * @return a copy of the message properties.
     */
    public MessageProperty[] getProperties()
    {
        // Codes_SRS_HTTPSSINGLEMESSAGE_11_013: [The function shall return a copy of the message properties.]
        int propertiesSize = this.properties.length;
        MessageProperty[] propertiesCopy =
                new MessageProperty[propertiesSize];

        for (int i = 0; i < propertiesSize; ++i)
        {
            MessageProperty property = this.properties[i];
            MessageProperty propertyCopy =
                    new MessageProperty(property.getName(),
                            property.getValue());
            propertiesCopy[i] = propertyCopy;
        }

        return propertiesCopy;
    }

    /**
     * Returns whether the property name and value constitute a valid HTTPS
     * application property. The property is valid if it is a valid application
     * property and its name begins with 'iothub-app-'.
     *
     * @param name the property name.
     * @param value the property value.
     *
     * @return whether the property is a valid HTTPS property.
     */
    protected static boolean isValidHttpsAppProperty(String name, String value)
    {
        String lowercaseName = name.toLowerCase();
        if (MessageProperty.isValidAppProperty(name, value)
                && lowercaseName.startsWith(HTTPS_APP_PROPERTY_PREFIX))
        {
            return true;
        }

        return false;
    }

    /**
     * Returns an application-defined property name with the prefix 'iothub-app'
     * removed. If the prefix is not present, the property name is left
     * untouched.
     *
     * @param httpsAppProperty the HTTPS property name.
     *
     * @return the property name with the prefix 'iothub-app' removed.
     */
    protected static String httpsAppPropertyToAppProperty(
            String httpsAppProperty)
    {
        String canonicalizedProperty = httpsAppProperty.toLowerCase();
        if (canonicalizedProperty.startsWith(HTTPS_APP_PROPERTY_PREFIX))
        {
            return canonicalizedProperty
                    .substring(HTTPS_APP_PROPERTY_PREFIX.length());
        }

        return canonicalizedProperty;
    }

    protected HttpsSingleMessage()
    {
    }
}
