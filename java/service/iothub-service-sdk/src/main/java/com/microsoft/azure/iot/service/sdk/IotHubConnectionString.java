/*
 * Copyright (c) Microsoft. All rights reserved.
 * Licensed under the MIT license. See LICENSE file in the project root for full license information.
 */

package com.microsoft.azure.iot.service.sdk;

import java.net.MalformedURLException;
import java.net.URL;

public class IotHubConnectionString extends IotHubConnectionStringBuilder
{
    protected static final String VALUE_PAIR_DELIMITER = ";";
    protected static final String VALUE_PAIR_SEPARATOR = "=";
    protected static final String HOST_NAME_SEPARATOR = ".";

    protected static final String HOST_NAME_PROPERTY_NAME = "HostName";
    protected static final String SHARED_ACCESS_KEY_NAME_PROPERTY_NAME = "SharedAccessKeyName";
    protected static final String SHARED_ACCESS_KEY_PROPERTY_NAME = "SharedAccessKey";
    protected static final String SHARED_ACCESS_SIGNATURE_PROPERTY_NAME = "SharedAccessSignature";

    // Included in the device connection string
    protected String hostName;
    protected String iotHubName;
    protected AuthenticationMethod authenticationMethod;
    protected String sharedAccessKeyName;
    protected String sharedAccessKey;
    protected String sharedAccessSignature;

    // Connection
    private static final String URL_SEPARATOR_0 = "/";
    private static final String URL_SEPARATOR_1 = "?";
    private static final String URL_SEPARATOR_2 = "&";
    private static final String URL_HTTPS = "https:" + URL_SEPARATOR_0 + URL_SEPARATOR_0;
    private static final String URL_PATH_DEVICES = "devices";
    private static final String URL_API_VERSION = "api-version=2015-08-15-preview";
    private static final String URL_MAX_COUNT = "maxCount=";
    private static final String URL_PATH_DEVICESTATISTICS = "statistics";
    private static final String USER_SEPARATOR = "@";
    private static final String USER_SAS = "sas.";
    private static final String USER_ROOT = "root.";

    protected IotHubConnectionString() {}

    public String getUserString()
    {
        StringBuilder stringBuilder = new StringBuilder();
        stringBuilder.append(this.sharedAccessKeyName);
        stringBuilder.append(USER_SEPARATOR);
        stringBuilder.append(USER_SAS);
        stringBuilder.append(USER_ROOT);
        stringBuilder.append(this.iotHubName);
        return stringBuilder.toString();
    }

    public URL getUrlDevice(String deviceId) throws MalformedURLException
    {
        if (!Tools.isNullOrEmpty(deviceId))
        {
            StringBuilder stringBuilder = new StringBuilder();
            stringBuilder.append(URL_HTTPS);
            stringBuilder.append(hostName);
            stringBuilder.append(URL_SEPARATOR_0);
            stringBuilder.append(URL_PATH_DEVICES);
            stringBuilder.append(URL_SEPARATOR_0);
            stringBuilder.append(deviceId);
            stringBuilder.append(URL_SEPARATOR_1);
            stringBuilder.append(URL_API_VERSION);
            return new URL(stringBuilder.toString());
        }
        else
        {
            return null;
        }
    }

    public URL getUrlDeviceList(Integer maxCount) throws MalformedURLException
    {
        StringBuilder stringBuilder = new StringBuilder();
        stringBuilder.append(URL_HTTPS);
        stringBuilder.append(hostName);
        stringBuilder.append(URL_SEPARATOR_0);
        stringBuilder.append(URL_PATH_DEVICES);
        stringBuilder.append(URL_SEPARATOR_0);
        stringBuilder.append(URL_SEPARATOR_1);
        stringBuilder.append(URL_MAX_COUNT);
        stringBuilder.append(maxCount.toString());
        stringBuilder.append(URL_SEPARATOR_2);
        stringBuilder.append(URL_API_VERSION);
        return new URL(stringBuilder.toString());
    }

    public URL getUrlDeviceStatistics() throws MalformedURLException
    {
        StringBuilder stringBuilder = new StringBuilder();
        stringBuilder.append(URL_HTTPS);
        stringBuilder.append(hostName);
        stringBuilder.append(URL_SEPARATOR_0);
        stringBuilder.append(URL_PATH_DEVICESTATISTICS);
        stringBuilder.append(URL_SEPARATOR_0);
        stringBuilder.append(URL_PATH_DEVICES);
        stringBuilder.append(URL_SEPARATOR_1);
        stringBuilder.append(URL_API_VERSION);
        return new URL(stringBuilder.toString());
    }

    public String getIotHubName()
    {
        return this.iotHubName;
    }

    public AuthenticationMethod getAuthenticationMethod()
    {
        return this.authenticationMethod;
    }

    public String getSharedAccessKeyName()
    {
        return this.sharedAccessKeyName;
    }

    public String getSharedAccessKey()
    {
        return this.sharedAccessKey;
    }

    public String getSharedAccessSignature()
    {
        return this.sharedAccessSignature;
    }

    public String getHostName()
    {
        return this.hostName;
    }

    protected void setSharedAccessKeyName(String sharedAccessKeyName)
    {
        this.sharedAccessKeyName = sharedAccessKeyName;
    }

    protected void setSharedAccessKey(String sharedAccessKey)
    {
        this.sharedAccessKey = sharedAccessKey;
    }

    protected void setSharedAccessSignature(String sharedAccessSignature)
    {
        this.sharedAccessSignature = sharedAccessSignature;
    }

    @Override
    public String toString()
    {
        StringBuilder stringBuilder = new StringBuilder();
        stringBuilder.append(HOST_NAME_PROPERTY_NAME);
        stringBuilder.append(VALUE_PAIR_SEPARATOR);
        stringBuilder.append(this.hostName);
        stringBuilder.append(VALUE_PAIR_DELIMITER);

        stringBuilder.append(SHARED_ACCESS_KEY_NAME_PROPERTY_NAME);
        stringBuilder.append(VALUE_PAIR_SEPARATOR);
        stringBuilder.append(this.sharedAccessKeyName);
        stringBuilder.append(VALUE_PAIR_DELIMITER);

        stringBuilder.append(SHARED_ACCESS_KEY_PROPERTY_NAME);
        stringBuilder.append(VALUE_PAIR_SEPARATOR);
        stringBuilder.append(this.sharedAccessKey);
        stringBuilder.append(VALUE_PAIR_DELIMITER);

        stringBuilder.append(SHARED_ACCESS_SIGNATURE_PROPERTY_NAME);
        stringBuilder.append(VALUE_PAIR_SEPARATOR);
        stringBuilder.append(this.sharedAccessSignature);

        return stringBuilder.toString();
    }
}
