/*
 * Copyright (c) Microsoft. All rights reserved.
 * Licensed under the MIT license. See LICENSE file in the project root for full license information.
 */

package com.microsoft.azure.iot.service.sdk;

import com.microsoft.azure.iot.service.auth.SymmetricKey;

import javax.crypto.KeyGenerator;
import javax.json.JsonObject;
import java.security.NoSuchAlgorithmException;
import java.util.Base64;

public class IotHubDevice extends Device
{
    protected IotHubDevice(String id)
    {
        if (Tools.isNullOrEmpty(id))
        {
            throw new IllegalArgumentException();
        }
        
        this.id = id;
        this.generationId = "";
        this.eTag = "";
        
        SymmetricKey symmKey = new SymmetricKey();
        KeyGenerator keyGenerator = null;
        try
        {
            keyGenerator = KeyGenerator.getInstance("AES");
        } catch (NoSuchAlgorithmException e)
        {
        }
        if (keyGenerator != null)
        {   
            symmKey.setPrimaryKey(Base64.getEncoder().encodeToString(keyGenerator.generateKey().getEncoded()));
            symmKey.setSecondaryKey(Base64.getEncoder().encodeToString(keyGenerator.generateKey().getEncoded()));
        }
        this.symmetricKey = symmKey;
        
        this.status = DeviceStatus.Disabled;
        this.statusReason = "";
        this.statusUpdatedTime = "0001-01-01T00:00:00";
        this.connectionState = DeviceConnectionState.Disconnected;
        this.connectionStateUpdatedTime = "0001-01-01T00:00:00";
        this.lastActivityTime = "0001-01-01T00:00:00";
        this.cloudToDeviceMessageCount = 0;
        this.forceUpdate = false;
    }

    protected IotHubDevice(JsonObject jsonObject)
    {
        if ((jsonObject == JsonObject.NULL) || (jsonObject == null))
        {
            throw new IllegalArgumentException();
        }
        try
        {
            this.id = Tools.getStringValueFromJsonObject(jsonObject, "deviceId");
            this.generationId = Tools.getStringValueFromJsonObject(jsonObject, "generationId");
            this.eTag = Tools.getStringValueFromJsonObject(jsonObject, "etag");
            
            JsonObject authenticationObject = jsonObject.getJsonObject("authentication");
            if (authenticationObject == JsonObject.NULL)
            {
                this.symmetricKey = null;
            }
            else
            {
                JsonObject symmKeyObject = authenticationObject.getJsonObject("symmetricKey");
                if (symmKeyObject == JsonObject.NULL)
                {
                    this.symmetricKey = null;
                }
                else
                {
                    String primKeyValue = Tools.getStringValueFromJsonObject(symmKeyObject, "primaryKey");
                    String secKeyValue = Tools.getStringValueFromJsonObject(symmKeyObject, "secondaryKey");
                    SymmetricKey symmKey = new SymmetricKey();
                    symmKey.setPrimaryKey(primKeyValue);
                    symmKey.setSecondaryKey(secKeyValue);
                    this.symmetricKey = symmKey;
                }
            }
            
            String state = Tools.getStringValueFromJsonObject(jsonObject, "status");
            if ("enabled".equals(state))
                this.status = DeviceStatus.Enabled;
            else
                this.status = DeviceStatus.Disabled;
            
            this.statusReason = Tools.getStringValueFromJsonObject(jsonObject, "statusReason");
            this.statusUpdatedTime = Tools.getStringValueFromJsonObject(jsonObject, "statusUpdatedTime");
            
            String connState = Tools.getStringValueFromJsonObject(jsonObject, "connectionState");
            if ("connected".equals(connState))
                this.connectionState = DeviceConnectionState.Connected;
            else
                this.connectionState = DeviceConnectionState.Disconnected;
           
            this.connectionStateUpdatedTime = Tools.getStringValueFromJsonObject(jsonObject, "connectionStateUpdatedTime");
            this.lastActivityTime = Tools.getStringValueFromJsonObject(jsonObject, "lastActivityTime");
            
            this.cloudToDeviceMessageCount = Tools.getNumberValueFromJsonObject(jsonObject, "cloudToDeviceMessageCount");
        }
        catch (Exception e)
        {
            throw new IllegalArgumentException();
        }
    }

