/*
 * Copyright (c) Microsoft. All rights reserved.
 * Licensed under the MIT license. See LICENSE file in the project root for full license information.
 */

package com.microsoft.azure.iot.service.sdk;

/** 
 * Class used to abstract the different 
 * authentication methods used to use IoT Hub SDK. 
 * 
 */
public abstract class AuthenticationMethod
{
    /**
     * Authentication policy name
     */
    protected String policyName;

    /**
     * Shared access policy key (if applies)
     */
    protected String key;

    /**
     * Shared access policy token (if applies)
     */
    protected String token;

    /**
     * Abstract helper function to populate {@code IotHubConnectionString} class with
     * proper authentication properties
     * 
     * @param iotHubConnectionStringBuilder  The {@link IotHubConnectionString} that is
     *                                       to be populated with the authentication
     *                                       properties.
     * @return An instance of {@link IotHubConnectionString} populated with the
     * the authentication properties.
     */
    protected abstract IotHubConnectionString populate(IotHubConnectionString iotHubConnectionStringBuilder);
}
