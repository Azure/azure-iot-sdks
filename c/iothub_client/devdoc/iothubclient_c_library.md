


#IoTHub client "C" Library
Revision 1.7
03/16/2016
 
#Revisions:
|Revision	 |Updated By|	Date	      |Major updates                                                             |
|------------|----------|-----------------|--------------------------------------------------------------------------|
|0.1	     |   TA	    |   01/15/2015	  |Initial draft                                                             |
|0.2	     |   TA	    |   01/22/2015	  |Reviewed comments and simplified API                                      |
|0.3	     |   TA	    |   01/30/2015	  |Completed API for both the IOTHUB_message & IOTHUB_client                 |
|1.0	     |   TA	    |   03/05/2015	  |Updated based on the final implementation of the IOTHUB_client APIs       |
|1.1	     |   TE	    |   04/14/2015	  |Adjust the config to use a function pointer rather than an enum           |
|1.2	     |   DC	    |   04/19/2015	  |Rename DeviceHub to IoTHub                                                |
|1.3	     |   AP	    |   05/18/2015	  |Rework to bring _DoWork and threadlessness.                               |
|1.5	     |   AP	    |   07/02/2015	  |Add _SetOption                                                            |
|1.6	     |   TE	    |   09/24/2015	  |Rework to bring up to date with current API                               |
|1.7	     |   DR	    |   03/16/2016	  |Adding ability to create multi-device transport.     
|1.8         |   AP     |   06/30/2016    |Added x509 options and convert to .md


#Overview
The IoTHub client "C" library offers developers a means of communication to & from an IoT Hub.


The library allows a device to achieve the supported communication patterns by a IoT Hub:
-	**Event**. A message sent by a device to a IoT Hub. The device does not expect a response from IoT Hub, but the event message can result in errors (e.g. not being processed …).
-	**Message**. A message sent by IoT Hub to the device. IoT Hub does not expect a response from the device, but the message can result in errors (e.g. failed delivery…)


The library offers the following features:
-	The library provides an encapsulation for the message transfer between a device & IoT Hub.
-	The library supports AMQP, HTTP, and MQTT protocols for communication with a IoT Hub. 
-	The library is a static lib.
-	The source code for the library will be available under the "IOTHUB_client" under the "Azure/azure-iot-sdks" in the GitHub repository.
-	The repository is located at: https://github.com/Azure/azure-iot-sdks

The APIs of this library cannot be called from different threads on the same handle without risking data races. Therefore, should more than 1 thread need to access concurrently the APIs of this module on the same handle, there needs to be a user-level synchronization mechanism that guarantees that two APIs are not called at the same time.

#Example1 – SendEventAsync:

