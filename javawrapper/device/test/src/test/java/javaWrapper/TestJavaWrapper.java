package javaWrapper;

import java.util.HashMap;

import static org.hamcrest.CoreMatchers.is;
import static org.junit.Assert.*;

import org.junit.Test;

import com.sun.jna.Pointer;
import com.sun.jna.ptr.IntByReference;

import javaWrapper.Iothub_client_wrapperLibrary.IOTHUB_CLIENT_RESULT;
import javaWrapper.Iothub_client_wrapperLibrary.IOTHUB_MESSAGE_HANDLE;
import javaWrapper.Iothub_client_wrapperLibrary.IOTHUB_MESSAGE_RESULT;
import javaWrapper.Iothub_client_wrapperLibrary.IotHubEventCallback;
import javaWrapper.Iothub_client_wrapperLibrary.IotHubMessageCallback;
import javaWrapper.Iothub_client_wrapperLibrary.IotHubFileUploadCallback;
import javaWrapper.Iothub_client_wrapperLibrary.MAP_HANDLE;
import javaWrapper.Iothub_client_wrapperLibrary.MAP_RESULT;
import javaWrapper.Iothub_client_wrapperLibrary.MapFilterCallback;
import javaWrapper.DeviceClient.IotHubClientProtocol;
import javaWrapper.LoadTestLibrary;
 
public class TestJavaWrapper 
{
    String connectionString = "fake connection string";
    IotHubClientProtocol protocol = null;
    LoadTestLibrary test = new LoadTestLibrary(true);

    protected static class FileUploadCallback
            implements IotHubFileUploadCallback
    {
        public void execute(int status, Pointer context)
        {

        }
    }

    protected static class MessageCallback
        implements IotHubMessageCallback
    {
        public int execute(IOTHUB_MESSAGE_HANDLE msg, Pointer context)
        {
            return IotHubMessageResult.COMPLETE.ordinal();
        }
    }

    protected static class EventCallback 
        implements IotHubEventCallback
    {
        public void execute(int status, Pointer context)
        {
            
        }
    }
    
    protected static class MapCallback 
        implements MapFilterCallback
    {
        public int execute(String mapProperty, String mapValue)
        {
            int result = 0;
            
            return result;
        }
    }
    
    /*
     * Test enum definition
     */
    
    @Test
    public void testIotHubClientResultEnumDefinition() 
    {		
        assertEquals(IOTHUB_CLIENT_RESULT.IOTHUB_CLIENT_OK, 0);
        assertEquals(IOTHUB_CLIENT_RESULT.IOTHUB_CLIENT_INVALID_ARG, 1);
        assertEquals(IOTHUB_CLIENT_RESULT.IOTHUB_CLIENT_ERROR, 2);
        assertEquals(IOTHUB_CLIENT_RESULT.IOTHUB_CLIENT_INVALID_SIZE, 3);
        assertEquals(IOTHUB_CLIENT_RESULT.IOTHUB_CLIENT_INDEFINITE_TIME, 4);
    }
    
    @Test
    public void testIotHubMessageResultEnumDefinition() 
    {
        assertEquals(IOTHUB_MESSAGE_RESULT.IOTHUB_MESSAGE_OK, 0);
        assertEquals(IOTHUB_MESSAGE_RESULT.IOTHUB_MESSAGE_INVALID_ARG, 1);
        assertEquals(IOTHUB_MESSAGE_RESULT.IOTHUB_MESSAGE_INVALID_TYPE, 2);
        assertEquals(IOTHUB_MESSAGE_RESULT.IOTHUB_MESSAGE_ERROR, 3);
    }
    
    @Test
    public void testIotHubMapResultEnumDefinition() 
    {
        assertEquals(MAP_RESULT.MAP_OK, 0);
        assertEquals(MAP_RESULT.MAP_ERROR, 1);
        assertEquals(MAP_RESULT.MAP_INVALIDARG, 2);
        assertEquals(MAP_RESULT.MAP_KEYEXISTS, 3);
        assertEquals(MAP_RESULT.MAP_KEYNOTFOUND, 4);
        assertEquals(MAP_RESULT.MAP_FILTER_REJECT, 5);
    }
    
    /*
     * Test Message
     */
    
    @Test(expected = IllegalArgumentException.class) 
    public void constructorRejectsNullHandle() 
    { 
        final IOTHUB_MESSAGE_HANDLE handle = null; 
    
        new Message(handle); 
    } 
    
