// Copyright (c) Microsoft. All rights reserved.
// Licensed under the MIT license. See LICENSE file in the project root for full license information.

/** @file iothub_client_ll.h
*	@brief	 APIs that allow a user (usually a device) to communicate
*		     with an Azure IoTHub.
*
*	@details IoTHubClient_LL is a module that allows a user (usually a
*			 device) to communicate with an Azure IoTHub. It can send events
*			 and receive messages. At any given moment in time there can only
*			 be at most 1 message callback function.
*
*			 This API surface contains a set of APIs that allows the user to
*			 interact with the lower layer portion of the IoTHubClient. These APIs
*			 contain @c _LL_ in their name, but retain the same functionality like the
*			 @c IoTHubClient_... APIs, with one difference. If the @c _LL_ APIs are
*			 used then the user is responsible for scheduling when the actual work done
*			 by the IoTHubClient happens (when the data is sent/received on/from the wire).
*			 This is useful for constrained devices where spinning a separate thread is
*			 often not desired.
*/

#ifndef DONT_USE_UPLOADTOBLOB

#ifndef IOTHUB_CLIENT_LL_UPLOADTOBLOB_H
#define IOTHUB_CLIENT_LL_UPLOADTOBLOB_H

#include "iothub_client_ll.h"

#include "azure_c_shared_utility/umock_c_prod.h"
#ifdef __cplusplus
#include <cstddef>
extern "C"
{
#else
#include <stddef.h>
#endif

typedef struct IOTHUB_CLIENT_LL_UPLOADTOBLOB_HANDLE_DATA* IOTHUB_CLIENT_LL_UPLOADTOBLOB_HANDLE;

    MOCKABLE_FUNCTION(, IOTHUB_CLIENT_LL_UPLOADTOBLOB_HANDLE, IoTHubClient_LL_UploadToBlob_Create, const IOTHUB_CLIENT_CONFIG*, config);
    MOCKABLE_FUNCTION(, IOTHUB_CLIENT_RESULT, IoTHubClient_LL_UploadToBlob_Impl, IOTHUB_CLIENT_LL_UPLOADTOBLOB_HANDLE, handle, const char*, destinationFileName, const unsigned char*, source, size_t, size);
    MOCKABLE_FUNCTION(, IOTHUB_CLIENT_RESULT, IoTHubClient_LL_UploadToBlob_SetOption, IOTHUB_CLIENT_LL_UPLOADTOBLOB_HANDLE, handle, const char*, optionName, const void*, value);
    MOCKABLE_FUNCTION(, void, IoTHubClient_LL_UploadToBlob_Destroy, IOTHUB_CLIENT_LL_UPLOADTOBLOB_HANDLE, handle);
#ifdef __cplusplus
}
#endif

#endif /* IOTHUB_CLIENT_LL_UPLOADTOBLOB_H */

#else
#error "trying to #include iothub_client_ll_uploadtoblob.h in the presence of #define DONT_USE_UPLOADTOBLOB"
#endif /*DONT_USE_UPLOADTOBLOB*/
