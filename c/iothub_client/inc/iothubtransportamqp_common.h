// Copyright (c) Microsoft. All rights reserved.
// Licensed under the MIT license. See LICENSE file in the project root for full license information.

#ifndef IOTHUBTRANSPORTAMQP_COMMON_H
#define IOTHUBTRANSPORTAMQP_COMMON_H

#include "iothub_transport_ll.h"
#include "azure_c_shared_utility/xio.h"
#include "azure_uamqp_c/sasl_mechanism.h"
#include "azure_uamqp_c/cbs.h"

#ifdef __cplusplus
extern "C"
{
#endif
		
	typedef struct AMQP_TRANSPORT_CBS_CONNECTION_TAG
	{
		// How long a SAS token created by the transport is valid, in milliseconds.
		size_t sas_token_lifetime;
		// Maximum period of time for the transport to wait before refreshing the SAS token it created previously, in milliseconds.
		size_t sas_token_refresh_time;
		// Maximum time the transport waits for  uAMQP cbs_put_token() to complete before marking it a failure, in milliseconds.
		size_t cbs_request_timeout;

		// AMQP SASL I/O transport created on top of the TLS I/O layer.
		XIO_HANDLE sasl_io;
		// AMQP SASL I/O mechanism to be used.
		SASL_MECHANISM_HANDLE sasl_mechanism;
		// Connection instance with the Azure IoT CBS.
		CBS_HANDLE cbs_handle;
	} AMQP_TRANSPORT_CBS_CONNECTION;

#ifdef __cplusplus
}
#endif

#endif /*IOTHUBTRANSPORTAMQP_COMMON_H*/
