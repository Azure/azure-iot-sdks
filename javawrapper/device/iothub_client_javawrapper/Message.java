// Copyright(c) Microsoft.All rights reserved. 
// Licensed under the MIT license.See LICENSE file in the project root for full license information. 


package javaWrapper;

import com.sun.jna.ptr.IntByReference;
import com.sun.jna.ptr.PointerByReference;

import javaWrapper.Iothub_client_wrapperLibrary.IOTHUB_MESSAGE_HANDLE;
import javaWrapper.Iothub_client_wrapperLibrary.IOTHUB_MESSAGE_RESULT;
import javaWrapper.Iothub_client_wrapperLibrary.MAP_HANDLE;

public class Message 
{
	public static IOTHUB_MESSAGE_HANDLE messageHandle = null;
	private static MAP_HANDLE propMap;
	private Map map;
	
	
	public Message(IOTHUB_MESSAGE_HANDLE _messageHandle)
	{
		if (_messageHandle == null) 
        {
            throw new IllegalArgumentException("_messageHandle cannot be 'null'.");
        }      
		
		messageHandle = _messageHandle;
	}
	
	public Message()
	{
		
	}
	
	public Message(byte[] body) 
    {
        if (body == null) 
        {
            throw new IllegalArgumentException("Message body cannot be 'null'.");
        }      

        messageHandle = Iothub_client_wrapperLibrary.INSTANCE.IoTHubMessage_CreateFromByteArray(body, body.length);
   
		map = new Map();
    }
    
	public Message(String body) 
	{
		if (body == null) 
		{
			throw new IllegalArgumentException("Message body cannot be 'null'.");
		}      

		messageHandle = Iothub_client_wrapperLibrary.INSTANCE.IoTHubMessage_CreateFromString(body);
	
		map = new Map();
	}
    
	public IOTHUB_MESSAGE_HANDLE clone()
	{
		new Message(Iothub_client_wrapperLibrary.INSTANCE.IoTHubMessage_Clone(messageHandle));
		
		return messageHandle;
	}
    
	public byte[] getBytes()
	{
		return Iothub_client_wrapperLibrary.INSTANCE.IoTHubMessage_GetString(messageHandle).getBytes();
	}
    
	public MAP_HANDLE getProperties() 
	{
		return Iothub_client_wrapperLibrary.INSTANCE.IoTHubMessage_Properties(messageHandle);	
	}
    
	public int setProperty(String name, String value) 
	{    
		if (name == null) 
		{
			throw new IllegalArgumentException("Property name cannot be 'null'.");
		}

		if (value == null) 
		{
			throw new IllegalArgumentException("Property value cannot be 'null'.");
		}

		propMap = this.getProperties();
		map.setMapHandle(propMap);
		
		return map.mapAddOrUpdate(name, value);
	}
    
	public String getMessageId()
	{
		return Iothub_client_wrapperLibrary.INSTANCE.IoTHubMessage_GetMessageId(messageHandle);
	}
    
	public int setMessageId(String messageId)
	{
		if (messageId == null) 
		{
			throw new IllegalArgumentException("MessageId value cannot be 'null'.");
		}
    	
		return Iothub_client_wrapperLibrary.INSTANCE.IoTHubMessage_SetMessageId(messageHandle, messageId);
	}
    
	public String getCorrelationId()
	{
		return Iothub_client_wrapperLibrary.INSTANCE.IoTHubMessage_GetCorrelationId(messageHandle);
	}
    
	public int setCorrelationId(String correlationId)
	{
		if (correlationId == null) 
		{
			throw new IllegalArgumentException("correlationId value cannot be 'null'.");
		}
    	
		return Iothub_client_wrapperLibrary.INSTANCE.IoTHubMessage_SetCorrelationId(messageHandle, correlationId);
	}
    
	public void destroy(String correlationId)
	{
		if (correlationId == null) 
		{
			throw new IllegalArgumentException("correlationId value cannot be 'null'.");
		}
		
		Iothub_client_wrapperLibrary.INSTANCE.IoTHubMessage_Destroy(messageHandle);
	}
    
	IOTHUB_MESSAGE_HANDLE getMessageHandle()
	{
		return Message.messageHandle;
	}
}
