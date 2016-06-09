// Copyright (c) Arduino. All rights reserved.
// Licensed under the MIT license. See LICENSE file in the project root for full license information.
//

#ifndef HTTPSCLIENT_H
#define HTTPSCLIENT_H

#include <Client.h>
#include <Print.h>

#define HTTPS_PORT          443

class HTTPSClient : Print
{
    public:
        HTTPSClient(Client* sslClient);
        int begin(const char* host, int port = HTTPS_PORT);
        uint8_t connected();
        void setTimeout(unsigned long timeout);
        int sendRequest(const char* method, const char* path);
        int sendHeader(const String& header);
        int sendBody(const unsigned char *content, int length);
        int readStatus();
        int readHeader(String& name, String& value);
        size_t contentLength();
        int readBody(unsigned char *content, int length);
        void end();

        virtual size_t write(uint8_t);
        virtual size_t write(const uint8_t *buffer, size_t size);

    private:
        String readLine();

        Client* _sslClient;
        size_t _contentLength;
};

#endif