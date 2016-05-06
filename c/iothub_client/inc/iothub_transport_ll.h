// Copyright (c) Microsoft. All rights reserved.
// Licensed under the MIT license. See LICENSE file in the project root for full license information.

#ifndef IOTHUB_TRANSPORT_LL_H
#define IOTHUB_TRANSPORT_LL_H

#include "azure_c_shared_utility/doublylinkedlist.h"
#include "iothub_message.h"
#include "iothub_client_ll.h"

#ifdef __cplusplus
extern "C"
{
#endif

	/** @brief	This struct captures device configuration. */
	typedef struct IOTHUB_DEVICE_CONFIG_TAG
	{
		/** @brief	A string that identifies the device. */
		const char* deviceId;

		/** @brief	The device key used to authenticate the device. */
		const char* deviceKey;

		/** @brief	The device SAS used to authenticate the device in place of using the device key. */
		const char* deviceSasToken;

	} IOTHUB_DEVICE_CONFIG;

	typedef void* TRANSPORT_LL_HANDLE;
	typedef void* IOTHUB_DEVICE_HANDLE;

	typedef IOTHUB_CLIENT_RESULT(*pfIoTHubTransport_SetOption)(TRANSPORT_LL_HANDLE handle, const char *optionName, const void* value);
	typedef TRANSPORT_LL_HANDLE(*pfIoTHubTransport_Create)(const IOTHUBTRANSPORT_CONFIG* config);
	typedef void (*pfIoTHubTransport_Destroy)(TRANSPORT_LL_HANDLE handle);
	typedef IOTHUB_DEVICE_HANDLE(*pfIotHubTransport_Register)(TRANSPORT_LL_HANDLE handle, const IOTHUB_DEVICE_CONFIG* device, IOTHUB_CLIENT_LL_HANDLE iotHubClientHandle, PDLIST_ENTRY waitingToSend);
	typedef void(*pfIotHubTransport_Unregister)(IOTHUB_DEVICE_HANDLE deviceHandle);
	typedef int (*pfIoTHubTransport_Subscribe)(IOTHUB_DEVICE_HANDLE handle);
	typedef void (*pfIoTHubTransport_Unsubscribe)(IOTHUB_DEVICE_HANDLE handle);
	typedef void (*pfIoTHubTransport_DoWork)(TRANSPORT_LL_HANDLE handle, IOTHUB_CLIENT_LL_HANDLE iotHubClientHandle);
	typedef IOTHUB_CLIENT_RESULT(*pfIoTHubTransport_GetSendStatus)(IOTHUB_DEVICE_HANDLE handle, IOTHUB_CLIENT_STATUS *iotHubClientStatus);

#define TRANSPORT_PROVIDER_FIELDS                            \
pfIoTHubTransport_SetOption IoTHubTransport_SetOption;       \
pfIoTHubTransport_Create IoTHubTransport_Create;             \
pfIoTHubTransport_Destroy IoTHubTransport_Destroy;           \
pfIotHubTransport_Register IoTHubTransport_Register;         \
pfIotHubTransport_Unregister IoTHubTransport_Unregister;      \
pfIoTHubTransport_Subscribe IoTHubTransport_Subscribe;       \
pfIoTHubTransport_Unsubscribe IoTHubTransport_Unsubscribe;   \
pfIoTHubTransport_DoWork IoTHubTransport_DoWork;             \
pfIoTHubTransport_GetSendStatus IoTHubTransport_GetSendStatus  /*there's an intentional missing ; on this line*/ \

	typedef struct TRANSPORT_PROVIDER_TAG
	{
		TRANSPORT_PROVIDER_FIELDS;
	}TRANSPORT_PROVIDER;

#ifdef __cplusplus
}
#endif

#endif /* IOTHUB_TRANSPORT_LL_H */
