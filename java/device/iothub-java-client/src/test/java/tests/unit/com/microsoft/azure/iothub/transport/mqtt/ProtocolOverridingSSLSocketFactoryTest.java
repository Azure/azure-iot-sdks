// Copyright (c) Microsoft. All rights reserved.
// Licensed under the MIT license. See LICENSE file in the project root for full license information.

package tests.unit.com.microsoft.azure.iothub.transport.mqtt;

import static org.hamcrest.core.Is.is;
import static org.junit.Assert.assertNotNull;
import static org.junit.Assert.assertThat;
import javax.net.ssl.SSLSocketFactory;
import javax.naming.spi.DirStateFactory.Result;
import javax.net.ssl.SSLSocket;
import java.net.InetAddress;
import java.net.Socket;
import java.lang.String;
import java.lang.IllegalArgumentException;
import com.microsoft.azure.iothub.transport.mqtt.ProtocolOverridingSSLSocketFactory;
import mockit.Deencapsulation;
import mockit.Mocked;
import mockit.NonStrictExpectations;
import mockit.Verifications;
import org.eclipse.paho.client.mqttv3.*;
import org.junit.Test;
import org.omg.CORBA.Any;

import java.io.IOException;
import java.net.URLEncoder;
import java.util.Queue;

import static org.junit.Assert.assertEquals;
import static org.junit.Assert.assertNull;

public class ProtocolOverridingSSLSocketFactoryTest {
	
	@Mocked
	protected SSLSocketFactory mockDelegate = (SSLSocketFactory)(SSLSocketFactory.getDefault());
	
	@Mocked
	protected String[] mockEnabledProtocols = new String[]{"TLSv1"};
	
	@Mocked
	protected SSLSocket mockSocket;
	
	@Mocked
	protected String mockHost = "test.host";
	
	@Mocked
	protected int mockPort = 1;
	
	@Mocked
	protected InetAddress mockLocalInetAddress;
	
	@Mocked
	protected int mockLocalPort = 1;
	
	@Mocked
	protected InetAddress mockHostInetAddress;
	
	@Mocked
	protected boolean mockAutoClose;
	
	// Tests_SRS_PROTOCOLOVERRIDINGSSLSOCKETFACTORYTEST_15_001: [The constructor shall save the SSLSocketFactory and the enabled protocols.]
    @Test
    public void constructorSavesCorrectSSLSocketFactoryAndenabledProtocols() throws IOException {
    	ProtocolOverridingSSLSocketFactory factory = new ProtocolOverridingSSLSocketFactory(mockDelegate, mockEnabledProtocols);
    	
    	SSLSocketFactory actualFactory = Deencapsulation.getField(factory, "underlyingSSLSocketFactory");
    	SSLSocketFactory expectedFactory = mockDelegate;
    	
    	String[] actualEnabledProtocols = Deencapsulation.getField(factory, "enabledProtocols");
    	String[] expectedProtocols = mockEnabledProtocols;
    	
    	assertEquals(expectedFactory, actualFactory);
    	assertEquals(actualEnabledProtocols, expectedProtocols);
    }
	
	// Tests_SRS_PROTOCOLOVERRIDINGSSLSOCKETFACTORYTEST_15_002: [The constructor shall throw a new IllegalArgumentException
    // if the SSLSocketFactory parameter is null or empty.]
    @Test(expected = IllegalArgumentException.class)
    public void constructorThrowsIllegalArgumentExceptionIfDelegateIsNull(){
    	final SSLSocketFactory delegate = null;
    	final String[] enabledProtocols = new String[]{"TLSv1"};
        new ProtocolOverridingSSLSocketFactory(delegate, enabledProtocols);
    }
    
