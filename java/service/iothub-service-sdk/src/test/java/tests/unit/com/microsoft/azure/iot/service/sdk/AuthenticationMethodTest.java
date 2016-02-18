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
public class AuthenticationMethodTest
{
    /**
     * Test of getAuthenticationMethodSharedAccessKey class.
     * Create and get
     */
    @Test
    public void ServiceAuthenticationWithSharedAccessPolicyKey() throws Exception
    {
        // Arrange
        String policyName = "SharedAccessKey";
        String sharedAccessKey = "1234567890abcdefghijklmnopqrstvwxyz=";
        // Act
        ServiceAuthenticationWithSharedAccessPolicyKey serviceAuthenticationWithSharedAccessPolicyKey =
                new ServiceAuthenticationWithSharedAccessPolicyKey(policyName, sharedAccessKey);
        // Assert
        assertEquals("PolicyName mismatch!", policyName, serviceAuthenticationWithSharedAccessPolicyKey.getPolicyName());
        assertEquals("SharedAccessKey mismatch!", sharedAccessKey, serviceAuthenticationWithSharedAccessPolicyKey.getKey());
    }

    /**
     * Test of ServiceAuthenticationWithSharedAccessPolicyToken class.
     * Create and get
     */
    @Test
    public void getAuthenticationMethodSharedAccessToken() throws Exception
    {
        // Arrange
        String policyName = "SharedAccessKey";
        String sharedAccessToken = "1234567890abcdefghijklmnopqrstvwxyz=";
        // Act
        ServiceAuthenticationWithSharedAccessPolicyToken serviceAuthenticationWithSharedAccessPolicyToken =
                new ServiceAuthenticationWithSharedAccessPolicyToken(policyName, sharedAccessToken);
        // Assert
        assertEquals("PolicyName mismatch!", policyName, serviceAuthenticationWithSharedAccessPolicyToken.getPolicyName());
        assertEquals("SharedAccessToken mismatch!", sharedAccessToken, serviceAuthenticationWithSharedAccessPolicyToken.getToken());
    }
}
