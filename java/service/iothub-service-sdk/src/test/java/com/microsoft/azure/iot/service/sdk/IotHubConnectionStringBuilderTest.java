/*
 * Copyright (c) Microsoft. All rights reserved.
 * Licensed under the MIT license. See LICENSE file in the project root for full license information.
 */

package com.microsoft.azure.iot.service.sdk;

import mockit.integration.junit4.JMockit;
import org.junit.Assert;
import org.junit.Test;
import org.junit.runner.RunWith;
import java.net.URI;

@RunWith(JMockit.class)
public class IotHubConnectionStringBuilderTest
{
    @Test (expected = Exception.class)
    public void createChecksInputParameter() throws Exception
    {
        IotHubConnectionString iotHubConnectionString = IotHubConnectionStringBuilder.createConnectionString(null);
    }

    @Test (expected = Exception.class)
    public void createParsesKeysMissing() throws Exception
    {
        String connectionString = "a=A;b=B;HostName=";

        IotHubConnectionString iotHubConnectionString = IotHubConnectionStringBuilder.createConnectionString(connectionString);
    }

    @Test
    public void createParsesSimpleHostName() throws Exception
    {
        String iotHubName = "";
        String hostName = "HOSTNAME" + iotHubName;
        String sharedAccessKeyName = "ACCESSKEYNAME";
        String policyName = "SharedAccessKey";
        String sharedAccessKey = "1234567890abcdefghijklmnopqrstvwxyz=";
        String connectionString = "HostName=" + hostName + ";SharedAccessKeyName=" + sharedAccessKeyName + ";" + policyName + "=" + sharedAccessKey;
        String userString = sharedAccessKeyName +  "@sas.root." + iotHubName;
        URI httpsUri = URI.create("");
        URI amqpUri = URI.create("");

        IotHubConnectionString iotHubConnectionString = IotHubConnectionStringBuilder.createConnectionString(connectionString);

        Assert.assertEquals("Parser error: HostName mismatch!", hostName, iotHubConnectionString.getHostName());
        Assert.assertEquals("Parser error: SharedAccessKeyName mismatch!", sharedAccessKeyName, iotHubConnectionString.getSharedAccessKeyName());
        Assert.assertEquals("Parser error: SharedAccessKey mismatch!", iotHubConnectionString.getSharedAccessKey(), sharedAccessKey);
        Assert.assertEquals("Parser error: SharedAccessSignature mismatch!", "", iotHubConnectionString.getSharedAccessSignature());
        Assert.assertEquals("Parser error: IotHubName mismatch!", iotHubName, iotHubConnectionString.getIotHubName());
        Assert.assertEquals("Parser error: UserString mismatch!", userString, iotHubConnectionString.getUserString());
    }

    @Test (expected = Exception.class)
    public void createParsesInvalidHostName() throws Exception
    {
        String iotHubName = "b.c.d";
        String hostName = "@" + iotHubName;
        String sharedAccessKeyName = "ACCESSKEYNAME";
        String policyName = "SharedAccessKey";
        String sharedAccessKey = "1234567890abcdefghijklmnopqrstvwxyz=";
        String connectionString = "HostName=" + hostName + ";SharedAccessKeyName=" + sharedAccessKeyName + ";" + policyName + "=" + sharedAccessKey;
        String userString = sharedAccessKeyName +  "@sas.root." + iotHubName;
        URI httpsUri = URI.create("");
        URI amqpUri = URI.create("");

        IotHubConnectionString iotHubConnectionString = IotHubConnectionStringBuilder.createConnectionString(connectionString);
    }

