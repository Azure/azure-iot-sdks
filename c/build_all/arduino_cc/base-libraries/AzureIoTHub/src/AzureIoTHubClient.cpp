// Copyright (c) Microsoft. All rights reserved.
// Licensed under the MIT license. See LICENSE file in the project root for full license information.

#include <time.h>
#include <sys/time.h>

#include <AzureIoTHubClient.h>

Client* AzureIoTHubClient::sslClient = NULL;

AzureIoTHubClient::AzureIoTHubClient()
{
    //...
}

AzureIoTHubClient::AzureIoTHubClient(const AzureIoTHubClient& src)
{
    AzureIoTHubClient::sslClient = src.sslClient;
}

AzureIoTHubClient& AzureIoTHubClient::operator=(const AzureIoTHubClient src)
{
    AzureIoTHubClient::sslClient = src.sslClient;
    return *this;
}

AzureIoTHubClient::~AzureIoTHubClient()
{
    AzureIoTHubClient::sslClient = NULL;
}

void AzureIoTHubClient::begin(Client& sslClient)
{
    AzureIoTHubClient::sslClient = &sslClient;
}

void AzureIoTHubClient::end()
{
    AzureIoTHubClient::sslClient = NULL;
}

void AzureIoTHubClient::setEpochTime(unsigned long epochTime)
{
#ifndef ARDUINO_ARCH_ESP8266
    struct timeval tv;

    tv.tv_sec = epochTime;
    tv.tv_usec = 0;
    settimeofday(&tv, NULL);
#endif
}
