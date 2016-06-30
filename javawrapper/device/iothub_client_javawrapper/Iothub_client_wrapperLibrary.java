
// Copyright(c) Microsoft.All rights reserved. 
// Licensed under the MIT license.See LICENSE file in the project root for full license information. 
package javaWrapper;


import java.util.Arrays;
import java.util.List;

import com.sun.jna.Callback;
import com.sun.jna.IntegerType;
import com.sun.jna.Native;
import com.sun.jna.NativeLibrary;
import com.sun.jna.Pointer;
import com.sun.jna.PointerType;
import com.sun.jna.Structure;
import com.sun.jna.ptr.ByReference;
import com.sun.jna.ptr.IntByReference;
import com.sun.jna.Library;
import javaWrapper.LoadTestLibrary;

/*
 * JNA Wrapper for library iothub_client_java
 */
import com.sun.jna.ptr.PointerByReference;

public interface Iothub_client_wrapperLibrary extends Library{
    
    public static final String JNA_LIBRARY_NAME = "iothub_client_java";
    public static final String JNA_LIBRARY_NAME1 = "iothub_client_mock";
    
    public static final NativeLibrary JNA_NATIVE_LIB = NativeLibrary.getInstance(
            (LoadTestLibrary.isTest() ? Iothub_client_wrapperLibrary.JNA_LIBRARY_NAME1 
                                      : Iothub_client_wrapperLibrary.JNA_LIBRARY_NAME ));
    
    public static final Iothub_client_wrapperLibrary INSTANCE = (Iothub_client_wrapperLibrary)Native.loadLibrary(
                (LoadTestLibrary.isTest() ? Iothub_client_wrapperLibrary.JNA_LIBRARY_NAME1
                                          : Iothub_client_wrapperLibrary.JNA_LIBRARY_NAME),
                                            Iothub_client_wrapperLibrary.class);
    
    /*
     *				iothub_client.h
     */
    
    /*
     * Original signature : IOTHUB_CLIENT_HANDLE IoTHubClient_CreateFromConnectionString(const char*, IOTHUB_CLIENT_TRANSPORT_PROVIDER)
     */
    public IOTHUB_CLIENT_HANDLE IoTHubClient_CreateFromConnectionString(String connectionString, Object protocol);
    
    /*
     * Original signature : IOTHUB_CLIENT_HANDLE DECLDIR IoTHubClient_Create(const IOTHUB_CLIENT_CONFIG*)
     */
    public IOTHUB_CLIENT_HANDLE IoTHubClient_Create(IOTHUB_CLIENT_CONFIG config);
    
    /*
     * Original signature : IOTHUB_CLIENT_RESULT IoTHubClient_Destroy(IOTHUB_CLIENT_HANDLE)
     */
    public int IoTHubClient_Destroy(Pointer iotHubClientHandle);
    
    /*
     * Original signature : IOTHUB_CLIENT_RESULT IoTHubClient_SendEventAsync(IOTHUB_CLIENT_HANDLE, IOTHUB_MESSAGE_HANDLE, IOTHUB_CLIENT_EVENT_CONFIRMATION_CALLBACK, void*)
     */
    public int IoTHubClient_SendEventAsync(Pointer iotHubClientHandle, IOTHUB_MESSAGE_HANDLE eventMessageHandle, IotHubEventCallback eventConfirmationCallback, Pointer userContextCallback);
    /*
     * Original signature : IOTHUB_CLIENT_RESULT IoTHubClient_GetSendStatus(IOTHUB_CLIENT_HANDLE, IOTHUB_CLIENT_STATUS*)
     */
    public int IoTHubClient_GetSendStatus(Pointer iotHubClientHandle, IntByReference iotHubClientStatus);
    
    /*
     * Original signature : IOTHUB_CLIENT_RESULT IoTHubClient_SetMessageCallback(IOTHUB_CLIENT_HANDLE, IOTHUB_CLIENT_MESSAGE_CALLBACK_ASYNC, void*)
     */
    public int IoTHubClient_SetMessageCallback(Pointer iotHubClientHandle, IotHubMessageCallback messageCallback, Pointer userContextCallback);
    