```c
#include <stdio.h>
#include <stdlib.h>

#include "iothub_client.h"
#include "iothub_message.h"
#include "threadapi.h"
#include "crt_abstractions.h"
#include "iothubtransportamqp.h"


#define MAX_NUMBER_OF_MESSAGES 5

static const unsigned int SLEEP_IN_MILLISECONDS = 500;
static const int MESSAGE_BASE_TRACKING_ID = 42;
static int g_callbackInvoked;


typedef struct EVENT_INSTANCE_TAG
{
    IOTHUB_MESSAGE_HANDLE messageHandle;
    int messageTrackingId;  // For tracking the messages within the user callback.
} EVENT_INSTANCE;

static void ReceiveConfirmationCallback(IOTHUB_CLIENT_CONFIRMATION_RESULT result, void* userContextCallback)
{
    EVENT_INSTANCE* eventInstance = (EVENT_INSTANCE*)userContextCallback;
    (void)printf("Confirmation[%d] received for message tracking id = %d with result = %d\r\n", g_callbackInvoked, eventInstance->messageTrackingId, result);
    /* Some device specific action code goes here... */
    g_callbackInvoked++;
}

int main(void)
{
    const char* connectionString = "[device connection string]";
    IOTHUB_CLIENT_HANDLE iotHubClientHandle;

    EVENT_INSTANCE eventInstance[MAX_NUMBER_OF_MESSAGES];

    size_t msgLength = 1024;
    const char* msgText = malloc(msgLength);
    g_callbackInvoked = 0;

    (void)printf("Starting the IoTHub client sample to Send Event Asynchronously...\r\n");


    if ((iotHubClientHandle = IoTHubClient_CreateFromConnectionString(connectionString, AMQP_Protocol)) == NULL)
    {
        (void)printf("ERROR: iotHubClientHandle is NULL!\r\n");
    }
    else
    {
        IOTHUB_CLIENT_STATUS sendStatus;
        for (int i = 0; i < MAX_NUMBER_OF_MESSAGES; i++)
        {
            sprintf_s((char*)msgText, msgLength, "Message_%d_From_IoTHubClient", i);
            if ((eventInstance[i].messageHandle = IoTHubMessage_CreateFromByteArray((const unsigned char*)msgText, strlen(msgText))) == NULL)
            {
                (void)printf("ERROR: IoTHubMessageHandle is NULL!\r\n");
            }
            else
            {
                eventInstance[i].messageTrackingId = MESSAGE_BASE_TRACKING_ID + i;
                if (IoTHubClient_SendEventAsync(iotHubClientHandle, eventInstance[i].messageHandle, ReceiveConfirmationCallback, &eventInstance[i]) != IOTHUB_CLIENT_OK)
                {
                    (void)printf("ERROR: IoTHubClient_SendEventAsync..........FAILED!\r\n");
                }
                else
                {
                    (void)printf("IoTHubClient_SendEventAsync accepted data for transmission to IoT Hub.\r\n");
                }                    
                (void)printf("IoTHubClient_GetSendStatus() returns %d\r\n", IoTHubClient_GetSendStatus(iotHubClientHandle,&sendStatus));
                (void)printf("IoTHubClient_GetSendStatus indicated a client status of: %d\r\n", sendStatus);
            }
        }
        while (g_callbackInvoked < MAX_NUMBER_OF_MESSAGES)
        {
            ThreadAPI_Sleep(SLEEP_IN_MILLISECONDS);
        }
        (void)printf("IoTHubClient_GetSendStatus() returns %d\r\n", IoTHubClient_GetSendStatus(iotHubClientHandle, &sendStatus));
        (void)printf("IoTHubClient_GetSendStatus indicated a client status of: %d\r\n", sendStatus);
    }
    IoTHubClient_Destroy(iotHubClientHandle);
    return 0;
}
```


#Example2 – ReceiveMessage

```c
#include <stdio.h>
#include <stdlib.h>

#include "iothub_client.h"
#include "iothub_message.h"
#include "threadapi.h"
#include "crt_abstractions.h"
#include "iothubtransportamqp.h"
 
#define MAX_NUMBER_OF_MESSAGES 100
 
static const unsigned int SLEEP_IN_MILLISECONDS = 500;
static const int MESSAGE_BASE_TRACKING_ID = 42;
static int g_callbackInvoked;
 
static IOTHUBMESSAGE_DISPOSITION_RESULT ReceiveMessageCallback(IOTHUB_MESSAGE_HANDLE message, void* userContextCallback)
{
    int* callbackInvoked = (int*)userContextCallback;
    const unsigned char* payload;
    size_t size;
    if (IoTHubMessage_GetByteArray(message,&payload,&size) == IOTHUB_MESSAGE_OK)
    {
        (void)printf("Received message [%d] with Data: <<<%.*s>>> & Size=%d\r\n", *callbackInvoked, (int)size, payload, (int)size);
        /* Some device specific action code goes here... */
    }
    else
    {
        (void)printf("Received message [%d] with bad data\r\n",*callbackInvoked);
    }
    (*callbackInvoked)++;
    return IOTHUBMESSAGE_ACCEPTED;
}
 
int main(void)
{
    const char* connectionString = "[device connection string]"; 
    IOTHUB_CLIENT_HANDLE iotHubClientHandle;
 
    g_callbackInvoked = 0;
 
    (void)printf("Starting the IoTHub client sample to Receive messages...\r\n");
  
    if ((iotHubClientHandle = IoTHubClient_CreateFromConnectionString(connectionString, AMQP_Protocol)) == NULL)
    {
        (void)printf("ERROR: iotHubClientHandle is NULL!\r\n");
    }
    else
    {
        if (IoTHubClient_SetMessageCallback(iotHubClientHandle, ReceiveMessageCallback, &g_callbackInvoked) != IOTHUB_CLIENT_OK)
        {
            (void)printf("ERROR: IoTHubClient_SetMessageCallback..........FAILED!\r\n");
        }
        else
        {
            time_t theTime;
            (void)printf("IoTHubClient_SetMessageCallback...successful.\r\n");
            while (g_callbackInvoked < MAX_NUMBER_OF_MESSAGES)
            {
               ThreadAPI_Sleep(SLEEP_IN_MILLISECONDS);
            }
            if(IoTHubClient_GetLastMessageReceiveTime(iotHubClientHandle, &theTime)!=IOTHUB_CLIENT_OK)
            {
                 printf("unable to provide the last received message time...\r\n");
            }
            else
            {
                 printf("the time when the last message was received was %s\r\n", ctime(&theTime));
            }
        }
    }
    IoTHubClient_Destroy(iotHubClientHandle);
    return 0;
}

```

