// Copyright (c) Microsoft. All rights reserved.
// Licensed under the MIT license. See LICENSE file in the project root for full license information.

package tests.unit.com.microsoft.azure.iothub.net;

import static org.hamcrest.CoreMatchers.is;
import static org.junit.Assert.assertThat;

import com.microsoft.azure.iothub.net.IotHubEventUri;
import com.microsoft.azure.iothub.net.IotHubUri;

import mockit.Mocked;
import mockit.NonStrictExpectations;
import mockit.Verifications;
import org.junit.Test;

import java.net.URISyntaxException;

/** Unit tests for IotHubEventUri. */
public class IotHubEventUriTest
{
    protected static String EVENT_PATH = "/messages/events";

    @Mocked IotHubUri mockIotHubUri;

    // Tests_SRS_IOTHUBEVENTURI_11_001: [The constructor returns a URI with the format "[iotHubHostname]/devices/[deviceId]/messages/events?api-version=2016-02-03".]
    @Test
    public void constructorConstructsIotHubUriCorrectly()
            throws URISyntaxException
    {
        final String iotHubHostname = "test.iothub";
        final String deviceId = "test-deviceid";

        new IotHubEventUri(iotHubHostname, deviceId);

        new Verifications()
        {
            {
                new IotHubUri(iotHubHostname, deviceId, EVENT_PATH);
            }
        };
    }

    // Tests_SRS_IOTHUBEVENTURI_11_002: [The string representation of the IoT Hub event URI shall be constructed with the format '[iotHubHostname]/devices/[deviceId]/messages/events?api-version=2016-02-03 '.]
    @Test
    public void toStringIsCorrect() throws URISyntaxException
    {
        final String iotHubHostname = "test.iothub";
        final String deviceId = "test-deviceid";
        final String uriStr = "test-uri-str";
        new NonStrictExpectations()
        {
            {
                mockIotHubUri.toString();
                result = uriStr;
            }
        };
        IotHubEventUri eventUri =
                new IotHubEventUri(iotHubHostname, deviceId);

        String testUriStr = eventUri.toString();

        final String expectedUriStr = uriStr;
        assertThat(testUriStr, is(expectedUriStr));
    }

    // Tests_SRS_IOTHUBEVENTURI_11_003: [The function shall return the hostname given in the constructor.]
    @Test
    public void getHostnameIsCorrect() throws URISyntaxException
    {
        final String iotHubHostname = "test.iothub";
        final String deviceId = "test-deviceid";
        final String hostname = "test-hostname";
        new NonStrictExpectations()
        {
            {
                mockIotHubUri.getHostname();
                result = hostname;
            }
        };
        IotHubEventUri eventUri =
                new IotHubEventUri(iotHubHostname, deviceId);

        String testHostname = eventUri.getHostname();

        final String expectedHostname = hostname;
        assertThat(testHostname, is(expectedHostname));
    }

    // Tests_SRS_IOTHUBEVENTURI_11_004: [The function shall return a URI with the format '/devices/[deviceId]/messages/events'.]
    @Test
    public void getPathIsCorrect() throws URISyntaxException
    {
        final String iotHubHostname = "test.iothub";
        final String deviceId = "test-deviceid";
        final String path = "test-path";
        new NonStrictExpectations()
        {
            {
                mockIotHubUri.getPath();
                result = path;
            }
        };
        IotHubEventUri eventUri =
                new IotHubEventUri(iotHubHostname, deviceId);

        String testPath = eventUri.getPath();

        final String expectedPath = path;
        assertThat(testPath, is(expectedPath));
    }
}