    /*
     * Original signature : IOTHUB_CLIENT_RESULT DECLDIR IoTHubClient_GetLastMessageReceiveTime(IOTHUB_CLIENT_HANDLE, time_t* )
     */
    public int IoTHubClient_GetLastMessageReceiveTime(Pointer iotHubClientHandle, time_t lastMessageReceiveTime);
    
    /*
     * Original signature : IOTHUB_CLIENT_RESULT IoTHubClient_SetOption(IOTHUB_CLIENT_HANDLE, const char*, const void*)
     */
    public int IoTHubClient_SetOption(Pointer iotHubClientHandle, String optionName, Object value);

    /*
     * Original signature : IOTHUB_CLIENT_RESULT IoTHubClient_UploadToBlobAsync(IOTHUB_CLIENT_HANDLE iotHubClientHandle, const char* destinationFileName, const unsigned char* source, size_t size, IOTHUB_CLIENT_FILE_UPLOAD_CALLBACK iotHubClientFileUploadCallback, void* context)
     */
    public int IoTHubClient_UploadToBlobAsync(Pointer iotHubClientHandle, String destinationFileName, String source, int size, IotHubFileUploadCallback fileUploadCallback, Pointer userContextCallback);

    /*
     *				iothubtransporthttp.h		
     */
    
    /*
     * Original signature : TRANSPORT_HANDLE IoTHubTransportHttp_Create(const IOTHUBTRANSPORT_CONFIG*)
     */
    public TRANSPORT_HANDLE IoTHubTransportHttp_Create(IOTHUBTRANSPORT_CONFIG config[]);
    
    /*
     * Original signature : void IoTHubTransportHttp_Destroy(TRANSPORT_HANDLE)
     */
    public void IoTHubTransportHttp_Destroy(TRANSPORT_HANDLE handle);
    
    /*
     * Original signature : int IoTHubTransportHttp_Subscribe(TRANSPORT_HANDLE)
     */
    public int IoTHubTransportHttp_Subscribe(TRANSPORT_HANDLE handle);
    
    /*
     * Original signature : void IoTHubTransportHttp_Unsubscribe(TRANSPORT_HANDLE)
     */
    public void IoTHubTransportHttp_Unsubscribe(TRANSPORT_HANDLE handle);
    
    /*
     * Original signature : <code>void IoTHubTransportHttp_DoWork(TRANSPORT_HANDLE, IOTHUB_CLIENT_LL_HANDLE)</code><br>
     */
    public void IoTHubTransportHttp_DoWork(TRANSPORT_HANDLE handle, IOTHUB_CLIENT_HANDLE iotHubClientHandle);
    
    /*
     * Original signature : IOTHUB_CLIENT_RESULT IoTHubTransportHttp_GetSendStatus(TRANSPORT_HANDLE, IOTHUB_CLIENT_STATUS*)
     */
    public int IoTHubTransportHttp_GetSendStatus(TRANSPORT_HANDLE handle, IntByReference iotHubClientStatus);
    
    /*
     * Original signature : IOTHUB_CLIENT_RESULT IoTHubTransportHttp_SetOption(TRANSPORT_HANDLE, const char*, const void*)
     */
    public int IoTHubTransportHttp_SetOption(TRANSPORT_HANDLE handle, String optionName, Pointer value);
    
    /*
     * Original signature : void* HTTP_Protocol()
     */
    public Pointer HTTP_Protocol(); 
    
    /*
     *				iothubtransportamqp.h		
     */
    
    /*
     * Original signature : void* AMQP_Protocol()
     */
    public Pointer AMQP_Protocol();
    
    /*
     *				iothubtransportamqp.h		
     */
    
    /*
     * Original signature : void* MQTT_Protocol()
     */
    public Pointer MQTT_Protocol();
    
