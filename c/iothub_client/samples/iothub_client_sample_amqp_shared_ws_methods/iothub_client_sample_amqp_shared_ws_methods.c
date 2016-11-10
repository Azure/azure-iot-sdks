// Copyright (c) Microsoft. All rights reserved.
// Licensed under the MIT license. See LICENSE file in the project root for full license information.

#include <stdio.h>
#include <stdlib.h>

#include "azure_c_shared_utility/platform.h"
#include "azure_c_shared_utility/threadapi.h"
#include "azure_c_shared_utility/crt_abstractions.h"
#include "iothub_client.h"
#include "iothub_message.h"
#include "iothubtransportamqp_websockets.h"
#include "../../../certs/certs.h"

static const char* hubName = "[IoT Hub Name]";
static const char* hubSuffix = "[IoT Hub Suffix]";
static const char* deviceId1 = "[device id 1]";
static const char* deviceId2 = "[device id 2]";
static const char* deviceKey1 = "[device key 1]";
static const char* deviceKey2 = "[device key 2]";

#define DOWORK_LOOP_NUM     42

static int DeviceMethodCallback(const char* method_name, const unsigned char* payload, size_t size, unsigned char** response, size_t* resp_size, void* userContextCallback)
{
	const char* device_id = (const char*)userContextCallback;

	(void)printf("\r\nDevice Method called for device %s\r\n", device_id);
	(void)printf("Device Method name:    %s\r\n", method_name);
	(void)printf("Device Method payload: %.*s\r\n", (int)size, (const char*)payload);

	int status = 200;
	char* RESPONSE_STRING = "{ \"Response\": \"This is the response from the device\" }";
	(void)printf("\r\nResponse status: %d\r\n", status);
	(void)printf("Response payload: %s\r\n\r\n", RESPONSE_STRING);

	*resp_size = strlen(RESPONSE_STRING);
	if ((*response = malloc(*resp_size)) == NULL)
	{
		status = -1;
	}
	else
	{
		memcpy(*response, RESPONSE_STRING, *resp_size);
	}
	return status;
}

void iothub_client_sample_amqp_shared_ws_methods_run(void)
{
	TRANSPORT_HANDLE transport_handle;
	IOTHUB_CLIENT_HANDLE iotHubClientHandle1;
	IOTHUB_CLIENT_HANDLE iotHubClientHandle2;

    (void)printf("Starting the IoTHub client sample C2D methods on AMQP over WebSockets with multiplexing ...\r\n");

    if (platform_init() != 0)
    {
		(void)printf("Failed to initialize the platform.\r\n");
    }
	else if ((transport_handle = IoTHubTransport_Create(AMQP_Protocol_over_WebSocketsTls, hubName, hubSuffix)) == NULL)
	{
		(void)printf("Failed to creating the protocol handle.\r\n");
	}
    else
    {
		IOTHUB_CLIENT_CONFIG client_config1;
		client_config1.deviceId = deviceId1;
		client_config1.deviceKey = deviceKey1;
		client_config1.deviceSasToken = NULL;
		client_config1.iotHubName = hubName;
		client_config1.iotHubSuffix = hubSuffix;
		client_config1.protocol = AMQP_Protocol_over_WebSocketsTls;
		client_config1.protocolGatewayHostName = NULL;

		IOTHUB_CLIENT_CONFIG client_config2;
		client_config2.deviceId = deviceId2;
		client_config2.deviceKey = deviceKey2;
		client_config2.deviceSasToken = NULL;
		client_config2.iotHubName = hubName;
		client_config2.iotHubSuffix = hubSuffix;
		client_config2.protocol = AMQP_Protocol_over_WebSocketsTls;
		client_config2.protocolGatewayHostName = NULL;

		
        if ((iotHubClientHandle1 = IoTHubClient_CreateWithTransport(transport_handle, &client_config1)) == NULL)
        {
            (void)printf("ERROR: iotHubClientHandle1 is NULL!\r\n");
        }
		else if ((iotHubClientHandle2 = IoTHubClient_CreateWithTransport(transport_handle, &client_config2)) == NULL)
		{
			(void)printf("ERROR: iotHubClientHandle2 is NULL!\r\n");
		}
        else
        {
            bool traceOn = true;
            IoTHubClient_SetOption(iotHubClientHandle1, "logtrace", &traceOn);

            // Add certificate information
            if (IoTHubClient_SetOption(iotHubClientHandle1, "TrustedCerts", certificates) != IOTHUB_CLIENT_OK)
            {
				(void)printf("failure to set option \"TrustedCerts\"\r\n");
            }

            /* Here subscribe for C2D methods */
			if (IoTHubClient_SetDeviceMethodCallback(iotHubClientHandle1, DeviceMethodCallback, (void*)deviceId1) != IOTHUB_CLIENT_OK)
			{
				(void)printf("ERROR: IoTHubClient_SetDeviceMethodCallback for the first device..........FAILED!\r\n");
			}
			else if (IoTHubClient_SetDeviceMethodCallback(iotHubClientHandle2, DeviceMethodCallback, (void*)deviceId2) != IOTHUB_CLIENT_OK)
			{
				(void)printf("ERROR: IoTHubClient_SetDeviceMethodCallback for the second device..........FAILED!\r\n");
			}
			else
			{
                (void)printf("IoTHubClient_SetMessageCallback...successful.\r\n");
				(void)printf("Waiting for C2D methods ...\r\n");

                do
                {
                    ThreadAPI_Sleep(1);
                } while (1);
            }

			IoTHubClient_Destroy(iotHubClientHandle1);
			IoTHubClient_Destroy(iotHubClientHandle2);
        }

		IoTHubTransport_Destroy(transport_handle);

        platform_deinit();
    }
}

int main(void)
{
    iothub_client_sample_amqp_shared_ws_methods_run();
    return 0;
}
