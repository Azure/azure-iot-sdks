// Copyright (c) Microsoft. All rights reserved.
// Licensed under the MIT license. See LICENSE file in the project root for full license information.

package com.microsoft.azure.iothub.net;

import java.util.Collections;
import java.util.HashMap;
import java.util.Map;

/**
 * A URI for a device to instruct an IoT Hub to mark a message as being
 * rejected.
 */
public final class IotHubRejectUri
{
    /**
     * The path to be appended to an IoT Hub URI. The %s will be replaced by the
     * message etag.
     */
    public static final String REJECT_PATH_FORMAT = "/messages/devicebound/%s";

    /**
     * The reject URI query parameter.
     */
    public static final Map<String, String> REJECT_QUERY_PARAM;
    static
    {
        HashMap<String, String> rejectQueryParam = new HashMap<>();
        rejectQueryParam.put("?reject", "true");
        REJECT_QUERY_PARAM = Collections.unmodifiableMap(rejectQueryParam);
    }

    /** The underlying IoT Hub URI. */
    protected final IotHubUri uri;

    /**
     * Constructor. Returns a URI for a device to instruct an IoT Hub to mark a
     * message as being rejected. The URI does not include a protocol.
     *
     * @param iotHubHostname the IoT Hub name.
     * @param deviceId the device ID.
     * @param eTag the message e-tag.
     */
    public IotHubRejectUri(String iotHubHostname, String deviceId, String eTag)
    {
        // Codes_SRS_IOTHUBREJECTURI_11_001: [The constructor returns a URI with the format "[iotHubHostname]/devices/[deviceId]/messages/devicebound/[eTag]??reject=true&api-version=2016-02-03" (the query parameters can be in any order).]
        String rejectPath = String.format(REJECT_PATH_FORMAT, eTag);

        this.uri = new IotHubUri(iotHubHostname, deviceId, rejectPath,
                        REJECT_QUERY_PARAM);
    }

    /**
     * Returns the string representation of the IoT Hub reject message URI.
     *
     * @return the string representation of the IoT Hub reject mesage URI.
     */
    @Override
    public String toString()
    {
        // Codes_SRS_IOTHUBREJECTURI_11_002: [The string representation of the IoT Hub event URI shall be constructed with the format "[iotHubHostname]/devices/[deviceId]/messages/devicebound/[eTag]??reject=true&api-version=2016-02-03" (the query parameters can be in any order).]
        return this.uri.toString();
    }

    /**
     * Returns the string representation of the IoT Hub hostname.
     *
     * @return the string representation of the IoT Hub hostname.
     */
    public String getHostname()
    {
        // Codes_SRS_IOTHUBREJECTURI_11_003: [The function shall return the hostname given in the constructor.]
        return this.uri.getHostname();
    }

    /**
     * Returns the string representation of the IoT Hub path.
     *
     * @return the string representation of the IoT Hub path.
     */
    public String getPath()
    {
        // Codes_SRS_IOTHUBREJECTURI_11_004: [The function shall return a URI with the format '/devices/[deviceId]/messages/devicebound/[eTag].]
        return this.uri.getPath();
    }

    protected IotHubRejectUri()
    {
        this.uri = null;
    }
}
