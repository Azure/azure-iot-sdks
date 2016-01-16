// Copyright (c) Microsoft. All rights reserved.
// Licensed under the MIT license. See LICENSE file in the project root for full license information.

package com.microsoft.azure.iothub.transport.https;

import com.microsoft.azure.iothub.Message;
import com.microsoft.azure.iothub.MessageProperty;

import java.nio.charset.Charset;
import java.nio.charset.StandardCharsets;

import javax.naming.SizeLimitExceededException;

/**
 * Builds a batched IoT Hub request body as a JSON array. The batched message
 * has a maximum size of 256 kb.
 */
public final class HttpsBatchMessage implements HttpsMessage
{
    // Note: this limit is defined by the IoT Hub.
    public static final int SERVICEBOUND_MESSAGE_MAX_SIZE_BYTES = 255 * 1024 - 1;

    /**
     * The value for the "content-type" header field in a batched HTTPS
     * request.
     */
    public static String HTTPS_BATCH_CONTENT_TYPE = "application/vnd.microsoft.iothub.json";

    /**
     * The charset used to encode IoT Hub messages. The server will interpret
     * the JSON array using UTF-8 by default according to RFC4627.
     */
    public static Charset BATCH_CHARSET = StandardCharsets.UTF_8;

    /** The current batched message body. */
    protected String batchBody;
    /** The current number of messages in the batch. */
    protected int numMsgs;

    /** Constructor. Initializes the batch body as an empty JSON array. */
    public HttpsBatchMessage()
    {
        // Codes_SRS_HTTPSBATCHMESSAGE_11_001: [The constructor shall initialize the batch message with the body as an empty JSON array.]
        this.batchBody = "[]";
        this.numMsgs = 0;
    }

    /**
     * Adds a message to the batch.
     *
     * @param msg the message to be added.
     *
     * @throws SizeLimitExceededException if adding the message causes the
     * batched message to exceed 256 kb in size. The batched message will remain
     * as if the message was never added.
     */
    public void addMessage(HttpsSingleMessage msg)
            throws SizeLimitExceededException
    {
        String jsonMsg = msgToJson(msg);
        // Codes_SRS_HTTPSBATCHMESSAGE_11_002: [The function shall add the message as a JSON object appended to the current JSON array.]
        String newBatchBody = addJsonObjToArray(jsonMsg, this.batchBody);

        // Codes_SRS_HTTPSBATCHMESSAGE_11_008: [If adding the message causes the batched message to exceed 256 kb in size, the function shall throw a SizeLimitExceededException.]
        // Codes_SRS_HTTPSBATCHMESSAGE_11_009: [If the function throws a SizeLimitExceedException, the batched message shall remain as if the message was never added.]
        byte[] newBatchBodyBytes = newBatchBody.getBytes(BATCH_CHARSET);

        if (newBatchBodyBytes.length > SERVICEBOUND_MESSAGE_MAX_SIZE_BYTES) {
            String errMsg = String.format("Service-bound message size (%d bytes) cannot exceed %d bytes.\n",
                    newBatchBodyBytes.length, SERVICEBOUND_MESSAGE_MAX_SIZE_BYTES);
            throw new SizeLimitExceededException(errMsg);
        }

        this.batchBody = newBatchBody;
        this.numMsgs++;
    }

    /**
     * Returns the current batch body as a UTF-8 encoded byte array.
     *
     * @return the current batch body as a UTF-8 encoded byte array.
     */
    public byte[] getBody()
    {
        // Codes_SRS_HTTPSBATCHMESSAGE_11_006: [The function shall return the current batch message body.]
        // Codes_SRS_HTTPSBATCHMESSAGE_11_007: [The batch message body shall be encoded using UTF-8.]
        return this.batchBody.getBytes(BATCH_CHARSET);
    }

    /**
     * Returns the message content-type as 'application/vnd.microsoft.iothub.json'.
     *
     * @return the message content-type as 'application/vnd.microsoft.iothub.json'.
     */
    public String getContentType()
    {
        // Codes_SRS_HTTPSBATCHMESSAGE_11_011: [The function shall return 'application/vnd.microsoft.iothub.json'.]
        return HTTPS_BATCH_CONTENT_TYPE;
    }

    /**
     * Returns an empty list of properties for the batched message.
     *
     * @return an empty list of properties for the batched message.
     */
    public MessageProperty[] getProperties()
    {
        // Codes_SRS_HTTPSBATCHMESSAGE_11_012: [The function shall return an empty array.]
        return new MessageProperty[0];
    }

    /**
     * Returns the number of messages currently in the batch.
     *
     * @return the number of messages currently in the batch.
     */
    public int numMessages()
    {
        // Codes_SRS_HTTPSBATCHMESSAGE_11_010: [The function shall return the number of messages currently in the batch.]
        return this.numMsgs;
    }

    /**
     * Converts a service-bound message to a JSON object with the correct
     * format.
     *
     * @param msg the message to be converted to a corresponding JSON object.
     *
     * @return the JSON string representation of the message.
     */
    protected static String msgToJson(HttpsSingleMessage msg)
    {
        StringBuilder jsonMsg = new StringBuilder("{");
        // Codes_SRS_HTTPSBATCHMESSAGE_11_003: [The JSON object shall have the field "body" set to the raw message.]
        jsonMsg.append("\"body\":");
        jsonMsg.append("\"" + msg.getBodyAsString() + "\",");
        // Codes_SRS_HTTPSBATCHMESSAGE_11_004: [The JSON object shall have the field "base64Encoded" set to whether the raw message was Base64-encoded.]
        jsonMsg.append("\"base64Encoded\":");
        jsonMsg.append(Boolean.toString(msg.isBase64Encoded()));
        // Codes_SRS_HTTPSBATCHMESSAGE_11_005: [The JSON object shall have the field "properties" set to a JSON object which has the field "content-type" set to the content type of the raw message.]
        MessageProperty[] properties = msg.getProperties();
        int numProperties = properties.length;
        if (numProperties > 0)
        {
            jsonMsg.append(",");
            jsonMsg.append("\"properties\":");
            jsonMsg.append("{");
            for (int i = 0; i < numProperties - 1; ++i)
            {
                MessageProperty property = properties[i];
                jsonMsg.append("\"" + property.getName() + "\":");
                jsonMsg.append("\"" + property.getValue() + "\",");
            }
            if (numProperties > 0)
            {
                MessageProperty property = properties[numProperties - 1];
                jsonMsg.append("\"" + property.getName() + "\":");
                jsonMsg.append("\"" + property.getValue() + "\"");
            }
            jsonMsg.append("}");
        }
        jsonMsg.append("}");

        return jsonMsg.toString();
    }

    /**
     * Adds a JSON object to a JSON array.
     *
     * @param jsonObj the object to be added to the JSON array.
     * @param jsonArray the JSON array.
     *
     * @return the JSON string representation of the JSON array with the object
     * added.
     */
    protected static String addJsonObjToArray(String jsonObj, String jsonArray)
    {
        if (jsonArray.equals("[]"))
        {
            return "[" + jsonObj + "]";
        }

        // removes the closing brace of the JSON array.
        String openJsonArray = jsonArray.substring(0, jsonArray.length() - 1);

        return openJsonArray + "," + jsonObj + "]";
    }
}