#Types defined by the IoTHub client:
```c
#define IOTHUB_CLIENT_RESULT_VALUES       \
    IOTHUB_CLIENT_OK,                     \
    IOTHUB_CLIENT_INVALID_ARG,            \
    IOTHUB_CLIENT_ERROR,                  \
    IOTHUB_CLIENT_INVALID_SIZE,           \
    IOTHUB_CLIENT_INDEFINITE_TIME         \
 
DEFINE_ENUM(IOTHUB_CLIENT_RESULT, IOTHUB_CLIENT_RESULT_VALUES);
 
#define IOTHUB_CLIENT_CONFIRMATION_RESULT_VALUES     \
    IOTHUB_CLIENT_CONFIRMATION_OK,                   \
    IOTHUB_CLIENT_CONFIRMATION_BECAUSE_DESTROY,      \
    IOTHUB_CLIENT_CONFIRMATION_MESSAGE_TIMEOUT,      \
    IOTHUB_CLIENT_CONFIRMATION_ERROR                 \
 
DEFINE_ENUM(IOTHUB_CLIENT_CONFIRMATION_RESULT, IOTHUB_CLIENT_CONFIRMATION_RESULT_VALUES);
 
#define IOTHUB_CLIENT_STATUS_VALUES       \
    IOTHUB_CLIENT_SEND_STATUS_IDLE,       \
    IOTHUB_CLIENT_SEND_STATUS_BUSY        \
 
DEFINE_ENUM(IOTHUB_CLIENT_STATUS, IOTHUB_CLIENT_STATUS_VALUES);

#define IOTHUBMESSAGE_DISPOSITION_RESULT_VALUES \
    IOTHUBMESSAGE_ACCEPTED, \
    IOTHUBMESSAGE_REJECTED, \
    IOTHUBMESSAGE_ABANDONED
 
DEFINE_ENUM(IOTHUBMESSAGE_DISPOSITION_RESULT, IOTHUBMESSAGE_DISPOSITION_RESULT_VALUES);
 
typedef void(*IOTHUB_CLIENT_EVENT_CONFIRMATION_CALLBACK)(IOTHUB_CLIENT_CONFIRMATION_RESULT result, void* userContextCallback);
typedef IOTHUBMESSAGE_DISPOSITION_RESULT (*IOTHUB_CLIENT_MESSAGE_CALLBACK_ASYNC)(IOTHUB_MESSAGE_HANDLE message, void* userContextCallback);
typedef const void*(*IOTHUB_CLIENT_TRANSPORT_PROVIDER)(void);
 
typedef struct IOTHUB_CLIENT_CONFIG_TAG
{
    IOTHUB_CLIENT_TRANSPORT_PROVIDER protocol;
    const char* deviceId;
    const char* deviceKey;
    const char* iotHubName;
    const char* iotHubSuffix;
    const char* protocolGatewayHostName;
} IOTHUB_CLIENT_CONFIG;

typedef struct IOTHUB_CLIENT_DEVICE_CONFIG_TAG
{
    IOTHUB_CLIENT_TRANSPORT_PROVIDER protocol;
    void * transportHandle;
    const char* deviceId;
    const char* deviceKey;
} IOTHUB_CLIENT_DEVICE_CONFIG;

typedef struct IOTHUBTRANSPORT_CONFIG_TAG
{
	const IOTHUB_CLIENT_CONFIG* upperConfig;
	PDLIST_ENTRY waitingToSend;
}IOTHUBTRANSPORT_CONFIG;
```

#Types defined by the IoTHub message module:
```c
#define IOTHUB_MESSAGE_RESULT_VALUES         \
    IOTHUB_MESSAGE_OK,                       \
    IOTHUB_MESSAGE_INVALID_ARG,              \
    IOTHUB_MESSAGE_INVALID_TYPE,             \
    IOTHUB_MESSAGE_ERROR                     \

DEFINE_ENUM(IOTHUB_MESSAGE_RESULT, IOTHUB_MESSAGE_RESULT_VALUES);

#define IOTHUBMESSAGE_CONTENT_TYPE_VALUES \
IOTHUBMESSAGE_BYTEARRAY, \
IOTHUBMESSAGE_STRING, \
IOTHUBMESSAGE_UNKNOWN \

DEFINE_ENUM(IOTHUBMESSAGE_CONTENT_TYPE, IOTHUBMESSAGE_CONTENT_TYPE_VALUES);
```