    @Test 
    public void TestMessageBody() 
    { 
        final byte[] body = { 1, 2, 3 };

        Message msg = new Message(body); 
        
        assertTrue(msg instanceof Message); 
    } 

    @Test 
    public void TestConstructorSavesBodyString() 
    { 
        String body = "123"; 

        Message msg = new Message(body); 
    
        String testBody = msg.getString();
        assertThat(testBody, is(body)); 
    } 
    
    @Test 
    public void TestConstructorNullString() 
    { 
        String body = null; 

        Message msg = new Message(body); 
    
        IOTHUB_MESSAGE_HANDLE handle = msg.getMessageHandle();
        assertEquals(null, handle); 
    } 
    
    @Test 
    public void TestConstructorSavesBodyCharArray() 
    { 
        byte[] body = {49,50,51}; 

        Message msg = new Message(body); 
        byte[] testBody = msg.getBytes(); 
    
        assertThat(testBody, is(body)); 
    } 

    @Test 
    public void TestPropertyNullParameter() 
    { 
        String body = "123"; 
        int result;

        Message msg = new Message(body); 
        result = msg.setProperty(null, null);
       
        assertEquals(result, MAP_RESULT.MAP_INVALIDARG); 
    } 
    
    @Test 
    public void TestProperty() 
    { 
        String body = "123"; 
        int result;

        Message msg = new Message(body); 
        result = msg.setProperty("key", "value");
        
        Map map = new Map();
        String value = map.mapGetValueFromKey("key");
        
        assertEquals(result, MAP_RESULT.MAP_OK); 
        assertEquals(value, "value"); 
    } 
    
    @Test 
    public void TestCorrelationId() 
    { 
        String body = "123"; 
        int result;
        String correlationId;

        Message msg = new Message(body); 
        result = msg.setCorrelationId("xyz");

        correlationId = msg.getCorrelationId();
        
        assertEquals(result, IOTHUB_MESSAGE_RESULT.IOTHUB_MESSAGE_OK); 
        assertEquals("xyz", correlationId); 
    } 
    
    @Test 
    public void TestCorrelationIdNullParameter() 
    { 
        String body = "123"; 
        int result;

        Message msg = new Message(body); 
        result = msg.setCorrelationId(null);
        
        assertEquals(result, IOTHUB_MESSAGE_RESULT.IOTHUB_MESSAGE_INVALID_ARG); 
    } 
    
    @Test 
    public void TestMessageId() 
    { 
        String body = "123"; 
        int result;
        String messageId;

        Message msg = new Message(body); 
        result = msg.setMessageId("xyz");

        messageId = msg.getMessageId();
        
        assertEquals(result, IOTHUB_MESSAGE_RESULT.IOTHUB_MESSAGE_OK); 
        assertEquals("xyz", messageId); 
    } 
    
    @Test 
    public void TestMessageIdNullParameter() 
    { 
        String body = "123"; 
        int result;

        Message msg = new Message(body); 
        result = msg.setMessageId(null);
        
        assertEquals(result, IOTHUB_MESSAGE_RESULT.IOTHUB_MESSAGE_INVALID_ARG);  
    } 
    
    @Test 
    public void TestClone() 
    { 
        String body = "123"; 
        
        Message msg = new Message(body); 
        msg.getMessageHandle();
        msg.clone();
        String body1 = msg.getString();
       
        assertEquals(body, body1); 
    } 
    
    /*
     * Test DeviceClient
     */
    
    @Test 
    public void TestDeviceClient() 
    { 
        DeviceClient client = new DeviceClient(connectionString, IotHubClientProtocol.HTTPS);
        assertTrue(client instanceof DeviceClient); 
        
        DeviceClient client1 = new DeviceClient(connectionString, IotHubClientProtocol.AMQPS);
        assertTrue(client1 instanceof DeviceClient); 
        
        DeviceClient client2 = new DeviceClient(connectionString, IotHubClientProtocol.MQTT);
        assertTrue(client2 instanceof DeviceClient); 	
        
        DeviceClient client3 = new DeviceClient(connectionString, IotHubClientProtocol.AMQP_WEB_SOCKET);
        assertTrue(client3 instanceof DeviceClient); 	 
    } 
    
