/*
 * Copyright (c) Microsoft. All rights reserved.
 * Licensed under the MIT license. See LICENSE file in the project root for full license information.
 */

package com.microsoft.azure.iot.service.sdk;

import com.google.gson.annotations.SerializedName;

public enum ImportMode
{
    @SerializedName("createOrUpdate")
    CreateOrUpdate,

    @SerializedName("create")
    Create,

    @SerializedName("update")
    Update,

    @SerializedName("updateIfMatchETag")
    UpdateIfMatchETag,

    @SerializedName("createOrUpdateIfMatchETag")
    CreateOrUpdateIfMatchETag,

    @SerializedName("delete")
    Delete,

    @SerializedName("deleteIfMatchETag")
    DeleteIfMatchETag
}
