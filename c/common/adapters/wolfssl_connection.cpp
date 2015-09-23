// Copyright (c) Microsoft. All rights reserved.
// Licensed under the MIT license. See LICENSE file in the project root for full license information.

#include <memory.h>
#include "wolfssl/ssl.h"
#include "wolfssl_connection.h"
#include "iot_logging.h"

WolfSSLConnection::WolfSSLConnection()
{
    wolfSSL_Init();

    WOLFSSL_METHOD* method = wolfTLSv1_2_client_method();
    if(method != NULL)
    {
        sslContext = wolfSSL_CTX_new(method);
    }
	else
	{
		sslContext = NULL;
	}
    
    isConnected = false;
}

WolfSSLConnection::~WolfSSLConnection()
{
    if (sslContext != NULL)
    {
        wolfSSL_CTX_free(sslContext);
        sslContext = NULL;
    }

    wolfSSL_Cleanup();  
}

static int receiveCallback(WOLFSSL* ssl, char *buf, int sz, void *ctx)
{
    int fd = *(int*)ctx;
	int result;

	(void)ssl;

    fd_set rfds;
    FD_ZERO(&rfds);
    FD_SET(fd, &rfds);
    
	if (lwip_select(FD_SETSIZE, &rfds, NULL, NULL, NULL) < 0)
	{
		result = -1;
	}
	else
	{
		result = lwip_recv(fd, buf, sz, 0);
	}
            
    return result;
}

static int sendCallback(WOLFSSL* ssl, char *buf, int sz, void *ctx)
{
    int fd = *(int*)ctx;
	int result;

	(void)ssl;

    fd_set wfds;
    FD_ZERO(&wfds);
    FD_SET(fd, &wfds);
    
	if (lwip_select(FD_SETSIZE, NULL, &wfds, NULL, NULL) < 0)
	{
		return -1;
	}
	else
	{
		result = lwip_send(fd, buf, sz, 0);
	}

	return result;
}

int WolfSSLConnection::connect(const char* host, const int port)
{
    int result;
    
    if(sslContext == NULL)
    {
		LogError("NULL SSL context\r\n");
        result = __LINE__;
    }
    else
    {
        if (init_socket(SOCK_STREAM) < 0)
        {
			LogError("init_socket failed\r\n");
            result = __LINE__;
        }
        else
        {
            if (set_address(host, port) != 0)
            {
				LogError("set_address failed\r\n");
                result = __LINE__;
            }
            else if (lwip_connect(_sock_fd, (const struct sockaddr *) &_remoteHost, sizeof(_remoteHost)) < 0)
            {
                close();
				LogError("lwip_connect failed\r\n");
                result = __LINE__;
            }
            else
            {
                wolfSSL_SetIOSend(sslContext, &sendCallback);
                wolfSSL_SetIORecv(sslContext, &receiveCallback);
            
                ssl = wolfSSL_new(sslContext);
                if(ssl == NULL) 
                {
					LogError("wolfssl new error\r\n");
                    result = __LINE__;
                }
                else
                {
                    wolfSSL_set_fd(ssl, _sock_fd);
                
                    result = wolfSSL_connect(ssl);
                    if (result != SSL_SUCCESS) 
                    {
                        LogError("wolfssl connect error=%d\r\n", result);
                        result = __LINE__;
                    }
                    else
                    {
                        result = 0;
                        isConnected = true;
                    }
                }
            }
        }
    }

    return result;
};

bool WolfSSLConnection::is_connected(void)
{
    return isConnected;
}

int WolfSSLConnection::send(char* data, int length)
{
    int result;
    
    if (!isConnected)
    {
        result = 0;
    }
    else
    {  
        result = wolfSSL_write(ssl, data, length);
    }
    
    return result;
}

int WolfSSLConnection::send_all(char* data, int length)
{
    return send(data, length);
}

int WolfSSLConnection::receive(char* data, int length)
{
    int result;
    
    if (!isConnected)
    {
        result = 0;
    }
    else
    {
        result = wolfSSL_read(ssl, data, length);
    }
    
    return result;
}

int WolfSSLConnection::receive_all(char* data, int length)
{
    return receive(data, length);
}

bool WolfSSLConnection::close(bool shutdown)
{
    bool result;
    
    if (!isConnected)
    {
        result = true;
    }
    else
    {
        isConnected = false;

        wolfSSL_CTX_free(sslContext);
        result = Socket::close(shutdown) == 0;
    }

    return result;
}

bool WolfSSLConnection::load_certificate(const unsigned char* certificate, size_t size)
{
    bool result;
    
    if (sslContext == NULL)
    {
		LogError("NULL SSL context\r\n");
		result = false;
    }
    else
    {
        result = (wolfSSL_CTX_load_verify_buffer(sslContext,(unsigned char*)certificate, size, SSL_FILETYPE_PEM) == SSL_SUCCESS);
    }
        
    return result;
}
