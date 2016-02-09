// Copyright (c) Microsoft. All rights reserved.
// Licensed under the MIT license. See LICENSE file in the project root for full license information.

package com.microsoft.azure.iothub.net;

import java.io.UnsupportedEncodingException;
import java.net.URLEncoder;
import java.nio.charset.Charset;
import java.nio.charset.StandardCharsets;
import java.util.Iterator;
import java.util.Map;

/**
 * A URI for a device to connect to an IoT Hub.
 */
public final class IotHubUri
{
    /**
     * The device ID and specific IoT Hub method path will be interpolated into
     * the path.
     */
    public static final String PATH_FORMAT = "/devices/%s%s";

    /** The API version will be passed as a param in the URI. */
    public static final String API_VERSION = "api-version=2016-02-03";

    /** The charset used when URL-encoding the IoT Hub name and device ID. */
    public static final Charset IOTHUB_URL_ENCODING_CHARSET =
            StandardCharsets.UTF_8;

    /**
     * The IoT Hub resource URI is the hostname and path component that is
     * common to all IoT Hub communication methods between the given device and
     * IoT Hub.
     */
    protected String hostname;
    protected String path;
    protected String uri;

    /**
     * Constructor. Creates a URI to an IoT Hub method. The URI does not include
     * a protocol. The function will safely escape the given arguments.
     *
     * @param iotHubHostname the IoT Hub hostname.
     * @param deviceId the device ID.
     * @param iotHubMethodPath the path from the IoT Hub resource to the
     * method.
     * @param queryParams the URL query parameters. Can be null.
     */
    public IotHubUri(String iotHubHostname, String deviceId,
            String iotHubMethodPath, Map<String, String> queryParams)
    {
        this.hostname = iotHubHostname;

        String rawPath = String.format(
                PATH_FORMAT, deviceId, iotHubMethodPath);
        // Codes_SRS_IOTHUBURI_11_011: [The constructor shall URL-encode the device ID.]
        // Codes_SRS_IOTHUBURI_11_012: [The constructor shall URL-encode the IoT Hub method path.]
        this.path = urlEncodePath(rawPath);

        // Codes_SRS_IOTHUBURI_11_008: [If queryParams is not empty, the constructor shall return a URI pointing to the address '[iotHubHostname]/devices/[deviceId]/[IoT Hub method path]? api-version=2016-02-03 &[queryFragment] '.]
        // Codes_SRS_IOTHUBURI_11_009: [If the queryParams is empty, the constructor shall return a URI pointing to the address '[iotHubHostname]/devices/[deviceId]/[IoT Hub method path]?api-version=2016-02-03'.]
        StringBuilder uriBuilder = new StringBuilder(this.hostname);
        uriBuilder.append(this.path);
        uriBuilder.append("?");
        uriBuilder.append(API_VERSION);
        if (queryParams != null)
        {
            Iterator<Map.Entry<String, String>> paramIter =
                    queryParams.entrySet().iterator();
            while (paramIter.hasNext())
            {
                Map.Entry<String, String> param = paramIter.next();
                uriBuilder.append("&");
                // Codes_SRS_IOTHUBURI_11_013: [The constructor shall URL-encode each query param.]
                appendQueryParam(uriBuilder, param.getKey(),
                        param.getValue());
            }
        }

        this.uri = uriBuilder.toString();
    }

    /**
     * Constructor. Equivalent to {@code new IotHubUri(iotHubHostname, deviceId,
     * iotHubMethodPath, null)}.
     *
     * @param iotHubHostname the IoT Hub hostname.
     * @param deviceId the device ID.
     * @param iotHubMethodPath the path from the IoT Hub resource to the
     * method.
     */
    public IotHubUri(String iotHubHostname, String deviceId,
            String iotHubMethodPath)
    {
        // Codes_SRS_IOTHUBURI_11_007: [The constructor shall return a URI pointing to the address '[iotHubHostname] /devices/[deviceId]/[IoT Hub method path]?api-version=2016-02-03'.]
        // Codes_SRS_IOTHUBURI_11_015: [The constructor shall URL-encode the device ID.]
        // Codes_SRS_IOTHUBURI_11_016: [The constructor shall URL-encode the IoT Hub method path.]
        this(iotHubHostname, deviceId, iotHubMethodPath, null);
    }

