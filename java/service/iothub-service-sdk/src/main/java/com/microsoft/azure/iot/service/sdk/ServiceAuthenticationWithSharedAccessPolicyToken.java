/*
 * Copyright (c) Microsoft. All rights reserved.
 * Licensed under the MIT license. See LICENSE file in the project root for full license information.
 */

package com.microsoft.azure.iot.service.sdk;

/**
 * Extend AuthenticationMethod class, provide getters
 * for protected properties and implement populate function to set
 * ServiceAuthenticationWithSharedAccessPolicyToken type policy on
 * given IotHubConnectionString object.
 */
public class ServiceAuthenticationWithSharedAccessPolicyToken extends AuthenticationMethod
{
    /**
     * Populate given IotHubConnectionString with proper
     * policy token authentication data
     *
     * @param iotHubConnectionString The iotHubConnectionString object to populate
     * @return The populated IotHubConnectionString object
     */
    @Override
    protected IotHubConnectionString populate(IotHubConnectionString iotHubConnectionString)
    {
        // Codes_SRS_SERVICE_SDK_JAVA_SERVICEAUTHENTICATIONWITHSHAREDACCESSTOKEN_12_002: [The function shall throw IllegalArgumentException if the input object is null]
        if (iotHubConnectionString == null)
        {
            throw new IllegalArgumentException("Input parameter \"iotHubConnectionStringBuilder\" is null");
        }

        // Codes_SRS_SERVICE_SDK_JAVA_SERVICEAUTHENTICATIONWITHSHAREDACCESSTOKEN_12_003: [The function shall save the policyName and token to the target object]
        iotHubConnectionString.setSharedAccessKeyName(this.policyName);
        iotHubConnectionString.setSharedAccessSignature(this.token);

        // Codes_SRS_SERVICE_SDK_JAVA_SERVICEAUTHENTICATIONWITHSHAREDACCESSTOKEN_12_004: [The function shall set the access key to null]
        iotHubConnectionString.setSharedAccessKey(null);

        return iotHubConnectionString;
    }

    // Codes_SRS_SERVICE_SDK_JAVA_SERVICEAUTHENTICATIONWITHSHAREDACCESSTOKEN_12_001: [Provide access to the following properties: policyName, token]

    /**
     * Constructor to create instance from policy name and policy key
     *
     * @param policyName The policy name string
     * @param token The token string
     */
    public ServiceAuthenticationWithSharedAccessPolicyToken(String policyName, String token)
    {
        this.setPolicyName(policyName);
        this.setToken(token);
    }

    /**
     * Getter for policy name
     * @return The policy name string
     */
    public String getPolicyName()
    {
        return this.policyName;
    }

    /**
     * Setter for policy name
     * @param policyName The string value to set
     */
    protected void setPolicyName(String policyName)
    {
        this.policyName = policyName;
    }

    /**
     * Getter for policy token
     * @return The policy token string
     */
    public String getToken()
    {
        return this.token;
    }

    /**
     * Setter for policy token
     * @param token The string value to set
     */
    protected void setToken(String token)
    {
        this.token = token;
    }

}
