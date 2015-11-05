/*
 * Copyright (c) Microsoft. All rights reserved.
 * Licensed under the MIT license. See LICENSE file in the project root for full license information.
 */

package com.microsoft.azure.iot.service.sdk;

import java.util.HashMap;
import java.util.StringTokenizer;
import java.util.regex.Pattern;

/**
 * Provide static constructors to create IotHubConnectionString object
 */
public class IotHubConnectionStringBuilder
{
    private static final String HOST_NAME_REGEX = "[a-zA-Z0-9_\\-\\.]+$";
    private static final String SHARED_ACCESS_KEY_NAME_REGEX = "^[a-zA-Z0-9_\\-@\\.]+$";
    private static final String SHARED_ACCESS_KEY_REGEX = "^.+$";
    private static final String SHARED_ACCESS_SIGNATURE_REGEX = "^.+$";

    private String hostNameSuffix;
    private static final HashMap<String, String> keyValueMap = new HashMap<String, String>();

    /**
     * Static constructor to create IotHubConnectionString deserialize the given string
     *
     * @param connectionString The serialized connection string
     * @return The IotHubConnectionString object
     * @throws Exception This exception is thrown if the object creation failed
     */
    public static IotHubConnectionString createConnectionString(String connectionString) throws Exception
    {
        // Codes_SRS_SERVICE_SDK_JAVA_IOTHUBCONNECTIONSTRINGBUILDER_12_001: [The function shall throw IllegalArgumentException if the input string is empty or null]
        if (Tools.isNullOrEmpty(connectionString))
        {
            throw new IllegalArgumentException("connection string cannotbe null or empty");
        }
        // Codes_SRS_SERVICE_SDK_JAVA_IOTHUBCONNECTIONSTRINGBUILDER_12_002: [The function shall create a new IotHubConnectionString object deserializing the given string]
        IotHubConnectionString iotHubConnectionString = new IotHubConnectionString();
        parse(connectionString, iotHubConnectionString);
        return iotHubConnectionString;
    }

    /**
     * Static constructor to create IotHubConnectionString from host name and authentication method
     *
     * @param hostName The hostName string
     * @param authenticationMethod The AuthenticationMethod object
     * @return The IotHubConnectionString object
     * @throws Exception This exception is thrown if the object creation failed
     */
    public static IotHubConnectionString createConnectionString(String hostName, AuthenticationMethod authenticationMethod) throws Exception
    {
        // Codes_SRS_SERVICE_SDK_JAVA_IOTHUBCONNECTIONSTRINGBUILDER_12_003: [The function shall throw IllegalArgumentException if the input string is empty or null]
        if (Tools.isNullOrEmpty(hostName))
        {
            throw new IllegalArgumentException("hostName cannot be null or empty");
        }
        // Codes_SRS_SERVICE_SDK_JAVA_IOTHUBCONNECTIONSTRINGBUILDER_12_004: [The function shall throw IllegalArgumentException if the input authenticationMethod is null]
        if (authenticationMethod == null)
        {
            throw new IllegalArgumentException("authenticationMethod cannot be null");
        }
        // Codes_SRS_SERVICE_SDK_JAVA_IOTHUBCONNECTIONSTRINGBUILDER_12_005: [The function shall create a new IotHubConnectionString object using the given hostname and auhtenticationMethod]
        IotHubConnectionString iotHubConnectionString = new IotHubConnectionString();
        setHostName(hostName, iotHubConnectionString);
        setAuthenticationMethod(authenticationMethod, iotHubConnectionString);
        validate(iotHubConnectionString);
        return iotHubConnectionString;
    }

