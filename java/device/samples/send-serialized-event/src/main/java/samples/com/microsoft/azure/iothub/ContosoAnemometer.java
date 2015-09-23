// Copyright (c) Microsoft. All rights reserved.
// Licensed under the MIT license. See LICENSE file in the project root for full license information.

package samples.com.microsoft.azure.iothub;

import com.google.gson.Gson;

public class ContosoAnemometer {
    public String deviceId;
    public double windSpeed;

    public boolean TurnFanOn() {
        System.out.println("Turning fan on.");
        return true;
    }

    public boolean TurnFanOff() {
        System.out.println("Turning fan off.");
        return true;
    }

    public boolean SetAirResistance(int position) {
        System.out.printf("Setting Air Resistance Position to %d.\r\n", position);
        return true;
    }

    public String serialize() {
        Gson gson = new Gson();

        return gson.toJson(this);
    }
}
