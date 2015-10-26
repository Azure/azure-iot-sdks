/*
 * Copyright (c) Microsoft. All rights reserved.
 * Licensed under the MIT license. See LICENSE file in the project root for full license information.
 */

package com.microsoft.azure.iot.service.sdk;

import javax.json.JsonNumber;
import javax.json.JsonObject;
import javax.json.JsonString;
import javax.json.JsonValue;
import java.util.Map;

/**
 * Collection of static helper functions
 */
public class Tools
{
    /**
     * Helper function to check if the
     * input string is null or empty
     *
     * @param value string to check
     * @return true if the input string is empty or null
     */
    public static Boolean isNullOrEmpty(String value)
    {
        Boolean retVal;
        
        if (value == null)
            // Codes_SRS_SERVICE_SDK_JAVA_TOOLS_12_001: [The function shall return true if the input is null]
            retVal = true;
        else
            // Codes_SRS_SERVICE_SDK_JAVA_TOOLS_12_002: [The function shall return true if the input string’s length is zero]
            // Codes_SRS_SERVICE_SDK_JAVA_TOOLS_12_003: [The function shall return false otherwise]
            retVal = value.length() == 0;
        
        return retVal;
    }

    /**
     * Helper function to check if the
     * input string is null or contains only whitespace(s)
     *
     * @param value string to check
     * @return true if the input string is empty or contains only whitespace(s)
     */
    public static Boolean isNullOrWhiteSpace(String value)
    {
        Boolean retVal;
        
        if (value == null)
        {
            // Codes_SRS_SERVICE_SDK_JAVA_TOOLS_12_004: [The function shall return true if the input is null]
            retVal = true;
        }
        else
        {
            // Codes_SRS_SERVICE_SDK_JAVA_TOOLS_12_005: [The function shall call the isNullOrEmpty function and return with it’s return value]
            retVal = isNullOrEmpty(value.trim());
        }
        return retVal;
    }

    /**
     * Helper function to get a value from the given Map
     * if the key name exists
     *
     * @param map Map object to get the value from
     * @param keyName name of the key
     * @return the value of the given key if exists otherwise empty string
     */
    public static String getValueStringByKey(Map map, String keyName)
    {
        String retVal;
        
        if ((map == null) || (keyName == null))
        {
            // Codes_SRS_SERVICE_SDK_JAVA_TOOLS_12_006: [The function shall return empty string if any of the input is null]
            retVal = "";
        }
        else
        {
            // Codes_SRS_SERVICE_SDK_JAVA_TOOLS_12_007: [The function shall get the value of the given key from the map]
            Object val = map.get(keyName);
            if (val != null)
                // Codes_SRS_SERVICE_SDK_JAVA_TOOLS_12_008: [The function shall return with trimmed string if the value of the key is not null]
                retVal = val.toString().trim();
            else
                // Codes_SRS_SERVICE_SDK_JAVA_TOOLS_12_009: [The function shall return with empty string if the value of the key is null]
                retVal = "";
        }
        
        return retVal;
    }

    /**
     * Helper function to get a value from the given JsonObject
     * if the key name exists
     *JsonObject object to get the value from
     * @param jsonObject JsonObject object to get the value from
     * @param key name of the key
     * @return the value of the given key if exists otherwise empty string
     */
    public static String getValueFromJsonObject(JsonObject jsonObject, String key)
    {
        String retVal;
        // Codes_SRS_SERVICE_SDK_JAVA_TOOLS_12_010: [The function shall return empty string if any of the input is null]
        if ((jsonObject == null) || (jsonObject == JsonObject.NULL) || (key == null) || (key.length() == 0))
        {
            retVal = "";
        }
        else
        {
            // Codes_SRS_SERVICE_SDK_JAVA_TOOLS_12_011: [The function shall get the JsonValue of the key]
            JsonValue jsonValue = jsonObject.get(key);
            if (jsonValue != JsonValue.NULL)
            {
                // Codes_SRS_SERVICE_SDK_JAVA_TOOLS_12_012: [The function shall get the JsonString from the JsonValue if the JsonValue is not null]
                // Codes_SRS_SERVICE_SDK_JAVA_TOOLS_12_013: [The function shall return the string value from the JsonString calling the getValueFromJsonString function]
                retVal = getValueFromJsonString(jsonObject.getJsonString(key));
            }
            else
            {
                // Codes_SRS_SERVICE_SDK_JAVA_TOOLS_12_014: [The function shall return empty string if the JsonValue is null]
                retVal = "";
            }
        }
        return retVal;
    }

