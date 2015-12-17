package com.microsoft.azure.iothub.transport;

import com.microsoft.azure.iothub.AzureHubType;
import com.microsoft.azure.iothub.DeviceClientConfig;
import com.microsoft.azure.iothub.auth.IotHubSasToken;
import com.microsoft.azure.iothub.net.IotHubUri;

public class TransportUtils {
    /**
     * Generates a valid SAS token from the client configuration.
     *
     * @param config the DeviceClientConfig
     *
     * @return a SAS token that authenticates the device to the IoT Hub.
     */
    public static IotHubSasToken buildToken(DeviceClientConfig config)
    {
        String iotHubHostname = config.getIotHubHostname();
        String deviceId = config.getDeviceId();
        String deviceKey = config.getDeviceKey();
        long tokenValidSecs = config.getTokenValidSecs();

        long msgExpiryTime = System.currentTimeMillis() / 1000l + tokenValidSecs + 1l;

        String resourceUri = IotHubUri.getResourceUri(iotHubHostname, deviceId);

        IotHubSasToken sasToken = new IotHubSasToken(resourceUri, deviceId, deviceKey, msgExpiryTime);

        if (config.targetHubType == AzureHubType.IoTHub) {
            sasToken.appendSknValue = false;
        } else if (config.targetHubType == AzureHubType.EventHub) {
            sasToken.appendSknValue = true;
        }

        return sasToken;
    }

    /** Generates a reconnection time with an exponential backoff
     * and a maximum value of 60 seconds.
     *
     * @param currentAttempt the number of attempts
     * @return the sleep interval until the next attempt.
     */
    public static int generateSleepInterval(int currentAttempt)
    {
        int interval = (int)((Math.pow(2, currentAttempt) - 1) * 1000);
        if (interval > 60000)
        {
            interval = 60000;
        }
        return interval;
    }
}
