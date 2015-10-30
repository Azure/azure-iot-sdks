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
public class ServiceAuthenticationWithSharedAccessPolicyKeyTest
{
    @Test (expected = Exception.class)
    public void populateChecksArgument() throws Exception
    {
        class TestAuth extends ServiceAuthenticationWithSharedAccessPolicyKey
        {
            public TestAuth(String policyName, String key)
            {
                super(policyName, key);
            }

            public void populateTest()
            {
                this.populate(null);
            }
        }
        TestAuth testAuth = new TestAuth("", "");
        testAuth.populateTest();
    }

    @Test
    public void populateTest() throws Exception
    {
        class TestAuth extends ServiceAuthenticationWithSharedAccessPolicyKey
        {
            public TestAuth(String policyName, String key)
            {
                super(policyName, key);
            }

            public IotHubConnectionString populateTest(IotHubConnectionString iotHubConnectionString)
            {
                return this.populate(iotHubConnectionString);
            }
        }

        String deviceID = "Device";
        String policyName = "SharedAccessKey";
        String sharedAccessToken = "1234567890abcdefghijklmnopqrstvwxyz=";

        TestAuth testAuth = new TestAuth(policyName, sharedAccessToken);
        IotHubConnectionString iotHubConnectionString = IotHubConnectionStringBuilder.createConnectionString(deviceID, testAuth);

        Assert.assertEquals("PolicyName mismatch!", policyName, iotHubConnectionString.getSharedAccessKeyName());
    }
}