#IoTHub client Structures
##IOTHUB_CLIENT_CONFIG
struct members

|Name	                    |Description                                               |
|---------------------------|----------------------------------------------------------|               
|protocol	                |A function pointer that is passed into the IoTHubClientCreate.  A function definition for amqp,  AMQP_Protocol, is defined in the include iothubtransportamqp.h.  A function definition for http, HTTP_Protocol, is defined in the include iothubtransporthttp.h.  A function definition for mqtt, MQTT_Protocol, is defined in the include iothubtransportmqtt.h.   |
|deviceId	                |A string that identifies the device.                      |
|deviceKey	                |The device key used to authenticate the device.           |
|iotHubName	                |The IoT Hub name to which the device is connecting.       |
|iotHubSuffix	            |The IoT Hub suffix goes here, e.g., azure-devices.net.    |
|protocolGatewayHostName	|The address of the protocol gateway the client will use to connect to send data and receive messages.  This is for protocols which are supported via a protocol gateway.  For example, mqtt. |


##IOTHUB_CLIENT_DEVICE_CONFIG
struct members

|Name	                    |Description                                                |
|---------------------------|-----------------------------------------------------------|
|protocol	                |A function pointer that is passed into the IoTHubClientCreate.  A function definition for amqp,  AMQP_Protocol, is defined in the include iothubtransportamqp.h.  A function definition for http, HTTP_Protocol, is defined in the include iothubtransporthttp.h.  A function definition for mqtt, MQTT_Protocol, is defined in the include iothubtransportmqtt.h.    |
|deviceId	                |A string that identifies the device.                       |
|deviceKey	                |The device key used to authenticate the device.            |
|transportHandle	        |The transport connection to be used for this device.       |


#IoTHub client APIs

##IOTHUB_CLIENT_HANDLE IoTHubClient_CreateFromConnectionString(const char\* connectionString, IOTHUB_CLIENT_TRANSPORT_PROVIDER protocol);


Creates a IoT Hub client for communication with an existing IoT Hub using the specified connection string parameter. The API does not allow sharing of a connection across multiple devices. This is a blocking call.

*Sample connection string:*

HostName=\[IoT Hub name goes here\].\[IoT Hub suffix goes here, e.g., azure-devices.net\]; DeviceId=\[Device ID goes here\];SharedAccessKey=\[Device key goes here\];

###Arguments

|Name	            |Description
|-------------------|-----------------------------
|connectionString	|Pointer to a character string
|protocol	        |Function pointer for protocol implementation


###Return
-	A Non-NULL handle value that is used when invoking other functions for IoT Hub client.
-	NULL on failure. 

##IOTHUB_CLIENT_HANDLE IoTHubClient_Create(const IOTHUB_CLIENT_CONFIG\* config);


Creates a IoT Hub client for communication with an existing IoT Hub using the specified parameters. The API does not allow sharing of a connection across multiple devices. This is a blocking call.

###Arguments
|Name	            |Description
|-------------------|
|config         	|Pointer to a IOTHUB_CLIENT_CONFIG structure


###Return
- A Non-NULL handle value that is used when invoking other functions for IoT Hub client.
- NULL on failure.

##IOTHUB_CLIENT_HANDLE IoTHubClient_CreateWithTransport(const IOTHUB_CLIENT_DEVICE_CONFIG\* config);


This API allows sharing of a connection across multiple devices. Creates a IoT Hub client for communication with an existing IoT Hub using the specified parameters. This is a blocking call.

###Arguments

|Name	        |Description
|---------------|
|config	        |Pointer to a IOTHUB_CLIENT_DEVICE_CONFIG structure

###Return
- A Non-NULL handle value that is used when invoking other functions for IoT Hub client.
- NULL on failure.

##void IoTHubClient_Destroy(IOTHUB_CLIENT_HANDLE iotHubClientHandle);

Disposes of resources allocated by the IoT Hub client.  Any pending events that have not yet been sent to the IoT Hub will be immediately completed with a IOTHUB_CLIENT_CONFIRMATION_BECAUSE_DESTROY status.  Other events that are actually out on the wire but not finished may receive an IOTHUB_CLIENT_CONFIRMATION_ERROR status.  This is a blocking call.

###Arguments
|Name	            |Description
|-------------------|
|iotHubClientHandle	|The handle created by a call to the create function.

