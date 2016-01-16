/*
 * Copyright (c) Microsoft. All rights reserved.
 * Licensed under the MIT license. See LICENSE file in the project root for full license information.
 */

package com.microsoft.azure.iot.service.sdk;

import mockit.integration.junit4.JMockit;
import org.junit.Test;
import org.junit.runner.RunWith;

import static org.junit.Assert.assertEquals;

@RunWith(JMockit.class)
public class ServiceAuthenticationWithSharedAccessPolicyTokenTest
{
    // Tests_SRS_SERVICE_SDK_JAVA_SERVICEAUTHENTICATIONWITHSHAREDACCESSTOKEN_12_002: [The function shall throw IllegalArgumentException if the input object is null]
    // Assert
    @Test (expected = IllegalArgumentException.class)
    public void populate_input_null() throws Exception
    {
        // Arrange
        String newPolicyName = "XXX";
        String newPolicyToken = "YYY";
        ServiceAuthenticationWithSharedAccessPolicyToken auth = new ServiceAuthenticationWithSharedAccessPolicyToken(newPolicyName, newPolicyToken);
        // Act
        auth.populate(null);
    }

    // Tests_SRS_SERVICE_SDK_JAVA_SERVICEAUTHENTICATIONWITHSHAREDACCESSTOKEN_12_003: [The function shall save the policyName and token to the target object]
    // Tests_SRS_SERVICE_SDK_JAVA_SERVICEAUTHENTICATIONWITHSHAREDACCESSTOKEN_12_004: [The function shall set the access key to null]
    @Test
    public void populate_good_case() throws Exception
    {
        // Arrange
        String regex = "[a-zA-Z0-9_\\-\\.]+$";
        String iotHubName = "b.c.d";
        String hostName = "HOSTNAME." + iotHubName;
        String sharedAccessKeyName = "ACCESSKEYNAME";
        String policyName = "SharedAccessKey";
        String sharedAccessKey = "1234567890abcdefghijklmnopqrstvwxyz=";
        String connectionString = "HostName=" + hostName + ";SharedAccessKeyName=" + sharedAccessKeyName + ";" + policyName + "=" + sharedAccessKey;
        IotHubConnectionString iotHubConnectionString = IotHubConnectionStringBuilder.createConnectionString(connectionString);
        String newPolicyName = "XXX";
        String newPolicyToken = "YYY";
        ServiceAuthenticationWithSharedAccessPolicyToken auth = new ServiceAuthenticationWithSharedAccessPolicyToken(newPolicyName, newPolicyToken);
        // Act
        auth.populate(iotHubConnectionString);
        // Assert
        assertEquals(newPolicyName, iotHubConnectionString.getSharedAccessKeyName());
        assertEquals(newPolicyToken, iotHubConnectionString.getSharedAccessSignature());
        assertEquals(null, iotHubConnectionString.getSharedAccessKey());
        // Act
        auth.setPolicyName(policyName);
        auth.setToken(sharedAccessKey);
        // Assert
        assertEquals(policyName, auth.getPolicyName());
        assertEquals(sharedAccessKey, auth.getToken());
    }

    // Tests_SRS_SERVICE_SDK_JAVA_SERVICEAUTHENTICATIONWITHSHAREDACCESSTOKEN_12_001: [Provide access to the following properties: policyName, token]
    @Test
    public void propertyAccess()
    {
        // Arrange
        String policyName = "accessKeyPolicy";
        String token = "11223344556677889900";
        // Act
        ServiceAuthenticationWithSharedAccessPolicyToken auth = new ServiceAuthenticationWithSharedAccessPolicyToken(policyName, token);
        // Assert
        assertEquals(policyName, auth.getPolicyName());
        assertEquals(token, auth.getToken());
    }

}

