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


public final class DeviceClient 
{
	private static IOTHUB_CLIENT_HANDLE handle = new IOTHUB_CLIENT_HANDLE(null);
	private IOTHUB_CLIENT_TRANSPORT_PROVIDER transport;
	
	protected DeviceClient()
	{
	
	}
	
	public enum IotHubClientProtocol 
	{ 
		HTTPS, AMQPS, MQTT 
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
		    default: 
		    // should never happen. 
		        throw new IllegalStateException( 
		       		"Invalid client protocol specified."); 
		} 

		handle = Iothub_client_wrapperLibrary.INSTANCE.IoTHubClient_CreateFromConnectionString(connString, transport);
	}
	
	public void open()
	{
		if (handle == null)
		{
			throw new IllegalStateException(
                    "Cannot open ");
		}
	}
	
	public void close()
	{
		if (handle == null)
		{
			throw new IllegalStateException(
                    "Cannot close ");
		}
	}
	
	public void sendEventAsync(Message message,
	            IotHubEventCallback callback,
	            Pointer lockobj)
	{
		int status = IOTHUB_CLIENT_RESULT.IOTHUB_CLIENT_ERROR;
		
		if (handle == null)
		{
			throw new IllegalStateException(
                    "Cannot send event from "
                            + "an IoT Hub client that is closed.");
		}
			
		status = Iothub_client_wrapperLibrary.INSTANCE.IoTHubClient_SendEventAsync(handle.getPointer(), message.getMessageHandle(), callback, lockobj);
		
		if (status != IOTHUB_CLIENT_RESULT.IOTHUB_CLIENT_OK)
		{
			throw new IllegalArgumentException("SendEventAsync error");
		}
	}
	
	
	
	public void setMessageCallback(IotHubMessageCallback messageCallback, Pointer userContextCallback)
    {
		Iothub_client_wrapperLibrary.INSTANCE.IoTHubClient_SetMessageCallback(handle.getPointer(), messageCallback, userContextCallback);
    }
	
	public void setOption(String optionName, Object value)
    {
		int status = IOTHUB_CLIENT_RESULT.IOTHUB_CLIENT_ERROR;
		
		if(optionName == null)
        {
            throw new IllegalArgumentException("optionName is null");
        }
        else
        {
        	if (this.transport.getClass() == HTTP_Protocol.class)
        	{
        		status = Iothub_client_wrapperLibrary.INSTANCE.IoTHubClient_SetOption(handle.getPointer(), optionName, value);
        	
        		if (status == IOTHUB_CLIENT_RESULT.IOTHUB_CLIENT_OK)
        		{
        			throw new IllegalArgumentException("failure to set option");
        		}	
        	}
        	else if(this.transport.getClass() == AMQP_Protocol.class)
        	{
        		throw new IllegalArgumentException("AMQP does not have setOption");
        	}
        	else
        	{
        		throw new IllegalArgumentException("MQTT does not have setOption");
        	}
        }
    }
}
