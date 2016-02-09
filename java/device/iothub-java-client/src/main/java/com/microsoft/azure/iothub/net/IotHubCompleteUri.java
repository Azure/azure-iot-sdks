// Copyright (c) Microsoft. All rights reserved.
// Licensed under the MIT license. See LICENSE file in the project root for full license information.

package com.microsoft.azure.iothub.net;

/**
 * A URI for a device to instruct an IoT Hub to mark a message as being
 * completed.
 */
public final class IotHubCompleteUri
{
    /**
     * The path to be appended to an IoT Hub URI. The %s will be replaced by the
     * message etag.
     */
    public static final String COMPLETE_PATH_FORMAT =
            "/messages/devicebound/%s";

    /** The underlying IoT Hub URI. */
    protected final IotHubUri uri;

    /**
     * Constructor. Returns a URI for a device to instruct an IoT Hub to mark a
     * message as being completed. The URI does not include a protocol.
     *
     * @param iotHubHostname the IoT Hub name.
     * @param deviceId the device ID.
     * @param eTag the message e-tag.
     */
    public IotHubCompleteUri(String iotHubHostname, String deviceId, String eTag)
    {
        // Codes_SRS_IOTHUBCOMPLETEURI_11_001: [The constructor returns a URI with the format "[iotHubHostname]/devices/[deviceId]/messages/devicebound/[eTag]?api-version=2016-02-03".]
        String completePath = String.format(COMPLETE_PATH_FORMAT, eTag);
        this.uri = new IotHubUri(iotHubHostname, deviceId, completePath);
    }

    /**
     * Returns the string representation of the IoT Hub complete message URI.
     *
     * @return the string representation of the IoT Hub complete message URI.
     */
    @Override
    public String toString()
    {
        // Codes_SRS_IOTHUBCOMPLETEURI_11_002: [The string representation of the IoT Hub event URI shall be constructed with the format "[iotHubHostname]/devices/[deviceId]/messages/devicebound/[eTag]?api-version=2016-02-03".]
        return this.uri.toString();
    }

    /**
     * Returns the string representation of the IoT Hub hostname.
     *
     * @return the string representation of the IoT Hub hostname.
     */
    public String getHostname()
    {
        // Codes_SRS_IOTHUBCOMPLETEURI_11_003: [The function shall return the hostname given in the constructor.]
        return this.uri.getHostname();
    }

    /**
     * Returns the string representation of the IoT Hub path.
     *
     * @return the string representation of the IoT Hub path.
     */
    public String getPath()
    {
        // Codes_SRS_IOTHUBCOMPLETEURI_11_004: [The function shall return a URI with the format '/devices/[deviceId]/messages/devicebound/[eTag].]
        return this.uri.getPath();
    }

    protected IotHubCompleteUri()
    {
        this.uri = null;
    }
}