    // Tests_SRS_PROTOCOLOVERRIDINGSSLSOCKETFACTORYTEST_15_003: [The constructor shall throw a new IllegalArgumentException
    // if the enabled protocols parameter is null or empty.]
    @Test(expected = IllegalArgumentException.class)
    public void constructorThrowsIllegalArgumentExceptionIfEnabledProtocolsIsEmpty(){
    	final SSLSocketFactory delegate = mockDelegate;
    	final String[] enabledProtocols = null;
        new ProtocolOverridingSSLSocketFactory(delegate, enabledProtocols);
    }
    
    // Tests_SRS_PROTOCOLOVERRIDINGSSLSOCKETFACTORYTEST_15_004: [The function shall return the default cipher suites
    // of the saved socket factory.]
    @Test
    public void getDefaultCipherSuitesCorrect() throws IOException {
    	ProtocolOverridingSSLSocketFactory factory = new ProtocolOverridingSSLSocketFactory(mockDelegate, mockEnabledProtocols);
    	
    	SSLSocketFactory actualFactory = Deencapsulation.getField(factory, "underlyingSSLSocketFactory");
    	
    	String[] actualDefaultCipher = actualFactory.getDefaultCipherSuites();
    	String[] expectedDefalutCipher = mockDelegate.getDefaultCipherSuites();
    	
    	assertEquals(expectedDefalutCipher, actualDefaultCipher);
    }
    
    // Tests_SRS_PROTOCOLOVERRIDINGSSLSOCKETFACTORYTEST_15_005: [The function shall return the supported cipher suites
    // of the saved socket factory.]
    @Test
    public void getSupportedCipherCorrect() throws IOException {
    	ProtocolOverridingSSLSocketFactory factory = new ProtocolOverridingSSLSocketFactory(mockDelegate, mockEnabledProtocols);
    	
    	SSLSocketFactory actualFactory = Deencapsulation.getField(factory, "underlyingSSLSocketFactory");
    	
    	String[] actualSupportedCipher = actualFactory.getSupportedCipherSuites();
    	String[] expectedSupportedCipher = mockDelegate.getSupportedCipherSuites();
    	
    	assertEquals(expectedSupportedCipher, actualSupportedCipher);
    }
    
    // Tests_SRS_PROTOCOLOVERRIDINGSSLSOCKETFACTORYTEST_15_006: [The function shall return a new socket object
    // over the saved enabled protocols.]
  	@Test
    public void createSocketEnabledProtocolsCorrectBySocketAndHostAndPortAndAutoClose() throws IOException {
  		new NonStrictExpectations()
        {
            {
            	mockSocket.isConnected();
            	result = true;
            }
        };
        
  		ProtocolOverridingSSLSocketFactory factory = new ProtocolOverridingSSLSocketFactory(mockDelegate, mockEnabledProtocols);
  		
  		final Socket actualSocket = factory.createSocket(mockSocket, mockHost, mockPort, mockAutoClose);
    	assertEquals(((SSLSocket) actualSocket).getEnabledProtocols(), mockEnabledProtocols);
    }

    
    // Tests_SRS_PROTOCOLOVERRIDINGSSLSOCKETFACTORYTEST_15_007: [The function shall throw a IllegalArgumentException
    // if the socket object is null.]
    @Test(expected = IllegalArgumentException.class)
    public void createSocketWithSocketClientDoesNotSetSocket() throws IOException {
    	ProtocolOverridingSSLSocketFactory factory = new ProtocolOverridingSSLSocketFactory(mockDelegate, mockEnabledProtocols);
    	factory.createSocket(null, mockHost, mockPort, mockAutoClose);
    }
    
    // Tests_SRS_PROTOCOLOVERRIDINGSSLSOCKETFACTORYTEST_15_008: [The function shall throw a IllegalArgumentException
    // if the host is null.]
    @Test(expected = IllegalArgumentException.class)
    public void createSocketWithSocketClientDoesNotSetHost() throws IOException {
    	ProtocolOverridingSSLSocketFactory factory = new ProtocolOverridingSSLSocketFactory(mockDelegate, mockEnabledProtocols);
    	final Socket ActualSocket = factory.createSocket(mockSocket, null, mockPort, mockAutoClose);
    }
    
