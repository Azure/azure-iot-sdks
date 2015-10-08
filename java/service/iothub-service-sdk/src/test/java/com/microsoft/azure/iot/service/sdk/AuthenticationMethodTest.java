/*
 * Copyright (c) Microsoft. All rights reserved.
 * Licensed under the MIT license. See LICENSE file in the project root for full license information.
 */

package com.microsoft.azure.iot.service.sdk;

import mockit.integration.junit4.JMockit;
import org.junit.Assert;
import org.junit.Test;
import org.junit.runner.RunWith;


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
        String policyName = "SharedAccessKey";
        String sharedAccessKey = "1234567890abcdefghijklmnopqrstvwxyz=";
        ServiceAuthenticationWithSharedAccessPolicyKey serviceAuthenticationWithSharedAccessPolicyKey =
                new ServiceAuthenticationWithSharedAccessPolicyKey(policyName, sharedAccessKey);

        Assert.assertEquals("PolicyName mismatch!", policyName, serviceAuthenticationWithSharedAccessPolicyKey.getPolicyName());
        Assert.assertEquals("SharedAccessKey mismatch!", sharedAccessKey, serviceAuthenticationWithSharedAccessPolicyKey.getKey());
    }

    /**
     * Test of ServiceAuthenticationWithSharedAccessPolicyToken class.
     * Create and get
     */
    @Test
    public void getAuthenticationMethodSharedAccessToken() throws Exception
    {
        String policyName = "SharedAccessKey";
        String sharedAccessToken = "1234567890abcdefghijklmnopqrstvwxyz=";
        ServiceAuthenticationWithSharedAccessPolicyToken serviceAuthenticationWithSharedAccessPolicyToken =
                new ServiceAuthenticationWithSharedAccessPolicyToken(policyName, sharedAccessToken);

        Assert.assertEquals("PolicyName mismatch!", policyName, serviceAuthenticationWithSharedAccessPolicyToken.getPolicyName());
        Assert.assertEquals("SharedAccessToken mismatch!", sharedAccessToken, serviceAuthenticationWithSharedAccessPolicyToken.getToken());
    }
}