    /*
     * Original signature : void* AMQP_Protocol_over_WebSocketsTls()
     */
    public Pointer AMQP_Protocol_over_WebSocketsTls();
    
    
    /*
     *				iothub_message.h		
     */
    
    /*
     * Original signature : IOTHUB_MESSAGE_HANDLE IoTHubMessage_CreateFromString(const char* source)
     */
    public IOTHUB_MESSAGE_HANDLE IoTHubMessage_CreateFromString(String source);
    
    /*
     * Original signature : IOTHUB_MESSAGE_HANDLE IoTHubMessage_CreateFromByteArray(const unsigned char* byteArray, size_t size)
     */
    public IOTHUB_MESSAGE_HANDLE IoTHubMessage_CreateFromByteArray(byte[] byteArray, int size);
    
    /*
     * Original signature : IOTHUB_MESSAGE_HANDLE IoTHubMessage_Clone(IOTHUB_MESSAGE_HANDLE iotHubMessageHandle)
     */
    public IOTHUB_MESSAGE_HANDLE IoTHubMessage_Clone(IOTHUB_MESSAGE_HANDLE iotHubMessageHandle);
    
    /*
     * Original signature : MAP_HANDLE IoTHubMessage_Properties(IOTHUB_MESSAGE_HANDLE iotHubMessageHandle)
     */
    public MAP_HANDLE IoTHubMessage_Properties(IOTHUB_MESSAGE_HANDLE iotHubMessageHandle);
    
    /*
     * Original signature : const char* IoTHubMessage_GetString(IOTHUB_MESSAGE_HANDLE iotHubMessageHandle)
     */
    public String IoTHubMessage_GetString(IOTHUB_MESSAGE_HANDLE iotHubMessageHandle);
    
    /*
     * Original signature : IOTHUB_MESSAGE_RESULT IoTHubMessage_GetByteArray(IOTHUB_MESSAGE_HANDLE, const unsigned char**, size_t* )
     */
    public int IoTHubMessage_GetByteArray(IOTHUB_MESSAGE_HANDLE iotHubMessageHandle, PointerByReference buffer, IntByReference size);
    
    /*
     * Original signature : IOTHUBMESSAGE_CONTENT_TYPE IoTHubMessage_GetContentType(IOTHUB_MESSAGE_HANDLE )
     */
    public IOTHUBMESSAGE_CONTENT_TYPE IoTHubMessage_GetContentType(IOTHUB_MESSAGE_HANDLE iotHubMessageHandle);
    
    /*
     * Original signature : const char* IoTHubMessage_GetMessageId(IOTHUB_MESSAGE_HANDLE )
     */
    public String IoTHubMessage_GetMessageId(IOTHUB_MESSAGE_HANDLE iotHubMessageHandle);
    
    /*
     * Original signature : IOTHUB_MESSAGE_RESULT IoTHubMessage_SetMessageId(IOTHUB_MESSAGE_HANDLE, const char* )
     */
    public int IoTHubMessage_SetMessageId(IOTHUB_MESSAGE_HANDLE iotHubMessageHandle, String messageId);
    
    /*
     * Original signature : const char* IoTHubMessage_GetCorrelationId(IOTHUB_MESSAGE_HANDLE )
     */
    public String IoTHubMessage_GetCorrelationId(IOTHUB_MESSAGE_HANDLE iotHubMessageHandle);
    
    /*
     * Original signature : IOTHUB_MESSAGE_RESULT IoTHubMessage_SetCorrelationId(IOTHUB_MESSAGE_HANDLE, const char* )
     */
    public int IoTHubMessage_SetCorrelationId(IOTHUB_MESSAGE_HANDLE iotHubMessageHandle, String correlationId);
    
    /*
     * Original signature : void IoTHubMessage_Destroy(IOTHUB_MESSAGE_HANDLE iotHubMessageHandle)
     */
    public void IoTHubMessage_Destroy(IOTHUB_MESSAGE_HANDLE iotHubMessageHandle);
    
