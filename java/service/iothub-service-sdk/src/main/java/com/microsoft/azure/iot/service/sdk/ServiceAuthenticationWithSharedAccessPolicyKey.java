/*
 * Copyright (c) Microsoft. All rights reserved.
 * Licensed under the MIT license. See LICENSE file in the project root for full license information.
 */

package com.microsoft.azure.iot.service.sdk;

/**
 * Extend AuthenticationMethod class, provide getters
 * for protected properties and implement populate function to set
 * ServiceAuthenticationWithSharedAccessPolicyKey type policy on
 * given IotHubConnectionString object.
 */
public class ServiceAuthenticationWithSharedAccessPolicyKey extends AuthenticationMethod
{
    /**
     * Constructor to create instance from policy name and policy key
     *
     * @param policyName The policy name string
     * @param key The policy key string
     */
    public ServiceAuthenticationWithSharedAccessPolicyKey(String policyName, String key)
    {
        this.policyName = policyName;
        this.key = key;
    }

    /**
     * Populate given IotHubConnectionString with proper
     * policy key authentication data
     *
     * @param iotHubConnectionString The iotHubConnectionString object to populate
     * @return The populated IotHubConnectionString object
     */
    @Override
    protected IotHubConnectionString populate(IotHubConnectionString iotHubConnectionString)
    {
        // Codes_SRS_SERVICE_SDK_JAVA_SERVICEAUTHENTICATIONWITHSHAREDACCESSKEY_12_002: [The function shall throw IllegalArgumentException if the input object is null]
        if (iotHubConnectionString == null)
        {
            throw new IllegalArgumentException("Input parameter \"iotHubConnectionStringBuilder\" is null");
        }

        // Codes_SRS_SERVICE_SDK_JAVA_SERVICEAUTHENTICATIONWITHSHAREDACCESSKEY_12_003: [The function shall save the policyName and policyKey to the target object]
        iotHubConnectionString.setSharedAccessKeyName(this.policyName);
        iotHubConnectionString.setSharedAccessKey(this.key);

        // Codes_SRS_SERVICE_SDK_JAVA_SERVICEAUTHENTICATIONWITHSHAREDACCESSKEY_12_004: [The function shall set the access signature (token) to null]
        iotHubConnectionString.setSharedAccessSignature(null);

        return iotHubConnectionString;
    }

    // Codes_SRS_SERVICE_SDK_JAVA_SERVICEAUTHENTICATIONWITHSHAREDACCESSKEY_12_001: [Provide access to the following properties: policyName, key, token]

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
     * Getter for policy key
     * @return The policy key string
     */
    public String getKey()
    {
        return this.key;
    }

    /**
     * Setter for policy key
     * @param key The string value to set
     */
    protected void setKey(String key)
    {
        this.key = key;
    }
}
