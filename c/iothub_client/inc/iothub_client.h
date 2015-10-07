// Copyright (c) Microsoft. All rights reserved.
// Licensed under the MIT license. See LICENSE file in the project root for full license information.

/** @file iothub_client.h
*	@brief Extends the IoTHubCLient_LL module with additional features.
*
*	@details IoTHubClient is a module that extends the IoTHubCLient_LL
*			 module with 2 features:
*				- scheduling the work for the IoTHubCLient from a  
*				  thread, so that the user does not need to create their
*				  own thread
*				- thread-safe APIs
*/

#ifndef IOTHUB_CLIENT_H

#include "iothub_client_ll.h"

#ifdef __cplusplus
extern "C"
{
#endif

    typedef void* IOTHUB_CLIENT_HANDLE;

	/**
	* @brief	Creates a IoT Hub client for communication with an existing
	* 			IoT Hub using the specified connection string parameter.
	*
	* @param	connectionString	Pointer to a character string
	* @param	protocol			Function pointer for protocol implementation
	*
	*			Sample connection string:
	*				<blockquote>
	*					<pre>HostName=[IoT Hub name goes here].[IoT Hub suffix goes here, e.g., private.azure-devices-int.net];DeviceId=[Device ID goes here];SharedAccessKey=[Device key goes here];</pre>
	*				</blockquote>
	*
	* @return	A non-NULL @c IOTHUB_CLIENT_HANDLE value that is used when
	* 			invoking other functions for IoT Hub client and @c NULL on failure.
	*/
    extern IOTHUB_CLIENT_HANDLE IoTHubClient_CreateFromConnectionString(const char* connectionString, IOTHUB_CLIENT_TRANSPORT_PROVIDER protocol);

	/**
	* @brief	Creates a IoT Hub client for communication with an existing IoT
	* 			Hub using the specified parameters.
	*
	* @param	config	Pointer to an @c IOTHUB_CLIENT_CONFIG structure
	*
	*			The API does not allow sharing of a connection across multiple
	*			devices. This is a blocking call.
	*
	* @return	A non-NULL @c IOTHUB_CLIENT_HANDLE value that is used when
	* 			invoking other functions for IoT Hub client and @c NULL on failure.
	*/
    extern IOTHUB_CLIENT_HANDLE IoTHubClient_Create(const IOTHUB_CLIENT_CONFIG* config);

	/**
	* @brief	Disposes of resources allocated by the IoT Hub client. This is a
	* 			blocking call.
	*
	* @param	iotHubClientHandle	The handle created by a call to the create function.
	*/
    extern void IoTHubClient_Destroy(IOTHUB_CLIENT_HANDLE iotHubClientHandle);

	/**
	* @brief	Asynchronous call to send the message specified by @p eventMessageHandle.
	*
	* @param	iotHubClientHandle		   	The handle created by a call to the create function.
	* @param	eventMessageHandle		   	The handle to an IoT Hub message.
	* @param	eventConfirmationCallback  	The callback specified by the device for receiving
	* 										confirmation of the delivery of the IoT Hub message.
	* 										This callback can be expected to invoke the
	* 										::IoTHubClient_SendEventAsync function for the
	* 										same message in an attempt to retry sending a failing
	* 										message. The user can specify a @c NULL value here to
	* 										indicate that no callback is required.
	* @param	userContextCallback			User specified context that will be provided to the
	* 										callback. This can be @c NULL.
	*
	*			@b NOTE: The application behavior is undefined if the user calls
	*			the ::IoTHubClient_Destroy function from within any callback.
	*
	* @return	IOTHUB_CLIENT_OK upon success or an error code upon failure.
	*/
    extern IOTHUB_CLIENT_RESULT IoTHubClient_SendEventAsync(IOTHUB_CLIENT_HANDLE iotHubClientHandle, IOTHUB_MESSAGE_HANDLE eventMessageHandle, IOTHUB_CLIENT_EVENT_CONFIRMATION_CALLBACK eventConfirmationCallback, void* userContextCallback);

	/**
	* @brief	This function returns the current sending status for IoTHubClient.
	*
	* @param	iotHubClientHandle		The handle created by a call to the create function.
	* @param	iotHubClientStatus		The sending state is populated at the address pointed
	* 									at by this parameter. The value will be set to
	* 									@c IOTHUBCLIENT_SENDSTATUS_IDLE if there is currently
	* 								    no item to be sent and @c IOTHUBCLIENT_SENDSTATUS_BUSY
	* 								    if there are.
	*
	* @return	IOTHUB_CLIENT_OK upon success or an error code upon failure.
	*/
    extern IOTHUB_CLIENT_RESULT IoTHubClient_GetSendStatus(IOTHUB_CLIENT_HANDLE iotHubClientHandle, IOTHUB_CLIENT_STATUS *iotHubClientStatus);

	/**
	* @brief	Sets up the message callback to be invoked when IoT Hub issues a
	* 			message to the device. This is a blocking call.
	*
	* @param	iotHubClientHandle		   	The handle created by a call to the create function.
	* @param	messageCallback     	   	The callback specified by the device for receiving
	* 										messages from IoT Hub.
	* @param	userContextCallback			User specified context that will be provided to the
	* 										callback. This can be @c NULL.
	*
	*			@b NOTE: The application behavior is undefined if the user calls
	*			the ::IoTHubClient_Destroy function from within any callback.
	*
	* @return	IOTHUB_CLIENT_OK upon success or an error code upon failure.
	*/
    extern IOTHUB_CLIENT_RESULT IoTHubClient_SetMessageCallback(IOTHUB_CLIENT_HANDLE iotHubClientHandle, IOTHUB_CLIENT_MESSAGE_CALLBACK_ASYNC messageCallback, void* userContextCallback);

	/**
	* @brief	This function returns in the out parameter @p lastMessageReceiveTime
	* 			what was the value of the @c time function when the last message was
	* 			received at the client.
	*
	* @param	iotHubClientHandle				The handle created by a call to the create function.
	* @param	lastMessageReceiveTime  		Out parameter containing the value of @c time function
	* 											when the last message was received.
	*
	* @return	IOTHUB_CLIENT_OK upon success or an error code upon failure.
	*/
    extern IOTHUB_CLIENT_RESULT IoTHubClient_GetLastMessageReceiveTime(IOTHUB_CLIENT_HANDLE iotHubClientHandle, time_t* lastMessageReceiveTime);

	/**
	* @brief	This API sets a runtime option identified by parameter @p optionName
	* 			to a value pointed to by @p value. @p optionName and the data type
	* 			@p value is pointing to are specific for every option.
	*
	* @param	iotHubClientHandle	The handle created by a call to the create function.
	* @param	optionName		  	Name of the option.
	* @param	value			  	The value.
	*
	*			The options that can be set via this API are:
	*				- @b timeout - the maximum time in milliseconds a communication is
	*				  allowed to use. @p value is a pointer to an @c unsigned @c int with
	*				  the timeout value in milliseconds. This is only supported for the HTTP
	*				  protocol as of now. When the HTTP protocol uses CURL, the meaning of
	*				  the parameter is <em>total request time</em>. When the HTTP protocol uses
	*				  winhttp, the meaning is the same as the @c dwSendTimeout and
	*				  @c dwReceiveTimeout parameters of the
	*				  <a href="https://msdn.microsoft.com/en-us/library/windows/desktop/aa384116(v=vs.85).aspx">
	*				  WinHttpSetTimeouts</a> API.
	*				- @b CURLOPT_LOW_SPEED_LIMIT - only available for HTTP protocol and only
	*				  when CURL is used. It has the same meaning as CURL's option with the same
	*				  name. @p value is pointer to a long.
	*				- @b CURLOPT_LOW_SPEED_TIME - only available for HTTP protocol and only
	*				  when CURL is used. It has the same meaning as CURL's option with the same
	*				  name. @p value is pointer to a long.
	*				- @b CURLOPT_FORBID_REUSE - only available for HTTP protocol and only
	*				  when CURL is used. It has the same meaning as CURL's option with the same
	*				  name. @p value is pointer to a long.
	*				- @b CURLOPT_FRESH_CONNECT - only available for HTTP protocol and only
	*				  when CURL is used. It has the same meaning as CURL's option with the same
	*				  name. @p value is pointer to a long.
	*				- @b CURLOPT_VERBOSE - only available for HTTP protocol and only
	*				  when CURL is used. It has the same meaning as CURL's option with the same
	*				  name. @p value is pointer to a long.
	*
	* @return	IOTHUB_CLIENT_OK upon success or an error code upon failure.
	*/
    extern IOTHUB_CLIENT_RESULT IoTHubClient_SetOption(IOTHUB_CLIENT_HANDLE iotHubClientHandle, const char* optionName, const void* value);

#ifdef __cplusplus
}
#endif

#endif /* IOTHUB_CLIENT_H */
