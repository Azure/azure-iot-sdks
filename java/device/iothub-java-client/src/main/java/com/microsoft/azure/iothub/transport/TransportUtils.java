// Copyright (c) Microsoft. All rights reserved.
// Licensed under the MIT license. See LICENSE file in the project root for full license information.

package com.microsoft.azure.iothub.transport;

public class TransportUtils
{
    public static String javaDeviceClientIdentifier = "com.microsoft.azure.iothub-java-client/";
    public static String clientVersion = "1.0.5";

    private static byte[] sleepIntervals = {1, 2, 4, 8, 16, 32, 60};
    /** Generates a reconnection time with an exponential backoff
     * and a maximum value of 60 seconds.
     *
     * @param currentAttempt the number of attempts
     * @return the sleep interval until the next attempt.
     */
    public static byte generateSleepInterval(int currentAttempt)
    {
        if (currentAttempt > 7)
        {
            return sleepIntervals[6];
        }
        else if (currentAttempt > 0)
        {
            return sleepIntervals[currentAttempt - 1];
        }
        else
        {
            return 0;
        }
    }
}
