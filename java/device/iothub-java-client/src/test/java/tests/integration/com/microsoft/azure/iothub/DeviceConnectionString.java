// Copyright (c) Microsoft. All rights reserved.
// Licensed under the MIT license. See LICENSE file in the project root for full license information.

package tests.integration.com.microsoft.azure.iothub;

import com.microsoft.azure.iot.service.sdk.Device;

public class DeviceConnectionString
{
    public static String get(String iotHubConnectionString, Device device)
    {
        StringBuilder stringBuilder = new StringBuilder();
        String[] tokenArray = iotHubConnectionString.split(";");
        String hostName = "";
        for (int i = 0; i < tokenArray.length; i++)
        {
            String[] keyValueArray = tokenArray[i].split("=");
            if (keyValueArray[0].equals("HostName"))
            {
                hostName =  tokenArray[i] + ';';
                break;
            }
        }

        stringBuilder.append(hostName);
        stringBuilder.append(String.format("DeviceId=%s", device.getDeviceId()));
        stringBuilder.append(String.format(";SharedAccessKey=%s", device.getPrimaryKey()));
        return stringBuilder.toString();
    }
}