    @Override
    protected void clone(Device device)
    {
        if (device == null)
        {
            throw new IllegalArgumentException();
        }
        this.id = device.id;
        this.generationId = device.getGenerationId();
        this.eTag = device.geteTag();
        this.symmetricKey.setPrimaryKey(device.getPrimaryKey());
        this.symmetricKey.setSecondaryKey(device.getSecondaryKey());
        this.status = device.getStatus();
        this.statusReason = device.getStatusReason();
        this.statusUpdatedTime = device.getStatusUpdatedTime();
        this.connectionState = device.getConnectionState();
        this.connectionStateUpdatedTime = device.getConnectionStateUpdatedTime();
        this.lastActivityTime = device.getLastActivityTime();
        this.cloudToDeviceMessageCount = device.getCloudToDeviceMessageCount();
    }
    
    @Override
    protected void setForceUpdate(Boolean forceUpdate)
    {
        if (forceUpdate == null)
        {
            throw new IllegalArgumentException();
        }
        this.forceUpdate = forceUpdate;
    }

    @Override
    protected String serializeToJson()
    {
        StringBuilder jsonBody = new StringBuilder();
        jsonBody.append("{");
        Tools.appendJsonAttribute(jsonBody, "deviceId", this.id, true, false);
        Tools.appendJsonAttribute(jsonBody, "generationId", this.generationId, true, false);
        Tools.appendJsonAttribute(jsonBody, "etag", this.eTag, true, false);
        switch (this.connectionState)
        {
            case Connected:
                Tools.appendJsonAttribute(jsonBody, "connectionState", "Connected", true, false);
                break;
            case Disconnected:
                Tools.appendJsonAttribute(jsonBody, "connectionState", "Disconnected", true, false);
                break;
        }
        Tools.appendJsonAttribute(jsonBody, "connectionStateUpdatedTime", this.connectionStateUpdatedTime, true, false);
        switch (this.status)
        {
            case Enabled:
                Tools.appendJsonAttribute(jsonBody, "status", "enabled", true, false);
                break;
            case Disabled:
                Tools.appendJsonAttribute(jsonBody, "status", "disabled", true, false);
                break;
        }
        Tools.appendJsonAttribute(jsonBody, "statusReason", this.statusReason, true, false);
        Tools.appendJsonAttribute(jsonBody, "statusUpdatedTime", this.statusUpdatedTime, true, false);
        Tools.appendJsonAttribute(jsonBody, "lastActivityTime", this.lastActivityTime, true, false);
        Tools.appendJsonAttribute(jsonBody, "cloudToDeviceMessageCount", String.valueOf(this.cloudToDeviceMessageCount), false, true);
        
        if (this.symmetricKey != null)
        {
            jsonBody.append(",");
            
            jsonBody.append("\"");
            jsonBody.append("authentication");
            jsonBody.append("\"");
            jsonBody.append(":");
            jsonBody.append("{");
            
            jsonBody.append("\"");
            jsonBody.append("symmetricKey");
            jsonBody.append("\"");
            jsonBody.append(":");
            jsonBody.append("{");
            
            jsonBody.append("\"");
            jsonBody.append("primaryKey");
            jsonBody.append("\"");
            jsonBody.append(":");
            
            jsonBody.append("\"");
            jsonBody.append(this.symmetricKey.getPrimaryKey());
            jsonBody.append("\"");
            jsonBody.append(",");
            
            jsonBody.append("\"");
            jsonBody.append("secondaryKey");
            jsonBody.append("\"");
            jsonBody.append(":");
            
            jsonBody.append("\"");
            jsonBody.append(this.symmetricKey.getSecondaryKey());
            jsonBody.append("\"");
            
            jsonBody.append("}");
            jsonBody.append("}");
        }
        jsonBody.append("}");
        return jsonBody.toString();
    }
}
