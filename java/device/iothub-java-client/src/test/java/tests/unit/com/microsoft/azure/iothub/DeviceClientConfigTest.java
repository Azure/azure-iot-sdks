// Copyright (c) Microsoft. All rights reserved.
// Licensed under the MIT license. See LICENSE file in the project root for full license information.

package tests.unit.com.microsoft.azure.iothub;

import static org.hamcrest.CoreMatchers.is;
import static org.junit.Assert.assertThat;

import com.microsoft.azure.iothub.DeviceClientConfig;

import com.microsoft.azure.iothub.MessageCallback;
import mockit.Mocked;
import org.junit.Test;

import java.net.URISyntaxException;

/** Unit tests for IoTHubClientConfig. */
public class DeviceClientConfigTest
{
    // Tests_SRS_DEVICECLIENTCONFIG_11_014: [If the IoT Hub hostname is not a valid URI,
    // the constructor shall throw a URISyntaxException.]
    @Test(expected = URISyntaxException.class)
    public void constructorFailsForInvalidHostname() throws URISyntaxException
    {
        final String illegalIotHubHostname = "test.iothubhostname}{";
        final String deviceId = "test-deviceid";
        final String deviceKey = "test-devicekey";

        new DeviceClientConfig(illegalIotHubHostname, deviceId, deviceKey);
    }

    // Tests_SRS_DEVICECLIENTCONFIG_11_015: [If the IoT Hub hostname does not contain a '.',
    // the function shall throw an IllegalArgumentException.]
    @Test(expected = IllegalArgumentException.class)
    public void constructorFailsForHostnameWithoutIotHubName()
            throws URISyntaxException
    {
        final String illegalIotHubHostname = "badiothubhostname";
        final String deviceId = "test-deviceid";
        final String deviceKey = "test-devicekey";

        new DeviceClientConfig(illegalIotHubHostname, deviceId, deviceKey);
    }

    // Tests_SRS_DEVICECLIENTCONFIG_11_001: [The constructor shall save the IoT Hub hostname,
    // device ID, and device key.]
    // Tests_SRS_DEVICECLIENTCONFIG_11_002: [The function shall return the IoT Hub hostname given in the constructor.]
    @Test
    public void getIotHubHostnameReturnsIotHubHostname()
            throws URISyntaxException
    {
        final String iotHubHostname = "test.iothubhostname";
        final String deviceId = "test-deviceid";
        final String deviceKey = "test-devicekey";

        DeviceClientConfig config = new DeviceClientConfig(iotHubHostname, deviceId, deviceKey);
        String testIotHubHostname = config.getIotHubHostname();

        final String expectedIotHubHostname = iotHubHostname;
        assertThat(testIotHubHostname, is(expectedIotHubHostname));
    }

    // Tests_SRS_DEVICECLIENTCONFIG_11_002: [The function shall return the IoT Hub name given in the constructor,
    // where the IoT Hub name is embedded in the IoT Hub hostname as follows: [IoT Hub name].[valid HTML chars]+.]
    @Test
    public void getIotHubNameReturnsIotHubName() throws URISyntaxException
    {
        final String iotHubHostname = "test.iothubhostname";
        final String deviceId = "test-deviceid";
        final String deviceKey = "test-devicekey";

        DeviceClientConfig config = new DeviceClientConfig(iotHubHostname, deviceId, deviceKey);
        String testIotHubName = config.getIotHubName();

        final String expectedIotHubName = "test";
        assertThat(testIotHubName, is(expectedIotHubName));
    }

    // Tests_SRS_DEVICECLIENTCONFIG_11_003: [The function shall return the device ID given in the constructor.]
    @Test
    public void getDeviceIdReturnsDeviceId() throws URISyntaxException
    {
        final String iotHubHostname = "test.iothubhostname";
        final String deviceId = "test-deviceid";
        final String deviceKey = "test-devicekey";

        DeviceClientConfig config = new DeviceClientConfig(iotHubHostname, deviceId, deviceKey);
        String testDeviceId = config.getDeviceId();

        final String expectedDeviceId = deviceId;
        assertThat(testDeviceId, is(expectedDeviceId));
    }

    // Tests_SRS_DEVICECLIENTCONFIG_11_004: [The function shall return the device key given in the constructor.]
    @Test
    public void getDeviceKeyReturnsDeviceKey() throws URISyntaxException
    {
        final String iotHubHostname = "test.iothubhostname";
        final String deviceId = "test-deviceid";
        final String deviceKey = "test-devicekey";

        DeviceClientConfig config = new DeviceClientConfig(iotHubHostname, deviceId, deviceKey);
        String testDeviceKey = config.getDeviceKey();

        final String expectedDeviceKey = deviceKey;
        assertThat(testDeviceKey, is(expectedDeviceKey));
    }

