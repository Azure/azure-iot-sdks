#include "iotdm_internal.h"
#include "emscripten.h"

// BKTODO: how to throw on error?  maybe use EM_ASM?
EMSCRIPTEN_KEEPALIVE 
int CreateSession(char *connectionString)
{
    IOTHUB_CHANNEL_HANDLE IoTHubChannel = IoTHubClient_DM_Open(connectionString, COAP_TCPIP);
    if (NULL == IoTHubChannel)
    {
        LogError("IoTHubClientHandle is NULL!\r\n");

        return 0;
    }

    LogInfo("IoTHubClientHandle: %p\r\n", IoTHubChannel);

    LogInfo("prepare LWM2M objects");
    if (IOTHUB_CLIENT_OK != IoTHubClient_DM_CreateDefaultObjects(IoTHubChannel))
    {
        LogError("failure to create LWM2M objects for client: %p\r\n", IoTHubChannel);

        IoTHubClient_DM_Close(IoTHubChannel);
        return 0;
    }

    return (int)IoTHubChannel;

}

EMSCRIPTEN_KEEPALIVE 
void DestroySession(int session)
{
}

EMSCRIPTEN_KEEPALIVE 
void Connect(int session)
{
    // BKTODO: how to return error?
    IoTHubClient_DM_Connect((IOTHUB_CHANNEL_HANDLE)session, NULL, NULL);
}

EMSCRIPTEN_KEEPALIVE 
void Disconnect(int session)
{
    IoTHubClient_DM_Close((IOTHUB_CHANNEL_HANDLE)session);
}

EMSCRIPTEN_KEEPALIVE 
void DoWork(int session)
{
    // BKTODO: how to return error?
    IoTHubClient_DM_DoWork((IOTHUB_CHANNEL_HANDLE)session);
 }