    /*
     *				map.h		
     */
    
    /*
     * Original signature : MAP_RESULT Map_AddOrUpdate(MAP_HANDLE, const char*, const char* )
     */
    public MAP_HANDLE Map_Create(MapFilterCallback mapFilterFunc);
    
    /*
     * Original signature : Map_Destroy(MAP_HANDLE )
     */
    public void Map_Destroy(MAP_HANDLE handle);
    
    /*
     * Original signature : MAP_HANDLE Map_Clone(MAP_HANDLE )
     */
    public MAP_HANDLE Map_Clone(MAP_HANDLE handle);
    
    /*
     * Original signature : MAP_RESULT Map_Add(MAP_HANDLE , const char* , const char* )
     */
    public int Map_Add(MAP_HANDLE handle, String key, String value);
    
    /*
     * Original signature : MAP_RESULT Map_AddOrUpdate(MAP_HANDLE, const char*, const char* )
     */
    public int Map_AddOrUpdate(MAP_HANDLE handle, String key, String value);
        
    /*
     * Original signature : MAP_RESULT Map_Delete(MAP_HANDLE , const char* )
     */
    public int Map_Delete(MAP_HANDLE handle, String key);
    
    /*
     * Original signature : MAP_RESULT Map_ContainsValue(MAP_HANDLE, const char* , bool* )
     */
    public int Map_ContainsValue(MAP_HANDLE handle, String value, IntByReference valueExists);
    
    /*
     * Original signature : MAP_RESULT Map_ContainsKey(MAP_HANDLE handle, const char* key, bool* keyExists)
     */
    public int Map_ContainsKey(MAP_HANDLE handle, String key, IntByReference valueExists);
    
    /*
     * Original signature : const char* Map_GetValueFromKey(MAP_HANDLE, const char* )
     */
    public String Map_GetValueFromKey(MAP_HANDLE handle, String key);
    
    /*
     * Original signature : MAP_RESULT Map_GetInternals(MAP_HANDLE, const char*const**, const char*const**, size_t* )
     */
    public int Map_GetInternals(MAP_HANDLE handle, PointerByReference[] keys, PointerByReference[] values, IntByReference count);
    
    /*
     *				platform.h
     */
    
    /*
     * Original signature : int platform_init(void)
     */
    public int platform_init();
    
    /*
     * Original signature : void platform_deinit(void)
     */
    public void platform_deinit();
    
    
    public interface IotHubEventCallback extends Callback {
         void execute(int status, Pointer userContextCallback);
    };
    
    public static interface IotHubMessageCallback extends Callback {
         int execute(IOTHUB_MESSAGE_HANDLE message, Pointer userContextCallback);
    };
    
    public interface MapFilterCallback extends Callback {
         int execute(String mapProperty, String mapValue);
    };

    public interface IotHubFileUploadCallback extends Callback {
        void execute(int status, Pointer userContextCallback);
    };

    public static class IOTHUB_MESSAGE_HANDLE extends PointerType {
        public IOTHUB_MESSAGE_HANDLE(Pointer address) {
            super(address);
        }
        public IOTHUB_MESSAGE_HANDLE() {
            super();
        }
    };
    
    public static class MAP_HANDLE extends PointerType {
        public MAP_HANDLE(Pointer address) {
            super(address);
        }
        public MAP_HANDLE() {
            super();
        }
    };
    
    public static class TRANSPORT_HANDLE extends PointerType {
        public TRANSPORT_HANDLE(Pointer address) {
            super(address);
        }
        public TRANSPORT_HANDLE() {
            super();
        }
    };
    
    public static interface IOTHUB_CLIENT_TRANSPORT_PROVIDER extends Callback {
        Pointer execute();
    };
    
