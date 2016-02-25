// Copyright (c) Microsoft. All rights reserved.
// Licensed under the MIT license. See LICENSE file in the project root for full license information.

package tests.unit.com.microsoft.azure.iothub.net;

import static org.hamcrest.CoreMatchers.is;
import static org.junit.Assert.assertThat;

import com.microsoft.azure.iothub.net.IotHubCompleteUri;
import com.microsoft.azure.iothub.net.IotHubUri;

import mockit.Mocked;
import mockit.NonStrictExpectations;
import mockit.Verifications;
import org.junit.Test;

import java.net.URISyntaxException;

/** Unit tests for IotHubCompleteUri. */
public class IotHubCompleteUriTest
{
    /** The e-tag will be interpolated where the '%s' is placed. */
    protected static String COMPLETE_PATH_FORMAT = "/messages/devicebound/%s";

    @Mocked IotHubUri mockIotHubUri;

    // Tests_SRS_IOTHUBCOMPLETEURI_11_001: [The constructor returns a URI with the format "[iotHubHostname]/devices/[deviceId]/messages/devicebound/[eTag]?api-version=2016-02-03".]
    @Test
    public void constructorConstructsIotHubUriCorrectly()
            throws URISyntaxException
    {
        final String iotHubHostname = "test.iothub";
        final String deviceId = "test-deviceid";
        final String eTag = "test-etag";
        final String completePath = String.format(COMPLETE_PATH_FORMAT, eTag);

        new IotHubCompleteUri(iotHubHostname, deviceId, eTag);

        new Verifications()
        {
            {
                new IotHubUri(iotHubHostname, deviceId, completePath);
            }
        };
    }

    // Tests_SRS_IOTHUBCOMPLETEURI_11_002: [The string representation of the IoT Hub event URI shall be constructed with the format "[iotHubHostname]/devices/[deviceId]/messages/devicebound/[eTag]?api-version=2016-02-03".]
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
        IotHubCompleteUri completeUri =
                new IotHubCompleteUri(iotHubHostname, deviceId, eTag);

        String testUriStr = completeUri.toString();

        final String expectedUriStr = uriStr;
        assertThat(testUriStr, is(expectedUriStr));
    }

    // Tests_SRS_IOTHUBCOMPLETEURI_11_003: [The function shall return the hostname given in the constructor.] 
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
        IotHubCompleteUri completeUri =
                new IotHubCompleteUri(iotHubHostname, deviceId, eTag);

        String testHostname = completeUri.getHostname();

        final String expectedHostname = hostname;
        assertThat(testHostname, is(expectedHostname));
    }

    // Tests_SRS_IOTHUBCOMPLETEURI_11_004: [The function shall return a URI with the format '/devices/[deviceId]/messages/devicebound/[eTag].]
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
        IotHubCompleteUri completeUri =
                new IotHubCompleteUri(iotHubHostname, deviceId, eTag);

        String testPath = completeUri.getPath();

        final String expectedPath = path;
        assertThat(testPath, is(expectedPath));
    }
}
