/*
 * Copyright (c) Microsoft. All rights reserved.
 * Licensed under the MIT license. See LICENSE file in the project root for full license information.
 */

package com.microsoft.azure.iot.service.sdk;

import mockit.integration.junit4.JMockit;
import org.junit.Assert;
import org.junit.Test;
import org.junit.runner.RunWith;
import java.net.URL;

@RunWith(JMockit.class)
public class IotHubConnectionStringTest
{
    private static final String URL_API_VERSION = "api-version=2015-08-15-preview";
    
    @Test
    public void getIotHubNameTest() throws Exception
    {
        String iotHubName = "b.c.d";
        String hostName = "HOSTNAME." + iotHubName;
        String sharedAccessKeyName = "ACCESSKEYNAME";
        String policyName = "SharedAccessKey";
        String sharedAccessKey = "1234567890abcdefghijklmnopqrstvwxyz=";
        String connectionString = "HostName=" + hostName + ";SharedAccessKeyName=" + sharedAccessKeyName + ";" + policyName + "=" + sharedAccessKey;
        String userString = sharedAccessKeyName +  "@sas.root.HOSTNAME";

        IotHubConnectionString iotHubConnectionString = IotHubConnectionStringBuilder.createConnectionString(connectionString);

        Assert.assertEquals("IotHubName mismatch!", hostName, iotHubConnectionString.getHostName());
        Assert.assertNotNull("AuthenticationMethod null!", iotHubConnectionString.getAuthenticationMethod());

        Assert.assertEquals("SharedAccessKeyName mismatch!", sharedAccessKeyName, iotHubConnectionString.getSharedAccessKeyName());
        Assert.assertEquals("SharedAccessKey mismatch!", sharedAccessKey, iotHubConnectionString.getSharedAccessKey());
        Assert.assertEquals("SharedAccessSignature mismatch!", "", iotHubConnectionString.getSharedAccessSignature());
        Assert.assertEquals("UserString mismatch!", userString, iotHubConnectionString.getUserString());
        Assert.assertEquals("HostName mismatch!", hostName, iotHubConnectionString.getHostName());
    }

    @Test
    public void getUrlDeviceDeviceIdEmptyTest() throws Exception
    {
        String iotHubName = "b.c.d";
        String hostName = "HOSTNAME." + iotHubName;
        String sharedAccessKeyName = "ACCESSKEYNAME";
        String policyName = "SharedAccessKey";
        String sharedAccessKey = "1234567890abcdefghijklmnopqrstvwxyz=";
        String connectionString = "HostName=" + hostName + ";SharedAccessKeyName=" + sharedAccessKeyName + ";" + policyName + "=" + sharedAccessKey;
        String deviceName = "";

        IotHubConnectionString iotHubConnectionString = IotHubConnectionStringBuilder.createConnectionString(connectionString);

        Assert.assertEquals("Device URL mismatch!", null, iotHubConnectionString.getUrlDevice(deviceName));
    }


    @Test
    public void getUrlDeviceDeviceIdNullTest() throws Exception
    {
        String iotHubName = "b.c.d";
        String hostName = "HOSTNAME." + iotHubName;
        String sharedAccessKeyName = "ACCESSKEYNAME";
        String policyName = "SharedAccessKey";
        String sharedAccessKey = "1234567890abcdefghijklmnopqrstvwxyz=";
        String connectionString = "HostName=" + hostName + ";SharedAccessKeyName=" + sharedAccessKeyName + ";" + policyName + "=" + sharedAccessKey;

        IotHubConnectionString iotHubConnectionString = IotHubConnectionStringBuilder.createConnectionString(connectionString);

        Assert.assertEquals("Device URL mismatch!", null, iotHubConnectionString.getUrlDevice(null));
    }

    @Test
    public void getUrlDeviceTest() throws Exception
    {
        String iotHubName = "b.c.d";
        String hostName = "HOSTNAME." + iotHubName;
        String sharedAccessKeyName = "ACCESSKEYNAME";
        String policyName = "SharedAccessKey";
        String sharedAccessKey = "1234567890abcdefghijklmnopqrstvwxyz=";
        String connectionString = "HostName=" + hostName + ";SharedAccessKeyName=" + sharedAccessKeyName + ";" + policyName + "=" + sharedAccessKey;
        String deviceName = "DeviceName";
        URL expectedUrl = new URL("https://HOSTNAME.b.c.d/devices/DeviceName?" + URL_API_VERSION);

        IotHubConnectionString iotHubConnectionString = IotHubConnectionStringBuilder.createConnectionString(connectionString);

        Assert.assertEquals("Device URL mismatch!", expectedUrl, iotHubConnectionString.getUrlDevice(deviceName));
    }

    @Test
    public void getUrlDeviceListTest() throws Exception
    {
        String iotHubName = "b.c.d";
        String hostName = "HOSTNAME." + iotHubName;
        String sharedAccessKeyName = "ACCESSKEYNAME";
        String policyName = "SharedAccessKey";
        String sharedAccessKey = "1234567890abcdefghijklmnopqrstvwxyz=";
        String connectionString = "HostName=" + hostName + ";SharedAccessKeyName=" + sharedAccessKeyName + ";" + policyName + "=" + sharedAccessKey;
        Integer maxCount = 10;
        URL expectedUrl = new URL("https://HOSTNAME.b.c.d/devices/?maxCount=" + maxCount.toString() + "&" + URL_API_VERSION);

        IotHubConnectionString iotHubConnectionString = IotHubConnectionStringBuilder.createConnectionString(connectionString);

        Assert.assertEquals("Device URL list mismatch!", expectedUrl, iotHubConnectionString.getUrlDeviceList(maxCount));
    }
    

    @Test
    public void getUrlDeviceStatisticsTest() throws Exception
    {
        String iotHubName = "b.c.d";
        String hostName = "HOSTNAME." + iotHubName;
        String sharedAccessKeyName = "ACCESSKEYNAME";
        String policyName = "SharedAccessKey";
        String sharedAccessKey = "1234567890abcdefghijklmnopqrstvwxyz=";
        String connectionString = "HostName=" + hostName + ";SharedAccessKeyName=" + sharedAccessKeyName + ";" + policyName + "=" + sharedAccessKey;
        Integer maxCount = 10;
        URL expectedUrl = new URL("https://HOSTNAME.b.c.d/statistics/devices?" + URL_API_VERSION);

        IotHubConnectionString iotHubConnectionString = IotHubConnectionStringBuilder.createConnectionString(connectionString);
        
        URL s = iotHubConnectionString.getUrlDeviceStatistics();

        Assert.assertEquals("DeviceStatistics URL mismatch!", expectedUrl, s);
    }
    
    
}
