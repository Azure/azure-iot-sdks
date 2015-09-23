// Copyright (c) Microsoft. All rights reserved.
// Licensed under the MIT license. See LICENSE file in the project root for full license information.

package samples.com.microsoft.azure.iothub;

import com.google.gson.Gson;

public class ContosoThermostat505 {
    public int temperature;
    public int humidity;
    public boolean LowTemperatureAlarm;

    public boolean TurnFanOn() {
        System.out.println("Turning fan on.");
        return true;
    }

    public boolean TurnFanOff() {
        System.out.println("Turning fan off.");
        return true;
    }

    public boolean SetTemperature(int desiredTemperature) {
        System.out.printf("Setting home temperature to %d degrees.\r\n", desiredTemperature);
        this.temperature = desiredTemperature;
        return true;
    }

    public String serialize() {
        Gson gson = new Gson();

        return gson.toJson(this);
    }
}