###Return
- No return.

##IOTHUB_CLIENT_RESULT IoTHubClient_SendEventAsync(IOTHUB_CLIENT_HANDLE iotHubClientHandle, IOTHUB_MESSAGE_HANDLE eventMessageHandle, IOTHUB_CLIENT_EVENT_CONFIRMATION_CALLBACK eventConfirmationCallback, void\* userContextCallback);

Asynchronous call to send the message specified by the IoTHubMessageHandle. 
NOTE: The application behavior is undefined if the user calls the IoTHubClient_Destroy from within any callback.

###Arguments

|Name	                    |Description
|---------------------------|
|iotHubClientHandle	        |The handle created by a call to the create function.
|eventMessageHandle	        |The handle to a IoT Hub message. 
|eventConfirmationCallBack	|The callback specified by the device for receiving confirmation of the delivery of the IoT Hub message. This callback can be expected to invoke SendEventAsync for the same message in attempt to retry sending a failing message. The user can specify a NULL value here to indicate no callback required.
|userContextCallback	    |User specified context that will be provided to the callback. This can be NULL.

###Return
- IOTHUB_CLIENT_OK upon success.
- Error code upon failure.

##IOTHUB_CLIENT_RESULT IoTHubClient_SetMessageCallback(IOTHUB_CLIENT_HANDLE iotHubClientHandle, IOTHUB_CLIENT_MESSAGE_CALLBACK_ASYNC messageCallback, void\* userContextCallback);

Sets up the message callback invoked when IoT Hub issues a message to the device. This is a blocking call. NOTE: The application behavior is undefined if the user calls the IoTHubClient_Destroy from within any callback.

##Arguments

|Name	                |Description
|-----------------------|
|iotHubClientHandle	    |The handle created by a call to the create function.
|messageCallback	    |The callback specified by the device for receiving messages from IoT Hub.
|userContextCallback	|User specified context that will be provided to the callback. This can be NULL.

###Return
- IOTHUB_CLIENT_OK upon success.
- Error code upon failure.

##IOTHUB_CLIENT_RESULT IoTHubClient_GetLastMessageReceiveTime (IOTHUB_CLIENT_HANDLE iotHubClientHandle, time_t\* lastMessageReceiveTime);

This function returns in the out parameter lastMessageReceiveTime what was the value of the time() function when the last notification was received at the client.

###Arguments
|Name	                |Description
|-----------------------|
|iotHubClientHandle	    |The handle created by a call to the create function.
|lastMessageReceiveTime	|Out parameter containing the value of time() function when the last message was received

###Return
- IOTHUB_CLIENT_OK upon success.
- Error code upon failure.

##IOTHUB_CLIENT_RESULT IoTHubClient_GetSendStatus(IOTHUB_CLIENT_HANDLE iotHubClientHandle, IOTHUB_CLIENT_STATUS\* iotHubClientStatus);

This function returns the current sending status for IoTHubClient.

###Arguments

|Name           	    |Description
|-----------------------|
|iotHubClientHandle	    |The handle created by a call to the create function.
|iotHubClientStatus	    |A pointer to an IOTHUB_CLIENT_STATUS.  If the function call is successful then what is pointed to will receive: IOTHUBCLIENT_SENDSTATUS_IDLE if there are currently no items to be sent.  IOTHUBCLIENT_SENDSTATUS_BUSY if there are currently items to be sent.

###Return
- IOTHUB_CLIENT_OK upon success
- IOTHUBCLIENT _INVALID_ARG if called with NULL parameter
- Error code upon failure

##IOTHUB_CLIENT_MESSAGE_CALLBACK_ASYNC

Once a message is received from the service, if the user has set a callback, the receive callback shall be invoked. 
This call back is defined as:

```c
typedef IOTHUBMESSAGE_DISPOSITION_RESULT
(*IOTHUB_CLIENT_MESSAGE_CALLBACK_ASYNC)(IOTHUB_MESSAGE_HANDLE message, void* userContextCallback);
```

If the callback returns the status IOTHUBMESSAGE_ACCEPTED,  the client will accept the message, meaning that it will not be received again by the client. If the callback returns the status IOTHUBMESSAGE_REJECTED , the message will be rejected.  The message will not be resent to the device.  If the callback returns the status IOTHUBMESSAGE_ABANDONED, the message will be abandoned.  The implies that the user could not process the message but it expected that the message will be resent to the device from the service. message is only valid in the scope of the callback.

