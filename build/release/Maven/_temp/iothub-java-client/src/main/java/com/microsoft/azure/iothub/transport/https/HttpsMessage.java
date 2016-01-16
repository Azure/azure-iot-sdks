// Copyright (c) Microsoft. All rights reserved.
// Licensed under the MIT license. See LICENSE file in the project root for full license information.

package com.microsoft.azure.iothub.transport.https;

import com.microsoft.azure.iothub.MessageProperty;

/**
 * An HTTPS message. An HTTPS message is distinguished from a plain IoT Hub
 * message by its property names, which are prefixed with 'iothub-app-';
 * and by the explicit specification of a content-type.
 */
public interface HttpsMessage
{
    /** The prefix to be added to an HTTPS application-defined property. */
    String HTTPS_APP_PROPERTY_PREFIX = "iothub-app-";

    /**
     * Gets the message body.
     * @return Returns the message body.
     */
    byte[] getBody();

    /** Gets the message content type.
     * @return Returns the message content-type. */
    String getContentType();

    /**Gets the collection of message properties.
     * @return Returns the message properties. */
    MessageProperty[] getProperties();
}
