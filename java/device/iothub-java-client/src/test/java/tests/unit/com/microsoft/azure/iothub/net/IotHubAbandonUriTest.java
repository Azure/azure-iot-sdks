// Copyright (c) Microsoft. All rights reserved.
// Licensed under the MIT license. See LICENSE file in the project root for full license information.

package tests.unit.com.microsoft.azure.iothub.net;

import static org.hamcrest.CoreMatchers.is;
import static org.junit.Assert.assertThat;

import com.microsoft.azure.iothub.net.IotHubAbandonUri;
import com.microsoft.azure.iothub.net.IotHubUri;

import mockit.Mocked;
import mockit.NonStrictExpectations;
import mockit.Verifications;
import org.junit.Test;

import java.net.URISyntaxException;

/** Unit tests for IotHubAbandonUri. */
public class IotHubAbandonUriTest
{
    @Mocked IotHubUri mockIotHubUri;

    /** The e-tag will be interpolated where the '%s' is placed. */
    protected static String ABANDON_PATH_FORMAT =
            "/messages/devicebound/%s/abandon";

    // Tests_SRS_IOTHUBABANDONURI_11_001: [The constructor returns a URI with the format "[iotHubHostname]/devices/[deviceId]/messages/devicebound/[eTag]/abandon?api-version=2016-02-03".]
    @Test
    public void constructorConstructsIotHubUriCorrectly()
            throws URISyntaxException
    {
        final String iotHubHostname = "test.iothub";
        final String deviceId = "test-deviceid";
        final String eTag = "test-etag";
        final String abandonPath = String.format(ABANDON_PATH_FORMAT, eTag);

        new IotHubAbandonUri(iotHubHostname, deviceId, eTag);

        new Verifications()
        {
            {
                new IotHubUri(iotHubHostname, deviceId, abandonPath);
            }
        };
    }

    // Tests_SRS_IOTHUBABANDONURI_11_002: [The string representation of the IoT Hub event URI shall be constructed with the format "[iotHubHostname]/devices/[deviceId]/messages/devicebound/[eTag]/abandon?api-version=2016-02-03".]
    @Test
    public void toStringIsCorrect() throws URISyntaxException
    {
        final String iotHubHostname = "test.iothub";
        final String deviceId = "test-deviceid";
        final String eTag = "test-etag";
        final String uriStr = "test-uri-str";
        new NonStrictExpectations()
        {
            {
                mockIotHubUri.toString();
                result = uriStr;
            }
        };
        IotHubAbandonUri abandonUri =
                new IotHubAbandonUri(iotHubHostname, deviceId, eTag);

        String testUriStr = abandonUri.toString();

        final String expectedUriStr = uriStr;
        assertThat(testUriStr, is(expectedUriStr));
    }

    // Tests_SRS_IOTHUBABANDONURI_11_003: [The function shall return the hostname given in the constructor.] 
    @Test
    public void getHostnameIsCorrect() throws URISyntaxException
    {
        final String iotHubHostname = "test.iothub";
        final String deviceId = "test-deviceid";
        final String eTag = "test-etag";
        final String hostname = "test-hostname";
        new NonStrictExpectations()
        {
            {
                mockIotHubUri.getHostname();
                result = hostname;
            }
        };
        IotHubAbandonUri abandonUri =
                new IotHubAbandonUri(iotHubHostname, deviceId, eTag);

        String testHostname = abandonUri.getHostname();

        final String expectedHostname = hostname;
        assertThat(testHostname, is(expectedHostname));
    }

    // Tests_SRS_IOTHUBABANDONURI_11_004: [The function shall return a URI with the format '/devices/[deviceId]/messages/devicebound/[eTag]/abandon'.]
    @Test
    public void getPathIsCorrect() throws URISyntaxException
    {
        final String iotHubHostname = "test.iothub";
        final String deviceId = "test-deviceid";
        final String eTag = "test-etag";
        final String path = "test-path";
        new NonStrictExpectations()
        {
            {
                mockIotHubUri.getPath();
                result = path;
            }
        };
        IotHubAbandonUri abandonUri =
                new IotHubAbandonUri(iotHubHostname, deviceId, eTag);

        String testPath = abandonUri.getPath();

        final String expectedPath = path;
        assertThat(testPath, is(expectedPath));
    }
}
