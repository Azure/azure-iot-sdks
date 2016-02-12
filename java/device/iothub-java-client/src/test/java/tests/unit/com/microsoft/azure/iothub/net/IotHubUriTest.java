// Copyright (c) Microsoft. All rights reserved.
// Licensed under the MIT license. See LICENSE file in the project root for full license information.

package tests.unit.com.microsoft.azure.iothub.net;

import static org.junit.Assert.assertThat;
import static org.hamcrest.CoreMatchers.is;
import static org.hamcrest.CoreMatchers.anyOf;
import static org.hamcrest.CoreMatchers.equalTo;

import com.microsoft.azure.iothub.net.IotHubUri;

import org.junit.Test;

import java.util.HashMap;
import java.util.Map;

/** Unit tests for IotHubUri. */
public class IotHubUriTest
{
    // Tests_SRS_IOTHUBURI_11_007: [The constructor shall return a URI pointing to the address '[iotHubHostname] /devices/[deviceId]/[IoT Hub method path]?api-version=2016-02-03'.]
    // Tests_SRS_IOTHUBURI_11_001: [The string representation of the IoT Hub URI shall be constructed with the format '[iotHubHostname]/devices/[deviceId]/[IoT Hub method path]?api-version=2016-02-03(&[queryFragment]) '.]
    @Test
    public void shortConstructorHasCorrectFormat()
    {
        final String iotHubHostname = "sample.iothubhostname";
        final String deviceId = "sample-deviceid";
        final String iotHubMethodPath = "/sample-path";

        IotHubUri uri = new IotHubUri(iotHubHostname, deviceId, iotHubMethodPath);
        String testUriStr = uri.toString();

        final String expectedUriStr =
                "sample.iothubhostname/devices/sample-deviceid/sample-path?api-version=2016-02-03";
        assertThat(testUriStr, is(expectedUriStr));
    }

    // Tests_SRS_IOTHUBURI_11_015: [The constructor shall URL-encode the device ID.]
    @Test
    public void shortConstructorUrlEncodesDeviceId()
    {
        final String iotHubHostname = "sample.iothubhostname";
        final String deviceId = "?&sample-deviceid";
        final String iotHubMethodPath = "/sample-path";

        IotHubUri uri = new IotHubUri(iotHubHostname, deviceId, iotHubMethodPath);
        String testUriStr = uri.toString();

        final String expectedUriStr =
                "sample.iothubhostname/devices/%3F%26sample-deviceid/sample-path?api-version=2016-02-03";
        assertThat(testUriStr, is(expectedUriStr));
    }

    // Tests_SRS_IOTHUBURI_11_016: [The constructor shall URL-encode the IoT Hub method path.]
    @Test
    public void shortConstructorUrlEncodesIotHubMethodPath()
    {
        final String iotHubHostname = "sample.iothubhostname";
        final String deviceId = "sample-deviceid";
        final String iotHubMethodPath = "/sample-path?";

        IotHubUri uri = new IotHubUri(iotHubHostname, deviceId, iotHubMethodPath);
        String testUriStr = uri.toString();

        final String expectedUriStr =
                "sample.iothubhostname/devices/sample-deviceid/sample-path%3F?api-version=2016-02-03";
        assertThat(testUriStr, is(expectedUriStr));
    }

    // Tests_SRS_IOTHUBURI_11_008: [If queryParams is not empty, the constructor shall return a URI pointing to the address '[iotHubHostname]/devices/[deviceId]/[IoT Hub method path]? api-version=2016-02-03 &[queryFragment] '.]
    @Test
    public void constructorWithQueryFragmentHasCorrectFormat()
    {
        final String iotHubHostname = "sample.iothubhostname";
        final String deviceId = "sample-deviceid";
        final String iotHubMethodPath = "/sample-path";
        final Map<String, String> queryParams = new HashMap<>();
        queryParams.put("test", "true");

        IotHubUri uri = new IotHubUri(iotHubHostname, deviceId, iotHubMethodPath,
                queryParams);
        String testUriStr = uri.toString();

        final String expectedUriStr0 =
                "sample.iothubhostname/devices/sample-deviceid/sample-path?test=true&api-version=2016-02-03";
        final String expectedUriStr1 =
                "sample.iothubhostname/devices/sample-deviceid/sample-path?api-version=2016-02-03&test=true";
        assertThat(testUriStr, is(
                anyOf(equalTo(expectedUriStr0), equalTo(expectedUriStr1))));
    }

    // Tests_SRS_IOTHUBURI_11_009: [If the queryParams is empty, the constructor shall return a URI pointing to the address '[iotHubHostname]/devices/[deviceId]/[IoT Hub method path]?api-version=2016-02-03'.]
    @Test
    public void constructorWithoutQueryFragmentHasCorrectFormat()
    {
        final String iotHubHostname = "sample.iothubhostname";
        final String deviceId = "sample-deviceid";
        final String iotHubMethodPath = "/sample-path";

        IotHubUri uri = new IotHubUri(iotHubHostname, deviceId, iotHubMethodPath,
                null);
        String testUriStr = uri.toString();

        final String expectedUriStr =
                "sample.iothubhostname/devices/sample-deviceid/sample-path?api-version=2016-02-03";
        assertThat(testUriStr, is(expectedUriStr));
    }