##IOTHUB_CLIENT_RESULT IoTHubClient_SetOption(IOTHUB_CLIENT_HANDLE iotHubClientHandle, const char\* optionName, const void\* value);

IoTHubClient_SetOption shall set a runtime option identified by parameter `optionName` to a value pointed to by parameter value. The optionName and the data type value is pointing to are specific for every option.

###Options:

- "timeout" - the maximum time in miliseconds a communication is allowed to use. value is a pointer to an unsigned int with the meaning of "miliseconds". This is only supported for HTTP protocol so far. When the HTTP protocol uses CURL, the meaning of the parameter is total request time. When the HTTP protocol uses winhttp, the meaning is dwSendTimeout and dwReceiveTimeout parameters of WinHttpSetTimeouts API.
- "CURLOPT_LOW_SPEED_LIMIT" - only available for HTTP protocol and only when CURL is used. It has the same meaning as CURL's option with the same name. value is pointer to a long.
- "CURLOPT_LOW_SPEED_TIME"  - only available for HTTP protocol and only when CURL is used. It has the same meaning as CURL's option with the same name. value is pointer to a long.
- "CURLOPT_FORBID_REUSE"  - only available for HTTP protocol and only when CURL is used. It has the same meaning as CURL's option with the same name. value is pointer to a long.
- "CURLOPT_FRESH_CONNECT"  - only available for HTTP protocol and only when CURL is used. It has the same meaning as CURL's option with the same name. value is pointer to a long.
- "CURLOPT_VERBOSE"  - only available for HTTP protocol and only when CURL is used. It has the same meaning as CURL's option with the same name. value is pointer to a long.
- "messageTimeout" - maximum transmission time for an event from the moment when _SendAsync is called. value is a pointer to a uint64_t that contains the number of miliseconds after which events timeout. When the event times out, the event callback is invoked and the code IOTHUB_CLIENT_CONFIRMATION_MESSAGE_TIMEOUT is passed as parameter result.
By default messages never expire. The meaning of the messageTimeout value is the following:
    - 0 = disable message timeout for all messages send by _SendAsync from now on
    - Any other number - consider that number as the timeout.
- "x509certificate" - feeds a x509 certificate in PEM format to IoTHubClient to be used for authentication. value is a pointer to a null terminated string that contains the certificate. Example:
```c
const char* value =
"-----BEGIN CERTIFICATE-----"
"MIICpDCCAYwCCQCfIjBnPxs5TzANBgkqhkiG9w0BAQsFADAUMRIwEAYDVQQDDAls"
"b2NhbGhvc3QwHhcNMTYwNjIyMjM0MzI3WhcNMTYwNjIzMjM0MzI3WjAUMRIwEAYD"
...
"+s88wBF907s1dcY45vsG0ldE3f7Y6anGF60nUwYao/fN/eb5FT5EHANVMmnK8zZ2"
"tjWUt5TFnAveFoQWIoIbtzlTbOxUFwMrQFzFXOrZoDJmHNWc2u6FmVAkowoOSHiE"
"dkyVdoGPCXc="
"-----END CERTIFICATE-----";
```
- "x509privatekey" - feed a x509 private key in PEM format to IoTHubClient to be used for authentication. value is a pointer to a null terminated string that contains the key. Example:
```c
const char* privateKey = 
"-----BEGIN RSA PRIVATE KEY-----"            
"MIIEpQIBAAKCAQEA0zKK+Uu5I0nXq2V6+2gbdCsBXZ6j1uAgU/clsCohEAek1T8v"
"qj2tR9Mz9iy9RtXPMHwzcQ7aXDaz7RbHdw7tYXqSw8iq0Mxq2s3p4mo6gd5vEOiN"
...
"EyePNmkCgYEAng+12qvs0de7OhkTjX9FLxluLWxfN2vbtQCWXslLCG+Es/ZzGlNF"
"SaqVID4EAUgUqFDw0UO6SKLT+HyFjOr5qdHkfAmRzwE/0RBN69g2qLDN3Km1Px/k"
"xyJyxc700uV1eKiCdRLRuCbUeecOSZreh8YRIQQXoG8uotO5IttdVRc="        
"-----END RSA PRIVATE KEY-----";
```

##IotHubClient\_LL\_... APIs

IoTHubClient API surface also contains a separate set of APIs that allow the user to interact with the lower layer portion of the IoTHubClient. These APIs contain _LL_ in their name, but retain the same functionality like the IoTHubClient_... APIs, with one difference. If the _LL_ APIs are used the user is responsible for scheduling when the actual work done by the IoTHubClient happens (when the data is sent/received on/from the wire). This is useful for constrained devices where spinning a separate thread is often not desired.