    @Test (expected = IllegalStateException.class) 
    public void TestDeviceClientNullParameter() 
    { 
        DeviceClient client = new DeviceClient(null, IotHubClientProtocol.HTTPS);
    } 
    
    @Test 
    public void setMessageCallbackTest() 
    { 
        MessageCallback callback = new MessageCallback();
        Pointer context = null;
        int result;
        
        DeviceClient client = new DeviceClient(connectionString, IotHubClientProtocol.HTTPS);
       
        result = client.setMessageCallback(callback, context);
        assertEquals(result, IOTHUB_CLIENT_RESULT.IOTHUB_CLIENT_OK); 
    } 
    
    @Test 
    public void TestSendEventAsync() 
    { 
        EventCallback callback = new EventCallback();
        Message msg = new Message(); 
        Pointer context = null;
        int result;
        
        DeviceClient client = new DeviceClient(connectionString, IotHubClientProtocol.HTTPS);
       
        result = client.sendEventAsync(msg, callback, context);
        assertEquals(result, IOTHUB_CLIENT_RESULT.IOTHUB_CLIENT_OK); 
    } 
    
    @Test 
    public void TestSetOption() 
    { 
        DeviceClient client = new DeviceClient(connectionString, IotHubClientProtocol.HTTPS);
        int result;

        result = client.setOption("timeout", "241000");
        
        assertEquals(result, IOTHUB_CLIENT_RESULT.IOTHUB_CLIENT_OK); 
    }

    @Test
    public void TestUploadToBlobAsync()
    {
        DeviceClient client = new DeviceClient(connectionString, IotHubClientProtocol.HTTPS);
        int result;

        FileUploadCallback fileUploadCallback = new FileUploadCallback();
        Pointer usercontext = null;

        result = client.uploadToBlobAsync("filename", "content", 7, fileUploadCallback, usercontext);

        assertEquals(result, IOTHUB_CLIENT_RESULT.IOTHUB_CLIENT_OK);
    }

    @Test 
    public void TestSetOptionNullOptionName() 
    { 
        DeviceClient client = new DeviceClient(connectionString, IotHubClientProtocol.HTTPS);
        int result;

        result = client.setOption(null, "241000");
        
        assertEquals(result, IOTHUB_CLIENT_RESULT.IOTHUB_CLIENT_INVALID_ARG); 
    } 
    
    @Test 
    public void TestSetOptionNullValue() 
    { 
        DeviceClient client = new DeviceClient(connectionString, IotHubClientProtocol.HTTPS);
        int result;

        result = client.setOption("timeout", null);
        
        assertEquals(result, IOTHUB_CLIENT_RESULT.IOTHUB_CLIENT_INVALID_ARG); 
    } 
    
    /*
     * Test Map
     */
    
    @Test(expected = IllegalArgumentException.class) 
    public void TestConstructorMapRejectsNullHandle() 
    { 
        MAP_HANDLE handle = null; 
    
        new Map(handle); 
    } 
    
    @Test 
    public void TestMapAdd() 
    { 
        Map map = new Map(); 
        MapCallback callback = new MapCallback();
        int result;
        
        map.mapCreate(callback);
        result = map.mapAdd("key", "value");
        
        assertEquals(result, MAP_RESULT.MAP_OK); 
    } 
    
    @Test 
    public void TestMapAddNullKey() 
    { 
        Map map = new Map(); 
        MapCallback callback = new MapCallback();
        int result;
        
        map.mapCreate(callback);
        result = map.mapAdd(null, "value");
        
        assertEquals(result, MAP_RESULT.MAP_INVALIDARG); 
    } 
    
    @Test 
    public void TestMapAddNullValue() 
    { 
        Map map = new Map(); 
        MapCallback callback = new MapCallback();
        int result;
        
        map.mapCreate(callback);
        result = map.mapAdd("key", null);
        
        assertEquals(result, MAP_RESULT.MAP_INVALIDARG); 
    } 
    
    @Test 
    public void TestMapAddOrUpdate() 
    { 
        Map map = new Map(); 
        MapCallback callback = new MapCallback();
        int result;
        
        map.mapCreate(callback);
        result = map.mapAddOrUpdate("key", "value");
        
        assertEquals(result, MAP_RESULT.MAP_OK); 
    } 
    
