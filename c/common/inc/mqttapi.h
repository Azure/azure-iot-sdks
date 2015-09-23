// Copyright (c) Microsoft. All rights reserved.
// Licensed under the MIT license. See LICENSE file in the project root for full license information.

#ifndef MQTTAPI_H
#define MQTTAPI_H

#ifdef __cplusplus
extern "C" {
#include <cstddef>
#else
#include <stddef.h>
#endif
#include "macro_utils.h"
#include "crt_abstractions.h"

typedef void* MQTTAPI_HANDLE;
typedef void* MQTTAPI_TOPIC_HANDLE;

#define MQTTAPI_RESULT_VALUES                \
MQTTAPI_OK,                                  \
MQTTAPI_INVALID_ARG,                         \
MQTTAPI_ERROR_CONNECTED_INSTANCE,            \
MQTTAPI_ERROR                                \

DEFINE_ENUM(MQTTAPI_RESULT, MQTTAPI_RESULT_VALUES);

#define MQTTAPI_CONFIRMATION_RESULT_VALUES     \
    MQTTAPI_CONFIRMATION_OK,                   \
    MQTTAPI_CONFIRMATION_BECAUSE_DESTROY,      \
    MQTTAPI_CONFIRMATION_ERROR                 \

DEFINE_ENUM(MQTTAPI_CONFIRMATION_RESULT, MQTTAPI_CONFIRMATION_RESULT_VALUES);

typedef struct
{
    /** The length of the MQTT message payload in bytes. */
    size_t payloadlen;
    /** A pointer to the payload of the MQTT message. */
	unsigned char* payload;
} MQTTAPI_Message;

typedef struct
{
    const char* deviceId;
	const char* deviceKey;
	/**
	* String used to connect with a sas token. Format is <IOTHUBNAME>.<IOTHUBSUFFIX>\devices\<DEVICEID>
	*/
	const char* sasTokenSr;
    /**
    * String with the Server URI to be connected. Format required <i>protocol://host:port</i> 
    */
    const char* serverURI;
} MQTTAPI_ConnectOptions;

typedef bool MQTTAPI_MessageArrived(void* context, const MQTTAPI_Message* message);
typedef void MQTTAPI_DeliveryComplete(void* context, MQTTAPI_CONFIRMATION_RESULT result);

extern MQTTAPI_HANDLE MQTTAPI_Create(const MQTTAPI_ConnectOptions* options);
extern MQTTAPI_RESULT MQTTAPI_SetMessageCallback(MQTTAPI_HANDLE instance, void* context, MQTTAPI_MessageArrived * ma);
extern MQTTAPI_RESULT MQTTAPI_SetDeliveryCompletedCallback(MQTTAPI_HANDLE instance, MQTTAPI_DeliveryComplete* dc);


extern void MQTTAPI_Destroy(MQTTAPI_HANDLE instance);

extern MQTTAPI_RESULT MQTTAPI_PublishMessage(MQTTAPI_HANDLE instance, const char* topicName, const MQTTAPI_Message* msg, void* context);

extern MQTTAPI_TOPIC_HANDLE MQTTAPI_Subscribe(MQTTAPI_HANDLE instance, const char* topic);
extern void MQTTAPI_Unsubscribe(MQTTAPI_TOPIC_HANDLE topicInstance);

extern void MQTTAPI_DoWork(MQTTAPI_HANDLE instance);

#ifdef __cplusplus
}
#endif

#endif /* MQTTAPI_H*/
