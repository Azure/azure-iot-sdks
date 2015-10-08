/*
 * Copyright (c) Microsoft. All rights reserved.
 * Licensed under the MIT license. See LICENSE file in the project root for full license information.
 */

package com.microsoft.azure.iot.service.sdk;

import org.junit.Test;

import javax.json.JsonObject;

/**
 *
 * @author zolvarga
 */
public class IotHubDeviceTest
{
    /**
     * Test of clone method, of class IotHubDevice.
     */
    @Test (expected = Exception.class)
    public void testConstructorNull()
    {
        IotHubDevice iotHubDevice = new IotHubDevice((JsonObject) JsonObject.NULL);
    }
}