    /**
     * Returns the string representation of the IoT Hub URI.
     *
     * @return the string representation of the IoT Hub URI.
     */
    @Override
    public String toString()
    {
        // Codes_SRS_IOTHUBURI_11_001: [The string representation of the IoT Hub URI shall be constructed with the format '[iotHubHostname]/devices/[deviceId]/[IoT Hub method path]?api-version=2016-02-03(&[queryFragment]) '.]
        return this.uri;
    }

    /**
     * Returns the string representation of the IoT Hub hostname.
     *
     * @return the string representation of the IoT Hub hostname.
     */
    public String getHostname()
    {
        // Codes_SRS_IOTHUBURI_11_005: [The function shall return the IoT hub hostname given in the constructor.]
        return this.hostname;
    }

    /**
     * Returns the string representation of the IoT Hub path.
     *
     * @return the string representation of the IoT Hub path.
     */
    public String getPath()
    {
        // Codes_SRS_IOTHUBURI_11_006: [The function shall return a URI with the format '/devices/[deviceId]/[IoT Hub method path]'.]
        return this.path;
    }

    /**
     * Returns the string representation of the IoT Hub resource URI. The IoT
     * Hub resource URI is the hostname and path component that is common to all
     * IoT Hub communication methods between the given device and IoT Hub.
     * Safely escapes the IoT Hub resource URI.
     *
     * @param iotHubHostname the IoT Hub hostname.
     * @param deviceId the device ID.
     *
     * @return the string representation of the IoT Hub resource URI.
     */
    public static String getResourceUri(String iotHubHostname, String deviceId)
    {
        // Codes_SRS_IOTHUBURI_11_002: [The function shall return a URI with the format '[iotHubHostname]/devices/[deviceId]'.]
        // Codes_SRS_IOTHUBURI_11_019: [The constructor shall URL-encode the device ID.]
        IotHubUri iotHubUri = new IotHubUri(iotHubHostname, deviceId, "");
        return iotHubUri.getHostname() + iotHubUri.getPath();
    }

    /**
     * URL-encodes each subdirectory in the path.
     *
     * @param path the path to be safely escaped.
     *
     * @return a path with each subdirectory URL-encoded.
     */
    protected static String urlEncodePath(String path)
    {
        String[] pathSubDirs = path.split("/");
        StringBuilder urlEncodedPathBuilder = new StringBuilder();
        try
        {
            for (String subDir : pathSubDirs)
            {
                if (subDir.length() > 0)
                {
                    String urlEncodedSubDir = URLEncoder.encode(
                            subDir, IOTHUB_URL_ENCODING_CHARSET.name());
                    urlEncodedPathBuilder.append("/");
                    urlEncodedPathBuilder.append(urlEncodedSubDir);
                }
            }
        }
        catch (UnsupportedEncodingException e)
        {
            // should never happen.
            throw new IllegalStateException(e);
        }

        return urlEncodedPathBuilder.toString();
    }

    /**
     * URL-encodes the query param {@code name} and {@code value} using charset UTF-8 and
     * appends them to the URI.
     *
     * @param uriBuilder the URI.
     * @param name the query param name.
     * @param value the query param value.
     */
    protected static void appendQueryParam(StringBuilder uriBuilder,
            String name, String value)
    {
        try
        {
            String urlEncodedName = URLEncoder.encode(name,
                    IOTHUB_URL_ENCODING_CHARSET.name());
            String urlEncodedValue = URLEncoder.encode(value,
                    IOTHUB_URL_ENCODING_CHARSET.name());
            uriBuilder.append(urlEncodedName);
            uriBuilder.append("=");
            uriBuilder.append(urlEncodedValue);
        }
        catch (UnsupportedEncodingException e)
        {
            // should never happen, since the encoding is hard-coded.
            throw new IllegalStateException(e);
        }
    }

    protected IotHubUri()
    {

    }
}