##IOTHUB_CLIENT_RESULT void IoTHubClient_LL_DoWork(IOTHUB_CLIENT_HANDLE iotHubClientHandle);

This function is user called when work (sending/receiving) can be done by the IoTHubClient. All IoTHubClient interactions (in regards to network trafic and/or user level callbacks) are the effect of calling this function and they take place synchronously inside _DoWork.

###Arguments:
|Name	                    |Description
|---------------------------|
|IotHubClientHandle	        |The handle created by a call to the create function.

###Example of using DoWork:

```c
#include <stdio.h>
#include <stdlib.h>

#include "iothub_client.h"
#include "iothub_message.h"
#include "threadapi.h"
#include "crt_abstractions.h"
#include "iothubtransportamqp.h"


#define MAX_NUMBER_OF_MESSAGES 5

static const unsigned int SLEEP_IN_MILLISECONDS = 500;
static const int MESSAGE_BASE_TRACKING_ID = 42;
static int g_callbackInvoked;


typedef struct EVENT_INSTANCE_TAG
{
    IOTHUB_MESSAGE_HANDLE messageHandle;
    int messageTrackingId;  // For tracking the messages within the user callback.
} EVENT_INSTANCE;

static void MessageConfirmationCallback(IOTHUB_CLIENT_CONFIRMATION_RESULT result, void* userContextCallback)
{
    EVENT_INSTANCE* eventInstance = (EVENT_INSTANCE*)userContextCallback;
    (void)printf("Confirmation[%d] received for message tracking id = %d with result = %d\r\n", g_callbackInvoked, eventInstance->messageTrackingId, result);
    /* Some device specific action code goes here... */
    g_callbackInvoked++;
}

int main(void)
{
    const char* connectionString = "[device connection string]";
    IOTHUB_CLIENT_HANDLE iotHubClientHandle;

    EVENT_INSTANCE eventInstance[MAX_NUMBER_OF_MESSAGES];

    size_t msgLength = 1024;
    const char* msgText = malloc(msgLength);
    g_callbackInvoked = 0;

    (void)printf("Starting the IoTHub client sample to Send Event Asynchronously...\r\n");


    if ((iotHubClientHandle = IoTHubClient_LL_CreateFromConnectionString(connectionString, AMQP_Protocol)) == NULL)
    {
        (void)printf("ERROR: iotHubClientHandle is NULL!\r\n");
    }
    else
    {
        IOTHUB_CLIENT_RESULT sendResult,getResult;
        IOTHUB_CLIENT_STATUS sendStatus;
        for (int i = 0; i < MAX_NUMBER_OF_MESSAGES; i++)
        {
            sprintf_s((char*)msgText, msgLength, "Message_%d_From_IoTHubClient", i);
            if ((eventInstance[i].messageHandle = IoTHubMessage_CreateFromString(msgText)) == NULL)
            {
                (void)printf("ERROR: IoTHubMessageHandle is NULL!\r\n");
            }
            else
            {
                eventInstance[i].messageTrackingId = MESSAGE_BASE_TRACKING_ID + i;
                if ((sendResult = IoTHubClient_LL_SendEventAsync(iotHubClientHandle, eventInstance[i].messageHandle, MessageConfirmationCallback, &eventInstance[i])) != IOTHUB_CLIENT_OK)
                {
                    (void)printf("ERROR: IoTHubClient_LL_SendEventAsync..........FAILED! Status is: %d\r\n",sendResult);
                }
                else
                {
                    (void)printf("IoTHubClient_LL_SendEventAsync accepted data for transmission to IoT Hub.\r\n");
                }
                getResult = IoTHubClient_LL_GetSendStatus(iotHubClientHandle, &sendStatus);
                if (getResult != IOTHUB_CLIENT_OK)
                {
                    (void)printf("IoTHubClient_LL_GetSendStatus failed! status is: %d", getResult);
                }
                else
                {
                    (void)printf("IoTHubClient_LL_GetSendStatus() returns %d\r\n", sendStatus);
                }
            }
        }
        while (g_callbackInvoked < MAX_NUMBER_OF_MESSAGES)
        {
            IoTHubClient_LL_DoWork(iotHubClientHandle);
            getResult = IoTHubClient_LL_GetSendStatus(iotHubClientHandle, &sendStatus);
            if (getResult != IOTHUB_CLIENT_OK)
            {
                (void)printf("IoTHubClient_LL_GetSendStatus failed! status is: %d", getResult);
            }
            else
            {
                (void)printf("IoTHubClient_LL_GetSendStatus() returns %d\r\n", sendStatus);
            }
            ThreadAPI_Sleep(SLEEP_IN_MILLISECONDS);
        }
    }

    IoTHubClient_LL_Destroy(iotHubClientHandle);
    return 0;
}

```

