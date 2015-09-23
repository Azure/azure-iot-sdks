// Copyright (c) Microsoft. All rights reserved.
// Licensed under the MIT license. See LICENSE file in the project root for full license information.

package tests.unit.com.microsoft.azure.iothub;

import static org.hamcrest.CoreMatchers.is;
import static org.junit.Assert.assertThat;

import com.microsoft.azure.iothub.IotHubStatusCode;

import org.junit.Test;

/** Unit tests for IotHubStatusCode. */
public class IotHubStatusCodeTest
{
    // Tests_SRS_IOTHUBSTATUSCODE_11_001: [The function shall convert the given HTTPS status code to the corresponding IoT Hub status code.]
    @Test
    public void getIotHubStatusCodeMapsOkCorrectly()
    {
        final int httpsStatus = 200;
        IotHubStatusCode testStatus =
                IotHubStatusCode.getIotHubStatusCode(httpsStatus);

        final IotHubStatusCode expectedStatus = IotHubStatusCode.OK;
        assertThat(testStatus, is(expectedStatus));
    }

    // Tests_SRS_IOTHUBSTATUSCODE_11_001: [The function shall convert the given HTTPS status code to the corresponding IoT Hub status code.]
    @Test
    public void getIotHubStatusCodeMapsOkEmptyCorrectly()
    {
        final int httpsStatus = 204;
        IotHubStatusCode testStatus =
                IotHubStatusCode.getIotHubStatusCode(httpsStatus);

        final IotHubStatusCode expectedStatus = IotHubStatusCode.OK_EMPTY;
        assertThat(testStatus, is(expectedStatus));
    }

    // Tests_SRS_IOTHUBSTATUSCODE_11_001: [The function shall convert the given HTTPS status code to the corresponding IoT Hub status code.]
    @Test
    public void getIotHubStatusCodeMapsBadFormatCorrectly()
    {
        final int httpsStatus = 400;
        IotHubStatusCode testStatus =
                IotHubStatusCode.getIotHubStatusCode(httpsStatus);

        final IotHubStatusCode expectedStatus = IotHubStatusCode.BAD_FORMAT;
        assertThat(testStatus, is(expectedStatus));
    }

    // Tests_SRS_IOTHUBSTATUSCODE_11_001: [The function shall convert the given HTTPS status code to the corresponding IoT Hub status code.]
    @Test
    public void getIotHubStatusCodeMapsUnauthorizedCorrectly()
    {
        final int httpsStatus = 401;
        IotHubStatusCode testStatus =
                IotHubStatusCode.getIotHubStatusCode(httpsStatus);

        final IotHubStatusCode expectedStatus = IotHubStatusCode.UNAUTHORIZED;
        assertThat(testStatus, is(expectedStatus));
    }

    // Tests_SRS_IOTHUBSTATUSCODE_11_001: [The function shall convert the given HTTPS status code to the corresponding IoT Hub status code.]
    @Test
    public void getIotHubStatusCodeMapsTooManyDevicesCorrectly()
    {
        final int httpsStatus = 403;
        IotHubStatusCode testStatus =
                IotHubStatusCode.getIotHubStatusCode(httpsStatus);

        final IotHubStatusCode expectedStatus =
                IotHubStatusCode.TOO_MANY_DEVICES;
        assertThat(testStatus, is(expectedStatus));
    }

    // Tests_SRS_IOTHUBSTATUSCODE_11_001: [The function shall convert the given HTTPS status code to the corresponding IoT Hub status code.]
    @Test
    public void getIotHubStatusCodeMapsHubOrDeviceNotFoundCorrectly()
    {
        final int httpsStatus = 404;
        IotHubStatusCode testStatus =
                IotHubStatusCode.getIotHubStatusCode(httpsStatus);

        final IotHubStatusCode expectedStatus =
                IotHubStatusCode.HUB_OR_DEVICE_ID_NOT_FOUND;
        assertThat(testStatus, is(expectedStatus));
    }

    // Tests_SRS_IOTHUBSTATUSCODE_11_001: [The function shall convert the given HTTPS status code to the corresponding IoT Hub status code.]
    @Test
    public void getIotHubStatusCodeMapsPreconditionFailedCorrectly()
    {
        final int httpsStatus = 412;
        IotHubStatusCode testStatus =
                IotHubStatusCode.getIotHubStatusCode(httpsStatus);

        final IotHubStatusCode expectedStatus =
                IotHubStatusCode.PRECONDITION_FAILED;
        assertThat(testStatus, is(expectedStatus));
    }

    // Tests_SRS_IOTHUBSTATUSCODE_11_001: [The function shall convert the given HTTPS status code to the corresponding IoT Hub status code.]
    @Test
    public void getIotHubStatusCodeMapsThrottledCorrectly()
    {
        final int httpsStatus = 429;
        IotHubStatusCode testStatus =
                IotHubStatusCode.getIotHubStatusCode(httpsStatus);

        final IotHubStatusCode expectedStatus = IotHubStatusCode.THROTTLED;
        assertThat(testStatus, is(expectedStatus));
    }

    // Tests_SRS_IOTHUBSTATUSCODE_11_001: [The function shall convert the given HTTPS status code to the corresponding IoT Hub status code.]
    @Test
    public void getIotHubStatusCodeMapsInternalServerErrorCorrectly()
    {
        final int httpsStatus = 500;
        IotHubStatusCode testStatus =
                IotHubStatusCode.getIotHubStatusCode(httpsStatus);

        final IotHubStatusCode expectedStatus =
                IotHubStatusCode.INTERNAL_SERVER_ERROR;
        assertThat(testStatus, is(expectedStatus));
    }

    // Tests_SRS_IOTHUBSTATUSCODE_11_001: [The function shall convert the given HTTPS status code to the corresponding IoT Hub status code.]
    @Test
    public void getIotHubStatusCodeMapsServerBusyCorrectly()
    {
        final int httpsStatus = 503;
        IotHubStatusCode testStatus =
                IotHubStatusCode.getIotHubStatusCode(httpsStatus);

        final IotHubStatusCode expectedStatus = IotHubStatusCode.SERVER_BUSY;
        assertThat(testStatus, is(expectedStatus));
    }

    // Tests_SRS_IOTHUBSTATUSCODE_11_002: [If the given HTTPS status code does not map to an IoT Hub status code, the function return status code ERROR.]
    @Test
    public void getIotHubStatusCodeMapsOtherStatusCodeToError()
    {
        final int httpsStatus = -1;

        IotHubStatusCode testStatus =
                IotHubStatusCode.getIotHubStatusCode(httpsStatus);

        final IotHubStatusCode expectedStatus = IotHubStatusCode.ERROR;
        assertThat(testStatus, is(expectedStatus));
    }
}
