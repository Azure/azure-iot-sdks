/*
 * Copyright (c) Microsoft. All rights reserved.
 * Licensed under the MIT license. See LICENSE file in the project root for full license information.
 */

package com.microsoft.azure.iot.service.auth;

import com.microsoft.azure.iot.service.sdk.IotHubConnectionString;
import com.microsoft.azure.iot.service.sdk.IotHubConnectionStringBuilder;
import org.junit.Test;
import static org.junit.Assert.*;

/**
 *
 * @author zolvarga
 */
public class IotHubServiceSasTokenTest
{
    /**
     * Test of toString method, of class IotHubServiceSasToken.
     * Input: null
     * Expected: exception
     */
    @Test (expected = Exception.class)
    public void testToStringNull()
    {
        IotHubServiceSasToken iotHubServiceSasToken = new IotHubServiceSasToken(null);
    }

    /**
     * Test of toString method, of class IotHubServiceSasToken.
     * Input: valid connection string
     * Expected: valid token
     */
    @Test
    public void testToStringGoodCase() throws Exception
    {
        String iotHubName = "b.c.d";
        String hostName = "HOSTNAME." + iotHubName;
        String sharedAccessKeyName = "ACCESSKEYNAME";
        String policyName = "SharedAccessKey";
        String sharedAccessKey = "1234567890abcdefghijklmnopqrstvwxyz=";
        String connectionString = "HostName=" + hostName + ";SharedAccessKeyName=" + sharedAccessKeyName + ";" + policyName + "=" + sharedAccessKey;
        String expectedToken = "SharedAccessSignature sr=hostname.b.c.d&sig=M%2FT5oCM8WWs%2B%2FMv7okAVmfrzVM%2FGUyA7EIp%2FfKo8BeQ%3D&se=1474065852&skn=ACCESSKEYNAME";

        IotHubConnectionString iotHubConnectionString = IotHubConnectionStringBuilder.createConnectionString(connectionString);
        
        IotHubServiceSasToken iotHubServiceSasToken = new IotHubServiceSasToken(iotHubConnectionString);
        String token = iotHubServiceSasToken.toString();
        
        assertEquals(token.contains("SharedAccessSignature sr=hostname.b.c.d&sig="), true);
        assertEquals(token.contains("&se="), true);
        assertEquals(token.contains("&skn=ACCESSKEYNAME"), true);
    }
}
