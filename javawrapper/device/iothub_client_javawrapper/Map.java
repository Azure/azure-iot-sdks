// Copyright(c) Microsoft.All rights reserved. 
// Licensed under the MIT license.See LICENSE file in the project root for full license information. 

package javaWrapper;

import java.util.HashMap;

import com.sun.jna.ptr.IntByReference;

import javaWrapper.Iothub_client_wrapperLibrary.MAP_FILTER_CALLBACK;
import javaWrapper.Iothub_client_wrapperLibrary.MAP_HANDLE;
import javaWrapper.Iothub_client_wrapperLibrary.MAP_RESULT;
import javaWrapper.Iothub_client_wrapperLibrary.StringByReference;
import javaWrapper.Iothub_client_wrapperLibrary.size_t;

public class Map 
{
	private static MAP_HANDLE mapHandle;
	
	Map()
	{
	}
	
	Map(MAP_HANDLE _mapHandle)
	{
		if (_mapHandle == null) 
        {
            throw new IllegalArgumentException("_mapHandle cannot be 'null'.");
        } 
		
		mapHandle = _mapHandle;
	}
	
	public void setMapHandle(MAP_HANDLE _mapHandle) 
	{
		mapHandle = _mapHandle;
	}
	
	public void mapCreate()
	{
		MAP_FILTER_CALLBACK mapFilterFunc = null;
		
		mapHandle = Iothub_client_wrapperLibrary.INSTANCE.Map_Create(mapFilterFunc);
	}
	
	public void mapDestroy()
	{
		Iothub_client_wrapperLibrary.INSTANCE.Map_Destroy(mapHandle);
	}
	
	public MAP_HANDLE mapClone()
	{
		new Map(Iothub_client_wrapperLibrary.INSTANCE.Map_Clone(mapHandle));
		
		return mapHandle;
	}
	
	public int mapAdd(String key, String value)
	{
		return Iothub_client_wrapperLibrary.INSTANCE.Map_Add(mapHandle, key, value);
	}
	
	public int mapAddOrUpdate(String key, String value)
	{
		return Iothub_client_wrapperLibrary.INSTANCE.Map_AddOrUpdate(mapHandle, key, value);
	}
	
	public int mapDelete(String key)
	{
		return Iothub_client_wrapperLibrary.INSTANCE.Map_Delete(mapHandle, key);
	}
	
	public int mapContainsValue(String value)
	{
		IntByReference valueExists = null;
		
		return Iothub_client_wrapperLibrary.INSTANCE.Map_ContainsValue(mapHandle, value, valueExists);
	}
	
	public String mapGetValueFromKey(String key)
	{
		return Iothub_client_wrapperLibrary.INSTANCE.Map_GetValueFromKey(mapHandle, key);
	}
	
	public HashMap mapGetInternals()
	{
		StringByReference[] keys = null;
		StringByReference[] values = null;
		size_t count = new size_t(0);
		int result = MAP_RESULT.MAP_ERROR;
		HashMap keyPairValue = new HashMap();
		
		result = Iothub_client_wrapperLibrary.INSTANCE.Map_GetInternals(mapHandle, keys, values, count);
		
		if (result == MAP_RESULT.MAP_OK)
		{
			for (int i = 0; i < count.intValue(); i++)
			{
				keyPairValue.put(keys[i], values[i]);
			}
		}
		return keyPairValue;
	}
	
}