    // Tests_SRS_PROTOCOLOVERRIDINGSSLSOCKETFACTORYTEST_15_009: [The function shall throw a IllegalArgumentException
    // if the port is smaller than 0.]
    @Test(expected = IllegalArgumentException.class)
    public void createSocketWithSocketClientErrorPort() throws IOException {
    	ProtocolOverridingSSLSocketFactory factory = new ProtocolOverridingSSLSocketFactory(mockDelegate, mockEnabledProtocols);
    	final Socket ActualSocket = factory.createSocket(mockSocket, mockHost, 0, mockAutoClose);
    }
    
    // Tests_SRS_PROTOCOLOVERRIDINGSSLSOCKETFACTORYTEST_15_010: [The function shall return a new socket object
    // over the saved enabled protocols.]
    @Test
    public void createSocketEnabledProtocolsCorrectByNoParameter() throws IOException {
    	 
         ProtocolOverridingSSLSocketFactory factory = new ProtocolOverridingSSLSocketFactory(mockDelegate, mockEnabledProtocols);
         final Socket actualSocket = factory.createSocket();
         assertEquals(((SSLSocket) actualSocket).getEnabledProtocols(), mockEnabledProtocols);
    }
    
    // Tests_SRS_PROTOCOLOVERRIDINGSSLSOCKETFACTORYTEST_15_011: [The function shall return a new socket object
    // over the saved enabled protocols.]
    @Test
    public void createSocketEnabledProtocolsCorrectByHostAndPort() throws IOException {
    	
    	ProtocolOverridingSSLSocketFactory factory = new ProtocolOverridingSSLSocketFactory(mockDelegate, mockEnabledProtocols);
    	final Socket actualSocket = factory.createSocket(mockHost, mockPort);
    	assertEquals(((SSLSocket) actualSocket).getEnabledProtocols(), mockEnabledProtocols);
    }

    // Tests_SRS_PROTOCOLOVERRIDINGSSLSOCKETFACTORYTEST_15_012: [The function shall throw a IllegalArgumentException
    // if the host is null.]
    @Test(expected = IllegalArgumentException.class)
    public void createSocketWithHostAndPortDoesNotSetHost() throws IOException {
    	ProtocolOverridingSSLSocketFactory factory = new ProtocolOverridingSSLSocketFactory(mockDelegate, mockEnabledProtocols);
    	mockHost = null;
    	final Socket ActualSocket = factory.createSocket(mockHost, mockPort);
    }
    
    // Tests_SRS_PROTOCOLOVERRIDINGSSLSOCKETFACTORYTEST_15_013: [The function shall throw a IllegalArgumentException
    // if port is smaller than 0.]
    @Test(expected = IllegalArgumentException.class)
    public void createSocketWithHostAndPortByErrorPort() throws IOException {
    	ProtocolOverridingSSLSocketFactory factory = new ProtocolOverridingSSLSocketFactory(mockDelegate, mockEnabledProtocols);
    	final Socket ActualSocket = factory.createSocket(mockHost, 0);
    }
    
    // Tests_SRS_PROTOCOLOVERRIDINGSSLSOCKETFACTORYTEST_15_014: [The function shall return a new socket object
    // over the saved enabled protocols.]
    @Test
    public void createSocketEnabledProtocolsCorrectByHostAndPortAndLocalInetAddressAndLocalPort() throws IOException {
    	ProtocolOverridingSSLSocketFactory factory = new ProtocolOverridingSSLSocketFactory(mockDelegate, mockEnabledProtocols);
    	final Socket actualSocket = factory.createSocket(mockHost, mockPort, mockLocalInetAddress, mockLocalPort);
    	assertEquals(((SSLSocket) actualSocket).getEnabledProtocols(), mockEnabledProtocols);
    }

