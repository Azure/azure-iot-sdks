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
	
	
	public Message()
	{
		
	}
	
    public Message(byte[] body) 
    {
        if (body == null) {
            throw new IllegalArgumentException("Message body cannot be 'null'.");
        }      

        messageHandle = Iothub_client_wrapperLibrary.INSTANCE.IoTHubMessage_CreateFromByteArray(body, body.length);
    }
    
    public Message(String body) 
    {
        if (body == null) {
            throw new IllegalArgumentException("Message body cannot be 'null'.");
        }      

        messageHandle = Iothub_client_wrapperLibrary.INSTANCE.IoTHubMessage_CreateFromString(body);
    }
    
    public IOTHUB_MESSAGE_HANDLE clone()
    {
    	return Iothub_client_wrapperLibrary.INSTANCE.IoTHubMessage_Clone(messageHandle);
    }
    
    public byte[] getBytes()
    {
        String body;
        
        body = Iothub_client_wrapperLibrary.INSTANCE.IoTHubMessage_GetString(messageHandle);
		
		return body.getBytes();
    }
    
    public MAP_HANDLE getProperties() 
    {
    	return Iothub_client_wrapperLibrary.INSTANCE.IoTHubMessage_Properties(messageHandle);	
    }
    
    public void setProperty(String name, String value) 
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
        Map map = new Map(propMap);
        map.mapAddOrUpdate(name, value);
    }
    
    public String getMessageId()
    {
        return Iothub_client_wrapperLibrary.INSTANCE.IoTHubMessage_GetMessageId(messageHandle);
    }
    
    public void setMessageId(String messageId)
    {
        int status = IOTHUB_MESSAGE_RESULT.IOTHUB_MESSAGE_ERROR;
    	
    	status = Iothub_client_wrapperLibrary.INSTANCE.IoTHubMessage_SetMessageId(messageHandle, messageId);
    
    	if (status != IOTHUB_MESSAGE_RESULT.IOTHUB_MESSAGE_OK)
		{
			throw new IllegalArgumentException("failure to set Message Id");
		}	
    }
    
    public String getCorrelationId()
    {
    	return Iothub_client_wrapperLibrary.INSTANCE.IoTHubMessage_GetCorrelationId(messageHandle);
    }
    
    public void setCorrelationId(String correlationId)
    {
    	int status = IOTHUB_MESSAGE_RESULT.IOTHUB_MESSAGE_ERROR;;
    	
    	status = Iothub_client_wrapperLibrary.INSTANCE.IoTHubMessage_SetCorrelationId(messageHandle, correlationId);
    
    	if (status != IOTHUB_MESSAGE_RESULT.IOTHUB_MESSAGE_OK)
		{
			throw new IllegalArgumentException("failure to set Correlation Id");
		}	
    }
    
    public void destroy(String correlationId)
    {
    	Iothub_client_wrapperLibrary.INSTANCE.IoTHubMessage_Destroy(messageHandle);
    }
    
    IOTHUB_MESSAGE_HANDLE getMessageHandle()
    {
    	return Message.messageHandle;
    }
    
}