#IoTHub message APIs

##IOTHUB_MESSAGE_HANDLE IoTHubMessage_CreateFromByteArray(const unsigned char\* byteArray, size_t size);

Creates an IoT Hub message to be used for operations between the device and IoT Hub.  The data pointed to by byteArray may contain unprintable data and is not zero terminated.

###Arguments
|Name   	            |Description
|-----------------------|
|byteArray	            |A pointer to data for the message.
|size	                |The number of unsigned chars pointed to by byteArray.  If size is zero then byteArray may be NULL.  If size is not zero then byteArray MUST NOT be NULL.

###Return
- A None-NULL handle value that is used when invoking other functions for IoT Hub message.
- NULL on failure.

##IOTHUB_MESSAGE_HANDLE IoTHubMessage_CreateFromString(const char\* source);
Creates an IoT Hub message to be used for operations between the device and IoT Hub.  The data is assumed to be printable and is zero terminated.

###Arguments
|Name	                |Description
|-----------------------|
|source	                |A pointer to data for the message.

###Return
- A non-NULL handle value that is used when invoking other functions for IoT Hub message.
- NULL on failure.

##IOTHUB_MESSAGE_HANDLE IoTHubMessage_Clone(IOTHUB_MESSAGE_HANDLE iotHubMessageHandle);

Creates a new IoT Hub message with the content identical to that of the iotHubMessageHandle parameter.

###Arguments

|Name	                |Description
|-----------------------|
|iotHubMessageHandle	|Handle to the message to be cloned.

###Return
- A non-NULL handle value that is used when invoking other functions for IoT Hub message.
- NULL on failure.

##IOTHUB_MESSAGE_RESULT IoTHubMessage_GetByteArray(IOTHUB_MESSAGE_HANDLE iotHubMessageHandle, const unsigned char\*\* buffer, size_t\* size);
Fetches a pointer and size for the data associated with the IoT Hub message handle.  If the content type of the message is not IOTHUBMESSAGE_BYTEARRAY then the function returns IOTHUB_MESSAGE_INVALID_ARG.

###Arguments

|Name	                |Description
|-----------------------|
|iotHubMessageHandle	|Handle to the message to be cloned.
|buffer	                |Pointer to the memory location where the pointer to the buffer will be written.
|size	                |The size of the buffer will be written to this address.

###Return
- IOTHUB_MESSAGE_OK if the byte array was fetched successfully.
- Error code upon failure.

##const char\* IoTHubMessage_GetString(IOTHUB_MESSAGE_HANDLE iotHubMessageHandle);

Returns the zero terminated string stored in message.  If the content type of the message is not IOTHUBMESSAGE_STRING then the function returns NULL.

###Arguments

|Name	                |Description
|-----------------------|
|iotHubMessageHandle	|Handle to the message.

###Return
- Pointer to the zero terminated string stored in the message.
- NULL if an error occurs or the content type is incorrect.

##IOTHUBMESSAGE_CONTENT_TYPE IoTHubMessage_GetContentType(IOTHUB_MESSAGE_HANDLE iotHubMessageHandle);

Returns the content type of the message given by the parameter iotHubMessageHandle.

##Arguments
|Name	                |Description
|-----------------------|
|iotHubMessageHandle    |Handle to the message.

###Return
- IOTHUBMESSAGE_BYTEARRAY for a message created via IoTHubMessage_CreateFromByteArray
- IOTHUBMESSAGE_STRING for a message created via IoTHubMessage_CreateFromString
- IOTHUBMESSAGE_UNKNOWN otherwise.

##MAP_HANDLE IoTHubMessage_Properties(IOTHUB_MESSAGE_HANDLE iotHubMessageHandle);

Returns a handle to the message’s properties map.

###Arguments
|Name	                |Description
|-----------------------|
|iotHubMessageHandle	|Handle to the message.

###Return
MAP_HANDLE representing the message’s property map.

##void IoTHubMessage_Destroy(IOTHUB_MESSAGE_HANDLE ioTHubMessageHandle);
Disposes of resources allocated by the IoT Hub message.

###Arguments
|Name	                |Description
|-----------------------|
|ioTHubMessageHandle	|The handle created by a call to the create function.

Return
- No return.