    @Test (expected = Exception.class)
    public void createValidatesMissingIotHubName() throws Exception
    {
        String iotHubName = "";
        String hostName = "" + iotHubName;
        String sharedAccessKeyName = "XXX";
        String policyName = "SharedAccessKey";
        String sharedAccessKey = "1234567890abcdefghijklmnopqrstvwxyz=";
        String sharedAccessSignature = "";
        String connectionString = "HostName=" + hostName + ";SharedAccessKeyName=" + sharedAccessKeyName + ";" + policyName + "=" + sharedAccessKey + "SharedAccessSignature=" + sharedAccessSignature;

        IotHubConnectionString iotHubConnectionString = IotHubConnectionStringBuilder.createConnectionString(connectionString);
    }

    @Test (expected = Exception.class)
    public void createValidatesWrongSharedAccessKeyAndSignature() throws Exception
    {
        String iotHubName = "b.c.d";
        String hostName = "HOSTNAME." + iotHubName;
        String sharedAccessKeyName = "XXX";
        String policyName = "YYY";
        String sharedAccessKey = "";
        String connectionString = "HostName=" + hostName + ";SharedAccessKeyName=" + sharedAccessKeyName + ";" + policyName + "=" + sharedAccessKey;

        IotHubConnectionString iotHubConnectionString = IotHubConnectionStringBuilder.createConnectionString(connectionString);
    }

    @Test
    public void createParsesGoodCase() throws Exception
    {
        String iotHubName = "IOTHUBNAME";
        String iotHostName = "HOSTNAME";
        String sharedAccessKeyName = "ACCESSKEYNAME";
        String policyName = "SharedAccessKey";
        String sharedAccessKey = "1234567890abcdefghijklmnopqrstvwxyz=";
        String connectionString = "HostName=" + iotHubName + "." + iotHostName + ";SharedAccessKeyName=" + sharedAccessKeyName + ";" + policyName + "=" + sharedAccessKey;
        String userString = sharedAccessKeyName +  "@sas.root." + iotHubName;
        URI httpsUri = URI.create("");
        URI amqpUri = URI.create("");

        IotHubConnectionString iotHubConnectionString = IotHubConnectionStringBuilder.createConnectionString(connectionString);

        Assert.assertEquals("Parser error: HostName mismatch!", iotHubName + "." + iotHostName, iotHubConnectionString.getHostName());
        Assert.assertEquals("Parser error: SharedAccessKeyName mismatch!", sharedAccessKeyName, iotHubConnectionString.getSharedAccessKeyName());
        Assert.assertEquals("Parser error: SharedAccessKey mismatch!", iotHubConnectionString.getSharedAccessKey(), sharedAccessKey);
        Assert.assertEquals("Parser error: SharedAccessSignature mismatch!", "", iotHubConnectionString.getSharedAccessSignature());
        Assert.assertEquals("Parser error: IotHubName mismatch!", iotHubName, iotHubConnectionString.getIotHubName());
        Assert.assertEquals("Parser error: UserString mismatch!", userString, iotHubConnectionString.getUserString());
    }

    @Test (expected = Exception.class)
    public void createConnectionStringChecksInputParameterHostNameNull() throws Exception
    {
        AuthenticationMethod authenticationMethod = new ServiceAuthenticationWithSharedAccessPolicyKey("", "");
        IotHubConnectionString iotHubConnectionString = IotHubConnectionStringBuilder.createConnectionString(null, authenticationMethod);
    }

    @Test (expected = Exception.class)
    public void createConnectionStringChecksInputParameterHostNameEmpty() throws Exception
    {
        AuthenticationMethod authenticationMethod = new ServiceAuthenticationWithSharedAccessPolicyKey("", "");
        IotHubConnectionString iotHubConnectionString = IotHubConnectionStringBuilder.createConnectionString("", authenticationMethod);
    }

    @Test (expected = Exception.class)
    public void createConnectionStringChecksInputParameterAuthenticationMethodNull() throws Exception
    {
        IotHubConnectionString iotHubConnectionString = IotHubConnectionStringBuilder.createConnectionString("test", null);
    }

