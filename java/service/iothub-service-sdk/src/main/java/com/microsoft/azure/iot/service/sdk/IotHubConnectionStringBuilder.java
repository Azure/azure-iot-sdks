/*
 * Copyright (c) Microsoft. All rights reserved.
 * Licensed under the MIT license. See LICENSE file in the project root for full license information.
 */

package com.microsoft.azure.iot.service.sdk;

import java.util.HashMap;
import java.util.StringTokenizer;
import java.util.regex.Pattern;

public abstract class IotHubConnectionStringBuilder
{
    private static final String HOST_NAME_REGEX = "[a-zA-Z0-9_\\-\\.]+$";
    private static final String SHARED_ACCESS_KEY_NAME_REGEX = "^[a-zA-Z0-9_\\-@\\.]+$";
    private static final String SHARED_ACCESS_KEY_REGEX = "^.+$";
    private static final String SHARED_ACCESS_SIGNATURE_REGEX = "^.+$";

    private String hostNameSuffix;
    private static final HashMap<String, String> keyValueMap = new HashMap<String, String>();

    public static IotHubConnectionString createConnectionString(String connectionString) throws Exception
    {
        if (Tools.isNullOrEmpty(connectionString))
        {
            throw new IllegalArgumentException();
        }
        IotHubConnectionString iotHubConnectionString = new IotHubConnectionString();

        parse(connectionString, iotHubConnectionString);
        return iotHubConnectionString;
    }

    public static IotHubConnectionString createConnectionString(String hostName, AuthenticationMethod authenticationMethod) throws Exception
    {
        if (Tools.isNullOrEmpty(hostName) || (authenticationMethod == null))
        {
            throw new IllegalArgumentException();
        }
        IotHubConnectionString iotHubConnectionString = new IotHubConnectionString();

        setHostName(hostName, iotHubConnectionString);
        setAuthenticationMethod(authenticationMethod, iotHubConnectionString);
        validate(iotHubConnectionString);
        return iotHubConnectionString;
    }

    private static void parse(String connectionString, IotHubConnectionString iotHubConnectionString) throws Exception
    {
        keyValueMap.clear();
        StringTokenizer stringTokenizer1 = new StringTokenizer(connectionString, IotHubConnectionString.VALUE_PAIR_DELIMITER);
        while (stringTokenizer1.hasMoreTokens())
        {
            String currentToken = stringTokenizer1.nextToken();
            System.out.println(currentToken);

            String[] splitString = currentToken.split(IotHubConnectionString.VALUE_PAIR_SEPARATOR, 2);
            if (splitString.length == 2)
                keyValueMap.put(splitString[0], splitString[1]);
        }

        iotHubConnectionString.hostName = Tools.getValueStringByKey(keyValueMap, IotHubConnectionString.HOST_NAME_PROPERTY_NAME);
        iotHubConnectionString.sharedAccessKeyName = Tools.getValueStringByKey(keyValueMap, IotHubConnectionString.SHARED_ACCESS_KEY_NAME_PROPERTY_NAME);
        iotHubConnectionString.sharedAccessKey = Tools.getValueStringByKey(keyValueMap, IotHubConnectionString.SHARED_ACCESS_KEY_PROPERTY_NAME);
        iotHubConnectionString.sharedAccessSignature = Tools.getValueStringByKey(keyValueMap, IotHubConnectionString.SHARED_ACCESS_SIGNATURE_PROPERTY_NAME);
        iotHubConnectionString.iotHubName = parseIotHubName(iotHubConnectionString.hostName, iotHubConnectionString);

        if (Tools.isNullOrWhiteSpace(iotHubConnectionString.sharedAccessKey))
        {
            iotHubConnectionString.authenticationMethod = new ServiceAuthenticationWithSharedAccessPolicyToken(
                    iotHubConnectionString.sharedAccessKeyName,
                    iotHubConnectionString.sharedAccessSignature);
        }
        else if (Tools.isNullOrWhiteSpace(iotHubConnectionString.sharedAccessSignature))
        {
            iotHubConnectionString.authenticationMethod = new ServiceAuthenticationWithSharedAccessPolicyKey(
                    iotHubConnectionString.sharedAccessKeyName,
                    iotHubConnectionString.sharedAccessKey);
        }

        validate(iotHubConnectionString);
    }

    private static String parseIotHubName(String hostName, IotHubConnectionString iotHubConnectionString)
    {
        Integer index = iotHubConnectionString.hostName.indexOf(IotHubConnectionString.HOST_NAME_SEPARATOR);
        if (index >= 0)
        {
            return iotHubConnectionString.hostName.substring(0, index);
        }
        else
        {
            return "";
        }
    }

    private static void validate(IotHubConnectionString iotHubConnectionString) throws Exception
    {
        if (Tools.isNullOrWhiteSpace(iotHubConnectionString.sharedAccessKeyName))
        {
            throw new Exception("Should specify SharedAccessKeyName");
        }

        if (Tools.isNullOrWhiteSpace(iotHubConnectionString.sharedAccessKey) && Tools.isNullOrWhiteSpace(iotHubConnectionString.sharedAccessSignature))
        {
            throw new Exception("Should specify either sharedAccessKey or sharedAccessSignature");
        }

        validateFormat(iotHubConnectionString.hostName, IotHubConnectionString.HOST_NAME_PROPERTY_NAME, HOST_NAME_REGEX);
        validateFormatIfSpecified(iotHubConnectionString.sharedAccessKeyName, IotHubConnectionString.SHARED_ACCESS_KEY_NAME_PROPERTY_NAME, SHARED_ACCESS_KEY_NAME_REGEX);
        validateFormatIfSpecified(iotHubConnectionString.sharedAccessKey, IotHubConnectionString.SHARED_ACCESS_KEY_PROPERTY_NAME, SHARED_ACCESS_KEY_REGEX);
        validateFormatIfSpecified(iotHubConnectionString.sharedAccessSignature, IotHubConnectionString.SHARED_ACCESS_SIGNATURE_PROPERTY_NAME, SHARED_ACCESS_SIGNATURE_REGEX);
    }

    private static void validateFormat(String value, String propertyName, String regex)
    {
        final Pattern pattern = Pattern.compile(regex);
        if (!pattern.matcher(value).matches())
        {
            throw new IllegalArgumentException("The connection string has an invalid value for property: " + propertyName);
        }
    }

    private static void validateFormatIfSpecified(String value, String propertyName, String regex)
    {
        if (!Tools.isNullOrEmpty(value))
        {
            validateFormat(value, propertyName, regex);
        }
    }

    private static void setHostName(String hostName, IotHubConnectionString iotHubConnectionString)
    {
        validateFormat(hostName, IotHubConnectionString.HOST_NAME_PROPERTY_NAME, HOST_NAME_REGEX);
        iotHubConnectionString.hostName = hostName;
        iotHubConnectionString.iotHubName = parseIotHubName(iotHubConnectionString.hostName, iotHubConnectionString);
    }

    private static void setAuthenticationMethod(AuthenticationMethod authenticationMethod, IotHubConnectionString iotHubConnectionString)
    {
        authenticationMethod.populate(iotHubConnectionString);
        iotHubConnectionString.authenticationMethod = authenticationMethod;
    }
}