    // Tests_SRS_DEVICECLIENTCONFIG_11_005: [The function shall return the value of tokenValidSecs.]
    @Test
    public void getMessageValidSecsReturnsConstant() throws URISyntaxException
    {
        final String iotHubHostname = "test.iothubhostname";
        final String deviceId = "test-deviceid";
        final String deviceKey = "test-devicekey";

        DeviceClientConfig config = new DeviceClientConfig(iotHubHostname, deviceId, deviceKey);
        long testMessageValidSecs = config.getTokenValidSecs();

        final long expectedMessageValidSecs = 3600;
        assertThat(testMessageValidSecs, is(expectedMessageValidSecs));
    }

    // Tests_SRS_DEVICECLIENTCONFIG_25_016: [The function shall set the value of tokenValidSecs.]
    @Test
    public void getandsetMessageValidSecsReturnsConstant() throws URISyntaxException
    {
        final String iotHubHostname = "test.iothubhostname";
        final String deviceId = "test-deviceid";
        final String deviceKey = "test-devicekey";

        DeviceClientConfig config = new DeviceClientConfig(iotHubHostname, deviceId, deviceKey);

        long testsetMessageValidSecs = 60;
        config.setTokenValidSecs(testsetMessageValidSecs);
        long testgetMessageValidSecs= config.getTokenValidSecs();

        final long expectedMessageValidSecs = testsetMessageValidSecs;
        assertThat(testgetMessageValidSecs, is(expectedMessageValidSecs));
    }

    // Tests_SRS_DEVICECLIENTCONFIG_11_006: [The function shall set the message callback, with its associated context.]
    // Tests_SRS_DEVICECLIENTCONFIG_11_010: [The function shall return the current message callback.]
    @Test
    public void getAndSetMessageCallbackMatch(
            @Mocked final MessageCallback mockCallback)
            throws URISyntaxException
    {
        final String iotHubHostname = "test.iothubhostname";
        final String deviceId = "test-deviceid";
        final String deviceKey = "test-devicekey";

        DeviceClientConfig config = new DeviceClientConfig(iotHubHostname, deviceId, deviceKey);
        Object context = new Object();
        config.setMessageCallback(mockCallback, context);
        MessageCallback testCallback = config.getMessageCallback();

        final MessageCallback expectedCallback = mockCallback;
        assertThat(testCallback, is(expectedCallback));
    }

    // Tests_SRS_DEVICECLIENTCONFIG_11_006: [The function shall set the message callback, with its associated context.]
    // Tests_SRS_DEVICECLIENTCONFIG_11_011: [The function shall return the current message context.]
    @Test
    public void getAndSetMessageCallbackContextsMatch(
            @Mocked final MessageCallback mockCallback)
            throws URISyntaxException
    {
        final String iotHubHostname = "test.iothubhostname";
        final String deviceId = "test-deviceid";
        final String deviceKey = "test-devicekey";

        DeviceClientConfig config = new DeviceClientConfig(iotHubHostname, deviceId, deviceKey);
        Object context = new Object();
        config.setMessageCallback(mockCallback, context);
        Object testContext = config.getMessageContext();

        final Object expectedContext = context;
        assertThat(testContext, is(expectedContext));
    }

    // Tests_SRS_DEVICECLIENTCONFIG_11_012: [The function shall return 240000ms.]
    @Test
    public void getReadTimeoutMillisReturnsConstant() throws URISyntaxException
    {
        final String iotHubHostname = "test.iothubhostname";
        final String deviceId = "test-deviceid";
        final String deviceKey = "test-devicekey";

        DeviceClientConfig config = new DeviceClientConfig(iotHubHostname, deviceId, deviceKey);
        int testReadTimeoutMillis = config.getReadTimeoutMillis();

        final int expectedReadTimeoutMillis = 240000;
        assertThat(testReadTimeoutMillis, is(expectedReadTimeoutMillis));
    }

    // Tests_SRS_DEVICECLIENTCONFIG_11_013: [The function shall return 180s.]
    @Test
    public void getMessageLockTimeoutSecsReturnsConstant()
            throws URISyntaxException
    {
        final String iotHubHostname = "test.iothubhostname";
        final String deviceId = "test-deviceid";
        final String deviceKey = "test-devicekey";

        DeviceClientConfig config = new DeviceClientConfig(iotHubHostname, deviceId, deviceKey);
        int testMessageLockTimeoutSecs = config.getMessageLockTimeoutSecs();

        final int expectedMessageLockTimeoutSecs = 180;
        assertThat(testMessageLockTimeoutSecs,
                is(expectedMessageLockTimeoutSecs));
    }
}
