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
	
	public void mapClone()
	{
		Iothub_client_wrapperLibrary.INSTANCE.Map_Clone(mapHandle);
	}
	
	public void mapAdd(String key, String value)
	{
		int result = MAP_RESULT.MAP_ERROR;
		
		result = Iothub_client_wrapperLibrary.INSTANCE.Map_Add(mapHandle, key, value);
		
		if(result != MAP_RESULT.MAP_OK)
    	{
    		throw new IllegalArgumentException("Error adding map");
    	}
	}
	
	public void mapAddOrUpdate(String key, String value)
	{
		int result = MAP_RESULT.MAP_ERROR;
		
		result = Iothub_client_wrapperLibrary.INSTANCE.Map_AddOrUpdate(mapHandle, key, value);
		
		if(result != MAP_RESULT.MAP_OK)
    	{
    		throw new IllegalArgumentException("Error adding or updating map");
    	}
	}
	
	public void mapDelete(String key)
	{
		int result = MAP_RESULT.MAP_ERROR;
		
		result = Iothub_client_wrapperLibrary.INSTANCE.Map_Delete(mapHandle, key);
		
		if(result != MAP_RESULT.MAP_OK)
    	{
    		throw new IllegalArgumentException("Error deleting map");
    	}
	}
	
	public void mapContainsValue(String value)
	{
		int result = MAP_RESULT.MAP_ERROR;
		IntByReference valueExists = null;
		
		result = Iothub_client_wrapperLibrary.INSTANCE.Map_ContainsValue(mapHandle, value, valueExists);
		
		if(result != MAP_RESULT.MAP_OK)
    	{
    		throw new IllegalArgumentException("Error deleting map");
    	}
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
