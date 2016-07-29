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
	 
	    public ProtocolOverridingSSLSocketFactory(final SSLSocketFactory delegate, final String[] enabledProtocols) {
	        this.underlyingSSLSocketFactory = delegate;
	        this.enabledProtocols = enabledProtocols;
	    }
	 
	    //Override
	    public String[] getDefaultCipherSuites() {
	        return underlyingSSLSocketFactory.getDefaultCipherSuites();
	    }
	 
	    //Override
	    public String[] getSupportedCipherSuites() {
	        return underlyingSSLSocketFactory.getSupportedCipherSuites();
	    }
	 
	    //Override
	    public Socket createSocket(final Socket socket, final String host, final int port, final boolean autoClose) throws IOException {
	        final Socket underlyingSocket = underlyingSSLSocketFactory.createSocket(socket, host, port, autoClose);
	        return overrideProtocol(underlyingSocket);
	    }
	    
	    //Override
	    public Socket createSocket() throws IOException {
	        final Socket underlyingSocket = underlyingSSLSocketFactory.createSocket();
	        return overrideProtocol(underlyingSocket);
	    }
	 
	    //Override
	    public Socket createSocket(final String host, final int port) throws IOException, UnknownHostException {
	        final Socket underlyingSocket = underlyingSSLSocketFactory.createSocket(host, port);
	        return overrideProtocol(underlyingSocket);
	    }
	 
	    //Override
	    public Socket createSocket(final String host, final int port, final InetAddress localAddress, final int localPort) throws IOException, UnknownHostException {
	        final Socket underlyingSocket = underlyingSSLSocketFactory.createSocket(host, port, localAddress, localPort);
	        return overrideProtocol(underlyingSocket);
	    }
	 
	    //Override
	    public Socket createSocket(final InetAddress host, final int port) throws IOException {
	        final Socket underlyingSocket = underlyingSSLSocketFactory.createSocket(host, port);
	        return overrideProtocol(underlyingSocket);
	    }
	 
	    //Override
	    public Socket createSocket(final InetAddress host, final int port, final InetAddress localAddress, final int localPort) throws IOException {
	        final Socket underlyingSocket = underlyingSSLSocketFactory.createSocket(host, port, localAddress, localPort);
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
	        if (socket instanceof SSLSocket) {
	            if (enabledProtocols != null && enabledProtocols.length > 0) {
	                ((SSLSocket) socket).setEnabledProtocols(enabledProtocols);
	            }
	        }
	        return socket;
	    }
}