    @Test 
    public void TestMapAddOrUpdateNullKey() 
    { 
        Map map = new Map(); 
        MapCallback callback = new MapCallback();
        int result;
        
        map.mapCreate(callback);
        result = map.mapAddOrUpdate(null, "value");
        
        assertEquals(result, MAP_RESULT.MAP_INVALIDARG); 
    } 
    
    @Test 
    public void TestMapAddOrUpdateNullValue() 
    { 
        Map map = new Map(); 
        MapCallback callback = new MapCallback();
        int result;
        
        map.mapCreate(callback);
        result = map.mapAddOrUpdate("key", null);
        
        assertEquals(result, MAP_RESULT.MAP_INVALIDARG); 
    } 
    
    @Test 
    public void TestMapDelete() 
    { 
        Map map = new Map(); 
        MapCallback callback = new MapCallback();
        int result;
        
        map.mapCreate(callback);
        map.mapAdd("key", "value");
        result = map.mapDelete("key");
        
        assertEquals(result, MAP_RESULT.MAP_OK); 
    } 
    
    @Test 
    public void TestMapDeleteNullParameter() 
    { 
        Map map = new Map(); 
        MapCallback callback = new MapCallback();
        int result;
        
        map.mapCreate(callback);
        map.mapAdd("key", "value");
        result = map.mapDelete(null);
        
        assertEquals(result, MAP_RESULT.MAP_INVALIDARG); 
    } 
    
    @Test 
    public void TestMapContainsValue() 
    { 
        Map map = new Map(); 
        MapCallback callback = new MapCallback();
        IntByReference valueExists = new IntByReference(0);
        int result;
        
        map.mapCreate(callback);
        map.mapAdd("key", "value");
        result = map.mapContainsValue("value", valueExists);
        
        assertEquals(result, MAP_RESULT.MAP_OK); 
        assertEquals(valueExists.getValue(), 1); 
    } 
    
    @Test 
    public void TestMapContainsValueNullParameter() 
    { 
        Map map = new Map(); 
        MapCallback callback = new MapCallback();
        IntByReference valueExists = new IntByReference(0);
        int result;
        
        map.mapCreate(callback);
        map.mapAdd("key", "value");
        result = map.mapContainsValue(null, valueExists);
        
        assertEquals(result, MAP_RESULT.MAP_INVALIDARG); 
    } 
    
    @Test 
    public void TestMapContainsKey() 
    { 
        Map map = new Map(); 
        MapCallback callback = new MapCallback();
        IntByReference valueExists = new IntByReference(0);
        int result;
        
        map.mapCreate(callback);
        map.mapAdd("key", "value");
        result = map.mapContainsKey("key", valueExists);
        
        assertEquals(result, MAP_RESULT.MAP_OK); 
        assertEquals(valueExists.getValue(), 1); 
    } 
    
    @Test 
    public void TestMapContainsKeyNullParameter() 
    { 
        Map map = new Map(); 
        MapCallback callback = new MapCallback();
        IntByReference valueExists = new IntByReference(0);
        int result;
        
        map.mapCreate(callback);
        map.mapAdd("key", "value");
        result = map.mapContainsKey(null, valueExists);
        
        assertEquals(result, MAP_RESULT.MAP_INVALIDARG); 
    } 
    
    @Test 
    public void TestMapGetValueFromKey() 
    { 
        Map map = new Map(); 
        MapCallback callback = new MapCallback();
        String value;
        
        map.mapCreate(callback);
        map.mapAdd("key", "value");
        value = map.mapGetValueFromKey("key");
        
        assertEquals(value, "value"); 
    } 
    
    @Test 
    public void TestMapCreateAndClone() 
    { 
        Map map = new Map(); 
        MapCallback callback = new MapCallback();
        int result;
        IntByReference valueExists = new IntByReference(0);
        
        map.mapCreate(callback);
        map.mapAdd("key", "value");
        map.mapClone();

        result = map.mapContainsValue("value", valueExists);
        
        assertEquals(result, MAP_RESULT.MAP_OK); 
    } 
    
    @Test 
    public void TestMapGetInternals() 
    { 
        Map map = new Map(); 
        MapCallback callback = new MapCallback();
        String key = "key";
        HashMap keyPairValue = new HashMap();
        
        map.mapCreate(callback);
        map.mapAdd("key", "value");
        keyPairValue = map.mapGetInternals(1);
        
        assertEquals(keyPairValue.size(), 1); 
        assertEquals(keyPairValue.get(key) , "value"); 
    } 
}

