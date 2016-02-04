/*
 * Copyright (c) Microsoft. All rights reserved.
 * Licensed under the MIT license. See LICENSE file in the project root for full license information.
 */

package com.microsoft.azure.iot.service.sdk;

import java.net.MalformedURLException;
import java.net.URL;

/**
 * Expose all connections string properties and methods
 * for user, device and connection string serialization.
 */
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
    private static final String URL_API_VERSION = "api-version=2016-02-03";
    private static final String URL_MAX_COUNT = "top=";
    private static final String URL_PATH_DEVICESTATISTICS = "statistics";
    private static final String USER_SEPARATOR = "@";
    private static final String USER_SAS = "sas.";
    private static final String USER_ROOT = "root.";

    protected IotHubConnectionString() {}

    /**
     * Serialize user string
     *
     * @return The user string in the following format: "SharedAccessKeyName@sas.root.IotHubName"
     */
    public String getUserString()
    {
        // Codes_SRS_SERVICE_SDK_JAVA_IOTHUBCONNECTIONSTRING_12_001: [The function shall serialize the object
        // properties to a string using the following format: SharedAccessKeyName@sas.root.IotHubName]
        StringBuilder stringBuilder = new StringBuilder();
        stringBuilder.append(this.sharedAccessKeyName);
        stringBuilder.append(USER_SEPARATOR);
        stringBuilder.append(USER_SAS);
        stringBuilder.append(USER_ROOT);
        stringBuilder.append(this.iotHubName);
        return stringBuilder.toString();
    }

    /**
     * Create url for requesting device data
     *
     * @param deviceId The name of the device
     * @return The device Url in the following format: "https:hostname/devices/deviceId?api-version=201X-XX-XX"
     * @throws MalformedURLException This exception is thrown if the URL creation failed due to malformed string
     */
    public URL getUrlDevice(String deviceId) throws MalformedURLException, IllegalArgumentException
    {
        // Codes_SRS_SERVICE_SDK_JAVA_IOTHUBCONNECTIONSTRING_12_002: [The function shall throw IllegalArgumentException
        // if the input string is empty or null]
        if (Tools.isNullOrEmpty(deviceId))
        {
            throw new IllegalArgumentException("device name cannot be empty or null");
        }

        // Codes_SRS_SERVICE_SDK_JAVA_IOTHUBCONNECTIONSTRING_12_003: [The function shall create a URL object
        // from the given deviceId using the following format: https:hostname/devices/deviceId?api-version=201X-XX-XX]
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

    /**
     * Create url for requesting device list
     *
     * @param maxCount The number of requested devices
     * @return URL string to get the device list from IotHub
     * @throws MalformedURLException This exception is thrown if the URL creation failed due to malformed string
     */
    public URL getUrlDeviceList(Integer maxCount) throws MalformedURLException, IllegalArgumentException
    {
        // Codes_SRS_SERVICE_SDK_JAVA_IOTHUBCONNECTIONSTRING_12_004: [The constructor shall throw NullPointerException
        // if the input integer is null]
        if ((maxCount == null) || (maxCount < 1))
        {
            throw new IllegalArgumentException("maxCount cannot be null or less then 1");
        }

        // Codes_SRS_SERVICE_SDK_JAVA_IOTHUBCONNECTIONSTRING_12_005: [The function shall create a URL object from
        // the given integer using the following format: https:hostname/devices/?maxCount=XX&api-version=201X-XX-XX]
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

    /**
     * Create url for requesting device statistics
     *
     * @return The device statistics Url in the following format: "https:hostname/statistics/devices?api-version=201X-XX-XX"
     * @throws MalformedURLException This exception is thrown if the URL creation failed due to malformed string
     */
    public URL getUrlDeviceStatistics() throws MalformedURLException
    {
        // Codes_SRS_SERVICE_SDK_JAVA_IOTHUBCONNECTIONSTRING_12_006: [The function shall create a URL object from
        // the object properties using the following format: https:hostname/statistics/devices?api-version=201X-XX-XX]
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

    /**
     * Create url for processing a bulk import/export job
     *
     * @return The import/export job URL in the following format: "https:hostname/jobs/create?api-version=201X-XX-XX"
     * @throws MalformedURLException This exception is thrown if the URL creation failed due to malformed string
     */
    public URL getUrlCreateExportImportJob() throws MalformedURLException
    {
        // CODES_SRS_SERVICE_SDK_JAVA_IOTHUBCONNECTIONSTRING_15_008: [The function shall create a URL object
        // from the object properties using the following format: https:hostname/jobs/create?api-version=201X-XX-XX.]
        StringBuilder stringBuilder = new StringBuilder();
        stringBuilder.append(URL_HTTPS);
        stringBuilder.append(hostName);
        stringBuilder.append(URL_SEPARATOR_0);
        stringBuilder.append("jobs");
        stringBuilder.append(URL_SEPARATOR_0);
        stringBuilder.append("create");
        stringBuilder.append(URL_SEPARATOR_1);
        stringBuilder.append(URL_API_VERSION);
        return new URL(stringBuilder.toString());
    }

    /**
     *
     * @param jobId Create url for retrieving a bulk import/export job
     * @return The import/export job URL in the following format: "https:hostname/jobs/jobId?api-version=201X-XX-XX"
     * @throws MalformedURLException This exception is thrown if the URL creation failed due to malformed string
     */
    public URL getUrlImportExportJob(String jobId) throws MalformedURLException
    {
        // CODES_SRS_SERVICE_SDK_JAVA_IOTHUBCONNECTIONSTRING_15_009: [The function shall create a URL object from
        // the object properties using the following format: https:hostname/jobs/jobId?api-version=201X-XX-XX.]
        StringBuilder stringBuilder = new StringBuilder();
        stringBuilder.append(URL_HTTPS);
        stringBuilder.append(hostName);
        stringBuilder.append(URL_SEPARATOR_0);
        stringBuilder.append("jobs");
        stringBuilder.append(URL_SEPARATOR_0);
        stringBuilder.append(jobId);
        stringBuilder.append(URL_SEPARATOR_1);
        stringBuilder.append(URL_API_VERSION);
        return new URL(stringBuilder.toString());
    }

    /**
     * Serialize connection string
     *
     * @return Iot Hub connection string
     */
    @Override
    public String toString()
    {
        // Codes_SRS_SERVICE_SDK_JAVA_IOTHUBCONNECTIONSTRING_12_007: [The function shall serialize the object to a
        // string using the following format: HostName=HOSTNAME.b.c.d;SharedAccessKeyName=ACCESSKEYNAME;SharedAccessKey=1234567890abcdefghijklmnopqrstvwxyz=;SharedAccessSignature=]
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

    /**
     * Getter for iotHubName
     *
     * @return The iot hub name string
     */
    public String getIotHubName()
    {
        return this.iotHubName;
    }

    /**
     * Getter for authenticationMethod
     *
     * @return The authenticationMethod object
     */
    public AuthenticationMethod getAuthenticationMethod()
    {
        return this.authenticationMethod;
    }

    /**
     * Getter for sharedAccessKeyName
     *
     * @return The sharedAccessKeyName string
     */
    public String getSharedAccessKeyName()
    {
        return this.sharedAccessKeyName;
    }

    /**
     * Getter for sharedAccessKey
     *
     * @return The sharedAccessKey string
     */
    public String getSharedAccessKey()
    {
        return this.sharedAccessKey;
    }

    /**
     * Getter for sharedAccessSignature
     *
     * @return The sharedAccessSignature string
     */
    public String getSharedAccessSignature()
    {
        return this.sharedAccessSignature;
    }

    /**
     * Getter for hostName
     *
     * @return The hostName string
     */
    public String getHostName()
    {
        return this.hostName;
    }

    /**
     * Setter for sharedAccessKeyName
     *
     * @param sharedAccessKeyName The value of the signature to set
     */
    protected void setSharedAccessKeyName(String sharedAccessKeyName)
    {
        this.sharedAccessKeyName = sharedAccessKeyName;
    }

    /**
     * Setter for sharedAccessKey
     *
     * @param sharedAccessKey The value of the signature to set
     */
    protected void setSharedAccessKey(String sharedAccessKey)
    {
        this.sharedAccessKey = sharedAccessKey;
    }

    /**
     * Setter for sharedAccessSignature
     *
     * @param sharedAccessSignature The value of the signature to set
     */
    protected void setSharedAccessSignature(String sharedAccessSignature)
    {
        this.sharedAccessSignature = sharedAccessSignature;
    }
}
