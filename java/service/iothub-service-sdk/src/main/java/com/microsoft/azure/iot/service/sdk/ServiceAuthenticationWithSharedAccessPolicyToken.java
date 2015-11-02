/*
 * Copyright (c) Microsoft. All rights reserved.
 * Licensed under the MIT license. See LICENSE file in the project root for full license information.
 */

package com.microsoft.azure.iot.service.sdk;

public class ServiceAuthenticationWithSharedAccessPolicyToken extends AuthenticationMethod
{
    public ServiceAuthenticationWithSharedAccessPolicyToken(String policyName, String token)
    {
        super();
        this.setPolicyName(policyName);
        this.setToken(token);
    }

    public String getPolicyName()
    {
        return this.policyName;
    }

    private void setPolicyName(String policyName)
    {
        this.policyName = policyName;
    }

    public String getToken()
    {
        return this.token;
    }

    private void setToken(String token)
    {
        this.token = token;
    }

    @Override
    protected IotHubConnectionString populate(IotHubConnectionString iotHubConnectionString)
    {
        if (iotHubConnectionString == null)
        {
            throw new IllegalArgumentException("Input parameter \"iotHubConnectionStringBuilder\" is null");
        }

        iotHubConnectionString.setSharedAccessKeyName(this.policyName);
        iotHubConnectionString.setSharedAccessKey(null);
        iotHubConnectionString.setSharedAccessSignature(this.token);

        return iotHubConnectionString;
    }
}
