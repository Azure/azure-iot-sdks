/*
 * Copyright (c) Microsoft. All rights reserved.
 * Licensed under the MIT license. See LICENSE file in the project root for full license information.
 */

package samples.com.microsoft.azure.iot.service.sdk;

import com.microsoft.azure.iot.service.exceptions.IotHubException;
import com.microsoft.azure.iot.service.sdk.Device;
import com.microsoft.azure.iot.service.sdk.RegistryManager;

import java.io.IOException;
import java.net.URISyntaxException;

/** Manages device on IotHub - CRUD operations */
public class DeviceManagerSample
{
    /**
     * @param args
     * @throws IOException
     * @throws URISyntaxException
     */
    public static void main(String[] args) throws Exception
    {
        System.out.println("Starting sample...");
        
        System.out.println("Add Device started");
        AddDevice();
        System.out.println("Add Device finished");

        System.out.println("Get Device started");
        GetDevice();
        System.out.println("Get Device finished");
        
        System.out.println("Update Device started");
        UpdateDevice();
        System.out.println("Update Device finished");
        
        System.out.println("Remove Device started");
        RemoveDevice();
        System.out.println("Remove Device finished");
        
        System.out.println("Shutting down sample...");
    }
    
    private static void AddDevice() throws Exception
    {
        RegistryManager registryManager = RegistryManager.createFromConnectionString(SampleUtils.iotHubConnectionString);
        
        Device device = Device.createFromId(SampleUtils.deviceId, null, null);
        try
        {
            device = registryManager.addDevice(device);

            System.out.println("Device created: " + device.getDeviceId());
            System.out.println("Device key: " + device.getPrimaryKey());
        }
        catch (IotHubException iote)
        {
            iote.printStackTrace();
        }
        catch (IOException e)
        {
            e.printStackTrace();
        }
    }
    
    private static void GetDevice() throws Exception
    {
        RegistryManager registryManager = RegistryManager.createFromConnectionString(SampleUtils.iotHubConnectionString);
        
        Device returnDevice = null;
        try
        {
            returnDevice = registryManager.getDevice(SampleUtils.deviceId);

            System.out.println("Device: " + returnDevice.getDeviceId());
            System.out.println("Device primary key: " + returnDevice.getPrimaryKey());
            System.out.println("Device secondary key: " + returnDevice.getSecondaryKey());
            System.out.println("Device eTag: " + returnDevice.geteTag());
        }
        catch (IotHubException iote)
        {
            iote.printStackTrace();
        }
        catch (IOException e)
        {
            e.printStackTrace();
        }
    }
    
    private static void UpdateDevice() throws Exception
    {
        String primaryKey = "[New primary key goes here]";
        String secondaryKey = "[New secondary key goes here]";

        RegistryManager registryManager = RegistryManager.createFromConnectionString(SampleUtils.iotHubConnectionString);
        
        Device device = Device.createFromId(SampleUtils.deviceId, null, null);
        device.getSymmetricKey().setPrimaryKey(primaryKey);
        device.getSymmetricKey().setSecondaryKey(secondaryKey);
        try
        {
            device = registryManager.updateDevice(device);

            System.out.println("Device updated: " + device.getDeviceId());
            System.out.println("Device primary key: " + device.getPrimaryKey());
            System.out.println("Device secondary key: " + device.getSecondaryKey());
        }
        catch (IotHubException iote)
        {
            iote.printStackTrace();
        }
        catch (IOException e)
        {
            e.printStackTrace();
        }
    }
    
    private static void RemoveDevice() throws Exception
    {
        RegistryManager registryManager = RegistryManager.createFromConnectionString(SampleUtils.iotHubConnectionString);
        
        try
        {
            registryManager.removeDevice(SampleUtils.deviceId);
            System.out.println("Device removed: " + SampleUtils.deviceId);
        }
        catch (IotHubException iote)
        {
            iote.printStackTrace();
        }
        catch (IOException e)
        {
            e.printStackTrace();
        }
    }
}