    /**
     * Helper function to get trim the leading and trailing
     * parenthesis from a Json string if they exists
     *
     * @param jsonString JsonString to trim
     * @return trimmed string
     */
    public static String getValueFromJsonString(JsonString jsonString)
    {
        String retVal;
        // Codes_SRS_SERVICE_SDK_JAVA_TOOLS_12_015: [The function shall return empty string if the input is null]
        if (jsonString == null)
        {
            retVal = "";
        }
        else
        {
            // Codes_SRS_SERVICE_SDK_JAVA_TOOLS_12_016: [The function shall get the string value from JsonString]
            retVal = jsonString.toString();
            // Codes_SRS_SERVICE_SDK_JAVA_TOOLS_12_017: [The function shall trim the leading and trailing parenthesis from the string and return with it]
            if (retVal.startsWith("\""))
            {
                retVal = retVal.replaceFirst("\"", "");
            }
            if (retVal.endsWith("\""))
            {
                retVal = retVal.substring(0, retVal.length()-1);
            }
        }
        return retVal;
    }

    /**
     * Helper function to get numeric value
     * from a JsonObject
     *
     * @param jsonObject JsonObject object to get the value from
     * @param key name of the key
     * @return numeric value
     */
    public static long getNumberValueFromJsonObject(JsonObject jsonObject, String key)
    {
        long retVal;
        JsonNumber jsonNumber = null;
        // Codes_SRS_SERVICE_SDK_JAVA_TOOLS_12_018: [The function shall return zero if any of the input is null]
        if ((jsonObject == null) || (jsonObject == JsonObject.NULL) || (key == null) || (key.length() == 0))
        {
            retVal = 0;
        }
        else
        {
            // Codes_SRS_SERVICE_SDK_JAVA_TOOLS_12_019: [The function shall get the JsonValue of the key and return zero if it is null]
            JsonValue jsonValue = jsonObject.get(key);
            if (jsonValue != JsonValue.NULL)
            {
                // Codes_SRS_SERVICE_SDK_JAVA_TOOLS_12_020: [The function shall get the JsonNumber from the JsonValue and return zero if it is null]
                jsonNumber = jsonObject.getJsonNumber(key);
                if (jsonNumber != null)
                {
                    // Codes_SRS_SERVICE_SDK_JAVA_TOOLS_12_021: [The function shall return the long value from the JsonNumber if the JsonNumber is not null]
                    retVal = jsonNumber.longValue();
                }
                else
                {
                    retVal = 0;
                }
            }
            else
            {
                retVal = 0;
            }
        }
        return retVal;
    }

    /**
     * Helper function to properly craft Json string
     * of key-value pair
     *
     * @param strBuilder StringBuilder to work on
     * @param name name of the key
     * @param value value
     * @param isQuoted if true leading and trailing quotes will be added
     * @param isLast if false trailing comma will added
     */
    public static void appendJsonAttribute(StringBuilder strBuilder, String name, String value, Boolean isQuoted, Boolean isLast)
    {
        // Codes_SRS_SERVICE_SDK_JAVA_TOOLS_12_022: [The function shall do nothing if the input StringBuilder is null]
        if (strBuilder != null)
        {
            // Codes_SRS_SERVICE_SDK_JAVA_TOOLS_12_023: [The function shall append the input StringBuilder string using the following format: “name”:”value” if isQuoted is false]
            strBuilder.append("\"");

            if (Tools.isNullOrEmpty(name))
                strBuilder.append("");
            else
                strBuilder.append(name);

            strBuilder.append("\"");
            strBuilder.append(":");

            if (Tools.isNullOrEmpty(value))
            {
                strBuilder.append("null");
            }
            else
            {
                // Codes_SRS_SERVICE_SDK_JAVA_TOOLS_12_024: [The function shall append the input StringBuilder string using the following format: “name”:””value”” if isQuoted is true]
                if (isQuoted)
                {
                    strBuilder.append("\"");
                }

                strBuilder.append(value);

                if (isQuoted)
                {
                    strBuilder.append("\"");
                }
            }
            // Codes_SRS_SERVICE_SDK_JAVA_TOOLS_12_024: [The function shall append the input StringBuilder string with trailing “,” character isLast is false]
            if (!isLast)
            {
                strBuilder.append(",");
            }
        }
    }

}