    // Tests_SRS_IOTHUBURI_11_011: [The constructor shall URL-encode the device ID.]
    @Test
    public void constructorUrlEncodesDeviceId()
    {
        final String iotHubHostname = "sample.iothubhostname";
        final String deviceId = "?&sample-deviceid";
        final String iotHubMethodPath = "/sample-path";

        IotHubUri uri =
                new IotHubUri(iotHubHostname, deviceId, iotHubMethodPath, null);
        String testUriStr = uri.toString();

        final String expectedUriStr =
                "sample.iothubhostname/devices/%3F%26sample-deviceid/sample-path?api-version=2016-02-03";
        assertThat(testUriStr, is(expectedUriStr));
    }

    // Tests_SRS_IOTHUBURI_11_012: [The constructor shall URL-encode the IoT Hub method path.]
    @Test
    public void constructorUrlEncodesIotHubMethodPath()
    {
        final String iotHubHostname = "sample.iothubhostname";
        final String deviceId = "sample-deviceid";
        final String iotHubMethodPath = "/sample-path?";

        IotHubUri uri =
                new IotHubUri(iotHubHostname, deviceId, iotHubMethodPath, null);
        String testUriStr = uri.toString();

        final String expectedUriStr =
                "sample.iothubhostname/devices/sample-deviceid/sample-path%3F?api-version=2016-02-03";
        assertThat(testUriStr, is(expectedUriStr));
    }

    // Tests_SRS_IOTHUBURI_11_013: [The constructor shall URL-encode each query param.]
    @Test
    public void constructorUrlEncodesQueryParams()
    {
        final String iotHubHostname = "sample.iothubhostname";
        final String deviceId = "sample-deviceid";
        final String iotHubMethodPath = "/sample-path";
        final Map<String, String> queryParams = new HashMap<>();
        queryParams.put("test?", "true?");

        IotHubUri uri = new IotHubUri(iotHubHostname, deviceId, iotHubMethodPath,
                queryParams);
        String testUriStr = uri.toString();

        final String expectedUriStr0 =
                "sample.iothubhostname/devices/sample-deviceid/sample-path?test%3F=true%3F&api-version=2016-02-03";
        final String expectedUriStr1 =
                "sample.iothubhostname/devices/sample-deviceid/sample-path?api-version=2016-02-03&test%3F=true%3F";
        assertThat(testUriStr, is(
                anyOf(equalTo(expectedUriStr0), equalTo(expectedUriStr1))));
    }

    // Tests_SRS_IOTHUBURI_11_002: [The function shall return a URI with the format '[iotHubHostname]/devices/[deviceId]'.]
    @Test
    public void getResourceUriHasCorrectFormat()
    {
        final String iotHubHostname = "sample.iothubhostname";
        final String deviceId = "sample-deviceid";

        String testResourceUri = IotHubUri.getResourceUri(iotHubHostname, deviceId);

        final String expectedResourceUri =
                "sample.iothubhostname/devices/sample-deviceid";
        assertThat(testResourceUri, is(expectedResourceUri));
    }

    // Tests_SRS_IOTHUBURI_11_019: [The constructor shall URL-encode the device ID.]
    @Test
    public void getResourceUriUrlEncodesDeviceId()
    {
        final String iotHubHostname = "sample.iothubhostname";
        final String deviceId = "sample-deviceid?";

        String testResourceUri = IotHubUri.getResourceUri(iotHubHostname, deviceId);

        final String expectedResourceUri =
                "sample.iothubhostname/devices/sample-deviceid%3F";
        assertThat(testResourceUri, is(expectedResourceUri));
    }

    // Tests_SRS_IOTHUBURI_11_005: [The function shall return the IoT hub hostname given in the constructor.]
    @Test
    public void getHostnameReturnsHostname()
    {
        final String iotHubHostname = "sample.iothubhostname";
        final String deviceId = "sample-deviceid";
        final String iotHubMethodPath = "/sample-path";

        IotHubUri uri = new IotHubUri(iotHubHostname, deviceId, iotHubMethodPath);
        String testHostname = uri.getHostname();

        final String expectedHostname =
                "sample.iothubhostname";
        assertThat(testHostname, is(expectedHostname));
    }

    // Tests_SRS_IOTHUBURI_11_006: [The function shall return a URI with the format '/devices/[deviceId]/[IoT Hub method path]'.]
    @Test
    public void getPathReturnsPath()
    {
        final String iotHubHostname = "sample.iothubhostname";
        final String deviceId = "sample-deviceid";
        final String iotHubMethodPath = "/sample-path";

        IotHubUri uri = new IotHubUri(iotHubHostname, deviceId, iotHubMethodPath);
        String testPath = uri.getPath();

        final String expectedPath = "/devices/sample-deviceid/sample-path";
        assertThat(testPath, is(expectedPath));
    }
}
