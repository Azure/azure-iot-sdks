// Copyright (c) Microsoft. All rights reserved.
// Licensed under the MIT license. See LICENSE file in the project root for full license information.
package com.microsoft.azure.iothub.transport.mqtt;

import javax.net.ssl.SSLSocket;
import javax.net.ssl.SSLSocketFactory;
import java.io.IOException;
import java.net.InetAddress;
import java.net.Socket;
import java.net.UnknownHostException;

/**
 * A SSLSocketFactory which uses an existing SSLSocketFactory to delegate its operations to and overrides the
 * javax.net.ssl.SSLSocket.getEnabledProtocols() enabled protocols to the protocols that were passed to its
 * ProtocolOverridingSSLSocketFactory(javax.net.ssl.SSLSocketFactory, String[]) constructor
 *
 */

public class ProtocolOverridingSSLSocketFactory extends SSLSocketFactory{
	 	private final SSLSocketFactory underlyingSSLSocketFactory;
	    private final String[] enabledProtocols;
	 
	    public ProtocolOverridingSSLSocketFactory(final SSLSocketFactory delegate, final String[] enabledProtocols) throws IllegalArgumentException {
	    	// Codes_SRS_PROTOCOLOVERRIDINGSSLSOCKETFACTORYTEST_15_002: [The constructor shall throw a new IllegalArgumentException
	        // if the SSLSocketFactory parameter is null or empty.]
	    	if (delegate == null) {
	    		throw new IllegalArgumentException(
	                    "SSLSocketFactory object cannot be null.");
	    	}
	    	// Codes_SRS_PROTOCOLOVERRIDINGSSLSOCKETFACTORYTEST_15_003: [The constructor shall throw a new IllegalArgumentException
	        // if the enabled protocols parameter is null or empty.]
	    	if (enabledProtocols == null) {
	    		throw new IllegalArgumentException(
	                    "enabled Protocols object cannot be null.");
	    	}
	    	// Codes_SRS_PROTOCOLOVERRIDINGSSLSOCKETFACTORYTEST_15_001: [The constructor shall save the SSLSocketFactory and the enabled protocols.]
	        this.underlyingSSLSocketFactory = delegate;
	        this.enabledProtocols = enabledProtocols;
	    }
	 
	    //Override
	    public String[] getDefaultCipherSuites() {
	    	// Codes_SRS_PROTOCOLOVERRIDINGSSLSOCKETFACTORYTEST_15_004: [The function shall return the default cipher suites
	        // of the saved socket factory.]
	        return underlyingSSLSocketFactory.getDefaultCipherSuites();
	    }
	 
	    //Override
	    public String[] getSupportedCipherSuites() {
	    	// Codes_SRS_PROTOCOLOVERRIDINGSSLSOCKETFACTORYTEST_15_005: [The function shall return the supported cipher suites
	        // of the saved socket factory.]
	        return underlyingSSLSocketFactory.getSupportedCipherSuites();
	    }
	 
	    //Override
	    public Socket createSocket(final Socket socket, final String host, final int port, final boolean autoClose) throws IllegalArgumentException, IOException {
	    	// Codes_SRS_PROTOCOLOVERRIDINGSSLSOCKETFACTORYTEST_15_007: [The function shall throw a IllegalArgumentException
	        // if the socket object is null.]
	    	if (socket == null) {
	    		throw new IllegalArgumentException(
	                    "socket object cannot be null.");
	    	}
	        
	    	if (!socket.isConnected()) {
	    		throw new IllegalArgumentException(
	                    "socket is not connected.");
	    	}
	    	// Codes_SRS_PROTOCOLOVERRIDINGSSLSOCKETFACTORYTEST_15_008: [The function shall throw a IllegalArgumentException
	        // if the host is null.]
	    	if (host == null) {
	    		throw new IllegalArgumentException(
	                    "host String cannot be null.");
	    	}
	    	// Codes_SRS_PROTOCOLOVERRIDINGSSLSOCKETFACTORYTEST_15_009: [The function shall throw a IllegalArgumentException
	        // if the port is smaller than 0.]
	    	if (port < 1) {
	    		throw new IllegalArgumentException(
	                    "port is invalid.");
	    	}
	        final Socket underlyingSocket = underlyingSSLSocketFactory.createSocket(socket, host, port, autoClose);
	        // Codes_SRS_PROTOCOLOVERRIDINGSSLSOCKETFACTORYTEST_15_006: [The function shall return a new socket object
	        // over the saved enabled protocols.]
	        return overrideProtocol(underlyingSocket);
	    }
	    
	    //Override
	    public Socket createSocket() throws IOException {
	        final Socket underlyingSocket = underlyingSSLSocketFactory.createSocket();
	        // Codes_SRS_PROTOCOLOVERRIDINGSSLSOCKETFACTORYTEST_15_010: [The function shall return a new socket object
	        // over the saved enabled protocols.]
	        return overrideProtocol(underlyingSocket);
	    }
	 
	    //Override
	    public Socket createSocket(final String host, final int port) throws IllegalArgumentException, IOException {
	    	// Codes_SRS_PROTOCOLOVERRIDINGSSLSOCKETFACTORYTEST_15_012: [The function shall throw a IllegalArgumentException
	        // if the host is null.]
	    	if (host == null) {
	    		throw new IllegalArgumentException(
	                    "host String cannot be null.");
	    	}
	    	// Codes_SRS_PROTOCOLOVERRIDINGSSLSOCKETFACTORYTEST_15_013: [The function shall throw a IllegalArgumentException
	        // if port is smaller than 0.]
	    	if (port < 1) {
	    		throw new IllegalArgumentException(
	                    "port is invalid.");
	    	}
	    	final Socket underlyingSocket = underlyingSSLSocketFactory.createSocket(host, port);
	    	// Codes_SRS_PROTOCOLOVERRIDINGSSLSOCKETFACTORYTEST_15_011: [The function shall return a new socket object
	        // over the saved enabled protocols.]
	        return overrideProtocol(underlyingSocket);
	    }
	 