    /**
     * Deserialize connection string
     *
     * @param connectionString The connection string to deserialize
     * @param iotHubConnectionString The target object for deserialization
     * @throws Exception This exception is thrown if the parsing failed
     */
    protected static void parse(String connectionString, IotHubConnectionString iotHubConnectionString) throws Exception
    {
        // Codes_SRS_SERVICE_SDK_JAVA_IOTHUBCONNECTIONSTRINGBUILDER_12_006: [The function shall throw IllegalArgumentException if the input string is empty or null]
        if (Tools.isNullOrEmpty(connectionString))
        {
            throw new IllegalArgumentException("connectionString cannot be null or empty");
        }
        // Codes_SRS_SERVICE_SDK_JAVA_IOTHUBCONNECTIONSTRINGBUILDER_12_007: [The function shall throw IllegalArgumentException if the input target itoHubConnectionString is null]
        if (iotHubConnectionString == null)
        {
            throw new IllegalArgumentException("iotHubConnectionString cannot be null");
        }

        // Codes_SRS_SERVICE_SDK_JAVA_IOTHUBCONNECTIONSTRINGBUILDER_12_008: [The function shall throw exception if tokenizing or parsing failed]
        // Codes_SRS_SERVICE_SDK_JAVA_IOTHUBCONNECTIONSTRINGBUILDER_12_009: [The function shall tokenize and parse the given connection string and fill up the target IotHubConnectionString object with proper values]
        keyValueMap.clear();
        StringTokenizer stringTokenizer1 = new StringTokenizer(connectionString, IotHubConnectionString.VALUE_PAIR_DELIMITER);
        while (stringTokenizer1.hasMoreTokens())
        {
            String currentToken = stringTokenizer1.nextToken();

            String[] splitString = currentToken.split(IotHubConnectionString.VALUE_PAIR_SEPARATOR, 2);
            if (splitString.length == 2)
                keyValueMap.put(splitString[0], splitString[1]);
        }

        iotHubConnectionString.hostName = Tools.getValueStringByKey(keyValueMap, IotHubConnectionString.HOST_NAME_PROPERTY_NAME);
        iotHubConnectionString.sharedAccessKeyName = Tools.getValueStringByKey(keyValueMap, IotHubConnectionString.SHARED_ACCESS_KEY_NAME_PROPERTY_NAME);
        iotHubConnectionString.sharedAccessKey = Tools.getValueStringByKey(keyValueMap, IotHubConnectionString.SHARED_ACCESS_KEY_PROPERTY_NAME);
        iotHubConnectionString.sharedAccessSignature = Tools.getValueStringByKey(keyValueMap, IotHubConnectionString.SHARED_ACCESS_SIGNATURE_PROPERTY_NAME);
        iotHubConnectionString.iotHubName = parseIotHubName(iotHubConnectionString);

        // Codes_SRS_SERVICE_SDK_JAVA_IOTHUBCONNECTIONSTRINGBUILDER_12_010: [The function shall create new ServiceAuthenticationWithSharedAccessPolicyToken and set the authenticationMethod if sharedAccessKey is not defined]
        if (Tools.isNullOrWhiteSpace(iotHubConnectionString.sharedAccessKey))
        {
            iotHubConnectionString.authenticationMethod = new ServiceAuthenticationWithSharedAccessPolicyToken(
                    iotHubConnectionString.sharedAccessKeyName,
                    iotHubConnectionString.sharedAccessSignature);
        }
        // Codes_SRS_SERVICE_SDK_JAVA_IOTHUBCONNECTIONSTRINGBUILDER_12_011: [The function shall create new ServiceAuthenticationWithSharedAccessPolicyKey and set the authenticationMethod if the sharedAccessSignature is not defined]
        else if (Tools.isNullOrWhiteSpace(iotHubConnectionString.sharedAccessSignature))
        {
            iotHubConnectionString.authenticationMethod = new ServiceAuthenticationWithSharedAccessPolicyKey(
                    iotHubConnectionString.sharedAccessKeyName,
                    iotHubConnectionString.sharedAccessKey);
        }

        // Codes_SRS_SERVICE_SDK_JAVA_IOTHUBCONNECTIONSTRINGBUILDER_12_012: [The function shall validate the connection string object]
        validate(iotHubConnectionString);
    }

    /**
     * Parse the iot hub name part from the host name
     *
     * @param iotHubConnectionString The source iotHubConnectionString containing the hostName
     * @return The substring of the host name until the first "." character
     */
    protected static String parseIotHubName(IotHubConnectionString iotHubConnectionString)
    {
        // Codes_SRS_SERVICE_SDK_JAVA_IOTHUBCONNECTIONSTRINGBUILDER_12_013: [The function shall return the substring of the host name until the first “.” character]
        Integer index = iotHubConnectionString.hostName.indexOf(IotHubConnectionString.HOST_NAME_SEPARATOR);
        if (index >= 0)
        {
            return iotHubConnectionString.hostName.substring(0, index);
        }
        // Codes_SRS_SERVICE_SDK_JAVA_IOTHUBCONNECTIONSTRINGBUILDER_12_014: [The function shall return empty string if “.” character was not found]
        else
        {
            return "";
        }
    }

