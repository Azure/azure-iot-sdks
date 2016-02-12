// Copyright (c) Microsoft. All rights reserved.
// Licensed under the MIT license. See LICENSE file in the project root for full license information.

package tests.unit.com.microsoft.azure.iothub.net;

import static org.hamcrest.CoreMatchers.is;
import static org.junit.Assert.assertThat;

import com.microsoft.azure.iothub.net.IotHubRejectUri;
import com.microsoft.azure.iothub.net.IotHubUri;

import mockit.Mocked;
import mockit.NonStrictExpectations;
import mockit.Verifications;
import org.junit.Test;

import java.net.URISyntaxException;
import java.util.Map;

/** Unit tests for IotHubRejectUri. */
public class IotHubRejectUriTest
{
    /** The e-tag will be interpolated where the '%s' is placed. */
    protected static String REJECT_PATH_FORMAT = "/messages/devicebound/%s";

    @Mocked IotHubUri mockIotHubUri;

    // Tests_SRS_IOTHUBREJECTURI_11_001: [The constructor returns a URI with the format "[iotHubHostname]/devices/[deviceId]/messages/devicebound/[eTag]??reject=true&api-version=2016-02-03" (the query parameters can be in any order).]
    @Test
    public void constructorConstructsIotHubUriCorrectly()
            throws URISyntaxException
    {
        final String iotHubHostname = "test.iothub";
        final String deviceId = "test-deviceid";
        final String eTag = "test-etag";
        final String rejectPath = String.format(REJECT_PATH_FORMAT, eTag);

        new IotHubRejectUri(iotHubHostname, deviceId, eTag);

        new Verifications()
        {
            {
                new IotHubUri(iotHubHostname, deviceId, rejectPath,
                        (Map<String, String>) any);
            }
        };
    }

    // Tests_SRS_IOTHUBREJECTURI_11_002: [The string representation of the IoT Hub event URI shall be constructed with the format "[iotHubHostname]/devices/[deviceId]/messages/devicebound/[eTag]??reject=true&api-version=2016-02-03" (the query parameters can be in any order).]
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
        IotHubRejectUri rejectUri =
                new IotHubRejectUri(iotHubHostname, deviceId, eTag);

        String testUriStr = rejectUri.toString();

        final String expectedUriStr = uriStr;
        assertThat(testUriStr, is(expectedUriStr));
    }

    // Tests_SRS_IOTHUBREJECTURI_11_003: [The function shall return the hostname given in the constructor.] 
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
        IotHubRejectUri rejectUri =
                new IotHubRejectUri(iotHubHostname, deviceId, eTag);

        String testHostname = rejectUri.getHostname();

        final String expectedHostname = hostname;
        assertThat(testHostname, is(expectedHostname));
    }

    // Tests_SRS_IOTHUBREJECTURI_11_004: [The function shall return a URI with the format '/devices/[deviceId]/messages/devicebound/[eTag].]
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
        IotHubRejectUri rejectUri =
                new IotHubRejectUri(iotHubHostname, deviceId, eTag);

        String testPath = rejectUri.getPath();

        final String expectedPath = path;
        assertThat(testPath, is(expectedPath));
    }
}
