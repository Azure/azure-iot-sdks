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
        messageHandle = Iothub_client_wrapperLibrary.INSTANCE.IoTHubMessage_CreateFromByteArray(body, body.length);
        
        map = new Map();
    }
    
    public Message(String body) 
    {
        messageHandle = Iothub_client_wrapperLibrary.INSTANCE.IoTHubMessage_CreateFromString(body);
    
        map = new Map();
    }
    
    public IOTHUB_MESSAGE_HANDLE clone()
    {
        new Message(Iothub_client_wrapperLibrary.INSTANCE.IoTHubMessage_Clone(messageHandle));
        
        return messageHandle;
    }
    
    public String getString()
    {
        return Iothub_client_wrapperLibrary.INSTANCE.IoTHubMessage_GetString(messageHandle);
    }
    
    public byte[] getBytes()
    {
        PointerByReference buffer = new PointerByReference();
        IntByReference size = new IntByReference(0);
        
        Iothub_client_wrapperLibrary.INSTANCE.IoTHubMessage_GetByteArray(messageHandle, buffer, size);
        
        byte[] buffer1 = buffer.getValue().getByteArray(0, size.getValue());
        
        return buffer1;
    }
    
    public MAP_HANDLE getProperties() 
    {
        return Iothub_client_wrapperLibrary.INSTANCE.IoTHubMessage_Properties(messageHandle);	
    }
    
    public int setProperty(String name, String value) 
    {    
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
        return Iothub_client_wrapperLibrary.INSTANCE.IoTHubMessage_SetMessageId(messageHandle, messageId);
    }
    
    public String getCorrelationId()
    {
        return Iothub_client_wrapperLibrary.INSTANCE.IoTHubMessage_GetCorrelationId(messageHandle);
    }
    
    public int setCorrelationId(String correlationId)
    {
        return Iothub_client_wrapperLibrary.INSTANCE.IoTHubMessage_SetCorrelationId(messageHandle, correlationId);
    }
    
    public void destroy()
    {
        Iothub_client_wrapperLibrary.INSTANCE.IoTHubMessage_Destroy(messageHandle);
    }
    
    IOTHUB_MESSAGE_HANDLE getMessageHandle()
    {
        return Message.messageHandle;
    }
    
    public void setMessageHandle(IOTHUB_MESSAGE_HANDLE _messageHandle)
    {
        messageHandle = _messageHandle;
    }
}