    @Test
    public void createConnectionStringParsesGoodCase() throws Exception
    {
        String iotHubName = "IOTHUBNAME";
        String iotHostName = "HOSTNAME";
        String policyName = "SharedAccessKey";
        String sharedAccessKey = "1234567890abcdefghijklmnopqrstvwxyz=";
        String userString = "SharedAccessKey@sas.root." + iotHubName;

        ServiceAuthenticationWithSharedAccessPolicyKey serviceAuthenticationWithSharedAccessPolicyKey =
                new ServiceAuthenticationWithSharedAccessPolicyKey(policyName, sharedAccessKey);

        IotHubConnectionString iotHubConnectionString = IotHubConnectionStringBuilder.createConnectionString(iotHubName + "." + iotHostName, serviceAuthenticationWithSharedAccessPolicyKey);

        Assert.assertEquals("Parser error: HostName mismatch!", iotHubName + "." + iotHostName, iotHubConnectionString.getHostName());
        Assert.assertEquals("Parser error: SharedAccessKeyName mismatch!", policyName, iotHubConnectionString.getSharedAccessKeyName());
        Assert.assertEquals("Parser error: SharedAccessKey mismatch!", iotHubConnectionString.getSharedAccessKey(), sharedAccessKey);
        Assert.assertEquals("Parser error: SharedAccessSignature mismatch!", null, iotHubConnectionString.getSharedAccessSignature());
        Assert.assertEquals("Parser error: IotHubName mismatch!", iotHubName, iotHubConnectionString.getIotHubName());
        Assert.assertEquals("Parser error: UserString mismatch!", userString, iotHubConnectionString.getUserString());
    }

    @Test
    public void createConnectionStringtoString() throws Exception
    {
        String iotHubName = "b.c.d";
        String hostName = "HOSTNAME." + iotHubName;
        String policyName = "SharedAccessKey";
        String sharedAccessKey = "1234567890abcdefghijklmnopqrstvwxyz=";
        String expectedStr = "HostName=HOSTNAME.b.c.d;SharedAccessKeyName=SharedAccessKey;SharedAccessKey=1234567890abcdefghijklmnopqrstvwxyz=;SharedAccessSignature=null";

        ServiceAuthenticationWithSharedAccessPolicyKey serviceAuthenticationWithSharedAccessPolicyKey =
                new ServiceAuthenticationWithSharedAccessPolicyKey(policyName, sharedAccessKey);

        IotHubConnectionString iotHubConnectionString = IotHubConnectionStringBuilder.createConnectionString(hostName, serviceAuthenticationWithSharedAccessPolicyKey);

        Assert.assertEquals("toString mismatch!", expectedStr, iotHubConnectionString.toString());
    }

    @Test
    public void createConnectionStringBuilderExample1() throws Exception
    {
        ServiceAuthenticationWithSharedAccessPolicyKey auth = new ServiceAuthenticationWithSharedAccessPolicyKey("myPolicy", "<key>");

        IotHubConnectionString iotHubConnectionString = IotHubConnectionStringBuilder.createConnectionString("hostname", auth);
        String connString = iotHubConnectionString.toString();
        Assert.assertEquals("Connection string mismatch!", "HostName=hostname;SharedAccessKeyName=myPolicy;SharedAccessKey=<key>;SharedAccessSignature=null", connString);
    }

    @Test
    public void createConnectionStringBuilderExample2() throws Exception
    {
        ServiceAuthenticationWithSharedAccessPolicyToken auth = new ServiceAuthenticationWithSharedAccessPolicyToken("myPolicy", "<key>");

        IotHubConnectionString iotHubConnectionString = IotHubConnectionStringBuilder.createConnectionString("hostname", auth);
        String connString = iotHubConnectionString.toString();
        Assert.assertEquals("Connection string mismatch!", "HostName=hostname;SharedAccessKeyName=myPolicy;SharedAccessKey=null;SharedAccessSignature=<key>", connString);
    }
}