    public class HTTP_Protocol implements IOTHUB_CLIENT_TRANSPORT_PROVIDER {
        //@Override
        public Pointer execute() {
            return Iothub_client_wrapperLibrary.INSTANCE.HTTP_Protocol();	
        }
    }
    
    public class AMQP_Protocol implements IOTHUB_CLIENT_TRANSPORT_PROVIDER {
        //@Override
        public Pointer execute() {
            return Iothub_client_wrapperLibrary.INSTANCE.AMQP_Protocol();	
        }
    }
    
    public class MQTT_Protocol implements IOTHUB_CLIENT_TRANSPORT_PROVIDER {
        //@Override
        public Pointer execute() {
            return Iothub_client_wrapperLibrary.INSTANCE.MQTT_Protocol();	
        }
    }
    
    public class AMQP_Protocol_over_WebSocketsTls implements IOTHUB_CLIENT_TRANSPORT_PROVIDER {
        //@Override
        public Pointer execute() {
            return Iothub_client_wrapperLibrary.INSTANCE.AMQP_Protocol_over_WebSocketsTls();	
        }
    }
    
    public static class IOTHUB_CLIENT_HANDLE extends PointerType {
        public IOTHUB_CLIENT_HANDLE(Pointer address) {
            super(address);
        }
        public IOTHUB_CLIENT_HANDLE() {
            super();
        }
    };
    public static class IOTHUBTRANSPORT_CONFIG extends PointerType {
        public IOTHUBTRANSPORT_CONFIG(Pointer address) {
            super(address);
        }
        public IOTHUBTRANSPORT_CONFIG() {
            super();
        }
    };
    
    public static class IOTHUB_CLIENT_CONFIG extends Structure //implements Structure.ByReference
    {
        Object protocol;
        
        public String deviceId;
        
        public String deviceKey;

        public String iotHubName;
        
        public String iotHubSuffix;

        public String protocolGatewayHostName;
        
        public IOTHUB_CLIENT_CONFIG (Pointer p)
        {
            super(p);
            read();
        }
        
        public IOTHUB_CLIENT_CONFIG()
        {
            super();
        }
        @Override
        protected List<String> getFieldOrder() 
        {
            return Arrays.asList(new String[]{"deviceId","deviceKey","iotHubName","iotHubSuffix","protocolGatewayHostName"});
        }
    };
    
    public static interface IOTHUB_CLIENT_RESULT 
    {
        public static final int IOTHUB_CLIENT_OK = 0;
        public static final int IOTHUB_CLIENT_INVALID_ARG = 1;
        public static final int IOTHUB_CLIENT_ERROR = 2;
        public static final int IOTHUB_CLIENT_INVALID_SIZE = 3;
        public static final int IOTHUB_CLIENT_INDEFINITE_TIME = 4;
    }    
    
    public static interface IOTHUB_MESSAGE_RESULT 
    {
        public static final int IOTHUB_MESSAGE_OK = 0;
        public static final int IOTHUB_MESSAGE_INVALID_ARG = 1;
        public static final int IOTHUB_MESSAGE_INVALID_TYPE = 2;
        public static final int IOTHUB_MESSAGE_ERROR = 3;
    }    
    
    public static interface IOTHUBMESSAGE_CONTENT_TYPE 
    {
        public static final int IOTHUBMESSAGE_BYTEARRAY = 0;
        public static final int IOTHUBMESSAGE_STRING = 1;
        public static final int IOTHUBMESSAGE_UNKNOWN = 2;
    }    
    
    public static interface MAP_RESULT
    {
        public static final int MAP_OK = 0;
        public static final int MAP_ERROR = 1;
        public static final int MAP_INVALIDARG = 2;
        public static final int MAP_KEYEXISTS = 3;
        public static final int MAP_KEYNOTFOUND = 4;
        public static final int MAP_FILTER_REJECT = 5;
    }    
    
    public static class time_t extends IntegerType 
    {
        public time_t() { this(0); }

        public time_t(long value) { super(Native.LONG_SIZE, value); }
    }
    
}





