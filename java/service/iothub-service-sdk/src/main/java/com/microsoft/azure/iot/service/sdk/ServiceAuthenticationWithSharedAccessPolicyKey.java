/*
 * Copyright (c) Microsoft. All rights reserved.
 * Licensed under the MIT license. See LICENSE file in the project root for full license information.
 */

package com.microsoft.azure.iot.service.sdk;

public class ServiceAuthenticationWithSharedAccessPolicyKey extends AuthenticationMethod
{
    public ServiceAuthenticationWithSharedAccessPolicyKey(String policyName, String key)
    {
        super();
        this.setPolicyName(policyName);
        this.setKey(key);
    }

    public String getPolicyName()
    {
        return this.policyName;
    }

    private void setPolicyName(String policyName)
    {
        this.policyName = policyName;
    }

    public String getKey()
    {
        return this.key;
    }

    private void setKey(String key)
    {
        this.key = key;
    }

    @Override
    protected IotHubConnectionString populate(IotHubConnectionString iotHubConnectionString)
    {
        if (iotHubConnectionString == null)
        {
            throw new IllegalArgumentException("Input parameter \"iotHubConnectionStringBuilder\" is null");
        }

        iotHubConnectionString.setSharedAccessKeyName(this.policyName);
        iotHubConnectionString.setSharedAccessKey(this.key);
        iotHubConnectionString.setSharedAccessSignature(null);

        return iotHubConnectionString;
    }
}
