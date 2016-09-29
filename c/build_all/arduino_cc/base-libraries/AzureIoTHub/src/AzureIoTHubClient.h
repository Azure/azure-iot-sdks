// Copyright (c) Microsoft. All rights reserved.
// Licensed under the MIT license. See LICENSE file in the project root for full license information.

#ifndef AZUREIOTHUBCLIENT_H
#define AZUREIOTHUBCLIENT_H

#ifdef __cplusplus

#include <Client.h>

///
/// To improve developer experience, we will keep the decision about each sslClient Arduino will use in 
///   the .ino file. This library needs the pointer to sslClient to initialize the httpapi. The class 
///   AzureIoTHubClient provides the bridge between this 2 objects. 
///
class AzureIoTHubClient
{
public:
    AzureIoTHubClient();
    AzureIoTHubClient(const AzureIoTHubClient& src);
    AzureIoTHubClient& operator=(const AzureIoTHubClient src);
    virtual ~AzureIoTHubClient();

    void begin(Client& sslClient);
    void end();
    void setEpochTime(unsigned long epochTime);

    static Client* sslClient;
};

#endif

#endif
