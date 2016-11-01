/*
 * Copyright (c) Microsoft. All rights reserved.
 * Licensed under the MIT license. See LICENSE file in the project root for full license information.
 */
package com.microsoft.azure.iothub.ws.impl;

import javax.xml.bind.DatatypeConverter;
import java.nio.charset.StandardCharsets;

import java.security.InvalidParameterException;
import java.security.MessageDigest;
import java.security.NoSuchAlgorithmException;

import java.util.*;


public class WebSocketUpgrade
{
    public static final String RFC_GUID = "258EAFA5-E914-47DA-95CA-C5AB0DC85B11";
    private final char _colon = ':';
    private final char _slash = '/';
    private String _host = "";
    private String _path = "";
    private String _port = "";
    private String _protocol = "";
    private String _webSocketKey = "";
    private Map<String, String> _additionalHeaders = null;

    public WebSocketUpgrade(String hostName, String webSocketPath, int webSocketPort, String webSocketProtocol, Map<String, String> additionalHeaders)
    {
        setHost(hostName);
        setPath(webSocketPath);
        setPort(webSocketPort);
        setProtocol(webSocketProtocol);
        setAdditionalHeaders(additionalHeaders);
    }

    /**
     * Set host value in host header
     *
     * @param host The host header field value.
     */
    public void setHost(String host)
    {
        this._host = host;
    }

    /**
     * Set port value in host header
     *
     * @param port The port header field value.
     */
    public void setPort(int port)
    {
        this._port = "";

        if (port != 0)
        {
            this._port = String.valueOf(port);
        }
    }

    /**
     * Set path value in handshake
     *
     * @param path The path field value.
     */
    public void setPath(String path)
    {
        this._path = path;

        if (!this._path.isEmpty())
        {
            if (this._path.charAt(0) != this._slash)
            {
                this._path = this._slash + this._path;
            }
        }
    }

    /**
     * Set protocol value in protocol header
     *
     * @param protocol The protocol header field value.
     */
    public void setProtocol(String protocol)
    {
        this._protocol = protocol;
    }

    /**
     * Add field-value pairs to HTTP header
     *
     * @param additionalHeaders The Map containing the additional headers.
     */
    public void setAdditionalHeaders(Map<String, String> additionalHeaders)
    {
        _additionalHeaders = additionalHeaders;
    }

    /**
     * Utility function to clear all additional headers
     */
    public void clearAdditionalHeaders()
    {
        _additionalHeaders.clear();
    }

    /**
     * Utility function to create random, Base64 encoded key
     */
    private String createWebSocketKey()
    {
        byte[] key = new byte[16];

        for (int i = 0; i < 16; i++)
        {
            key[i] = (byte) (int) (Math.random() * 256);
        }

        return DatatypeConverter.printBase64Binary(key).trim();
    }

    public String createUpgradeRequest()
    {
        if (this._host.isEmpty())
        {
            throw new InvalidParameterException("host header has no value");
        }

        if (this._protocol.isEmpty())
        {
            throw new InvalidParameterException("protocol header has no value");
        }

        this._webSocketKey = createWebSocketKey();

        String _endOfLine = "\r\n";
        StringBuilder stringBuilder =
                new StringBuilder().append("GET https://").append(this._host).append(this._path).append(" HTTP/1.1").append(_endOfLine)
                        .append("Connection: Upgrade,Keep-Alive").append(_endOfLine)
                        .append("Upgrade: websocket").append(_endOfLine)
                        .append("Sec-WebSocket-Version: 13").append(_endOfLine)
                        .append("Sec-WebSocket-Key: ").append(this._webSocketKey).append(_endOfLine)
                        .append("Sec-WebSocket-Protocol: ").append(this._protocol).append(_endOfLine)
                        .append("Host: ").append(this._host).append(_endOfLine);

        if (_additionalHeaders != null)
        {
            for (Map.Entry<String, String> entry : _additionalHeaders.entrySet())
            {
                stringBuilder.append(entry.getKey() + ": " + entry.getValue()).append(_endOfLine);
            }
        }

        stringBuilder.append(_endOfLine);

        return stringBuilder.toString();
    }

    public Boolean validateUpgradeReply(byte[] responseBytes)
    {
        String httpString = new String(responseBytes, StandardCharsets.UTF_8);

        Boolean isStatusLineOk = false;
        Boolean isUpgradeHeaderOk = false;
        Boolean isConnectionHeaderOk = false;
        Boolean isProtocolHeaderOk = false;
        Boolean isAcceptHeaderOk = false;

        Scanner scanner = new Scanner(httpString);

        while (scanner.hasNextLine())
        {
            String line = scanner.nextLine();

            if ((line.toLowerCase().contains("http/1.1")) &&
                    (line.contains("101")) &&
                    (line.toLowerCase().contains("switching protocols")))
            {
                isStatusLineOk = true;

                continue;
            }

            if ((line.toLowerCase().contains("upgrade")) && (line.toLowerCase().contains("websocket")))
            {
                isUpgradeHeaderOk = true;

                continue;
            }

            if ((line.toLowerCase().contains("connection")) && (line.toLowerCase().contains("upgrade")))
            {
                isConnectionHeaderOk = true;

                continue;
            }

            if (line.toLowerCase().contains("sec-websocket-protocol") && (line.toLowerCase().contains(this._protocol.toLowerCase())))
            {
                isProtocolHeaderOk = true;

                continue;
            }

            if (line.toLowerCase().contains("sec-websocket-accept"))
            {
                MessageDigest messageDigest = null;

                try
                {
                    messageDigest = MessageDigest.getInstance("SHA-1");
                } catch (NoSuchAlgorithmException e)
                {
                    // can't happen since SHA-1 is a known digest
                    break;
                }

                String expectedKey = DatatypeConverter.printBase64Binary(messageDigest.digest((this._webSocketKey + RFC_GUID).getBytes())).trim();

                if (line.contains(expectedKey))
                {
                    isAcceptHeaderOk = true;
                }

                continue;
            }
        }

        scanner.close();

        if ((isStatusLineOk) && (isUpgradeHeaderOk) && (isConnectionHeaderOk) && (isProtocolHeaderOk) && (isAcceptHeaderOk))
        {
            return true;
        }
        else
        {
            return false;
        }
    }

    @Override
    public String toString()
    {
        StringBuilder builder = new StringBuilder();
        builder.append("WebSocketUpgrade [host=").append(_host).append(", path=").append(_path).append(", port=").append(_port).append(", protocol=").append(_protocol).append(", webSocketKey=").append(_webSocketKey);

        if ((_additionalHeaders != null) && (!_additionalHeaders.isEmpty()))
        {
            builder.append(", additionalHeaders=");

            for (Map.Entry<String, String> entry : _additionalHeaders.entrySet())
            {
                builder.append(entry.getKey() + ":" + entry.getValue()).append(", ");
            }

            int lastIndex = builder.lastIndexOf(", ");
            builder.delete(lastIndex, lastIndex + 2);
        }

        builder.append("]");

        return builder.toString();
    }
}
