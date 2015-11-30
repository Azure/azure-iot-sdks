// Copyright (c) Microsoft. All rights reserved.
// Licensed under the MIT license. See LICENSE file in the project root for full license information.

/** @file version.h
*	@brief Functions for managing the client SDK version.
*/

#ifndef IOTHUB_VERSION_H
#define IOTHUB_VERSION_H

#define IOTHUB_SDK_VERSION "1.0.0-preview.7"

#ifdef __cplusplus
extern "C"
{
#endif

    /**
     * @brief   Returns a pointer to a null terminated string containing the
     *          current IoT Hub Client SDK version.
     *
     * @return  Pointer to a null terminated string containing the
     *          current IoT Hub Client SDK version.
     */
    extern const char* IoTHubClient_GetVersionString(void);

#ifdef __cplusplus
}
#endif

#endif // IOTHUB_VERSION_H