    // Tests_SRS_PROTOCOLOVERRIDINGSSLSOCKETFACTORYTEST_15_015: [The function shall throw a IllegalArgumentException
    // if host is null.]
    @Test(expected = IllegalArgumentException.class)
    public void createSocketWithHostAndPortAndLocalAddressAndLocalPortDoesNotSetHost() throws IOException {
    	ProtocolOverridingSSLSocketFactory factory = new ProtocolOverridingSSLSocketFactory(mockDelegate, mockEnabledProtocols);
    	mockHost = null;
    	final Socket ActualSocket = factory.createSocket(mockHost, mockPort, mockLocalInetAddress, mockLocalPort);
    }
    
    // Tests_SRS_PROTOCOLOVERRIDINGSSLSOCKETFACTORYTEST_15_016: [The function shall throw a IllegalArgumentException
    // if port is smaller than 0.]
    @Test(expected = IllegalArgumentException.class)
    public void createSocketWithHostAndPortAndLocalAddressAndLocalPortByErrorPort() throws IOException {
    	ProtocolOverridingSSLSocketFactory factory = new ProtocolOverridingSSLSocketFactory(mockDelegate, mockEnabledProtocols);
    	final Socket ActualSocket = factory.createSocket(mockHost, 0, mockLocalInetAddress, mockLocalPort);
    }
    
    // Tests_SRS_PROTOCOLOVERRIDINGSSLSOCKETFACTORYTEST_15_017: [The function shall throw a IllegalArgumentException
    // if localAddress is null.]
    @Test(expected = IllegalArgumentException.class)
    public void createSocketWithHostAndPortAndLocalAddressAndLocalPortDoesNotSetLocalAddress() throws IOException {
    	ProtocolOverridingSSLSocketFactory factory = new ProtocolOverridingSSLSocketFactory(mockDelegate, mockEnabledProtocols);
    	final Socket ActualSocket = factory.createSocket(mockHost, mockPort, null, mockLocalPort);
    }
    
    // Tests_SRS_PROTOCOLOVERRIDINGSSLSOCKETFACTORYTEST_15_018: [The function shall throw a IllegalArgumentException
    // if localPort is smaller than 0.]
    @Test(expected = IllegalArgumentException.class)
    public void createSocketWithHostAndPortAndLocalAddressAndLocalPortByErrorLocalPort() throws IOException {
    	ProtocolOverridingSSLSocketFactory factory = new ProtocolOverridingSSLSocketFactory(mockDelegate, mockEnabledProtocols);
    	final Socket ActualSocket = factory.createSocket(mockHost, mockPort, mockLocalInetAddress, 0);
    }
    
    // Tests_SRS_PROTOCOLOVERRIDINGSSLSOCKETFACTORYTEST_15_019: [The function shall return a new socket object
    // over the saved enabled protocols.]
    @Test
    public void createSocketEnabledProtocolsCorrectByHostInetAddressAndPort() throws IOException {
    	ProtocolOverridingSSLSocketFactory factory = new ProtocolOverridingSSLSocketFactory(mockDelegate, mockEnabledProtocols);
    	final Socket actualSocket = factory.createSocket(mockHostInetAddress, mockPort);
    	assertEquals(((SSLSocket) actualSocket).getEnabledProtocols(), mockEnabledProtocols);
    }
    
    // Tests_SRS_PROTOCOLOVERRIDINGSSLSOCKETFACTORYTEST_15_020: [The function shall throw a IllegalArgumentException
    // if the hostInetAddress is null.]
    @Test(expected = IllegalArgumentException.class)
    public void createSocketWithHostAddressAndPortDoesNotSetHost() throws IOException {
    	ProtocolOverridingSSLSocketFactory factory = new ProtocolOverridingSSLSocketFactory(mockDelegate, mockEnabledProtocols);
    	mockHostInetAddress = null;
    	final Socket ActualSocket = factory.createSocket(mockHostInetAddress, mockPort);
    }
    
