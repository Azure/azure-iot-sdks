# ProtocolOverridingSSLSocketFactory Requirements

## Overview

A SSLSocketFactory to create socket that supports sepcify protocols. the factory implements SSLSocketFactory and overrides createSocket and getting Cipher Suite events

## References

## Exposed API

```java
public final class MqttIotHubConnection implements MqttCallback
{
    public ProtocolOverridingSSLSocketFactory(final SSLSocketFactory delegate, final String[] enabledProtocols) throws IllegalArgumentException;

    public String[] getDefaultCipherSuites();
    public String[] getSupportedCipherSuites();

    public Socket createSocket(final Socket socket, final String host, final int port, final boolean autoClose) throws IllegalArgumentException, IOException;
    public Socket createSocket() throws IOException;
    public Socket createSocket(final String host, final int port) throws IllegalArgumentException, IOException;
	public Socket createSocket(final String host, final int port, final InetAddress localAddress, final int localPort) throws IllegalArgumentException, IOException;
	public Socket createSocket(final InetAddress host, final int port) throws IllegalArgumentException, IOException;
	public Socket createSocket(final InetAddress host, final int port, final InetAddress localAddress, final int localPort) throws IllegalArgumentException, IOException;
}
```


### ProtocolOverridingSSLSocketFactory

```java
public ProtocolOverridingSSLSocketFactory(final SSLSocketFactory delegate, final String[] enabledProtocols) throws IllegalArgumentException;
```

**SRS_ProtocolOverridingSSLSocketFactory_15_001: [**The constructor shall save the SSLSocketFactory and the enabled protocols.**]**

**SRS_ProtocolOverridingSSLSocketFactory_15_002: [**The constructor shall throw a new IllegalArgumentException if the SSLSocketFactory parameter is null or empty.**]**

**SRS_PROTOCOLOVERRIDINGSSLSOCKETFACTORYTEST_15_003: [**The constructor shall throw a new IllegalArgumentException if the enabled protocols parameter is null or empty.**]**


### getDefaultCipherSuites

```java
public String[] getDefaultCipherSuites();
```

**SRS_PROTOCOLOVERRIDINGSSLSOCKETFACTORYTEST_15_004: [**The function shall return the default cipher suites of the saved socket factory.**]**.


### getSupportedCipherSuites

```java
public String[] getSupportedCipherSuites();
```

**SRS_PROTOCOLOVERRIDINGSSLSOCKETFACTORYTEST_15_005: [**The function shall return the supported cipher suites of the saved socket factory.**]**


### createSocket

```java
public Socket createSocket(final Socket socket, final String host, final int port, final boolean autoClose) throws IllegalArgumentException, IOException;
```

**SRS_PROTOCOLOVERRIDINGSSLSOCKETFACTORYTEST_15_006: [**The function shall return a new socket object over the saved enabled protocols.**]**

**SRS_PROTOCOLOVERRIDINGSSLSOCKETFACTORYTEST_15_007: [**The function shall throw a IllegalArgumentException if the socket object is null.**]**

**SRS_PROTOCOLOVERRIDINGSSLSOCKETFACTORYTEST_15_008: [**The function shall throw a IllegalArgumentException if the host is null.**]**

**SRS_PROTOCOLOVERRIDINGSSLSOCKETFACTORYTEST_15_009: [**The function shall throw a IllegalArgumentException if the port is smaller than 0.**]**


### createSocket

```java
public Socket createSocket() throws IOException;
```

**SRS_PROTOCOLOVERRIDINGSSLSOCKETFACTORYTEST_15_010: [**The function shall return a new socket object over the saved enabled protocols.**]**


### createSocket

```java
public Socket createSocket(final String host, final int port) throws IllegalArgumentException, IOException;
```

**SRS_PROTOCOLOVERRIDINGSSLSOCKETFACTORYTEST_15_011: [**The function shall return a new socket object over the saved enabled protocols.**]**

**SRS_PROTOCOLOVERRIDINGSSLSOCKETFACTORYTEST_15_012: [**The function shall throw a IllegalArgumentException if the host is null.**]**

**SRS_PROTOCOLOVERRIDINGSSLSOCKETFACTORYTEST_15_013: [**The function shall throw a IllegalArgumentException if port is smaller than 0.**]**


### createSocket

```java
public Socket createSocket(final String host, final int port, final InetAddress localAddress, final int localPort) throws IllegalArgumentException, IOException;
```

**SRS_PROTOCOLOVERRIDINGSSLSOCKETFACTORYTEST_15_014: [**The function shall return a new socket object over the saved enabled protocols.**]**

**SRS_PROTOCOLOVERRIDINGSSLSOCKETFACTORYTEST_15_015: [**The function shall throw a IllegalArgumentException if host is null.**]**

**SRS_PROTOCOLOVERRIDINGSSLSOCKETFACTORYTEST_15_016: [**The function shall throw a IllegalArgumentException if port is smaller than 0.**]**

**SRS_PROTOCOLOVERRIDINGSSLSOCKETFACTORYTEST_15_017: [**The function shall throw a IllegalArgumentException if localAddress is null.**]**

**SRS_PROTOCOLOVERRIDINGSSLSOCKETFACTORYTEST_15_018: [**The function shall throw a IllegalArgumentException if localPort is smaller than 0.**]**


### createSocket

```java
public Socket createSocket(final InetAddress host, final int port) throws IllegalArgumentException, IOException;
```

**SRS_PROTOCOLOVERRIDINGSSLSOCKETFACTORYTEST_15_019: [**The function shall return a new socket object over the saved enabled protocols.**]**

**SRS_PROTOCOLOVERRIDINGSSLSOCKETFACTORYTEST_15_020: [**The function shall throw a IllegalArgumentException if the hostInetAddress is null.**]**

**SRS_PROTOCOLOVERRIDINGSSLSOCKETFACTORYTEST_15_021: [**The function shall throw a IllegalArgumentException if the localPort is smaller than 0.**]**


### createSocket

```java
public Socket createSocket(final InetAddress host, final int port, final InetAddress localAddress, final int localPort) throws IllegalArgumentException, IOException;
```

**SRS_PROTOCOLOVERRIDINGSSLSOCKETFACTORYTEST_15_022: [**The function shall return a new socket object over the saved enabled protocols.**]**

**SRS_PROTOCOLOVERRIDINGSSLSOCKETFACTORYTEST_15_023: [**The function shall throw a IllegalArgumentException if the hostInetAddress is null.**]**

**SRS_PROTOCOLOVERRIDINGSSLSOCKETFACTORYTEST_15_024: [**The function shall throw a IllegalArgumentException if the port is smaller than 0.**]**

**SRS_PROTOCOLOVERRIDINGSSLSOCKETFACTORYTEST_15_025: [**The function shall throw a IllegalArgumentException if the localInetAddress is null.**]**

**SRS_PROTOCOLOVERRIDINGSSLSOCKETFACTORYTEST_15_026: [**The function shall throw a IllegalArgumentException if the localPort is smaller than 0.**]**