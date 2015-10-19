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

public class Tools
{
    public static Boolean isNullOrEmpty(String value)
    {
        Boolean retVal;
        
        if (value == null)
            retVal = true;
        else
            retVal = value.length() == 0;
        
        return retVal;
    }

    public static Boolean isNullOrWhiteSpace(String value)
    {
        Boolean retVal;
        
        if (value == null)
            retVal = true;
        else
            retVal = isNullOrEmpty(value.trim());
        
        return retVal;
    }

    public static String getValueStringByKey(Map map, String keyName)
    {
        String retVal;
        
        if ((map == null) || (keyName == null))
            retVal = "";
        else
        {
            Object val = map.get(keyName);
            if (val != null)
                retVal = val.toString().trim();
            else
                retVal = "";
        }
        
        return retVal;
    }

    public static String getStringValueFromJsonObject(JsonObject jsonObject, String key)
    {
        String strValue = "";
        if (jsonObject != JsonObject.NULL)
        {
            JsonValue jsonValue = jsonObject.get(key);
            if (jsonValue != JsonValue.NULL)
            {
                strValue = getValueFromJsonString(jsonObject.getJsonString(key));
            }
        }
        return strValue;
    }

    public static String getValueFromJsonString(JsonString jsonString)
    {
        String strValue = "";
        if (jsonString != null)
        {
            strValue = jsonString.toString();
            if (strValue.startsWith("\""))
            {
                strValue = strValue.replaceFirst("\"", "");
            }
            if (strValue.endsWith("\""))
            {
                strValue = strValue.substring(0, strValue.length()-1);
            }
        }
        return strValue;
    }

    public static long getNumberValueFromJsonObject(JsonObject jsonObject, String key)
    {
        long retVal = 0;
        JsonNumber jsonNumber = null;
        if (jsonObject != JsonObject.NULL)
        {
            JsonValue jsonValue = jsonObject.get(key);
            if (jsonValue != JsonValue.NULL)
            {
                jsonNumber = jsonObject.getJsonNumber(key);
            }
        }
        if (jsonNumber != null)
        {
            retVal = jsonNumber.longValue();
        }
        return retVal;
    }

    public static void appendJsonAttribute(StringBuilder strBuilder, String name, String value, Boolean isQuoted, Boolean isLast)
    {
        strBuilder.append("\"");
        strBuilder.append(name);
        strBuilder.append("\"");
        strBuilder.append(":");
        if (Tools.isNullOrEmpty(value))
        {
            strBuilder.append("null");
        }
        else
        {
            if (isQuoted)
                strBuilder.append("\"");
            strBuilder.append(value);
            if (isQuoted)
                strBuilder.append("\"");
        }
        if (!isLast)
            strBuilder.append(",");
    }

}
