/*
 * Copyright (c) Microsoft. All rights reserved.
 * Licensed under the MIT license. See LICENSE file in the project root for full license information.
 */

package com.microsoft.azure.iot.service.sdk;

import com.google.gson.annotations.SerializedName;

public class ExportImportDevice
{
    // CODES_SRS_SERVICE_SDK_JAVA_IMPORT_EXPORT_DEVICE_15_001: [The ExportImportDevice class has the following properties: Id,
    // Etag, ImportMode, Status, StatusReason, Authentication]

    @SerializedName("id")
    private String Id;

    @SerializedName("eTag")
    private String ETag;

    @SerializedName("importMode")
    private ImportMode ImportMode;

    @SerializedName("status")
    private DeviceStatus Status;

    @SerializedName("statusReason")
    private String StatusReason;

    @SerializedName("authentication")
    private AuthenticationMechanism Authentication;

    /**
     * Setter for device id.
     * @param id The device id.
     */
    public void setId(String id)
    {
        Id = id;
    }

    /**
     * Getter for device id.
     * @return The device id.
     */
    public String getId()
    {
        return this.Id;
    }

    /**
     * Getter for device eTag.
     * @return The device eTag.
     */
    public String getETag()
    {
        return ETag;
    }

    /**
     * Setter for device eTag.
     * @param ETag The device eTag.
     */
    public void setETag(String ETag)
    {
        this.ETag = ETag;
    }

    /**
     * Getter for device import mode.
     * @return The device import mode.
     */
    public com.microsoft.azure.iot.service.sdk.ImportMode getImportMode()
    {
        return ImportMode;
    }

    /**
     * Setter for device import mode.
     * @param importMode The device import mode.
     */
    public void setImportMode(com.microsoft.azure.iot.service.sdk.ImportMode importMode)
    {
        ImportMode = importMode;
    }

    /**
     * Getter for device status.
     * @return The device status.
     */
    public DeviceStatus getStatus()
    {
        return Status;
    }

    /**
     * Setter for device status.
     * @param status The device status.
     */
    public void setStatus(DeviceStatus status)
    {
        Status = status;
    }

    /**
     * Getter for device status reason.
     * @return The device status reason.
     */
    public String getStatusReason()
    {
        return StatusReason;
    }

    /**
     * Setter for device status reason.
     * @param statusReason The device status reason.
     */
    public void setStatusReason(String statusReason)
    {
        StatusReason = statusReason;
    }

    /**
     * Getter for device authentication mechanism.
     * @return The device authentication mechanism.
     */
    public AuthenticationMechanism getAuthentication()
    {
        return Authentication;
    }

    /**
     * Setter for device authentication mechanism.
     * @param authentication The device authentication mechanism.
     */
    public void setAuthentication(AuthenticationMechanism authentication)
    {
        Authentication = authentication;
    }

    @Override
    public boolean equals(Object other)
    {
        if (other == null)
        {
            return false;
        }

        if (other == this)
        {
            return true;
        }

        if (!(other instanceof ExportImportDevice))
        {
            return false;
        }

        ExportImportDevice otherExportImportDevice = (ExportImportDevice)other;
        if (!this.getId().equals(otherExportImportDevice.getId())
                || !this.getAuthentication().getSymmetricKey().getPrimaryKey().equals(otherExportImportDevice.getAuthentication().getSymmetricKey().getPrimaryKey())
                || !this.getAuthentication().getSymmetricKey().getSecondaryKey().equals(otherExportImportDevice.getAuthentication().getSymmetricKey().getSecondaryKey()))
        {
            return false;
        }

        return true;
    }

    @Override
    public int hashCode()
    {
        int result = Id.hashCode();
        result = 31 * result + Authentication.hashCode();
        return result;
    }
}