	    //Override
	    public Socket createSocket(final String host, final int port, final InetAddress localAddress, final int localPort) throws IllegalArgumentException, IOException {
	        // Codes_SRS_PROTOCOLOVERRIDINGSSLSOCKETFACTORYTEST_15_015: [The function shall throw a IllegalArgumentException
	        // if host is null.]
	    	if (host == null) {
	    		throw new IllegalArgumentException(
	                    "host String cannot be null.");
	    	}
	        // Codes_SRS_PROTOCOLOVERRIDINGSSLSOCKETFACTORYTEST_15_016: [The function shall throw a IllegalArgumentException
	        // if port is smaller than 0.]
	    	if (port < 1) {
	    		throw new IllegalArgumentException(
	                    "port is invalid.");
	    	}
	        // Codes_SRS_PROTOCOLOVERRIDINGSSLSOCKETFACTORYTEST_15_017: [The function shall throw a IllegalArgumentException
	        // if localAddress is null.]
	    	if (localAddress == null) {
	    		throw new IllegalArgumentException(
	                    "localAddress object cannot be null.");
	    	}
	        // Codes_SRS_PROTOCOLOVERRIDINGSSLSOCKETFACTORYTEST_15_018: [The function shall throw a IllegalArgumentException
	        // if localPort is smaller than 0.]
	    	if (localPort < 1) {
	    		throw new IllegalArgumentException(
	                    "localPort is invalid.");
	    	}
	    	
	    	final Socket underlyingSocket = underlyingSSLSocketFactory.createSocket(host, port, localAddress, localPort);
	    	// Codes_SRS_PROTOCOLOVERRIDINGSSLSOCKETFACTORYTEST_15_014: [The function shall return a new socket object
	        // over the saved enabled protocols.]
	        return overrideProtocol(underlyingSocket);
	    }
	 
	    //Override
	    public Socket createSocket(final InetAddress host, final int port) throws IllegalArgumentException, IOException {
	    	// Codes_SRS_PROTOCOLOVERRIDINGSSLSOCKETFACTORYTEST_15_020: [The function shall throw a IllegalArgumentException
	        // if the hostInetAddress is null.]
	    	if (host == null) {
	    		throw new IllegalArgumentException(
	                    "host object cannot be null.");
	    	}
	        // Codes_SRS_PROTOCOLOVERRIDINGSSLSOCKETFACTORYTEST_15_021: [The function shall throw a IllegalArgumentException
	        // if the localPort is smaller than 0.]
	    	if (port < 1) {
	    		throw new IllegalArgumentException(
	                    "port is invalid.");
	    	}
	    	
	        final Socket underlyingSocket = underlyingSSLSocketFactory.createSocket(host, port);
	        // Codes_SRS_PROTOCOLOVERRIDINGSSLSOCKETFACTORYTEST_15_019: [The function shall return a new socket object
	        // over the saved enabled protocols.]
	        return overrideProtocol(underlyingSocket);
	    }
	 
	    //Override
	    public Socket createSocket(final InetAddress host, final int port, final InetAddress localAddress, final int localPort) throws IllegalArgumentException, IOException {
	        // Codes_SRS_PROTOCOLOVERRIDINGSSLSOCKETFACTORYTEST_15_023: [The function shall throw a IllegalArgumentException
	        // if the hostInetAddress is null.]
	    	if (host == null) {
	    		throw new IllegalArgumentException(
	                    "host object cannot be null.");
	    	}
	        // Codes_SRS_PROTOCOLOVERRIDINGSSLSOCKETFACTORYTEST_15_024: [The function shall throw a IllegalArgumentException
	        // if the port is smaller than 0.]
	    	if (port < 1) {
	    		throw new IllegalArgumentException(
	                    "port is invalid.");
	    	}
	        // Codes_SRS_PROTOCOLOVERRIDINGSSLSOCKETFACTORYTEST_15_025: [The function shall throw a IllegalArgumentException
	        // if the localInetAddress is null.]
	    	if (localAddress == null) {
	    		throw new IllegalArgumentException(
	                    "localAddress object cannot be null.");
	    	}
	        // Codes_SRS_PROTOCOLOVERRIDINGSSLSOCKETFACTORYTEST_15_026: [The function shall throw a IllegalArgumentException
	        // if the localPort is smaller than 0.]
	    	if (localPort < 1) {
	    		throw new IllegalArgumentException(
	                    "localPort is invalid.");
	    	}
	        final Socket underlyingSocket = underlyingSSLSocketFactory.createSocket(host, port, localAddress, localPort);
	        // Codes_SRS_PROTOCOLOVERRIDINGSSLSOCKETFACTORYTEST_15_022: [The function shall return a new socket object
	        // over the saved enabled protocols.]
	        return overrideProtocol(underlyingSocket);
	    }
	 
	    /**
	     * Set the {@link javax.net.ssl.SSLSocket#getEnabledProtocols() enabled protocols} to {@link #enabledProtocols} if the <code>socket</code> is a
	     * {@link SSLSocket}
	     *
	     * @param socket The Socket
	     * @return
	     */
	    private Socket overrideProtocol(final Socket socket) {
	    	if (socket == null) {
	    		throw new IllegalArgumentException(
	                    "socket object cannot be null.");
	    	}
	    	
	        if (socket instanceof SSLSocket) {
	            if (enabledProtocols != null && enabledProtocols.length > 0) {
	                ((SSLSocket) socket).setEnabledProtocols(enabledProtocols);
	            }
	        }
		return socket;
	}
}