    /**
     * Validate IotHubConnectionString format
     *
     * @param iotHubConnectionString The object to validate
     * @throws IllegalArgumentException This exception is thrown if the input object is null
     */
    protected static void validate(IotHubConnectionString iotHubConnectionString) throws IllegalArgumentException
    {
        // Codes_SRS_SERVICE_SDK_JAVA_IOTHUBCONNECTIONSTRINGBUILDER_12_015: [The function shall throw IllegalArgumentException if the sharedAccessKeyName of the input itoHubConnectionString is empty]
        if (Tools.isNullOrWhiteSpace(iotHubConnectionString.sharedAccessKeyName))
        {
            throw new IllegalArgumentException("SharedAccessKeyName cannot be null or empty");
        }
        // CodesSRS_SERVICE_SDK_JAVA_IOTHUBCONNECTIONSTRINGBUILDER_12_016: [The function shall throw IllegalArgumentException if either of the sharedAccessKey or the sharedAccessSignature of the input itoHubConnectionString is empty]
        if (Tools.isNullOrWhiteSpace(iotHubConnectionString.sharedAccessKey) && Tools.isNullOrWhiteSpace(iotHubConnectionString.sharedAccessSignature))
        {
            throw new IllegalArgumentException("Should specify either sharedAccessKey or sharedAccessSignature");
        }

        // Codes_SRS_SERVICE_SDK_JAVA_IOTHUBCONNECTIONSTRINGBUILDER_12_017: [The function shall call property validation functions for hostname, sharedAccessKeyName, sharedAccessKey, sharedAccessSignature]
        validateFormat(iotHubConnectionString.hostName, IotHubConnectionString.HOST_NAME_PROPERTY_NAME, HOST_NAME_REGEX);
        validateFormatIfSpecified(iotHubConnectionString.sharedAccessKeyName, IotHubConnectionString.SHARED_ACCESS_KEY_NAME_PROPERTY_NAME, SHARED_ACCESS_KEY_NAME_REGEX);
        validateFormatIfSpecified(iotHubConnectionString.sharedAccessKey, IotHubConnectionString.SHARED_ACCESS_KEY_PROPERTY_NAME, SHARED_ACCESS_KEY_REGEX);
        validateFormatIfSpecified(iotHubConnectionString.sharedAccessSignature, IotHubConnectionString.SHARED_ACCESS_SIGNATURE_PROPERTY_NAME, SHARED_ACCESS_SIGNATURE_REGEX);
    }

    /**
     * Validate string property using given regex
     *
     * @param value The string value to validate
     * @param propertyName The property name
     * @param regex The regex used for validation
     */
    protected static void validateFormat(String value, String propertyName, String regex)
    {
        // Codes_SRS_SERVICE_SDK_JAVA_IOTHUBCONNECTIONSTRINGBUILDER_12_018: [The function shall validate the property value against the given regex]
        final Pattern pattern = Pattern.compile(regex);
        // Codes_SRS_SERVICE_SDK_JAVA_IOTHUBCONNECTIONSTRINGBUILDER_12_019: [The function shall throw IllegalArgumentException if the value did not match with the pattern]
        if (!pattern.matcher(value).matches())
        {
            throw new IllegalArgumentException("The connection string has an invalid value for property.");
        }
    }

    /**
     * Validate string property using given regex if value is not null or empty
     *
     * @param value string value to validate
     * @param propertyName property name
     * @param regex regex used for validation
     */
    protected static void validateFormatIfSpecified(String value, String propertyName, String regex)
    {
        // Codes_SRS_SERVICE_SDK_JAVA_IOTHUBCONNECTIONSTRINGBUILDER_12_020: [The function shall validate the property value against the given regex if the value is not null or empty]
        if (!Tools.isNullOrEmpty(value))
        {
            validateFormat(value, propertyName, regex);
        }
    }

    /**
     * Set host name value to target IotHubConnectionString object
     *
     * @param hostName host name string
     * @param iotHubConnectionString target IotHubConnectionString object
     */
    protected static void setHostName(String hostName, IotHubConnectionString iotHubConnectionString)
    {
        // Codes_SRS_SERVICE_SDK_JAVA_IOTHUBCONNECTIONSTRINGBUILDER_12_021: [The function shall validate the given hostName]
        validateFormat(hostName, IotHubConnectionString.HOST_NAME_PROPERTY_NAME, HOST_NAME_REGEX);
        // Codes_SRS_SERVICE_SDK_JAVA_IOTHUBCONNECTIONSTRINGBUILDER_12_022: [The function shall parse and set the hostname to the given target iotHubConnectionString object]
        iotHubConnectionString.hostName = hostName;
        iotHubConnectionString.iotHubName = parseIotHubName(iotHubConnectionString);
    }

    /**
     * Set authentication method to target IotHubConnectionString object
     *
     * @param authenticationMethod value to set
     * @param iotHubConnectionString target IotHubConnectionString object
     */
    protected static void setAuthenticationMethod(AuthenticationMethod authenticationMethod, IotHubConnectionString iotHubConnectionString)
    {
        // Codes_SRS_SERVICE_SDK_JAVA_IOTHUBCONNECTIONSTRINGBUILDER_12_023: [The function shall populate and set the authenticationMethod on the given target iotHubConnectionString object]
        authenticationMethod.populate(iotHubConnectionString);
        iotHubConnectionString.authenticationMethod = authenticationMethod;
    }
}
