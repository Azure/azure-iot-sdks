// Copyright(c) Microsoft.All rights reserved. 
// Licensed under the MIT license.See LICENSE file in the project root for full license information. 

package javaWrapper;

import com.sun.jna.Pointer;

import javaWrapper.Iothub_client_wrapperLibrary.AMQP_Protocol;
import javaWrapper.Iothub_client_wrapperLibrary.HTTP_Protocol;
import javaWrapper.Iothub_client_wrapperLibrary.IOTHUB_CLIENT_HANDLE;
import javaWrapper.Iothub_client_wrapperLibrary.IOTHUB_CLIENT_RESULT;
import javaWrapper.Iothub_client_wrapperLibrary.IOTHUB_CLIENT_TRANSPORT_PROVIDER;
import javaWrapper.Iothub_client_wrapperLibrary.IotHubEventCallback;
import javaWrapper.Iothub_client_wrapperLibrary.IotHubMessageCallback;
import javaWrapper.Iothub_client_wrapperLibrary.MQTT_Protocol;
import javaWrapper.Iothub_client_wrapperLibrary.AMQP_Protocol_over_WebSocketsTls;

public final class DeviceClient 
{
	private static IOTHUB_CLIENT_HANDLE handle = new IOTHUB_CLIENT_HANDLE(null);
	private IOTHUB_CLIENT_TRANSPORT_PROVIDER transport;
	
	public enum IotHubClientProtocol 
	{ 
		HTTPS, AMQPS, MQTT, AMQP_WEB_SOCKET 
	} 

	public DeviceClient(String connString, IotHubClientProtocol protocol)
	{
		switch (protocol) 
		{ 
			case HTTPS: 
				this.transport = new HTTP_Protocol(); 
				break; 
	        case AMQPS: 
	        	this.transport = new AMQP_Protocol();  
		        break; 
		    case MQTT: 
		    	this.transport = new MQTT_Protocol();
		        break; 
		    case AMQP_WEB_SOCKET: 
		    	this.transport = new AMQP_Protocol_over_WebSocketsTls();
		        break; 
		    default: 
		    	// should never happen. 
		        throw new IllegalStateException( 
		       		"Invalid client protocol specified."); 
		} 

		handle = Iothub_client_wrapperLibrary.INSTANCE.IoTHubClient_CreateFromConnectionString(connString, transport);
	
		if (handle == null)
		{
			throw new IllegalStateException(
					"Cannot create a connection ");
		}
	}
	
	public int sendEventAsync(Message message,
	            IotHubEventCallback callback,
	            Pointer object)
	{
		if (handle == null)
		{
			throw new IllegalStateException(
                    "Cannot send event from "
                            + "an IoT Hub client that is closed.");
		}
			
		return Iothub_client_wrapperLibrary.INSTANCE.IoTHubClient_SendEventAsync(handle.getPointer(), message.getMessageHandle(), callback, object);
	}
	
	public void setMessageCallback(IotHubMessageCallback messageCallback, Pointer userContextCallback)
    {
		Iothub_client_wrapperLibrary.INSTANCE.IoTHubClient_SetMessageCallback(handle.getPointer(), messageCallback, userContextCallback);
    }
	
	public int setOption(String optionName, Object value)
    {
		if(optionName == null)
        {
            throw new IllegalArgumentException("optionName is null");
        }
		
		if(value == null)
        {
            throw new IllegalArgumentException("value is null");
        }
        
		return Iothub_client_wrapperLibrary.INSTANCE.IoTHubClient_SetOption(handle.getPointer(), optionName, value);	
    }
}
