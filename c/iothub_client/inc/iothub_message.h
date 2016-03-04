// Copyright (c) Microsoft. All rights reserved.
// Licensed under the MIT license. See LICENSE file in the project root for full license information.

/** @file   iothub_message.h
*	@brief  The @c IoTHub_Message component encapsulates one message that
*           can be transferred by an IoT hub client.
*/

#ifndef IOTHUB_MESSAGE_H
#define IOTHUB_MESSAGE_H

#include "macro_utils.h"
#include "map.h" 

#ifdef __cplusplus
#include <cstddef>
#include <ctime>
#include <cstdint>
extern "C" 
{
#else
#include <stddef.h>
#include <time.h>
#include <stdint.h>
#endif

#define IOTHUB_MESSAGE_RESULT_VALUES         \
    IOTHUB_MESSAGE_OK,                       \
    IOTHUB_MESSAGE_INVALID_ARG,              \
    IOTHUB_MESSAGE_INVALID_TYPE,             \
    IOTHUB_MESSAGE_ERROR                     \

/** @brief Enumeration specifying the status of calls to various  
 *  APIs in this module.
 */
DEFINE_ENUM(IOTHUB_MESSAGE_RESULT, IOTHUB_MESSAGE_RESULT_VALUES);

#define IOTHUBMESSAGE_CONTENT_TYPE_VALUES \
IOTHUBMESSAGE_BYTEARRAY, \
IOTHUBMESSAGE_STRING, \
IOTHUBMESSAGE_UNKNOWN \

/** @brief Enumeration specifying the content type of the a given  
  * message.
  */
DEFINE_ENUM(IOTHUBMESSAGE_CONTENT_TYPE, IOTHUBMESSAGE_CONTENT_TYPE_VALUES);

typedef void* IOTHUB_MESSAGE_HANDLE;

/**
 * @brief   Creates a new IoT hub message from a byte array. The type of the
 *          message will be set to @c IOTHUBMESSAGE_BYTEARRAY.
 *
 * @param   byteArray   The byte array from which the message is to be created.
 * @param   size        The size of the byte array.
 *
 * @return  A valid @c IOTHUB_MESSAGE_HANDLE if the message was successfully
 *          created or @c NULL in case an error occurs.
 */
extern IOTHUB_MESSAGE_HANDLE IoTHubMessage_CreateFromByteArray(const unsigned char* byteArray, size_t size);

/**
 * @brief   Creates a new IoT hub message from a null terminated string.  The
 *          type of the message will be set to @c IOTHUBMESSAGE_STRING.
 *
 * @param   source  The null terminated string from which the message is to be
 *                  created.
 *
 * @return  A valid @c IOTHUB_MESSAGE_HANDLE if the message was successfully
 *          created or @c NULL in case an error occurs.
 */
extern IOTHUB_MESSAGE_HANDLE IoTHubMessage_CreateFromString(const char* source);

/**
 * @brief   Creates a new IoT hub message with the content identical to that
 *          of the @p iotHubMessageHandle parameter.
 *
 * @param   iotHubMessageHandle Handle to the message that is to be cloned.
 *
 * @return  A valid @c IOTHUB_MESSAGE_HANDLE if the message was successfully
 *          cloned or @c NULL in case an error occurs.
 */
extern IOTHUB_MESSAGE_HANDLE IoTHubMessage_Clone(IOTHUB_MESSAGE_HANDLE iotHubMessageHandle);

/**
 * @brief   Fetches a pointer and size for the data associated with the IoT
 *          hub message handle. If the content type of the message is not
 *          @c IOTHUBMESSAGE_BYTEARRAY then the function returns
 *          @c IOTHUB_MESSAGE_INVALID_ARG.
 *
 * @param   iotHubMessageHandle Handle to the message.
 * @param   buffer              Pointer to the memory location where the
 *                              pointer to the buffer will be written.
 * @param   size                The size of the buffer will be written to
 *                              this address.
 *
 * @return  Returns IOTHUB_MESSAGE_OK if the byte array was fetched successfully
 *          or an error code otherwise.
 */
extern IOTHUB_MESSAGE_RESULT IoTHubMessage_GetByteArray(IOTHUB_MESSAGE_HANDLE iotHubMessageHandle, const unsigned char** buffer, size_t* size);

/**
 * @brief   Returns the null terminated string stored in the message.
 *          If the content type of the message is not @c IOTHUBMESSAGE_STRING
 *          then the function returns @c NULL.
 *
 * @param   iotHubMessageHandle Handle to the message.
 *
 * @return  @c NULL if an error occurs or a pointer to the stored null
 *          terminated string otherwise.
 */
extern const char* IoTHubMessage_GetString(IOTHUB_MESSAGE_HANDLE iotHubMessageHandle);

/**
 * @brief   Returns the content type of the message given by parameter
 *          @c iotHubMessageHandle.
 *
 * @param   iotHubMessageHandle Handle to the message.
 *
 * @return  An @c IOTHUBMESSAGE_CONTENT_TYPE value.
 */
extern IOTHUBMESSAGE_CONTENT_TYPE IoTHubMessage_GetContentType(IOTHUB_MESSAGE_HANDLE iotHubMessageHandle);

/**
 * @brief   Gets a handle to the message's properties map.
 *
 * @param   iotHubMessageHandle Handle to the message.
 *
 * @return  A @c MAP_HANDLE pointing to the properties map for this message.
 */
extern MAP_HANDLE IoTHubMessage_Properties(IOTHUB_MESSAGE_HANDLE iotHubMessageHandle);

/**
* @brief   Gets the MessageId from the IOTHUB_MESSAGE_HANDLE.
*
* @param   iotHubMessageHandle Handle to the message.
*
* @return  A const char* pointing to the Message Id.
*/
extern const char* IoTHubMessage_GetMessageId(IOTHUB_MESSAGE_HANDLE iotHubMessageHandle);

/**
* @brief   Sets the MessageId for the IOTHUB_MESSAGE_HANDLE.
*
* @param   iotHubMessageHandle Handle to the message.
* @param   messageId Pointer to the memory location of the messageId
*
* @return  Returns IOTHUB_MESSAGE_OK if the messageId was set successfully
*          or an error code otherwise.
*/
extern IOTHUB_MESSAGE_RESULT IoTHubMessage_SetMessageId(IOTHUB_MESSAGE_HANDLE iotHubMessageHandle, const char* messageId);

/**
* @brief   Gets the CorrelationId from the IOTHUB_MESSAGE_HANDLE.
*
* @param   iotHubMessageHandle Handle to the message.
*
* @return  A const char* pointing to the Correlation Id.
*/
extern const char* IoTHubMessage_GetCorrelationId(IOTHUB_MESSAGE_HANDLE iotHubMessageHandle);

/**
* @brief   Sets the CorrelationId for the IOTHUB_MESSAGE_HANDLE.
*
* @param   iotHubMessageHandle Handle to the message.
* @param   correlationId Pointer to the memory location of the messageId
*
* @return  Returns IOTHUB_MESSAGE_OK if the messageId was set successfully
*          or an error code otherwise.
*/
extern IOTHUB_MESSAGE_RESULT IoTHubMessage_SetCorrelationId(IOTHUB_MESSAGE_HANDLE iotHubMessageHandle, const char* correlationId);

/**
 * @brief   Frees all resources associated with the given message handle.
 *
 * @param   iotHubMessageHandle Handle to the message.
 */
extern void IoTHubMessage_Destroy(IOTHUB_MESSAGE_HANDLE iotHubMessageHandle);

/**
* @brief    Records the time when the message has been delivered to SendEventAsync function.
            This function is intended to be called only by the client code, and not user code.
*
* @param   iotHubMessageHandle Handle to the message.
* @param   time - time as returned by time C function
*
* @return  Returns IOTHUB_MESSAGE_OK if the messageId was set successfully
*          or an error code otherwise.
*/
extern IOTHUB_MESSAGE_RESULT IoTHubMessage_SetSendEventAsyncTime(IOTHUB_MESSAGE_HANDLE iotHubMessageHandle, time_t time);

/**
* @brief    Returns the time when the message has been delivered to _SendEventAsync function
            This function is intended to be called only by the client code, and not user code.
*
* @param   iotHubMessageHandle Handle to the message.
*
* @return  NULL if there was an error, or a const pointer to the stored time_t.
*/
extern const time_t* IoTHubMessage_GetSendEventAsyncTime(IOTHUB_MESSAGE_HANDLE iotHubMessageHandle);

/**
* @brief    Returns the value of "messageTimeout" option
*
* @param   iotHubMessageHandle Handle to the message.
*
* @return  NULL if there was an error, or a const pointer to the stored uint64_t
*/
extern const uint64_t* IoTHubMessage_GetMessageTimeout(IOTHUB_MESSAGE_HANDLE iotHubMessageHandle);

/**
* @brief    Records the value of "messageTimeout" option
*
* @param   iotHubMessageHandle Handle to the message.
*
* @return  IOTHUB_MESSAGE_OK if there was no error, or an error code.
*/
extern IOTHUB_MESSAGE_RESULT IoTHubMessage_SetMessageTimeout(IOTHUB_MESSAGE_HANDLE iotHubMessageHandle, uint64_t messageTimeout);

#ifdef __cplusplus
}
#endif

#endif /* IOTHUB_MESSAGE_H */