    // Tests_SRS_PROTOCOLOVERRIDINGSSLSOCKETFACTORYTEST_15_021: [The function shall throw a IllegalArgumentException
    // if the localPort is smaller than 0.]
    @Test(expected = IllegalArgumentException.class)
    public void createSocketWithHostAddressAndPortByErrorPort() throws IOException {
    	ProtocolOverridingSSLSocketFactory factory = new ProtocolOverridingSSLSocketFactory(mockDelegate, mockEnabledProtocols);
    	InetAddress host = null;
    	final Socket ActualSocket = factory.createSocket(mockHostInetAddress, 0);
    }
    // Tests_SRS_PROTOCOLOVERRIDINGSSLSOCKETFACTORYTEST_15_022: [The function shall return a new socket object
    // over the saved enabled protocols.]
    @Test
    public void createSocketEnabledProtocolsCorrectByHostInetAddressAndPortAndLocalInetAddressAndLocalPort() throws IOException {
    	ProtocolOverridingSSLSocketFactory factory = new ProtocolOverridingSSLSocketFactory(mockDelegate, mockEnabledProtocols);
        final Socket actualSocket = factory.createSocket(mockHostInetAddress, mockPort, mockLocalInetAddress, mockLocalPort);
        assertEquals(((SSLSocket) actualSocket).getEnabledProtocols(), mockEnabledProtocols);
    }
    
    // Tests_SRS_PROTOCOLOVERRIDINGSSLSOCKETFACTORYTEST_15_023: [The function shall throw a IllegalArgumentException
    // if the hostInetAddress is null.]
    @Test(expected = IllegalArgumentException.class)
    public void createSocketWithHostInetAddressAndPortAndLocalAddressAndLocalPortDoesNotSetHost() throws IOException {
    	ProtocolOverridingSSLSocketFactory factory = new ProtocolOverridingSSLSocketFactory(mockDelegate, mockEnabledProtocols);
    	mockHostInetAddress = null;
    	final Socket ActualSocket = factory.createSocket(mockHostInetAddress, mockPort, mockLocalInetAddress, mockLocalPort);
    }

    // Tests_SRS_PROTOCOLOVERRIDINGSSLSOCKETFACTORYTEST_15_024: [The function shall throw a IllegalArgumentException
    // if the port is smaller than 0.]
    @Test(expected = IllegalArgumentException.class)
    public void createSocketWithHostInetAddressAndPortAndLocalAddressAndLocalPortByErrorPort() throws IOException {
    	ProtocolOverridingSSLSocketFactory factory = new ProtocolOverridingSSLSocketFactory(mockDelegate, mockEnabledProtocols);
    	mockHostInetAddress = null;
    	final Socket ActualSocket = factory.createSocket(mockHostInetAddress, 0, mockLocalInetAddress, mockLocalPort);
    }
    
    // Tests_SRS_PROTOCOLOVERRIDINGSSLSOCKETFACTORYTEST_15_025: [The function shall throw a IllegalArgumentException
    // if the localInetAddress is null.]
    @Test(expected = IllegalArgumentException.class)
    public void createSocketWithHostInetAddressAndPortAndLocalAddressAndLocalPortDoesNotSetLocalAddress() throws IOException {
    	ProtocolOverridingSSLSocketFactory factory = new ProtocolOverridingSSLSocketFactory(mockDelegate, mockEnabledProtocols);
    	final Socket ActualSocket = factory.createSocket(mockHostInetAddress, mockPort, null, mockLocalPort);
    }
    
    // Tests_SRS_PROTOCOLOVERRIDINGSSLSOCKETFACTORYTEST_15_026: [The function shall throw a IllegalArgumentException
    // if the localPort is smaller than 0.]
    @Test(expected = IllegalArgumentException.class)
    public void createSocketWithHostInetAddressAndPortAndLocalAddressAndLocalPortByErrorLocalPort() throws IOException {
    	ProtocolOverridingSSLSocketFactory factory = new ProtocolOverridingSSLSocketFactory(mockDelegate, mockEnabledProtocols);
    	final Socket ActualSocket = factory.createSocket(mockHostInetAddress, mockPort, mockLocalInetAddress, 0);
    }
}
